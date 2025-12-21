#include "PTZ_YZ_BY010W.h"
#include "debug/Exceptions.h"

ECCS_BEGIN

PTZ_YZ_BY010W::PTZ_YZ_BY010W() : m_addr(1) {
}

PTZ_YZ_BY010W::~PTZ_YZ_BY010W() {
    if (m_socket) m_socket->close();
}

bool PTZ_YZ_BY010W::Init(int slotID, const std::map<str, str>& config) {
    // 调用基类初始化 (解析 ID, IP, Port, 校验 Index 等)
    if (!DeviceBase::Init(slotID, config)) return false;

    // 获取缓存配置
    m_ip = GetPropValue<str>("IP");   // 使用基类的辅助函数更方便
    m_port = GetPropValue<int>("Port");

    // 获取物理地址
    // 既然 ID 是 0x30010001，那么 Index(01) 通常就是 RS485/Pelco-D 的物理地址
    //m_addr = m_deviceID.GetIndex();

    return true;
}

void PTZ_YZ_BY010W::PtzMove(u8 action, u8 speed) {
    u8 cmd2 = 0x00;
    u8 d1 = 0x00; // Pan Speed
    u8 d2 = 0x00; // Tilt Speed

    // action 定义在 Packet_Def.h (1=Up, 2=Down, 3=Left, 4=Right)
    switch (action) {
    case 1: cmd2 = 0x08; d2 = speed; break; // Up
    case 2: cmd2 = 0x10; d2 = speed; break; // Down
    case 3: cmd2 = 0x04; d1 = speed; break; // Left
    case 4: cmd2 = 0x02; d1 = speed; break; // Right
    }
    SendPelcoD(0x00, cmd2, d1, d2);
}

void PTZ_YZ_BY010W::PtzStop() {
    SendPelcoD(0x00, 0x00, 0x00, 0x00);
}

void PTZ_YZ_BY010W::PtzPreset(u8 action, u8 index) {
    // 1=Set, 2=Goto (假设协议定义)
    if (action == 2) SendPelcoD(0x00, 0x07, 0x00, index);      // Call
    else if (action == 1) SendPelcoD(0x00, 0x03, 0x00, index); // Set
}

void PTZ_YZ_BY010W::SendPelcoD(u8 cmd1, u8 cmd2, u8 d1, u8 d2) {
    if (!Connect()) return;

    u8 buf[7];
    buf[0] = 0xFF;
    buf[1] = m_addr; 
    buf[2] = cmd1;
    buf[3] = cmd2;
    buf[4] = d1;
    buf[5] = d2;

    u32 sum = buf[1] + buf[2] + buf[3] + buf[4] + buf[5];
    buf[6] = (u8)(sum % 256);

    try {
        m_socket->write(buf, 7);
    }
    catch (std::exception& e) {
        LOG_ERROR("[Slot %d] PTZ Send failed: %s", m_slotID, e.what());
        SetState(STATE_ERROR);
        m_socket->close();
    }
}

bool PTZ_YZ_BY010W::Connect() {
    if (IsOnline() && m_socket && m_socket->isOpen()) return true;
    try {
        SetState(STATE_CONNECTING);
        m_socket = std::make_shared<TcpSocket>(m_ip, m_port);
        m_socket->setConnTimeout(500);
        m_socket->setRecvTimeout(200);
        m_socket->open();
        SetState(STATE_ONLINE);
        return true;
    }
    catch (std::exception&) {
        SetState(STATE_OFFLINE);
        return false;
    }
}

bool PTZ_YZ_BY010W::Start() {
    if (!DeviceBase::Start()) return false;

    // 启动读取线程 (基类方法)
    StartReader();

    // 打开角度实时回传
    // 协议: FF Addr 00 09 00 05 CS
    LOG_INFO("[Slot %d] PTZ: Enable Real-time Angle Report", m_slotID);
    SendPelcoD(0x00, 0x09, 0x00, 0x05);

    return true;
}

void PTZ_YZ_BY010W::Stop() {
    // 关闭角度实时回传
    // 协议: FF Addr 00 0B 00 05 CS
    if (IsOnline()) {
        SendPelcoD(0x00, 0x0B, 0x00, 0x05);
    }

    // 停止读取
    StopReader();

    DeviceBase::Stop();
}

int PTZ_YZ_BY010W::ReadRaw(u8* buf, u32 maxLen) {
    if (!m_socket || !m_socket->isOpen()) return -1;
    try {
        return m_socket->read(buf, maxLen);
    }
    catch (...) {
        return -1;
    }
}

void PTZ_YZ_BY010W::OnRawDataReceived(const u8* data, u32 len) {
    // Pelco-D 回包固定 7 字节
    // 简单处理粘包：我们只处理缓冲区里找到的第一个完整包
    // 实际工程中应该用 RingBuffer 处理分包，这里简化演示

    if (len < 7) return;

    for (u32 i = 0; i <= len - 7; ++i) {
        if (data[i] == 0xFF) { // 帧头
            // 校验 Checksum
            u8 addr = data[i + 1];
            if (addr != m_addr) continue; // 不是我的地址

            u32 sum = data[i + 1] + data[i + 2] + data[i + 3] + data[i + 4] + data[i + 5];
            if ((u8)(sum % 256) == data[i + 6]) {
                // 校验通过，解析
                ParsePelcoResponse(data + i, 7);
                i += 6; 
            }
        }
    }
}

void PTZ_YZ_BY010W::ParsePelcoResponse(const u8* data, u32 len) 
{
    u8 cmd2 = data[3];
    u16 val = (data[4] << 8) | data[5];

    if (cmd2 == 0x59) 
    { 
        // 0-35999 (0.01度)
        float panAngle = val / 100.0f;

        // 构造 OW 包推送到上层
        rpc::PtzPosition pos;
        pos.pan = panAngle;
        pos.tilt = 0;
        pos.zoom = 0;

        // 推送 Packet
        auto pkt = std::make_shared<rpc::OwPtzPosition>(pos);
        // 调用基类回调
        if (m_statusCb) m_statusCb(pkt);

        // LOG_DEBUG("[PTZ] Pan Angle: %.2f", panAngle);
    } else if (cmd2 == 0x5B) 
    {
        float tiltAngle = val / 100.0f;
        rpc::PtzPosition pos;
        pos.pan = 0;
        pos.tilt = tiltAngle;
        pos.zoom = 0;

        auto pkt = std::make_shared<rpc::OwPtzPosition>(pos);
        if (m_statusCb) m_statusCb(pkt);

        // LOG_DEBUG("[PTZ] Tilt Angle: %.2f", tiltAngle);
    }
}

ECCS_END