/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
 * Copyright 2008-2014 Pelican Mapping
 * http://osgearth.org
 *
 * osgEarth is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <osgEarthFeatures/FeatureTileSource>
#include <osgEarthFeatures/ResampleFilter>
#include <osgEarthFeatures/TransformFilter>
#include <osgEarthFeatures/BufferFilter>
#include <osgEarthSymbology/Style>
//TODO: replace this with GeometryRasterizer
#include <osgEarthSymbology/AGG.h>
#include <osgEarth/Registry>
#include <osgEarth/FileUtils>
#include <osgEarthFeatures/CropFilter>
#include <osgEarthSymbology/Geometry>
#include <osgEarth/ThreadingUtils>

#include <osg/Vec2d>
#include <osg/Notify>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <vector>
#include "Vector/GEORasterizerOptions.h"
#include "Vector/FeatureSourceGEO.h"
#include "Vector/FeatureCursorGEO.h"
#include <sstream>
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <osg/Timer>
//#include <android/log.h>
//#define LC "[GEORasterizer] "
#define SQLITE_SCOPED_LOCK GDAL_SCOPED_LOCK

using namespace osgEarth::Features;
using namespace osgEarth::Symbology;
//using namespace osgEarth::Drivers;
using namespace OpenThreads;

/********************************************************************/

class GEORasterizerTileSource : public FeatureTileSource
{
public:
    struct RenderFrame {
        double xmin, ymin;
        double xmax, ymax;
        double xf, yf;
    };

public:
    GEORasterizerTileSource( const TileSourceOptions& options ) : FeatureTileSource( options ),
        _options( options )
    {
        //nop
    }

    TileSource::Status
    initialize(const osgDB::Options* dbOptions)
	{
		_bAllgetfea=false;
		if ( !getProfile() )
		{
			setProfile( osgEarth::Registry::instance()->getGlobalGeodeticProfile() );
		}

		if ( _features.valid() )
		{
			_features->initialize( dbOptions );

			// Try to fill the DataExtent list using the FeatureProfile
			const FeatureProfile* featureProfile = _features->getFeatureProfile();
			if (featureProfile != NULL)
			{
				if (featureProfile->getProfile() != NULL)
				{
					// Use specified profile's GeoExtent
					DataExtent dext(featureProfile->getProfile()->getExtent());
					getDataExtents().push_back(dext);
					if (dext.bounds().height()>45&&dext.bounds().width()>45)
					{
						_bAllgetfea=true;
					}

				}
				else if (featureProfile->getExtent().isValid() == true)
				{
					// Use FeatureProfile's GeoExtent
					DataExtent dext(featureProfile->getExtent());
					getDataExtents().push_back(dext);
					if (dext.bounds().height()>45&&dext.bounds().width()>45)
					{
						_bAllgetfea=true;
					}
				}
			}
		}
		else
		{
			return Status::Error("No FeatureSource provided; nothing will be rendered");
		}

		_initialized = true;
		return STATUS_OK;
	}

	//override
	osg::Image*
		createImage( const osgEarth::TileKey& key, osgEarth::ProgressCallback* progress )
	{
		osgEarth::GeoExtent imageExtent=key.getExtent();
	    int level=key.getLevelOfDetail();
	    if(level<3 || level>20)
	    {
	    	return 0L;
	    }
		//SQLITE_SCOPED_LOCK;
		osg::ref_ptr<osg::Image> image = NULL;
		StyleSheet* styles=NULL;
		Style style;
		double gamma =1.3;
//		__android_log_print(ANDROID_LOG_INFO,"createImage","1");
		{
			Threading::ScopedWriteLock exclusive1(_lockMutex);
			//		SQLITE_SCOPED_LOCK;
			if ( !_features.valid() || !_features->getFeatureProfile() )
				return 0L;
			// style data
			styles = _options.styles();
			gamma = _options.gamma().get();
		}

//		__android_log_print(ANDROID_LOG_INFO,"createImage","if feature and get style done");


		std::vector<int> queryOids;
		osg::ref_ptr<GeoGlobe::GeoVector::GEOFeatureSource> tmpfs=NULL;
		{
			Threading::ScopedWriteLock exclusive2(_lockMutex);
			tmpfs=(GeoGlobe::GeoVector::GEOFeatureSource*)_features.get();

			if(!tmpfs)
			{
				return 0L;
			}
			std::string sql="select oid,xmin,ymin,xmax,ymax from dataindex";
			//__android_log_print(ANDROID_LOG_ERROR, "GEORASTER _bAllgetfea", "is: %d", (int)_bAllgetfea);
			if (tmpfs->getFeatureCount()>500 && (!_bAllgetfea))
			{
				std::stringstream buf;
				if (level>=1 && level<=12)//0-3�㲻��ʾ,4-15�ּ���Ⱦ,4,5,6���ã�7��8��9��10��11��12��
				{
					for(int i=1;i<=level;i++)
					{
						//int tem=(int)1000*pow(0.8,i);
						int tem=(int)500*pow(0.8,i);
						if (i==1)
						{
							buf<<" where oid%"<<tem<<"==0";
						}
						else
						{
							buf<<" or oid%"<<tem<<"==0";
						}

					}
					sql=sql+buf.str();
				}
			}

//			__android_log_print(ANDROID_LOG_INFO,"queryImage","5");

			tmpfs->_geoFeaClass->excuteSeletSqlandQuerytoGetOID(imageExtent.bounds(),sql,queryOids);
		}
		//__android_log_print(ANDROID_LOG_ERROR, "GEORASTER queryOids", "is: %d", queryOids.size());
		if(queryOids.size()>0)
		{
//			__android_log_print(ANDROID_LOG_INFO,"createImage","new image done");
			image = new osg::Image();
			//image =osgDB::readImageFile("/storage/emulated/0/osgEarth/DATA/tempt.png");
			image->allocateImage( 256,256, 1, GL_RGBA, GL_UNSIGNED_BYTE );
			//image->s
//			__android_log_print(ANDROID_LOG_INFO,"createImage","allocatimage done");

			agg::rendering_buffer rbuf( image->data(), image->s(), image->t(), image->s()*4 );
			agg::renderer<agg::span_abgr32> ren(rbuf);
			ren.clear(agg::rgba8(0,0,0,0));

			RenderFrame frame;
			frame.xmin = imageExtent.xMin();
			frame.ymin = imageExtent.yMin();
			frame.xmax = imageExtent.xMax();
			frame.ymax = imageExtent.yMax();
			frame.xf   = (double)image->s() / imageExtent.width();
			frame.yf   = (double)image->t() / imageExtent.height();

			agg::rasterizer ras;
			ras.gamma(gamma);
			ras.filling_rule(agg::fill_even_odd);

			LineSymbol*    styleLine=NULL;
			PolygonSymbol* stylePoly=NULL;
			if(styles)
			{
				styleLine = styles->getDefaultStyle()->getSymbol<LineSymbol>();
				stylePoly = styles->getDefaultStyle()->getSymbol<PolygonSymbol>();
			}
			else
			{

				styleLine = style.getSymbol<LineSymbol>();
				stylePoly = style.getSymbol<PolygonSymbol>();
			}

			double distance;
			agg::rgba8 LinefgColor;
			agg::rgba8 PolyfgColor;
			if (styleLine)
			{

			   distance = styleLine->stroke()->width().value() * 0.5 * (imageExtent.width() / (double)image->s());

			   const osg::Vec4 colorPoly = static_cast<osg::Vec4>(styleLine->stroke()->color());

			   osg::Vec4 c = colorPoly;
			   unsigned int a = (unsigned int)(127+(c.a()*255)/2); // scale alpha up
			   LinefgColor = agg::rgba8( (unsigned int)(c.r()*255), (unsigned int)(c.g()*255), (unsigned int)(c.b()*255), a );
			}
			if (stylePoly)
			{
				const osg::Vec4 colorPoly = static_cast<osg::Vec4>(stylePoly->fill()->color());

				osg::Vec4 c = colorPoly;
				unsigned int a = (unsigned int)(127+(c.a()*255)/2); // scale alpha up
				PolyfgColor = agg::rgba8( (unsigned int)(c.r()*255), (unsigned int)(c.g()*255), (unsigned int)(c.b()*255), a );
			}
			{
				Threading::ScopedWriteLock exclusive3(_lockMutex);

				for(std::vector<int>::iterator i=queryOids.begin();i!=queryOids.end();i++)//��ÿ��feature����
				{
					GEOFeatureH handle;
					tmpfs->_geoFeaClass->createGEOFeatureH(*i,&handle);
					GEOGeometryH* geomRef=handle.getGeomH();
//					__android_log_print(ANDROID_LOG_INFO,"queryImage","getgeom");
					/////////////////////////////////////////////////////////////poly
					if (stylePoly)
					{
//						__android_log_print(ANDROID_LOG_INFO,"queryImage","7poly begin");
						int nump=geomRef->getNumPoints();
						for (int i=0;i<nump;i++)
						{
							osg::Vec2d p0;
							p0=geomRef->getGeoPoint(i);
							double x = osg::clampBetween(p0.x(),frame.xmin,frame.xmax);
							double y = osg::clampBetween(p0.y(),frame.ymin,frame.ymax);
							double x0 = frame.xf*(x-frame.xmin);
							double y0 = frame.yf*(y-frame.ymin);
							if (i==0)
								ras.move_to_d( x0, y0 );
							else
								ras.line_to_d( x0, y0 );
						}//for point
						ras.render(ren, PolyfgColor);
						ras.reset();
//						__android_log_print(ANDROID_LOG_INFO,"queryImage","7poly end");
					}//pology


					///////////////////////////////////////////////////////////////////line
					if (styleLine)
					{
//						__android_log_print(ANDROID_LOG_INFO,"queryImage","7line begin");
						Symbology::Geometry* geom = GEOFeatureUtils::createGeometry( geomRef);

						osg::ref_ptr<Symbology::Geometry> output;

						Symbology::BufferParameters params;

						params._capStyle = Symbology::BufferParameters::CAP_FLAT;

						params._cornerSegs = 1;

						geom->buffer(distance,output,params);

//						__android_log_print(ANDROID_LOG_INFO,"queryImage","7line between");
						for(  osgEarth::Symbology::Geometry::const_iterator p = output->begin(); p != output->end(); p++ )
						{
						const osg::Vec3d& p0 = *p;
//						double x = osg::clampBetween(p0.x(),frame.xmin,frame.xmax);
//						double y = osg::clampBetween(p0.y(),frame.ymin,frame.ymax);
//						double x0 = frame.xf*(x-frame.xmin);
//						double y0 = frame.yf*(y-frame.ymin);
						double x0 = frame.xf*(p0.x()-frame.xmin);
						double y0 = frame.yf*(p0.y()-frame.ymin);
						if ( p == output->begin() )
						   ras.move_to_d( x0, y0 );
						else
						   ras.line_to_d( x0, y0 );
						}
						ras.render(ren, LinefgColor);
						ras.reset();
					}//line
				}//for feature
//				__android_log_print(ANDROID_LOG_INFO,"queryImage","7line end");
				//convert from ABGR to RGBA
				unsigned char* pixel = image->data();
				for(int i=0; i<image->s()*image->t()*4; i+=4, pixel+=4)
				{
					std::swap( pixel[0], pixel[3] );
					std::swap( pixel[1], pixel[2] );
				}
			}

		}//query oid>0


		// allocate the image.
		//osg::ref_ptr<osg::Image> image = new osg::Image();
//		__android_log_print(ANDROID_LOG_INFO,"createImage","new image done");
//		osg::ref_ptr<osg::Image> image =osgDB::readImageFile("/storage/emulated/0/osgEarth/DATA/tempt.png");
//		//image->allocateImage( 256,256, 1, GL_RGBA, GL_UNSIGNED_BYTE );
//		//image->s
//		__android_log_print(ANDROID_LOG_INFO,"createImage","allocatimage done");

//		preProcess( image.get());
//
//		// figure out if and how to style the geometry.
//		if ( styles )
//		{
//			const Style* style = styles->getDefaultStyle();
//			Query query;
//			query.tileKey()=key;
//
//			__android_log_print(ANDROID_LOG_INFO,"createImage","2");
//
//			osg::Timer_t cur1=osg::Timer::instance()->tick();
//			queryAndRenderFeaturesToRasterizerByStyle( *style,query,image.get());
//			osg::Timer_t cur2=osg::Timer::instance()->tick();
//			OE_NOTICE<<"queryandrenderfeature complete in"<<osg::Timer::instance()->delta_s(cur1,cur2)<<"s"<<std::endl;
//
//		}
//		else
//		{
//			Query query;
//			query.tileKey()=key;
//			__android_log_print(ANDROID_LOG_INFO,"createImage","style else error");
//			queryAndRenderFeaturesToRasterizerByStyle( Style(),query, image.get() );
//		}
//
//		// final tile processing after all styles are done
//		postProcess( image.get());

//		__android_log_print(ANDROID_LOG_ERROR,"createImage","done");
		return image.release();

	}

	bool queryAndRenderFeaturesToRasterizerByStyle(const Style& style,const Query& query,osg::Image* image)
	{   
//		__android_log_print(ANDROID_LOG_INFO,"queryImage","3");
//		SQLITE_SCOPED_LOCK;
		osgEarth::GeoExtent imageExtent=(*query.tileKey()).getExtent();
	    int level=(*query.tileKey()).getLevelOfDetail();
		
		RenderFrame frame;
		frame.xmin = imageExtent.xMin();
		frame.ymin = imageExtent.yMin();
		frame.xf   = (double)image->s() / imageExtent.width();
		frame.yf   = (double)image->t() / imageExtent.height();

//		__android_log_print(ANDROID_LOG_INFO,"queryImage","4");
		agg::rendering_buffer rbuf( image->data(), image->s(), image->t(), image->s()*4 );

		// Create the renderer and the rasterizer
		agg::renderer<agg::span_abgr32> ren(rbuf);
		agg::rasterizer ras;

		// Setup the rasterizer
		ras.gamma(_options.gamma().get());
		ras.filling_rule(agg::fill_even_odd);
//		__android_log_print(ANDROID_LOG_INFO,"queryImage","set up the rasterizer end");

		const LineSymbol*    styleLine = style.getSymbol<LineSymbol>();
		const PolygonSymbol* stylePoly = style.getSymbol<PolygonSymbol>();

		double distance;
		agg::rgba8 LinefgColor;
		agg::rgba8 PolyfgColor;
		if (styleLine)
		{

		   distance = styleLine->stroke()->width().value() * 0.5 * (imageExtent.width() / (double)image->s());

		   const osg::Vec4 colorPoly = static_cast<osg::Vec4>(styleLine->stroke()->color());

		   osg::Vec4 c = colorPoly;
		   unsigned int a = (unsigned int)(127+(c.a()*255)/2); // scale alpha up
		   LinefgColor = agg::rgba8( (unsigned int)(c.r()*255), (unsigned int)(c.g()*255), (unsigned int)(c.b()*255), a );
		}
		if (stylePoly)
		{
			const osg::Vec4 colorPoly = static_cast<osg::Vec4>(stylePoly->fill()->color());

			osg::Vec4 c = colorPoly;
			unsigned int a = (unsigned int)(127+(c.a()*255)/2); // scale alpha up
			PolyfgColor = agg::rgba8( (unsigned int)(c.r()*255), (unsigned int)(c.g()*255), (unsigned int)(c.b()*255), a );
		}
		std::vector<int> queryOids;
		osg::ref_ptr<GeoGlobe::GeoVector::GEOFeatureSource> tmpfs=NULL;
		{
			Threading::ScopedWriteLock exclusive(_lockMutex);
			tmpfs=(GeoGlobe::GeoVector::GEOFeatureSource*)_features.get();

			if(!tmpfs)
			{
				return 0L;
			}
			std::string sql="select oid,xmin,ymin,xmax,ymax from dataindex";
			//__android_log_print(ANDROID_LOG_ERROR, "GEORASTER _bAllgetfea", "is: %d", (int)_bAllgetfea);
			if (tmpfs->getFeatureCount()>500 && (!_bAllgetfea))
			{
				std::stringstream buf;
				if (level>=1 && level<=12)//0-3�㲻��ʾ,4-15�ּ���Ⱦ,4,5,6���ã�7��8��9��10��11��12��
				{
					for(int i=1;i<=level;i++)
					{
						//int tem=(int)1000*pow(0.8,i);
						int tem=(int)500*pow(0.8,i);
						if (i==1)
						{
							buf<<" where oid%"<<tem<<"==0";
						}
						else
						{
							buf<<" or oid%"<<tem<<"==0";
						}

					}
					sql=sql+buf.str();
				}
			}

//			__android_log_print(ANDROID_LOG_INFO,"queryImage","5");


			tmpfs->_geoFeaClass->excuteSeletSqlandQuerytoGetOID(imageExtent.bounds(),sql,queryOids);

		}
		



//		__android_log_print(ANDROID_LOG_INFO,"queryImage","6");
		//__android_log_print(ANDROID_LOG_ERROR, "GEORASTER queryOids", "is: %d", queryOids.size());
		if (queryOids.size()>0)
		{
				for(std::vector<int>::iterator i=queryOids.begin();i!=queryOids.end();i++)//��ÿ��feature����
				{
					GEOFeatureH handle;
					{
						Threading::ScopedWriteLock exclusive(_lockMutex);
						tmpfs->_geoFeaClass->createGEOFeatureH(*i,&handle);
					}

					GEOGeometryH* geomRef=handle.getGeomH();
//					__android_log_print(ANDROID_LOG_INFO,"queryImage","getgeom");
					/////////////////////////////////////////////////////////////poly
					if (stylePoly)
					{
//						__android_log_print(ANDROID_LOG_INFO,"queryImage","7poly begin");
						int nump=geomRef->getNumPoints();
						for (int i=0;i<nump;i++)
						{
							osg::Vec2d p0;
							p0=geomRef->getGeoPoint(i);
							double x0 = frame.xf*(p0.x()-frame.xmin);
							double y0 = frame.yf*(p0.y()-frame.ymin);
							if (i==0)
								ras.move_to_d( x0, y0 );
							else
								ras.line_to_d( x0, y0 );
						}//for point
						ras.render(ren, PolyfgColor);
						ras.reset();
//						__android_log_print(ANDROID_LOG_INFO,"queryImage","7poly end");
					}//pology
					

					///////////////////////////////////////////////////////////////////line
					if (styleLine)
					{
//						__android_log_print(ANDROID_LOG_INFO,"queryImage","7line begin");
						Symbology::Geometry* geom = GEOFeatureUtils::createGeometry( geomRef);

						osg::ref_ptr<Symbology::Geometry> output;

						Symbology::BufferParameters params;

						params._capStyle = Symbology::BufferParameters::CAP_FLAT;

						params._cornerSegs = 1;

						geom->buffer(distance,output,params);

//						__android_log_print(ANDROID_LOG_INFO,"queryImage","7line between");
						for(  osgEarth::Symbology::Geometry::const_iterator p = output->begin(); p != output->end(); p++ )
						{
						const osg::Vec3d& p0 = *p;
						double x0 = frame.xf*(p0.x()-frame.xmin);
						double y0 = frame.yf*(p0.y()-frame.ymin);

						if ( p == output->begin() )
						   ras.move_to_d( x0, y0 );
						else
						   ras.line_to_d( x0, y0 );
						}
						ras.render(ren, LinefgColor);
						ras.reset();
					}//line
				}//for feature	
//				__android_log_print(ANDROID_LOG_INFO,"queryImage","7line end");
		}// (queryOids.size()>0)
		
		return true;
	}

    //override
//    bool preProcess(osg::Image* image, osg::Referenced* buildData)
//    {
//        agg::rendering_buffer rbuf( image->data(), image->s(), image->t(), image->s()*4 );
//        agg::renderer<agg::span_abgr32> ren(rbuf);
//        ren.clear(agg::rgba8(0,0,0,0));
//        return true;
//    }
//
//    bool postProcess( osg::Image* image, osg::Referenced* data )
//    {
//        //convert from ABGR to RGBA
//        unsigned char* pixel = image->data();
//        for(int i=0; i<image->s()*image->t()*4; i+=4, pixel+=4)
//        {
//            std::swap( pixel[0], pixel[3] );
//            std::swap( pixel[1], pixel[2] );
//        }
//        return true;
//    }
	bool preProcess(osg::Image* image)
	{
		agg::rendering_buffer rbuf( image->data(), image->s(), image->t(), image->s()*4 );
		agg::renderer<agg::span_abgr32> ren(rbuf);
		ren.clear(agg::rgba8(0,0,0,0));
		return true;
	}

	bool postProcess( osg::Image* image)
	{
		//convert from ABGR to RGBA
		unsigned char* pixel = image->data();
		for(int i=0; i<image->s()*image->t()*4; i+=4, pixel+=4)
		{
			std::swap( pixel[0], pixel[3] );
			std::swap( pixel[1], pixel[2] );
		}
		return true;
	}
    // rasterizes a geometry.
    virtual std::string getExtension()  const 
    {
        return "png";
    }

private:
    const GEORasterizerOptions _options;
    bool _bAllgetfea;
    std::string _configPath;
    Threading::ReadWriteMutex      _lockMutex;
};

/**
 * Plugin entry point for the geo AGGLite feature rasterizer
 */
class GEORasterizerTileSourceDriver : public TileSourceDriver
{
    public:
        GEORasterizerTileSourceDriver() {}

        virtual const char* className()
        {
            return "geo feature rasterizer";
        }
        
        virtual bool acceptsExtension(const std::string& extension) const
        {
            return
                osgDB::equalCaseInsensitive( extension, "osgearth_georasterizer" );
        }

        virtual ReadResult readObject(const std::string& file_name, const Options* options) const
        {
            std::string ext = osgDB::getFileExtension( file_name );
            if ( !acceptsExtension( ext ) )
            {
                return ReadResult::FILE_NOT_HANDLED;
            }

            return new GEORasterizerTileSource( getTileSourceOptions(options) );
        }
};

REGISTER_OSGPLUGIN(osgearth_georasterizer,GEORasterizerTileSourceDriver)
