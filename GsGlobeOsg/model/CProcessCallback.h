#pragma once
#include <Model/CT3D_Geometry.h>
#include <Model/CT3D_3DGroup.h>
#include <Model/CT3D_3DSolid.h>
#include <Model/CT3D_3DSurface.h>
#include <Model/CT3D_3DTriangleMesh.h>
#include <Model/CT3D_3DModelObject.h>
#pragma region 处理几何数据

//拆分几何信息的回调方法
class CProcessCallback
{
public:
	virtual void SpliteOne(CGeometry* pGeometry) = 0;
};

//拆分几何信息
class CProcessGeometry
{
	CProcessCallback* m_pDoSplite;
	CGeometry* m_pGeometry;

public:
	CProcessGeometry(CGeometry* pGeom, CProcessCallback* pCallback)
	{
		SetCallBack(pCallback);
		SetGeometry(pGeom);
	}

	vector<D3dxMaterialParam> m_vecD3dxMaterialParam;
	unsigned long* m_pAdjacencyTable;
	long m_nAdjacencyTableCount;


	~CProcessGeometry()
	{
		m_vecD3dxMaterialParam.clear();
	}

	void DoSplite();

protected:
	void SetCallBack(CProcessCallback* pDoSplite);
	void SetGeometry(CGeometry* pGeometry);

protected:
	void SpliteGeometry(CGeometry* pGeometry);
	void SpliteGroup(C3DGroup* pGroup);
	void SpliteSolid(C3DSolid* pSolid);
	void SpliteSurface(C3DSurface* pSurface);
	void SpliteTriangleMesh(C3DTriangleMesh* pTriangleMesh);
	void SpliteModelObj(C3DModelObject* pModelObject);
};

class CProcessDraw : public CProcessCallback
{
public:
	virtual void SpliteOne(CGeometry* pGeometry) = 0;
};

template<class T>
class CProecssGetNode : public CProcessCallback
{
	std::vector<T*>* m_pVecNode;
public:
	CProecssGetNode(std::vector<T*>* pVecNode )
	{
		m_pVecNode = pVecNode;
	}

	virtual void SpliteOne(CGeometry* pGeometry)
	{
		T* pMesh = dynamic_cast<T*>(pGeometry);
		if (pMesh != NULL)
			m_pVecNode->push_back(pMesh);
	}
};

#pragma endregion 处理几何数据