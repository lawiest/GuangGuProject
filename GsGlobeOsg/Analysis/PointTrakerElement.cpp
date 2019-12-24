#include "PointTrakerElement.h"
#include "osgViewer/Viewer"
#include "osgEarth/Geodata"
#include "tool/CodeTransformHelp.h"
#include "osgEarthSymbology/Style"
#include "osgEarthSymbology/Color" 
#include "tool/GGEarthManipulator.h"
#include <OsgCoreSingleton.h>

PointTrakerElement::PointTrakerElement()
{
	m_mapNode = GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode();
	m_bIsFirst = true;
	m_PlaceNode = NULL;
	m_bIsFinished = false;
}

PointTrakerElement::~PointTrakerElement()
{
}

bool PointTrakerElement::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{   
	if (m_bIsFinished)
		return false;

	if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
	{
		osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
		osg::Group* root = static_cast<osg::Group*>(view->getSceneData());

		osgEarth::GeoPoint mapPoint;
		osg::Vec3d world;
		osgUtil::LineSegmentIntersector::Intersections hits;
		if ( view->computeIntersections(ea.getX(), ea.getY(), hits) )
		{
			world = hits.begin()->getWorldIntersectPoint();
			// convert to map coords:
			mapPoint.fromWorld(m_mapNode->getTerrain()->getSRS(), world );
		}

		if(m_bIsFirst)
		{
			std::string na = CodeHelp::String_To_UTF8("请双击选取当前位置");
			osgEarth::Symbology::Style chapin;
			chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->size()=18.0f;
			//chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color()=osgEarth::Symbology::Color::White;
			chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color()=osgEarth::Symbology::Color::White;
			chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->font() = "msyh.ttc";
			chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
			m_PlaceNode = new osgEarth::Annotation::PlaceNode(mapPoint, na , chapin);
			root->addChild( m_PlaceNode);

			m_bIsFirst = false;
		}

		if (NULL != m_PlaceNode)
			m_PlaceNode->setPosition(mapPoint);
	} 
	else if (ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK)
	{
		osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
		GGEarthManipulator* manip = static_cast<GGEarthManipulator* >(view->getCameraManipulator());
		osgEarth::Viewpoint vp = manip->getViewpoint();

		osg::Group* root = static_cast<osg::Group*>(view->getSceneData());
		root->removeChild(m_PlaceNode);

		m_bIsFinished = true;
	}

	return false;
}
