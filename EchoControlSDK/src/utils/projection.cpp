
#include "projection.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

ECCS_BEGIN
namespace guass{


const static double a = 6378137.0;			// 长半径
const static double b = 6356752.3142;		// 短半径
const static double x_offset = 500000.0;	// 东偏500km

const static double es2 = (a*a)/(b*b)-1.0;	// 第二偏心率的平方
const static double e2 = 1.0 - (b*b)/(a*a);	// 偏心率的平方
const static double e4 = e2*e2;
const static double e6 = e4*e2;
const static double e8 = e6*e2;
const static double e10 = e8*e2;
const static double c = a*a/b;
const static double d = a*(1.0 - e2);

namespace X{								// 子午线弧长计算参数
    const static double A0 = d*(1.0 + (3.0/4.0)*e2 + (45.0/64.0)*e4 + (175.0/256.0)*e6 + (11025.0/16384.0)*e8 /* + ... */);
    const static double B0 = d*((3.0/4.0)*e2 + (45.0/64.0)*e4 + (175.0/256.0)*e6 + (11025.0/16384.0)*e8 /* + ... */);
    const static double C0 = d*((15.0/32.0)*e4 + (175.0/368.0)*e6 + (3675.0/8192.0)*e8 /* + ... */);
    const static double D0 = d*((35.0/96.0)*e6 + (735.0/2048.0)*e8 /* + ... */);
    const static double E0 = d*((315.0/1024.0)*e8 /* + ... */);
}

namespace Bf{								// 底点纬度计算参数
    const static double C0 = 1 + e2/4 + 7*e4/64 + 15*e6/256 + 579*e8/16384 + 1515*e10/65536 /* + ... */;
    const static double C1 = 3*e2/8 + 3*e4/16 + 213*e6/2048 + 255*e8/4096 + 20861*e10/524288 /* + ... */;
    const static double C2 = 21*e4/256 + 21*e6/256 + 533*e8/8192 + 197*e10/4096 /* + ... */;
    const static double C3 = 151*e6/6144 + 151*e8/4096 + 5019*e10/131072 /* + ... */;
    const static double C4 = 1097*e8/131072 + 1097*e10/65536  /* + ... */;
    const static double C5 = 8011*e10/2621440 /* + ... */;
}

const static double PI = M_PI;
const static double factor = PI / 180.0;


void BL2XY_Precise(const double& B_d, const double& L_d, const double& L0_d, double& y, double& x)
{
    // degree -> rad
    const double B = B_d * factor;
    const double L = L_d * factor;
    const double L0 = L0_d * factor;

    const double sB = sin(B);
    const double sB3 = sB*sB*sB;
    const double sB5 = sB3*sB*sB;
    const double sB7 = sB5*sB*sB;

    const double cB = cos(B);
    const double cB2 = cB*cB;
    const double cB3 = cB2*cB;
    const double cB4 = cB3*cB;
    const double cB5 = cB4*cB;
    const double cB6 = cB5*cB;

    const double tB = tan(B);
    const double tB2= tB*tB;
    const double tB4 = tB2*tB2;

    const double l = L - L0;
    const double l2 = l*l;
    const double l3 = l2*l;
    const double l4 = l3*l;
    const double l5 = l4*l;
    const double l6 = l5*l;

    const double n2 = es2*cB2;
    const double n4 = n2*n2;
    const double N = (a*a)/(b*sqrt(1+n2));													// 卯酉圈曲率半径
    const double X0 = X::A0*B - X::B0*sB*cB - X::C0*sB3*cB - X::D0*sB5*cB - X::E0*sB7*cB;	// X0 为赤道至 纬度为 B 的子午线弧长

    y = X0 + (N/2.0)*tB*cB2*l2 + (N/24.0)*tB*cB4*(5.0 - tB2 + 9.0*n2 + 4.0*n4)*l4
        + (N/720.0)*tB*cB6*(61.0 - 58.0*tB2 + tB4 + 270.0*n2 - 330.0*n2*tB2)*l6 /* + ... */;
    x = N*cB*l + (N/6.0)*cB3*(1.0 - tB2 + n2)*l3
        + (N/120.0)*cB5*(5.0 - 18.0*tB2 + tB4 + 14.0*n2 - 58.0*n2*l2)*l5 /* + ... */;
    x += x_offset;
}
void BL2XY_Fast(const double& B_d, const double& L_d, const double& L0_d, double& y, double& x)
{
    // degree -> rad
    const double B = B_d * factor;
    const double L = L_d * factor;
    const double L0 = L0_d * factor;

    const double sB = sin(B);
    const double sB3 = sB*sB*sB;

    const double cB = cos(B);
    const double cB2 = cB*cB;
    const double cB3 = cB2*cB;
    const double cB4 = cB3*cB;

    const double tB = tan(B);
    const double tB2= tB*tB;

    const double l = L - L0;
    const double l2 = l*l;
    const double l3 = l2*l;
    const double l4 = l3*l;

    const double n2 = es2*cB2;
    const double n4 = n2*n2;
    const double N = (a*a)/(b*sqrt(1+n2));					// 卯酉圈曲率半径
    const double X0 = X::A0*B - X::B0*sB*cB - X::C0*sB3*cB;	// X0为赤道至"纬度为B的子午线"弧长

    y = X0 + (N/2.0)*tB*cB2*l2 + (N/24.0)*tB*cB4*(5.0 - tB2 + 9.0*n2 + 4.0*n4)*l4 /* + ... */;
    x = N*cB*l + (N/6.0)*cB3*(1.0 - tB2 + n2)*l3 /* + ... */;
    x += x_offset;
}

void XY2BL_Precise(const double& x, const double& y, const double& L0_d, double& B_d, double& L_d)
{
    const double B0 = Bf::C0*y/a;
    const double bf = B0 + Bf::C1*sin(2*B0) + Bf::C2*sin(4*B0) + Bf::C3*sin(6*B0)
        + Bf::C4*sin(8*B0)+ Bf::C5*sin(10*B0);	// 底点纬度(rad)
    const double cBf = cos(bf);
    const double tB = tan(bf);
    const double tB2 = tB*tB;
    const double tB4 = tB2*tB2;
    const double nf2 = es2*cBf*cBf;
    const double n = (x-x_offset)*sqrt(1+nf2)/c;
    const double n2 = n*n;
    const double n3 = n*n2;
    const double n4 = n*n3;
    const double n5 = n*n4;
    const double n6 = n*n5;

    B_d = bf/factor - ((1+nf2)/PI)*tB*(90*n2 - 7.5*(5 + 3*tB2 + nf2 - 9*nf2*tB2)*n4 + 0.25*(61 + 90*tB2 + 45*tB4)*n6 /* ... */);
    L_d = L0_d + (180*n - 30*(1 + 2*tB2 + nf2)*n3 + 1.5*(5 + 28*tB2 + 24*tB4)*n5 /* ... */)/(PI*cBf);
}
void XY2BL_Fast(const double& x, const double& y, const double& L0_d, double& B_d, double& L_d)
{
    const double B0 = Bf::C0*y/a;
    const double bf = B0 + Bf::C1*sin(2*B0) + Bf::C2*sin(4*B0) + Bf::C3*sin(6*B0);	// 底点纬度(rad)
    const double cBf = cos(bf);
    const double tB = tan(bf);
    const double tB2 = tB*tB;
    const double nf2 = es2*cBf*cBf;
    const double n = (x-x_offset)*sqrt(1+nf2)/c;
    const double n2 = n*n;
    const double n3 = n*n2;
    const double n4 = n*n3;

    B_d = bf/factor - ((1+nf2)/PI)*tB*(90*n2 - 7.5*(5 + 3*tB2 + nf2 - 9*nf2*tB2)*n4 /* ... */);
    L_d = L0_d + (180*n - 30*(1 + 2*tB2 + nf2)*n3 /* ... */)/(PI*cBf);
}


}
ECCS_END
