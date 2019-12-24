//********************************************************************************** 
//** 文件名称：GeometrySet.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 几何集类实现
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-11
//********************************************************************************** 
#include "GeometrySet.h"

GeometrySet::GeometrySet(void)
{
}

GeometrySet::~GeometrySet(void)
{
}
//********************************************************************************** 
//** 函数名称： readBuff
//** 功能描述： 从缓冲区中读数据
//** 输    入： buffer:Buffer对象的引用           
//** 输    出： 无
//** 返回值：几何对象列表引用
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
GeometryList& GeometrySet::readBuffer(Buffer &buffer)
{
    GeometryItem item;

    for(UINT i=0;i< m_uGeometryNum;i++)
    {
        //读取几何ID
        buffer.read((char*)&item.uID,sizeof(UINT64));
        
        //读取几何类型
        buffer.read((char*)&item.byType,sizeof(byte));
       
        //读取几何数据长度
        buffer.read((char*)&item.uDataLen,sizeof(UINT));
        
        //根据几何类型创建相应的几何类
        item.pGeometry = CGeometry::CreateGeometry(item.byType);
        
        //从相应的缓冲区读取数据
        item.pGeometry->readBuffer(buffer);
        
        //读取创建时间
        buffer.read((char*)&item.uFeatureCreateTime,sizeof(UINT64));
        //读取更新时间
        buffer.read((char*)&item.uFeatureUpdateTime,sizeof(UINT64));
        m_GeometryList.push_back(item);
    }
     return m_GeometryList;
}


//********************************************************************************** 
//** 函数名称： writeBuff
//** 功能描述： 将数据写到缓冲区
//** 输    入： buffer:Buffer对象的引用          
//** 输    出： 无
//** 返回值：无
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
void GeometrySet::writeBuffer(Buffer &buffer)
{
    for(UINT i=0;i< m_uGeometryNum;i++)
    {
        //写入几何ID
        buffer.write((char*)&m_GeometryList[i].uID,sizeof(UINT64));
        //写入几何类型
        buffer.write((char*)&m_GeometryList[i].byType,sizeof(byte));

        //写入几何数据长度
        buffer.write((char*)&m_GeometryList[i].uDataLen,sizeof(UINT));

        //从相应的缓冲区写入数据
        m_GeometryList[i].pGeometry->writeBuffer(buffer);

        //写入创建时间
        buffer.write((char*)&m_GeometryList[i].uFeatureCreateTime,sizeof(UINT64));

        //写入更新时间
        buffer.write((char*)&m_GeometryList[i].uFeatureUpdateTime,sizeof(UINT64));
    }
}
//**********************************************************************************
//** 函数名称： setMaterialNum
//** 功能描述： 设置几何体的个数
//** 输    入： 文件中读取要素的个数          
//** 输    出： 无
//** 返回值：无
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
void  GeometrySet::setGeometryNum(UINT uMaterialNum)
{
    m_uGeometryNum = uMaterialNum;
}


//********************************************************************************** 
//** 函数名称： getMaterialNum
//** 功能描述： 得到几何体的个数
//** 输    入： 无          
//** 输    出： 无
//** 返回值：要素的个数
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
UINT GeometrySet::getGeometryNum() const
{
	return m_uGeometryNum;
}

void GeometrySet::setGeometryList(GeometryList &v)
{
	m_GeometryList=v;
}
GeometryList& GeometrySet::getGeometryList()
{
	return m_GeometryList;
}
