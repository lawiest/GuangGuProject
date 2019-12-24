//********************************************************************************** 
//** 文件名称：CT3D_3DLinearRing.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 线环类实现
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-11
//********************************************************************************** 


#include "CT3D_3DLinearRing.h"
#include "Buffer.h"
#include "CT3D_Vertex3d.h"
#include "CT3D_Vertex3f.h"
#include "CT3D_TexturedVertex3d.h"
#include "CT3D_TexturedVertex3f.h"
#include "CT3D_ShadedVertex3d.h"
#include "CT3D_ShadedVertex3f.h"
#include "CT3D_ThirdVersatileVertex3d.h"
#include "CT3D_ThirdVersatileVertex3f.h"
//********************************************************************************** 
//** 函数名称： C3DLinearRing
//** 功能描述： 构造函数1
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-11
//**********************************************************************************
C3DLinearRing::C3DLinearRing()
{
    m_nVertNum = 0;
    m_vVertices = NULL;
    m_bInOrOutRing = 0; // 外环
    m_byTypeID = GEOTYPE_3D_LINEARRING;
    m_byVertexType = VERTEXTYPE_V3d;
}

//********************************************************************************** 
//** 函数名称： C3DLinearRing
//** 功能描述： 构造函数2
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-11
//**********************************************************************************
C3DLinearRing::C3DLinearRing(int nVertexNum, Vertex3d* const vVertices,int bInOrOutRing)
:m_nVertNum(nVertexNum), 
 m_vVertices(vVertices),
 m_bInOrOutRing(bInOrOutRing)
{
    m_byTypeID = GEOTYPE_3D_LINEARRING;
    m_byVertexType = VERTEXTYPE_V3d;
}

//********************************************************************************** 
//** 函数名称： ~C3DPolygon
//** 功能描述： 析构函数
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
 C3DLinearRing::~C3DLinearRing()
{
    ReleaseVertexArray(m_byVertexType, m_vVertices);     

}

//********************************************************************************** 
//** 函数名称： readBuffer
//** 功能描述： 将数据读入buf
//** 输    入： Buffer对象buf的引用           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
bool C3DLinearRing::readBuffer( Buffer & buf)
{
	bool bRet = C3DGeometry::readBuffer(buf);
	if(!bRet)
	{
		return false;
	}
	//读取顶点类型编码
	buf.read((void*)&m_byVertexType,sizeof(byte));
	//modify by yangling 20131208 begin
	if(m_byVertexType >= VERTEXTYPE_End || m_byVertexType < VERTEXTYPE_V3f)
	{
		return false;
	}
	//modify by yangling 20131208 end
	
	//读取线环顶点数
	buf.read((void*)&m_nVertNum,sizeof(int));
	//读取线环结点序列
	if(m_nVertNum > 0)
	{
		if(m_byVertexType == VERTEXTYPE_V3d)	
		{
            m_vVertices = new Vertex3d[m_nVertNum];
            Vertex3d* pList = (Vertex3d*)m_vVertices;
            //用for循环避免字节对齐问题
            for (int nNum=0; nNum < m_nVertNum; nNum++)
            {
                buf.read(&pList[nNum].x,sizeof(double));
                buf.read(&pList[nNum].y,sizeof(double));
                buf.read(&pList[nNum].z,sizeof(double));
            }
            pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3f) 
		{
            m_vVertices = new Vertex3f[m_nVertNum];
            Vertex3f* pList = (Vertex3f*)m_vVertices;
            //用for循环避免字节对齐问题
            for (int nNum=0; nNum < m_nVertNum; nNum++)
            {
                buf.read(&pList[nNum].x,sizeof(float));
                buf.read(&pList[nNum].y,sizeof(float));
                buf.read(&pList[nNum].z,sizeof(float));
            }
            pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3dT2d)
		{
            m_vVertices = new TexturedVertex3d[m_nVertNum];
            TexturedVertex3d* pList = (TexturedVertex3d*)m_vVertices;
            //用for循环避免字节对齐问题
            for (int nNum=0; nNum < m_nVertNum; nNum++)
            {
                buf.read(&pList[nNum].x,sizeof(double));
                buf.read(&pList[nNum].y,sizeof(double));
                buf.read(&pList[nNum].z,sizeof(double));
                buf.read(&pList[nNum].tu,sizeof(double));
                buf.read(&pList[nNum].tv,sizeof(double));   
            }
            pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3fT2f)
		{
            m_vVertices = new TexturedVertex3f[m_nVertNum];
            TexturedVertex3f* pList = (TexturedVertex3f*)m_vVertices;
            //用for循环避免字节对齐问题
            for (int nNum=0; nNum < m_nVertNum; nNum++)
            {
                buf.read(&pList[nNum].x,sizeof(float));
                buf.read(&pList[nNum].y,sizeof(float));
                buf.read(&pList[nNum].z,sizeof(float));
                buf.read(&pList[nNum].tu,sizeof(float));
                buf.read(&pList[nNum].tv,sizeof(float));   
            }
            pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3dN3dC3f)
		{
            m_vVertices = new ShadedVertex3d[m_nVertNum];
            ShadedVertex3d* pList = (ShadedVertex3d*)m_vVertices;
            //用for循环避免字节对齐问题
            for (int nNum=0; nNum < m_nVertNum; nNum++)
            {
                buf.read(&pList[nNum].x,sizeof(double));
                buf.read(&pList[nNum].y,sizeof(double));
                buf.read(&pList[nNum].z,sizeof(double));
                buf.read(&pList[nNum].nx,sizeof(double));
                buf.read(&pList[nNum].ny,sizeof(double));
                buf.read(&pList[nNum].nz,sizeof(double));
                buf.read(&pList[nNum].r,sizeof(float));
                buf.read(&pList[nNum].g,sizeof(float));
                buf.read(&pList[nNum].b,sizeof(float));
            }
            pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3fN3fC3f)
		{
            m_vVertices = new ShadedVertex3f[m_nVertNum];
            ShadedVertex3f* pList = (ShadedVertex3f*)m_vVertices;
            //用for循环避免字节对齐问题
            for (int nNum=0; nNum < m_nVertNum; nNum++)
            {
                buf.read(&pList[nNum].x,sizeof(float));
                buf.read(&pList[nNum].y,sizeof(float));
                buf.read(&pList[nNum].z,sizeof(float));
                buf.read(&pList[nNum].nx,sizeof(float));
                buf.read(&pList[nNum].ny,sizeof(float));
                buf.read(&pList[nNum].nz,sizeof(float));
                buf.read(&pList[nNum].r,sizeof(float));
                buf.read(&pList[nNum].g,sizeof(float));
                buf.read(&pList[nNum].b,sizeof(float));
            }
            pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3dT2dN3dC3f)
		{
            m_vVertices = new VersatileVertex3d[m_nVertNum];
            VersatileVertex3d* pList=NULL;
            pList = (VersatileVertex3d*)m_vVertices;
            //用for循环避免字节对齐问题
            for (int nNum=0; nNum < m_nVertNum; nNum++)
            {
                buf.read(&pList[nNum].x,sizeof(double));
                buf.read(&pList[nNum].y,sizeof(double));
                buf.read(&pList[nNum].z,sizeof(double));
                buf.read(&pList[nNum].tu,sizeof(double));
                buf.read(&pList[nNum].tv,sizeof(double));
                buf.read(&pList[nNum].nx,sizeof(double));
                buf.read(&pList[nNum].ny,sizeof(double));
                buf.read(&pList[nNum].nz,sizeof(double));
                buf.read(&pList[nNum].r,sizeof(float));
                buf.read(&pList[nNum].g,sizeof(float));
                buf.read(&pList[nNum].b,sizeof(float));

            }
            pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3fT2fN3fC3f)
		{
            m_vVertices = new VersatileVertex3f[m_nVertNum];
            VersatileVertex3f* pList=NULL;
            pList = (VersatileVertex3f*)m_vVertices;
            //用for循环避免字节对齐问题
            for (int nNum=0; nNum < m_nVertNum; nNum++)
            {
                buf.read(&pList[nNum].x,sizeof(float));
                buf.read(&pList[nNum].y,sizeof(float));
                buf.read(&pList[nNum].z,sizeof(float));
                buf.read(&pList[nNum].tu,sizeof(float));
                buf.read(&pList[nNum].tv,sizeof(float));
                buf.read(&pList[nNum].nx,sizeof(float));
                buf.read(&pList[nNum].ny,sizeof(float));
                buf.read(&pList[nNum].nz,sizeof(float));
                buf.read(&pList[nNum].r,sizeof(float));
                buf.read(&pList[nNum].g,sizeof(float));
                buf.read(&pList[nNum].b,sizeof(float));
            }
            pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3dT2dT2dN3dC3f)
		{
			m_vVertices = new SecondVersatileVertex3d[m_nVertNum];
			SecondVersatileVertex3d* pList=NULL;
			pList = (SecondVersatileVertex3d*)m_vVertices;
			//用for循环避免字节对齐问题
			for (int nNum=0; nNum < m_nVertNum; nNum++)
			{
				buf.read(&pList[nNum].x,sizeof(double));
				buf.read(&pList[nNum].y,sizeof(double));
				buf.read(&pList[nNum].z,sizeof(double));
				buf.read(&pList[nNum].tu,sizeof(double));
				buf.read(&pList[nNum].tv,sizeof(double));
				buf.read(&pList[nNum].tu2,sizeof(double));
				buf.read(&pList[nNum].tv2,sizeof(double));
				buf.read(&pList[nNum].nx,sizeof(double));
				buf.read(&pList[nNum].ny,sizeof(double));
				buf.read(&pList[nNum].nz,sizeof(double));
				buf.read(&pList[nNum].r,sizeof(float));
				buf.read(&pList[nNum].g,sizeof(float));
				buf.read(&pList[nNum].b,sizeof(float));

			}
			pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3fT2fT2fN3fC3f)
		{
			m_vVertices = new SecondVersatileVertex3f[m_nVertNum];
			SecondVersatileVertex3f* pList=NULL;
			pList = (SecondVersatileVertex3f*)m_vVertices;
			//用for循环避免字节对齐问题
			for (int nNum=0; nNum < m_nVertNum; nNum++)
			{
				buf.read(&pList[nNum].x,sizeof(float));
				buf.read(&pList[nNum].y,sizeof(float));
				buf.read(&pList[nNum].z,sizeof(float));
				buf.read(&pList[nNum].tu,sizeof(float));
				buf.read(&pList[nNum].tv,sizeof(float));
				buf.read(&pList[nNum].tu2,sizeof(float));
				buf.read(&pList[nNum].tv2,sizeof(float));
				buf.read(&pList[nNum].nx,sizeof(float));
				buf.read(&pList[nNum].ny,sizeof(float));
				buf.read(&pList[nNum].nz,sizeof(float));
				buf.read(&pList[nNum].r,sizeof(float));
				buf.read(&pList[nNum].g,sizeof(float));
				buf.read(&pList[nNum].b,sizeof(float));
			}
			pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3dT2dT2dT2dN3dC3f)
		{
			m_vVertices = new ThirdVersatileVertex3d[m_nVertNum];
			ThirdVersatileVertex3d* pList=NULL;
			pList = (ThirdVersatileVertex3d*)m_vVertices;
			//用for循环避免字节对齐问题
			for (int nNum=0; nNum < m_nVertNum; nNum++)
			{
				buf.read(&pList[nNum].x,sizeof(double));
				buf.read(&pList[nNum].y,sizeof(double));
				buf.read(&pList[nNum].z,sizeof(double));
				buf.read(&pList[nNum].tu,sizeof(double));
				buf.read(&pList[nNum].tv,sizeof(double));
				buf.read(&pList[nNum].tu2,sizeof(double));
				buf.read(&pList[nNum].tv2,sizeof(double));
				buf.read(&pList[nNum].tu3,sizeof(double));
				buf.read(&pList[nNum].tv3,sizeof(double));
				buf.read(&pList[nNum].nx,sizeof(double));
				buf.read(&pList[nNum].ny,sizeof(double));
				buf.read(&pList[nNum].nz,sizeof(double));
				buf.read(&pList[nNum].r,sizeof(float));
				buf.read(&pList[nNum].g,sizeof(float));
				buf.read(&pList[nNum].b,sizeof(float));

			}
			pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3fT2fT2fT2fN3fC3f)
		{
			m_vVertices = new ThirdVersatileVertex3f[m_nVertNum];
			ThirdVersatileVertex3f* pList=NULL;
			pList = (ThirdVersatileVertex3f*)m_vVertices;
			//用for循环避免字节对齐问题
			for (int nNum=0; nNum < m_nVertNum; nNum++)
			{
				buf.read(&pList[nNum].x,sizeof(float));
				buf.read(&pList[nNum].y,sizeof(float));
				buf.read(&pList[nNum].z,sizeof(float));
				buf.read(&pList[nNum].tu,sizeof(float));
				buf.read(&pList[nNum].tv,sizeof(float));
				buf.read(&pList[nNum].tu2,sizeof(float));
				buf.read(&pList[nNum].tv2,sizeof(float));
				buf.read(&pList[nNum].tu3,sizeof(float));
				buf.read(&pList[nNum].tv3,sizeof(float));
				buf.read(&pList[nNum].nx,sizeof(float));
				buf.read(&pList[nNum].ny,sizeof(float));
				buf.read(&pList[nNum].nz,sizeof(float));
				buf.read(&pList[nNum].r,sizeof(float));
				buf.read(&pList[nNum].g,sizeof(float));
				buf.read(&pList[nNum].b,sizeof(float));
			}
			pList = NULL;
		}
	}
	//读取内环还是外环的标志
	buf.read((void*)&m_bInOrOutRing,sizeof(byte));
	//modify by yangling 20131208 begin
	if((m_bInOrOutRing !=0)&&(m_bInOrOutRing!=1))
	{
		return false;
	}
	//modify by yangling 20131208 end
	return true;		
}

//********************************************************************************** 
//** 函数名称： writeBuffer
//** 功能描述： 将数据写入buf
//** 输    入： Buffer对象buf的引用           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-11
//**********************************************************************************
void C3DLinearRing::writeBuffer(Buffer& buf)
{
	C3DGeometry::writeBuffer(buf);
	//写入顶点类型编码
	buf.write((void*)&m_byVertexType,sizeof(byte));
	//写入线环顶点数
	buf.write((void*)&m_nVertNum,sizeof(int));
	//写入线环结点序列
	if(m_nVertNum > 0)
	{
		if(m_byVertexType == VERTEXTYPE_V3d)	
		{
            Vertex3d* pList = (Vertex3d*)m_vVertices;
            //用for循环避免字节对齐问题
            for (int nNum=0; nNum < m_nVertNum; nNum++)
            {
                buf.write(&pList[nNum].x,sizeof(double));
                buf.write(&pList[nNum].y,sizeof(double));
                buf.write(&pList[nNum].z,sizeof(double));
            }
            pList = NULL;

		}
		else if(m_byVertexType == VERTEXTYPE_V3f) 
		{
            Vertex3f* pList = (Vertex3f*)m_vVertices;
            //用for循环避免字节对齐问题
            for (int nNum=0; nNum < m_nVertNum; nNum++)
            {
                buf.write(&pList[nNum].x,sizeof(float));
                buf.write(&pList[nNum].y,sizeof(float));
                buf.write(&pList[nNum].z,sizeof(float));
            }
            pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3dT2d)
		{
            TexturedVertex3d* pList = (TexturedVertex3d*)m_vVertices;
            //用for循环避免字节对齐问题
            for (int nNum=0; nNum < m_nVertNum; nNum++)
            {
                buf.write(&pList[nNum].x,sizeof(double));
                buf.write(&pList[nNum].y,sizeof(double));
                buf.write(&pList[nNum].z,sizeof(double));
                buf.write(&pList[nNum].tu,sizeof(double));
                buf.write(&pList[nNum].tv,sizeof(double));   
            }
            pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3fT2f)
		{
            TexturedVertex3f* pList = (TexturedVertex3f*)m_vVertices;
            //用for循环避免字节对齐问题
            for (int nNum=0; nNum < m_nVertNum; nNum++)
            {
                buf.write(&pList[nNum].x,sizeof(float));
                buf.write(&pList[nNum].y,sizeof(float));
                buf.write(&pList[nNum].z,sizeof(float));
                buf.write(&pList[nNum].tu,sizeof(float));
                buf.write(&pList[nNum].tv,sizeof(float));   
            }
            pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3dN3dC3f)
		{
            ShadedVertex3d* pList = (ShadedVertex3d*)m_vVertices;
            //用for循环避免字节对齐问题
            for (int nNum=0; nNum < m_nVertNum; nNum++)
            {
                buf.write(&pList[nNum].x,sizeof(double));
                buf.write(&pList[nNum].y,sizeof(double));
                buf.write(&pList[nNum].z,sizeof(double));
                buf.write(&pList[nNum].nx,sizeof(double));
                buf.write(&pList[nNum].ny,sizeof(double));
                buf.write(&pList[nNum].nz,sizeof(double));
                buf.write(&pList[nNum].r,sizeof(float));
                buf.write(&pList[nNum].g,sizeof(float));
                buf.write(&pList[nNum].b,sizeof(float));
            }
            pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3fN3fC3f)
		{
            ShadedVertex3f* pList = (ShadedVertex3f*)m_vVertices;
            //用for循环避免字节对齐问题
            for (int nNum=0; nNum < m_nVertNum; nNum++)
            {
                buf.write(&pList[nNum].x,sizeof(float));
                buf.write(&pList[nNum].y,sizeof(float));
                buf.write(&pList[nNum].z,sizeof(float));
                buf.write(&pList[nNum].nx,sizeof(float));
                buf.write(&pList[nNum].ny,sizeof(float));
                buf.write(&pList[nNum].nz,sizeof(float));
                buf.write(&pList[nNum].r,sizeof(float));
                buf.write(&pList[nNum].g,sizeof(float));
                buf.write(&pList[nNum].b,sizeof(float));
            }
            pList = NULL;

		}
		else if(m_byVertexType == VERTEXTYPE_V3dT2dN3dC3f)
		{
            VersatileVertex3d* pList=NULL;
            pList = (VersatileVertex3d*)m_vVertices;
            //用for循环避免字节对齐问题
            for (int nNum=0; nNum < m_nVertNum; nNum++)
            {
                buf.write(&pList[nNum].x,sizeof(double));
                buf.write(&pList[nNum].y,sizeof(double));
                buf.write(&pList[nNum].z,sizeof(double));
                buf.write(&pList[nNum].tu,sizeof(double));
                buf.write(&pList[nNum].tv,sizeof(double));
                buf.write(&pList[nNum].nx,sizeof(double));
                buf.write(&pList[nNum].ny,sizeof(double));
                buf.write(&pList[nNum].nz,sizeof(double));
                buf.write(&pList[nNum].r,sizeof(float));
                buf.write(&pList[nNum].g,sizeof(float));
                buf.write(&pList[nNum].b,sizeof(float));

            }
            pList = NULL;

		}
		else if(m_byVertexType == VERTEXTYPE_V3fT2fN3fC3f)
		{
            VersatileVertex3f* pList=NULL;
            pList = (VersatileVertex3f*)m_vVertices;
            //用for循环避免字节对齐问题
            for (int nNum=0; nNum < m_nVertNum; nNum++)
            {
                buf.write(&pList[nNum].x,sizeof(float));
                buf.write(&pList[nNum].y,sizeof(float));
                buf.write(&pList[nNum].z,sizeof(float));
                buf.write(&pList[nNum].tu,sizeof(float));
                buf.write(&pList[nNum].tv,sizeof(float));
                buf.write(&pList[nNum].nx,sizeof(float));
                buf.write(&pList[nNum].ny,sizeof(float));
                buf.write(&pList[nNum].nz,sizeof(float));
                buf.write(&pList[nNum].r,sizeof(float));
                buf.write(&pList[nNum].g,sizeof(float));
                buf.write(&pList[nNum].b,sizeof(float));
            }
            pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3dT2dT2dN3dC3f)
		{
			SecondVersatileVertex3d* pList=NULL;
			pList = (SecondVersatileVertex3d*)m_vVertices;
			for (int nNum=0; nNum < m_nVertNum; nNum++)
			{
				buf.write(&pList[nNum].x,sizeof(double));
				buf.write(&pList[nNum].y,sizeof(double));
				buf.write(&pList[nNum].z,sizeof(double));
				buf.write(&pList[nNum].tu,sizeof(double));
				buf.write(&pList[nNum].tv,sizeof(double));
				buf.write(&pList[nNum].tu2,sizeof(double));
				buf.write(&pList[nNum].tv2,sizeof(double));
				buf.write(&pList[nNum].nx,sizeof(double));
				buf.write(&pList[nNum].ny,sizeof(double));
				buf.write(&pList[nNum].nz,sizeof(double));
				buf.write(&pList[nNum].r,sizeof(float));
				buf.write(&pList[nNum].g,sizeof(float));
				buf.write(&pList[nNum].b,sizeof(float));

			}
			pList = NULL;

		}
		else if(m_byVertexType == VERTEXTYPE_V3fT2fT2fN3fC3f)
		{
			SecondVersatileVertex3f* pList=NULL;
			pList = (SecondVersatileVertex3f*)m_vVertices;
			for (int nNum=0; nNum < m_nVertNum; nNum++)
			{
				buf.write(&pList[nNum].x,sizeof(float));
				buf.write(&pList[nNum].y,sizeof(float));
				buf.write(&pList[nNum].z,sizeof(float));
				buf.write(&pList[nNum].tu,sizeof(float));
				buf.write(&pList[nNum].tv,sizeof(float));
				buf.write(&pList[nNum].tu2,sizeof(float));
				buf.write(&pList[nNum].tv2,sizeof(float));
				buf.write(&pList[nNum].nx,sizeof(float));
				buf.write(&pList[nNum].ny,sizeof(float));
				buf.write(&pList[nNum].nz,sizeof(float));
				buf.write(&pList[nNum].r,sizeof(float));
				buf.write(&pList[nNum].g,sizeof(float));
				buf.write(&pList[nNum].b,sizeof(float));
			}
			pList = NULL;
		}
		else if(m_byVertexType == VERTEXTYPE_V3dT2dT2dT2dN3dC3f)
		{
			ThirdVersatileVertex3d* pList=NULL;
			pList = (ThirdVersatileVertex3d*)m_vVertices;
			for (int nNum=0; nNum < m_nVertNum; nNum++)
			{
				buf.write(&pList[nNum].x,sizeof(double));
				buf.write(&pList[nNum].y,sizeof(double));
				buf.write(&pList[nNum].z,sizeof(double));
				buf.write(&pList[nNum].tu,sizeof(double));
				buf.write(&pList[nNum].tv,sizeof(double));
				buf.write(&pList[nNum].tu2,sizeof(double));
				buf.write(&pList[nNum].tv2,sizeof(double));
				buf.write(&pList[nNum].tu3,sizeof(double));
				buf.write(&pList[nNum].tv3,sizeof(double));
				buf.write(&pList[nNum].nx,sizeof(double));
				buf.write(&pList[nNum].ny,sizeof(double));
				buf.write(&pList[nNum].nz,sizeof(double));
				buf.write(&pList[nNum].r,sizeof(float));
				buf.write(&pList[nNum].g,sizeof(float));
				buf.write(&pList[nNum].b,sizeof(float));

			}
			pList = NULL;

		}
		else if(m_byVertexType == VERTEXTYPE_V3fT2fT2fT2fN3fC3f)
		{
			ThirdVersatileVertex3f* pList=NULL;
			pList = (ThirdVersatileVertex3f*)m_vVertices;
			for (int nNum=0; nNum < m_nVertNum; nNum++)
			{
				buf.write(&pList[nNum].x,sizeof(float));
				buf.write(&pList[nNum].y,sizeof(float));
				buf.write(&pList[nNum].z,sizeof(float));
				buf.write(&pList[nNum].tu,sizeof(float));
				buf.write(&pList[nNum].tv,sizeof(float));
				buf.write(&pList[nNum].tu2,sizeof(float));
				buf.write(&pList[nNum].tv2,sizeof(float));
				buf.write(&pList[nNum].tu3,sizeof(float));
				buf.write(&pList[nNum].tv3,sizeof(float));
				buf.write(&pList[nNum].nx,sizeof(float));
				buf.write(&pList[nNum].ny,sizeof(float));
				buf.write(&pList[nNum].nz,sizeof(float));
				buf.write(&pList[nNum].r,sizeof(float));
				buf.write(&pList[nNum].g,sizeof(float));
				buf.write(&pList[nNum].b,sizeof(float));
			}
			pList = NULL;
		}
	}
	//写入内环还是外环的标志
	buf.write((void*)&m_bInOrOutRing,sizeof(byte));
	
}
	
void C3DLinearRing::setVertexType(byte byVertexType)
{
	m_byVertexType=byVertexType;
}
byte C3DLinearRing::getVertexType()
{
	return m_byVertexType;
}

void C3DLinearRing::setVertNum(int nVertNum)
{
	m_nVertNum=nVertNum;
}
int C3DLinearRing::getVertNum()
{
	return m_nVertNum;
}

void C3DLinearRing::setInOrOutRing(byte bInOrOutRing)
{
	m_bInOrOutRing=bInOrOutRing;
}
byte C3DLinearRing::getInOrOutRing()
{
	return m_bInOrOutRing;
}
	

size_t C3DLinearRing::sizeBuffer()
{
    size_t nSize = 0;

    nSize += C3DGeometry::sizeBuffer();
    nSize += sizeof(byte);//顶点类型编码
     nSize += sizeof(int); // 线环顶点数
    //线环顶点序列
    //*************************************************

    if(m_byVertexType == VERTEXTYPE_V3d)	
    {	
        nSize += Vertex3d::sizeBuffer() * m_nVertNum;
    }
    else if(m_byVertexType == VERTEXTYPE_V3f)
    {
        nSize += Vertex3f::sizeBuffer() * m_nVertNum;
    }
    else if(m_byVertexType == VERTEXTYPE_V3dT2d)
    {
        nSize += TexturedVertex3d::sizeBuffer() * m_nVertNum;
    }
    else if(m_byVertexType == VERTEXTYPE_V3fT2f)
    {
        nSize += TexturedVertex3f::sizeBuffer() * m_nVertNum;	
    }
    else if(m_byVertexType == VERTEXTYPE_V3dN3dC3f)
    {
        nSize += ShadedVertex3d::sizeBuffer() * m_nVertNum;
    }
    else if(m_byVertexType == VERTEXTYPE_V3fN3fC3f)
    {
        nSize += ShadedVertex3f::sizeBuffer() * m_nVertNum;
    }
    else if(m_byVertexType == VERTEXTYPE_V3dT2dN3dC3f)
    {
        nSize += VersatileVertex3d::sizeBuffer() * m_nVertNum;
    }
    else if(m_byVertexType == VERTEXTYPE_V3fT2fN3fC3f)
    {
        nSize += VersatileVertex3f::sizeBuffer() * m_nVertNum;
    }  
	else if(m_byVertexType == VERTEXTYPE_V3dT2dT2dN3dC3f)
	{
		nSize += SecondVersatileVertex3d::sizeBuffer() * m_nVertNum;
	}
	else if(m_byVertexType == VERTEXTYPE_V3fT2fT2fN3fC3f)
	{
		nSize += SecondVersatileVertex3f::sizeBuffer() * m_nVertNum;
	} 
	else if(m_byVertexType == VERTEXTYPE_V3dT2dT2dT2dN3dC3f)
	{
		nSize += ThirdVersatileVertex3d::sizeBuffer() * m_nVertNum;
	}
	else if(m_byVertexType == VERTEXTYPE_V3fT2fT2fT2fN3fC3f)
	{
		nSize += ThirdVersatileVertex3d::sizeBuffer() * m_nVertNum;
	} 
    nSize += sizeof(byte); //内外环标志


    //*************************************************

    return nSize;
}