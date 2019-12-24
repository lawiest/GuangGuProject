//********************************************************************************** 
//** 文件名称：CT3D_3DPolygon.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维多边形类定义
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-11
//********************************************************************************** 

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#pragma warning(disable:4251)
#endif
#ifndef _CT3D_3DPOLYGON_H_
#define _CT3D_3DPOLYGON_H_

#include "ModelDef.h"
#include "CT3D_3DSurface.h"
#include "CT3D_3DLinearRing.h"
#include "CT3D_3DTriangleMesh.h"
#include "Buffer.h"
#include <vector>
#include <utility>

using namespace std;


typedef vector<C3DLinearRing*>_3DLinearRingList;

class /*DLL_EXPORT*/ C3DPolygon 
	: public C3DSurface
{

private:
 
    /** 线性环列表*/
    _3DLinearRingList m_vecRingList;

public:

	//********************************************************************************** 
	//** 函数名称： C3DPolygon
	//** 功能描述： 构造函数
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	C3DPolygon();

	//********************************************************************************** 
	//** 函数名称： ~C3DPolygon
	//** 功能描述： 析构函数
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	virtual ~C3DPolygon();
	
	//********************************************************************************** 
	//** 函数名称： readBuffer
	//** 功能描述： 将数据读入buf
	//** 输    入： Buffer对象buf的引用           
	//** 输    出： 无
	//** 返回值：	成功为true，失败为false
	//** 作    者： 李静
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	bool readBuffer( Buffer & buf);

	//********************************************************************************** 
	//** 函数名称： writeBuffer
	//** 功能描述： 将数据写入buf
	//** 输    入： Buffer对象buf的引用           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 李静
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	void writeBuffer(Buffer& buf);

	
	int getLinearRingNum();

	void setLinearRingList(const _3DLinearRingList& vRingPair);
	_3DLinearRingList& getLinearRingList();

	virtual size_t sizeBuffer();
};

#endif 
