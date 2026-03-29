#include "Light_HL_525_4W.h"
#include "../../../debug/Exceptions.h"

ECCS_BEGIN

Light_HL_525_4W::Light_HL_525_4W() {
}

Light_HL_525_4W::~Light_HL_525_4W() {
    if (m_socket) m_socket->close();
}

ECCS_Error Light_HL_525_4W::Init(int slotID, const std::map<str, str>& config) {
    // 基类初始化
    if (DeviceBase::Init(slotID, config) != ECCS_SUCCESS) 
        return ECCS_ERR_NOT_INIT;

    // 获取配置
    m_ip = GetPropValue<str>("IP");
    m_port = GetPropValue<int>("Port");

    if (!Connect()) {
        LOG_WARNING("[Slot %d] Start: Connect failed, will retry in run loop.", m_slotID);
        // 注意：这里返回 true，允许线程启动，以便在线程里重连
    }

    return ECCS_SUCCESS;
}

ECCS_Error Light_HL_525_4W::SetLightSwitch(bool isOpen) {
    // 0x1F:开, 0x2F:关
    return SendHexCmd(isOpen ? 0x1F : 0x2F, 0x00, 0x00);
}

ECCS_Error Light_HL_525_4W::SetBrightness(u8 level) {
    // 0x9F: 调节电流
    u16 current = (u16)level * 255 / 100;
    return SendHexCmd(0x9F, (current >> 8) & 0xFF, current & 0xFF);
}

ECCS_Error Light_HL_525_4W::SetStrobe(bool isOpen) {
    // 0x3F: 开闪烁, 0x4F: 关闪烁
    return SendHexCmd(isOpen ? 0x3F : 0x4F, 0x00, 0x00);
}

ECCS_Error Light_HL_525_4W::SendHexCmd(u8 cmd, u8 vh, u8 vl) {
    if (!Connect()) return ECCS_ERR_DEV_OFFLINE;

    u8 buf[7];
    buf[0] = 0xFF; // Header
    buf[1] = 0x01; // HL-525 固定地址 0x01 (不同于云台的动态地址)
    buf[2] = 0x00;
    buf[3] = cmd;
    buf[4] = vh;
    buf[5] = vl;

    // Checksum
    u32 sum = 0;
    for (int i = 1; i <= 5; i++) sum += buf[i];
    buf[6] = (u8)(sum & 0xFF);

    try {
        m_socket->write(buf, 7);
        std::string hexStr;
        char tmp[4];
        for (int i = 0; i < 7; ++i) {
            snprintf(tmp, sizeof(tmp), "%02X ", buf[i]);
            hexStr += tmp;
        }
        LOG_DEBUG("[NetSpeaker] Hex: %s", hexStr.c_str());
        return ECCS_SUCCESS;
    }
    catch (std::exception& e) {
        LOG_ERROR("[Slot %d] Send failed: %s", m_slotID, e.what());
        SetState(STATE_ERROR, 101);
        m_socket->close();
        return ECCS_ERR_DEV_SEND_FAILED;
    }
}

bool Light_HL_525_4W::Connect() {
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