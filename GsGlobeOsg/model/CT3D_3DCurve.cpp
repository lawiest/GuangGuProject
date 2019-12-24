//********************************************************************************** 
//** 文件名称：CT3D_3DSurface.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维曲线类实现
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-11
//********************************************************************************** 

//#include "stdafx.h"
#include "CT3D_3DCurve.h"

//********************************************************************************** 
//** 函数名称： C3DSurface
//** 功能描述： 构造函数
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-11
//**********************************************************************************
C3DCurve::C3DCurve()
{
	
}

//********************************************************************************** 
//** 函数名称： ~C3DPolygon
//** 功能描述： 析构函数
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-11
//**********************************************************************************
C3DCurve::~C3DCurve()
{

}

//********************************************************************************** 
//** 函数名称： readBuffer
//** 功能描述： 将数据读入buf
//** 输    入： Buffer对象buf的引用           
//** 输    出： 无
//** 返回值：	成功为true，失败为false
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
bool C3DCurve::readBuffer(Buffer& buf)
{
	return C3DGeometry::readBuffer(buf);
}

//********************************************************************************** 
//** 函数名称： writeBuffer
//** 功能描述： 将数据写入buf
//** 输    入： Buffer对象buf的引用           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
void C3DCurve::writeBuffer(Buffer& buf)
{	
	C3DGeometry::writeBuffer(buf);
}

size_t C3DCurve::sizeBuffer()
{
	return C3DGeometry::sizeBuffer();
}
	
