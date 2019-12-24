//********************************************************************************** 
//** 文件名称：CT3D_VersatileVertex3dWithoutRGB.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维顶点类（全有顶点 ）实现
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-13
//********************************************************************************** 

#include "CT3D_VersatileVertex3dWithoutRGB.h"

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
VersatileVertex3dWithoutRGB::VersatileVertex3dWithoutRGB(double dx,double dy,
                                     double dz,double dnx,
                                     double dny,double dnz,
                                     double du,double dv)
:
Vertex3d(dx,dy,dz),
nx(dnx),
ny(dny),
nz(dnz),
tu(du),
tv(dv)
{

}

//********************************************************************************** 
//** 函数名称： VersatileVertex3dWithoutRGB
//** 功能描述： 构造函数2
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
VersatileVertex3dWithoutRGB::VersatileVertex3dWithoutRGB(Vertex3d& vPos,Vertex3d& vNor,double fu,double fv):
Vertex3d(vPos),
nx(vNor.x),
ny(vNor.y),
nz(vNor.z),
tu(fu),
tv(fv)
{

}

/** 构造函数3
*/
VersatileVertex3dWithoutRGB::VersatileVertex3dWithoutRGB(Vertex3d& vPos,Vertex3d& vNor,Vertex2d& vTex):
Vertex3d(vPos),
nx(vNor.x),
ny(vNor.y),
nz(vNor.z),
tu(vTex.x),
tv(vTex.y)
{

}

/** @name 数据操作
*  @{
*/

/** 获得顶点法向量	
* @param 
* @return 顶点法向量
*/
Vertex3d VersatileVertex3dWithoutRGB::GetNormal() const
{
    return Vertex3d(nx,ny,nz);
}

/** 设置顶点法向量
* @param [in]  vNor 顶点法向量	
* @return
*/
void VersatileVertex3dWithoutRGB::SetNormal(const Vertex3d& vNor) 
{
    nx = vNor.x;
    ny = vNor.y;
    nz = vNor.z;
}

/** 获得顶点纹理坐标	
* @param 
* @return 顶点纹理坐标	
*/
Vertex2d VersatileVertex3dWithoutRGB::GetTexCoor() const
{
    return Vertex2d(tu,tv);
}

/** 设置顶点纹理坐标	
* @param [in] vTex 顶点纹理坐标		
* @return
*/
void VersatileVertex3dWithoutRGB::SetTexCoor(const Vertex2d& vTex) 
{
    tu = vTex.x;
    tv = vTex.y;
}

size_t VersatileVertex3dWithoutRGB::sizeBuffer()
{
    size_t nSize = Vertex3d::sizeBuffer() + 5* sizeof(double)/* + 3* sizeof(float) */;
    return nSize;
}