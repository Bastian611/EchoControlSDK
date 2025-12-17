#include "PTZ_YZ_BY010W.h"
#include "../../../debug/Exceptions.h"

ECCS_BEGIN

PTZ_YZ_BY010W::PTZ_YZ_BY010W() : m_addr(1) {
}

PTZ_YZ_BY010W::~PTZ_YZ_BY010W() {
    if (m_socket) m_socket->close();
}

bool PTZ_YZ_BY010W::Init(int slotID, const std::map<str, str>& config) {
    if (!Device::Init(slotID, config)) return false;

    m_deviceID = DeviceID(did::DEVICE_PTZ, did::PTZ_YZ_BY010W);
    m_ip = m_props.GetString("IP");
    m_port = m_props.Get<int>("Port");
    m_addr = (u8)m_props.Get<int>("ID");
    if (m_addr == 0) m_addr = 1;

    return true;
}

void PTZ_YZ_BY010W::PtzMove(u8 action, u8 speed) {
    u8 cmd2 = 0x00;
    u8 d1 = 0x00; // Pan Speed
    u8 d2 = 0x00; // Tilt Speed

    // action: 1=Up, 2=Down, 3=Left, 4=Right
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
    // 1=Set, 2=Goto
    if (action == 2) SendPelcoD(0x00, 0x07, 0x00, index);
    else if (action == 1) SendPelcoD(0x00, 0x03, 0x00, index);
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