//********************************************************************************** 
//** 文件名称：CT3D_Vertex3d.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维顶点基类定义
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-13
//********************************************************************************** 


#ifndef _CT3D_VERTEX3D_H_
#define _CT3D_VERTEX3D_H_
#include <osg/Vec3d>
#include "CT3D_MathEx.h"

	enum
	{
		VERTEXTYPE_V3f,				  // for Vertex3f
		VERTEXTYPE_V3d,				  // for Vertex3d

		VERTEXTYPE_V3fT2f,			  // for TexturedVertex3f
		VERTEXTYPE_V3dT2d,			  // for TexturedVertex3d
		
		VERTEXTYPE_V3fN3fC3f,		  // for ShadedVertex3f
		VERTEXTYPE_V3dN3dC3f,		  // for ShadedVertex3d
		
		VERTEXTYPE_V3fT2fN3fC3f,	  // for VersatileVertex3f
		VERTEXTYPE_V3dT2dN3dC3f,	  // for VersatileVertex3d

		VERTEXTYPE_V3fT2fT2fN3fC3f,	  // for SecondVersatileVertex3f
		VERTEXTYPE_V3dT2dT2dN3dC3f,	  // for SecondVersatileVertex3d

		VERTEXTYPE_V3fT2fT2fT2fN3fC3f,// for ThirdVersatileVertex3f
		VERTEXTYPE_V3dT2dT2dT2dN3dC3f,// for ThirdVersatileVertex3d

		VERTEXTYPE_V3dT2dN3dC3f_WithoutRGB,
		VERTEXTYPE_V3dT2dT2dN3dC3f_WithoutRGB,
		VERTEXTYPE_V3dT2dT2dT2dN3dC3f_WithoutRGB,

		VERTEXTYPE_V3fT2fN3fC3f_WithoutRGB,
		VERTEXTYPE_V3fT2fT2fN3fC3f_WithoutRGB,
		VERTEXTYPE_V3fT2fT2fT2fN3fC3f_WithoutRGB,

		VERTEXTYPE_V3fN3f,				//for VertexNormal3f
		VERTEXTYPE_V3dN3d,				//for VertexNormal3d

		//modify by yangling 20131208 begin
		VERTEXTYPE_End
		//modify by yangling 20131208 end
		
	};

	typedef struct tagPLANE3D 
	{
		double A, B, C, D;	
	}PLANE3D;

	typedef struct tagLocation
	{
		osg::Vec3d worldPosition;
		union
		{  
			struct
			{
				double dLongitude;//经度
				double dLatitude;//纬度
				double dElevation;//高程
			};

			struct
			{
				double X,Y,Z;
			};
			struct
			{
				double dblX,dblY,dblZ;
			};
			struct
			{
				double x,y,z;
			};
		}; 

	}Location;//定位

	typedef struct tagRotation
	{
		union
		{  
			struct
			{
				double dLongitudeDegree;//绕经线方向
				double dLatitudeDegree;//绕纬线方向
				double dNormalDegree;//绕法线方向
			};

			struct
			{
				double X,Y,Z;
			};
			struct
			{
				double dblX,dblY,dblZ;
			};
			struct
			{
				double x,y,z;
			};
		}; 

	}Rotation;//旋转

	typedef struct tagZoom
	{
		union
		{  
			struct
			{
				double dLongitudeOrientation;//经线方向
				double dLatitudeOrientation;//纬线方向
				double dNormalOrientation;//法线方向
			};

			struct
			{
				double X,Y,Z;
			};
			struct
			{
				double dblX,dblY,dblZ;
			};
			struct
			{
				double x,y,z;
			};
		}; 

	}Zoom;//缩放

#pragma region Vector2D

class  Vertex2d
{
	/** @defgroup Vertex2d Vertex2d-二维顶点  
    *  @{
    */
public:
	/** @name 属性（成员变量）
	* @{
	*/
	
	/** 横,纵坐标 */
	double x;
    double y;
	/** @} */ // 属性结尾

	/** @defgroup Vertex2d 操作（成员函数）
	* @{
	*/

    /** @name 构造与析构函数
	*  @{
	*/

	/** 构造函数1
	*/
	Vertex2d():x(0.0),y(0.0){}
	/** 构造函数2
	*/
	Vertex2d(double dx,double dy):x(dx),y(dy){}
	/** 构造函数3
	*/
	Vertex2d(const Vertex2d& rv):x(rv.x),y(rv.y){}
	/** @} */ // 构造与析构函数结尾

	/** 内联函数。重载二元操作符"=="，用于判断向量或坐标是否相等。
	* @param [in] vt3d	当前需要判断的向量或坐标
	* @return 1-向量或坐标相等；0-向量或坐标不等
	*/
	inline int operator==(const Vertex2d& vt2d) const
	{
		return (fabs(vt2d.x - x) < MathEx::TOL && fabs(vt2d.y - y) < MathEx::TOL);
	}

	/** 内联函数。重载二元操作符"=="，用于判断向量或坐标是否相等。
	* @param [in] vt3d	当前需要判断的向量或坐标
	* @return 1-向量或坐标相等；0-向量或坐标不等
	*/
	inline int operator!=(const Vertex2d& vt2d) const
	{
		return !((*this) == vt2d);
	}

	/** 内联函数。重载二元操作符"="，用于对向量或坐标的赋值。
	* @param [in] vt3d	当前用于赋值的向量或坐标
	* @return 赋值后的向量或坐标，返回值为引用形式，用于连续赋值
	*/
	inline Vertex2d& operator=(const Vertex2d& vt2d)
	{
		x = vt2d.x;
		y = vt2d.y;
		return (*this);
	}

	/** 内联函数。重载二元操作符"+="，用于对向量的复合赋值（加法）。
	* @param [in] vt2d	当前用于复合赋值的向量
	* @return 
	*/
	inline void operator+=(const Vertex2d& vt2d)
	{
		x += vt2d.x;
		y += vt2d.y;
	}

	/** 内联函数。重载二元操作符"-="，用于对向量的复合赋值（减法）
	* @param [in] vt3d	当前用于复合赋值的向量
	* @return 
	*/
	inline void operator-=(const Vertex2d& vt2d)
	{
		x -= vt2d.x;
		y -= vt2d.y;
	}

	/** 内联函数。重载一元操作符"+"，用于取得向量或坐标的当前值。
	* @param 
	* @return 向量或坐标的当前值
	*/
	inline Vertex2d operator+() const
	{
		return (*this);
	}

	/** 内联函数。重载一元操作符"-"，用于取得向量或坐标的相反值。
	* @param 
	* @return 向量或坐标的相反值
	*/
	inline Vertex2d operator-() const
	{
		return Vertex2d(-x,-y);
	}
	/** @}*/  // 数据操作组结尾

	/** @} */ // 操作结尾

	/** @} */ // 模块结尾
};

class  Vertex2f
{
	/** @defgroup Vertex2f Vertex2f-二维顶点  
    *  @{
    */
public:
	/** @name 属性（成员变量）
	* @{
	*/
	
	/** 横,纵坐标 */
	float x,y;
	/** @} */ // 属性结尾

	/** @defgroup Vertex2d 操作（成员函数）
	* @{
	*/

    /** @name 构造与析构函数
	*  @{
	*/

	/** 构造函数1
	*/
	Vertex2f():x(0.0),y(0.0){}
	/** 构造函数2
	*/
	Vertex2f(float fx,float fy):x(fx),y(fy){}
	/** 构造函数3
	*/
	Vertex2f(const Vertex2f& rv):x(rv.x),y(rv.y){}
    /** @} */ // 构造与析构函数结尾
	
	/** @} */ // 操作结尾

	/** @} */ // 模块结尾
};


#pragma endregion Vector2D
//##ModelId=49E86360031F
class  Vertex3d 
{
	/** @defgroup Vertex3d Vertex3d-3d简单顶点（基类）
	*  @{
	*/
public:

	/** @name 属性（成员变量）
	* @{
	*/
	
	//##ModelId=49E8636A0373
	/** X坐标值 */
	double x;

	//##ModelId=49E86375038D
	/** Y坐标值 */
	double y;

	//##ModelId=49E8637C0243
	/** Z坐标值 */
	double z;

	/** @} */ // 属性结尾

public:

	//构造函数
	Vertex3d():x(0.0),y(0.0),z(0.0){}

	Vertex3d(double dx,double dy,double dz):x(dx),y(dy),z(dz){}

	Vertex3d(const Vertex3d& rv):x(rv.x),y(rv.y),z(rv.z){}
	
 	Vertex3d(const tagLocation& rv):x(rv.x),y(rv.y),z(rv.z){}

 	Vertex3d(const tagRotation& rv):x(rv.x),y(rv.y),z(rv.z){}

 	Vertex3d(const tagZoom& rv):x(rv.x),y(rv.y),z(rv.z){}
	

	// 析构函数
	~Vertex3d(){}
	
	/** @} */ // 构造与析构函数结尾
	
	
	
	/** @name 数据操作
	*  @{
	*/

	//##ModelId=49EECBA403C8
	/** 对向量置空
	* @param 
	* @return 
	*/
	inline void Zero()
	{
		x = y = z =0.0;
	}
	/** 对向量进行单位化
	* @param 
	* @return 
	*/
	inline void Normailize()
	{
		double dLength = Length();
		(*this) /= dLength; 
	}

	//##ModelId=49EECF9A0196
	/** 内联函数。重载二元操作符"=="，用于判断向量或坐标是否相等。
	* @param [in] vt3d	当前需要判断的向量或坐标
	* @return 1-向量或坐标相等；0-向量或坐标不等
    */
	inline int operator==(const Vertex3d& vt3d) const
	{
		return (fabs(vt3d.x - x) < MathEx::TOL && fabs(vt3d.y - y) < MathEx::TOL && fabs(vt3d.z - z) < MathEx::TOL);
	}

	//##ModelId=49EECF9A0196
	/** 内联函数。重载二元操作符"=="，用于判断向量或坐标是否相等。
	* @param [in] vt3d	当前需要判断的向量或坐标
	* @return 1-向量或坐标相等；0-向量或坐标不等
	*/
	inline int operator!=(const Vertex3d& vt3d) const
	{
		return !((*this) == vt3d);
	}

	//##ModelId=49EED00902FD
	/** 内联函数。重载二元操作符"="，用于对向量或坐标的赋值。
	* @param [in] vt3d	当前用于赋值的向量或坐标
	* @return 赋值后的向量或坐标，返回值为引用形式，用于连续赋值
    */
	inline Vertex3d& operator=(const Vertex3d& vt3d)
	{
		x = vt3d.x;
		y = vt3d.y;
		z = vt3d.z;
		return (*this);
	}

	inline Vertex3d& operator=(const tagLocation& vt3d)
	{
		x = vt3d.x;
		y = vt3d.y;
		z = vt3d.z;
		return (*this);
	}

	inline Vertex3d& operator=(const tagZoom& vt3d)
	{
		x = vt3d.x;
		y = vt3d.y;
		z = vt3d.z;
		return (*this);
	}

	inline Vertex3d& operator=(const tagRotation& vt3d)
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
	inline void operator+=(const Vertex3d& vt3d)
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
	inline void operator-=(const Vertex3d& vt3d)
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
	inline void operator^=(const Vertex3d& vt3d)
	{
		double dTempX,dTempY,dTempZ;
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
	inline void operator*=(double rhd)
	{
		x *= rhd;
		y *= rhd;
		z *= rhd;
	}

	
	/** 内联函数。重载二元操作符"*="，用于对向量的复合赋值（数乘）
	* @param [in] v	当前用于复合赋值的乘数
	* @return 
    */
	inline void operator*=(const Vertex3d& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
	}



	//##ModelId=49EED0B8006D
	/** 内联函数。重载二元操作符"/="，用于对向量的复合赋值（点乘）。
	* @param [in] rhd	当前用于复合赋值的除数
	* @return 
    */
	inline void operator/=(double rhd)
	{
		if(fabs(rhd) < MathEx::TOL)	// 判断除数是否为0，若为0，则不处理
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
	inline Vertex3d operator+() const
	{
		return (*this);
	}

	//##ModelId=49EEE24C01C5
	/** 内联函数。重载一元操作符"-"，用于取得向量或坐标的相反值。
	* @param 
	* @return 向量或坐标的相反值
    */
	inline Vertex3d operator-() const
	{
		return Vertex3d(-x,-y,-z);
	}
	
	/** @} */ // 数据操作组结尾
	
	
	
	/** @name 计算操作
	*  @{
	*/

	//##ModelId=49EEDC6D030D
	/** 用于计算向量的长度（模）
	* @param 
	* @return 向量的长度
    */
	inline double Length() const
	{
		return sqrt(x*x + y*y + z*z);
	}

	//##ModelId=49EEDC7C0251
	/** 用于计算向量的长度的平方
	* @param 
	* @return 向量的长度的平方
    */
	inline double LenghtEx() const
	{
		return x*x + y*y + z*z;
	}

	//##ModelId=49EEDD110128
	/** 用于计算向量在XOY平面的投影向量的长度
	* @param 
	* @return 向量在XOY平面的投影向量的长度
    */
	inline double Length2d() const
	{
		return sqrt(x*x + y*y);
	}

	//##ModelId=49EEDD1D0290
	/** 用于计算向量在XOY平面的投影向量的长度的平方
	* @param 
	* @return 向量在XOY平面的投影向量的长度的平方
    */
	inline double Length2dEx() const
	{
		return x*x + y*y;
	}

	//##ModelId=49EF16FB01E4
	/** 用于计算向量与X轴正向的夹角
	* @param 
	* @return 向量与X轴正向的夹角（弧度表示）
    */
	inline double GetVecXDirection() const
	{
		return GetCrossAngle(Vertex3d(1,0,0));
	}

	//##ModelId=49EF16FB01E4
	/** 用于计算向量与Y轴正向的夹角
	* @param 
	* @return 向量与Y轴正向的夹角（弧度表示）
    */
	inline double GetVecYDirection() const
	{
		return GetCrossAngle(Vertex3d(0,1,0));
	}

	//##ModelId=49EF16FB01E4
	/** 用于计算向量与Z轴正向的夹角
	* @param 
	* @return 向量与Z轴正向的夹角（弧度表示）
    */
	inline double GetVecZDirection() const
	{
		return GetCrossAngle(Vertex3d(0,0,1));
	}
    /** 用于计算三维点结构大小
    * @param 
    * @return 
    */
   static size_t sizeBuffer(); 

	//##ModelId=49EF174700BB
	/** 用于计算当前向量与传入向量的夹角
	* @param [in] rhv 传入向量
	* @return 当前向量与传入向量的夹角（弧度表示）
    */
	double GetCrossAngle(const Vertex3d& rhv) const;

	/** 用于计算当前向量与传入向量的夹角(锐角)
	* @param [in] rhv 传入向量
	* @return 当前向量与传入向量的夹角(锐角)（弧度表示）
    */
	double GetCrossAcuteAngle(const Vertex3d& rhv) const;

	/** 用于计算当前向量与传入向量的夹角(锐角)的余弦值
	* @param [in] rhv 传入向量
	* @return 当前向量与传入向量的夹角(锐角)的余弦值
    */
	double GetCrossAcuteAngleCos(const Vertex3d& rhv) const;

	//##ModelId=49EF175701E4
	/** 用于计算当前向量与传入向量的夹角的余弦值
	* @param [in] rhv 传入向量
	* @return 当前向量与传入向量的夹角的余弦值
    */
	double GetCrossAngleCos(const Vertex3d& rhv) const;

    /** 用于计算当前向量与传入向量的差向量的长度
	* @param [in] rhv 传入向量
	* @return 当前向量与传入向量的的差向量的长度
    */
	double DVertexToVertex3d(const Vertex3d& rhv) const;

    /** 用于计算当前向量与传入向量的差向量在XOY平面的投影向量的长度
	* @param [in] rhv 传入向量
	* @return 当前向量与传入向量的的差向量在XOY平面的投影向量的长度
    */
	double DVertexToVertex2d(const Vertex3d& rhv) const;

	void GetAlphaBeta(double& a, double& b) const;
	
    /** 用于计算当前向量在传入向量上的投影长度
	* @param [in] rhv 传入向量
	* @return 当前向量在传入向量上的投影长度
    */
	double GetProjectLengthInVector(Vertex3d& vVec) const;

	/** 用于计算当前向量在传入向量上的投影高度
	* @param [in] rhv 传入向量
	* @return 当前向量在传入向量上的投影高度
    */
	double GetProjectHeightInVector(Vertex3d& vVec) const;



};



 const Vertex3d operator+(const Vertex3d& lhv, const Vertex3d& rhv);


 const Vertex3d operator-(const Vertex3d& lhv, const Vertex3d& rhv);


 const Vertex3d operator^(const Vertex3d& lhv, const Vertex3d& rhv);


 const Vertex3d operator*(const Vertex3d& lhv, double rhd);


 const Vertex3d operator*(double lhd, const Vertex3d& rhv);


 double operator*(const Vertex3d& lhv, const Vertex3d& rhv);


 const Vertex3d operator/(const Vertex3d& lhv, double rhd);

 
 const Vertex3d GetNormal(const Vertex3d& v1,const Vertex3d& v2,const Vertex3d& v3);

 /** @} */ // 模块结尾



#endif 
