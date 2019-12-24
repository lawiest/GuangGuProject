//********************************************************************************** 
//** 文件名称：SharedModel.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 共享模型数据实现
//** 创 建 人： 李静
//** 创建日期： 2012-04-11
//********************************************************************************** 

#include "SharedModel.h"


//********************************************************************************** 
//** 函数名称： CSharedModel
//** 功能描述： 构造函数
//** 输    入： 无          
//** 输    出： 无
//** 返回值：无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
CSharedModel::CSharedModel(void)
{
	m_nModelObjectID=0;
	m_nSDMLen=-1;
    m_pGeometry = NULL;
}

//********************************************************************************** 
//** 函数名称： ~CSharedModel
//** 功能描述： 析构函数
//** 输    入： 无          
//** 输    出： 无
//** 返回值：无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
CSharedModel::~CSharedModel(void)
{
    if(NULL!=m_pGeometry)
	{
		delete m_pGeometry;
	}
}

//********************************************************************************** 
//** 函数名称： CFeatureClass
//** 功能描述： 拷贝构造函数
//** 输    入： sharedmodel的引用        
//** 输    出： 无
//** 返回值：无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
CSharedModel::CSharedModel(const CSharedModel& sharedmodel)
{
	copy(sharedmodel);
}

//********************************************************************************** 
//** 函数名称： read
//** 功能描述： 读取文件数据
//** 输    入： 流对象f的引用           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************

//********************************************************************************** 
//** 函数名称： copy
//** 功能描述： 拷贝传入的要素对象的各项属性
//** 输    入： sharedmodel:共享模型对象的引用          
//** 输    出： 无
//** 返回值：无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
void CSharedModel::copy(const CSharedModel& sharedmodel)
{
	m_nModelObjectID=sharedmodel.m_nModelObjectID;
	m_nSDMLen=sharedmodel.m_nSDMLen;
	m_pGeometry = sharedmodel.m_pGeometry;
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
bool CSharedModel::readBuffer(Buffer &buffer)
{

	//读取模型对象ID
	buffer.read((void*)&m_nModelObjectID,sizeof(UINT64));
	//读取共享模型数据长度
	buffer.read((void*)&m_nSDMLen,sizeof(int));
	//读取共享模型对象的数据
	buffer.setPos(buffer.getPos() +sizeof(UINT64)/*+sizeof(UINT64)+sizeof(int)*/);

	byte nType=0;
	buffer.read((void*)&nType,sizeof(byte));
	buffer.setPos(buffer.getPos() - 9);
	if (nType!=255)
	{
		
		m_pGeometry=CGeometry::CreateGeometry(nType);
		
		//buffer.setPos(buffer.getPos() + sizeof(UINT64)+sizeof(int));


		m_pGeometry->readBuffer(buffer);
	}
	else
	{
		return false;
	}
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
void CSharedModel::writeBuffer(Buffer &buffer)
{
	//写入模型对象ID
	buffer.write((void*)&m_nModelObjectID,sizeof(UINT64));
	//写入共享模型数据长度
	buffer.write((void*)&m_nSDMLen,sizeof(int));
	//写入共享模型对象的数据
	int nType=m_pGeometry->getTypeID();
	
	if (nType!=255)
	{
		m_pGeometry->writeBuffer(buffer);
	}
}
void CSharedModel::setModelObjectID(UINT64 nModelObjectID)
{
	m_nModelObjectID=nModelObjectID;
}
UINT64 CSharedModel::getModelObjectID()
{
	return m_nModelObjectID;
}


void CSharedModel::setSDMLen(int nLen)
{
	m_nSDMLen=nLen;
}
int CSharedModel::getSDMLen()
{
	return m_nSDMLen;
}

void CSharedModel::setObjectData(CGeometry* pGeometry)
{
	m_pGeometry=pGeometry;
}
CGeometry* CSharedModel::getObjectData()
{
	return m_pGeometry;
}
//********************************************************************************** 
//** 函数名称： sizeBuffer
//** 功能描述： 得到共享模型大小
//** 输    入： 无          
//** 输    出： 无
//** 返回值：共享模型大小
//** 作    者： 杨灵
//** 创建日期： 2012-04-11
//**********************************************************************************
size_t CSharedModel::sizeBuffer()
{
    size_t sSDMSize = 0;
    if(m_pGeometry != NULL)
    {
        sSDMSize = sizeof(UINT64) + sizeof(int) + m_pGeometry->sizeBuffer();
    }
    else
    {
        sSDMSize = sizeof(UINT64) + sizeof(int);
    }
    return sSDMSize;

}


void CSharedModel::readlodBuffer(Buffer &buffer)
{
	//读取lodInfo的个数
	byte nLodCount = 0;
	buffer.read((char *)&nLodCount,sizeof(byte));
	for(byte nLodIndex = 0;nLodIndex<nLodCount;nLodIndex++)
	{
		LodInfo lodData;
		lodData.readBuffer(buffer);
		m_vecLodInfo.push_back(lodData);
	} 	
}

void CSharedModel::writeLodBuffer(Buffer &buffer)
{

	//写入lodInfo的个数
	byte nLodCount = m_vecLodInfo.size();
	buffer.write((char *)&nLodCount,sizeof(byte));
	for(byte nLodIndex = 0;nLodIndex<nLodCount;nLodIndex++)
	{
		LodInfo lodData;
		m_vecLodInfo[nLodIndex].writeBuffer(buffer);
	} 
}

size_t CSharedModel::sizeLodBuffer()
{
	size_t nSLodSize = 0;
	nSLodSize += sizeof(byte);
	for(byte nLodIndex = 0;nLodIndex < m_vecLodInfo.size();nLodIndex ++)
	{
		nSLodSize += m_vecLodInfo[nLodIndex].sizeBuffer();
	}
	return nSLodSize;
}

byte CSharedModel::GetLodCount()
{
	return m_vecLodInfo.size();
}

void CSharedModel::ReleaseLodMTData()
{
	for (byte nLodIndex = 0;nLodIndex<m_vecLodInfo.size();nLodIndex++)
	{
		int nMTCount =m_vecLodInfo[nLodIndex].vecMTData.size();
		for(int nMTIndex = 0;nMTIndex<nMTCount;nMTIndex++)
		{
			MTData tempData =  m_vecLodInfo[nLodIndex].vecMTData[nMTIndex];
			if (tempData.pMTData)
			{
				delete []tempData.pMTData;
				tempData.pMTData = 0;
			}
		}
	}
}
