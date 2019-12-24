
#include "PointSymbolElement.h"
#include "OsgCoreSingleton.h"
#include "innerimp.h"
#include <osgEarth/GeoTransform>
#include <osgEarthSymbology/PointSymbol>
#include <osgEarthSymbology/Geometry>
#include <osgEarthAnnotation/FeatureNode>

GLOBE_NS

PointSymbolElement::PointSymbolElement()
{
	osg::ref_ptr<osgEarth::Geometry> ptrGeo = new osgEarth::PointSet();
	ptrGeo->push_back(0,0,0);
	osg::ref_ptr<osgEarth::Features::Feature> ptrFeature = new osgEarth::Features::Feature(ptrGeo, GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS());
	//feature->geoInterp() = GEOINTERP_RHUMB_LINE;

	osgEarth::Style style;
	style.getOrCreate<osgEarth::PointSymbol>()->fill() = osgEarth::Color::Cyan;
	style.getOrCreate<osgEarth::PointSymbol>()->size() = 5.0f;
	//geomStyle.getOrCreate<PointSymbol>()->tessellationSize() = 75000;
	style.getOrCreate<osgEarth::AltitudeSymbol>()->clamping() = osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
	style.getOrCreate<osgEarth::AltitudeSymbol>()->technique() = osgEarth::AltitudeSymbol::TECHNIQUE_GPU;

	osg::ref_ptr<osgEarth::Annotation::FeatureNode> ptrFeatureNode = new osgEarth::Annotation::FeatureNode(ptrFeature, style);

	m_ptrElementHandle = new ReferenceImp<osgEarth::Annotation::FeatureNode >(ptrFeatureNode);
	m_ElementType = ePointSymbolElement;
}

void PointSymbolElement::setColor(int r, int g, int b, float a )
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->getStyle();
	style.getOrCreate<osgEarth::PointSymbol>()->fill() = osgEarth::Symbology::Color(r / 255.0, g / 255.0, b / 255.0, a);

	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->setStyle(style);
}

void PointSymbolElement::setSize(int nSize)
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->getStyle();
	style.getOrCreate<osgEarth::PointSymbol>()->size() = nSize;

	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->setStyle(style);
}

void PointSymbolElement::AddPoint(double x, double y, double z)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->getFeature()->getGeometry()->push_back(x,y,z);
	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->dirty();
}

PointSymbolElement::~PointSymbolElement()
{
	
}

GLOBE_ENDNS