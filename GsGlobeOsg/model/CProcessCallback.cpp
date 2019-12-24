#include <Model/CProcessCallback.h>

//////////////////////////////////////////////////////////////////////////
#pragma region 辅助工具

void CProcessGeometry::SetCallBack(CProcessCallback* pDoSplite)
{
	m_pDoSplite = pDoSplite;
}
void CProcessGeometry::SetGeometry(CGeometry* pGeometry)
{
	m_pGeometry = pGeometry;
}

void CProcessGeometry::DoSplite()
{
	if (NULL == m_pGeometry || NULL == m_pDoSplite)
		return;

	SpliteGeometry(m_pGeometry);
}

void CProcessGeometry::SpliteGeometry(CGeometry* pGeometry)
{
	if (pGeometry == NULL)
		return;

	switch (pGeometry->getTypeID())
	{
		//组
	case GEOTYPE_3D_GROUP:
		SpliteGroup(dynamic_cast<C3DGroup*>(pGeometry));
		break;

	case GEOTYPE_3D_SOLID:
		SpliteSolid(dynamic_cast<C3DSolid*>(pGeometry));
		break;
	case GEOTYPE_3D_MODELOBJECT:
		SpliteModelObj(dynamic_cast<C3DModelObject*>(pGeometry));
		break;
		//面类型
	case GEOTYPE_3D_SURFACE:
		SpliteSurface(dynamic_cast<C3DSurface*>(pGeometry));
		break;

		//绘制三角网
	case GEOTYPE_3D_TRIANGLEMESH:
		//SpliteTriangleMesh(dynamic_cast<C3DTriangleMesh*>(pGeometry));
		m_pDoSplite->SpliteOne(pGeometry);
		break;

	case GEOTYPE_3D_REFERENCE:			//三维参考 暂时不实现
		m_pDoSplite->SpliteOne(pGeometry);
		break;

	case GEOTYPE_3D_GEOMETRY:			//基类
	case GEOTYPE_3D_CURVE:				//三维曲线	数据结构中没有绘制信息
	case GEOTYPE_3D_LOFTING:			//三维放样对象 数据结构中没有绘制信息
	case GEOTYPE_3D_FEATUREINFOGROUP:	//要素信息集合 暂时不实现

	case GEOTYPE_3D_POINT:				//点\抽象类型
	case GEOTYPE_3D_SINGLEPOINT:		//单点
	case GEOTYPE_3D_MULTIPOINT:			//多点
	case GEOTYPE_3D_LINEARRING:			//环线
	case GEOTYPE_3D_LINESTRING:			//三维折线
	case GEOTYPE_3D_POLYGON:			//多边形
	default:
		break;
	}

}

void CProcessGeometry::SpliteGroup(C3DGroup* pGroup)
{
	if (pGroup == NULL)
		return ;

	_GeometryList pGeoCollection;
	pGroup->GetCollection(pGeoCollection);
	int num = pGeoCollection.size();
	for (int j = 0; j < num; j++)
	{
		SpliteGeometry(pGeoCollection[j].second);
	}
}
void CProcessGeometry::SpliteSolid(C3DSolid* pSolid)
{
	if (pSolid == NULL)
		return;

	//对每一个面进行拆分
	_3DSurfaceList& surlist = pSolid->get3DSurfaceList();
	for ( size_t i = 0; i < surlist.size(); i++ )
	{
		SpliteSurface(surlist.at(i).second);
	}
}

void CProcessGeometry::SpliteModelObj(C3DModelObject* pModelObject)
{
	if (pModelObject == NULL)
		return;

	//对每一个面进行拆分
	_3DSurfaceList& surlist = pModelObject->get3DSurfaceList();
	for ( size_t i = 0; i < surlist.size(); i++ )
	{
		SpliteSurface(surlist.at(i).second);
	}
	pModelObject->GetMaterialParamVec(m_vecD3dxMaterialParam);
	pModelObject->GetAdjacencyTable(m_pAdjacencyTable, m_nAdjacencyTableCount);
}

void CProcessGeometry::SpliteSurface(C3DSurface* pSurface)
{
	SpliteGeometry(dynamic_cast<C3DGeometry*>(pSurface));
}