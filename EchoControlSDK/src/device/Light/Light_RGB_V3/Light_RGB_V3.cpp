#include "Light_RGB_V3.h"

ECCS_BEGIN

Light_RGB_V3::Light_RGB_V3()
    : m_curMode(RGB_V3_LightMode::Off), m_curPower(1),
    m_curFocusType(RGB_V3_FocusType::Manual), m_curFocusValue(0), m_curFlashHz(0)
{}

Light_RGB_V3::~Light_RGB_V3() { Stop(); }

ECCS_Error Light_RGB_V3::Init(int slotID, const std::map<str, str>& config) {
    if (DeviceBase::Init(slotID, config) != ECCS_SUCCESS) return ECCS_ERR_NOT_INIT;
    m_ip = GetPropValue<str>("IP");
    m_port = GetPropValue<int>("Port");
    if (m_port == 0) m_port = 23;
    return ECCS_SUCCESS;
}

ECCS_Error Light_RGB_V3::Start() {
    DeviceBase::Start();
    return Connect() ? ECCS_SUCCESS : ECCS_ERR_DEV_OFFLINE;
}

ECCS_Error Light_RGB_V3::Stop() {
    SetLightSwitch(false);
    StopReader();
    if (m_socket) m_socket->close();
    return DeviceBase::Stop();
}

bool Light_RGB_V3::Reconnect() { return Connect(); }

ECCS_Error Light_RGB_V3::SetLightSwitch(bool isOpen) {
    m_curMode = isOpen ? RGB_V3_LightMode::Lighting : RGB_V3_LightMode::Off;
    return SendCommand();
}

ECCS_Error Light_RGB_V3::SetWorkMode(RGB_V3_LightMode mode) {
    m_curMode = mode;
    return SendCommand();
}

ECCS_Error Light_RGB_V3::SetLightLevel(u8 power) {
    if (power < 1) 
        power = 1; 
    if (power > 10) 
        power = 10;
    m_curPower = power;
    return SendCommand();
}

ECCS_Error Light_RGB_V3::SetFlashFreq(u8 hz) {
    if (hz > 20) 
        hz = 20;
    m_curFlashHz = hz;
    return SendCommand();
}

ECCS_Error Light_RGB_V3::SetFocus(RGB_V3_FocusType type, u16 value) {
    m_curFocusType = type;
    m_curFocusValue = value;
    return SendCommand();
}

ECCS_Error Light_RGB_V3::SendCommand() {
    if (!m_socket || !m_socket->isOpen()) 
        return ECCS_ERR_DEV_OFFLINE;

    u8 buf[9];
    buf[0] = 0x7F; 
    buf[1] = 0x70;
    buf[2] = (u8)m_curMode;
    buf[3] = m_curPower;
    buf[4] = (u8)m_curFocusType;
    buf[5] = (u8)(m_curFocusValue >> 8);
    buf[6] = (u8)(m_curFocusValue & 0xFF);
    buf[7] = m_curFlashHz;

    // 校验和：字节 3-8 (索引 2-7) 之和
    u32 sum = 0;
    for (int i = 2; i <= 7; ++i) sum += buf[i];
    buf[8] = (u8)(sum & 0xFF);

    try {
        m_socket->write(buf, 9);
        SetState(m_curMode == RGB_V3_LightMode::Off ? STATE_ONLINE : STATE_WORKING);
        return ECCS_SUCCESS;
    }
    catch (...) {
        SetState(STATE_ERROR, ECCS_ERR_DEV_SEND_FAILED);
        return ECCS_ERR_DEV_SEND_FAILED;
    }
}

void Light_RGB_V3::OnRawDataReceived(const u8* data, u32 len) {
    m_recvBuf.insert(m_recvBuf.end(), data, data + len);
    while (m_recvBuf.size() >= 18) {
        if (m_recvBuf[0] != 0x7F || m_recvBuf[1] != 0x70) {
            m_recvBuf.erase(m_recvBuf.begin());
            continue;
        }
        ParseFeedback(m_recvBuf.data());
        m_recvBuf.erase(m_recvBuf.begin(), m_recvBuf.begin() + 18);
    }
}

void Light_RGB_V3::ParseFeedback(const u8* d) {
    // 字节 14-15 (索引 13-14): 角度 (10倍)
    u16 angleRaw = (d[13] << 8) | d[14];
    m_actualAngle = angleRaw / 10.0f;

    // 字节 16 (索引 15): 温度
    m_devTemp = (float)d[15];

    // 字节 18 (索引 17): 错误码
    u8 err = d[17];
    if (err != 0) SetState(STATE_ERROR, err);

    // 推送状态包（可选）
    rpc::DeviceStatus ds;
    ds.deviceID = m_deviceID.Value();
    ds.state = (u8)GetState();
    ds.temperature = m_devTemp.load();
    auto pkt = std::make_shared<rpc::OwDeviceStatus>(ds);
    if (m_statusCb) m_statusCb(pkt);
}

bool Light_RGB_V3::Connect() {
    if (IsOnline() && m_socket && m_socket->isOpen()) return true;
    try {
        SetState(STATE_CONNECTING);
        m_socket = std::make_shared<TcpSocket>(m_ip, m_port);
        m_socket->setConnTimeout(500);
        m_socket->open();
        SetState(STATE_ONLINE);
        StartReader();
        return true;
    }
    catch (...) {
        SetState(STATE_OFFLINE);
        return false;
    }
}

int Light_RGB_V3::ReadRaw(u8* buf, u32 maxLen) {
    if (!m_socket || !m_socket->isOpen()) return -1;
    return m_socket->read(buf, maxLen);
}

ECCS_END