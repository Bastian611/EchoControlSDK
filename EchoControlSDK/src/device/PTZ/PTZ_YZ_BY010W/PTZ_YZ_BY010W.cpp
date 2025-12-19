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

    // 3获取缓存配置
    m_ip = GetPropValue<str>("IP");   // 使用基类的辅助函数更方便
    m_port = GetPropValue<int>("Port");

    // 获取物理地址
    // 既然 ID 是 0x30010001，那么 Index(01) 通常就是 RS485/Pelco-D 的物理地址
    m_addr = m_deviceID.GetIndex();
    if (m_addr == 0) m_addr = 1; // 防御性保底

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
    buf[1] = m_addr; // 使用从 ID 中提取的地址
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
        m_socket->open();
        SetState(STATE_ONLINE);
        return true;
    }
    catch (std::exception&) {
        SetState(STATE_OFFLINE);
        return false;
    }
}

ECCS_END