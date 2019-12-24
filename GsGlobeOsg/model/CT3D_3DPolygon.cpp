//********************************************************************************** 
//** 文件名称：CT3D_3DPolygon.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维多边形类实现
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-11
//********************************************************************************** 


#include "CT3D_3DPolygon.h"

//********************************************************************************** 
//** 函数名称： C3DPolygon
//** 功能描述： 构造函数
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-11
//**********************************************************************************
C3DPolygon::C3DPolygon()
{
    m_byTypeID = GEOTYPE_3D_POLYGON;
}


//********************************************************************************** 
//** 函数名称： ~C3DPolygon
//** 功能描述： 析构函数
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-11
//**********************************************************************************
C3DPolygon::~C3DPolygon()
{
    int nCount = m_vecRingList.size();
    for(int i=0; i < nCount; i++)
    {
        if(m_vecRingList[i])
            delete m_vecRingList[i];
        m_vecRingList[i]= NULL;
    }
    m_vecRingList.clear();

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
bool C3DPolygon::readBuffer( Buffer & buf)
{
	bool bRet =C3DSurface::readBuffer(buf);
	//modify by yangling 20131208 begin
	if(!bRet)
	{
		return false;
	}
	//modify by yangling 20131208 end
    int nLinearRingNum = 0;
	//读取线性环个数
	buf.read((char*)&nLinearRingNum,sizeof(int));
	//读取线性环列表
    C3DLinearRing* pLinearRing =NULL;
    size_t nSize = 0;
	for(int i=0;i < nLinearRingNum;i++)
	{	
        buf.read((char*)&nSize,sizeof(unsigned int));//下面对象的大小
        pLinearRing = new C3DLinearRing();
        pLinearRing->readBuffer(buf);
        m_vecRingList.push_back(pLinearRing);
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
void C3DPolygon::writeBuffer(Buffer& buf)
{
	C3DSurface::writeBuffer(buf);
    int nSize = 0;
	//写入线性环个数
    int nLinearRingNum = getLinearRingNum();
	buf.write((char*)&nLinearRingNum,sizeof(int));
	//写入线性环列表
	for(int i=0;i< nLinearRingNum;i++)
	{
        nSize = m_vecRingList[i]->sizeBuffer();
;		buf.write((char*)&nSize,sizeof(unsigned int));
        m_vecRingList[i]->writeBuffer(buf);
	}
}

int C3DPolygon::getLinearRingNum()
{
	return m_vecRingList.size();
}

void C3DPolygon::setLinearRingList(const  _3DLinearRingList& vRingList)
{
	m_vecRingList=vRingList;
}
 _3DLinearRingList& C3DPolygon::getLinearRingList()
{
	return m_vecRingList;
}
	
size_t C3DPolygon::sizeBuffer()
{
	size_t nSize = 0;
	nSize += C3DSurface::sizeBuffer();
	nSize += sizeof(int);//线性环个数

	for (int i=0;i<getLinearRingNum();i++)
	{
		nSize += sizeof(UINT);//线环大小变量
		nSize += m_vecRingList[i]->sizeBuffer();//线环大小值
	}
	return nSize;
}

