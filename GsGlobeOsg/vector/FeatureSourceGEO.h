#ifndef OSGEARTHFEATURES_FEATURE_GEOFEATURE_SOURCE_GEO
#define OSGEARTHFEATURES_FEATURE_GEOFEATURE_SOURCE_GEO 1

#include <osgEarth/Registry>
#include <osgEarth/FileUtils>
#include <osgEarth/StringUtils>
#include <osgEarth/GeoData>
#include <osgEarth/Bounds>
#include <osgEarthFeatures/FeatureSource>
#include <osgEarthFeatures/Filter>
#include <osgEarthFeatures/BufferFilter>
#include <osgEarthFeatures/ScaleFilter>
#include <osgEarthFeatures/GeometryUtils>

#include <osg/Notify>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <list>


#include "GEOFeatureOptions.h"
#include "FeatureCursorGEO.h"
#include "GEOFeatureUtils.h"
#include "GeoFeature_SqliteApi.h"
#define GEO_SCOPED_LOCK GDAL_SCOPED_LOCK
#define LC "[GEO FeatureSource] "

namespace GeoGlobe
{
	namespace GeoVector
	{
		
		using namespace osgEarth::Features;


		//吉奥要素来源类
		class GEOFeatureSource : public FeatureSource
		{
		public:
			//构造函数，传入要素图层参数类 GEOFeatureOptions，及其对应的本地数据库文件访问类GEOFeatureClass
			GEOFeatureSource( const GEOFeatureOptions& options ) : FeatureSource( options ),
			 _geoFeaClass(0L),
			  _options( options ),
			  _featureCount(-1),
			  _countFeature(0),
			  _geoHeight(0.0),
			  _needsSync(false)
			  //_writable(false)
			{
				//nop
			}

			/** Destruct the object, cleaning up and OGR handles. */
	virtual ~GEOFeatureSource()
    { 
	   GEO_SCOPED_LOCK;

       if (_geoFeaClass)
       {
		   delete _geoFeaClass;
		   _geoFeaClass=NULL;
       }
    }

 //初始化函数
			void initialize( const osgDB::Options* dbOptions )
			{
				
					if ( _options.url().isSet() )
					{
						_source = _options.url()->full();
						_geoFeaClass=new GEOFeatureClass;
						_geoFeaClass->init(_source);
					}
					else if ( _options.connection().isSet() )
					{
						_source = _options.connection().value();
					}

					// establish the geometry:
					_geometry = 
						_options.geometry().valid()       ? _options.geometry().get() :
						_options.geometryConfig().isSet() ? parseGeometry( *_options.geometryConfig() ) :
						_options.geometryUrl().isSet()    ? parseGeometryUrl( *_options.geometryUrl(), dbOptions ) :
						0L;
					_geoHeight=_options.height().value();
				
			}
			//要素的概括类，包括空间参数等
			const FeatureProfile* createFeatureProfile()
			{
				FeatureProfile* result = 0L;
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// see if we have a custom profile.
				//′′?¨profileèy??·?ê?
				//1------------------------------------------------------------
				osg::ref_ptr<const Profile> profile;
				if ( _options.profile().isSet() )
				{
					profile = Profile::create( *_options.profile() );
				}
				//2-------------------------------------------------------
				if ( _geometry.valid() )
				{
					// if the user specified explicit geometry, use that and the calculated
					// extent of the geometry to derive a profile.
				 osgEarth::GeoExtent ex;
					if ( profile.valid() )
					{                
						//ex = profile->getExtent();
						ex = GeoExtent(profile->getSRS(), _geometry->getBounds());
					}

					if ( !ex.isValid() )
					{
						// default to WGS84 Lat/Long
						ex = osgEarth::Registry::instance()->getGlobalGeodeticProfile()->getExtent();
					}
					result = new FeatureProfile( ex );
				}
				//3-----------------------------------------------------------
				else if ( !_source.empty() && _geoFeaClass)
				{
					// otherwise, assume we're loading from the URL:
					GEO_SCOPED_LOCK;

					if ( profile.valid() )
					{
						result = new FeatureProfile( profile->getExtent() );
					}
					else
					{
						osg::ref_ptr<SpatialReference> srs =SpatialReference::get("wgs84");
						if (srs.valid())
						{
						 osgEarth::GeoExtent extent(srs.get(),_geoFeaClass->_geoFeaMetatable.XMin,_geoFeaClass->_geoFeaMetatable.YMin,
								_geoFeaClass->_geoFeaMetatable.XMax,_geoFeaClass->_geoFeaMetatable.YMax);
							result = new FeatureProfile(extent);
						}
					}
					//-------------------------------------------------------------------------------------
					///////////////////////////////////////////////////////////////////////////////////////////////////////////////
					_featureCount=_geoFeaClass->getFeatureCount();
					geoGEOMETRYTYPE geoType = (geoGEOMETRYTYPE)_geoFeaClass->_geoFeaMetatable.GeometryType;
					if (geoType == GEO_GEOMETRY_POLYGON)
					{
						_geometryType= osgEarth::Symbology::Geometry::TYPE_POLYGON;
					}
					else if (geoType == GEO_GEOMETRY_PATH || geoType==GEO_GEOMETRY_LINE)
					{
						_geometryType = osgEarth::Symbology::Geometry::TYPE_LINESTRING;
					}
					else if (geoType == GEO_GEOMETRY_RING )
					{
						_geometryType = osgEarth::Symbology::Geometry::TYPE_RING;
					}
					else if ( geoType == GEO_GEOMETRY_POINT)
					{
						_geometryType = osgEarth::Symbology::Geometry::TYPE_POINTSET;
					}
					else if (
						geoType == GEO_GEOMETRY_COLLECTION ||
						geoType == GEO_GEOMETRY_MULTIPOINT ||
						geoType == GEO_GEOMETRY_POLYLINE )
					{
						_geometryType = osgEarth::Symbology::Geometry::TYPE_MULTI;
					}

				}
				else
				{
					OE_INFO << LC 
						<< "Feature Source: no valid source data available" << std::endl;
				}
				return result;
			}
//得到访问到的当前要素
	FeatureCursor* createFeatureCursor( const Symbology::Query& query )
	{
				 if ( _geometry.valid() )
        {
            return new GeometryFeatureCursor(
                _geometry.get(),
                getFeatureProfile(),
                _options.filters() );
        }
        else if ( !_source.empty() && _geoFeaClass)
        {
            // GEO_SCOPED_LOCK;

             // cursor is responsible for the OGR handles.
			/*osg::ref_ptr<FeatureCursorGEO> f=new FeatureCursorGEO(_geoFeaClass,
				this,getFeatureProfile(),
				query,
				_options.filters());
			_countFeature=_countFeature+f->_querySize;
			return f.release();*/
                return new FeatureCursorGEO(_geoFeaClass,
					this,getFeatureProfile(),
					query,_geoHeight,
					_options.filters());
				
        }
		else 
			return 0L;
	}
	//判断该要素是否在指定范围内
			bool exeOidBound(int oid,osgEarth::Bounds bds)
			{
				GEO_SCOPED_LOCK;
				return _geoFeaClass->bOidIntersectObjectBound(oid,bds);
			}
			//根据FeatureID获得要素
			virtual Feature* getFeature( FeatureID fid )
			{
				Feature* result = NULL;

				if ( !isBlacklisted(fid) )
				{
					GEO_SCOPED_LOCK;
					GEOFeatureH handle;
					_geoFeaClass->createGEOFeatureH(fid,&handle);
					if (handle.valid())
					{
						const FeatureProfile* p = getFeatureProfile();
						const SpatialReference* srs = p ? p->getSRS() : 0L;
						result = GEOFeatureUtils::createFeature( &handle, srs );
					}
				}
				return result;
			}

			virtual int getFeatureCount() const
			{
				return _featureCount;
			}

		    

			virtual osgEarth::Symbology::Geometry::Type getGeometryType() const
			{
				return _geometryType;
			}

		
			
#if 0
	////////////////////////////////////////////////////////////////////?Yê±2?Dèòaμ?
	//D′2ù×÷?Yê±2?????
	bool _writable;
	virtual bool isWritable() const
	{
		return _writable;
	}
	virtual bool insertFeature(Feature* feature)
	{
		OGR_SCOPED_LOCK;
		OGRFeatureH feature_handle = OGR_F_Create( OGR_L_GetLayerDefn( _layerHandle ) );
		if ( feature_handle )
		{
			const AttributeTable& attrs = feature->getAttrs();

			// assign the attributes:
			int num_fields = OGR_F_GetFieldCount( feature_handle );
			for( int i=0; i<num_fields; i++ )
			{
				OGRFieldDefnH field_handle_ref = OGR_F_GetFieldDefnRef( feature_handle, i );
				std::string name = OGR_Fld_GetNameRef( field_handle_ref );
				int field_index = OGR_F_GetFieldIndex( feature_handle, name.c_str() );

				AttributeTable::const_iterator a = attrs.find( toLower(name) );
				if ( a != attrs.end() )
				{
					switch( OGR_Fld_GetType(field_handle_ref) )
					{
					case OFTInteger:
						OGR_F_SetFieldInteger( feature_handle, field_index, a->second.getInt(0) );
						break;
					case OFTReal:
						OGR_F_SetFieldDouble( feature_handle, field_index, a->second.getDouble(0.0) );
						break;
					case OFTString:
						OGR_F_SetFieldString( feature_handle, field_index, a->second.getString().c_str() );
						break;
					default:break;
					}
				}
			}

			// assign the geometry:
			OGRFeatureDefnH def = ::OGR_L_GetLayerDefn( _layerHandle );

			OGRwkbGeometryType reported_type = OGR_FD_GetGeomType( def );

			OGRGeometryH ogr_geometry = OgrUtils::createOgrGeometry( feature->getGeometry(), reported_type );
			if ( OGR_F_SetGeometryDirectly( feature_handle, ogr_geometry ) != OGRERR_NONE )
			{
				OE_WARN << LC << "OGR_F_SetGeometryDirectly failed!" << std::endl;
			}

			if ( OGR_L_CreateFeature( _layerHandle, feature_handle ) != OGRERR_NONE )
			{
				//TODO: handle error better
				OE_WARN << LC << "OGR_L_CreateFeature failed!" << std::endl;
				OGR_F_Destroy( feature_handle );
				return false;
			}

			// clean up the feature
			OGR_F_Destroy( feature_handle );
		}
		else
		{
			//TODO: handle error better
			OE_WARN << LC << "OGR_F_Create failed." << std::endl;
			return false;
		}

		dirty();

		return true;
	}
	virtual bool deleteFeature(FeatureID fid)
	{
		if (_writable && _layerHandle)
		{
			OGR_SCOPED_LOCK;
			if (OGR_L_DeleteFeature( _layerHandle, fid ) == OGRERR_NONE)
			{
				_needsSync = true;
				return true;
			}            
		}
		return false;
	}




	//////////////////////////////////////////////////??ê?D?μ?′|àí￡??Y2?????
	FeatureSchema _schema;
	void initSchema()
	{
		OGRFeatureDefnH layerDef =  OGR_L_GetLayerDefn( _layerHandle );
		for (int i = 0; i < OGR_FD_GetFieldCount( layerDef ); i++)
		{
			OGRFieldDefnH fieldDef = OGR_FD_GetFieldDefn( layerDef, i );
			std::string name;
			name = std::string( OGR_Fld_GetNameRef( fieldDef ) );
			OGRFieldType ogrType = OGR_Fld_GetType( fieldDef );
			_schema[ name ] = OgrUtils::getAttributeType( ogrType );
		}
	}
	const FeatureSchema& getSchema() const
	{
		return _schema;
	} 
	/////////////////////////////////////////////////////////////////////////////////////
#endif
		protected:
				//////////////////////////////////////WKT geometry可以忽略
				// parses an explicit WKT geometry string into a Geometry.
		Symbology::Geometry* parseGeometry( const osgEarth::Config& geomConf )
		{
			return GeometryUtils::geometryFromWKT( geomConf.value() );
		}

				// read the WKT geometry from a URL, then parse into a Geometry.
        Symbology::Geometry* parseGeometryUrl( const std::string& geomUrl, const osgDB::Options* dbOptions )
		{
			ReadResult r = URI(geomUrl).readString( dbOptions );
			if ( r.succeeded() )
			{
				Config conf( "geometry", r.getString() );
				return parseGeometry( conf );
			}
			return 0L;
		}

		//private:
		public:
			std::string _source;//???t??

			GEOFeatureClass* _geoFeaClass;//本地数据库文件访问类GEOFeatureClass

			osg::ref_ptr<Symbology::Geometry> _geometry; // 获取某一几何体
			const GEOFeatureOptions _options;//要素图层的参数类

			int _featureCount;//所有的要素个数
			bool _needsSync;
			osgEarth::Symbology::Geometry::Type _geometryType;//几何体类型

			unsigned   _countFeature;
			float      _geoHeight;
		};

		class GEOFeatureSourceFactory : public FeatureSourceDriver
		{
		public:
			GEOFeatureSourceFactory()
			{
				supportsExtension( "osgearth_feature_geoStar", "GEOStar feature driver for osgEarth" );
			}

			virtual const char* className()
			{
				return "GEOStar Feature Reader";
			}

			virtual ReadResult readObject(const std::string& file_name, const Options* options) const
			{
				if ( !acceptsExtension(osgDB::getLowerCaseFileExtension( file_name )))
					return ReadResult::FILE_NOT_HANDLED;

				return ReadResult( new GEOFeatureSource( getFeatureSourceOptions(options) ) );
			}
		};

		REGISTER_OSGPLUGIN(osgearth_feature_geoStar, GEOFeatureSourceFactory)
	}
}
#endif//OSGEARTHFEATURES_FEATURE_GEOFEATURE_SOURCE_GEO