//********************************************************************************** 
//** 文件名称：CT3D_SecondVersatileVertex3dWithoutRGB.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维顶点类（全有顶点 ）实现
//** 创 建 人： 杨
//** 创建日期： 2013-08-27
//********************************************************************************** 

#include "CT3D_SecondVersatileVertex3dWithoutRGB.h"

/** @name 构造与析构函数
*  @{
*/

//********************************************************************************** 
//** 函数名称： SecondVersatileVertex3dWithoutRGB
//** 功能描述： 构造函数1
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨
//** 创建日期： 2013-08-27
//**********************************************************************************
SecondVersatileVertex3dWithoutRGB::SecondVersatileVertex3dWithoutRGB(double dblx,double dbly,
                                     double dblz,double dblnx,
                                     double dblny,double dblnz,
                                     double dblu,double dblv,
									 double dblu2,double dblv2):
Vertex3d(dblx,dbly,dblz),
nx(dblnx),
ny(dblny),
nz(dblnz),
tu(dblu),
tv(dblv),
tu2(dblu2),
tv2(dblv2)
{

}

//********************************************************************************** 
//** 函数名称： SecondVersatileVertex3dWithoutRGB
//** 功能描述： 构造函数2
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨
//** 创建日期： 2013-08-27
//**********************************************************************************
SecondVersatileVertex3dWithoutRGB::SecondVersatileVertex3dWithoutRGB(Vertex3d& vPos,Vertex3d& vNor,double dblu,double dblv,double dblu2,double dblv2)
:Vertex3d(vPos),
nx(vNor.x),
ny(vNor.y),
nz(vNor.z),
tu(dblu),
tv(dblv),
tu2(dblu2),
tv2(dblv2)
{

}

/** 构造函数3
*/
SecondVersatileVertex3dWithoutRGB::SecondVersatileVertex3dWithoutRGB(Vertex3d& vPos,Vertex3d& vNor,Vertex2d& vTex,Vertex2d& vTex2)
:Vertex3d(vPos),
nx(vNor.x),
ny(vNor.y),
nz(vNor.z),
tu(vTex.x),
tv(vTex.y),
tu2(vTex2.x),
tv2(vTex2.y)
{

}


/** 获得顶点纹理坐标	
* @param 
* @return 顶点纹理坐标	
*/
Vertex2d SecondVersatileVertex3dWithoutRGB::GetSecondTexCoor() const
{
	return Vertex2d(tu2,tv2);
}



/** 设置顶点纹理坐标	
* @param [in] vTex 顶点纹理坐标		
* @return
*/
void SecondVersatileVertex3dWithoutRGB::SetSecondTexCoor(const Vertex2d& vTex) 
{
	tu2 = vTex.x;
	tv2 = vTex.y;
}

size_t SecondVersatileVertex3dWithoutRGB::sizeBuffer()
{
    size_t nSize = Vertex3d::sizeBuffer() + 7* sizeof(double)/* + 3* sizeof(float)*/ ;
    return nSize;
}