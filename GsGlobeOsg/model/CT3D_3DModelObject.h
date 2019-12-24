//********************************************************************************** 
//** 文件名称：CT3D_C3DSolid.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维体类的定义
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-11
//********************************************************************************** 

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#pragma warning(disable:4251)
#endif
#ifndef _CT3D_3DMODELOBJECT_H_
#define _CT3D_3DMODELOBJECT_H_

#include "ModelDef.h"
#include "CT3D_3DGeometry.h"
#include "CT3D_3DSurface.h"
#include <utility>
#include <vector>
#include <fstream>
using namespace std;

typedef pair<UINT,C3DSurface*> _3DSurfacePair;
typedef vector<_3DSurfacePair> _3DSurfaceList;//三维曲面列表

/** 关系表*/
typedef struct _D3dxMaterialParam
{
	unsigned long nAttribId;			//材质标识ID
	unsigned long nFaceStart;			//起始三角面标识ID
	unsigned long nFaceCount;			//三角面个数
	unsigned long nVertexStart;			//起始顶点标识ID
	unsigned long nVertexCount;			//顶点个数

	_D3dxMaterialParam()
	{
		nAttribId = 0;
		nFaceStart = 0;
		nFaceCount = 0;
		nVertexStart = 0;
		nVertexCount = 0;
	}
} D3dxMaterialParam;

class /*DLL_EXPORT*/ C3DModelObject 
: public C3DGeometry
{

private:    

    /** 面列表 */
	_3DSurfaceList m_3DSurfaceList;

	/** 关系表集合*/
	std::vector<D3dxMaterialParam> m_vecD3dxMaterialParam;

	/** 邻接表*/
	unsigned long* m_pAdjacencyTable;

	/**邻接表个数*/
	long m_lAdjacencyCount;

public:
	//********************************************************************************** 
	//** 函数名称： C3DModelObject
	//** 功能描述： 构造函数
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	C3DModelObject();

	//********************************************************************************** 
	//** 函数名称： ~C3DModelObject
	//** 功能描述： 析构函数
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	virtual ~C3DModelObject();
	

	//********************************************************************************** 
	//** 函数名称： readBuffer
	//** 功能描述： 将数据读入buf
	//** 输    入： Buffer对象buf的引用           
	//** 输    出： 无
	//** 返回值：	成功为true，失败为false
	//** 作    者： 李静
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	bool readBuffer( Buffer & buf);

	
	
	//********************************************************************************** 
	//** 函数名称： writeBuffer
	//** 功能描述： 将数据写入buf
	//** 输    入： Buffer对象buf的引用           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 李静
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	void writeBuffer(Buffer& buf);
	int getSurfaceNum();

	void set3DSurfaceList(const _3DSurfaceList& v3DSurfaceList);
	_3DSurfaceList& get3DSurfaceList();

	virtual size_t sizeBuffer();

	/** 获取关系表 */
 	void GetMaterialParamVec(vector<D3dxMaterialParam>& vecD3dxMaterialParam)
 	{
 		vecD3dxMaterialParam = m_vecD3dxMaterialParam;
 	}
 
 	/** 设置关系表 */
 	void SetMaterialParamVec(vector<D3dxMaterialParam>& vecD3dxMaterialParam)
 	{
 		m_vecD3dxMaterialParam = vecD3dxMaterialParam;
 	}

	/** 获取邻接表 */
	void GetAdjacencyTable(unsigned long*& pAdjacencyTable, long& lAdjacencyCount)
	{
		pAdjacencyTable = m_pAdjacencyTable;
		lAdjacencyCount = m_lAdjacencyCount;
	}

	/** 设置邻接表 */
	void SetAdjacencyTable(unsigned long* pAdjacencyTable, long lAdjacencyCount)
	{
		m_pAdjacencyTable = pAdjacencyTable;
		m_lAdjacencyCount = lAdjacencyCount;
	}
 
};

#endif 
