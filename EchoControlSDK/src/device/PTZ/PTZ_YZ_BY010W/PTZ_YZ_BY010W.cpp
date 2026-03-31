#include "PTZ_YZ_BY010W.h"
#include "debug/Exceptions.h"

ECCS_BEGIN

PTZ_YZ_BY010W::PTZ_YZ_BY010W() : m_addr(1) {
}

PTZ_YZ_BY010W::~PTZ_YZ_BY010W() {
    Stop();
}

ECCS_Error PTZ_YZ_BY010W::Init(int slotID, const std::map<str, str>& config) 
{
    if (DeviceBase::Init(slotID, config) != ECCS_SUCCESS) 
        return ECCS_ERR_NOT_INIT;
    m_ip = GetPropValue<str>("IP");
    m_port = GetPropValue<int>("Port");
    m_addr = (u8)GetPropValue<int>("Address");
    if (m_addr == 0) m_addr = 1;
    return ECCS_SUCCESS;
}

ECCS_Error PTZ_YZ_BY010W::Start() 
{
    DeviceBase::Start();
    if (!Connect()) 
        return ECCS_ERR_DEV_OFFLINE;

    // 开启角度实时回传
    SendPelcoD(0x00, 0x09, 0x00, 0x05);
    return ECCS_SUCCESS;
}

ECCS_Error PTZ_YZ_BY010W::Stop() 
{
    if (IsOnline()) {
        // 关闭回传
        SendPelcoD(0x00, 0x0B, 0x00, 0x05);
    }
    StopReader();
    if (m_socket) {
        m_socket->close();
    }
    return DeviceBase::Stop();
}

bool PTZ_YZ_BY010W::Reconnect() 
{
    if (Connect()) {
        SendPelcoD(0x00, 0x09, 0x00, 0x05); // 重连后恢复回传
        return true;
    }
    return false;
}

ECCS_Error PTZ_YZ_BY010W::PtzMove(u8 action, u8 speed) 
{
    u8 c2 = 0; u8 d1 = 0; u8 d2 = 0;
    switch (action) {
    case 1: c2 = 0x08; d2 = speed; break; // Up
    case 2: c2 = 0x10; d2 = speed; break; // Down
    case 3: c2 = 0x04; d1 = speed; break; // Left
    case 4: c2 = 0x02; d1 = speed; break; // Right
    }
    ECCS_Error err = SendPelcoD(0x00, c2, d1, d2);
    if (err == ECCS_SUCCESS) {
        SetState(STATE_WORKING);
    }
    return err;
}

ECCS_Error PTZ_YZ_BY010W::PtzStop() 
{
    ECCS_Error err = SendPelcoD(0x00, 0x00, 0x00, 0x00);
    SetState(STATE_ONLINE);
    return err;
}

ECCS_Error PTZ_YZ_BY010W::PtzPreset(u8 action, u8 index)
{
    return ECCS_ERR_NOT_SUPPORTED;
}

ECCS_Error PTZ_YZ_BY010W::PtzReset() 
{
    
    return SendPelcoD(0x00, 0x07, 0x00, 0x63);
}

ECCS_Error PTZ_YZ_BY010W::PtzQueryPosition() 
{
    // 主动查询水平和垂直
    SendPelcoD(0x00, 0x51, 0x00, 0x00);
    return SendPelcoD(0x00, 0x53, 0x00, 0x00);
}

ECCS_Error PTZ_YZ_BY010W::PtzZoom(bool isZoomIn)
{
    return ECCS_ERR_NOT_SUPPORTED;
}

ECCS_Error PTZ_YZ_BY010W::PtzSetAbsolutePos(float pan, float tilt) 
{
    // 换算公式：角度 * 100
    u16 p = (u16)(pan * 100);
    u16 t = (u16)(tilt * 100);
    // 4B 水平定位，4D 垂直定位
    ECCS_Error ret = SendPelcoD(0x00, 0x4B, (p >> 8), (p & 0xFF));
    if (ret != ECCS_SUCCESS) {
        return ret;
    }
    else {
        ret = SendPelcoD(0x00, 0x4D, (t >> 8), (t & 0xFF));
    }
    return ret;
}

ECCS_Error PTZ_YZ_BY010W::PtzSetScanRange(float startAngle, float endAngle) 
{
    u16 val = (u16)(startAngle * 100);
    u8 dataH = (u8)(val >> 8);
    u8 dataL = (u8)(val & 0xFF);

    LOG_INFO("[Slot %d] PTZ: Set Scan START to %.2f deg", m_slotID, startAngle);
    ECCS_Error ret = SendPelcoD(0x01, 0x11, dataH, dataL);
    if (ret != ECCS_SUCCESS) {
        return ret;
    }
    else {
        msleep(1000);
        val = (u16)(endAngle * 100);
        dataH = (u8)(val >> 8);
        dataL = (u8)(val & 0xFF);

        LOG_INFO("[Slot %d] PTZ: Set Scan END to %.2f deg", m_slotID, endAngle);
        ret = SendPelcoD(0x01, 0x13, dataH, dataL);
    }
    return ret;
}

ECCS_Error PTZ_YZ_BY010W::PtzStartScan() 
{
    ECCS_Error ret = SendPelcoD(0x00, 0x1B, 0x00, 0x00);
    if (ret == ECCS_SUCCESS) 
    {
        SetState(STATE_WORKING);
    }
    return ret;
}

ECCS_Error PTZ_YZ_BY010W::PtzStopScan()
{
    ECCS_Error ret = SendPelcoD(0x00, 0x1D, 0x00, 0x00);
    if (ret == ECCS_SUCCESS)
    {
        SetState(STATE_ONLINE);
    }
    return ret;
}

ECCS_Error PTZ_YZ_BY010W::SendPelcoD(u8 cmd1, u8 cmd2, u8 data1, u8 data2) 
{
    if (!m_socket || !m_socket->isOpen()) 
        return ECCS_ERR_DEV_OFFLINE;
    u8 buf[7] = { 0xFF, m_addr, cmd1, cmd2, data1, data2, 0 };
    u32 sum = buf[1] + buf[2] + buf[3] + buf[4] + buf[5];
    buf[6] = (u8)(sum & 0xFF);
    try {
        m_socket->write(buf, 7);
        return ECCS_SUCCESS;
    }
    catch (...) {
        SetState(STATE_ERROR, ECCS_ERR_DEV_SEND_FAILED);
        return ECCS_ERR_DEV_SEND_FAILED;
    }
}

bool PTZ_YZ_BY010W::Connect() 
{
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

int PTZ_YZ_BY010W::ReadRaw(u8* buf, u32 maxLen) 
{
    if (!m_socket || !m_socket->isOpen()) return -1;
    try {
        return m_socket->read(buf, maxLen);
    }
    catch (...) {
        return -1;
    }
}

void PTZ_YZ_BY010W::OnRawDataReceived(const u8* data, u32 len) 
{
    m_recvBuf.insert(m_recvBuf.end(), data, data + len);
    while (m_recvBuf.size() >= 7) {
        if (m_recvBuf[0] != 0xFF) { m_recvBuf.erase(m_recvBuf.begin()); continue; }
        u32 sum = m_recvBuf[1] + m_recvBuf[2] + m_recvBuf[3] + m_recvBuf[4] + m_recvBuf[5];
        if ((u8)(sum & 0xFF) == m_recvBuf[6]) {
            ParseResponse(m_recvBuf.data());
            m_recvBuf.erase(m_recvBuf.begin(), m_recvBuf.begin() + 7);
        }
        else {
            m_recvBuf.erase(m_recvBuf.begin());
        }
    }
}

void PTZ_YZ_BY010W::OnStateEnter(DevState state)
{
    if (state == STATE_ONLINE) {
        // 从配置读取业务零位
        float zeroPan = GetPropValue<float>("Zero_Pan");
        float zeroTilt = GetPropValue<float>("Zero_Tilt");

        LOG_INFO("[Slot %d] PTZ: Moving to Configured Zero (%.2f, %.2f) at MAX speed",
            m_slotID, zeroPan, zeroTilt);
        if (m_lastPan != 0 || m_lastTilt != 0)
            SetState(STATE_WORKING);
    }
}

void PTZ_YZ_BY010W::ParseResponse(const u8* data) {
    u8 type = data[3];
    u16 val = (data[4] << 8) | data[5];
    float angle = val / 100.0f;

    if (type == 0x59) {
        m_lastPan = angle;       // 水平回传
    }
    else if (type == 0x5B) {                   // 垂直回传
        if (angle > 180.0f) 
        {
            angle -= 360.0f;
        }   
        m_lastTilt = angle;
    }
    else {
        return;
    }

    PtzPosition pos = { m_lastPan.load(), m_lastTilt.load(), 0 };
    auto pkt = std::make_shared<rpc::OwPtzPosition>(pos);
    if (m_statusCb) m_statusCb(pkt);
}

ECCS_END