//********************************************************************************** 
//** 文件名称：CT3D_Vertex3f.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维顶点类定义
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-13
//********************************************************************************** 

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _CT3D_VERTEX3F_H_
#define _CT3D_VERTEX3F_H_
#include"ModelDef.h"
#include "CT3D_MathEx.h"

//##ModelId=49E99AC50251
class /*DLL_EXPORT*/ Vertex3f 
{
	/** @defgroup Vertex3f Vertex3f-3f简单顶点（基类） 
    *  @{
    */
public:
	/** @name 属性（成员变量）
	* @{
	*/

	/** 空间坐标 */
	float x; 
    float y; 
    float z;

	/** @} */ // 属性结尾

public:

	/** @defgroup Vertex3f 操作（成员函数）
	* @{
	*/
	
	
	/** @name 构造与析构函数
	*  @{
	*/

	// 构造函数 ：（初始化列表）
	Vertex3f(float fx=0.0,float fy=0.0,float fz=0.0):x(fx),y(fy),z(fz){}
	
	// 构造函数 ：拷贝构造函数（初始化列表）
	Vertex3f(const Vertex3f& rv):x(rv.x),y(rv.y),z(rv.z){}
	
	// 析构函数
	~Vertex3f(){}
	
	/** @} */ // 构造与析构函数结尾
	
	
	
	/** @name 数据操作
	*  @{
	*/

	//##ModelId=49EECBA403C8
	/** 对向量进行单位化
	* @param 
	* @return 
	*/
	inline void Normailize()
	{
		float dLength = Length();
		(*this) /= dLength; 
	}

	//##ModelId=49EECF9A0196
	/** 内联函数。重载二元操作符"=="，用于判断向量或坐标是否相等。
	* @param [in] vt3d	当前需要判断的向量或坐标
	* @return 1-向量或坐标相等；0-向量或坐标不等
	*/
	inline int operator==(const Vertex3f& vt3d) const
	{
		return (fabs(vt3d.x - x) < MathEx::TOL_F && fabs(vt3d.y - y) < MathEx::TOL_F && fabs(vt3d.z - z) < MathEx::TOL_F);
	}

	//##ModelId=49EECF9A0196
	/** 内联函数。重载二元操作符"=="，用于判断向量或坐标是否相等。
	* @param [in] vt3d	当前需要判断的向量或坐标
	* @return 1-向量或坐标相等；0-向量或坐标不等
	*/
	inline int operator!=(const Vertex3f& vt3d) const
	{
		return !((*this) == vt3d);
	}

	//##ModelId=49EED00902FD
	/** 内联函数。重载二元操作符"="，用于对向量或坐标的赋值。
	* @param [in] vt3d	当前用于赋值的向量或坐标
	* @return 赋值后的向量或坐标，返回值为引用形式，用于连续赋值
	*/
	inline Vertex3f& operator=(const Vertex3f& vt3d)
	{
		x = vt3d.x;
		y = vt3d.y;
		z = vt3d.z;
		return (*this);
	}

	//##ModelId=49EED0B6001F
	/** 内联函数。重载二元操作符"+="，用于对向量的复合赋值（加法）。
	* @param [in] vt3d	当前用于复合赋值的向量
	* @return 
	*/
	inline void operator+=(const Vertex3f& vt3d)
	{
		x += vt3d.x;
		y += vt3d.y;
		z += vt3d.z;
	}

	//##ModelId=49EED0B8006D
	/** 内联函数。重载二元操作符"-="，用于对向量的复合赋值（减法）
	* @param [in] vt3d	当前用于复合赋值的向量
	* @return 
	*/
	inline void operator-=(const Vertex3f& vt3d)
	{
		x -= vt3d.x;
		y -= vt3d.y;
		z -= vt3d.z;
	}

	//##ModelId=49EED0B8006D
	/** 内联函数。重载二元操作符"^="，用于对向量的复合赋值（叉乘）
	* @param [in] vt3d	当前用于复合赋值的向量
	* @return 
	*/
	inline void operator^=(const Vertex3f& vt3d)
	{
		float dTempX,dTempY,dTempZ;
		dTempX = y * vt3d.z - z * vt3d.y;
		dTempY = z * vt3d.x - x * vt3d.z;
		dTempZ = x * vt3d.y - y * vt3d.x;
		x = dTempX; y = dTempY; z = dTempZ;
	}

	//##ModelId=49EED0B8006D
	/** 内联函数。重载二元操作符"*="，用于对向量的复合赋值（数乘）
	* @param [in] rhd	当前用于复合赋值的乘数
	* @return 
	*/
	inline void operator*=(float rhd)
	{
		x *= rhd;
		y *= rhd;
		z *= rhd;
	}


	//##ModelId=49EED0B8006D
	/** 内联函数。重载二元操作符"/="，用于对向量的复合赋值（点乘）。
	* @param [in] rhd	当前用于复合赋值的除数
	* @return 
	*/
	inline void operator/=(float rhd)
	{
		if(fabs(rhd) < MathEx::TOL_F)	// 判断除数是否为0，若为0，则不处理
			return;
		else
		{
			x /= rhd;
			y /= rhd;
			z /= rhd;
		}
	}

	//##ModelId=49EEE24C01C5
	/** 内联函数。重载一元操作符"+"，用于取得向量或坐标的当前值。
	* @param 
	* @return 向量或坐标的当前值
	*/
	inline Vertex3f operator+() const
	{
		return (*this);
	}

	//##ModelId=49EEE24C01C5
	/** 内联函数。重载一元操作符"-"，用于取得向量或坐标的相反值。
	* @param 
	* @return 向量或坐标的相反值
	*/
	inline Vertex3f operator-() const
	{
		return Vertex3f(-x,-y,-z);
	}
	
	/** @} */ // 数据操作组结尾
	
	
    /** 用于计算三维点结构大小
    * @param 
    * @return 
    */
   static size_t sizeBuffer(); 
	/** @name 计算操作
	*  @{
	*/

	//##ModelId=49EEDC6D030D
	/** 用于计算向量的长度（模）
	* @param 
	* @return 向量的长度
	*/
	inline float Length() const
	{
		return sqrt(x*x + y*y + z*z);
	}

	//##ModelId=49EEDC7C0251
	/** 用于计算向量的长度的平方
	* @param 
	* @return 向量的长度的平方
	*/
	inline float LenghtEx() const
	{
		return x*x + y*y + z*z;
	}

	//##ModelId=49EEDD110128
	/** 用于计算向量在XOY平面的投影向量的长度
	* @param 
	* @return 向量在XOY平面的投影向量的长度
	*/
	inline float Length2d() const
	{
		return sqrt(x*x + y*y);
	}

	//##ModelId=49EEDD1D0290
	/** 用于计算向量在XOY平面的投影向量的长度的平方
	* @param 
	* @return 向量在XOY平面的投影向量的长度的平方
	*/
	inline float Length2dEx() const
	{
		return x*x + y*y;
	}

	//##ModelId=49EF16FB01E4
	/** 用于计算向量与X轴正向的夹角
	* @param 
	* @return 向量与X轴正向的夹角（弧度表示）
	*/
	inline float GetVecXDirection() const
	{
		return GetCrossAngle(Vertex3f(1,0,0));
	}

	//##ModelId=49EF16FB01E4
	/** 用于计算向量与Y轴正向的夹角
	* @param 
	* @return 向量与Y轴正向的夹角（弧度表示）
	*/
	inline float GetVecYDirection() const
	{
		return GetCrossAngle(Vertex3f(0,1,0));
	}

	//##ModelId=49EF16FB01E4
	/** 用于计算向量与Z轴正向的夹角
	* @param 
	* @return 向量与Z轴正向的夹角（弧度表示）
	*/
	inline float GetVecZDirection() const
	{
		return GetCrossAngle(Vertex3f(0,0,1));
	}

	//##ModelId=49EF174700BB
	/** 用于计算当前向量与传入向量的夹角
	* @param [in] rhv 传入向量
	* @return 当前向量与传入向量的夹角（弧度表示）
	*/
	float GetCrossAngle(const Vertex3f& rhv) const;

	//##ModelId=49EF175701E4
	/** 用于计算当前向量与传入向量的夹角的余弦值
	* @param [in] rhv 传入向量
	* @return 当前向量与传入向量的夹角的余弦值
	*/
	float GetCrossAngleCos(const Vertex3f& rhv) const;

    /** 用于计算当前向量与传入向量的差向量的长度
	* @param [in] rhv 传入向量
	* @return 当前向量与传入向量的的差向量的长度
    */
	float DVertexToVertex3d(const Vertex3f& rhv) const;

	/** 用于计算当前向量与传入向量的差向量在XOY平面的投影向量的长度
	* @param [in] rhv 传入向量
	* @return 当前向量与传入向量的的差向量在XOY平面的投影向量的长度
    */
	float DVertexToVertex2d(const Vertex3f& rhv) const;
	
	/** @} */ // 计算操作组结尾
	
	/** @} */ // 操作结尾

};

/*DLL_EXPORT*/ const Vertex3f operator+(const Vertex3f& lhv, const Vertex3f& rhv);


/*DLL_EXPORT*/ const Vertex3f operator-(const Vertex3f& lhv, const Vertex3f& rhv);


/*DLL_EXPORT*/ const Vertex3f operator^(const Vertex3f& lhv, const Vertex3f& rhv);


/*DLL_EXPORT*/ const Vertex3f operator*(const Vertex3f& lhv, float rhd);


/*DLL_EXPORT*/ const Vertex3f operator*(float lhd, const Vertex3f& rhv);


/*DLL_EXPORT*/ float operator*(const Vertex3f& lhv, const Vertex3f& rhv);


/*DLL_EXPORT*/ const Vertex3f operator/(const Vertex3f& lhv, float rhd);
	/** @} */ // 模块结尾


#endif /* _INC_VERTEX3F_49E99AC50251_INCLUDED */
