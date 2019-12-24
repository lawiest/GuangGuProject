//********************************************************************************** 
//** 文件名称：CT3D_3DGroup.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 几何组合体类的实现
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-14
//********************************************************************************** 
#include "CT3D_3DGroup.h"
#include "Buffer.h"

//********************************************************************************** 
//** 函数名称： CreateObject
//** 功能描述： 开辟一个当前类对象所需的内存空间
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
CGeometry* C3DGroup::CreateObject()
{
    return new C3DGroup;
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
bool C3DGroup::readBuffer( Buffer & buf)
{
    bool bRet = C3DGeometry::readBuffer(buf);
    if(!bRet)
	{
		return false;
	}
    //指针数组包含的对象信息**************

    buf.read((char*)&m_nGeometryNum,sizeof(int));//组中对象个数

    CGeometry* pGeometry = NULL;
    byte byType = -1;
    m_vGeoCollection.reserve(m_nGeometryNum);
    for(int i=0; i<m_nGeometryNum; i++)
    {
    	
    	unsigned int  nSize = 0;
    	buf.read((char*)&nSize,sizeof(unsigned int));//下面对象的大小
    		
    	char* p = (char*)buf.data() + buf.getPos()+sizeof(UID64);//跳过对象ID
    	memcpy((char*)&byType,p,sizeof(byte));//获得对象的 类型 以此判别是哪一类对象
    	pGeometry = CGeometry::CreateGeometry(byType);

		//如果对象创建失败
		if (NULL == pGeometry)
			continue;

		//读取对象数据；如果读取失败，返回False
    	if (!pGeometry->readBuffer(buf))
		{
			SafeDelete(pGeometry);
			return false;
		}
    	C3DGeometry* p3DGeometry = (C3DGeometry*) pGeometry;
        _GeometryPair geopair(nSize,p3DGeometry);
    	m_vGeoCollection.push_back(geopair);//把3DGeometry的指针添加到组对几何对象的存储结构中
        p = NULL;
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
void C3DGroup::writeBuffer(Buffer& buf)
{	
    C3DGeometry::writeBuffer(buf);

    //指针数组包含的对象信息**************

    int nCount = 0;
    buf.write((char*)&m_nGeometryNum,sizeof(int));//组中对象个数
    byte byType = -1;
    for(int i=0; i<m_nGeometryNum; i++)
    {

        buf.write((char*)& m_vGeoCollection[i].first,sizeof(unsigned int));//下面对象的大小
        m_vGeoCollection[i].second->writeBuffer(buf);
       
    }
}

size_t C3DGroup::sizeBuffer()
{
	size_t nSize = 0;

	nSize += C3DGeometry::sizeBuffer();
	nSize += sizeof(int);

	for (int i=0;i<getGeometryNum();i++)
	{
		nSize += sizeof(UINT);
		nSize += m_vGeoCollection[i].second->sizeBuffer();
	}
	return nSize;
}
