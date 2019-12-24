
//#if defined (_MSC_VER) && (_MSC_VER >= 1000)
//#pragma once
//#endif
//#ifndef _INC_C3DOBLIQUE_49E347280178_INCLUDED
//#define _INC_C3DOBLIQUE_49E347280178_INCLUDED

#include "ModelDef.h"
#include "CT3D_3DSurface.h"
#include "ModelDef.h"
//#include "MemManager.h"
#include <vector>

using namespace std;

struct ImageInfo
{
	int width;
	int height;
	unsigned int depth;
	unsigned char *pData;

	ImageInfo()
	{
		width = 0;
		height = 0;
		depth = 0;
		pData = NULL;
	}

	~ImageInfo()
	{
		width = 0;
		height = 0;
		depth = 0;
		if(pData != NULL)
		{
			delete [] pData;
			pData = NULL;
		}
	}
};

class  C3DOblique : public C3DSurface
{
protected:
	/** 几何数据大小*/
	long m_lGeoBufferSize;

	/** 版本号长度*/
	unsigned char m_bVersionLength;

	/** 版本号*/
	string m_sDataVersion;

	/** 逐点变换标志*/
	unsigned char m_bConverAllVertexFlag;

	/** 对象名称长度*/
	unsigned char m_bObjNameLength;

	/** 对象名称*/
	string m_sObjectName;

	/** 定位点(经纬度)*/
	Vertex3d m_vLocation;

	/** 包围盒最小顶点位置(经纬度)*/
	Vertex3d m_vMinVertex;

	/** 包围盒最大顶点位置(经纬度)*/
	Vertex3d m_vMaxVertex;

	/** 顶点类型*/
	unsigned char m_chVertexType;

	/** 顶点列表*/
	void* m_pVertexList;

	/** 顶点个数*/
	long m_lVertexNum;

	/** 顶点索引列表*/
	long* m_pIndexList;

	/** 顶点索引个数*/
	long m_lIndexNum;

public:
	/** 纹理数据大小*/
	unsigned int m_lTexBufferSize;

	/** 纹理数据*/
	ImageInfo* m_pImageInfo;

public:
	C3DOblique();

	virtual ~C3DOblique();

	void SetObjectName(string sObjectName)
	{
		m_sObjectName = sObjectName;
	}

	string GetObjectName()
	{
		return m_sObjectName;
	}
	
	void SetLocation(Vertex3d vLocation)
	{
		m_vLocation = vLocation;
	}

	void GetLocation(Vertex3d& vLocation)
	{
		vLocation = m_vLocation;
	}

	osg::Vec3d GetLocation()
	{
		//vLocation = m_vLocation;
		return osg::Vec3d(m_vLocation.x,m_vLocation.y,m_vLocation.z);
	}

	void SetMinVertex(Vertex3d vMinVertex)
	{
		m_vMinVertex = vMinVertex;
	}

	void GetMinVertex(Vertex3d& vMinVertex)
	{
		vMinVertex = m_vMinVertex;
	}

	void SetMaxVertex(Vertex3d vMaxVertex)
	{
		m_vMaxVertex = vMaxVertex;
	}

	void GetMaxVertex(Vertex3d& vMaxVertex)
	{
		vMaxVertex = m_vMaxVertex;
	}

	void SetVertexType(unsigned char iVertexType)
	{
		m_chVertexType = iVertexType;
	}

	unsigned char GetVertexType() const
	{
		return m_chVertexType;
	}

	void SetVertices(void* const vVertexList)
	{
		m_pVertexList = vVertexList;
	}
	
	void SetVertices(void* const vVertexList,long lVertNum)
	{
		FreeVertexBuffer();
		m_pVertexList = vVertexList;
		m_lVertexNum = lVertNum;
	}
	
	void SetVerticesNum(long lVertNum)
	{
		m_lVertexNum = lVertNum;
	}

	void GetVertices(void*& vVertexList) const
	{
		vVertexList = m_pVertexList;
	}

	void* GetVertices() const
	{
		return m_pVertexList;
	}

	void GetVertices(void*& vVertexList,long& lVertNum) const
	{
		vVertexList = m_pVertexList;
		lVertNum = m_lVertexNum;
	}
	
	long GetVerticesNum() const
	{
		return m_lVertexNum;
	}
	
	void GetTriangles(vector<long>& tr3d)
	{
		tr3d.reserve(m_lIndexNum);
		for(int i=0; i<m_lIndexNum; i++)
		{
			tr3d.push_back(m_pIndexList[i]);
		}
	}

	void SetTriangles(long* const lTriangleList, long lTriListSize)
	{
		m_lIndexNum = lTriListSize;
		m_pIndexList = lTriangleList;
	}
	
	void GetTriangles(long*& lTriangleList, long& lTriListSize) const
	{
		lTriListSize = m_lIndexNum;
		lTriangleList = m_pIndexList;
	}

	void GetImageData(void*& vImageData,long& lImageNum) const
	{
		vImageData = (void*)m_pImageInfo->pData;
		lImageNum = m_lTexBufferSize;/*m_pImageInfo->width * m_pImageInfo->height * (m_pImageInfo->depth / 8);*/
	}
	
public:

	bool readBuffer( Buffer & buf);

	void writeBuffer(Buffer& buf);

	virtual size_t sizeBuffer();


public:
	int GetVertexStrideFromType(int nType);
	void FreeVertexBuffer();
	void FreeIndexBuffer();

//public:

	//void SetVertices(void* vVertexList,long lVertNum, long lBufSize, byte byVerType);
	//bool DoubleVertex2FloatVertex(bool bInverseTexCood);

	//void SetTriangles(long* const lTriangleList, long lTriListSize);
	//void FreeIndexBuffer();
};


