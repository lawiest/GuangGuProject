//********************************************************************************** 
//** 文件名称：CT3D_SecondVersatileVertex3d.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维顶点类（全有顶点 ）定义，
//** 创 建 人： 杨
//** 创建日期： 2013-08-27
//********************************************************************************** 

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include "CT3D_VersatileVertex3d.h"


class /*DLL_EXPORT*/ SecondVersatileVertex3d : public Vertex3d
{
    /** @defgroup VersatileVertex3d VersatileVertex3d-全有顶点  
    *  @{
    */
public:
    /** 法线 */
    double nx; 
    double ny; 
    double nz;

    /** 纹理坐标 */
    double tu; 
    double tv;

    /** 纹理坐标 */
    double tu2; 
    double tv2;

    /** 颜色 */
    float r; 
    float g; 
    float b;
 

    /** @} */ // 属性结尾
public:
    /** @name 构造与析构函数
    *  @{
    */

	//********************************************************************************** 
	//** 函数名称： SecondVersatileVertex3d
	//** 功能描述： 构造函数1
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨
	//** 创建日期： 2012-08-27
	//**********************************************************************************
    SecondVersatileVertex3d(double dblx = 0.0,double dbly = 0.0,
        double dblz = 0.0,double dblnx = 0.0,
        double dblny = 0.0,double dblnz = 0.0,
        double dblu = 0.0,double dblv = 0.0,
		double dblu2 = 0.0,double dblv2 = 0.0,
        float fcr = 0.0,float fcg = 0.0,
        float fcb =0.0);


	//********************************************************************************** 
	//** 函数名称： SecondVersatileVertex3d
	//** 功能描述： 构造函数2
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
    SecondVersatileVertex3d(Vertex3d& vPos,Vertex3d& vNor,double dblu,double dblv,double dblu2,double dblv2 ,float fcr,float fcg,float fcb);


    /** 构造函数3
    */
    SecondVersatileVertex3d(Vertex3d& vPos,Vertex3d& vNor,Vertex2d& vTex,Vertex2d& vTex2,Vertex3f& vClr);



    Vertex2d GetSecondTexCoor() const;

   

    void SetSecondTexCoor(const Vertex2d& vTex); 

    
    static size_t sizeBuffer();

};


