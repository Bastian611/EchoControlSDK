
//------------------------------------------------------
// WGS84大地坐标 与 高斯克吕格投影坐标 转换
//
// 参考:
//  《一种车载GPS系统坐标转换公式及其应用》
//  《高精度底点纬度公式-曾启雄》
//  《高斯投影坐标正反算公式及适合电算的高斯投影公式》
//------------------------------------------------------

#pragma once
#include "../global.h"
#include "../types.h"

ECCS_BEGIN
namespace guass{


//------------------------------------------------------
// WGS84大地坐标 -> 高斯投影坐标 (高精度，慢)
//  B: 纬度, degree
//  L: 经度, degree
// L0: 中央经线, degree
//------------------------------------------------------
void BL2XY_Precise(const double& B_d, const double& L_d, const double& L0_d, double& y, double& x);

//------------------------------------------------------
// WGS84大地坐标 -> 高斯投影坐标 (低精度，快)
//  B: 纬度, degree
//  L: 经度, degree
// L0: 中央经线, degree
//------------------------------------------------------
void BL2XY_Fast(const double& B_d, const double& L_d, const double& L0_d, double& y, double& x);

//------------------------------------------------------
// 高斯投影坐标 -> WGS84大地坐标 (高精度，慢)
// L0: 中央经线, degree
//  B: 纬度, degree
//  L: 经度, degree
//------------------------------------------------------
void XY2BL_Precise(const double& x, const double& y, const double& L0_d, double& B_d, double& L_d);

//------------------------------------------------------
// 高斯投影坐标 -> WGS84大地坐标 (低精度，快)
// L0: 中央经线, degree
// B: 纬度, degree
// L: 经度, degree
//------------------------------------------------------
void XY2BL_Fast(const double& x, const double& y, const double& L0_d, double& B_d, double& L_d);


}
ECCS_END
