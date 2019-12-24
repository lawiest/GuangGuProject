//********************************************************************************** 
//** 文件名称：GeometrySet.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 几何集类定义
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-11
//********************************************************************************** 
#ifndef _GEOMETRYSET_H_
#define _GEOMETRYSET_H_
#pragma once
#include <memory>
#include <vector>
#include <fstream>
#include "Buffer.h"
#include "CT3D_Geometry.h"
#include "ModelDef.h"
#include "GeometrySetFile.h"

using namespace std;

class /*DLL_EXPORT*/ GeometrySet
{
public:
    GeometrySet(void);
    virtual ~GeometrySet(void);
    //********************************************************************************** 
    //** 函数名称： readBuff
    //** 功能描述： 从缓冲区中读数据
    //** 输    入： buffer:Buffer对象的引用           
    //** 输    出： 无
    //** 返回值：几何对象列表引用
    //** 作    者： 杨灵
    //** 创建日期： 2012-04-16
    //**********************************************************************************
    virtual GeometryList& readBuffer(Buffer &buffer);

    //********************************************************************************** 
    //** 函数名称： writeBuff
    //** 功能描述： 将数据写到缓冲区
    //** 输    入： buffer:Buffer对象的引用          
    //** 输    出： 无
    //** 返回值：无
    //** 作    者： 杨灵
    //** 创建日期： 2012-04-16
    //**********************************************************************************
    virtual void writeBuffer(Buffer &buffer);

    //**********************************************************************************
    //** 函数名称： setMaterialNum
    //** 功能描述： 设置几何体的个数
    //** 输    入： 文件中读取要素的个数          
    //** 输    出： 无
    //** 返回值：无
    //** 作    者： 杨灵
    //** 创建日期： 2012-04-16
    //**********************************************************************************
    void  setGeometryNum(UINT nMaterialNum);


    //********************************************************************************** 
    //** 函数名称： getMaterialNum
    //** 功能描述： 得到几何体的个数
    //** 输    入： 无          
    //** 输    出： 无
    //** 返回值：要素的个数
    //** 作    者： 杨灵
    //** 创建日期： 2012-04-16
    //*********************************************************************************
    UINT getGeometryNum() const;


public:
    GeometryList m_GeometryList;//几何体列表
private:
    UINT m_uGeometryNum;//几何体个数

public:
	void setGeometryList(GeometryList &v);
	GeometryList& getGeometryList();

};
#endif