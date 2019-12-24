#include "LayerBox.h"
#include <osg/Node>
#include "ImageLayer.h"
#include "TerrainLayer.h"
#include "PlaceNameLayer.h"
#include "WFSLayer.h"
#include "FeatureClassLayer.h"
#include "PlaceName/PlaceNameLayerPrivate.h"
#include <osgEarth/MapNode>
#include "ObliqueImageLayer.h"
#include <osgEarth/GeoTransform>
#include <osgEarth/ImageLayer>
#include <osgEarth/ModelLayer>
#include "OsgCoreSingleton.h"

GLOBE_NS

LayerBox::LayerBox()
{

}

Layer* LayerBox::LayerByUID(int nUID)
{
	osgEarth::MapNode* ptrMapNode =  OsgCoreSingleton::Instance()->MapNode();

	if (NULL == ptrMapNode)
		return NULL;

	osgEarth::ImageLayer* osgImageLayer = dynamic_cast<osgEarth::ImageLayer*>(ptrMapNode->getMap()->getLayerByUID(nUID));

	for (std::vector<LayerPtr>::iterator it = m_vecLayer.begin();it!= m_vecLayer.end();it++)
	{
		osg::ref_ptr<osgEarth::ImageLayer> ptrImageLayer = (*it)->Handle()->CastTo<osg::ref_ptr<osgEarth::ImageLayer>>();

		if (ptrImageLayer.get() == osgImageLayer)
			return (*it);
	}

	return NULL;
}

Layer* LayerBox::ImageAt(int nIndex)
{
	if ( nIndex<0 && nIndex >= m_vecLayer.size() )
		return NULL;

	return m_vecLayer[nIndex].p;
}

int LayerBox::ImageCount()
{
	osgEarth::MapNode* ptrMapNode = OsgCoreSingleton::Instance()->MapNode();

	if (NULL == ptrMapNode)
		return -1;

	return dynamic_cast<osgEarth::MapNode*>(ptrMapNode)->getMap()->getNumLayers();
}

int LayerBox::TerrainCount()
{
	osgEarth::MapNode* ptrMapNode = OsgCoreSingleton::Instance()->MapNode();

	if (NULL == ptrMapNode)
		return -1;

	return dynamic_cast<osgEarth::MapNode*>(ptrMapNode)->getMap()->getNumLayers();
}

void LayerBox::AddLayer(Layer* layer)
{
	osgEarth::MapNode* ptrMapNode = OsgCoreSingleton::Instance()->MapNode();

	if (NULL == layer || NULL == ptrMapNode)
		return;

	switch (layer->LayerType())
	{
	case eImage:
		{
			ImageLayerPtr imageLayer = dynamic_cast<ImageLayer*>(layer);

			if (imageLayer)
			{
				osg::ref_ptr<osgEarth::ImageLayer> ptrImageLayer = imageLayer->Handle()->CastTo<osg::ref_ptr<osgEarth::ImageLayer> >();

				ptrMapNode->getMap()->addLayer(ptrImageLayer.get());
				m_vecLayer.push_back(layer);
			}
		}
		break;
	case eTerrain:
		{
			TerrainLayerPtr terrainLayer = dynamic_cast<TerrainLayer*>(layer);

			if (terrainLayer)
			{
				osg::ref_ptr<osgEarth::ElevationLayer> ptrTerrainLayer = terrainLayer->Handle()->CastTo<osg::ref_ptr<osgEarth::ElevationLayer> >();

				ptrMapNode->getMap()->addLayer(ptrTerrainLayer.get());
				m_vecLayer.push_back(layer);
			}
		}
		break;
	case ePlanceName:
		{
			PlaceNameLayerPtr placeNameLayer = dynamic_cast<PlaceNameLayer*>(layer);

			if (placeNameLayer)
			{
				osg::ref_ptr<PlaceNameLayerPrivate> ptrplaceNameLayerPrivate = placeNameLayer->Handle()->CastTo<osg::ref_ptr<PlaceNameLayerPrivate> >();

				ptrMapNode->addChild(ptrplaceNameLayerPrivate.get());
				m_vecLayer.push_back(layer);
			}
		}
		break;
	case eVector:
		{
			if (layer->StorageType() == eWFS)
			{
				WFSLayerPtr wfsLayer = dynamic_cast<WFSLayer*>(layer);
				  
				if (wfsLayer)
				{
					osg::ref_ptr<osgEarth::ModelLayer> ptrWFSLayerPrivate = wfsLayer->Handle()->CastTo<osg::ref_ptr<osgEarth::ModelLayer> >();
					ptrMapNode->getMap()->addLayer(ptrWFSLayerPrivate);

					m_vecLayer.push_back(layer);
				}
			}
		}
		break;
	case eOPModel:
		{
			ObliqueImageLayerPtr ptrOPLayer = dynamic_cast<ObliqueImageLayer*>(layer);

			if (ptrOPLayer)
			{
				osg::ref_ptr<osgEarth::GeoTransform> ptrOsgLayer = ptrOPLayer->Handle()->CastTo<osg::ref_ptr<osgEarth::GeoTransform> >();
				ptrMapNode->addChild(ptrOsgLayer);

				m_vecLayer.push_back(layer);
			}
		}
		break;
	case eFeatureClassLayer:
		{
			FeatureClassLayerPtr ptrFeatureClassLayer = dynamic_cast<FeatureClassLayer*>(layer);

			if (ptrFeatureClassLayer)
			{
				std::vector<GeoBillBoardElementPtr> vecElement= ptrFeatureClassLayer->getAllElement();

				std::vector<GeoBillBoardElementPtr>::iterator it = vecElement.begin();
				for (it; it != vecElement.end(); it++)
				{
					osg::ref_ptr<osg::Node> ptrNode = (*it)->LodHandle()->CastTo<osg::ref_ptr<osg::Node> >();

					OsgCoreSingleton::Instance()->RTTPickGroup()->addChild(ptrNode);
					//ptrMapNode->addChild(ptrNode);
				}

				m_vecLayer.push_back(layer);
			}
		}
	break;
	}
}

void LayerBox::RemoveLayer(Layer* layer)
{
	osgEarth::MapNode* ptrMapNode = OsgCoreSingleton::Instance()->MapNode();

	if (NULL == layer || NULL == ptrMapNode)
		return;

	std::vector<LayerPtr>::iterator it = std::find(m_vecLayer.begin(), m_vecLayer.end(), layer);

	if (it != m_vecLayer.end())
	{
		switch (layer->LayerType())
		{
		case eImage:
		{
			ImageLayerPtr imageLayer = dynamic_cast<ImageLayer*>(layer);

			if (imageLayer)
			{
				osg::ref_ptr<osgEarth::ImageLayer> ptrImageLayer = imageLayer->Handle()->CastTo<osg::ref_ptr<osgEarth::ImageLayer> >();

				ptrMapNode->getMap()->removeLayer(ptrImageLayer.get());
				m_vecLayer.erase(it);
			}
		}
		break;
		case eTerrain:
		{
			TerrainLayerPtr terrainLayer = dynamic_cast<TerrainLayer*>(layer);

			if (terrainLayer)
			{
				osg::ref_ptr<osgEarth::ElevationLayer> ptrTerrainLayer = terrainLayer->Handle()->CastTo<osg::ref_ptr<osgEarth::ElevationLayer> >();

				ptrMapNode->getMap()->removeLayer(ptrTerrainLayer.get());
				m_vecLayer.erase(it);
			}
		}
		break;
		case ePlanceName:
		{
			PlaceNameLayerPtr terrainLayer = dynamic_cast<PlaceNameLayer*>(layer);

			if (terrainLayer)
			{
				osg::ref_ptr<PlaceNameLayerPrivate> ptrPlaceNamePrivate = terrainLayer->Handle()->CastTo<osg::ref_ptr<PlaceNameLayerPrivate> >();

				ptrMapNode->removeChild(ptrPlaceNamePrivate.get());
				m_vecLayer.erase(it);
			}
		}
		break;
		case eVector:
		{
			if (layer->StorageType() == eWFS)
			{
				WFSLayerPtr wfsLayer = dynamic_cast<WFSLayer*>(layer);

				if (wfsLayer)
				{
					osg::ref_ptr<osgEarth::ModelLayer> ptrWFSLayerPrivate = wfsLayer->Handle()->CastTo<osg::ref_ptr<osgEarth::ModelLayer> >();

					ptrMapNode->getMap()->removeLayer(ptrWFSLayerPrivate);

					m_vecLayer.erase(it);
				}
			}
		}
		break;
		case eOPModel:
		{
			ObliqueImageLayerPtr ptrOPLayer = dynamic_cast<ObliqueImageLayer*>(layer);

			if (ptrOPLayer)
			{
				osg::ref_ptr<osgEarth::GeoTransform> ptrOsgLayer = ptrOPLayer->Handle()->CastTo<osg::ref_ptr<osgEarth::GeoTransform> >();
				ptrMapNode->removeChild(ptrOsgLayer);
				m_vecLayer.erase(it);
			}
		}
		break;
		case eFeatureClassLayer:
		{
			FeatureClassLayerPtr ptrFeatureClassLayer = dynamic_cast<FeatureClassLayer*>(layer);

			if (ptrFeatureClassLayer)
			{
				std::vector<GeoBillBoardElementPtr> vecElement = ptrFeatureClassLayer->getAllElement();

				std::vector<GeoBillBoardElementPtr>::iterator billit = vecElement.begin();
				for (billit; billit != vecElement.end(); billit++)
				{
					osg::ref_ptr<osg::Node> ptrNode = (*billit)->LodHandle()->CastTo<osg::ref_ptr<osg::Node> >();
					OsgCoreSingleton::Instance()->RTTPickGroup()->removeChild(ptrNode);
					//ptrMapNode->removeChild(ptrNode);
				}

				m_vecLayer.erase(it);
			}
		}
		}
	}
}


void LayerBox::RemoveAllLayer()
{
	RemoveAllImageLayer();
	RemoveAllTerrainLayer();
}

void LayerBox::RemoveAllImageLayer()
{
	osgEarth::MapNode* ptrMapNode = OsgCoreSingleton::Instance()->MapNode();

	if (NULL == ptrMapNode)
		return;

	for (std::vector<LayerPtr>::iterator it = m_vecLayer.begin();it != m_vecLayer.end();it++)
	{
		osg::ref_ptr<osgEarth::ImageLayer> ptrImageLayer = (*it)->Handle()->CastTo<osg::ref_ptr<osgEarth::ImageLayer> >();

		if (ptrImageLayer)
			ptrMapNode->getMap()->removeLayer(ptrImageLayer.get());
	}
}

void LayerBox::RemoveAllTerrainLayer()
{

}

void LayerBox::MoveTo(Layer* layer, int nIndex)
{
	osgEarth::MapNode* ptrMapNode = OsgCoreSingleton::Instance()->MapNode();

	if (NULL == ptrMapNode)
		return;

	osgEarth::ImageLayer* ptrImageLayer = layer->Handle()->CastTo<osg::ref_ptr<osgEarth::ImageLayer> >();

	if (NULL == ptrImageLayer)
		return;

	int nLayerCnt = dynamic_cast<osgEarth::MapNode*>(ptrMapNode)->getMap()->getNumLayers();

	if (nIndex < 0)
		nIndex = 0;

	if (nIndex >= nLayerCnt)
		nIndex = nLayerCnt-1;

	ptrMapNode->getMap()->moveLayer(ptrImageLayer, nIndex);
}

LayerBox::~LayerBox()
{

}

GLOBE_ENDNS