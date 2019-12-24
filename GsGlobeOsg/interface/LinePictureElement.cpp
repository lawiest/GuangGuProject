#include "globe\LinePictureElement.h"
#include "Analysis\LineShapeMath.h"
#include "Analysis\LineGeometry.h"
#include "osgDB\ReadFile"
#include <osgEarth/GeoData>
#include <osgEarth/MapNode>
#include "OsgCoreSingleton.h"
#include "Analysis/MatrixTransformVisible.h"
#include "innerimp.h"
#include <osg/LineWidth>

GLOBE_NS

DotLine3DMath dot3dLine;
//

osg::Geometry* createAxis(const osg::Vec3& corner, const osg::Vec3& xdir, const osg::Vec3& ydir, const osg::Vec3& zdir)
{
	// set up the Geometry.
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;

	osg::Vec3Array* coords = new osg::Vec3Array(1);
	(*coords)[0] = corner;
	geom->setVertexArray(coords);
	osg::Vec4 x_color(0.0f, 0.2f, 1.0f, 0.0f);
	osg::Vec4Array* color = new osg::Vec4Array(1);
	(*color)[0] = x_color;
	geom->setColorArray(color,osg::Array::BIND_OVERALL);

	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, 2));

	osg::StateSet* stateset = new osg::StateSet;
	osg::LineWidth* linewidth = new osg::LineWidth();
	linewidth->setWidth(10.0f);
	stateset->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geom->setStateSet(stateset);

	return geom.release();
}
//
LinePictureElement3D::LinePictureElement3D(float w, float pictureH, const char * image):
	LinePictureElement3D(w,pictureH,image,LinePictureMixType::SRC)
{
	
}
LinePictureElement3D::LinePictureElement3D(float w, float pictureH, const char* image, LinePictureMixType type, TextureType textureType)
{
	//
	m_imageUrl = image;
	//
	m_MixType = type;
	osg::ref_ptr<osg::LineGeometry> lineGeometry = new osg::LineGeometry(w, pictureH, osgDB::readImageFile(image),type,textureType);
	osg::ref_ptr<osg::Geode> ptrGeode = new osg::Geode();
	osg::ref_ptr<GeoGlobe::MatrixTransformVisible> tranform = new GeoGlobe::MatrixTransformVisible();
	//osg::ref_ptr<osg::Geometry> osgLineGeom = createAxis(osg::Vec3(0.0f, 0.0f, 0.0f), osg::Vec3(295.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 295.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 295.0f));//new osg::Geometry();
																																												   //osgLineGeom->setVertexArray()
	ptrGeode->addDrawable(lineGeometry);
	//ptrGeode->addDrawable(osgLineGeom);
	tranform->addChild(ptrGeode);
	lineGeometry->setName("line_opengl_pic");
	ptrGeode->setName("line_Geode_pic");
	tranform->setName("line_Trans_pic");

	m_ptrGeode = new ReferenceImp<osg::Geode>(ptrGeode);
	m_ptrLinesGeom = new ReferenceImp<osg::LineGeometry>(lineGeometry);
	m_ptrTranForm = new ReferenceImp<GeoGlobe::MatrixTransformVisible>(tranform);
	//m_ptrNodeLineGeom = new ReferenceImp<osg::Geometry>(osgLineGeom);
	m_ptrElementHandle = m_ptrTranForm;
	m_ElementType = eLinePictureElement3D;
}

LinePictureElement3D::~LinePictureElement3D()
{
}

void LinePictureElement3D::setPoints(const std::vector<KERNEL_NAME::GsRawPoint3D>& points)
{
	m_linePoints = points;
	reBuild();
}

void LinePictureElement3D::AddPoint(KERNEL_NAME::GsRawPoint3D & point)
{
	m_linePoints.push_back(point);
	reBuild();
}

KERNEL_NAME::GsRawPoint3D LinePictureElement3D::GetPoint(int index)
{
	if (index < m_linePoints.size())
		return m_linePoints[index];
	return KERNEL_NAME::GsRawPoint3D();
}

int LinePictureElement3D::PointSize()
{
	return m_linePoints.size();
}

std::vector<KERNEL_NAME::GsRawPoint3D> LinePictureElement3D::GetPoints()
{
	return m_linePoints;
}

float LinePictureElement3D::GetLineScale()
{
	osg::ref_ptr<osg::LineGeometry> lineGeom = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::LineGeometry>>();
	return lineGeom->GetLineScale();
}

void LinePictureElement3D::SetLineScale(float v)
{
	osg::ref_ptr<osg::LineGeometry> lineGeom = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::LineGeometry>>();
	lineGeom->SetLineScale(v);
}

std::string LinePictureElement3D::GetImageUrl()
{
	return m_imageUrl;
}

float LinePictureElement3D::GetLineWidth()
{
	osg::ref_ptr<osg::LineGeometry> lineGeom = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::LineGeometry>>();
	return lineGeom->getLineWidth();
}

void LinePictureElement3D::SetLineWidth(float width)
{
	osg::ref_ptr<osg::LineGeometry> lineGeom = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::LineGeometry>>();
	lineGeom->setLineWidth(width);
}

float LinePictureElement3D::GetImageHeight()
{
	osg::ref_ptr<osg::LineGeometry> lineGeom = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::LineGeometry>>();
	return lineGeom->getLineImageHeight();
}

bool LinePictureElement3D::Visible()
{
	osg::ref_ptr<GeoGlobe::MatrixTransformVisible> trans = m_ptrTranForm->CastTo<osg::ref_ptr<GeoGlobe::MatrixTransformVisible>>();
	return trans->Visible();
}

void LinePictureElement3D::Visible(bool bVisible)
{
	osg::ref_ptr<GeoGlobe::MatrixTransformVisible> trans = m_ptrTranForm->CastTo<osg::ref_ptr<GeoGlobe::MatrixTransformVisible>>();
	trans->setVisible(bVisible);
}

void LinePictureElement3D::SetLineColor(const KERNEL_NAME::GsColor & color)
{
	osg::ref_ptr<osg::LineGeometry> lineGeom = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::LineGeometry>>();
	lineGeom->setLineColor(osg::Vec4(color.R / 255.f, color.G / 255.f, color.B / 255.f, color.A / 255.f));
}

KERNEL_NAME::GsColor LinePictureElement3D::GetLineColor()
{
	osg::ref_ptr<osg::LineGeometry> lineGeom = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::LineGeometry>>();
	osg::Vec4 color = lineGeom->getLineColor();
	return KERNEL_NAME::GsColor((unsigned char )(color.r()*255),color.g()*255,color.b()*255,color.a()*255);
}

void LinePictureElement3D::SetFixLod(float fixlod)
{
	osg::ref_ptr<osg::LineGeometry> lineGeom = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::LineGeometry>>();
	lineGeom->setFixLod(fixlod);
}

float LinePictureElement3D::GetFixLod()
{
	osg::ref_ptr<osg::LineGeometry> lineGeom = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::LineGeometry>>();
	return lineGeom->getFixLod();
}

LinePictureMixType LinePictureElement3D::GetPictureMixType()
{
	return m_MixType;
}

void LinePictureElement3D::SetAnimatorAction(AnimatorAction * action)
{
	osg::ref_ptr<osg::LineGeometry> lineGeom = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::LineGeometry>>();
	lineGeom->setAnimatorAction(action);
}

AnimatorAction * LinePictureElement3D::GetAnimatorAction()
{
	osg::ref_ptr<osg::LineGeometry> lineGeom = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::LineGeometry>>();
	return lineGeom->getAnimatorAction();
}

void LinePictureElement3D::reBuild()
{
	//
	//为了做添加的线的过程
	//osg::ref_ptr<osg::Geometry> lineGeox = m_ptrNodeLineGeom->CastTo<osg::ref_ptr<osg::Geometry>>();
	//
	osg::ref_ptr<osgEarth::MapNode> ptrMapNode = GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode();
	osg::Vec3d vecPos;
	osg::Vec3d firstPos;

	osg::ref_ptr<osg::Vec3dArray> points=new osg::Vec3dArray();
	double len = 0;
	int size = m_linePoints.size();

	for (int i = 0; i < size; i++)
	{
		osgEarth::GeoPoint mapPoint(ptrMapNode->getMapSRS(), m_linePoints[i].X, m_linePoints[i].Y, m_linePoints[i].Z);
		mapPoint.toWorld(vecPos);
		
		if (i == 0)
			firstPos = vecPos;
		points->push_back(vecPos -firstPos);
		if(i>0)
			len+= (points->at(i - 1) - points->at(i)).length();
	}
	//
	osgEarth::GeoPoint mapPoint(ptrMapNode->getMapSRS(), m_linePoints[0].X, m_linePoints[0].Y, m_linePoints[0].Z);
	osg::Vec3d groud1;
	mapPoint.toWorld(groud1);
	//
	mapPoint.z() = m_linePoints[0].Z + 200;
	osg::Vec3d groud2;
	mapPoint.toWorld(groud2);
	osg::Vec3 grouddir = groud2 - groud1;
	grouddir.normalize();
	//线的三角切分
	dot3dLine.start(size, grouddir, len, false);
	for (int i = 0; i < size; i++)
	{
		dot3dLine.addPoint(points->at(i));
	}
	dot3dLine.end();
	//设置切分的的过程
	osg::ref_ptr<osg::LineGeometry> lineGeom = m_ptrLinesGeom->CastTo<osg::ref_ptr<osg::LineGeometry>>();
	lineGeom->SetLineShape(dot3dLine.VertexPoints(), dot3dLine.GetVerTexOffset(), dot3dLine.GetCoorTextVertex(), dot3dLine.Indexs());
	lineGeom->SetLineLength(len);
	//
	//lineGeox->setVertexArray(points);
	//lineGeox->setPrimitiveSet(0, new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, points->size()));
	// 转换的过程
	osg::ref_ptr<osg::MatrixTransform> trans = m_ptrTranForm->CastTo<osg::ref_ptr<osg::MatrixTransform>>();
	trans->setMatrix(osg::Matrix::translate(firstPos));

	lineGeom->dirtyDisplayList();
}

GLOBE_ENDNS