//********************************************************************************** 
//** 文件名称：CT3D_VersatileVertex3f.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维顶点类（全有顶点 ）实现，
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-13
//********************************************************************************** 

#include "CT3D_SecondVersatileVertex3f.h"

SecondVersatileVertex3f::SecondVersatileVertex3f(float fx,float fy,float fz,float fnx,float fny,float fnz,float fu,float fv,float fu2,float fv2,float fcr,float fcg,float fcb):
Vertex3f(fx,fy,fz),
nx(fnx),
ny(fny),
nz(fnz),
tu(fu),
tv(fv),
tu2(fu2),
tv2(fv2),
r(fcr),
g(fcg),
b(fcb)
{

}

/** 构造函数2
*/
SecondVersatileVertex3f::SecondVersatileVertex3f(Vertex3f& vPos,Vertex3f& vNor,float fu,float fv,float fu2,float fv2,float fcr,float fcg,float fcb)
:Vertex3f(vPos),
nx(vNor.x),
ny(vNor.y),
nz(vNor.z),
tu(fu),
tv(fv),
tu2(fu2),
tv2(fv2),
r(fcr),
g(fcg),
b(fcb)
{

}

SecondVersatileVertex3f::SecondVersatileVertex3f(Vertex3f& vPos,Vertex3f& vNor,Vertex2f& vTex,Vertex2f& vTex2,Vertex3f& vClr)
:Vertex3f(vPos),
nx(vNor.x),
ny(vNor.y),
nz(vNor.z),
tu(vTex.x),
tv(vTex.y),
tu2(vTex2.x),
tv2(vTex2.y),
r(vClr.x),
g(vClr.y),
b(vClr.z)
{

}
size_t SecondVersatileVertex3f::sizeBuffer()
{
    size_t nSize = Vertex3f::sizeBuffer() +  10* sizeof(float) ;
    return nSize;
}