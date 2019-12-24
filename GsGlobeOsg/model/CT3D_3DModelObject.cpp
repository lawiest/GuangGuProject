//********************************************************************************** 
//** 文件名称：CT3D_C3DSolid.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维体类实现
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-11
//********************************************************************************** 


#include "CT3D_3DModelObject.h"
#include "Buffer.h"

//********************************************************************************** 
//** 函数名称： C3DModelObject
//** 功能描述： 构造函数
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-11
//**********************************************************************************
 C3DModelObject::C3DModelObject()
{
    m_byTypeID = GEOTYPE_3D_MODELOBJECT;
	m_lAdjacencyCount = 0;
	m_pAdjacencyTable = NULL;
}



 //********************************************************************************** 
 //** 函数名称： ~C3DModelObject
 //** 功能描述： 析构函数
 //** 输    入： 无           
 //** 输    出： 无
 //** 返回值：	无
 //** 作    者： 杨灵
 //** 创建日期： 2012-04-11
 //**********************************************************************************
 C3DModelObject::~C3DModelObject()
{
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
 	m_vecD3dxMaterialParam.clear();

	if (m_pAdjacencyTable != NULL)
	{
		delete []m_pAdjacencyTable;
		m_pAdjacencyTable = NULL;
	}
	m_lAdjacencyCount = 0;
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
bool C3DModelObject::readBuffer( Buffer & buf)
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
			continue;

		if (!pSurface->readBuffer(buf))
		{
			SafeDelete(pSurface);
			return false;
		}
		
		_3DSurfacePair surfacepair(nSize,pSurface);
		m_3DSurfaceList.push_back(surfacepair);

	}
	//------------------------------------------------------------------
	int nCount1 = 0;
	buf.read((char*)&nCount1,sizeof(int));//关系表个数

	m_vecD3dxMaterialParam.reserve(nCount1);
	for(int i=0; i<nCount1; i++)
	{				
		unsigned char nSize = 0;
		buf.read((void*)&nSize,sizeof(unsigned char));//第i个关系表的大小

		//第i个关系表
		D3dxMaterialParam matParam;
		buf.read((void*)&matParam.nAttribId, sizeof(unsigned long));
		buf.read((void*)&matParam.nFaceStart, sizeof(unsigned long));
		buf.read((void*)&matParam.nFaceCount, sizeof(unsigned long));
		buf.read((void*)&matParam.nVertexStart, sizeof(unsigned long));
		buf.read((void*)&matParam.nVertexCount, sizeof(unsigned long));

		m_vecD3dxMaterialParam.push_back(matParam);
	}

	buf.read((char*)&m_lAdjacencyCount,sizeof(long));
	if(m_lAdjacencyCount > 0)
	{
		m_pAdjacencyTable = new unsigned long[m_lAdjacencyCount];
		buf.read((char*)m_pAdjacencyTable,sizeof(unsigned long) * m_lAdjacencyCount);
	}
	//------------------------------------------------------------------

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
void C3DModelObject::writeBuffer(Buffer& buf)
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

	//------------------------------------------------------------------
	int nCount1 = m_vecD3dxMaterialParam.size();	//关系表个数
	buf.write((char*)&nCount1, sizeof(int));

	for(int i=0; i<nCount1; i++)
	{				
		unsigned char nSize = 0;
		nSize = sizeof(D3dxMaterialParam);
		buf.write((void*)&nSize,sizeof(unsigned char));//第i个关系表的大小

		//第i个关系表
		buf.write((void*)&(m_vecD3dxMaterialParam[i].nAttribId), sizeof(unsigned long));
		buf.write((void*)&(m_vecD3dxMaterialParam[i].nFaceStart), sizeof(unsigned long));
		buf.write((void*)&(m_vecD3dxMaterialParam[i].nFaceCount), sizeof(unsigned long));
		buf.write((void*)&(m_vecD3dxMaterialParam[i].nVertexStart), sizeof(unsigned long));
		buf.write((void*)&(m_vecD3dxMaterialParam[i].nVertexCount), sizeof(unsigned long));
	}

	buf.write((char*)&m_lAdjacencyCount,sizeof(long));
	if(m_lAdjacencyCount > 0)
	{
		buf.write((char*)m_pAdjacencyTable,sizeof(unsigned long) * m_lAdjacencyCount);
	}
	//------------------------------------------------------------------
}



void C3DModelObject::set3DSurfaceList(const _3DSurfaceList& v3DSurfaceList)
{
	m_3DSurfaceList=v3DSurfaceList;
}
_3DSurfaceList& C3DModelObject::get3DSurfaceList()
{
	return m_3DSurfaceList;
}
int C3DModelObject::getSurfaceNum()
{
	return m_3DSurfaceList.size();
}

size_t C3DModelObject::sizeBuffer()
{
	size_t nSize = 0;
	nSize += C3DGeometry::sizeBuffer();
	nSize += sizeof(int);//面个数
	for (int i=0;i<getSurfaceNum();i++)
	{
		nSize += sizeof(UINT);//面大小
		nSize += m_3DSurfaceList[i].second->sizeBuffer();
	}

	//关系表
	nSize += sizeof(int);						//关系表个数
	for (size_t i=0; i<m_vecD3dxMaterialParam.size(); i++)
	{
		nSize += sizeof(unsigned char);			//关系表大小
		nSize += sizeof(D3dxMaterialParam);		//关系表数据
	}

	//邻接表
	nSize += sizeof(long);									//m_lAdjacencyCount
	nSize += sizeof(unsigned long)*m_lAdjacencyCount;		//m_pAdjacencyTable

	return nSize;
}
