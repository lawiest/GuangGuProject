#include "Layer.h"
#include <osg/ref_ptr>
#include <osgEarth/Layer>
#include <osgEarth/MapNode>
#include "OsgCoreSingleton.h"

GLOBE_NS

Layer::Layer() :m_dXMin(-180), m_dXMax(180), m_dYMin(-90), m_dYMax(90), m_nTopLevel(0),m_nBottomLevel(20)
{
	m_dLongitude = 110.0;
	m_dLatitude = 30.0;
	m_dHeigh = OsgCoreSingleton::Instance()->MapNode()->getMapSRS()->getEllipsoid()->getRadiusEquator() * 1.7;
}

DataType Layer::LayerType()
{
	return m_LayerType;
}

StoreType Layer::StorageType()
{
	return m_StoreType;
}

std::string Layer::LayerUrl()
{
	return m_strLayerUrl;
}

void Layer::Range(double dXMin, double dXMax, double dYMin, double dYMax)
{
	m_dXMin = dXMin;
	m_dXMax = dXMax;
	m_dYMin = dYMin;
	m_dYMax = dYMax;
}

void Layer::setCenter(double dLongitude, double dLatitude, double dHeight)
{
	m_dLongitude = dLongitude;
	m_dLatitude = dLatitude;
	m_dHeigh = dHeight;
}

double Layer::Longitude()
{
	return m_dLongitude;
}

double Layer::Latitude()
{
	return m_dLatitude;
}

double Layer::Height()
{
	return m_dHeigh;
}

double Layer::xMin()
{
	return m_dXMin;
}

double Layer::xMax()
{
	return m_dXMax;
}

double Layer::yMin()
{
	return m_dYMin;
}

double Layer::yMax()
{
	return m_dYMax;
}

int Layer::Top()
{
	return m_nTopLevel;
}

int Layer::Bottom()
{
	return m_nBottomLevel;
}

void Layer::getCenter(double& dLongitude, double& dLatitude, double& dHeight)
{
	dLongitude = m_dLongitude;
	dLatitude = m_dLatitude;
	dHeight = m_dHeigh;
}

GsBox Layer::Range()
{
	return GsBox(m_dXMin, m_dYMin, m_dXMax, m_dYMax);
}

Layer::~Layer()
{

}

int Layer::UID()
{
	return m_ptrLayerHandle->CastTo<osg::ref_ptr<osgEarth::Layer>>()->getUID();
}

GLOBE_ENDNS