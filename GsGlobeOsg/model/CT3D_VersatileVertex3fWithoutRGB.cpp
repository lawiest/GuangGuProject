//********************************************************************************** 
//** 文件名称：CT3D_VersatileVertex3fWithoutRGB.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维顶点类（全有顶点 ）实现，
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-13
//********************************************************************************** 

#include "CT3D_VersatileVertex3fWithoutRGB.h"

VersatileVertex3fWithoutRGB::VersatileVertex3fWithoutRGB(float fx,float fy,float fz,float fnx,float fny,float fnz,float fu,float fv):
Vertex3f(fx,fy,fz),
nx(fnx),
ny(fny),
nz(fnz),
tu(fu),
tv(fv)
{

}

/** 构造函数2
*/
VersatileVertex3fWithoutRGB::VersatileVertex3fWithoutRGB(Vertex3f& vPos,Vertex3f& vNor,float fu,float fv)
:Vertex3f(vPos),
nx(vNor.x),
ny(vNor.y),
nz(vNor.z),
tu(fu),
tv(fv)
{

}

VersatileVertex3fWithoutRGB::VersatileVertex3fWithoutRGB(Vertex3f& vPos,Vertex3f& vNor,Vertex2f& vTex)
:Vertex3f(vPos),
nx(vNor.x),
ny(vNor.y),
nz(vNor.z),
tu(vTex.x),
tv(vTex.y)
{

}

size_t VersatileVertex3fWithoutRGB::sizeBuffer()
{
    size_t nSize = Vertex3f::sizeBuffer() +  5* sizeof(float) ;
    return nSize;
}