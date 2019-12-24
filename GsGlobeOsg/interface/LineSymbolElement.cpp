
#include "LineSymbolElement.h"
#include "OsgCoreSingleton.h"
#include "innerimp.h"
#include <osgEarthSymbology/LineSymbol>
#include <osgEarthSymbology/Geometry>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarth/GeoTransform>

GLOBE_NS

LineSymbolElement::LineSymbolElement()
{
	osg::ref_ptr<osgEarth::Geometry> ptrGeo = new osgEarth::LineString();

	osg::ref_ptr<osgEarth::Features::Feature> ptrFeature = new osgEarth::Features::Feature(ptrGeo, GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS());
	ptrFeature->geoInterp() = osgEarth::GEOINTERP_GREAT_CIRCLE;

	osgEarth::Style style;
	//osgEarth::AltitudeSymbol* alt = style.getOrCreate<osgEarth::AltitudeSymbol>();
	//alt->clamping() = alt->CLAMP_TO_TERRAIN;
	//alt->technique() = alt->TECHNIQUE_DRAPE;
	// offset to mitigate Z fighting
	osgEarth::RenderSymbol* render = style.getOrCreate<osgEarth::RenderSymbol>();
	render->depthOffset()->enabled() = true;
	render->depthOffset()->automatic() = true;

	// define a style for the line
	osgEarth::LineSymbol* ls = style.getOrCreate<osgEarth::LineSymbol>();
	ls->stroke()->color() = osgEarth::Color::Yellow;
	ls->stroke()->width() = 4.0f;
	ls->stroke()->widthUnits() = osgEarth::Units::PIXELS;
	ls->tessellation() = 150;

	osg::ref_ptr<osgEarth::Annotation::FeatureNode> ptrFeatureNode = new osgEarth::Annotation::FeatureNode(ptrFeature, style);

	m_ptrElementHandle = new ReferenceImp<osgEarth::Annotation::FeatureNode >(ptrFeatureNode);
	m_ElementType = eLineSymbolElement;
}

void LineSymbolElement::setColor(int r, int g, int b, float a )
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->getStyle();
	style.getOrCreate<osgEarth::LineSymbol>()->stroke()->color() = osgEarth::Symbology::Color(r / 255.0, g / 255.0, b / 255.0, a);

	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->setStyle(style);
}

void LineSymbolElement::setSize(int nSize)
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->getStyle();
	style.getOrCreate<osgEarth::LineSymbol>()->stroke()->width() = nSize;

	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->setStyle(style);
}

void LineSymbolElement::AddPoint(double x, double y, double z)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->getFeature()->getGeometry()->push_back(x,y,z);
	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->dirty();
}

LineSymbolElement::~LineSymbolElement()
{
	
}

GLOBE_ENDNS