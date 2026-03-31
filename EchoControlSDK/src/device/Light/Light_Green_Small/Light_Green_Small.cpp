#include "Light_Green_Small.h"

ECCS_BEGIN

Light_Green_Small::Light_Green_Small() 
{
    m_bIsLighting = false;
}
Light_Green_Small::~Light_Green_Small() 
{ 
    Stop(); 
}

ECCS_Error Light_Green_Small::Init(int slotID, const std::map<str, str>& config) 
{
    if (DeviceBase::Init(slotID, config) != ECCS_SUCCESS) 
        return ECCS_ERR_NOT_INIT;
    m_ip = GetPropValue<str>("IP");
    m_port = GetPropValue<int>("Port");
    if (m_port == 0) 
        m_port = 23;
    return ECCS_SUCCESS;
}

ECCS_Error Light_Green_Small::Start() 
{
    DeviceBase::Start();
    bool ret = Connect();
    if (ret) {
        SetStrobe(false);
        msleep(200);
        SetLightSwitch(false);
    }
    return ret ? ECCS_SUCCESS : ECCS_ERR_DEV_OFFLINE;
}

ECCS_Error Light_Green_Small::Stop() 
{
    SetLightSwitch(false);
    if (m_socket) 
        m_socket->close();
    return DeviceBase::Stop();
}

bool Light_Green_Small::Reconnect() 
{ 
    return Connect(); 
}

ECCS_Error Light_Green_Small::SetLightSwitch(bool isOpen) 
{
    m_bIsLighting = isOpen;
    return SendFixedPacket(isOpen ? 0x21 : 0x22); // 21:开, 22:关
}

ECCS_Error Light_Green_Small::SetStrobe(bool isOpen)
{
    if (!m_bIsLighting) {
        return ECCS_ERR_DEV_LIGHT_CLOSE;
    }
    return SendFixedPacket(isOpen ? 0x25 : 0x26); // 25:开频闪, 26:关频闪
}

ECCS_Error Light_Green_Small::SendFixedPacket(u8 cmd) 
{
    if (!m_socket || !m_socket->isOpen()) 
        return ECCS_ERR_DEV_OFFLINE;

    // 格式：7F 70 CMD 00 00 AA 55 A5 5A
    u8 buf[9] = { 0x7F, 0x70, cmd, 0x00, 0x00, 0xAA, 0x55, 0xA5, 0x5A };

    try {
        m_socket->write(buf, 9);
        // 只有关机指令 0x22 认为是 ONLINE，其他状态（开机、频闪）均视为 WORKING
        SetState(cmd == 0x22 ? STATE_ONLINE : STATE_WORKING);
        return ECCS_SUCCESS;
    }
    catch (...) {
        SetState(STATE_ERROR, ECCS_ERR_DEV_SEND_FAILED);
        return ECCS_ERR_DEV_SEND_FAILED;
    }
}

bool Light_Green_Small::Connect() 
{
    if (IsOnline() && m_socket && m_socket->isOpen()) 
        return true;
    try {
        SetState(STATE_CONNECTING);
        m_socket = std::make_shared<TcpSocket>(m_ip, m_port);
        m_socket->setConnTimeout(500);
        m_socket->open();
        SetState(STATE_ONLINE);
        return true;
    }
    catch (...) {
        SetState(STATE_OFFLINE);
        return false;
    }
}

int Light_Green_Small::ReadRaw(u8* buf, u32 maxLen) 
{
    if (!m_socket || !m_socket->isOpen()) 
        return -1;
    return m_socket->read(buf, maxLen);
}

ECCS_END