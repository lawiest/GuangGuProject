//********************************************************************************** 
//** 文件名称：CT3D_3DGeometry.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维几何类实现
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-11
//********************************************************************************** 

#include "CT3D_3DGeometry.h"

//********************************************************************************** 
//** 函数名称： C3DGeometry
//** 功能描述： 构造函数
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-11
//**********************************************************************************
C3DGeometry::C3DGeometry()
{
	m_uFID = -1;
    m_byTypeID = GEOTYPE_3D_GEOMETRY;
    m_vMaxPos.x = 0.0;
    m_vMaxPos.y = 0.0;
    m_vMaxPos.z = 0.0;
    m_vMinPos.x = 0.0;
    m_vMinPos.y = 0.0;
    m_vMinPos.z = 0.0;
    m_strObjName="3DGeometry";
    m_nObjNameLen = m_strObjName.length();
}

//********************************************************************************** 
//** 函数名称： ~C3DGeometry
//** 功能描述： 析构函数
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-11
//**********************************************************************************
C3DGeometry::~C3DGeometry()
{

}
//********************************************************************************** 
//** 函数名称： CreateObject
//** 功能描述： 创建三维几何对象
//** 输    入： 无           
//** 输    出： 无
//** 返回值：几何对象指针
//** 作    者： 杨灵
//** 创建日期： 2012-04-11
//**********************************************************************************
CGeometry* C3DGeometry::CreateObject()
{
    return new C3DGeometry;
}
//********************************************************************************** 
//** 函数名称： readBuffer
//** 功能描述： 将数据读入buf
//** 输    入： Buffer对象buf的引用           
//** 输    出： 无
//** 返回值：	成功为true，失败为false
//** 作    者： 杨灵
//** 创建日期： 2012-04-11
//**********************************************************************************
bool C3DGeometry::readBuffer( Buffer& buf)
{
	bool bRet = CGeometry::readBuffer(buf);
	if(!bRet)
	{
		return false;
	}
	//读取AABB的最小顶点位置(X,Y,Z最小)
	buf.read((char*)&m_vMinPos.x,sizeof(double));
    buf.read((char*)&m_vMinPos.y,sizeof(double));
    buf.read((char*)&m_vMinPos.z,sizeof(double));
	//读取AABB的最大顶点位置(X,Y,Z最大)
	buf.read((char*)&m_vMaxPos.x,sizeof(double));
    buf.read((char*)&m_vMaxPos.y,sizeof(double));
    buf.read((char*)&m_vMaxPos.z,sizeof(double));
	return true;
}

//********************************************************************************** 
//** 函数名称： writeBuffer
//** 功能描述： 将数据写入buf
//** 输    入： Buffer对象buf的引用           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-11
//**********************************************************************************
void C3DGeometry::writeBuffer(Buffer& buf)
{
	CGeometry::writeBuffer(buf);
	//写入AABB的最小顶点位置(X,Y,Z最小)
	buf.write((char*)&m_vMinPos.x,sizeof(double));
    buf.write((char*)&m_vMinPos.y,sizeof(double));
    buf.write((char*)&m_vMinPos.z,sizeof(double));
	//写入AABB的最大顶点位置(X,Y,Z最大)
	buf.write((void*)&m_vMaxPos.x,sizeof(double));
    buf.write((void*)&m_vMaxPos.y,sizeof(double));
    buf.write((void*)&m_vMaxPos.z,sizeof(double));
}

void C3DGeometry::setMinPos(Vertex3d vMinPos)
{
	m_vMinPos=vMinPos;
}
Vertex3d& C3DGeometry::getMinPos()
{
	return m_vMinPos;
}

void C3DGeometry::setMaxPos(Vertex3d vMaxPos)
{
	m_vMaxPos=vMaxPos;
}
Vertex3d& C3DGeometry::getMaxPos()
{
	return m_vMaxPos;
}

size_t C3DGeometry::sizeBuffer()
{
    size_t nSize = 0;
    //nSize += CGeometry::sizeBuffer(); 
    //nSize += Vertex3d::sizeBuffer()*2; 
    return nSize;
}