#pragma once
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include "OsgCoreSingleton.h"
#include <Layer.h>
#include <Element.h>

GLOBE_NS

class Element;

enum GeoEventType
{
	ADD_NODE,
	REMOVE_NODE,
	ADD_OVERLAYNODE,
	ADD_LAYER,
	ADD_ELEMENT
};

class GeoGlobleEventAdapter : public osg::Referenced
{
public:
	GeoGlobleEventAdapter();

	void setEventType(GeoEventType eventType);
	void setData(osg::Node* ptrData);
	osg::Node* getData();

	void setLayer(Layer* ptrLayer);
	Layer* getLayer();

	void setElement(Element* ptrLayer);
	Element* getElement();

	GeoEventType EventType();

	virtual ~GeoGlobleEventAdapter() {};

private:
	osg::ref_ptr<osg::Node> m_ptrData;
	LayerPtr m_ptrLayer;
	ElementPtr m_ptrELement;
	GeoEventType m_EventType;
};

class GeoGlobleEventHandler :public osgGA::GUIEventHandler
{
public:
	GeoGlobleEventHandler();

	virtual ~GeoGlobleEventHandler();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

private:

};

GLOBE_ENDNS

