// Copyright (C) 1991 - 1999 Rational Software Corporation


#include "CT3D_ShadedVertex3d.h"

//********************************************************************************** 
//** 函数名称： ShadedVertex3d
//** 功能描述： 构造函数1
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
ShadedVertex3d::ShadedVertex3d(double dx,double dy,double dz,double dnx,double dny,double dnz,float r,float g, float b)
:Vertex3d(dx,dy,dz),
nx(dx),
ny(dy),
nz(dz),
r(r),
g(g),
b(b)
{

}

//********************************************************************************** 
//** 函数名称： ShadedVertex3d
//** 功能描述： 构造函数2
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
ShadedVertex3d::ShadedVertex3d(Vertex3d& vPos,Vertex3d& vNor,Vertex3f& vColor)
:Vertex3d(vPos),
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
size_t ShadedVertex3d::sizeBuffer()
{
    size_t nSize = Vertex3d::sizeBuffer() + 3*(sizeof(double) + sizeof(float));
    return nSize;
}