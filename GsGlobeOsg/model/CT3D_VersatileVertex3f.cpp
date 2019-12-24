//********************************************************************************** 
//** 文件名称：CT3D_VersatileVertex3f.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维顶点类（全有顶点 ）实现，
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-13
//********************************************************************************** 

#include "CT3D_VersatileVertex3f.h"

VersatileVertex3f::VersatileVertex3f(float fx,float fy,float fz,float fnx,float fny,float fnz,float fu,float fv,float fcr,float fcg,float fcb):
Vertex3f(fx,fy,fz),
nx(fnx),
ny(fny),
nz(fnz),
tu(fu),
tv(fv),
r(fcr),
g(fcg),
b(fcb)
{

}

/** 构造函数2
*/
VersatileVertex3f::VersatileVertex3f(Vertex3f& vPos,Vertex3f& vNor,float fu,float fv,float fcr,float fcg,float fcb)
:Vertex3f(vPos),
nx(vNor.x),
ny(vNor.y),
nz(vNor.z),
tu(fu),
tv(fv),
r(fcr),
g(fcg),
b(fcb)
{

}

VersatileVertex3f::VersatileVertex3f(Vertex3f& vPos,Vertex3f& vNor,Vertex2f& vTex,Vertex3f& vClr)
:Vertex3f(vPos),
nx(vNor.x),
ny(vNor.y),
nz(vNor.z),
tu(vTex.x),
tv(vTex.y),
r(vClr.x),
g(vClr.y),
b(vClr.z)
{

}

size_t VersatileVertex3f::sizeBuffer()
{
    size_t nSize = Vertex3f::sizeBuffer() +  8* sizeof(float) ;
    return nSize;
}