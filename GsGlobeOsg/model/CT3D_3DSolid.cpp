//********************************************************************************** 
//** 文件名称：CT3D_C3DSolid.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维体类实现
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-11
//********************************************************************************** 


#include "CT3D_3DSolid.h"
#include "Buffer.h"

//********************************************************************************** 
//** 函数名称： C3DSolid
//** 功能描述： 构造函数
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-11
//**********************************************************************************
 C3DSolid::C3DSolid()
{
    m_byTypeID = GEOTYPE_3D_SOLID;
}



 //********************************************************************************** 
 //** 函数名称： ~C3DSolid
 //** 功能描述： 析构函数
 //** 输    入： 无           
 //** 输    出： 无
 //** 返回值：	无
 //** 作    者： 杨灵
 //** 创建日期： 2012-04-11
 //**********************************************************************************
 C3DSolid::~C3DSolid()
{
	//return;
    int nSize = m_3DSurfaceList.size();
    for(int i = 0; i< nSize; i++)
    {
        if(m_3DSurfaceList[i].second!= NULL)
        {
            delete m_3DSurfaceList[i].second;
            m_3DSurfaceList[i].second = NULL;
        }
    }
    m_3DSurfaceList.clear();
}

//********************************************************************************** 
//** 函数名称： readBuffer
//** 功能描述： 将数据读入buf
//** 输    入： Buffer对象buf的引用           
//** 输    出： 无
//** 返回值：	成功为true，失败为false
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
bool C3DSolid::readBuffer( Buffer & buf)
{
	
	bool bRet =C3DGeometry::readBuffer(buf);

	
	if(!bRet)
		return false;
	
	//读取面个数
    int nSurfaceNum = 0;
	buf.read((void*)&nSurfaceNum,sizeof(int));
    byte byType = 0;//实际的面类型
	//读取面列表
	for(int i=0;i< nSurfaceNum;i++)
	{
		UINT nSize=0;
        buf.read((void*)&nSize,sizeof(UINT));//对象的大小
		
        //得到Surface的面类型
        memcpy(&byType, (byte*)buf.getCurPtr() + sizeof(UINT64),sizeof(byte));

		//如果类型错误
		if (255 == byType)
			continue;

        //创建面对象
        C3DSurface* pSurface = (C3DSurface*)CreateGeometry(byType);
		if (NULL == pSurface)
			/*return false*/
			continue;
		//if (buf.size()!=303401)
		//{

		//if (buf.size()==303401)
		//{
		//	//int a=0;
		//	//return true;
		//	return false;
		//}

		if (!pSurface->readBuffer(buf))
		{
			SafeDelete(pSurface);
			return false;
		}
		
		//if (buf.size()==303401)
		//{
		//	//int a=0;
		//	//return true;
		//	return false;
		//}


		_3DSurfacePair surfacepair(nSize,pSurface);

		m_3DSurfaceList.push_back(surfacepair);
		/*}*/
	//	return true;
	}
	return true;
}

//********************************************************************************** 
//** 函数名称： writeBuffer
//** 功能描述： 将数据写入buf
//** 输    入： Buffer对象buf的引用           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
void C3DSolid::writeBuffer(Buffer& buf)
{
	C3DGeometry::writeBuffer(buf);
	//写入面个数
    int nSurfaceNum = m_3DSurfaceList.size();
	buf.write((void*)&nSurfaceNum,sizeof(int));
	//写入面列表
	for(int i=0;i<nSurfaceNum;i++)
	{
		buf.write((void*)&m_3DSurfaceList[i].first,sizeof(UINT));
        m_3DSurfaceList[i].second->writeBuffer(buf);
	}
}



void C3DSolid::set3DSurfaceList(const _3DSurfaceList& v3DSurfaceList)
{
	m_3DSurfaceList=v3DSurfaceList;
}
_3DSurfaceList& C3DSolid::get3DSurfaceList()
{
	return m_3DSurfaceList;
}
int C3DSolid::getSurfaceNum()
{
	return m_3DSurfaceList.size();
}

size_t C3DSolid::sizeBuffer()
{
	size_t nSize = 0;
	nSize += C3DGeometry::sizeBuffer();
	nSize += sizeof(int);//面个数
	for (int i=0;i<getSurfaceNum();i++)
	{
		nSize += sizeof(UINT);//面大小
		nSize += m_3DSurfaceList[i].second->sizeBuffer();
	}
	return nSize;
}
