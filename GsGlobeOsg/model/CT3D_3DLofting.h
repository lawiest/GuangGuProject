//********************************************************************************** 
//** 文件名称：CT3D_3DLofting.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维放样对象类的定义
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-15
//********************************************************************************** 
#pragma once
#include "CT3D_3DLineString.h"
#include "ModelDef.h"

typedef struct _tagRoundTube
{
    double dDiameter;//直径
}RoundTube;//圆管


typedef struct _tagSquareTube
{
    double dSectionLength;//横截面长度
    double dSectionWidth;//横截面宽度
}SquareTube;//方管


typedef struct _tagSpecialTube
{
    unsigned int uGeometrySize;//横截面几何对象大小
    CGeometry* pGeometry;//横截面几何对象
}SpecialTube;//特殊形状管
typedef struct _tagSectionData
{
    union
    {
        RoundTube RTube;
        SquareTube SqTube;
        SpecialTube SpTube; 
    
    }data;
}SectionData;

class /*DLL_EXPORT*/ C3DLofting :
    public C3DLineString
{
protected:
    UID64 m_MatID;//材质ID
   //截面类型 
    //0: 圆管
    //1:方管
    //2:特殊形状管
    byte m_SectionType;

    //截面数据
    SectionData m_SectionData;

public:
	//********************************************************************************** 
	//** 函数名称： C3DLofting
	//** 功能描述： 构造函数
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
    C3DLofting(void);
public:
	//********************************************************************************** 
	//** 函数名称： ~C3DLofting
	//** 功能描述： 析构函数
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
    virtual ~C3DLofting(void);
   
	//********************************************************************************** 
	//** 函数名称： readBuffer
	//** 功能描述： 从缓冲区中解析当前对象，并给成员变量赋值
	//** 输    入： buf	缓冲区Buffer对象的引用           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
    bool readBuffer( Buffer & buf);

	//********************************************************************************** 
	//** 函数名称： writeBuffer
	//** 功能描述： 把当前对象的成员变量数据，打包成一个缓冲区
	//** 输    入： buf	缓冲区Buffer对象的引用           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
    void writeBuffer(Buffer& buf);

	void setMatID(UID64 uMatID);
	UID64 getMatID();

	void setSectionType(byte bySectionType);
	byte getSectionType();

	void setSectionData(SectionData sData);
	SectionData getSectionData();

	virtual size_t sizeBuffer();
};
