
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include "CT3D_SecondVersatileVertex3d.h"


class /*DLL_EXPORT*/ ThirdVersatileVertex3d : public Vertex3d
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

    /** 纹理坐标 */
    double tu3; 
    double tv3;


    /** 颜色 */
    float r; 
    float g; 
    float b;
    /** @name 属性（成员变量）
    */
   
    /** @} */ // 属性结尾


public:




    /** @name 构造与析构函数
    *  @{
    */

	//********************************************************************************** 
	//** 函数名称： VersatileVertex3d
	//** 功能描述： 构造函数1
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨
	//** 创建日期： 2013-08-27
	//**********************************************************************************
    ThirdVersatileVertex3d(double dblx = 0.0,double dbly = 0.0,
        double dblz = 0.0,double dblnx = 0.0,
        double dblny = 0.0,double dblnz = 0.0,
        double dblu = 0.0,double dblv = 0.0,
		double dblu2 = 0.0,double dblv2 = 0.0,
		double dblu3 = 0.0,double dblv3 = 0.0,
        float fcr = 0.0,float fcg = 0.0,
        float fcb =0.0);


	//********************************************************************************** 
	//** 函数名称： VersatileVertex3d
	//** 功能描述： 构造函数2
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨
	//** 创建日期： 2012-04-16
	//**********************************************************************************
    ThirdVersatileVertex3d(Vertex3d& vPos,Vertex3d& vNor,double dblu,double dblv,double dblu2,double dblv2,double dblu3,double dblv3 ,float fcr,float fcg,float fcb);


    /** 构造函数3
    */
    ThirdVersatileVertex3d(Vertex3d& vPos,Vertex3d& vNor,Vertex2d& vTex,Vertex2d& vTex2,Vertex2d& vTex3,Vertex3f& vClr);


	/** 获得顶点纹理坐标	
    * @param 
    * @return 顶点纹理坐标	
    */
    Vertex2d GetThirdTexCoor() const;

   
		/** 设置顶点纹理坐标	
    * @param [in] vTex 顶点纹理坐标		
    * @return
    */
    void SetThirdTexCoor(const Vertex2d& vTex); 


    /** 得到点结构大小
    * @param 
    * @return 点结构大小
    */
    static size_t sizeBuffer();

};


