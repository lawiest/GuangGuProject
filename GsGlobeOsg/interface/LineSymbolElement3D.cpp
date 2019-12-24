
#include "LineSymbolElement3D.h"
#include "OsgCoreSingleton.h"
#include "innerimp.h"
#include <osg/Geode>
#include <osg/Geometry>
#include <osgEarth/GeoData>
#include <osgEarth/MapNode>
#include <osg/Array>
#include <osgUtil/SmoothingVisitor>
#include <osg/LineWidth>
#include <osg/Depth>
#include <osg/MatrixTransform>
#include <osgUtil/Tessellator> 

GLOBE_NS

LineSymbolElement3D::LineSymbolElement3D(Mode model)
{
	osg::ref_ptr<osg::Geode> ptrGeode = new osg::Geode();
	osg::ref_ptr<osg::MatrixTransform> ptrMatrix = new osg::MatrixTransform();
	ptrMatrix->addChild(ptrGeode);
	m_ptrElementHandle = new ReferenceImp<osg::MatrixTransform>(ptrMatrix);

	osg::ref_ptr<osg::Geometry> ptrLinesGeom = new osg::Geometry();
	m_ptrLinesGeom = new ReferenceImp<osg::Geometry>(ptrLinesGeom);

	osg::ref_ptr<osg::Vec3Array> ptrVertices = new osg::Vec3Array();
	m_ptrVertex = new ReferenceImp<osg::Vec3Array>(ptrVertices);
	
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));

	osg::ref_ptr<osg::StateSet> stateset = ptrLinesGeom->getOrCreateStateSet();
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	//设置法线跟着缩放
	stateset->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
	stateset->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false), osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
	//stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	stateset->setRenderBinDetails(11, "RenderBin");
	//开启透明
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	
	ptrLinesGeom->setVertexArray(ptrVertices);
	ptrLinesGeom->setNormalArray(normals.get(), osg::Array::BIND_OVERALL);

	osg::ref_ptr<osg::Vec4Array> ptrColors = new osg::Vec4Array;
	m_ptrColors = new ReferenceImp<osg::Vec4Array>(ptrColors);

	ptrColors->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	ptrLinesGeom->setColorArray(ptrColors, osg::Array::BIND_OVERALL);
	ptrLinesGeom->setCullingActive(false);

	osg::PrimitiveSet::Mode geoModel;

	if (model == LINES)
		geoModel = osg::PrimitiveSet::LINES;
	else if (model == LINE_STRIP)
		geoModel = osg::PrimitiveSet::LINE_STRIP;
	else if (model == LINE_LOOP)
		geoModel = osg::PrimitiveSet::LINE_LOOP;
	else if (model == POLYGON)
		geoModel = osg::PrimitiveSet::POLYGON;

	osg::ref_ptr<osg::DrawArrays> ptrDrawArray = new osg::DrawArrays(geoModel, 0, ptrVertices->size());
	m_ptrDrawArray = new ReferenceImp<osg::DrawArrays>(ptrDrawArray);

	ptrLinesGeom->addPrimitiveSet(ptrDrawArray);

	ptrGeode->addDrawable(ptrLinesGeom);
	ptrGeode->setCullingActive(false);
	ptrGeode->setDataVariance(osg::Object::DYNAMIC);

	ptrGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	ptrGeode->getOrCreateStateSet()->setAttributeAndModes(new osg::Program, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

	m_ElementType = elineSymbolElement3D;
}

void LineSymbolElement3D::setColor(int r, int g, int b, int a )
{
	osg::ref_ptr<osg::Vec4Array> ptrLinesGeom = m_ptrColors->CastTo<osg::ref_ptr<osg::Vec4Array> >();

	if (!ptrLinesGeom)
		return;

	ptrLinesGeom->begin()->set(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	ptrLinesGeom->dirty();
}

KERNEL_NAME::GsColor LineSymbolElement3D::getColor()
{
	osg::ref_ptr<osg::Vec4Array> ptrLinesGeom = m_ptrColors->CastTo<osg::ref_ptr<osg::Vec4Array> >();

	if (!ptrLinesGeom)
		return KERNEL_NAME::GsColor();

	return KERNEL_NAME::GsColor((ptrLinesGeom->begin()->r() * 255), ptrLinesGeom->begin()->g() * 255, ptrLinesGeom->begin()->b() * 255, ptrLinesGeom->begin()->a() * 255);
}

void LineSymbolElement3D::Rotate(GsRawPoint3D gsAxle, double dbDegree)
{
	osg::ref_ptr<osg::Geometry> ptrLinesGeom = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::Geometry> >();
	osg::Vec3 vec = ptrLinesGeom->getBound().center();
	osg::Vec3 axis = osg::Vec3(vec.x() + m_origin.X, vec.y() + m_origin.Y, vec.z() + m_origin.Z);
	axis.normalize();

	osg::ref_ptr<osg::MatrixTransform> ptrMat = m_ptrElementHandle->CastTo<osg::ref_ptr<osg::MatrixTransform> >();
	ptrMat->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(dbDegree), axis)*osg::Matrix::translate(m_origin.X, m_origin.Y, m_origin.Z));
}

int LineSymbolElement3D::getLineWidth()
{
	return m_nSize;
}

void LineSymbolElement3D::setLineWidth(int nSize)
{
	osg::ref_ptr<osg::Geometry> ptrLinesGeom = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::Geometry> >();
	if (!ptrLinesGeom)
		return;

	m_nSize = nSize;

	osg::ref_ptr<osg::LineWidth> ptrLineWidth = new osg::LineWidth(nSize);
	ptrLinesGeom->getOrCreateStateSet()->setAttributeAndModes(ptrLineWidth, osg::StateAttribute::ON);
}

void LineSymbolElement3D::Finish()
{
	osg::ref_ptr<osg::MatrixTransform> ptrMatrix = m_ptrElementHandle->CastTo<osg::ref_ptr<osg::MatrixTransform> >();
	osg::ref_ptr<osg::Geode> ptrGeode = dynamic_cast<osg::Geode*>(ptrMatrix->getChild(0));
	osg::ref_ptr<osg::Geometry> ptrGeo = ptrGeode->getDrawable(0)->asGeometry();

	osg::ref_ptr<osgUtil::Tessellator> tscx = new osgUtil::Tessellator();
	tscx->setTessellationNormal(osg::Vec3(0.0, 0.0, 1.0));
	tscx->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
	tscx->setBoundaryOnly(false);
	tscx->setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD); // the commonest tessellation is default, ODD. GE2 allows intersections of constraints to be found.
	tscx->retessellatePolygons(*(ptrGeo)); // this should insert extra vertices where constraints overlap

	osg::ref_ptr<osg::Vec3Array> ptrVertices = m_ptrVertex->CastTo<osg::ref_ptr<osg::Vec3Array> >();

	if (!ptrVertices)
		return;

	ptrVertices->dirty();
}

const std::vector<KERNEL_NAME::GsRawPoint3D> LineSymbolElement3D::getPoints()
{
	return m_points;
}

void LineSymbolElement3D::AddPoint(double x, double y, double z)
{
	m_points.push_back(KERNEL_NAME::GsRawPoint3D(x,y,z));

	osg::ref_ptr<osg::Vec3Array> ptrVertices = m_ptrVertex->CastTo<osg::ref_ptr<osg::Vec3Array> >();

	if (!ptrVertices)
		return;

	osg::ref_ptr<osgEarth::MapNode> ptrMapNode = GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode();
	osg::Vec3d vecPos;
	osgEarth::GeoPoint mapPoint(ptrMapNode->getMapSRS(), x, y, z);
	mapPoint.toWorld(vecPos);

	if (ptrVertices->size() == 0)
	{
		ptrVertices->push_back(osg::Vec3d(0.0, 0.0, 0.0));
		m_origin.Set3D(vecPos.x(), vecPos.y(), vecPos.z());
		m_ptrElementHandle->CastTo<osg::ref_ptr<osg::MatrixTransform> >()->setMatrix(osg::Matrix::translate(vecPos.x(), vecPos.y(), vecPos.z()));
	}
	else
	{
		ptrVertices->push_back(osg::Vec3d(vecPos.x()- m_origin.X, vecPos.y()- m_origin.Y, vecPos.z()- m_origin.Z));
	}

	//osgUtil::SmoothingVisitor::smooth(*g_ptrLinesGeom, osg::DegreesToRadians(30.0));

	ptrVertices->dirty();

	osg::ref_ptr<osg::DrawArrays> ptrDrawArray = m_ptrDrawArray->CastTo<osg::ref_ptr<osg::DrawArrays> >();
	if (!ptrDrawArray)
		return;

	ptrDrawArray->setCount(ptrVertices->size());
}

LineSymbolElement3D::~LineSymbolElement3D()
{
	
}

GLOBE_ENDNS