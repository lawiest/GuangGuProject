#include "OverlayElement.h"
#include "OsgCoreSingleton.h"
#include "innerimp.h"
#include <osgearth/MapNode>
#include <tool/OverlayNode.h>
#include <ObliqueImageLayer.h>
#include <osgEarth/GeoTransform>
#include <TextureMovieElement.h>
#include <OsgCoreSingleton.h>

GLOBE_NS

GsReferencePtr OverlayElement::m_ptrOverlayNode = NULL;

OverlayElement::OverlayElement(Element* ptrOverLayElement, Layer* ptrBaseLayer)
{
	if (!ptrBaseLayer || ptrBaseLayer->LayerType() != eOPModel)
		return;
	 
	osg::Vec3 center(0.0f, 0.0f, 0.0f);
	float radius = 100.0f;
	float baseHeight = center.z() - radius*0.5;

	osg::ref_ptr<GeoGlobe::OverlayNode> overlayNode = new GeoGlobe::OverlayNode(GeoGlobe::OverlayNode::OBJECT_DEPENDENT_WITH_ORTHOGRAPHIC_OVERLAY);
	overlayNode->setContinuousUpdate(true);
	if (ptrOverLayElement != NULL)
		overlayNode->setOverlaySubgraph(ptrOverLayElement->Handle()->CastTo<osg::ref_ptr<osg::Node> >());

	m_ptrOverlayNode = new ReferenceImp<GeoGlobe::OverlayNode>(overlayNode);

	osg::ref_ptr<osgEarth::GeoTransform> ptrOsgbLocator = ptrBaseLayer->Handle()->CastTo<osg::ref_ptr<osgEarth::GeoTransform> >();
	overlayNode->setOverlayBaseHeight(baseHeight - 0.01);
	overlayNode->addChild(ptrOsgbLocator);

	//因为倾斜图层在之前加过一次，这里视频贴到倾斜上时又会加一次，需要把之前移除掉
	//OsgCoreSingleton::Instance()->MapNode()->removeChild(ptrOsgbLocator);

	if (ptrOverLayElement != NULL)
	{
		osg::ref_ptr<osgEarth::GeoTransform> ptrLocator =dynamic_cast<osgEarth::GeoTransform*>(ptrOverLayElement->Handle()->CastTo<osg::ref_ptr<osg::Node> >().get());

		//if (ptrLocator.valid())
		//	ptrLocator->update();
	}

	m_ElementType = eOverlayElement;
}

void OverlayElement::addOverlaySubgraph(Element* ptrElement)
{
	if (ptrElement != NULL)
	{
		osg::ref_ptr<osgEarth::GeoTransform> ptrLocator = dynamic_cast<osgEarth::GeoTransform*>(ptrElement->Handle()->CastTo<osg::ref_ptr<osg::Node> >().get());

		//if (ptrLocator.valid())
		//	ptrLocator->update();
	}

	if (m_ptrOverlayNode)
		m_ptrOverlayNode->CastTo<osg::ref_ptr<GeoGlobe::OverlayNode> >()->setOverlaySubgraph(ptrElement->Handle()->CastTo<osg::ref_ptr<osg::Node> >());
}

void OverlayElement::removeOverlaySubgraph(Element* ptrElement)
{
	if (m_ptrOverlayNode)
	{	
		osg::ref_ptr<osg::Node> ptrNode = ptrElement->Handle()->CastTo<osg::ref_ptr<osg::Node> >();
		//GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->removeChild(ptrNode);
		m_ptrOverlayNode->CastTo<osg::ref_ptr<GeoGlobe::OverlayNode> >()->removeOverlaySubgraph(ptrNode);
	}
}

void OverlayElement::setProjectDirection(double x, double y, double z)
{
	if (m_ptrOverlayNode)
		m_ptrOverlayNode->CastTo<osg::ref_ptr<GeoGlobe::OverlayNode> >()->setProjectDirection(osg::Vec3d(x, y, z));
}

void OverlayElement::addChild(osg::Node* ptrNode)
{
	if (m_ptrOverlayNode)
		m_ptrOverlayNode->CastTo<osg::ref_ptr<GeoGlobe::OverlayNode> >()->addChild(ptrNode);
}

void OverlayElement::removeChildren()
{
	if (m_ptrOverlayNode)
	{
		osg::ref_ptr<GeoGlobe::OverlayNode> ptrOverlayNode = m_ptrOverlayNode->CastTo<osg::ref_ptr<GeoGlobe::OverlayNode> >();
		ptrOverlayNode->removeChildren(0, ptrOverlayNode->getNumChildren());
	}
}

OverlayElement::~OverlayElement()
{

}

GLOBE_ENDNS
