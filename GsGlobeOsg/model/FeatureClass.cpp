//********************************************************************************** 
//** 文件名称：FeatureClass.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 要素类实现
//** 创 建 人： 李静
//** 创建日期： 2012-04-11
//********************************************************************************** 


#include "FeatureClass.h"
//********************************************************************************** 
//** 函数名称： CFeatureClass
//** 功能描述： 构造函数
//** 输    入： 无          
//** 输    出： 无
//** 返回值：无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
CFeatureClass::CFeatureClass(void)
{
	m_nFCLid=0;
	m_nFCLNameLen=-1;
	m_strFCLName="";
	m_nStatus=-1;
	m_nProNameLen=-1;
	m_strProName="";

}

//********************************************************************************** 
//** 函数名称： ~CFeatureClass
//** 功能描述： 析构函数
//** 输    入： 无          
//** 输    出： 无
//** 返回值：无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
CFeatureClass::~CFeatureClass(void)
{
}

//********************************************************************************** 
//** 函数名称： CFeatureClass
//** 功能描述： 拷贝构造函数
//** 输    入： featureclass的引用        
//** 输    出： 无
//** 返回值：无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
CFeatureClass::CFeatureClass(CFeatureClass& featureclass)
{
	copy(featureclass);
}

//********************************************************************************** 
//** 函数名称： copy
//** 功能描述： 拷贝传入的要素对象的各项属性
//** 输    入： featureclass:要素对象的引用          
//** 输    出： 无
//** 返回值：无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
void  CFeatureClass::copy(const CFeatureClass& featureclass)
{
	//拷贝传入的要素的各项属性
	m_nFCLid=featureclass.m_nFCLid;
	m_nFCLNameLen=featureclass.m_nFCLNameLen;
	m_strFCLName=featureclass.m_strFCLName;
	m_nStatus=featureclass.m_nStatus;
	m_nProNameLen=featureclass.m_nProNameLen;
	m_strProName=featureclass.m_strProName;
}

//********************************************************************************** 
//** 函数名称： readBuffer
//** 功能描述： 从缓冲区中读数据
//** 输    入： buffer:Buffer对象的引用           
//** 输    出： 无
//** 返回值：成功为true，失败为false
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
bool CFeatureClass::readBuffer(Buffer &buffer)
{
	/**读取要素类ID*/
	buffer.read((void *)&m_nFCLid,sizeof(int64));
	/**读取要素类名称长度*/
	buffer.read((void*)&m_nFCLNameLen,sizeof(int));
	char ca[4096];
    memset(ca,0,4096);
	/**读取要素类名称*/
	buffer.read((void*)ca,m_nFCLNameLen);
	//modify by yangling 20131208 begin
	if(m_nFCLNameLen > 4096)
	{
		return false;
	}
	//modify by yangling 20131208 end
	m_strFCLName.assign(ca);
	/**读取要素类状态*/
	buffer.read((void*)&m_nStatus,sizeof(int));
	/**读取属性表的名称长度*/
	buffer.read((void*)&m_nProNameLen,sizeof(int));
	//再次初始化临时字符串
	memset(ca,0,4096);
	//modify by yangling 20131208 begin
	if(m_nProNameLen > 4096)
	{
		return false;
	}
	//modify by yangling 20131208 end
	/**读取属性表的名称*/
	buffer.read((void*)ca,m_nProNameLen);
	m_strProName.assign(ca);
	return true;
}

//********************************************************************************** 
//** 函数名称： writeBuffer
//** 功能描述： 将数据写到缓冲区
//** 输    入： buffer:Buffer对象的引用          
//** 输    出： 无
//** 返回值：无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
void CFeatureClass::writeBuffer(Buffer &buffer)
{
	/**写入要素类ID*/
	buffer.write((void *)&m_nFCLid,sizeof(int64));
	/**写入要素类名称长度*/
	buffer.write((void*)&m_nFCLNameLen,sizeof(int));
	
	/**写入要素类名称*/
	buffer.write((void*)m_strFCLName.c_str(),m_nFCLNameLen);
	
	/**写入要素类状态*/
	buffer.write((void*)&m_nStatus,sizeof(int));
	/**写入属性表的名称长度*/
	buffer.write((void*)&m_nProNameLen,sizeof(int));
	
	/**写入属性表的名称*/
	buffer.write((void*)m_strProName.c_str(),m_nProNameLen);
	
}
void CFeatureClass::setFCLID(UINT64 nFCLID)
{
	m_nFCLid=nFCLID;
}
UINT64 CFeatureClass::getFCLID()
{
	return m_nFCLid;
}

void CFeatureClass::setFCLName(string strFCLName)
{
	m_strFCLName=strFCLName;
}
string CFeatureClass::getFCLName()
{
	return m_strFCLName;
}
int CFeatureClass::getFCLNameLen()
{
	return m_strFCLName.size();
}

void CFeatureClass::setStatus(int nStatus)
{
	m_nStatus=nStatus;
}
int CFeatureClass::getStatus()
{
	return m_nStatus;
}

void CFeatureClass::setPropName(string strPropName)
{
	m_strProName=strPropName;
}
string CFeatureClass::getPropName()
{
	return m_strProName;
}
int CFeatureClass::getPropNameLen()
{
	return m_strProName.size();
}
//********************************************************************************** 
//** 函数名称： sizeBuffer
//** 功能描述： 得到要素类大小
//** 输    入： 无          
//** 输    出： 无
//** 返回值：要素类大小
//** 作    者： 杨灵
//** 创建日期： 2012-04-11
//**********************************************************************************
size_t CFeatureClass::sizeBuffer()
{
    size_t sFeatureClassSize = 0;
    sFeatureClassSize = sizeof(UINT64) + sizeof(int) + m_nFCLNameLen + sizeof(int) + sizeof(int) + m_nProNameLen;
    return sFeatureClassSize;
}