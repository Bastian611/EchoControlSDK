#include "DeviceBase.h"
#include "handler/EchoControlHandler.h"
#include <cstdlib> // for strtoul

ECCS_BEGIN

DeviceBase::DeviceBase()
    : Thread(), m_slotID(0), m_devState(STATE_OFFLINE)
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

    return true;
}

bool DeviceBase::Start() {
    if (m_state == TS_RUNNING) return true;
    Thread::start();
    LOG_INFO("[Slot %d] Device Thread Started.", m_slotID);
    return true;
}

void DeviceBase::Stop() {
    if (m_state != TS_RUNNING && m_thread == nullptr) return;

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
    return m_devState == STATE_ONLINE || m_devState == STATE_WORKING;
}

DeviceID DeviceBase::GetDeviceID() const {
    return m_deviceID;
}

void DeviceBase::SetStatusCallback(StatusCallback cb) {
    m_statusCb = cb;
}

void DeviceBase::SetState(DevState newState, int errCode) {
    if (m_devState == newState) return;
    m_devState = newState;

    // 构造状态包
    rpc::DeviceStatus status;
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
            if (pe && pe->GetPacket()) {
                // 调用单例 Handler 进行分发
                EchoControlHandler::Instance().Dispatch(this, pe->GetPacket());
            }
        }

        OnCustomEvent(e);
    }
}

// 虚函数默认实现
void DeviceBase::OnRegisterProperties() {}
void DeviceBase::OnCustomEvent(Event_Ptr& e) {}

ECCS_END