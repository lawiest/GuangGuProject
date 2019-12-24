#include "Feature.h"

GLOBE_NS

Feature::Feature()
{

}

int Feature::getRefFID()
{
	return m_nRefFID;
}

void Feature::setRefFID(int nFID)
{
	m_nRefFID = nFID;
}

GsRawPoint3D Feature::getOriginPoint()
{
	return m_OriginPoint;
}

void Feature::setOriginPoint(GsRawPoint3D point)
{
	m_OriginPoint = point;
}

GsRawPoint3D Feature::getPickedPoint()
{
	return m_geoPickedPoint;
}

void Feature::setPickedPoint(const GsRawPoint3D& point)
{
	m_geoPickedPoint = point;
}

Feature::~Feature()
{

}

GLOBE_ENDNS