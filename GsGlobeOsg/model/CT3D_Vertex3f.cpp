//********************************************************************************** 
//** 文件名称：CT3D_Vertex3f.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维顶点类实现
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-13
//********************************************************************************** 

#include "CT3D_Vertex3f.h"



size_t Vertex3f::sizeBuffer()
{
    size_t nSize = 3*sizeof(float);
    return nSize;
}
	//##ModelId=49EED0B8006D
	/** 内联函数。重载二元操作符"*="，用于对向量的复合赋值（数乘）
	* @param [in] m4x4	当前用于复合赋值的乘数
	* @return 
    */

		//##ModelId=49EF174700BB
		float Vertex3f::GetCrossAngle(const Vertex3f& rhv) const
		{
			// TODO: Add your specialized code here.

			float dCosAngle = GetCrossAngleCos(rhv);
			float Angle;

			// 计算注意，考虑到acos函数只能计算-1到1之间，因此对于1和-1分开处理
			if(fabs(dCosAngle - 1) < MathEx::TOL) 
				Angle = 0;
			else if(fabs(dCosAngle + 1) < MathEx::TOL)
				Angle = float(MathEx::ONE_PI);
			else
				Angle = acos( dCosAngle );
			return Angle;
		}

		//##ModelId=49EF175701E4
		float Vertex3f::GetCrossAngleCos(const Vertex3f& rhv) const
		{
			// TODO: Add your specialized code here.
			float ddd = Length() * rhv.Length();
			if(fabs(ddd) < MathEx::TOL)
				return 1; // 0向量与任意向量平行，其夹角可以认为是0°，cos值为1
			return ((*this) * rhv) / ddd;
		}

		float Vertex3f::DVertexToVertex2d(const Vertex3f& rhv) const
		{
			return ((*this) - rhv).Length2d();
		}
		float Vertex3f:: DVertexToVertex3d(const Vertex3f& rhv) const
		{
			return ((*this) - rhv).Length();
		}

		const Vertex3f operator+(const Vertex3f& lhv, const Vertex3f& rhv)
		{
			Vertex3f vRes = lhv;
			vRes += rhv;
			return vRes;
		}

		const Vertex3f operator-(const Vertex3f& lhv, const Vertex3f& rhv)
		{
			Vertex3f vRes = lhv;
			vRes -= rhv;
			return vRes;
		}

		const Vertex3f operator^(const Vertex3f& lhv, const Vertex3f& rhv)
		{
			Vertex3f vRes = lhv;
			vRes ^= rhv;
			return vRes;
		}

		const Vertex3f operator*(const Vertex3f& lhv, float rhd)
		{
			Vertex3f vRes = lhv;
			vRes *= rhd;
			return vRes;
		}

		const Vertex3f operator*(float lhd, const Vertex3f& rhv)
		{
			Vertex3f vRes = rhv;
			vRes *= lhd;
			return vRes;
		}

		float operator*(const Vertex3f& lhv, const Vertex3f& rhv)
		{
			// 如果Vertex3f的数据成员xyz如果是私有的，那么需要在类中声明这个函数为friend，即使这样，其他函数都不用声明为friend，因为采用的都是类的内部实现
			return lhv.x * rhv.x + lhv.y * rhv.y + lhv.z * rhv.z;
		}

		const Vertex3f operator/(const Vertex3f& lhv, float rhd)
		{
			Vertex3f vRes = lhv;
			vRes /= rhd;
			return vRes;
		}

