#include "SquareElement.h"
#include "Analysis/SquareGeometry.h"
#include <osgEarth/MapNode>
#include "Analysis/MatrixTransformVisible.h"
#include "osg/Geode"
#include "innerimp.h"
#include "OsgCoreSingleton.h"
#include <osgEarth/GeoTransform>

GLOBE_NS


SquareElement::SquareElement(float w, float h)
{
	osg::ref_ptr<osg::SquareGeometry> geomtry = new osg::SquareGeometry(w, h);
	osg::ref_ptr<osg::Geode> ptrGeode = new osg::Geode();
	ptrGeode->addDrawable(geomtry);
	//
	//tranform->addChild(ptrGeode);
	ptrGeode->setName("line_Geode_pic");
	// 会自动转换到地平面的坐标
	osg::ref_ptr<osgEarth::GeoTransform> ptrLocator = new osgEarth::GeoTransform();
	ptrLocator->addChild(ptrGeode);
	m_ptrSquaRef = new ReferenceImp<osg::SquareGeometry>(geomtry);
	m_ptrElementHandle = new ReferenceImp<osgEarth::GeoTransform>(ptrLocator);
	m_ElementType = ePointSymbolElement;
}

SquareElement::SquareElement(float w, float h, const char * image, LinePictureMixType type)
{
	osg::ref_ptr<osg::Image> osgimage;
	if (image)
		osgimage = osgDB::readImageFile(image);
	osg::ref_ptr<osg::SquareGeometry> geomtry = new osg::SquareGeometry(w, h,osgimage,type);
	osg::ref_ptr<osg::Geode> ptrGeode = new osg::Geode();
	ptrGeode->addDrawable(geomtry);
	//
	ptrGeode->setName("line_Geode_pic");
	// 会自动转换到地平面的坐标
	osg::ref_ptr<osgEarth::GeoTransform> ptrLocator = new osgEarth::GeoTransform();
	ptrLocator->addChild(ptrGeode);
	m_ptrSquaRef = new ReferenceImp<osg::SquareGeometry>(geomtry);
	m_ptrElementHandle = new ReferenceImp<osgEarth::GeoTransform>(ptrLocator);
	m_ElementType = eSquareElement;
}

SquareElement::~SquareElement()
{
}

void SquareElement::setColor(const KERNEL_NAME::GsColor & color)
{
	m_ptrSquaRef->CastTo<osg::ref_ptr<osg::SquareGeometry>>()->setColor(osg::Vec4(color.R / 255.f, color.G / 255.f, color.B / 255.f, color.A / 255.f));
}

KERNEL_NAME::GsColor SquareElement::getColor()
{
	osg::Vec4 color = m_ptrSquaRef->CastTo<osg::ref_ptr<osg::SquareGeometry>>()->getColor();
	return KERNEL_NAME::GsColor::FromARGBF(color.r(), color.g(), color.b(), color.a());
}

void SquareElement::setPosition(double x, double y, double z)
{
	m_dLongitude = x;
	m_dLatitude = y;
	m_dHeigh = z;
	Rebuild();
}

KERNEL_NAME::GsRawPoint3D SquareElement::getPosition()
{
	return KERNEL_NAME::GsRawPoint3D(m_dLongitude,m_dLatitude,m_dHeigh);
}

void SquareElement::getPosition(double & x, double & y, double & z)
{
	x = m_dLongitude;
	y = m_dLatitude;
	z = m_dHeigh;
}

bool SquareElement::Visible()
{
	return false;
}

void SquareElement::Visible(bool bVisible)
{
}

void SquareElement::SetAnimatorAction(AnimatorAction * action)
{
	m_ptrSquaRef->CastTo<osg::ref_ptr<osg::SquareGeometry>>()->setAnimatorAction(action);
}

AnimatorAction * SquareElement::GetAnimatorAction()
{
	return m_ptrSquaRef->CastTo<osg::ref_ptr<osg::SquareGeometry>>()->getAnimatorAction();
}

void SquareElement::Rebuild()
{
	osgEarth::GeoPoint point = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), osg::Vec3d(m_dLongitude, m_dLatitude, m_dHeigh));
	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::GeoTransform>>()->setPosition(point);
	//osg::ref_ptr<osgEarth::MapNode> ptrMapNode = GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode();
	////
	//osg::Vec3d vecPos;
	//osgEarth::GeoPoint mapPoint(ptrMapNode->getMapSRS(), m_dLongitude,m_dLatitude,m_dHeigh);
	//mapPoint.toWorld(vecPos);
	////
	//osg::ref_ptr<osg::MatrixTransform> trans = m_ptrElementHandle->CastTo<osg::ref_ptr<osg::MatrixTransform>>();
	//trans->setMatrix(osg::Matrix::translate(vecPos));
	//trans->dirtyBound();
}

GLOBE_ENDNS