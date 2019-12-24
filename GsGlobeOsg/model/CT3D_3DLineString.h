//********************************************************************************** 
//** 文件名称：CT3D_C3DLineString.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维折线类的定义
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-11
//********************************************************************************** 

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _CT3D_3DLINESTRING_H_
#define _CT3D_3DLINESTRING_H_

#include "ModelDef.h"
#include "CT3D_3DCurve.h"
#include "CT3D_Vertex3d.h"
#include "CT3D_Vertex3f.h"
#include "Buffer.h"


class /*DLL_EXPORT*/ C3DLineString 
: public C3DCurve
{

public:

    /** 线宽 */
    double m_dLineWidth;

    /** 顶点类型编码 */
    byte m_byVertexType;

    /** 顶点数 */
    long m_lVertNum;

	/** 顶点序列，以vector装载 */
	void *m_vVertices;
public:

	//********************************************************************************** 
	//** 函数名称： C3DLineString
	//** 功能描述： 构造函数1
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-11
	//**********************************************************************************	
	C3DLineString();
	

	//********************************************************************************** 
	//** 函数名称： C3DLineString
	//** 功能描述： 构造函数2
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	C3DLineString(int nVertNum, void* const vVertices, unsigned char vertexType, double dLineWidth);
	
	
	//********************************************************************************** 
	//** 函数名称： ~C3DLineString
	//** 功能描述： 析构函数
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	virtual ~C3DLineString();
	
	//********************************************************************************** 
	//** 函数名称： readBuffer
	//** 功能描述： 将数据读入buf
	//** 输    入： Buffer对象buf的引用           
	//** 输    出： 无
	//** 返回值：	无
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

	void setLineWidth(double dLineWidth);
	double getLineWidth();

	void setVertexType(byte byVertexType);
	byte getVertexType();


	void setVertNum(long lVertNum);
	long getVertNum();

	void SetVertices(void* const vVertices)
	{
		m_vVertices = vVertices;
	}

	void GetVertices(void*& vVertices) const
	{
		vVertices = m_vVertices;
	}

	virtual size_t sizeBuffer();
};

#endif 
