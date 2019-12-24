
#ifndef OSGEARTH_DRIVER_GEO_WFS_FEATURE_SOURCE_OPTIONS
#define OSGEARTH_DRIVER_GEO_WFS_FEATURE_SOURCE_OPTIONS 1

#include <osgEarth/Common>
#include <osgEarth/URI>
#include <osgEarthFeatures/FeatureSource>
#include "GEOWFS.h"
#include <osg/Vec3d>

namespace GeoGlobe { namespace GeoVector
{
    
    using namespace osgEarth::Features;

    /**
     * Options for the WFS driver.
     */
    class GEOWFSFeatureOptions : public FeatureSourceOptions // NO EXPORT; header only
    {
    public:
        /** Base URL of the WFS service (not including any WFS parameters) */
        osgEarth::optional<osgEarth::URI>& url() { return _url; }//WFS服务地址
        const osgEarth::optional<osgEarth::URI>& url() const { return _url; }

        /** deprecated */
        optional<Config>& geometryProfileOptions() { return _geometryProfileConf; }
        const optional<Config>& geometryProfileOptions() const { return _geometryProfileConf; }

        /** WFS typename parameter (see WFS spec) */
        osgEarth::optional<std::string>& typeName() { return _typename; }
        const osgEarth::optional<std::string>& typeName() const { return _typename; }

        /** Cap on the number of features that the WFS service will return on a single request */
        optional<unsigned int>& maxFeatures() { return _maxFeatures; }
        const optional<unsigned int>& maxFeatures() const { return _maxFeatures;}

        /** Response format to request (geojson, gml) */
        osgEarth::optional<std::string>& outputFormat() { return _outputFormat; }
        const osgEarth::optional<std::string>& outputFormat() const { return _outputFormat; }

        /** Explicity disable a "TFS" tiled feature source queries */
        osgEarth::optional<bool>& disableTiling() { return _disableTiling; }
        const osgEarth::optional<bool>& disableTiling() const { return _disableTiling;}

        /** The number of map units to buffer bounding box requests with 
         *  to ensure that enough data is returned.  This is useful when rendering buffered lines
         *  using the AGGLite driver.         
         */
        osgEarth::optional<double>& buffer() { return _buffer;}
        const osgEarth::optional<double>& buffer() const { return _buffer;}

        osgEarth::optional<std::string>& cachePath() { return _cachePath; }//矢量要素数据的缓存路径
        const osgEarth::optional<std::string>& cachePath() const { return _cachePath; }

		optional<float>& height() {return _geoHeight;}
		const optional<float>& height()const {return _geoHeight;}

		osgEarth::optional<double>& ShaowColorA() { return m_dbShaowColorA; }
		const osgEarth::optional<double>& ShaowColorA() const { return m_dbShaowColorA; }
		osgEarth::optional<double>& ShaowColorR() { return m_dbShaowColorR; }
		const osgEarth::optional<double>& ShaowColorR() const { return m_dbShaowColorR; }
		osgEarth::optional<double>& ShaowColorG() { return m_dbShaowColorG; }
		const osgEarth::optional<double>& ShaowColorG() const { return m_dbShaowColorG; }
		osgEarth::optional<double>& ShaowColorB() { return m_dbShaowColorB; }
		const osgEarth::optional<double>& ShaowColorB() const { return m_dbShaowColorB; }
		osgEarth::optional<double>& ShaowOffset() { return m_dbOffset; }
		const osgEarth::optional<double>& ShaowOffset() const { return m_dbOffset; }
		optional<int>& ShadowPos() { return m_eShadowPos; }
		const optional<int>& ShadowPos()const { return m_eShadowPos; }

		osgEarth::optional<double>& OriginX() { return m_dbOriginX; }
		const osgEarth::optional<double>& OriginX() const { return m_dbOriginX; }
		osgEarth::optional<double>& OriginY() { return m_dbOriginY; }
		const osgEarth::optional<double>& OriginY() const { return m_dbOriginY; }
		osgEarth::optional<double>& OriginZ() { return m_dbOriginZ; }
		const osgEarth::optional<double>& OriginZ() const { return m_dbOriginZ; }

		optional<float>& transparent() {return _transparent;}
		const optional<float>& transparent()const {return _transparent;}

		optional<int>& Size() { return m_nsize; }
		const optional<int>& Size()const { return m_nsize; }
		osgEarth::optional<double>& ColorA() { return m_dbColorA; }
		const osgEarth::optional<double>& ColorA() const { return m_dbColorA; }
		osgEarth::optional<double>& ColorR() { return m_dbColorR; }
		const osgEarth::optional<double>& ColorR() const { return m_dbColorR; }
		osgEarth::optional<double>& ColorG() { return m_dbColorG; }
		const osgEarth::optional<double>& ColorG() const { return m_dbColorG; }
		osgEarth::optional<double>& ColorB() { return m_dbColorB; }
		const osgEarth::optional<double>& ColorB() const { return m_dbColorB; }
		osgEarth::optional<std::string>& FontFile() { return m_strFontFile; }//矢量要素数据的缓存路径
		const osgEarth::optional<std::string>& FontFile() const { return m_strFontFile; }

    public:
        GEOWFSFeatureOptions( const ConfigOptions& opt =ConfigOptions() ) :
          FeatureSourceOptions( opt ),
          _buffer( 0 )
        {
            setDriver( "geowfs" );
            fromConfig( _conf );            
        }

        virtual ~GEOWFSFeatureOptions() { }

    public:
        Config getConfig() const 
		{
            Config conf = FeatureSourceOptions::getConfig();
            conf.set( "url", _url ); 
            conf.set( "geometry_profile", _geometryProfileConf );
            conf.set( "typename", _typename );
            conf.set( "outputformat", _outputFormat);
            conf.set( "maxfeatures", _maxFeatures );
            conf.set( "disable_tiling", _disableTiling );
            conf.set( "request_buffer", _buffer);
			conf.set( "cachePath", _cachePath );
			conf.set( "height", _geoHeight );
			conf.set( "OriginX", m_dbOriginX );
			conf.set( "OriginY", m_dbOriginY );
			conf.set( "OriginZ", m_dbOriginZ );
			conf.set( "transparent", _transparent);

			conf.set("Size", m_nsize);
			conf.set("ColorA", m_dbColorA);
			conf.set("ColorR", m_dbColorR);
			conf.set("ColorG", m_dbColorG);
			conf.set("ColorB", m_dbColorB);

			conf.set("ShaowColorA", m_dbShaowColorA);
			conf.set("ShaowColorR", m_dbShaowColorR);
			conf.set("ShaowColorG", m_dbShaowColorG);
			conf.set("ShaowColorB", m_dbShaowColorB);
			conf.set("ShadowPos", m_eShadowPos);
			conf.set("ShaowOffset", m_dbOffset);

			conf.set("FontFile", m_strFontFile);
            return conf;
        }

    protected:
        void mergeConfig( const osgEarth::Config& conf )
		{
            FeatureSourceOptions::mergeConfig( conf );
            fromConfig( conf );
        }

    private:
        void fromConfig( const osgEarth::Config& conf ) 
		{            
            conf.get( "url", _url );
            conf.get( "geometry_profile", _geometryProfileConf );
            conf.get( "typename", _typename);
            conf.get( "outputformat", _outputFormat );
            conf.get( "maxfeatures", _maxFeatures );
            conf.get( "disable_tiling", _disableTiling);
            conf.get( "request_buffer", _buffer);
			conf.get( "cachePath", _cachePath);
			conf.get( "height", _geoHeight);
			conf.get( "OriginX", m_dbOriginX );
			conf.get( "OriginY", m_dbOriginY );
			conf.get( "OriginZ", m_dbOriginZ );
			conf.get( "transparent", _transparent);

			conf.get("Size", m_nsize);
			conf.get("ColorA", m_dbColorA);
			conf.get("ColorR", m_dbColorR);
			conf.get("ColorG", m_dbColorG);
			conf.get("ColorB", m_dbColorB);
			conf.get("FontFile", m_strFontFile);

			conf.get("ShaowColorA", m_dbShaowColorA);
			conf.get("ShaowColorR", m_dbShaowColorR);
			conf.get("ShaowColorG", m_dbShaowColorG);
			conf.get("ShaowColorB", m_dbShaowColorB);
			conf.get("ShadowPos", m_eShadowPos);
			conf.get("ShaowOffset", m_dbOffset);
        }

        osgEarth::optional<osgEarth::URI>         _url;  
		osgEarth::optional<std::string> _cachePath;
        osgEarth::optional<std::string> _typename;
        optional<Config>      _geometryProfileConf;
        osgEarth::optional<std::string> _outputFormat;
        optional<unsigned>    _maxFeatures;            
        optional<bool>    _disableTiling;            
        optional<double>  _buffer; 
		optional<float>   _geoHeight;

		//osg::Vec3d   m_vecOriginPoint;
		optional<double>  m_dbOriginX;
		optional<double>  m_dbOriginY;
		optional<double>  m_dbOriginZ;

		optional<float>   _transparent;

		optional<int>  m_nsize;
		optional<double>  m_dbColorA;
		optional<double>  m_dbColorR;
		optional<double>  m_dbColorG;
		optional<double>  m_dbColorB;
		optional<double>  m_dbShaowColorA;
		optional<double>  m_dbShaowColorR;
		optional<double>  m_dbShaowColorG;
		optional<double>  m_dbShaowColorB;
		optional<int> m_eShadowPos;
		optional<double> m_dbOffset;
		optional<std::string> m_strFontFile;

	public:
		osgEarth::GeoExtent _dataExtent;//矢量要素图层的范围
		osg::Vec2d      _center;
		osg::ref_ptr< GEOWFSCapabilities > _capabilities;
        // GEOWFSFeatureOptions初始化函数
        bool init()
        {
        	//获取WFS服务Capabilities的请求地址
        	std::string capUrl;

			if ( url().isSet() )
			{
				char sep = url()->full().find_first_of('?') == std::string::npos? '?' : '&';

				capUrl = url()->full() + sep + "SERVICE=WFS&VERSION=1.0.0&REQUEST=GetCapabilities";
			}   
			if (cachePath().isSet() && typeName().isSet() )
			{
				std::string fn = cachePath().get() + "/" + "cap" + typeName().get() + ".xml";
				_capabilities = GEOWFSCapabilitiesReader::read(capUrl,fn);
			}
			else
			{
				_capabilities = GEOWFSCapabilitiesReader::read(capUrl);
			}
			
			if ( !_capabilities.valid() )//如果WFSCapabilities获取失败，则输出错误文件
			{
				OE_WARN << "[osgEarth::WFS] Unable to read WFS GetCapabilities." << std::endl;
				return false;
			}
			else//如果WFSCapabilities获取成功，则获得相应图层的GEOWFSFeatureType
			{
				OE_INFO << "[osgEarth::WFS] Got capabilities from " << capUrl << std::endl;
				//Find the feature type by name
				osg::ref_ptr< GEOWFSFeatureType > featureType = _capabilities->getFeatureTypeByName(typeName().get() );
				if (featureType.valid())
				{
					if (featureType->getExtent().isValid())
					{
						_dataExtent=featureType->getExtent();
						_dataExtent.getCentroid( _center.x(), _center.y() );
						return true;
					}
				}
			}
			return false;
		}



    };

} } // namespace osgEarth::Drivers

#endif // OSGEARTH_DRIVER_GEO_WFS_FEATURE_SOURCE_OPTIONS

