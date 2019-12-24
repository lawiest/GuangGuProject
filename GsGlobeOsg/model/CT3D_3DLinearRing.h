//********************************************************************************** 
//** 文件名称：CT3D_3DLinearRing.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 线环类定义
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-11
//********************************************************************************** 

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _CT3D_3DLINEARRING_H_
#define _CT3D_3DLINEARRING_H_

#include "ModelDef.h"

#include "CT3D_3DGeometry.h"

class  C3DLinearRing : public C3DGeometry
{

protected:

    /** 顶点类型编码 */
    byte m_byVertexType;

    /** 线环顶点数 */
    int m_nVertNum;

	/** 线环结点序列，以vector装载，直接保存顶点对象，顶点依序存入容器 */
	void *m_vVertices;

	/** 内环还是外环的标志 0：外环 1：内环*/
	byte m_bInOrOutRing;
	

public:

    

	//********************************************************************************** 
	//** 函数名称： C3DLinearRing
	//** 功能描述： 构造函数1
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	C3DLinearRing();
    
	//********************************************************************************** 
	//** 函数名称： C3DLinearRing
	//** 功能描述： 构造函数2
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-11
	//**********************************************************************************
    C3DLinearRing(int nVertexNum, Vertex3d* const vVertices,int bInOrOutRing = 0);	
   
	
	
	//********************************************************************************** 
	//** 函数名称： ~C3DPolygon
	//** 功能描述： 析构函数
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	virtual ~C3DLinearRing();
	
	//********************************************************************************** 
	//** 函数名称： readBuffer
	//** 功能描述： 将数据读入buf
	//** 输    入： Buffer对象buf的引用           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 李静
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	virtual bool readBuffer( Buffer & buf);
	
	//********************************************************************************** 
	//** 函数名称： writeBuffer
	//** 功能描述： 将数据写入buf
	//** 输    入： Buffer对象buf的引用           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 李静
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	virtual void writeBuffer(Buffer& buf);

public:
	void setVertexType(byte byVertexType);
	byte getVertexType();

	void setVertNum(int VertNum);
	int getVertNum();

	void setInOrOutRing(byte bInOrOutRing);
	byte getInOrOutRing();

	void SetVertices( void* const vVertices)
	{
		m_vVertices = vVertices;
	}
	void GetVertices(void*& vVertices) const
	{
		vVertices = m_vVertices;
	}
    virtual size_t sizeBuffer();


};

#endif /* _INC_C3DLINEARRING_49E34728009C_INCLUDED */
