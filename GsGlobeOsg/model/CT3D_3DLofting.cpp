#include "CT3D_3DLofting.h"

//********************************************************************************** 
//** 函数名称： C3DLofting
//** 功能描述： 构造函数
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
C3DLofting::C3DLofting(void)
{

}

//********************************************************************************** 
//** 函数名称： ~C3DLofting
//** 功能描述： 析构函数
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
C3DLofting::~C3DLofting(void)
{
}

//********************************************************************************** 
//** 函数名称： readBuffer
//** 功能描述： 从缓冲区中解析当前对象，并给成员变量赋值
//** 输    入： buf	缓冲区Buffer对象的引用           
//** 输    出： 无
//** 返回值：	成功为true，失败为false
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
bool C3DLofting::readBuffer( Buffer & buf)
{
    bool bRet = C3DLineString::readBuffer(buf);
	//modify by yangling 20131208 begin
	if(!bRet)
	{
		return false;
	}
	//modify by yangling 20131208 end
    buf.read((char*)&m_MatID,sizeof(UID64));
    buf.read((char*)&m_SectionType,sizeof(byte));
    if( 0 ==m_SectionType )
    {
         buf.read((char*)&(m_SectionData.data.RTube.dDiameter),sizeof(double));
    }
    else if( 1 == m_SectionType )
    {
        buf.read((char*)&m_SectionData.data.SqTube.dSectionLength,sizeof(double));
        buf.read((char*)&m_SectionData.data.SqTube.dSectionWidth,sizeof(double));

    }
    else if (2 == m_SectionType)
    {
        //读取对象大小
        buf.read((char*)&m_SectionData.data.SpTube.uGeometrySize,sizeof(unsigned int));
        byte byGeometryType = 1;
        //跳过对象ID
        char* p = (char*)buf.data() + buf.getPos()+sizeof(UID64);
        //读取对象类型
        memcpy((char*)&byGeometryType,p,sizeof(byte));
        //创建对象       
        m_SectionData.data.SpTube.pGeometry  = Geometry::CreateGeometry(byGeometryType);
        //调用对象的读方法
        if(m_SectionData.data.SpTube.pGeometry != NULL)
        {
            m_SectionData.data.SpTube.pGeometry->readBuffer(buf);
        }
        p = NULL;
    }
	else
	{
		return false;
	}
	return true;
}

//********************************************************************************** 
//** 函数名称： writeBuffer
//** 功能描述： 把当前对象的成员变量数据，打包成一个缓冲区
//** 输    入： buf	缓冲区Buffer对象的引用           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
void C3DLofting::writeBuffer(Buffer& buf)
{
    C3DLineString::writeBuffer(buf);
    buf.write((char*)&m_MatID,sizeof(UID64));
    buf.write((char*)&m_SectionType,sizeof(byte));
    if( 0 ==m_SectionType )
    {
        buf.write((char*)&m_SectionData.data.RTube.dDiameter,sizeof(double));
    }
    else if( 1 == m_SectionType )
    {
        buf.write((char*)&m_SectionData.data.SqTube.dSectionLength,sizeof(double));
        buf.write((char*)&m_SectionData.data.SqTube.dSectionWidth,sizeof(double));

    }
    else if (2 == m_SectionType)
    {
        //存储对象大小
        buf.write((char*)&m_SectionData.data.SpTube.uGeometrySize,sizeof(unsigned int));
        //调用对象的写方法
        if(m_SectionData.data.SpTube.pGeometry != NULL)
        {
            m_SectionData.data.SpTube.pGeometry->writeBuffer(buf);
        }
    }
    else
    {

    }
}

void C3DLofting::setMatID(UID64 uMatID)
{
	m_MatID=uMatID;
}
UID64 C3DLofting::getMatID()
{
	return m_MatID;
}

void C3DLofting::setSectionType(byte bySectionType)
{
	m_SectionType=bySectionType;
}
byte C3DLofting::getSectionType()
{
	return m_SectionType;
}

void C3DLofting::setSectionData(SectionData sData)
{
	if(0==m_SectionType)
	{
		m_SectionData.data.RTube.dDiameter=sData.data.RTube.dDiameter;
	}
	if (1==m_SectionType)
	{
		m_SectionData.data.SqTube.dSectionLength=sData.data.SqTube.dSectionLength;
		m_SectionData.data.SqTube.dSectionWidth=sData.data.SqTube.dSectionWidth;
	}
	if(2==m_SectionType)
	{
		m_SectionData.data.SpTube.uGeometrySize=sData.data.SpTube.uGeometrySize;
		m_SectionData.data.SpTube.pGeometry=sData.data.SpTube.pGeometry;
	}
	
}
SectionData C3DLofting::getSectionData()
{
	
		return m_SectionData;
	
}

size_t C3DLofting::sizeBuffer()
{
	size_t nSize = 0;
	nSize += C3DLineString::sizeBuffer();
	nSize += sizeof(UINT64);//材质对象ID
	nSize += sizeof(byte);//截面类型

	if (0==getSectionType())
	{
		nSize += sizeof(double);//管直径
	}
	if (1==getSectionType())
	{
		nSize += sizeof(double);//横截面长度
		nSize += sizeof(double);//横截面宽度
		
	}
	if (2==getSectionType())
	{
		nSize += sizeof(UINT);//横截面几何对象大小
		nSize += getSectionData().data.SpTube.pGeometry->sizeBuffer();
	}
	return nSize;
}
