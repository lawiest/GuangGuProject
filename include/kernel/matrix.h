#pragma once
#include "../utility/preconfig.h" 
#include "event.h"
#include <string.h>  
#include <mathhelp.h>  

KERNEL_NS    
	
/// \brief 基于模板的二维矩阵
/// \brief 仿射坐标转换
/// \details 仿射变换公式
/// \details x_new = m11 * x + m21 * y + dx;
/// \details y_new = m12 * x + m22 * y + dy;
/// \details 反射变换矩阵常规表达
/// \details [m11,m12,0]     
/// \details [m21,m22,0]	  
/// \details [dx ,dy ,1]
template<typename T=float>
class GS_API GsMatrixT
{
protected:
	union{

		T m_m[6];
		struct
		{
			T m_m11,m_m12,m_m21,m_m22,m_dx,m_dy;
		};
	}; 
public:
	 GsMatrixT()
	 {
		 Elements(1,0,0,1,0,0);
	 }
	/// \brief  由矩阵构造矩阵
	GsMatrixT(const GsMatrixT& rhs)
	{
		memcpy(m_m,rhs.m_m,6 * sizeof(T));
	}

	/// \brief  构造矩阵6元素
    GsMatrixT(T m11, 
           T m12,
           T m21, 
           T m22,
           T dx, 
           T dy)
	{
		m_m11 = m11;
		m_m12 = m12;
		m_m21 = m21;
		m_m22 = m22;
		m_dx = dx;
		m_dy = dy;
	}

	/// \brief 此方法初始化新矩阵，这样它表示从 source 参数指定的矩形映射到 target 参数中的三个浮点型点定义的平行四边形的几何变换。
    GsMatrixT(T* source,T* target)
	{
		GsMatrixT src(source[2] - source[0], source[3] - source[1],source[4] - source[0],source[5] - source[1],source[0],source[1]);
		GsMatrixT tar(target[2] - target[0], target[3] - target[1],target[4] - target[0],target[5] - target[1],target[0],target[1]);

		src.Invert();
		src.Multiply(tar);

		m_m11 = src.m_m11;
		m_m12 = src.m_m12;
		m_m21 = src.m_m21;
		m_m22 = src.m_m22;
		m_dx  = src.m_dx ;
		m_dy  = src.m_dy ;

	}

	/// \brief 平移矩阵
	GsMatrixT(T* offset)
	{
		Elements(1,0,0,1,0,0);
		Translate(offset[0],offset[1]);
	}

    /// \brief 旋转矩阵,角度单位（弧度）
	GsMatrixT(T fRotationAngle)
	{
		Elements(1,0,0,1,0,0);
		Rotate(fRotationAngle);
	}

	/// \brief 缩放矩阵
	GsMatrixT(T scalex,T scaley)
	{
		m_m11 = scalex;
		m_m12 = 0;
		m_m21 = 0;
		m_m22 = scaley;
		m_dx =  0;
		m_dy =  0;
	}

	virtual ~GsMatrixT(){}

	/// \brief 得到矩阵6元素
	bool Elements(T *m) const
	{
		memcpy(m,m_m,sizeof(T) * 6);
		return true;
	}
	/// \brief 以模板得到矩阵6元素
	template<class U>
	bool ElementsT(U *m) const
	{
		for (int i = 0; i < 6; i++)
			m[i] = m_m[i];
		return true;
	}

	/// \brief 以模板得到矩阵6元素
	template<class U>
	bool ElementsT(U* m11,U* m12,U* m21,U* m22,U* dx,U* dy) const
	{
		if (m11) *m11 = m_m11;
		if (m12) *m12 = m_m12;
		if (m21) *m21 = m_m21;
		if (m22) *m22 = m_m22;
		if (dx) *dx = m_dx;
		if (dy) *dy = m_dy;
		return true;
	}

	/// \brief 获取矩阵数据的指针
	const T* Ptr()const
	{
		return m_m;
	}
	/// \brief 获取矩阵数据的指针
	T* Ptr()
	{
		return m_m;
	}
	/// \brief 为矩阵6元素赋值
    GsMatrixT<T>& Elements(T m11, T m12, T m21, T m22, T dx, T dy)
	{
		m_m11 = m11;
		m_m12 = m12;
		m_m21 = m21;
		m_m22 = m22;
		m_dx = dx;
		m_dy = dy;
		return *this;
	}
	/// \brief 为矩阵6元素赋值
	template<class U>
	GsMatrixT<T>& ElementsT(U m11, U m12, U m21, U m22, U dx, U dy)
	{
		m_m11 = m11;
		m_m12 = m12;
		m_m21 = m21;
		m_m22 = m22;
		m_dx = dx;
		m_dy = dy;
		return *this;
	}
	/// \brief 为矩阵6元素赋值
	template<class U>
	GsMatrixT<T>& ElementsT(const U* m)
	{
		for (int i = 0; i < 6; i++)
			m_m[i] = m[i];
		return *this;
	}

	/// \brief 矩阵X轴上偏移量
    T OffsetX() const
	{
		return m_dx;
	}

	/// \brief 矩阵Y轴上偏移量
    T OffsetY() const
	{
		return m_dy;
	}

	/// \brief 矩阵X轴上缩放量
	T ScaleX() const
	{
		return m_m11;
	}

	/// \brief 矩阵Y轴上缩放量
    T ScaleY() const
	{
		return m_m22;
	}

	/// \brief 矩阵旋转角度
	T RotationAngle() const
	{
		return asin(m_m12);
	}

	/// \brief 矩阵初始化，导入一个单位矩阵
	GsMatrixT<T>& Reset()
	{   
		 Elements(1,0,0,1,0,0);
		return *this;
	}

	/// \brief 矩阵相乘(左乘）
	GsMatrixT& LeftMultiply(const GsMatrixT& matrix)
	{
		GsMatrixT<T> mx(matrix);
		mx.Multiply(*this);
		*this = mx;
		return *this;
	}
	/// \brief 矩阵相乘(右乘）
    GsMatrixT& Multiply(const GsMatrixT& matrix)
	{   
		//当前矩阵乘以目标矩阵 
		T m[6] = {m_m[0],m_m[1],m_m[2],m_m[3],m_m[4],m_m[5]};

		//相乘计算
		m_m11 = m[0] * matrix.m_m11 + m[1] * matrix.m_m21;
		m_m12 = m[0] * matrix.m_m12 + m[1] * matrix.m_m22;
		m_m21 = m[2] * matrix.m_m11 + m[3] * matrix.m_m21;
		m_m22 = m[2] * matrix.m_m12 + m[3] * matrix.m_m22;
		m_dx  = m[4] * matrix.m_m11 + m[5] * matrix.m_m21 + matrix.m_dx;
		m_dy  = m[4] * matrix.m_m12 + m[5] * matrix.m_m22 + matrix.m_dy;

		return *this;
	}
    
	/// \brief 矩阵乘法操作符 
	GsMatrixT operator *(const GsMatrixT& matrix)const
	{   
		GsMatrixT out(*this);
		return out.Multiply(matrix);
	}

	/// \brief 仿射变换矩阵平移
	/// \param offsetX x轴上平移量
	/// \param offsetY y轴上平移量
    GsMatrixT<T>& Translate(T offsetX, T offsetY)
	{   
		//计算平移
		m_dx += offsetX;
		m_dy += offsetY;

		return *this;
	}

	/// \brief 生成一个偏移的矩阵
	/// \param offsetX x轴上平移量
	/// \param offsetY y轴上平移量
    static GsMatrixT<T> DoTranslate(T offsetX, T offsetY)
	{
		GsMatrixT<T> mx;
		mx.Translate(offsetX,offsetY);
		return mx;
	}

	/// \brief 生成缩放矩阵
	/// \param scaleX x轴上缩放量
	/// \param scaleY y轴上缩放量
	static GsMatrixT<T> DoScale(T scaleX, T scaleY)
	{
		GsMatrixT<T> mx;
		mx.Scale(scaleX,scaleY);
		return mx;
	}

	/// \brief 生成一个旋转矩阵
	/// \param angle 矩阵旋转角度单位（弧度）
	static GsMatrixT<T> DoRotate(T angle)
	{
		GsMatrixT<T> mx;
		mx.Rotate(angle);
		return mx;
	}

	/// \brief 仿射变换矩阵旋转
	/// \param angle 矩阵旋转角度
	/// \param x 选中中心x坐标
	/// \param y 选中中心y坐标
	static GsMatrixT<T> DoRotate(T angle,T x,T y)
	{
		GsMatrixT<T> mx;
		mx.Rotate(angle,x,y);
		return mx;
	}
	/// \brief 仿射变换剪切变换（错切变换）
	/// \param shearX 矩阵横向剪切距离
	/// \param shearY 矩阵纵向剪切距离
    static GsMatrixT<T> DoShear(T shearX, T shearY)
	{   
		GsMatrixT<T> mx;
		mx.Shear(shearX,shearY);
		return mx;
	}

	/// \brief 仿射变换矩阵缩放
	/// \param scaleX x轴上缩放量
	/// \param scaleY y轴上缩放量
    GsMatrixT<T>& Scale(T scaleX, T scaleY)
	{  
		//计算缩放
		m_m11 *= scaleX;
		m_m21 *= scaleX;
		m_dx  *= scaleX;
		m_m12 *= scaleY;
		m_m22 *= scaleY;
		m_dy  *= scaleY;

		return *this;
	}

	/// \brief 仿射变换矩阵旋转
	/// \param angle 矩阵旋转角度单位（弧度）
    GsMatrixT<T>& Rotate(T angle)
	{   
		Rotate(angle, 0, 0); 
		return *this;
	}
    
	/// \brief 仿射变换矩阵旋转
	/// \param angle 矩阵旋转角度
	/// \param x 选中中心x坐标
	/// \param y 选中中心y坐标
    GsMatrixT<T>& Rotate(T angle, T x,T y)
	{   
		//计算角度
		T ca = cos(angle); 
		T sa = sin(angle);

		//得到6元素
		T m[6] = {m_m[0],m_m[1],m_m[2],m_m[3],m_m[4],m_m[5]};

		//计算旋转
		m_m11 = m[0] * ca - m[1] * sa;
		m_m12 = m[0] * sa + m[1] * ca;
		m_m21 = m[2] * ca - m[3] * sa;
		m_m22 = m[2] * sa + m[3] * ca;
		m_dx  = m[4] * ca - m[5] * sa + x - x * ca + y * sa;
		m_dy  = m[4] * sa + m[5] * ca + y - x * sa - y * ca;

		return *this;
	}

	/// \brief 仿射变换剪切变换（错切变换）
	/// \param shearX 矩阵横向剪切距离
	/// \param shearY 矩阵纵向剪切距离
    GsMatrixT<T>& Shear(T shearX, T shearY)
	{   
		//得到6元素
		T m[6] = {m_m[0],m_m[1],m_m[2],m_m[3],m_m[4],m_m[5]};

		m_m11 = m[0] + m[1] * shearX;
		m_m12 = m[0] * shearY + m[1];
		m_m21 = m[2] + m[3] * shearX;
		m_m22 = m[2] * shearY + m[3];
		m_dx  = m[4] + m[5] * shearX;
		m_dy  = m[4] * shearY + m[5];

		return *this;
	}

	/// \brief 矩阵求逆
    GsMatrixT<T>& Invert()
	{  
		//如果不可逆则返回错误
		if(! IsInvertible())
			return *this;

		//得到6元素
		T m[6] = {m_m[0],m_m[1],m_m[2],m_m[3],m_m[4],m_m[5]};

		//矩阵的行列式
		T D = m[0] * m[3] - m[1] * m[2];

		m_m11 = m[3]/D;
		m_m12 = -m[1]/D;
		m_m21 = -m[2]/D;
		m_m22 = m[0]/D;
		m_dx = (m[2] * m[5] - m[3] * m[4])/D;
		m_dy = (m[1] * m[4] - m[0] * m[5])/D;

		return *this;
	}

	/// \brief 点变换
	template<class Q>
    bool TransformPointsT(Q* pts, int count) const 
	{   
		for(int i = 0;i<count;i++)
		{
			Q x = pts[2 * i];
			Q y = pts[2 * i+1];
			pts[2 * i]   = x * m_m11 + y * m_m21 + m_dx;
			pts[2 * i+1] = x * m_m12 + y * m_m22 + m_dy;
		}
		return true;
	}

	/// \brief 变换
	template<class Q>
    bool TransformVectorsT(Q* pts, int count) const
	{   
		for(int i = 0;i<count;i++)
		{
			Q x = pts[2 * i];
			Q y = pts[2 * i+1];
			pts[2 * i]   = x * m_m11 + y * m_m12;
			pts[2 * i+1] = x * m_m21 + y * m_m22;
		}
		return true;
	}

	/// \brief 判断矩阵是否可以求逆
    bool IsInvertible() const
	{   
		//如果矩阵的行列式为0，则矩阵不可逆
		if(Utility::GsMath::IsEqual((m_m[0] * m_m[3] - m_m[1] * m_m[2]),0))
			return false;
		return true;
	}

	/// \brief 判断矩阵是否是单位矩阵
    bool IsIdentity() const
	{
		return Utility::GsMath::IsEqual(m_m11,1) && 
			   Utility::GsMath::IsEqual(m_m12,0) && 
			   Utility::GsMath::IsEqual(m_m21,0) && 
			   Utility::GsMath::IsEqual(m_m22,1) && 
			   Utility::GsMath::IsEqual(m_dx ,0) && 
			   Utility::GsMath::IsEqual(m_dy ,0);
	}

	/// \brief 判断矩阵是否相等
    bool Equals(const GsMatrixT &rhs) const
	{
		return Utility::GsMath::IsEqual(m_m11,rhs.m_m11) && 
			   Utility::GsMath::IsEqual(m_m12,rhs.m_m12) && 
			   Utility::GsMath::IsEqual(m_m21,rhs.m_m21) && 
			   Utility::GsMath::IsEqual(m_m22,rhs.m_m22) && 
			   Utility::GsMath::IsEqual(m_dx ,rhs.m_dx ) && 
			   Utility::GsMath::IsEqual(m_dy ,rhs.m_dy);
	}

	/// \brief 矩阵等号操作符
    GsMatrixT& operator=(const GsMatrixT &rhs)
	{   
		memcpy(m_m,rhs.m_m, sizeof(T) * 6);
		return *this;
	}
};

/// \brief 单精度矩阵
typedef GsMatrixT<float>  GsMatrix;
/// \brief 双精度矩阵
typedef GsMatrixT<double> GsMatrixD;
	 
/// \brief 支持对象设置矩阵，对象析构时恢复。
template<typename O,typename M>
class GS_API GsTransformLayer
{
	O* m_TransObject;
	M m_MatrixOri;
	
	/// \details 响应此事件向Display上绘制数据不会记录到Cache中
	Utility::GsDelegate<void(const M&)> OnSetTransform;
	Utility::GsDelegate<M()> OnGetTransform;

public:
	GsTransformLayer(O* t,const M& mx)
	{
		m_TransObject = t;
		if(t)
		{
			m_MatrixOri = t->Transform();
			t->Transform(mx);
		}
	}
	GsTransformLayer(O* t)
	{
		m_TransObject = t;
		if(t)
			m_MatrixOri = t->Transform(); 
	}

	/// \brief 恢复矩阵
	void Restore()
	{
		if(!m_TransObject)
			return;
		m_TransObject->Transform(m_MatrixOri);
		m_TransObject = NULL;
	}
	~GsTransformLayer()
	{
		Restore();
	}

};
KERNEL_ENDNS