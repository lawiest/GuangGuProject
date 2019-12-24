//********************************************************************************** 
//** 文件名称：CT3D_ShadedVertex3f.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维顶点类（用于光照计算渲染色彩的顶点）实现，
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-13
//********************************************************************************** 


#include "CT3D_ShadedVertex3f.h"

ShadedVertex3f::ShadedVertex3f(float fx,float fy,float fz,float fnx,float fny,float fnz,float fr,float fg, float fb)
:Vertex3f(fx,fy,fz),
nx(fnx),
ny(fny),
nz(fnz),
r(fr),
g(fg),
b(fb)
{

}

ShadedVertex3f::ShadedVertex3f(Vertex3f& vPos,Vertex3f& vNor,Vertex3f& vColor)
:Vertex3f(vPos),
nx(vNor.x),
ny(vNor.y),
nz(vNor.z),
r(vColor.x), 
g(vColor.y), 
b(vColor.z)
{

}
//********************************************************************************** 
//** 函数名称： sizeBuffer
//** 功能描述：计算三维点结构的大小
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	点结构大小
//** 作    者： 杨灵
//** 创建日期： 2012-04-26
//**********************************************************************************
size_t ShadedVertex3f::sizeBuffer()
{
    size_t nSize = Vertex3f::sizeBuffer() + 6*sizeof(float);
    return nSize;
}
