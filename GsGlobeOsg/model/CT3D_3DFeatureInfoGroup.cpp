//********************************************************************************** 
//** 文件名称：CT3D_3DFeatureInfoGroup.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维要素组合类实现
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-15
//********************************************************************************** 
#include "CT3D_3DFeatureInfoGroup.h"
#include "Buffer.h"



//********************************************************************************** 
//** 函数名称： CreateObject
//** 功能描述： 开辟一个当前类对象所需的内存空间
//** 输    入： 无            
//** 输    出： 无
//** 返回值：	开辟的当前类对象内存空间的首地址
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
CGeometry* C3DFeatureInfoGroup::CreateObject()
{
	return new C3DFeatureInfoGroup;
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
bool C3DFeatureInfoGroup::readBuffer( Buffer & buf)
{
    bool bRet = C3DGeometry::readBuffer(buf);
	if(!bRet)
	{
		return false;
	}
	int nCount = 0;
	buf.read((char*)&nCount,sizeof(int));

	FEATURE_INFO fi;
	m_vFeatureInfos.reserve(nCount);
	for(int i=0; i<nCount; i++)
	{
        buf.read((char*)&fi,sizeof(UID64)+ Envelope3D::sizeBuffer());
		
		m_vFeatureInfos.push_back(fi);
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
void C3DFeatureInfoGroup::writeBuffer(Buffer& buf)
{
    C3DGeometry::writeBuffer(buf);

	int nCount = m_vFeatureInfos.size();
	buf.write((char*)&nCount,sizeof(int));

	for(int i=0; i<nCount; i++)
	{
        buf.write(&(m_vFeatureInfos[i]), sizeof(UID64)+Envelope3D::sizeBuffer());
	}

}

void C3DFeatureInfoGroup::setFeatureinfoNum(int nFeatureinfoNum)
{
	m_nFeatureinfoNum=nFeatureinfoNum;
}
int C3DFeatureInfoGroup::getFeatureinfoNum()
{
	return m_nFeatureinfoNum;
}
size_t C3DFeatureInfoGroup::sizeBuffer()
{
	size_t nSize = 0;
	nSize+=C3DGeometry::sizeBuffer();
	nSize+=sizeof(int);//几何对象个数
	nSize+=getFeatureinfoNum()*(sizeof(UID64)+Envelope3D::sizeBuffer());
	return nSize;
}
