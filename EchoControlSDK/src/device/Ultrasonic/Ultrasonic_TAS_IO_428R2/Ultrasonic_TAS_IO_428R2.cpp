#include "Ultrasonic_TAS_IO_428R2.h"
#include "../../../debug/Exceptions.h"
#include "../../../debug/Logger.h"

ECCS_BEGIN

Ultrasonic_TAS_IO_428R2::Ultrasonic_TAS_IO_428R2() {
}

Ultrasonic_TAS_IO_428R2::~Ultrasonic_TAS_IO_428R2() {
    if (m_socket) m_socket->close();
}

bool Ultrasonic_TAS_IO_428R2::Init(int slotID, const std::map<str, str>& config) {
    // 1. 基类初始化
    if (!DeviceBase::Init(slotID, config)) return false;

    // 2. 获取配置
    m_ip = GetPropValue<str>("IP");
    m_port = GetPropValue<int>("Port");

    // 默认端口 10123 (参考你的旧代码)
    if (m_port == 0) m_port = 10123;

    return true;
}

void Ultrasonic_TAS_IO_428R2::SetSwitch(u8 channel, bool isOpen) {
    // Channel 1 -> Address 0x0000
    // Channel 2 -> Address 0x0001
    // ...
    u16 addr = (channel > 0) ? (channel - 1) : 0;

    // UnitID 固定为 0x11 (参考旧代码)
    SendModbusCmd(0x11, addr, isOpen);
}

void Ultrasonic_TAS_IO_428R2::SendModbusCmd(u8 unitId, u16 addr, bool on) {
    if (!Connect()) return;

    // Modbus-TCP 报文结构 (12字节)
    u8 buf[12];

    // 1. Transaction ID (2 bytes) - 事务标识符，通常自增或0
    buf[0] = 0x00;
    buf[1] = 0x00;

    // 2. Protocol ID (2 bytes) - Modbus协议固定为0
    buf[2] = 0x00;
    buf[3] = 0x00;

    // 3. Length (2 bytes) - 后续字节数 = UnitID(1) + Func(1) + Data(4) = 6
    buf[4] = 0x00;
    buf[5] = 0x06;

    // 4. Unit ID (1 byte) - 从站地址
    buf[6] = unitId;

    // 5. Function Code (1 byte) - 05 Write Single Coil (写单个线圈)
    buf[7] = 0x05;

    // 6. Output Address (2 bytes) - 寄存器地址
    buf[8] = (addr >> 8) & 0xFF;
    buf[9] = addr & 0xFF;

    // 7. Output Value (2 bytes) - FF00=ON, 0000=OFF
    buf[10] = on ? 0xFF : 0x00;
    buf[11] = 0x00;

    try {
        m_socket->write(buf, 12);
        // LOG_DEBUG("[Slot %d] Ultrasonic Set: Ch %d -> %d", m_slotID, addr+1, on);
    }
    catch (std::exception& e) {
        LOG_ERROR("[Slot %d] Ultrasonic Send failed: %s", m_slotID, e.what());
        SetState(STATE_ERROR, 101);
        m_socket->close();
    }
}

bool Ultrasonic_TAS_IO_428R2::Connect() {
    if (IsOnline() && m_socket && m_socket->isOpen()) return true;

    try {
        SetState(STATE_CONNECTING);
        m_socket = std::make_shared<TcpSocket>(m_ip, m_port);
        m_socket->setConnTimeout(500); // 500ms 连接超时
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