#include "stdafx.h"
#include "GEOFeatureUtils.h"
#include <iostream>
#define LC "[FeatureSource] "

using namespace osgEarth::Features;


//将吉奥几何体的坐标数据传给osgEarth中的几何体坐标
void
	GEOFeatureUtils::populate( GEOGeometryH* geomHandle, Symbology::Geometry* target, int numIndex ,int numPoints)
{
	for( int v=numIndex;v<numIndex+numPoints; v=v+1 ) // reverse winding.. we like ccw,geoglobe ′?·?μ?μ??3Dò?aíaè|??ê±??￡??úè|?3ê±??
	{
		double x=0, y=0, z=0;
		//OGR_G_GetPoint( geomHandle, v, &x, &y, &z );
		geomHandle->getGeoPoint(v,x,y,z);
		osg::Vec3d p( x, y, z );
		if ( target->size() == 0 || p != target->back() ) // remove dupes
			target->push_back( p );
	}
}

void
	GEOFeatureUtils::populate( GEOGeometryH* geomHandle, Symbology::Geometry* target, int numIndex ,int numPoints,float height)
{
	for( int v=numIndex;v<numIndex+numPoints; v=v+1 ) // reverse winding.. we like ccw,geoglobe ′?·?μ?μ??3Dò?aíaè|??ê±??￡??úè|?3ê±??
	{
		double x=0, y=0, z=0;
		//OGR_G_GetPoint( geomHandle, v, &x, &y, &z );
		geomHandle->getGeoPoint(v,x,y,z);
		osg::Vec3d p( x, y, height);
		if ( target->size() == 0 || p != target->back() ) // remove dupes
			target->push_back( p );
	}
}
//由吉奥的几何体创建osgEarth中的多边形几何体
Symbology::Polygon*
	GEOFeatureUtils::createPolygon( GEOGeometryH* geomHandle )
{
	Symbology::Polygon* output = 0L;

	int numParts =geomHandle->getNumInfo() ;//获取当前多边形的个数
	int numPartPoints;
	int type;
	std::vector<osg::Vec3d>  chas;
	double x=0, y=0, z=0;
	if ( numParts == 1 )//简单多边形
	{
		int numPoints =  geomHandle->getNumPoints();
		output = new Symbology::Polygon( numPoints );

		geomHandle->getGeoPoint(0,x,y,z);
		osg::Vec3d pt1( x, y, z );

		geomHandle->getGeoPoint(1,x,y,z);
		osg::Vec3d pt2( x, y, z );

		geomHandle->getGeoPoint(2,x,y,z);
		osg::Vec3d pt3( x, y, z );

		osg::Vec3d cha=(pt3-pt2)^(pt2-pt1);
		cha.normalize();
		chas.push_back(cha);
		populate( geomHandle, output,0,numPoints);
		output->rewind( Symbology::Ring::ORIENTATION_CCW );
	}
	else if ( numParts > 1 )//复杂多边形
	{
		
		for(int p=0;p<numParts;p++)
		{
			numPartPoints=geomHandle->getPartPoints(p);
			type=geomHandle->getPartType(p);
			if (type==3)
			{
				//?D???3??
				output = new Symbology::Polygon( numPartPoints );
				

				int v1=geomHandle->getPartOffset(p);
				geomHandle->getGeoPoint(v1,x,y,z);
				osg::Vec3d pt1( x, y, z );

				geomHandle->getGeoPoint(v1+1,x,y,z);
				osg::Vec3d pt2( x, y, z );

				geomHandle->getGeoPoint(v1+2,x,y,z);
				osg::Vec3d pt3( x, y, z );

				osg::Vec3d cha=(pt3-pt2)^(pt2-pt1);
				cha.normalize();
				chas.push_back(cha);
				
			}
		}
	    bool same=false;
		if (chas[1]*chas[0]>0)
		{
		     same=true;
		}


		
			numPartPoints=geomHandle->getPartPoints(1);
			
			output = new Symbology::Polygon( numPartPoints );
			populate( geomHandle, output, geomHandle->getPartOffset(1),numPartPoints);
			output->rewind( Symbology::Ring::ORIENTATION_CCW );
			
		    numPartPoints=geomHandle->getPartPoints(0);
			Symbology::Ring* hole = new Symbology::Ring( numPartPoints );
			populate( geomHandle, hole, geomHandle->getPartOffset(0),numPartPoints);
			hole->rewind( Symbology::Ring::ORIENTATION_CW );
			output->getHoles().push_back( hole );
			
		
	}
	return output;
}

Symbology::Polygon*
	GEOFeatureUtils::createPolygon( GEOGeometryH* geomHandle,float height )
{
	Symbology::Polygon* output = 0L;

	int numParts =geomHandle->getNumInfo() ;//èy?a×é??êy
	int numPartPoints;
	int type;
	std::vector<osg::Vec3d>  chas;
	double x=0, y=0, z=0;
	if ( numParts == 1 )//1???òμ￥?à±?D?
	{
		int numPoints =  geomHandle->getNumPoints();
		output = new Symbology::Polygon( numPoints );

		geomHandle->getGeoPoint(0,x,y,z);
		osg::Vec3d pt1( x, y, z );

		geomHandle->getGeoPoint(1,x,y,z);
		osg::Vec3d pt2( x, y, z );

		geomHandle->getGeoPoint(2,x,y,z);
		osg::Vec3d pt3( x, y, z );

		osg::Vec3d cha=(pt3-pt2)^(pt2-pt1);
		cha.normalize();
		chas.push_back(cha);
		populate( geomHandle, output,0,numPoints,height);
		output->rewind( Symbology::Ring::ORIENTATION_CCW );
	}
	else if ( numParts > 1 )//′?μoààDí?à±?D?
	{
		
		//′?′??úèYóD′óá?D1??￡????ò??ê???2?′|àí￡???á÷3ìoá?Tó°?ì?￡é?3y?￡zhangli2016-11-04
		/*for(int p=0;p<numParts;p++)
		{
			numPartPoints=geomHandle->getPartPoints(p);
			type=geomHandle->getPartType(p);
			if (type==3)
			{
				//?D???3??
				output = new Symbology::Polygon( numPartPoints );
				

				int v1=geomHandle->getPartOffset(p);
				geomHandle->getGeoPoint(v1,x,y,z);
				osg::Vec3d pt1( x, y, z );

				geomHandle->getGeoPoint(v1+1,x,y,z);
				osg::Vec3d pt2( x, y, z );

				geomHandle->getGeoPoint(v1+2,x,y,z);
				osg::Vec3d pt3( x, y, z );

				osg::Vec3d cha=(pt3-pt2)^(pt2-pt1);
				cha.normalize();
				chas.push_back(cha);
				
			}
		}
	    bool same=false;
		if (chas[1]*chas[0]>0)
		{
		     same=true;
		}*/
		//for(int p=0;p<numParts;p++)
		//{
		//	numPartPoints=geomHandle->getPartPoints(p);
		//	if ( p == 0 )
		//	{
		//		output = new Symbology::Polygon( numPartPoints );
		//		populate( geomHandle, output, geomHandle->getPartOffset(p),numPartPoints);
		//		output->rewind( Symbology::Ring::ORIENTATION_CCW );
		//	}
		//	else
		//	{
		//		Symbology::Ring* hole = new Symbology::Ring( numPartPoints );
		//		populate( geomHandle, hole, geomHandle->getPartOffset(p),numPartPoints);
		//		//hole->open();
		//		hole->rewind( Symbology::Ring::ORIENTATION_CW );
		//		output->getHoles().push_back( hole );
		//	}
		//}

		
			numPartPoints=geomHandle->getPartPoints(1);
			
			output = new Symbology::Polygon( numPartPoints );
			populate( geomHandle, output, geomHandle->getPartOffset(1),numPartPoints,height);
			output->rewind( Symbology::Ring::ORIENTATION_CCW );
			
		    numPartPoints=geomHandle->getPartPoints(0);
			Symbology::Ring* hole = new Symbology::Ring( numPartPoints );
			populate( geomHandle, hole, geomHandle->getPartOffset(0),numPartPoints,height);
			hole->rewind( Symbology::Ring::ORIENTATION_CW );
			output->getHoles().push_back( hole );
			
		
	}
	//?à???à±?D?μ??Yê±?á±?
	//else if ( numParts > 0 )
	//{
	//	for( int p = 0; p < numParts; p++ )
	//	{
	//		/*GEOGeometryH partRef = OGR_G_GetGeometryRef( geomHandle, p );
	//		int numPoints = OGR_G_GetPointCount( partRef );*/
	//		GEOGeometryH partRef ;
	//		int numPoints = partRef.getNumPoints();
	//		if ( p == 0 )
	//		{
	//			output = new Symbology::Polygon( numPoints );
	//			populate( partRef, output, numPoints );
	//			//output->open();
	//			output->rewind( Symbology::Ring::ORIENTATION_CCW );
	//		}
	//		else
	//		{
	//			Symbology::Ring* hole = new Symbology::Ring( numPoints );
	//			populate( partRef, hole, numPoints );
	//			//hole->open();
	//			hole->rewind( Symbology::Ring::ORIENTATION_CW );
	//			output->getHoles().push_back( hole );
	//		}
	//	}
	//}
	return output;
}
//由吉奥的几何体创建osgEarth中的几何体，包括不同的类型
Symbology::Geometry*
	GEOFeatureUtils::createGeometry( GEOGeometryH* geomHandle )
{
	Symbology::Geometry* output = 0L;

	geoGEOMETRYTYPE geoType = (geoGEOMETRYTYPE)geomHandle->getGeomType();        

	if (geoType == GEO_GEOMETRY_POLYGON)
	{
		output = createPolygon( geomHandle );
	}
	else if (geoType == GEO_GEOMETRY_PATH || geoType==GEO_GEOMETRY_LINE||
		geoType == GEO_GEOMETRY_POLYLINE )
	{
		int numPoints = geomHandle->getNumPoints();
		output = new Symbology::LineString( numPoints );
		populate( geomHandle, output,0, numPoints );
	}
	else if (
		geoType == GEO_GEOMETRY_RING )
	{
		int numPoints = geomHandle->getNumPoints();
		output = new Symbology::Ring( numPoints );
		populate( geomHandle, output,0, numPoints );
	}
	else if ( geoType == GEO_GEOMETRY_POINT)
	{
		int numPoints = geomHandle->getNumPoints();
		output = new Symbology::PointSet( numPoints );
		populate( geomHandle, output,0, numPoints );
	}
	else if (
		geoType == GEO_GEOMETRY_COLLECTION ||
		geoType == GEO_GEOMETRY_MULTIPOINT )
	{
		Symbology::MultiGeometry* multi = new Symbology::MultiGeometry();

		int numGeoms = geomHandle->getNumPoints();
		/*int numParts =geomHandle->getNumInfo() ;
		if(numParts>1)
		{
		std::cout<<"numParts="<<numParts<<std::endl ;
		for (int i=0;i<numParts;i++)
		{
		int num=geomHandle->getPartPoints(i);
		std::cout<<"Part="<<i<<","<<"PartPoints="<<num<<","
		<<"("<<geomHandle->_infoData[i*3]<<","<<geomHandle->_infoData[i*3+1]<<","<<geomHandle->_infoData[i*3+2]<<")"<<std::endl ;
		}
		}*/
		for( int n=0; n<numGeoms; n++ )
		{
		/*	GEOGeometryH subGeomRef = OGR_G_GetGeometryRef( geomHandle, n );*/
			
			/*if ( subGeomRef )
			{
			Symbology::Geometry* geom = createGeometry( subGeomRef );
			if ( geom ) multi->getComponents().push_back( geom );
			}*/
		} 

		output = multi;
	}

	return output;
}

Symbology::Geometry*
	GEOFeatureUtils::createGeometry( GEOGeometryH* geomHandle,float height)
{
	Symbology::Geometry* output = 0L;

	geoGEOMETRYTYPE geoType = (geoGEOMETRYTYPE)geomHandle->getGeomType();        

	if (geoType == GEO_GEOMETRY_POLYGON)
	{
		output = createPolygon( geomHandle,height);
	}
	else if (geoType == GEO_GEOMETRY_PATH || geoType==GEO_GEOMETRY_LINE||
		geoType == GEO_GEOMETRY_POLYLINE )
	{
		int numPoints = geomHandle->getNumPoints();
		output = new Symbology::LineString( numPoints );
		populate( geomHandle, output,0, numPoints,height);
	}
	else if (
		geoType == GEO_GEOMETRY_RING )
	{
		int numPoints = geomHandle->getNumPoints();
		output = new Symbology::Ring( numPoints );
		populate( geomHandle, output,0, numPoints,height);
	}
	else if ( geoType == GEO_GEOMETRY_POINT)
	{
		int numPoints = geomHandle->getNumPoints();
		output = new Symbology::PointSet( numPoints );
		populate( geomHandle, output,0, numPoints ,height);
	}
	else if (
		geoType == GEO_GEOMETRY_COLLECTION ||
		geoType == GEO_GEOMETRY_MULTIPOINT )
	{
		Symbology::MultiGeometry* multi = new Symbology::MultiGeometry();

		int numGeoms = geomHandle->getNumPoints();
		/*int numParts =geomHandle->getNumInfo() ;
		if(numParts>1)
		{
		std::cout<<"numParts="<<numParts<<std::endl ;
		for (int i=0;i<numParts;i++)
		{
		int num=geomHandle->getPartPoints(i);
		std::cout<<"Part="<<i<<","<<"PartPoints="<<num<<","
		<<"("<<geomHandle->_infoData[i*3]<<","<<geomHandle->_infoData[i*3+1]<<","<<geomHandle->_infoData[i*3+2]<<")"<<std::endl ;
		}
		}*/
		for( int n=0; n<numGeoms; n++ )
		{
		/*	GEOGeometryH subGeomRef = OGR_G_GetGeometryRef( geomHandle, n );*/
			
			/*if ( subGeomRef )
			{
			Symbology::Geometry* geom = createGeometry( subGeomRef );
			if ( geom ) multi->getComponents().push_back( geom );
			}*/
		} 

		output = multi;
	}

	return output;
}
Feature*
	GEOFeatureUtils::createFeature( GEOFeatureH* handle, const SpatialReference* srs)
{
	long fid = handle->getOid();

	GEOGeometryH* geomRef = handle->getGeomH();	

	Symbology::Geometry* geom = 0;

	if ( geomRef)
	{
		geom = GEOFeatureUtils::createGeometry( geomRef );
	}

	Feature* feature = new Feature( geom, srs, Style(), fid );

	return feature;
}

Feature*
	GEOFeatureUtils::createFeature( GEOFeatureH* handle, const SpatialReference* srs,float height)
{
	long fid = handle->getOid();

	GEOGeometryH* geomRef = handle->getGeomH();	

	Symbology::Geometry* geom = 0;

	if ( geomRef)
	{
		geom = GEOFeatureUtils::createGeometry(geomRef,height);
	}

	Feature* feature = new Feature( geom, srs, Style(), fid );

	return feature;
}
////////////////////////////////////////////////////////////////////////////////////////////create the feature and geometry by the spaceInterval
//void
//	GEOFeatureUtils::populate( GEOGeometryH* geomHandle, Symbology::Geometry* target, int numIndex ,int numPoints,int sampleInterval)
//{
//	for( int v=numIndex;v<numIndex+numPoints; v=v+sampleInterval) // reverse winding.. we like ccw,geoglobe ′?·?μ?μ??3Dò?aíaè|??ê±??￡??úè|?3ê±??
//	{
//		double x=0, y=0, z=0;
//		//OGR_G_GetPoint( geomHandle, v, &x, &y, &z );
//		geomHandle->getGeoPoint(v,x,y,z);
//		osg::Vec3d p( x, y, z );
//		if ( target->size() == 0 || p != target->back() ) // remove dupes
//			target->push_back( p );
//	}
//}
//
//Symbology::Polygon*
//	GEOFeatureUtils::createPolygon( GEOGeometryH* geomHandle,int sampleInterval)
//{
//	Symbology::Polygon* output = 0L;
//
//	int numParts =geomHandle->getNumInfo() ;//èy?a×é??êy
//	int numPartPoints;
//	int type;
//	std::vector<osg::Vec3d>  chas;
//	double x=0, y=0, z=0;
//	if ( numParts == 1 )//1???òμ￥?à±?D?
//	{
//		int numPoints =  geomHandle->getNumPoints();
//		output = new Symbology::Polygon( numPoints );
//
//		geomHandle->getGeoPoint(0,x,y,z);
//		osg::Vec3d pt1( x, y, z );
//
//		geomHandle->getGeoPoint(1,x,y,z);
//		osg::Vec3d pt2( x, y, z );
//
//		geomHandle->getGeoPoint(2,x,y,z);
//		osg::Vec3d pt3( x, y, z );
//
//		osg::Vec3d cha=(pt3-pt2)^(pt2-pt1);
//		cha.normalize();
//		chas.push_back(cha);
//		populate( geomHandle, output,0,numPoints);
//		output->rewind( Symbology::Ring::ORIENTATION_CCW );
//	}
//	else if ( numParts > 1 )//′?μoààDí?à±?D?
//	{
//		
//		for(int p=0;p<numParts;p++)
//		{
//			numPartPoints=geomHandle->getPartPoints(p);
//			type=geomHandle->getPartType(p);
//			if (type==3)
//			{
//				//?D???3??
//				output = new Symbology::Polygon( numPartPoints );
//				
//
//				int v1=geomHandle->getPartOffset(p);
//				geomHandle->getGeoPoint(v1,x,y,z);
//				osg::Vec3d pt1( x, y, z );
//
//				geomHandle->getGeoPoint(v1+1,x,y,z);
//				osg::Vec3d pt2( x, y, z );
//
//				geomHandle->getGeoPoint(v1+2,x,y,z);
//				osg::Vec3d pt3( x, y, z );
//
//				osg::Vec3d cha=(pt3-pt2)^(pt2-pt1);
//				cha.normalize();
//				chas.push_back(cha);
//				
//			}
//		}
//	    bool same=false;
//		if (chas[1]*chas[0]>0)
//		{
//		     same=true;
//		}
//		//for(int p=0;p<numParts;p++)
//		//{
//		//	numPartPoints=geomHandle->getPartPoints(p);
//		//	if ( p == 0 )
//		//	{
//		//		output = new Symbology::Polygon( numPartPoints );
//		//		populate( geomHandle, output, geomHandle->getPartOffset(p),numPartPoints);
//		//		output->rewind( Symbology::Ring::ORIENTATION_CCW );
//		//	}
//		//	else
//		//	{
//		//		Symbology::Ring* hole = new Symbology::Ring( numPartPoints );
//		//		populate( geomHandle, hole, geomHandle->getPartOffset(p),numPartPoints);
//		//		//hole->open();
//		//		hole->rewind( Symbology::Ring::ORIENTATION_CW );
//		//		output->getHoles().push_back( hole );
//		//	}
//		//}
//
//		
//			numPartPoints=geomHandle->getPartPoints(1);
//			
//			output = new Symbology::Polygon( numPartPoints );
//			populate( geomHandle, output, geomHandle->getPartOffset(1),numPartPoints);
//			output->rewind( Symbology::Ring::ORIENTATION_CCW );
//			
//		    numPartPoints=geomHandle->getPartPoints(0);
//			Symbology::Ring* hole = new Symbology::Ring( numPartPoints );
//			populate( geomHandle, hole, geomHandle->getPartOffset(0),numPartPoints);
//			hole->rewind( Symbology::Ring::ORIENTATION_CW );
//			output->getHoles().push_back( hole );
//			
//		
//	}
//	return output;
//}
//
//Symbology::Geometry*
//	GEOFeatureUtils::createGeometry( GEOGeometryH* geomHandle,int sampleInterval)
//{
//	Symbology::Geometry* output = 0L;
//
//	geoGEOMETRYTYPE geoType = (geoGEOMETRYTYPE)geomHandle->getGeomType();        
//
//	if (geoType == GEO_GEOMETRY_POLYGON)
//	{
//		output = createPolygon( geomHandle );
//	}
//	else if (geoType == GEO_GEOMETRY_PATH || geoType==GEO_GEOMETRY_LINE||
//		geoType == GEO_GEOMETRY_POLYLINE )
//	{
//		int numPoints = geomHandle->getNumPoints();
//		output = new Symbology::LineString( numPoints );
//		populate( geomHandle, output,0, numPoints,sampleInterval);
//	}
//	else if (
//		geoType == GEO_GEOMETRY_RING )
//	{
//		int numPoints = geomHandle->getNumPoints();
//		output = new Symbology::Ring( numPoints );
//		populate( geomHandle, output,0, numPoints );
//	}
//	else if ( geoType == GEO_GEOMETRY_POINT)
//	{
//		int numPoints = geomHandle->getNumPoints();
//		output = new Symbology::PointSet( numPoints );
//		populate( geomHandle, output,0, numPoints );
//	}
//	else if (
//		geoType == GEO_GEOMETRY_COLLECTION ||
//		geoType == GEO_GEOMETRY_MULTIPOINT )
//	{
//		Symbology::MultiGeometry* multi = new Symbology::MultiGeometry();
//
//		int numGeoms = geomHandle->getNumPoints();
//		/*int numParts =geomHandle->getNumInfo() ;
//		if(numParts>1)
//		{
//		std::cout<<"numParts="<<numParts<<std::endl ;
//		for (int i=0;i<numParts;i++)
//		{
//		int num=geomHandle->getPartPoints(i);
//		std::cout<<"Part="<<i<<","<<"PartPoints="<<num<<","
//		<<"("<<geomHandle->_infoData[i*3]<<","<<geomHandle->_infoData[i*3+1]<<","<<geomHandle->_infoData[i*3+2]<<")"<<std::endl ;
//		}
//		}*/
//		for( int n=0; n<numGeoms; n++ )
//		{
//		/*	GEOGeometryH subGeomRef = OGR_G_GetGeometryRef( geomHandle, n );*/
//			
//			/*if ( subGeomRef )
//			{
//			Symbology::Geometry* geom = createGeometry( subGeomRef );
//			if ( geom ) multi->getComponents().push_back( geom );
//			}*/
//		} 
//
//		output = multi;
//	}
//
//	return output;
//}
//Feature*
//	GEOFeatureUtils::createFeature( GEOFeatureH* handle, const SpatialReference* srs,int sampleInterval)
//{
//	long fid = handle->getOid();
//
//	GEOGeometryH* geomRef = handle->getGeomH();	
//
//	Symbology::Geometry* geom = 0;
//
//	if ( geomRef)
//	{
//		geom = GEOFeatureUtils::createGeometry( geomRef,sampleInterval);
//	}
//
//	Feature* feature = new Feature( geom, srs, Style(), fid );
//
//	return feature;
//}