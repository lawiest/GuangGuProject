
#include "PointSymbolElement3D.h"
#include "OsgCoreSingleton.h"
#include "innerimp.h"
#include "Analysis/SphereElement.h"

GLOBE_NS

PointSymbolElement3D::PointSymbolElement3D(double x, double y, double z)
{
	osg::ref_ptr<osgEarth::MapNode> ptrMapNode = GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode();
	osg::ref_ptr<GeoGlobe::SphereElement> ptrSphereEle = new GeoGlobe::SphereElement(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode());
	ptrSphereEle->setPosition(osgEarth::GeoPoint(ptrMapNode->getMapSRS(), x, y, z));
	ptrSphereEle->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON| osg::StateAttribute::OVERRIDE);
	//ptrSphereEle->getOrCreateStateSet()->setAttributeAndModes(new osg::Program, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	m_ptrElementHandle = new ReferenceImp<GeoGlobe::SphereElement>(ptrSphereEle);
	m_ElementType = ePointSymbolElement3D;
}

void PointSymbolElement3D::setColor(int r, int g, int b, float a )
{
	osg::ref_ptr<GeoGlobe::SphereElement> ptrSphereEle = Handle()->CastTo<osg::ref_ptr<GeoGlobe::SphereElement> >();

	if (ptrSphereEle)
		ptrSphereEle->setColor(osg::Vec4(r, g, b, a));
}

void PointSymbolElement3D::setSize(int nSize)
{
	osg::ref_ptr<GeoGlobe::SphereElement> ptrSphereEle = Handle()->CastTo<osg::ref_ptr<GeoGlobe::SphereElement> >();

	if (ptrSphereEle)
		ptrSphereEle->setSize(nSize/1.0);
}

void PointSymbolElement3D::setPosition(double x, double y, double z)
{
	osg::ref_ptr<osgEarth::MapNode> ptrMapNode = GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode();
	osg::ref_ptr<GeoGlobe::SphereElement> ptrSphereEle = Handle()->CastTo<osg::ref_ptr<GeoGlobe::SphereElement> >();

	if (ptrSphereEle)
		ptrSphereEle->setPosition(osgEarth::GeoPoint(ptrMapNode->getMapSRS(), x, y, z));
}

PointSymbolElement3D::~PointSymbolElement3D()
{
	
}

GLOBE_ENDNS