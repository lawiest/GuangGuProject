#include "FeatureClassLayer.h"
#include "osgEarth/GeoData"
#include "base/ImageTileSource.h"
#include <osgEarth/ImageLayer>
#include "innerimp.h"
#include <OsgCoreSingleton.h>
#include <osgEarth/MapNode>
#include <tool/CodeTransformHelp.h>

GLOBE_NS

FeatureClassLayer::FeatureClassLayer(std::string strUrl, StoreType eSerType, std::string strFont , bool bCoverAutoHide, QueryElevationCallback* ptrQueryElv, int nMinLod, int nMaxLod)
{
}
FeatureClassLayer::FeatureClassLayer(KERNEL_NAME::GsFeatureClass * ptrFeatureClass, std::string  layerName,
																					std::string  labeField, 
																					std::string strIcon,
																					std::string strFont, 
																					bool bCoverAutoHide, 
																					QueryElevationCallback * ptrQueryElv, 
																					int nMinLod, 
																					int nMaxLod)
{
	GsFeatureCursorPtr ptrCur = ptrFeatureClass->Search();
	GsFeaturePtr ptrFea = ptrCur->Next();
	m_QueryElv = ptrQueryElv;

	while (true)
	{
		if (NULL == ptrFea)
			break;

		GsGeometryPtr pGeo = ptrFea->Geometry();
		GsGeometryBlob* blob = pGeo->GeometryBlobPtr();
		double *pCorrd = blob->Coordinate();
		int Dms = blob->CoordinateDimension();

		double x = pCorrd[0];
		double y = pCorrd[1];
		double z = 0.0;
		if (Dms == 3)
		{
			z = pCorrd[2];
		}
		else if (Dms == 2 && NULL != m_QueryElv)
		{
			z = m_QueryElv->Query(x,y);
		}

		string strName = ptrFea->ValueString(ptrFeatureClass->Fields().FindField(labeField.c_str()));
		string strOID = std::to_string(ptrFea->OID());
		strName = CodeHelp::UTF8_To_string(strName);
		string strLayerName = ptrFeatureClass->Name();

		osg::Vec3d vecPos = osg::Vec3d(x, y, z);
		GLOBE_NAME::OsgCoreSingleton::Instance()->CorrectPosByOblique(vecPos);

		GeoBillBoardElement* ptrBillBord = new GeoBillBoardElement(vecPos.x(), vecPos.y(), vecPos.z(), strName, strIcon, -100, nMinLod, nMaxLod);
		ptrBillBord->CoverAutoHide(bCoverAutoHide);
		ptrBillBord->Font(strFont);
		ptrBillBord->setValue("LayerName", layerName);
		ptrBillBord->setValue("OID", strOID);
		ptrBillBord->setValue("x", std::to_string(x));
		ptrBillBord->setValue("y", std::to_string(y));
		ptrBillBord->setValue("z", std::to_string(z));

		m_vecBillBord.push_back(ptrBillBord);

		if (ptrCur->Next(ptrFea) == NULL)
			break;
	}

	GsBox box = ptrFeatureClass->Extent();
	Range(box.XMin, box.XMax, box.YMin, box.YMax);

	m_LayerType = eFeatureClassLayer;
}

int FeatureClassLayer::UID()
{
	return 0;
}

void FeatureClassLayer::Filter(std::string strFilter)
{
	std::vector<GeoBillBoardElementPtr>::iterator it = m_vecBillBord.begin();
	for (it; it != m_vecBillBord.end(); it++)
	{
		std::string strName;
		(*it)->getValue(std::string("name"), strName);
		if (strName.find("strFilter") == std::string::npos)
			(*it)->Visible(false);
		else
			(*it)->Visible(true);
	}
}

FeatureClassLayer::~FeatureClassLayer()
{
}

void FeatureClassLayer::Visible(bool bVisible)
{
	std::vector<GeoBillBoardElementPtr>::iterator it = m_vecBillBord.begin();
	for (it; it != m_vecBillBord.end(); it++)
	{
		(*it)->Visible(bVisible);
	}
}

std::vector<GeoBillBoardElementPtr>& FeatureClassLayer::getAllElement()
{
	return m_vecBillBord;
}

bool FeatureClassLayer::Visible() const
{
	return m_vecBillBord[0]->Visible();
}

GLOBE_ENDNS