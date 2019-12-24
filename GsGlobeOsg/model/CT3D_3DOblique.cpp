
#include "CT3D_3DOblique.h"
#include "CT3D_Vertex3d.h"
#include "CT3D_Vertex3f.h"
#include "CT3D_ShadedVertex3d.h"
#include "CT3D_ShadedVertex3f.h"
#include "CT3D_TexturedVertex3d.h"
#include "CT3D_TexturedVertex3f.h"
#include "CT3D_ThirdVersatileVertex3d.h"
#include "CT3D_ThirdVersatileVertex3dWithoutRGB.h"
#include "CT3D_ThirdVersatileVertex3fWithoutRGB.h"
#include "CT3D_ThirdVersatileVertex3f.h"
#include "CT3D_NormaledVertex3d.h"
#include "CT3D_NormaledVertex3f.h"
#include "Buffer.h"

C3DOblique::C3DOblique()
{
	m_byTypeID = GEOTYPE_3D_OBLIQUE;

	m_lGeoBufferSize = 0;
	m_bVersionLength = 6;
	m_sDataVersion = "GMDL52";
	m_bConverAllVertexFlag = 0;
	m_bObjNameLength = 0;
	m_sObjectName = "";
	m_pVertexList = NULL;	
	m_lVertexNum = 0;
	m_chVertexType = VERTEXTYPE_V3dT2d;
	m_pIndexList = NULL;
	m_lIndexNum = 0;
	m_lTexBufferSize = 0;
	m_pImageInfo = NULL;
}

C3DOblique::~C3DOblique()
{
	FreeVertexBuffer();
	FreeIndexBuffer();
	if (m_pImageInfo != NULL)
	{
		delete m_pImageInfo;
		m_pImageInfo = NULL;
	}
}

bool C3DOblique::readBuffer( Buffer & buf)
{
// 	bool bRet = C3DSurface::readBuffer(buf);
// 	if(!bRet)
// 		return false;

	//对象特有信息************************
	/** 几何数据大小*/
	buf.read((char*)&m_lGeoBufferSize,sizeof(long));

	/** 版本号长度*/
	buf.read((char*)&m_bVersionLength,sizeof(unsigned char));

	/** 版本号*/
	if (m_bVersionLength>0)
	{
		char* strTempName = new char[m_bVersionLength+1];
		buf.read( (char*)strTempName, m_bVersionLength);
		strTempName[m_bVersionLength]='\0';
		string strName(strTempName);
		m_sDataVersion = strName;
		delete[] strTempName;

	}

	/** 逐点变换标志*/
	buf.read((char*)&m_bConverAllVertexFlag,sizeof(unsigned char));

	/** 对象名称长度*/
	buf.read((char*)&m_bObjNameLength,sizeof(unsigned char));

	/** 对象名称*/
	if (m_bObjNameLength>0)
	{
		char* strTempName = new char[m_bObjNameLength+1];
		buf.read( (char*)strTempName, m_bObjNameLength);
		strTempName[m_bObjNameLength]='\0';
		string strName(strTempName);
		m_sObjectName = strName;
		delete[] strTempName;

	}

	/** 定位点(经纬度)*/
	buf.read((char*)&m_vLocation.x,sizeof(double));
	buf.read((char*)&m_vLocation.y,sizeof(double));
	buf.read((char*)&m_vLocation.z,sizeof(double));

	/** 包围盒最小顶点位置(经纬度)*/
	buf.read((char*)&m_vMinVertex.x,sizeof(double));
	buf.read((char*)&m_vMinVertex.y,sizeof(double));
	buf.read((char*)&m_vMinVertex.z,sizeof(double));

	/** 包围盒最大顶点位置(经纬度)*/
	buf.read((char*)&m_vMaxVertex.x,sizeof(double));
	buf.read((char*)&m_vMaxVertex.y,sizeof(double));
	buf.read((char*)&m_vMaxVertex.z,sizeof(double));

	//顶点类型编码
	buf.read((char*)&m_chVertexType,sizeof(unsigned char));

	//顶点列表
	buf.read((char*)&m_lVertexNum,sizeof(long));

	if(m_chVertexType == VERTEXTYPE_V3dT2d)
	{
		TexturedVertex3d* pList = (TexturedVertex3d*)BufferManager::AllocBuff(sizeof(TexturedVertex3d) * m_lVertexNum);
		m_pVertexList = pList;
		if (NULL == m_pVertexList)
			return false;

		//用for循环避免字节对齐问题
		for (int nNum=0; nNum < m_lVertexNum; nNum++)
		{
			buf.read(&pList[nNum].x,sizeof(double));
			buf.read(&pList[nNum].y,sizeof(double));
			buf.read(&pList[nNum].z,sizeof(double));
			buf.read(&pList[nNum].tu,sizeof(double));
			buf.read(&pList[nNum].tv,sizeof(double));   
		}
		pList = NULL;
	}


	buf.read((char*)&m_lIndexNum,sizeof(long));
	if(m_lIndexNum > 0)
	{
		m_pIndexList = new long[m_lIndexNum];
		buf.read((char*)m_pIndexList,sizeof(long) * m_lIndexNum);
	}

	/** 纹理数据大小*/
 	buf.read((char*)&m_lTexBufferSize,sizeof(unsigned int));
 
 	/** 纹理数据*/
//  	if (m_pImageInfo == NULL)
//  	{
//  		m_pImageInfo = new ImageInfo;
//  	}
//  	buf.read((char*)&m_pImageInfo->width, sizeof(int));
//  	buf.read((char*)&m_pImageInfo->height, sizeof(int));
//  	buf.read((char*)&m_pImageInfo->depth, sizeof(unsigned int));
  	if (m_pImageInfo == NULL)
  	{
  		m_pImageInfo = new ImageInfo;
  	}
	//m_lTexBufferSize = 35787;
 	m_pImageInfo->pData = new unsigned char[m_lTexBufferSize];
 	buf.read((char*)m_pImageInfo->pData, m_lTexBufferSize);

	return true;
}


void C3DOblique::writeBuffer(Buffer& buf)
{
	//C3DSurface::writeBuffer(buf);

	//对象独立信息**************************

	/** 几何数据大小*/
	buf.write((char*)&m_lGeoBufferSize,sizeof(long));

	/** 版本号长度*/
	buf.write((char*)&m_bVersionLength,sizeof(unsigned char));

	/** 版本号*/
	buf.write((char*)m_sDataVersion.c_str(),m_bVersionLength);

	/** 逐点变换标志*/
	buf.write((char*)&m_bConverAllVertexFlag,sizeof(unsigned char));

	/** 对象名称长度*/
	buf.write((char*)&m_bObjNameLength,sizeof(unsigned char));

	/** 对象名称*/
	buf.write((char*)m_sObjectName.c_str(),m_bObjNameLength);

	/** 定位点(经纬度)*/
	buf.write((char*)&m_vLocation.x,sizeof(double));
	buf.write((char*)&m_vLocation.y,sizeof(double));
	buf.write((char*)&m_vLocation.z,sizeof(double));

	/** 包围盒最小顶点位置(经纬度)*/
	buf.write((char*)&m_vMinVertex.x,sizeof(double));
	buf.write((char*)&m_vMinVertex.y,sizeof(double));
	buf.write((char*)&m_vMinVertex.z,sizeof(double));

	/** 包围盒最大顶点位置(经纬度)*/
	buf.write((char*)&m_vMaxVertex.x,sizeof(double));
	buf.write((char*)&m_vMaxVertex.y,sizeof(double));
	buf.write((char*)&m_vMaxVertex.z,sizeof(double));


	//顶点类型编码
	buf.write((char*)&m_chVertexType,sizeof(unsigned char));

	//顶点列表
	buf.write((char*)&m_lVertexNum,sizeof(long));

	if((m_lVertexNum > 0) && m_pVertexList)
	{	
		if(m_chVertexType == VERTEXTYPE_V3dT2d)
		{
			TexturedVertex3d* pList = (TexturedVertex3d*)m_pVertexList;
			//用for循环避免字节对齐问题
			for (int nNum=0; nNum < m_lVertexNum; nNum++)
			{
				buf.write(&pList[nNum].x,sizeof(double));
				buf.write(&pList[nNum].y,sizeof(double));
				buf.write(&pList[nNum].z,sizeof(double));
				buf.write(&pList[nNum].tu,sizeof(double));
				buf.write(&pList[nNum].tv,sizeof(double));   
			}
			pList = NULL;
		}
	}

	buf.write((char*)&m_lIndexNum,sizeof(long));
	if(m_lIndexNum > 0)
	{
		buf.write((char*)m_pIndexList,sizeof(long) * m_lIndexNum);
	}

	/** 纹理数据大小*/
	buf.write((char*)&m_lTexBufferSize,sizeof(unsigned int));

	/** 纹理数据*/
	buf.write((char*)&m_pImageInfo->width, sizeof(int));
	buf.write((char*)&m_pImageInfo->height, sizeof(int));
	buf.write((char*)&m_pImageInfo->depth, sizeof(unsigned int));
	buf.write((char*)m_pImageInfo->pData, sizeof(unsigned char)*(m_pImageInfo->width*m_pImageInfo->height*(m_pImageInfo->depth/8)));
}

int C3DOblique::GetVertexStrideFromType(int nType)
{
	switch (nType)
	{
	case VERTEXTYPE_V3d:
		{	
			return Vertex3d::sizeBuffer();
		}
	case  VERTEXTYPE_V3f:
		{
			return Vertex3f::sizeBuffer();
		}
	case VERTEXTYPE_V3dT2d:
		{
			return TexturedVertex3d::sizeBuffer();
		}
	case VERTEXTYPE_V3fT2f:
		{
			return TexturedVertex3f::sizeBuffer();	
		}
	case  VERTEXTYPE_V3dN3dC3f:
		{
			return ShadedVertex3d::sizeBuffer();
		}
	case  VERTEXTYPE_V3fN3fC3f:
		{
			return ShadedVertex3f::sizeBuffer();
		}
	case  VERTEXTYPE_V3dT2dN3dC3f:
		{
			return VersatileVertex3d::sizeBuffer();
		}
	case  VERTEXTYPE_V3dT2dN3dC3f_WithoutRGB:
		{
			return VersatileVertex3dWithoutRGB::sizeBuffer();
		}
	case  VERTEXTYPE_V3fT2fN3fC3f:
		{
			return VersatileVertex3f::sizeBuffer();
		}
	case  VERTEXTYPE_V3fT2fN3fC3f_WithoutRGB:
		{
			return VersatileVertex3fWithoutRGB::sizeBuffer();
		}
	case  VERTEXTYPE_V3dT2dT2dN3dC3f:
		{
			return SecondVersatileVertex3d::sizeBuffer();
		}
	case  VERTEXTYPE_V3dT2dT2dN3dC3f_WithoutRGB:
		{
			return SecondVersatileVertex3dWithoutRGB::sizeBuffer();
		}
	case  VERTEXTYPE_V3fT2fT2fN3fC3f:
		{
			return SecondVersatileVertex3f::sizeBuffer();
		}
	case  VERTEXTYPE_V3fT2fT2fN3fC3f_WithoutRGB:
		{
			return SecondVersatileVertex3fWithoutRGB::sizeBuffer();
		}
	case  VERTEXTYPE_V3dT2dT2dT2dN3dC3f:
		{
			return ThirdVersatileVertex3d::sizeBuffer();
		}
	case  VERTEXTYPE_V3dT2dT2dT2dN3dC3f_WithoutRGB:
		{
			return ThirdVersatileVertex3dWithoutRGB::sizeBuffer();
		}
	case  VERTEXTYPE_V3fT2fT2fT2fN3fC3f:
		{
			return ThirdVersatileVertex3f::sizeBuffer();
		}
	case  VERTEXTYPE_V3fT2fT2fT2fN3fC3f_WithoutRGB:
		{
			return ThirdVersatileVertex3fWithoutRGB::sizeBuffer();
		}
	default:
		break;
	}

	return 0;
}

size_t C3DOblique::sizeBuffer()
{
	size_t nSize = 0;

	//共有信息(Geometry，3DGeometry)
	//*************************************************
	//nSize += C3DSurface::sizeBuffer();

	//专有信息
	//*************************************************
	nSize += sizeof(long);
	nSize += sizeof(unsigned char);
	nSize += m_bVersionLength;
	nSize += sizeof(unsigned char);
	nSize += sizeof(unsigned char);
	nSize += m_bObjNameLength;
	nSize += 3*sizeof(double);
	nSize += 3*sizeof(double);
	nSize += 3*sizeof(double);
	nSize += sizeof(unsigned char); // 	unsigned char m_chVertexType;
	nSize += sizeof(long); // 	long m_lVertexNum;
	nSize += GetVertexStrideFromType(m_chVertexType) * m_lVertexNum;
	nSize += sizeof(long); // 	long m_lIndexNum;
	nSize += sizeof(long) * m_lIndexNum; // 	long* m_pIndexList;
	nSize += sizeof(long);
	nSize += m_lTexBufferSize;

	return nSize;

}

void C3DOblique::FreeVertexBuffer()
{
	if (NULL != m_pVertexList)
	{
		BufferManager::FreeBuffer((unsigned char*)m_pVertexList, GetVertexStrideFromType(m_chVertexType)*m_lVertexNum);
		m_pVertexList = NULL;
	}
}

void C3DOblique::FreeIndexBuffer()
{
	if (NULL != m_pIndexList)
	{
		BufferManager::FreeBuffer((unsigned char*)m_pIndexList, sizeof(long)*m_lIndexNum);
		m_pIndexList = NULL;
	}
}