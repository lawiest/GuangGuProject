//********************************************************************************** 
//** 文件名称：CT3D_VersatileVertex3f.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维顶点类（全有顶点 ）定义，
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-13
//********************************************************************************** 

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include "CT3D_SecondVersatileVertex3f.h"


class /*DLL_EXPORT*/ ThirdVersatileVertex3f : public Vertex3f
{
public:

    /** 法线 */
    float nx; 
    float ny; 
    float nz;

    /** 纹理坐标 */
    float tu; 
    float tv;

    /** 纹理坐标 */
    float tu2; 
    float tv2;

    /** 纹理坐标 */
    float tu3; 
    float tv3;

    /** 颜色 */
    float r; 
    float g; 
    float b;

	
public:

	/** 构造函数1
	*/
	ThirdVersatileVertex3f(float fx = 0.0,float fy = 0.0,float fz = 0.0,float fnx = 0.0,float fny = 0.0,float fnz = 0.0,float fu = 0.0,float fv = 0.0,float fu2 = 0.0,float fv2 = 0.0,float fu3 = 0.0,float fv3 = 0.0,float fcr = 0.0,float fcg = 0.0,float fcb = 0.0);

	/** 构造函数2
	*/
	ThirdVersatileVertex3f(Vertex3f& vPos,Vertex3f& vNor,float fu,float fv,float fu2,float fv2,float fu3,float fv3,float fcr,float fcg,float fcb);

	/** 构造函数3
	*/
	ThirdVersatileVertex3f(Vertex3f& vPos,Vertex3f& vNor,Vertex2f& vTex,Vertex2f& vTex2,Vertex2f& vTex3,Vertex3f& vClr);

	/** 获得顶点纹理坐标	
	* @param 
	* @return 顶点纹理坐标	
	*/
	Vertex2f GetThirdTexCoor() const
	{
		return Vertex2f(tu3,tv3);
	}

	/** 设置顶点纹理坐标	
	* @param [in] vTex 顶点纹理坐标		
	* @return
	*/
	void SetThirdTexCoor(const Vertex2f& vTex) 
	{
		tu3 = vTex.x;
		tv3 = vTex.y;
	}	
	static size_t sizeBuffer();
};


