#pragma once
#include "../DeviceBase.h"

ECCS_BEGIN

class IPTZ_Device : public DeviceBase
{
public:
    // 基础移动 (1=Up, 2=Down, 3=Left, 4=Right)
    virtual ECCS_Error PtzMove(u8 action, u8 speed) = 0;
    virtual ECCS_Error PtzStop() = 0;

    // 预置位与归零
    virtual ECCS_Error PtzPreset(u8 action, u8 index) = 0; // 1=Set, 2=Goto
    virtual ECCS_Error PtzReset() = 0; // 一键归零/重启

    // 绝对定位
    virtual ECCS_Error PtzSetAbsolutePos(float pan, float tilt) = 0;

    // 主动查询
    virtual ECCS_Error PtzQueryPosition() = 0;

    // 变倍控制 (Zoom In/Out)
    virtual ECCS_Error PtzZoom(bool isZoomIn) = 0;

    // =================================================
    // 线扫 (Line Scan)
    // =================================================
    /**
     * @brief 设置线扫起始角度
     * @param startAngle 水平起始角度 (0.00 - 359.99)
     * @param endAngle 水平终止角度 (0.00 - 359.99)
     */
    virtual ECCS_Error PtzSetScanRange(float startAngle, float endAngle) = 0;

    /**
     * @brief 启动线扫
     */
    virtual ECCS_Error PtzStartScan() = 0;

    /**
     * @brief 停止线扫
     */
    virtual ECCS_Error PtzStopScan() = 0;
};

ECCS_END