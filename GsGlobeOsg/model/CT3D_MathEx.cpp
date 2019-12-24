//********************************************************************************** 
//** 文件名称：CT3D_MathEx.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维数学类实现
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-13
//********************************************************************************** 
#include "CT3D_MathEx.h"

double const MathEx::TOL = 1e-6;
double const MathEx::TOL_F = 1e-6;
double const MathEx::ONE_PI = 4.0 * atan( 1.0 );

double const MathEx::TWO_PI =  2.0 * MathEx::ONE_PI;
double const MathEx::HALF_PI = 0.5 * MathEx::ONE_PI;
double const MathEx::dDegToRad = MathEx::ONE_PI / 180.0;
double const MathEx::dRadToDeg = 180.0 / MathEx::ONE_PI;
const double MathEx::dDoubleMax = 1e+14;
const double MathEx::dDoubleMin = -1e+14;

MathEx::MathEx(void)
{
}

MathEx::~MathEx(void)
{
}

double MathEx::ACos (double dCosAngle)
{
    double dAngle;
    if(fabs(dCosAngle - 1) < MathEx::TOL) 
        dAngle = 0;
    else if(fabs(dCosAngle + 1) < MathEx::TOL)
        dAngle = ONE_PI;
    else
        dAngle = acos( dCosAngle );

    return dAngle;
}
//-----------------------------------------------------------------------
double MathEx::ASin (double dSinAngle)
{
    if(fabs(dSinAngle + 1) < MathEx::TOL)
        return (-HALF_PI);
    else if(fabs(dSinAngle + 1) < MathEx::TOL)
        return (HALF_PI);
    else
        return asin(dSinAngle);
}

double MathEx::Sign (double fValue)
{
    if ( fValue > MathEx::TOL )
        return 1.0;

    if ( fValue < -MathEx::TOL )
        return -1.0;

    return 0.0;
}

double MathEx::RadianToDegree(double angle)
{
    return angle * dRadToDeg;
}

double MathEx::DegreeToRadian(double angle)
{
    return angle * dDegToRad;
}
