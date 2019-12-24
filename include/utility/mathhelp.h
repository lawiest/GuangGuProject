 #pragma once
#include "preconfig.h" 
#include "stringhelp.h"
#include <stdlib.h> 
#include <math.h>
#include <float.h>
UTILITY_NS  
	
/// \brief 数学相关的函数
class GS_API GsMath
{
public:
	template<class T>
	static T Random(T min,T max)
	{
		int n  = rand();
		return min + n * 1.0 * (max - min)/ INT_MAX ;
	}
	template<class U,class V>
	static U Max(U a,V b)
	{
		return a >b?a:b;
	}
	template<class U,class V>
	static U Min(U a,V b)
	{
		return a <b?a:b;
	}
	/// \brief 判断两个double是否相等
	static bool IsEqual(double d1,double d2);

	/// \brief 判断两个float是否相等
	static bool IsEqual(float f1,float f2);
	
	/// \brief 判断两个double是否相等
	static bool IsEqual(double d1, double d2,double tol);

	/// \brief 判断两个float是否相等
	static bool IsEqual(float f1, float f2,float tol);


	/// \brief 四舍五入到最邻近的整数
	static double Round(double r);

	/// \brief 四舍五入到最邻近的整数
	static float Round(float r);

	/// \brief 度转换为弧度
	static double ToRadian(double degree);
	/// \brief 弧度转换为度
	static double ToDegree(double radian);

	/// \brief 无效double
	/// \details Not-A-Number不是一个数字的缩写，可以表示无效的double值，0/0.除数为0的结果
	static double NaN();

	/// \brief 判断一个double值是否是无效的Double值
	static bool IsNaN(double val);

	/// \brief 获取圆周率PI
	static double Pi();
	
	/// \brief GUID 变成二进制数组 长度128位
	static bool GUID2Bytes(const char * strGuid, Utility::GsByteBuffer* pBuffer);

	/// \brief 获取一个GUID 格式为：E1F17786-DABD-4C0B-9679-514E6566E8B2
	static GsString NewGUID();

	/// \brief 获取自然对数
	static double Log(double val);

};
UTILITY_ENDNS