#ifndef OSGEARTH_GEOLayerContolCommon_H
#define OSGEARTH_GEOLayerContolCommon_H 1


#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarth/NodeUtils>
#include <osgEarth/ElevationQuery>
#include <osg/BoundingSphere>



namespace GeoGlobe
{
	namespace Tool
	{
        //吉奥图层控制基础类
		class GEOLayerContorCommon
		{
		public:
            //获得该图层对应的最大可视高度
			static void getAltitudeofLayer(double& altitude,const GeoExtent& extent,const MapNode* mapnode,const double& factor=5.0) 
			{
				osg::BoundingSphered bs =getBoundSphereInWorldCoords(extent,mapnode);
				altitude=bs.radius()*factor;
			}
            //根据该图层的经纬度范围确定其包围盒大小
			static osg::BoundingSphered
				getBoundSphereInWorldCoords(const GeoExtent& extent,const MapNode* mapnode  ) 
			{
				osg::Vec3d center, corner;
			 osgEarth::GeoExtent workingExtent;

				if ( !extent.isValid() )
				{
					return osg::BoundingSphered();
				}

				if ( extent.getSRS()->isEquivalentTo( mapnode->getMapSRS() ) )
				{
					workingExtent = extent;
				}
				else
				{
					workingExtent = extent.transform(  mapnode->getMapSRS() ); // safe.
				}

				workingExtent.getCentroid( center.x(), center.y() );

				double centerZ = 0.0;    
				if ( mapnode )
				{
					// Use an appropriate resolution for this extents width
					double resolution = workingExtent.width();
					ElevationQuery query(mapnode->getMap());
					GeoPoint p( mapnode->getMapSRS(), center, ALTMODE_ABSOLUTE );
					query.getElevation( p, center.z(), resolution );
					centerZ = center.z();
				}    

				corner.x() = workingExtent.xMin();
				corner.y() = workingExtent.yMin();
				corner.z() = 0;

				if ( mapnode->getMap()->isGeocentric() )
				{
					const SpatialReference* ecefSRS = workingExtent.getSRS()->getECEF();
					workingExtent.getSRS()->transform( center, ecefSRS, center );
					workingExtent.getSRS()->transform( corner, ecefSRS, corner );
					//workingExtent.getSRS()->transformToECEF( center, center );
					//workingExtent.getSRS()->transformToECEF( corner, corner );
				}

				if (workingExtent.getSRS()->isGeographic() &&
					( workingExtent.width() >= 90 || workingExtent.height() >= 90 ) )
				{
					return osg::BoundingSphered( osg::Vec3d(0,0,0), 2*center.length() );
				}

				return osg::BoundingSphered( center, (center-corner).length() );
			}

		};
	}
}

#endif


