#pragma once
#include <utility.h>
#include <GsReference.h>
#include <geometry.h>

GLOBE_NS

class GS_API Feature :public GeoStar::Utility::GsRefObject
{
public:
	Feature();
	virtual ~Feature();

	int getRefFID();
	void setRefFID(int nFID);

	GsRawPoint3D getOriginPoint();
	void setOriginPoint(GsRawPoint3D point);

	GsRawPoint3D getPickedPoint();
	void setPickedPoint(const GsRawPoint3D& point);

	std::map<std::string, std::string> m_attr;
	std::vector<GsRawPoint3D> m_geo;

private:
	GsRawPoint3D m_OriginPoint;
	GsRawPoint3D m_geoPickedPoint;
	//¹ØÁªID
	int m_nRefFID;
};

GS_SMARTER_PTR(Feature);

GLOBE_ENDNS

