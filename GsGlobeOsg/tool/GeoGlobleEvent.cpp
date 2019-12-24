
#include "GeoGlobleEvent.h"

#include <tool/CodeTransformHelp.h>
#include <osgEarth/MapNode>
#include <OverlayElement.h>
#include <LayerBox.h>
#include <ElementBox.h>

GLOBE_NS

GeoGlobleEventAdapter::GeoGlobleEventAdapter()
{
}

void GeoGlobleEventAdapter::setEventType(GeoEventType eventType)
{
	m_EventType = eventType;
}

GeoEventType GeoGlobleEventAdapter::EventType()
{
	return m_EventType;
}

void GeoGlobleEventAdapter::setData( osg::Node* ptrData)
{
	m_ptrData = ptrData;
}

osg::Node* GeoGlobleEventAdapter::getData()
{
	return m_ptrData;
}

void GeoGlobleEventAdapter::setLayer(Layer* ptrLayer)
{
	m_ptrLayer = ptrLayer;
}

Layer* GeoGlobleEventAdapter::getLayer()
{
	return m_ptrLayer;
}

void GeoGlobleEventAdapter::setElement(Element* ptrLayer)
{
	m_ptrELement = ptrLayer;
}

Element* GeoGlobleEventAdapter::getElement()
{
	return m_ptrELement;
}

GeoGlobleEventHandler::GeoGlobleEventHandler()
{

}

GeoGlobleEventHandler::~GeoGlobleEventHandler()
{

}

bool GeoGlobleEventHandler::handle(const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & aa)
{
	switch (ea.getEventType())
	{
	case osgGA::GUIEventAdapter::FRAME:
		{
			
		}break;
	case osgGA::GUIEventAdapter::USER:
		{
			GeoGlobleEventAdapter* ptrGeoEventAdapter = (GeoGlobleEventAdapter*)dynamic_cast<const GeoGlobleEventAdapter*>(ea.getUserData());

			if(ptrGeoEventAdapter==NULL)
				break;

			switch (ptrGeoEventAdapter->EventType())
			{
				case ADD_OVERLAYNODE:
				{
					OverlayElement::addChild(ptrGeoEventAdapter->getData());
				}break;
				case ADD_NODE:
				{
					OsgCoreSingleton::Instance()->MapNode()->addChild(ptrGeoEventAdapter->getData());
				}break;
				case REMOVE_NODE:
				{
					OsgCoreSingleton::Instance()->MapNode()->removeChild(ptrGeoEventAdapter->getData());
				}break;
				case ADD_LAYER:
				{
					Layer* ptrLayer = ptrGeoEventAdapter->getLayer();
					OsgCoreSingleton::Instance()->getLayerBox()->AddLayer(ptrLayer);
				}break;
				case ADD_ELEMENT:
				{
					Element* ptrElement = ptrGeoEventAdapter->getElement();
					OsgCoreSingleton::Instance()->getElementBox()->AddElement(ptrElement);
				}break;
			}

		}break;
	default:
		break;
	}
	
	return false;
}

GLOBE_ENDNS