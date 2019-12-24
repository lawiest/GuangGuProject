#include "PlaceNameLayer.h"
#include "innerimp.h"
#include "PlaceName/PlaceNameLayerPrivate.h"

GLOBE_NS

PlaceNameLayer::PlaceNameLayer(std::string strUrl, StoreType eSerType, bool bAutoHide, float minLOD, float maxLOD)
{
	osg::ref_ptr<PlaceNameLayerOptions> ptrPlaceNameLayerOptions;

	if (eSerType == eLocal)
	{
		ptrPlaceNameLayerOptions = new PlaceNameLayerOptions(strUrl, "simkai.ttf");
		ptrPlaceNameLayerOptions->_minLOD = minLOD;
		ptrPlaceNameLayerOptions->_maxLOD = maxLOD;
		ptrPlaceNameLayerOptions->_bCoverAutoHide = bAutoHide;
	}
	else if(eSerType == eGeoGlobeTile)
	{
		UTILITY_NAME::GsString gsHashString = UTILITY_NAME::GsStringHelp::ToString(UTILITY_NAME::GsHash::Hash64(strUrl.c_str()));
		string strCachPath = "./cache/" + std::string(gsHashString.c_str() )+ ".tfg";

		ptrPlaceNameLayerOptions = new PlaceNameLayerOptions(strCachPath, "simkai.ttf", strUrl);
		ptrPlaceNameLayerOptions->_minLOD = minLOD;
		ptrPlaceNameLayerOptions->_maxLOD = maxLOD;
		ptrPlaceNameLayerOptions->_bCoverAutoHide = bAutoHide;
	}

	osg::ref_ptr<PlaceNameLayerPrivate> ptrPlaceNameLayer = new PlaceNameLayerPrivate(ptrPlaceNameLayerOptions.get());
	m_ptrLayerHandle = new ReferenceImp <PlaceNameLayerPrivate>(ptrPlaceNameLayer);

	Range(ptrPlaceNameLayerOptions->_west, ptrPlaceNameLayerOptions->_east, ptrPlaceNameLayerOptions->_north, ptrPlaceNameLayerOptions->_south);
	m_LayerType = ePlanceName;
	m_StoreType = eSerType;

	m_strLayerUrl = strUrl;
	m_nTopLevel = ptrPlaceNameLayerOptions->_minLevel;
	m_nBottomLevel = ptrPlaceNameLayerOptions->_maxLevel;
}

PlaceNameLayer::PlaceNameLayer(KERNEL_NAME::GsTileClass* ptrTileClass)
{

}

int PlaceNameLayer::UID()
{
	return 0;
}


PlaceNameLayer::~PlaceNameLayer()
{

}

void PlaceNameLayer::Visible(bool bVisible)
{
	m_ptrLayerHandle->CastTo<osg::ref_ptr<PlaceNameLayerPrivate>>()->Visible(bVisible);
}

bool PlaceNameLayer::Visible() const
{
	return m_ptrLayerHandle->CastTo<osg::ref_ptr<PlaceNameLayerPrivate>>()->Visible();
}

GLOBE_ENDNS