#ifndef OSGEARTHFEATURES_FEATURE_GEOFEATURE_UTILS
#define OSGEARTHFEATURES_FEATURE_GEOFEATURE_UTILS 1

#include <osgEarthFeatures/Common>
#include <osgEarthFeatures/Feature>
#include <osgEarthSymbology/Geometry>
#include <osgEarth/StringUtils>
#include <osg/Notify>
#include <ogr_api.h>
//#include "stdafx.h"
#include <sqlite3.h>
#include "GeoFeature_SqliteApi.h"
#include "Geo_Geometry_Common.h"

using namespace osgEarth::Features;
using namespace osgEarth::Symbology;
using namespace GeoGlobe::GeoVector;

struct GEOFeatureUtils
{
	//将吉奥几何体的坐标数据传给osgEarth中的几何体坐标
	static void populate( GEOGeometryH* geomHandle, Symbology::Geometry* target, int numIndex,int numPoints);
	//////////////////////////////////////////////////////传入高度值，将几何体抬高一定高度
	static void populate( GEOGeometryH* geomHandle, Symbology::Geometry* target, int numIndex,int numPoints,float height);

	static Symbology::Polygon* createPolygon( GEOGeometryH* geomHandle );
	static Symbology::Polygon* createPolygon( GEOGeometryH* geomHandle,float height);

	static Symbology::Geometry* createGeometry( GEOGeometryH* geomHandle );
	static Symbology::Geometry* createGeometry( GEOGeometryH* geomHandle,float height);

	static Feature* createFeature( GEOFeatureH* handle, const SpatialReference* srs );
	static Feature* createFeature( GEOFeatureH* handle, const SpatialReference* srs,float height );
	/*static AttributeType getAttributeType( GEOFieldType type )
	{

	}*/
	////////////////////////////////////////////////////////override function ,in order to create the new geometry, the count of featrue not change,intstead of the count of 
	////////////////////////////////////////////////////////geometry in every feature
	/*static void populate( GEOGeometryH* geomHandle, Symbology::Geometry* target,int numIndex,int numPoints,int sampleInterval);
	static Symbology::Polygon* createPolygon( GEOGeometryH* geomHandle,int sampleInterval );
	static Symbology::Geometry* createGeometry( GEOGeometryH* geomHandle,int sampleInterval);
	static Feature* createFeature( GEOFeatureH* handle, const SpatialReference* srs,int sampleInterval );*/
};


#endif // OSGEARTHFEATURES_FEATURE_GEOFEATURE_UTILS

