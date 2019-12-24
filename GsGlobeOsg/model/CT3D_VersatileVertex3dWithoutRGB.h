//********************************************************************************** 
//** 文件名称：CT3D_VersatileVertex3dWithoutRGB.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维顶点类（全有顶点 ）定义，
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-13
//********************************************************************************** 

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _CT3D_VERSATILEVERTEX3DWITHOUTRGB_H_
#define _CT3D_VERSATILEVERTEX3DWITHOUTRGB_H_

#include "CT3D_Vertex3d.h"
#include "CT3D_Vertex3f.h"


class /*DLL_EXPORT*/ VersatileVertex3dWithoutRGB : public Vertex3d
{
    /** @defgroup VersatileVertex3dWithoutRGB VersatileVertex3dWithoutRGB-全有顶点  
    *  @{
    */
public:
    /** @name 属性（成员变量）
    * @{
    */

    /** 法线 */
    double nx;
    double ny; 
    double nz;

    /** 纹理坐标 */
    double tu; 
    double tv;

    /** @} */ // 属性结尾


public:

    /** @defgroup VersatileVertex3dWithoutRGB 操作（成员函数）
    * @{
    */


    /** @name 构造与析构函数
    *  @{
    */

	//********************************************************************************** 
	//** 函数名称： VersatileVertex3dWithoutRGB
	//** 功能描述： 构造函数1
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
    VersatileVertex3dWithoutRGB(double fx = 0.0,double fy = 0.0,
        double fz = 0.0,double fnx = 0.0,
        double fny = 0.0,double fnz = 0.0,
        double fu = 0.0,double fv = 0.0);


	//********************************************************************************** 
	//** 函数名称： VersatileVertex3dWithoutRGB
	//** 功能描述： 构造函数2
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
    VersatileVertex3dWithoutRGB(Vertex3d& vPos,Vertex3d& vNor,double fu,double fv);


    /** 构造函数3
    */
    VersatileVertex3dWithoutRGB(Vertex3d& vPos,Vertex3d& vNor,Vertex2d& vTex);


    /** @} */ // 构造与析构函数结尾



    /** @name 数据操作
    *  @{
    */

    /** 获得顶点法向量	
    * @param 
    * @return 顶点法向量
    */
    Vertex3d GetNormal() const;


    /** 设置顶点法向量
    * @param [in]  vNor 顶点法向量	
    * @return
    */
    void SetNormal(const Vertex3d& vNor); 


    /** 获得顶点纹理坐标	
    * @param 
    * @return 顶点纹理坐标	
    */
    Vertex2d GetTexCoor() const;


    /** 设置顶点纹理坐标	
    * @param [in] vTex 顶点纹理坐标		
    * @return
    */
    void SetTexCoor(const Vertex2d& vTex); 


    /** 得到点结构大小
    * @param 
    * @return 点结构大小
    */
    static size_t sizeBuffer();

};


#endif
