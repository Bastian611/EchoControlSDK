#include "DeviceBase.h"
#include "handler/EchoControlHandler.h"
#include <cstdlib> // for strtoul

ECCS_BEGIN

DeviceBase::DeviceBase()
    : Thread(), m_slotID(0), m_devState(STATE_UNKNOWN)
{
}

DeviceBase::~DeviceBase() {
    Stop();
}

bool DeviceBase::Init(int slotID, const std::map<str, str>& config) {
    m_slotID = slotID;

    // 注册通用属性 (直接使用内置的 RegisterProp)
    RegisterProp<int>("Transform", 1, "Transform"); // 0:Serial, 1:Net
    RegisterProp<str>("IP", "127.0.0.1", "IP");
    RegisterProp<int>("Port", 8000, "Port");
    RegisterProp<int>("Baud", 115200, "Baudrate");
    RegisterProp<str>("Type", "Unnamed", "Device Type");
    RegisterProp<str>("Model", "Unnamed", "Device Model");
    RegisterProp<str>("ID", "0x00000000", "Device ID");
    RegisterProp<bool>("Enable", false, "Enable");

    // 调用子类钩子 (子类注册特有属性，如 PtzSpeed)
    OnRegisterProperties();

    // 导入配置 (只导入已注册的属性)
    for (const auto& pair : config) {
        if (m_propMeta.count(pair.first)) {
            m_propValues[pair.first] = pair.second;
        }
    }

    // 解析并校验 ID (从属性中读取)
    str idStr = GetPropValue<str>("ID");
    u32 fullID = std::strtoul(idStr.c_str(), nullptr, 0);
    m_deviceID = DeviceID(fullID);

    if (!m_deviceID.IsIndexValid()) {
        LOG_ERROR("[Slot %d] Init Failed: Invalid Index in ID %s", m_slotID, idStr.c_str());
        return false;
    }

    m_shuttingDown = false;
    SetState(STATE_INITIALIZED);

    return true;
}

bool DeviceBase::Start() {
    if (m_state == TS_RUNNING) return true;
    Thread::start();
    LOG_INFO("[Slot %d] Device Thread Started.", m_slotID);
    return true;
}

void DeviceBase::Stop() {
    if (m_state != TS_RUNNING && m_thread == nullptr) 
        return;

    m_shuttingDown = true;

    Thread::quit();
    Thread::join();
    SetState(STATE_OFFLINE);
    LOG_INFO("[Slot %d] Device Thread Stopped.", m_slotID);
}

// --- 控制入口 ---

void DeviceBase::ExecutePacket(std::shared_ptr<rpc::RpcPacket> pkt) {
    if (pkt) {
        // 使用 new 创建裸指针，交给 Thread 接管所有权
        PacketEvent* e = new PacketEvent(pkt);
        postEvent(e);
    }
}

// --- 属性查询 ---

str DeviceBase::GetProperty(const str& key) const {
    auto it = m_propValues.find(key);
    return (it != m_propValues.end()) ? it->second : "";
}

bool DeviceBase::IsOnline() const {
    return IsStateOnline(m_devState);
}

DeviceID DeviceBase::GetDeviceID() const {
    return m_deviceID;
}

void DeviceBase::SetStatusCallback(StatusCallback cb) {
    m_statusCb = cb;
}

void DeviceBase::SetState(DevState newState, int errCode) 
{
    // 设备正在关闭，只允许进入 OFFLINE
    if (m_shuttingDown && newState != STATE_OFFLINE)
    {
        LOG_WARNING("Reject state change during shutdown: {%s} -> {%s}",
            DevStateToStr(m_devState),
            DevStateToStr(newState));
        return;
    }
    // 构造状态包
    rpc::DeviceStatus status;
    if (!IsValidStateTransition(m_devState, newState))
    {
        LOG_ERROR("Invalid state transition: {%s} -> {%s}",
            DevStateToStr(m_devState),
            DevStateToStr(newState));

        // 发送一次“状态未变，但有错误”的通知
        status.deviceID = m_deviceID.Value();
        status.slotID = (u8)m_slotID;
        status.state = (u8)newState; // enum -> u8
        status.errorCode = (u32)errCode;
        status.temperature = 0.0f; // 可扩展：从属性获取

        // 推送 OW 包
        auto pkt = std::make_shared<rpc::OwDeviceStatus>(status);
        if (m_statusCb) m_statusCb(pkt);

        return ;
    }

    if (m_devState == newState) return;

    OnStateExit(m_devState);
    m_devState = newState;
    OnStateEnter(m_devState);

    status.deviceID = m_deviceID.Value();
    status.slotID = (u8)m_slotID;
    status.state = (u8)newState; // enum -> u8
    status.errorCode = (u32)errCode;
    status.temperature = 0.0f; // 可扩展：从属性获取

    // 推送 OW 包
    auto pkt = std::make_shared<rpc::OwDeviceStatus>(status);
    if (m_statusCb) m_statusCb(pkt);

    // LOG_DEBUG("[Slot %d] State Changed: %s", m_slotID, DevStateToStr(newState));
}

void DeviceBase::run() {
    while (m_state == TS_RUNNING) {
        Event_Ptr e = m_eq.pop();
        if (!e) continue;

        if (e->eId() == EventTypes::Quit) break;

        // Packet 事件
        if (e->eId() == DeviceEventID::PacketArrival) {
            auto pe = std::dynamic_pointer_cast<PacketEvent>(e);
            if (!pe && !pe->GetPacket())
                continue;

            // 状态拦截
            if (!IsStateOnline(m_devState)) {
                LOG_WARNING(
                    "[Slot %d] Packet rejected in state: %s",
                    m_slotID,
                    DevStateToStr(m_devState)
                );
                continue;
            }

            // 调用单例 Handler 进行分发
            EchoControlHandler::Instance().Dispatch(this, pe->GetPacket());
        }

        OnCustomEvent(e);
    }
}

// 虚函数默认实现
void DeviceBase::OnRegisterProperties() {}
void DeviceBase::OnCustomEvent(Event_Ptr& e) {}

void DeviceBase::OnRawDataReceived(const u8* data, u32 len) {}

void DeviceBase::OnStateEnter(DevState state)
{
    switch (state)
    {
    case STATE_ONLINE:
        StartReader();
        break;

    case STATE_OFFLINE:
    case STATE_ERROR:
        StopReader();
        break;

    default:
        break;
    }
}

void DeviceBase::OnStateExit(DevState)
{
    // 目前不需要做任何事
}

void DeviceBase::StartReader() {
    if (m_keepReading) return;
    m_keepReading = true;
    m_readThread = new std::thread(&DeviceBase::ReadLoop, this);
}

void DeviceBase::StopReader() {
    m_keepReading = false;
    if (m_readThread) {
        if (m_readThread->joinable()) m_readThread->join();
        delete m_readThread;
        m_readThread = nullptr;
    }
}

bool DeviceBase::IsValidStateTransition(DevState from, DevState to) const
{
    if (from == to)
        return true;

    switch (from)
    {
    case STATE_UNKNOWN:
        return to == STATE_INITIALIZED;

    case STATE_INITIALIZED:
        return to == STATE_OFFLINE
            || to == STATE_CONNECTING;

    case STATE_OFFLINE:
        return to == STATE_CONNECTING
            || to == STATE_ERROR;

    case STATE_CONNECTING:
        return to == STATE_ONLINE
            || to == STATE_OFFLINE
            || to == STATE_ERROR;

    case STATE_ONLINE:
        return to == STATE_WORKING
            || to == STATE_OFFLINE
            || to == STATE_ERROR;

    case STATE_WORKING:
        return to == STATE_ONLINE
            || to == STATE_OFFLINE
            || to == STATE_ERROR;

    case STATE_ERROR:
        return to == STATE_OFFLINE
            || to == STATE_INITIALIZED;

    default:
        return false;
    }
}

bool DeviceBase::IsStateOnline(DevState state) const
{
    switch (state)
    {
    case STATE_ONLINE:
    case STATE_WORKING:
        return true;

    default:
        return false;
    }
}

void DeviceBase::ReadLoop() {
    // 缓冲区 1KB
    std::vector<u8> buf(1024);

    while (m_keepReading) {
        // 如果设备掉线，稍微休眠，避免死循环空转
        if (!IsOnline()) {
            msleep(200);
            continue;
        }

        // 调用子类实现的 ReadRaw (阻塞或带超时的读取)
        int len = ReadRaw(buf.data(), buf.size());

        if (len > 0) {
            // 收到数据，交给子类解析
            OnRawDataReceived(buf.data(), len);
        }
        else {
            // 读取超时或错误，稍微休眠
            // 注意：如果是严重错误（如断开），子类的 ReadRaw 内部应该处理连接状态
            msleep(10);
        }
    }
}

ECCS_END