#ifndef _ROTATEIMAGEHANDLER_
#define _ROTATEIMAGEHANDLER_

#include "osgGA/GUIEventHandler"
#include "CompassElement.h"

GLOBE_NS

class RotateImageHandler : public osgGA::GUIEventHandler
{
public:
	RotateImageHandler(CompassElement* ptrCompassEle);
	virtual ~RotateImageHandler();

protected:

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	
	CompassElementPtr m_ptrCompassEle;
};


GLOBE_ENDNS

#endif