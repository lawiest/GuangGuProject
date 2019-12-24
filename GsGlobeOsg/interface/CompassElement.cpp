
#include "CompassElement.h"
#include "OsgCoreSingleton.h"
#include "innerimp.h"
#include <osgEarthUtil/Controls>
#include"Analysis/RotateImageHandler.h"
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>

GLOBE_NS

using namespace osgEarth::Util::Controls;


CompassElement::CompassElement()
{
	m_ElementType = eCompassElement;
}

void CompassElement::setRotation(double angle)
{
	if (NULL == m_ptrElementHandle)
		return ;

	osg::ref_ptr<ImageControl> ptrImageControl = m_ptrElementHandle->CastTo<osg::ref_ptr<ImageControl> >();

	if (ptrImageControl)
		ptrImageControl->setRotation(angle);
}

void CompassElement::Visible(bool bVisible)
{
	if (NULL == m_ptrElementHandle)
		return ;

	osg::ref_ptr<ImageControl> ptrImageControl = m_ptrElementHandle->CastTo<osg::ref_ptr<ImageControl> >();

	if (ptrImageControl)
		ptrImageControl->setVisible(bVisible);
}

bool CompassElement::Visible()
{
	if (NULL == m_ptrElementHandle)
		return false;

	osg::ref_ptr<ImageControl> ptrImageControl = m_ptrElementHandle->CastTo<osg::ref_ptr<ImageControl> >();

	if (ptrImageControl)
		return ptrImageControl->visible();

	return false;
}

CompassElement::~CompassElement()
{
	
}

void CompassElement::setPosition(double x, double y, double z)
{
	osg::ref_ptr<VBox> ptrVBoxControl = m_ptrBoxHandle->CastTo<osg::ref_ptr<VBox> >();
	if (ptrVBoxControl)
		ptrVBoxControl->setPosition(-30.0 + x, -30.0 + y);
}

void CompassElement::getPosition(double& x, double& y, double& z)
{
	osg::ref_ptr<VBox> ptrVBoxControl = m_ptrBoxHandle->CastTo<osg::ref_ptr<VBox> >();
	if (ptrVBoxControl)
	{
		x = ptrVBoxControl->x().get();
		x = ptrVBoxControl->y().get();
	}
}

void CompassElement::setAlignment(Alignment Horiz, Alignment Vert)
{
	osg::ref_ptr<VBox> ptrVBoxControl = m_ptrBoxHandle->CastTo<osg::ref_ptr<VBox> >();
	if (ptrVBoxControl)
	{
		ptrVBoxControl->setHorizAlign((Control::Alignment)Horiz);
		ptrVBoxControl->setVertAlign((Control::Alignment)Vert);
	}
}

void CompassElement::creatCompass()
{
	osg::ref_ptr<ControlCanvas> ptrControlCanvas = ControlCanvas::getOrCreate(OsgCoreSingleton::Instance()->OsgViewer());
	osg::ref_ptr<VBox> center = new VBox();
	center->setBorderColor(1, 1, 1, 0);
	center->setBackColor(.6, .5, .4, 0.0);
	center->setHorizAlign(Control::ALIGN_RIGHT);
	center->setVertAlign(Control::ALIGN_TOP);

	m_ptrBoxHandle = new ReferenceImp<VBox>(center);
	
	// Add an image:
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("./Resources/Image/Compass_Rose_Classic.png");
	if (image.valid())
	{
		osg::ref_ptr<ImageControl> ptrImageControl = new ImageControl(image.get());
		m_ptrElementHandle = new ReferenceImp<ImageControl>(ptrImageControl);

		ptrImageControl->setHorizAlign(Control::ALIGN_CENTER);
		ptrImageControl->setFixSizeForRotation(true);
		center->addControl(ptrImageControl.get());
	}

	ptrControlCanvas->addControl(center.get());
	OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(new RotateImageHandler(this));
}

void CompassElement::Build()
{
	creatCompass();
}

GLOBE_ENDNS