//
//#include "PolygonSymbolElement.h"
//#include "OsgCoreSingleton.h"
//#include "innerimp.h"
//#include <osgEarthSymbology/LineSymbol>
//#include <osgEarthSymbology/Geometry>
//#include <osgEarthAnnotation/FeatureNode>
//#include <osgEarthUtil/ObjectLocator>
//
//GLOBE_NS
//
//PolygonSymbolElement::PolygonSymbolElement()
//{
//	osg::ref_ptr<osgEarth::Geometry> ptrGeo = new osgEarth::Polygon();
//
//	osgEarth::Style style;
//
//	osg::ref_ptr<osgEarth::Features::Feature> ptrFeature = new osgEarth::Features::Feature(ptrGeo, GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS());
//	ptrFeature->geoInterp() = osgEarth::GEOINTERP_RHUMB_LINE;
//
//	style.getOrCreate<osgEarth::LineSymbol>()->stroke()->color() = osgEarth::Color::Lime;
//	style.getOrCreate<osgEarth::LineSymbol>()->stroke()->width() = 4.0f;
//	style.getOrCreate<osgEarth::LineSymbol>()->stroke()->widthUnits() = osgEarth::Units::PIXELS;
//	style.getOrCreate<osgEarth::LineSymbol>()->tessellationSize() = 75000;
//
//	style.getOrCreate<osgEarth::PolygonSymbol>()->fill() = osgEarth::Color::Color(0.0f,0.0f,1.0f,0.5);
//	style.getOrCreate<osgEarth::AltitudeSymbol>()->clamping() = osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
//	style.getOrCreate<osgEarth::AltitudeSymbol>()->technique() = osgEarth::AltitudeSymbol::TECHNIQUE_DRAPE;
//
//	style.getOrCreate<osgEarth::RenderSymbol>()->depthOffset()->enabled() = true;
//	style.getOrCreate<osgEarth::RenderSymbol>()->depthOffset()->automatic() = true;
//
//	osg::ref_ptr<osgEarth::Annotation::FeatureNode> ptrFeatureNode = new osgEarth::Annotation::FeatureNode(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode(), ptrFeature, style);
//
//	m_ptrElementHandle = new ReferenceImp<osgEarth::Annotation::FeatureNode >(ptrFeatureNode);
//	m_ElementType = ePolygonSymbolElement;
//}
//
//void PolygonSymbolElement::setLineColor(int r, int g, int b, float a )
//{
//	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->getStyle();
//	style.getOrCreate<osgEarth::LineSymbol>()->stroke()->color() = osgEarth::Symbology::Color(r / 255.0, g / 255.0, b / 255.0, a);
//
//	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->setStyle(style);
//}
//
//void PolygonSymbolElement::setFillColor(int r, int g, int b, float a )
//{
//	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->getStyle();
//	style.getOrCreate<osgEarth::PolygonSymbol>()->fill() = osgEarth::Color::Color(r / 255.0, g / 255.0, b / 255.0, a);
//
//	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->setStyle(style);
//}
//
//void PolygonSymbolElement::ClearFillColor()
//{
//	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->getStyle();
//	style.getOrCreate<osgEarth::PolygonSymbol>()->fill()= osgEarth::Color::Color(0.0f, 0.0f, 0.0f, 0);
//
//	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->setStyle(style);
//}
//
//void PolygonSymbolElement::setLineSize(int nSize)
//{
//	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->getStyle();
//	style.getOrCreate<osgEarth::PointSymbol>()->size() = nSize;
//
//	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->setStyle(style);
//}
//
//void PolygonSymbolElement::AddPoint(double x, double y, double z)
//{
//	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->getFeature()->getGeometry()->push_back(x,y,z);
//	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Annotation::FeatureNode> >()->dirty();
//}
//
//PolygonSymbolElement::~PolygonSymbolElement()
//{
//	
//}
//
//GLOBE_ENDNS

#include "PolygonSymbolElement.h"
#include "Analysis/MatrixTransformVisible.h"
#include <osg/Geode>
#include <osg/Geometry>
#include "innerimp.h"
#include <osg/LineWidth>
#include <osg/LineWidth>
#include <osg/Depth>
#include <osgUtil/Tessellator> 
#include <osgEarth/MapNode>
#include "OsgCoreSingleton.h"
// 重新实现面的封装过程
GLOBE_NS

void setGeometryDepth(osg::ref_ptr<osg::Geometry> geom, bool on)
{
	osg::ref_ptr<osg::StateSet> stateset = geom->getOrCreateStateSet();
	if (on)
		stateset->removeAttribute(osg::StateAttribute::DEPTH);
	else
	stateset->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false), osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
}

osg::Geometry* CreateGeometry()
{
	osg::ref_ptr<osg::Geometry> ptrPolygon = new osg::Geometry();

	osg::ref_ptr<osg::Vec3Array> ptrVertices = new osg::Vec3Array();
	osg::ref_ptr<osg::StateSet> stateset = ptrPolygon->getOrCreateStateSet();
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	//设置法线跟着缩放
	stateset->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
	stateset->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false), osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
	//stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	stateset->setRenderBinDetails(11, "RenderBin");
	//开启透明
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	ptrPolygon->setVertexArray(ptrVertices);
	osg::ref_ptr<osg::Vec4Array> ptrColors = new osg::Vec4Array;
	ptrColors->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	ptrPolygon->setColorArray(ptrColors, osg::Array::BIND_OVERALL);
	ptrPolygon->setCullingActive(false);
	osg::ref_ptr<osg::DrawArrays> ptrDrawArray = new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, ptrVertices->size());
	ptrPolygon->addPrimitiveSet(ptrDrawArray);
	return ptrPolygon.release();
}

PolygonSymbolElement::PolygonSymbolElement()
{
	// 面的颜色
	m_LineWidth = 0;// 线宽
	m_VerFaceHeight = 0;
	m_isDepthTest = false;
	m_FillColor = KERNEL_NAME::GsColor(0, 0, 255, 128);
	m_LineColor = KERNEL_NAME::GsColor(0, 255, 255, 255);
	m_FaceColor = KERNEL_NAME::GsColor(0, 0, 255, 128);
	//
	osg::ref_ptr<osg::Geode> ptrGeode = new osg::Geode();
	osg::ref_ptr<GeoGlobe::MatrixTransformVisible> ptrTrans = new GeoGlobe::MatrixTransformVisible();
	// 创建面geometry
	osg::ref_ptr<osg::Geometry> ptrPolygon = CreateGeometry();
	osg::ref_ptr<osg::Geometry> ptrLine = CreateGeometry();
	osg::ref_ptr<osg::Geometry> ptrFace = CreateGeometry();
	//
	ptrGeode->setCullingActive(false);
	ptrGeode->addDrawable(ptrPolygon);
	ptrGeode->setDataVariance(osg::Object::DYNAMIC);

	ptrGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	ptrGeode->getOrCreateStateSet()->setAttributeAndModes(new osg::Program, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	// hand 保存
	ptrTrans->addChild(ptrGeode);
	//
	m_ptrGeode = new ReferenceImp<osg::Geode>(ptrGeode);
	m_ptrPolygonGeom = new ReferenceImp<osg::Geometry>(ptrPolygon);
	m_ptrFacePolyGeom = new ReferenceImp<osg::Geometry>(ptrFace);
	m_ptrLinesGeom = new ReferenceImp<osg::Geometry>(ptrLine);
	m_ptrElementHandle = new ReferenceImp<GeoGlobe::MatrixTransformVisible>(ptrTrans);
	m_ElementType = ePolygonSymbolElement;
	//
	setFillColor(m_FillColor);
	setFaceColor(m_FaceColor);
}

PolygonSymbolElement::~PolygonSymbolElement()
{

}

void PolygonSymbolElement::AddPoint(double x, double y, double z)
{
	m_Points.push_back(KERNEL_NAME::GsRawPoint3D(x, y, z));
	reBuild();
}

void PolygonSymbolElement::setLineColor(const KERNEL_NAME::GsColor & lineColor)
{
	m_LineColor = lineColor;
	osg::ref_ptr<osg::Geometry> ptrLine = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::Geometry>>();
	osg::Vec4Array* color = dynamic_cast<osg::Vec4Array*>(ptrLine->getColorArray());
	if (!color)
		return;
	color->begin()->set(lineColor.R / 255.f, lineColor.G / 255.f, lineColor.B / 255.f, lineColor.A / 255.f);
	color->dirty();
	ptrLine->dirtyDisplayList();
}

KERNEL_NAME::GsColor PolygonSymbolElement::getLineColor()
{
	return m_LineColor;
}

void PolygonSymbolElement::setFillColor(const KERNEL_NAME::GsColor & fillcolor)
{
	m_FillColor = fillcolor;
	osg::ref_ptr<osg::Geometry> ptrPolygon = m_ptrPolygonGeom->CastTo<osg::ref_ptr<osg::Geometry>>();
	osg::Vec4Array* color = dynamic_cast<osg::Vec4Array*>(ptrPolygon->getColorArray());
	if (!color)
		return;
	color->begin()->set(fillcolor.R / 255.f, fillcolor.G / 255.f, fillcolor.B / 255.f, fillcolor.A / 255.f);
	color->dirty();
	ptrPolygon->dirtyDisplayList();
}

KERNEL_NAME::GsColor PolygonSymbolElement::getFillColor()
{
	return m_FillColor;
}

void PolygonSymbolElement::setFaceColor(const KERNEL_NAME::GsColor & faceColor)
{
	m_FaceColor = faceColor;
	osg::ref_ptr<osg::Geometry> ptrFacePolygon = m_ptrFacePolyGeom->CastTo<osg::ref_ptr<osg::Geometry>>();
	osg::Vec4Array* color = dynamic_cast<osg::Vec4Array*>(ptrFacePolygon->getColorArray());
	if (!color)
		return;
	color->begin()->set(faceColor.R / 255.f, faceColor.G / 255.f, faceColor.B / 255.f, faceColor.A / 255.f);
	color->dirty();
	ptrFacePolygon->dirtyDisplayList();
}

KERNEL_NAME::GsColor PolygonSymbolElement::getFaceColor()
{
	return m_FaceColor;
}

void PolygonSymbolElement::setFaceHeight(float height)
{
	if (m_VerFaceHeight == height || height<0)
		return;

	osg::ref_ptr<osg::Geode> ptrGeode = m_ptrGeode->CastTo<osg::ref_ptr<osg::Geode>>();
	osg::ref_ptr<osg::Geometry> ptrFace = m_ptrFacePolyGeom->CastTo<osg::ref_ptr<osg::Geometry>>();
	//
	if (m_VerFaceHeight == 0 )
	{
		ptrGeode->addDrawable(ptrFace);
	}
	else if (height == 0)
	{
		ptrGeode->removeDrawable(ptrFace);
	}
	m_VerFaceHeight = height;
}

float PolygonSymbolElement::getFaceHeight()
{
	return m_VerFaceHeight;
}

bool PolygonSymbolElement::isDepthTest()
{
	return m_isDepthTest;
}

void PolygonSymbolElement::setDepthTest(bool cover)
{
	m_isDepthTest = cover;
	setGeometryDepth(m_ptrFacePolyGeom->CastTo<osg::ref_ptr<osg::Geometry>>(), cover);
	setGeometryDepth(m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::Geometry>>(), cover);
	setGeometryDepth(m_ptrPolygonGeom->CastTo<osg::ref_ptr<osg::Geometry>>(), cover);
}

bool PolygonSymbolElement::Visible()
{
	osg::ref_ptr<GeoGlobe::MatrixTransformVisible> trans = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::MatrixTransformVisible>>();
	return trans->Visible();
}

void PolygonSymbolElement::Visible(bool bVisible)
{
	osg::ref_ptr<GeoGlobe::MatrixTransformVisible> trans = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::MatrixTransformVisible>>();
	 trans->setVisible(bVisible);
}

void PolygonSymbolElement::setLineWidth(float width)
{
	if (m_LineWidth == width)
		return;
	osg::ref_ptr<osg::Geode> ptrGeode = m_ptrGeode->CastTo<osg::ref_ptr<osg::Geode>>();
	osg::ref_ptr<osg::Geometry> ptrLine = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::Geometry>>();
	
	if (m_LineWidth == 0 && width > 0)
	{
		ptrGeode->addDrawable(ptrLine);
	}
	else if (width <= 0)
	{
		ptrGeode->removeDrawable(ptrLine);
	}
	m_LineWidth = width;
	if (width > 0)
	{
		osg::ref_ptr<osg::LineWidth> ptrLineWidth = new osg::LineWidth(width);
		ptrLine->getOrCreateStateSet()->setAttributeAndModes(ptrLineWidth, osg::StateAttribute::ON);
	}
}

float PolygonSymbolElement::getLineWidth()
{
	return m_LineWidth;
}

const std::vector<KERNEL_NAME::GsRawPoint3D>& PolygonSymbolElement::getPoints()
{
	// TODO: 在此处插入 return 语句
	return m_Points;
}

void PolygonSymbolElement::setPoints(const std::vector<KERNEL_NAME::GsRawPoint3D>& points)
{
	m_Points = points;
	reBuild();
}

void PolygonSymbolElement::reBuild()
{
	//为了做添加的线的过程
	osg::ref_ptr<osg::Geometry> ptrPolygon = m_ptrPolygonGeom->CastTo<osg::ref_ptr<osg::Geometry>>();
	osg::ref_ptr<osg::Geometry> ptrFace=m_ptrFacePolyGeom->CastTo<osg::ref_ptr<osg::Geometry>>();
	//
	osg::ref_ptr<osgEarth::MapNode> ptrMapNode = GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode();
	osg::Vec3d vecPos;
	osg::Vec3d firstPos;

	osg::Vec3Array* points =dynamic_cast<osg::Vec3Array*>(ptrPolygon->getVertexArray());
	points->clear();
	osg::Vec3Array* facePoints = dynamic_cast<osg::Vec3Array*>(ptrFace->getVertexArray());
	facePoints->clear();

	float len = 0;
	int size = m_Points.size();
	for (int i = 0; i < size; i++)
	{
		osgEarth::GeoPoint mapPoint(ptrMapNode->getMapSRS(), m_Points[i].X, m_Points[i].Y, m_Points[i].Z);
		mapPoint.toWorld(vecPos);
		if (i == 0)
			firstPos = vecPos;

		if (m_VerFaceHeight > 0)
		{
			facePoints->push_back( (vecPos - firstPos));
			mapPoint.set(ptrMapNode->getMapSRS(), m_Points[i].X, m_Points[i].Y, m_Points[i].Z + m_VerFaceHeight,mapPoint.altitudeMode());
			mapPoint.toWorld(vecPos);
			//添加之后的高层
			points->push_back(vecPos - firstPos);
			facePoints->push_back((vecPos - firstPos));
		}
		else {
			points->push_back(vecPos - firstPos);
		}
	}
	//
	if (m_LineWidth > 0)
	{
		osg::ref_ptr<osg::Geometry> ptrLine = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::Geometry>>();
		osg::Vec3Array* linepoints = dynamic_cast<osg::Vec3Array*>(ptrLine->getVertexArray());
		linepoints->resize(points->size());
		// 拷贝坐标
		osg::Vec3Array* orPoints = m_VerFaceHeight > 0 ? facePoints : points;
		int step = m_VerFaceHeight > 0 ? 2 : 1;
		osg::Vec3Array::iterator begin = orPoints->begin();
		osg::Vec3Array::iterator _Dest=linepoints->begin();
		for (; begin != orPoints->end(); begin += step)
		{
			*_Dest++ = *begin;
		}
		ptrLine->setPrimitiveSet(0, new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, size));
	}
	//
	ptrPolygon->setPrimitiveSet(0, new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, size));
	// 绘制立体面的立面部分
	if (m_VerFaceHeight > 0)
	{
		// 若不是闭合的则闭合面的过程
		if (size>2&&m_Points[0] != m_Points[size - 1])
		{
			facePoints->push_back((*facePoints)[0]);
			facePoints->push_back((*facePoints)[1]);
		}
		ptrFace->setPrimitiveSet(0, new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_STRIP, 0, facePoints->size()));
		facePoints->dirty();
	}
	//面的颜色
	osg::ref_ptr<osgUtil::Tessellator> tscx = new osgUtil::Tessellator();
	tscx->setTessellationNormal(osg::Vec3(0.0, 0.0, 1.0));
	tscx->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
	tscx->setBoundaryOnly(false);
	tscx->setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD); // the commonest tessellation is default, ODD. GE2 allows intersections of constraints to be found.
	tscx->retessellatePolygons(*(ptrPolygon)); // this should insert extra vertices where constraints overlap
	// 转换的过程
	osg::ref_ptr<osg::MatrixTransform> trans = m_ptrElementHandle->CastTo<osg::ref_ptr<osg::MatrixTransform>>();
	trans->setMatrix(osg::Matrix::translate(firstPos));
	//
	points->dirty();
}
GLOBE_ENDNS