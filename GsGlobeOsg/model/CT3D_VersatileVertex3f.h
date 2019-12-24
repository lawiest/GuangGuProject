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
#ifndef _CT3D_VERSATILEVERTEX3F_H_
#define _CT3D_VERSATILEVERTEX3F_H_

#include "CT3D_Vertex3f.h"
#include "CT3D_Vertex3d.h"

class /*DLL_EXPORT*/ VersatileVertex3f : public Vertex3f
{
	/** @defgroup VersatileVertex3f VersatileVertex3f-全有顶点  
    *  @{
    */

public:
	/** @name 属性（成员变量）
	* @{
	*/
	
	/** 法线 */
	float nx; 
    float ny; 
    float nz;
	
	/** 纹理坐标 */
	float tu; 
    float tv;

	/** 颜色 */
	float r; 
    float g; 
    float b;

	/** @} */ // 属性结尾

public:

	/** @defgroup VersatileVertex3f 操作（成员函数）
	* @{
	*/
	
	
	/** @name 构造与析构函数
	*  @{
	*/

	/** 构造函数1
	*/
	VersatileVertex3f(float fx = 0.0,float fy = 0.0,float fz = 0.0,float fnx = 0.0,float fny = 0.0,float fnz = 0.0,float fu = 0.0,float fv = 0.0,float fcr = 0.0,float fcg = 0.0,float fcb = 0.0);
	  
	  
	  /** 构造函数2
	*/
	VersatileVertex3f(Vertex3f& vPos,Vertex3f& vNor,float fu,float fv,float fcr,float fcg,float fcb);
	 
	  /** 构造函数3
	*/
	  VersatileVertex3f(Vertex3f& vPos,Vertex3f& vNor,Vertex2f& vTex,Vertex3f& vClr);
	  
	
	/** @} */ // 构造与析构函数结尾
	
	
	
	/** @name 数据操作
	*  @{
	*/

	  /** 获得顶点法向量	
	* @param 
	* @return 顶点法向量
    */
	  Vertex3f GetNormal() const
	  {
		  return Vertex3f(nx,ny,nz);
	  }

	   /** 设置顶点法向量
	* @param [in]  vNor 顶点法向量	
	* @return
    */
	  void SetNormal(const Vertex3f& vNor) 
	  {
		  nx = vNor.x;
		  ny = vNor.y;
		  nz = vNor.z;
	  }
	  
	  /** 获得顶点纹理坐标	
	* @param 
	* @return 顶点纹理坐标	
    */
	  Vertex2f GetTexCoor() const
	  {
		  return Vertex2f(tu,tv);
	  }

	   /** 设置顶点纹理坐标	
	* @param [in] vTex 顶点纹理坐标		
	* @return
    */
	  void SetTexCoor(const Vertex2f& vTex) 
	  {
		  tu = vTex.x;
		  tv = vTex.y;
	  }
	  
	  /** 获得顶点颜色	
	* @param 
	* @return 顶点颜色
    */
	  Vertex3f GetColor() const
	  {
		  return Vertex3f(r,g,b);
	  }
	  
	  
	  /** 设置顶点颜色
	* @param [in] vClr	 顶点颜色
	* @return
    */
	  void SetColor(const Vertex3f& vClr)
	  {
		  r = vClr.x;
		  g = vClr.y;
		  b = vClr.z;
	}

	
static size_t sizeBuffer();
};

#endif
