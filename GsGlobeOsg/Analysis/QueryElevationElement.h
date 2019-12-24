#ifndef _QUERYELEVATIONELEMENT_
#define _QUERYELEVATIONELEMENT_

#include <osgEarth/MapNode>
#include <osgViewer/Viewer>
#include "osgGA/GUIEventHandler"
#include <LonAndLatElement.h>
#include <osgEarth/Viewpoint>

GLOBE_NS

class QueryElevationElement : public osgGA::GUIEventHandler
{
public:
	QueryElevationElement(LonAndLatElement* ptrLonAndLatEle);
	virtual ~QueryElevationElement();

	void CheckChange(osgEarth::Viewpoint viewpoint);
	UTILITY_NAME::GsDelegate<void()> Change;
	UTILITY_NAME::GsDelegate<void(KERNEL_NAME::GsRawPoint3D point)> MouseMovePoint;

protected:
	void update(osgViewer::View* view);
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	LonAndLatElementPtr m_ptrLonAndLatEle;
	osgEarth::Viewpoint m_preViewPoint;
};

GLOBE_ENDNS

#endif