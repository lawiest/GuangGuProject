#include "EventAdapter.h"
#include <osg/ref_ptr>
#include <tool/GeoGlobleEvent.h>
#include <FeatureClassLayer.h>

GLOBE_NS

EventAdapter::EventAdapter()
{
	
}

void EventAdapter::AddLayer(Layer* ptrLayer)
{
	osg::ref_ptr<GLOBE_NAME::GeoGlobleEventAdapter> ptrUserEvenAdapter = new GLOBE_NAME::GeoGlobleEventAdapter();
	ptrUserEvenAdapter->setEventType(GLOBE_NAME::ADD_LAYER);
	ptrUserEvenAdapter->setLayer(ptrLayer);
	GLOBE_NAME::OsgCoreSingleton::Instance()->OsgViewer()->getEventQueue()->userEvent(ptrUserEvenAdapter);
}

void AddElement(Element* ptrElement)
{
	osg::ref_ptr<GLOBE_NAME::GeoGlobleEventAdapter> ptrUserEvenAdapter = new GLOBE_NAME::GeoGlobleEventAdapter();
	ptrUserEvenAdapter->setEventType(GLOBE_NAME::ADD_ELEMENT);
	ptrUserEvenAdapter->setElement(ptrElement);
	GLOBE_NAME::OsgCoreSingleton::Instance()->OsgViewer()->getEventQueue()->userEvent(ptrUserEvenAdapter);
}

EventAdapter::~EventAdapter()
{

}

GLOBE_ENDNS