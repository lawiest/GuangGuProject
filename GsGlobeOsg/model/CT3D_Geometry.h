// Copyright (C) 1991 - 1999 Rational Software Corporation


#ifndef _CT3D_GEOMETRY_H_
#define _CT3D_GEOMETRY_H_ 1
#include "Model/ModelDef.h"
#include <iostream>
#include <fstream>
#include <string>
#include "Model/CT3D_Vertex3d.h"
#include "Model/Buffer.h"
using namespace std;

	enum
	{
		GEOTYPE_3D_GEOMETRY = 0,
		GEOTYPE_3D_POINT = 1,
		GEOTYPE_3D_SURFACE =3,
		GEOTYPE_3D_CURVE = 4,
        GEOTYPE_3D_GROUP = 5,
		GEOTYPE_3D_SOLID = 7,
		GEOTYPE_3D_TRIANGLEMESH = 12,
		GEOTYPE_3D_POLYGON = 15,
		GEOTYPE_3D_LINEARRING = 16,
		GEOTYPE_3D_LINESTRING = 18,
		GEOTYPE_3D_REFERENCE = 20,
		GEOTYPE_3D_SINGLEPOINT = 26,
		GEOTYPE_3D_MULTIPOINT = 27,
		GEOTYPE_3D_FEATUREINFOGROUP = 29,
		GEOTYPE_3D_LOFTING = 35,
        //新增类型请从40开始
		GEOTYPE_3D_MODELOBJECT = 41,   
		GEOTYPE_3D_OBLIQUE=42
        

	};

class /*DLL_EXPORT*/ CGeometry
{

protected:

	/** 要素ID*/
	UID64 m_uFID;

	/** 几何对象的类型，每个类型对应一个编号 */
	byte m_byTypeID;

	/** 几何对象的名字长度*/
	int m_nObjNameLen;

	/** 几何对象的名字*/
	string m_strObjName;

public:
	//********************************************************************************** 
	//** 函数名称： CGeometry
	//** 功能描述： 构造函数1
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	CGeometry();

	//********************************************************************************** 
	//** 函数名称： C3DGeometry
	//** 功能描述： 构造函数2
	//** 输    入： uFID:要素ID
	//**            byTypeID:几何类型ID
	//**            strObjName: 几何对象名称
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-11
	//************************************************************************************
    CGeometry(UID64 uFID,byte byTypeID, string strObjName);

	//********************************************************************************** 
	//** 函数名称： ~CGeometry
	//** 功能描述： 析构函数
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	virtual ~CGeometry();	
	
public:
	//********************************************************************************** 
	//** 函数名称： readBuffer
	//** 功能描述： 从缓存读取对象记录
	//** 输    入： pBuffer 缓存           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
    virtual bool readBuffer( Buffer & buf) ;

	//********************************************************************************** 
	//** 函数名称： readBuffer
	//** 功能描述： 将对象写入缓存
	//** 输    入： pBuffer 缓存           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
    virtual void writeBuffer(Buffer& buf) ;
	
	//********************************************************************************** 
	//** 函数名称： CreateGeometry
	//** 功能描述： 开辟一个当前类对象所需的内存空间
	//** 输    入： 几何对象的类型           
	//** 输    出： 无
	//** 返回值：	开辟的当前类对象内存空间的首地址
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
    static CGeometry * CreateGeometry(byte byType); 
public:
	virtual void setObjID(UID64 uFID);
	virtual UID64 getObjID();

	virtual void setTypeID(byte byTypeID);
	virtual byte getTypeID();

	virtual void setObjName(string strObjName);
	virtual string getObjName();
	virtual int getObjNameLen();

    virtual size_t sizeBuffer();
};

typedef CGeometry Geometry;

//********************************************************************************** 
//** 函数名称： CreateGeometry
//** 功能描述： 根据顶点类型删除顶点数组
//** 输    入： 顶点类型 iVertexType；顶点数组的指针引用pVertexPointer          
//** 输    出： 无
//** 返回值：	0代表删除成功；-1代表不成功
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
/*DLL_EXPORT*/ long ReleaseVertexArray(unsigned char iVertexType, void*& pVertexPointer);



#endif /* _INC_CGEOMETRY_49E53136004E_INCLUDED */
