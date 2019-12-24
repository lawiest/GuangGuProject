//********************************************************************************** 
//** 文件名称：CT3D_Quaternion.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 四元数类定义
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-14
//********************************************************************************** 

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_QUATERNION_49E34729030D_INCLUDED
#define _INC_QUATERNION_49E34729030D_INCLUDED

#include "ModelDef.h"
#include "CT3D_Vertex3d.h"

//##ModelId=49E34729030D
class  Quaternion 
{

	/** @defgroup Quaternion Quaternion-四元数
	*  @{
	*/
public:

	/** @name 属性（成员变量）
	* @{
	*/
	
	/** 成员变量 */
	double x,y,z,w;
	
	/** @} */ // 属性结尾

	/** @defgroup Material 操作（成员函数）
	* @{
	*/


	/** @name 构造与析构函数
	*  @{
	*/
private:
	//##ModelId=49E3473D0223
	Vertex3d* _v;
public:

	/** 构造函数1(默认为单元四元数)
	*/
	inline Quaternion (
		double fW = 1.0,
		double fX = 0.0, double fY = 0.0, double fZ = 0.0):w(fW),x(fX),y(fY),z(fZ){}	

	/** 构造函数2(由角度和向量构造四元数)
	*/
	inline Quaternion( double rfAngle, const Vertex3d& rkAxis)
	{
		FromAngleAxis(rfAngle,rkAxis);
	}
	
	/** 构造函数3(由已知的四元数来构建四元数)
	*/
	inline Quaternion (const Quaternion& rkQ)
	{
		w = rkQ.w;
		x = rkQ.x;
		y = rkQ.y;
		z = rkQ.z;
	}

	


	/** @} */ // 构造与析构函数结尾

	/** @name 计算操作
	*  @{
	*/

	/** 从轴向角得到四元数	
	* @param[in]rRadian 半径
	* @param[in]rkAxis 向量
	* @return 
    */
	void FromAngleAxis ( double rRadian, const Vertex3d& rkAxis);
	/** 从四元数得到半径和旋转向量	
	* @param[out]rRadian 半径
	* @param[out]rkAxis 向量
	* @return 
    */
	void ToAngleAxis (double& rRadian, Vertex3d& rkAxis) const;
	

	
	/** 赋值操作	
	* @param[in]rkQ 传入的四元数
	* @return 四元数的引用
    */
	inline Quaternion& operator= (const Quaternion& rkQ)
	{
		w = rkQ.w;
		x = rkQ.x;
		y = rkQ.y;
		z = rkQ.z;
		return *this;
	}

	Quaternion operator+ (const Quaternion& rkQ) const;
	Quaternion operator- (const Quaternion& rkQ) const;
	Quaternion operator* (const Quaternion& rkQ) const;
	Quaternion operator* (double fScalar) const;
	Vertex3d  operator* (const Vertex3d& v) const;

	Quaternion operator- () const;

	/** 等价判断	
	* @param[in]rkQ 传入的四元数
	* @return 1-等价,0-不等价
    */
	inline int operator== (const Quaternion& rhs) const //xx_0121
	{
		/*return (rhs.x == x) && 
			(rhs.y == y) &&
			(rhs.z == z) && 
			(rhs.w == w);*/

		return ( fabs(rhs.x - x) < MathEx::TOL ) &&
			( fabs(rhs.y - y) < MathEx::TOL ) &&
			( fabs(rhs.z - z) < MathEx::TOL ) &&
			( fabs(rhs.w - w) < MathEx::TOL );
	}
	/** 不等价判断	
	* @param[in]rkQ 传入的四元数
	* @return 1-不等价,0-等价
    */
	inline int operator!= (const Quaternion& rhs) const
	{
		return !operator==(rhs);
	}

    /** 计算当前四元数与传入的四元数的乘积	
	* @param[in]rkQ 传入的四元数
	* @return 当前四元数与传入的四元数的乘积
    */
	double Dot (const Quaternion& rkQ) const;  // dot product
	
	/** 计算当前四元数的模	
	* @param
	* @return 当前四元数的模
    */
	double Norm () const;  // squared-length

    /** 单位化当前四元数的模,并返回单位化之前四元数的长度
	* @param
	* @return 单位化之前的四元数的长度
    */
	double normalise(void);
	
	/** 求当前四元数的逆
	* @param
	* @return 当前四元数的逆
    */
	Quaternion Inverse () const;  // apply to non-zero quaternion
	Quaternion UnitInverse () const;  // apply to unit-length quaternion
	Quaternion Exp () const;
	Quaternion Log () const;

	/** Calculate the local roll element of this quaternion.
	@param reprojectAxis By default the method returns the 'intuitive' result
	that is, if you projected the local Y of the quaterion onto the X and
	Y axes, the angle between them is returned. If set to 0 though, the
	result is the actual yaw that will be used to implement the quaternion,
	which is the shortest possible path to get to the same orientation and 
	may involve less axial rotation. 
	*/
	double getRoll(int reprojectAxis = 1) const;
	/** Calculate the local pitch element of this quaternion
	@param reprojectAxis By default the method returns the 'intuitive' result
	that is, if you projected the local Z of the quaterion onto the X and
	Y axes, the angle between them is returned. If set to 1 though, the
	result is the actual yaw that will be used to implement the quaternion,
	which is the shortest possible path to get to the same orientation and 
	may involve less axial rotation. 
	*/
	double getPitch(int reprojectAxis = 1) const;
	/** Calculate the local yaw element of this quaternion
	@param reprojectAxis By default the method returns the 'intuitive' result
	that is, if you projected the local Z of the quaterion onto the X and
	Z axes, the angle between them is returned. If set to 1 though, the
	result is the actual yaw that will be used to implement the quaternion,
	which is the shortest possible path to get to the same orientation and 
	may involve less axial rotation. 
	*/
	double getYaw(int reprojectAxis = 1) const;		
	/// Equality with tolerance (tolerance is max angle difference)

	int equals(const Quaternion& rhs, double& tolerance) const;

	// spherical linear interpolation
	static Quaternion Slerp (double fT, const Quaternion& rkP,
		const Quaternion& rkQ, int shortestPath = 0);

	static Quaternion SlerpExtraSpins (double fT,
		const Quaternion& rkP, const Quaternion& rkQ,
		int iExtraSpins);

	// setup for spherical quadratic interpolation
	static void Intermediate (const Quaternion& rkQ0,
		const Quaternion& rkQ1, const Quaternion& rkQ2,
		Quaternion& rka, Quaternion& rkB);

	// spherical quadratic interpolation
	static Quaternion Squad (double fT, const Quaternion& rkP,
		const Quaternion& rkA, const Quaternion& rkB,
		const Quaternion& rkQ, int shortestPath = 0);

	// normalised linear interpolation - faster but less accurate (non-constant rotation velocity)
	static Quaternion nlerp(double fT, const Quaternion& rkP, 
		const Quaternion& rkQ, int shortestPath = 0);

	// cutoff for sine near zero
	static const double ms_fEpsilon;

	// special values
	static const Quaternion ZERO;
	static const Quaternion IDENTITY;

	void FromAxes (const Vertex3d* akAxis);
	void FromAxes (const Vertex3d& xAxis, const Vertex3d& yAxis, const Vertex3d& zAxis);
	void ToAxes (Vertex3d* akAxis) const;
	void ToAxes (Vertex3d& xAxis, Vertex3d& yAxis, Vertex3d& zAxis) const;
	/// Get the local x-axis
	Vertex3d xAxis(void) const;
	/// Get the local y-axis
	Vertex3d yAxis(void) const;
	/// Get the local z-axis
	Vertex3d zAxis(void) const;

	//##ModelId=49EC1D490196
	void Normalize();

	//##ModelId=49EF26290242
	

	//##ModelId=49EF269400DA
	Quaternion(const Vertex3d& xaxis, const Vertex3d& yaxis, const Vertex3d& zaxis);

    static size_t sizeBuffer();

};


#endif 
