#ifndef _POINT_TRAKER_ELEMENT_
#define _POINT_TRAKER_ELEMENT_

#include "osgGA/GUIEventHandler"
#include "osgEarth/MapNode"
#include "osgEarthAnnotation/PlaceNode"
 

class PointTrakerElement: public osgGA::GUIEventHandler 
{
public:
	PointTrakerElement();
	virtual ~PointTrakerElement();

	bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );

private: 
	osgEarth::MapNode* m_mapNode;
	osg::ref_ptr<osgEarth::Annotation::PlaceNode> m_PlaceNode;
	bool m_bIsFirst;
	bool m_bIsFinished;
};

#endif
