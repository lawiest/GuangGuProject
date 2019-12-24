//********************************************************************************** 
//** 文件名称：CT3D_3DMultiPoint.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维多点类实现
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-13
//********************************************************************************** 

#include "CT3D_3DMultiPoint.h"
#include "CT3D_Vertex3d.h"
#include "CT3D_Vertex3f.h"
#include "CT3D_TexturedVertex3d.h"
#include "CT3D_TexturedVertex3f.h"
#include "CT3D_ShadedVertex3d.h"
#include "CT3D_ShadedVertex3f.h"
#include "CT3D_ThirdVersatileVertex3d.h"
#include "CT3D_ThirdVersatileVertex3f.h"
#include "CT3D_AABBox.h"
#include "Buffer.h"

//********************************************************************************** 
//** 函数名称： GetVerticesEx
//** 功能描述： 获得三维多点顶点数据
//** 输    入： 三位顶点基类集合vt3d           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
void C3DMultiPoint::GetVerticesEx(vector<Vertex3d>& vt3d) const
{
	vt3d.reserve(m_lVertNum);
	if(m_byVertexType == VERTEXTYPE_V3d)
	{
		for(int i=0;i<m_lVertNum;i++)
		{
			Vertex3d tempVt = ((Vertex3d*)m_vVertices)[i];
			vt3d.push_back(tempVt);
		}
	}

	else if(m_byVertexType == VERTEXTYPE_V3f)
	{
		for(int i=0;i<m_lVertNum;i++)
		{
			Vertex3f tempVt = ((Vertex3f*)m_vVertices)[i];	
			vt3d.push_back(Vertex3d(tempVt.x,tempVt.y,tempVt.z));
		}
	}

	else if(m_byVertexType == VERTEXTYPE_V3fT2f)
	{
		for(int i=0;i<m_lVertNum;i++)
		{
			TexturedVertex3f tempVt = ((TexturedVertex3f*)m_vVertices)[i];	
			vt3d.push_back(Vertex3d(tempVt.x,tempVt.y,tempVt.z));
		}
	}

	else if(m_byVertexType == VERTEXTYPE_V3dT2d)
	{
		for(int i=0;i<m_lVertNum;i++)
		{
			TexturedVertex3d tempVt = ((TexturedVertex3d*)m_vVertices)[i];	
			vt3d.push_back(Vertex3d(tempVt.x,tempVt.y,tempVt.z));
		}
	}

	else if(m_byVertexType == VERTEXTYPE_V3dN3dC3f)
	{
		for(int i=0;i<m_lVertNum;i++)
		{
			ShadedVertex3d tempVt = ((ShadedVertex3d*)m_vVertices)[i];	
			vt3d.push_back(Vertex3d(tempVt.x,tempVt.y,tempVt.z));
		}
	}

	else if(m_byVertexType == VERTEXTYPE_V3fN3fC3f)
	{
		for(int i=0;i<m_lVertNum;i++)
		{
			ShadedVertex3f tempVt = ((ShadedVertex3f*)m_vVertices)[i];	
			vt3d.push_back(Vertex3d(tempVt.x,tempVt.y,tempVt.z));
		}
	}

	else if(m_byVertexType == VERTEXTYPE_V3fT2fN3fC3f)
	{
		for(int i=0;i<m_lVertNum;i++)
		{
			VersatileVertex3f tempVt = ((VersatileVertex3f*)m_vVertices)[i];	
			vt3d.push_back(Vertex3d(tempVt.x,tempVt.y,tempVt.z));
		}
	}

	else if(m_byVertexType == VERTEXTYPE_V3dT2dN3dC3f)
	{
		for(int i=0;i<m_lVertNum;i++)
		{
			VersatileVertex3d tempVt = ((VersatileVertex3d*)m_vVertices)[i];	
			vt3d.push_back(Vertex3d(tempVt.x,tempVt.y,tempVt.z));
		}
	}

	else if(m_byVertexType == VERTEXTYPE_V3dT2dT2dN3dC3f)
	{
		for(int i=0;i<m_lVertNum;i++)
		{
			SecondVersatileVertex3d tempVt = ((SecondVersatileVertex3d*)m_vVertices)[i];	
			vt3d.push_back(Vertex3d(tempVt.x,tempVt.y,tempVt.z));
		}
	}

	else if(m_byVertexType == VERTEXTYPE_V3fT2fT2fN3fC3f)
	{
		for(int i=0;i<m_lVertNum;i++)
		{
			ThirdVersatileVertex3f tempVt = ((ThirdVersatileVertex3f*)m_vVertices)[i];	
			vt3d.push_back(Vertex3d(tempVt.x,tempVt.y,tempVt.z));
		}
	}
}

//********************************************************************************** 
//** 函数名称： CreateObject
//** 功能描述： 开辟一个当前类对象所需的内存空间
//** 输    入： 无           
//** 输    出： 无
//** 返回值：	开辟的当前类对象内存空间的首地址
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
CGeometry* C3DMultiPoint::CreateObject()
{
	return new C3DMultiPoint;
}

//********************************************************************************** 
//** 函数名称： readBuffer
//** 功能描述： 从缓冲区中解析当前对象，并给成员变量赋值
//** 输    入： buf	缓冲区的引用           
//** 输    出： 无
//** 返回值：	成功为true，失败为false
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
bool C3DMultiPoint::readBuffer( Buffer & buf)
{
    bool bRet = C3DPoint::readBuffer(buf);
	//modify by yangling 20131208 begin
	if(!bRet)
	{
		return false;
	}
	//modify by yangling 20131208 end

	//专有信息******************************
	buf.read(&m_byVertexType, sizeof(unsigned char));
	//modify by yangling 20131208 begin
	if(m_byVertexType >= VERTEXTYPE_End || m_byVertexType < VERTEXTYPE_V3f)
	{
		return false;
	}
	//modify by yangling 20131208 end
	buf.read((char*)&m_lVertNum,sizeof(long));
    if(m_byVertexType == VERTEXTYPE_V3d)	
    {
        m_vVertices = new Vertex3d[m_lVertNum];
        Vertex3d* pList = (Vertex3d*)m_vVertices;
        //用for循环避免字节对齐问题
        for (int nNum=0; nNum < m_lVertNum; nNum++)
        {
            buf.read(&pList[nNum].x,sizeof(double));
            buf.read(&pList[nNum].y,sizeof(double));
            buf.read(&pList[nNum].z,sizeof(double));
        }
        pList = NULL;
    }
    else if(m_byVertexType == VERTEXTYPE_V3f) 
    {
        m_vVertices = new Vertex3f[m_lVertNum];
        Vertex3f* pList = (Vertex3f*)m_vVertices;
        //用for循环避免字节对齐问题
        for (int nNum=0; nNum < m_lVertNum; nNum++)
        {
            buf.read(&pList[nNum].x,sizeof(float));
            buf.read(&pList[nNum].y,sizeof(float));
            buf.read(&pList[nNum].z,sizeof(float));
        }
        pList = NULL;
    }
    else if(m_byVertexType == VERTEXTYPE_V3dT2d)
    {
        m_vVertices = new TexturedVertex3d[m_lVertNum];
        TexturedVertex3d* pList = (TexturedVertex3d*)m_vVertices;
        //用for循环避免字节对齐问题
        for (int nNum=0; nNum < m_lVertNum; nNum++)
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
        m_vVertices = new TexturedVertex3f[m_lVertNum];
        TexturedVertex3f* pList = (TexturedVertex3f*)m_vVertices;
        //用for循环避免字节对齐问题
        for (int nNum=0; nNum < m_lVertNum; nNum++)
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
        m_vVertices = new ShadedVertex3d[m_lVertNum];
        ShadedVertex3d* pList = (ShadedVertex3d*)m_vVertices;
        //用for循环避免字节对齐问题
        for (int nNum=0; nNum < m_lVertNum; nNum++)
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
        m_vVertices = new ShadedVertex3f[m_lVertNum];
        ShadedVertex3f* pList = (ShadedVertex3f*)m_vVertices;
        //用for循环避免字节对齐问题
        for (int nNum=0; nNum < m_lVertNum; nNum++)
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
        m_vVertices = new VersatileVertex3d[m_lVertNum];
        VersatileVertex3d* pList=NULL;
        pList = (VersatileVertex3d*)m_vVertices;
        //用for循环避免字节对齐问题
        for (int nNum=0; nNum < m_lVertNum; nNum++)
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
        m_vVertices = new VersatileVertex3f[m_lVertNum];
        VersatileVertex3f* pList=NULL;
        pList = (VersatileVertex3f*)m_vVertices;
        //用for循环避免字节对齐问题
        for (int nNum=0; nNum < m_lVertNum; nNum++)
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
		m_vVertices = new SecondVersatileVertex3d[m_lVertNum];
		SecondVersatileVertex3d* pList=NULL;
		pList = (SecondVersatileVertex3d*)m_vVertices;
		for (int nNum=0; nNum < m_lVertNum; nNum++)
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
		m_vVertices = new SecondVersatileVertex3f[m_lVertNum];
		SecondVersatileVertex3f* pList=NULL;
		pList = (SecondVersatileVertex3f*)m_vVertices;
		for (int nNum=0; nNum < m_lVertNum; nNum++)
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
		m_vVertices = new ThirdVersatileVertex3d[m_lVertNum];
		ThirdVersatileVertex3d* pList=NULL;
		pList = (ThirdVersatileVertex3d*)m_vVertices;
		for (int nNum=0; nNum < m_lVertNum; nNum++)
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
		m_vVertices = new ThirdVersatileVertex3f[m_lVertNum];
		ThirdVersatileVertex3f* pList=NULL;
		pList = (ThirdVersatileVertex3f*)m_vVertices;
		for (int nNum=0; nNum < m_lVertNum; nNum++)
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
	return true;
}

//********************************************************************************** 
//** 函数名称： writeBuffer
//** 功能描述： 把当前对象的成员变量数据，打包成一个缓冲区
//** 输    入： buf	缓冲区的引用           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
void C3DMultiPoint::writeBuffer(Buffer& buf)
{
    C3DPoint::writeBuffer(buf);

	//专有信息******************************	
	buf.write(&m_byVertexType, sizeof(unsigned char));
	buf.write((char*)&m_lVertNum,sizeof(long));


    if(m_byVertexType == VERTEXTYPE_V3d)	
    {
        Vertex3d* pList = (Vertex3d*)m_vVertices;
        //用for循环避免字节对齐问题
        for (int nNum=0; nNum < m_lVertNum; nNum++)
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
        for (int nNum=0; nNum < m_lVertNum; nNum++)
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
        for (int nNum=0; nNum < m_lVertNum; nNum++)
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
        for (int nNum=0; nNum < m_lVertNum; nNum++)
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
        for (int nNum=0; nNum < m_lVertNum; nNum++)
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
        for (int nNum=0; nNum < m_lVertNum; nNum++)
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
        for (int nNum=0; nNum < m_lVertNum; nNum++)
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
        for (int nNum=0; nNum < m_lVertNum; nNum++)
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
		for (int nNum=0; nNum < m_lVertNum; nNum++)
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
		for (int nNum=0; nNum < m_lVertNum; nNum++)
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
		for (int nNum=0; nNum < m_lVertNum; nNum++)
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
		for (int nNum=0; nNum < m_lVertNum; nNum++)
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
    else
    {
        return;
    }

}


size_t C3DMultiPoint::sizeBuffer()
{
	size_t nSize = 0;
	nSize += C3DPoint::sizeBuffer();
	nSize += sizeof(byte);//顶点类型编码
	nSize += sizeof(long);//顶点数
	//三维多点顶点序列
	//*************************************************

	if(m_byVertexType == VERTEXTYPE_V3d)	
	{	
		nSize += Vertex3d::sizeBuffer() * GetVerticesNum();
	}
	else if(m_byVertexType == VERTEXTYPE_V3f)
	{
		nSize += Vertex3f::sizeBuffer() * GetVerticesNum();
	}
	else if(m_byVertexType == VERTEXTYPE_V3dT2d)
	{
		nSize += TexturedVertex3d::sizeBuffer() * GetVerticesNum();
	}
	else if(m_byVertexType == VERTEXTYPE_V3fT2f)
	{
		nSize += TexturedVertex3f::sizeBuffer() * GetVerticesNum();	
	}
	else if(m_byVertexType == VERTEXTYPE_V3dN3dC3f)
	{
		nSize += ShadedVertex3d::sizeBuffer() * GetVerticesNum();
	}
	else if(m_byVertexType == VERTEXTYPE_V3fN3fC3f)
	{
		nSize += ShadedVertex3f::sizeBuffer() * GetVerticesNum();
	}
	else if(m_byVertexType == VERTEXTYPE_V3dT2dN3dC3f)
	{
		nSize += VersatileVertex3d::sizeBuffer() * GetVerticesNum();
	}
	else if(m_byVertexType == VERTEXTYPE_V3fT2fN3fC3f)
	{
		nSize += VersatileVertex3f::sizeBuffer() * GetVerticesNum();
	}  

	return nSize;
}

