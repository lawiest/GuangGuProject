#ifndef OSGEARTH_DRIVER_GEO_WMS_DRIVEROPTIONS
#define OSGEARTH_DRIVER_GEO_WMS_DRIVEROPTIONS 1

#include <osgEarth/Common>
#include <osgEarth/TileSource>
#include "GEOWMSCapabilities.h"

namespace GeoGlobe 
{
    

    class GEOWMSOptions : public osgEarth::TileSourceOptions // NO EXPORT; header only
    {
    public:
		osgEarth::optional<osgEarth::URI>& url() { return _url; }
        const osgEarth::optional<osgEarth::URI>& url() const { return _url; }

        osgEarth::optional<osgEarth::URI>& capabilitiesUrl() { return _capabilitiesUrl; }
        const osgEarth::optional<osgEarth::URI>& capabilitiesUrl() const { return _capabilitiesUrl; }

        osgEarth::optional<osgEarth::URI>& tileServiceUrl() { return _tileServiceUrl; }
        const osgEarth::optional<osgEarth::URI>& tileServiceUrl() const { return _tileServiceUrl; }

        osgEarth::optional<std::string>& layers() { return _layers; }
        const osgEarth::optional<std::string>& layers() const { return _layers; }

        osgEarth::optional<std::string>& style() { return _style; }
        const osgEarth::optional<std::string>& style() const { return _style; }

        osgEarth::optional<std::string>& format() { return _format; }
        const osgEarth::optional<std::string>& format() const { return _format; }

        osgEarth::optional<std::string>& wmsFormat() { return _wmsFormat; }
        const osgEarth::optional<std::string>& wmsFormat() const { return _wmsFormat; }

        osgEarth::optional<std::string>& wmsVersion() { return _wmsVersion; }
        const osgEarth::optional<std::string>& wmsVersion() const { return _wmsVersion; }

        osgEarth::optional<std::string>& elevationUnit() { return _elevationUnit; }
        const osgEarth::optional<std::string>& elevationUnit() const { return _elevationUnit; }

        osgEarth::optional<std::string>& srs() { return _srs; }
        const osgEarth::optional<std::string>& srs() const { return _srs; }

        osgEarth::optional<std::string>& crs() { return _crs; }
        const osgEarth::optional<std::string>& crs() const { return _crs; }

        osgEarth::optional<bool>& transparent() { return _transparent; }
        const osgEarth::optional<bool>& transparent() const { return _transparent; }

        osgEarth::optional<std::string>& times() { return _times; }
        const osgEarth::optional<std::string>& times() const { return _times; }

        osgEarth::optional<double>& secondsPerFrame() { return _secondsPerFrame; }
        const osgEarth::optional<double>& secondsPerFrame() const { return _secondsPerFrame; }

		osgEarth::optional<std::string>& cachePath() { return _cachePath; }
		const osgEarth::optional<std::string>& cachePath() const { return _cachePath; }
    public:
        GEOWMSOptions( const TileSourceOptions& opt =TileSourceOptions() ) : TileSourceOptions( opt ),
            _wmsVersion( "1.1.1" ),
            _elevationUnit( "m" ),
            _transparent( true ),
            _secondsPerFrame( 1.0 )
        {
            setDriver( "geowms" );
            fromConfig( _conf );
        }

        /** dtor */
        virtual ~GEOWMSOptions() { }

    public:
		osgEarth::Config getConfig() const {
			osgEarth::Config conf = TileSourceOptions::getConfig();
            conf.set("url", _url);
            conf.set("capabilities_url", _capabilitiesUrl);
            conf.set("tile_service_url", _tileServiceUrl);
            conf.set("layers", _layers);
            conf.set("style", _style);
            conf.set("format", _format);
            conf.set("wms_format", _wmsFormat);
            conf.set("wms_version", _wmsVersion);
            conf.set("elevation_unit", _elevationUnit);
            conf.set("srs", _srs);
            conf.set("crs", _crs);
            conf.set("transparent", _transparent);
            conf.set("times", _times);
            conf.set("seconds_per_frame", _secondsPerFrame );
			conf.set("cachePath", _cachePath );
            return conf;
        }

    protected:
        void mergeConfig( const osgEarth::Config& conf ) {
            TileSourceOptions::mergeConfig( conf );
            fromConfig( conf );
        }

    private:
        void fromConfig( const osgEarth::Config& conf ) {
            conf.get("url", _url);
            conf.get("capabilities_url", _capabilitiesUrl);
            conf.get("tile_service_url", _tileServiceUrl);
            conf.get("layers", _layers);
            conf.get("style", _style);
            conf.get("format", _format);
            conf.get("wms_format", _wmsFormat);
            conf.get("wms_version", _wmsVersion);
            conf.get("elevation_unit", _elevationUnit);
            conf.get("srs", _srs);
            conf.get("crs", _crs);
            conf.get("transparent", _transparent);
            conf.get("times", _times);
            conf.get("seconds_per_frame", _secondsPerFrame );
			conf.get("cachePath", _cachePath);
        }

        osgEarth::optional<osgEarth::URI>         _url;
        osgEarth::optional<osgEarth::URI>         _capabilitiesUrl;
        osgEarth::optional<osgEarth::URI>         _tileServiceUrl;
        osgEarth::optional<std::string> _layers;
        osgEarth::optional<std::string> _style;
        osgEarth::optional<std::string> _format;
        osgEarth::optional<std::string> _wmsFormat;
        osgEarth::optional<std::string> _wmsVersion;
        osgEarth::optional<std::string> _elevationUnit;
        osgEarth::optional<std::string> _srs;
        osgEarth::optional<std::string> _crs;
		osgEarth::optional<bool>        _transparent;
        osgEarth::optional<std::string> _times;
		osgEarth::optional<double>      _secondsPerFrame;
		osgEarth::optional<std::string> _cachePath;
	public:
		void getMapExtent(double& west,double& south,double& east,double& north)
		{
			double minx,miny,maxx,maxy;
			minx=-180;
			miny=-90;
			maxx=180;
			maxy=90;
			char sep = url()->full().find_first_of('?') == std::string::npos? '?' : '&';
			osg::ref_ptr<GEOWMSCapabilities> capabilities=NULL;
			osgEarth::URI capUrl = capabilitiesUrl().value();
			if ( capUrl.empty() )
			{
				capUrl = osgEarth::URI(
					url()->full() + 
					sep + 
					std::string("SERVICE=WMS") +
					std::string("&VERSION=") + wmsVersion().value() +
					std::string("&REQUEST=GetCapabilities") );
			}
			if (cachePath().isSet()&&layers().isSet())
			{
				std::string capCachePath = cachePath().value()+"/"+"Cap-"+layers().value()+".xml";

				capabilities = GEOWMSCapabilitiesReader::read(capUrl.full(),capCachePath);
			}
			else
				capabilities = GEOWMSCapabilitiesReader::read(capUrl.full());

			if ( !capabilities.valid() )
			{
				return ;
			}
			GEOWMSLayer* layer = capabilities->getLayerByName(layers().value() );
			if ( layer )
			{
				layer->getLatLonExtents(minx, miny, maxx, maxy);
			}
			west=minx;
			south=miny;
			east=maxx;
			north=maxy;
			return;
		}
		void getMapCenter(double& centerx,double& centery)
		{
			double minx,miny,maxx,maxy;
			minx=-180;
			miny=-90;
			maxx=180;
			maxy=90;
			getMapExtent(minx,miny,maxx,maxy);
			centerx=(minx+maxx)/2;
			centery=(miny+maxy)/2;
		    return;
		}
		
    };

} // namespace GeoGlobe

#endif // OSGEARTH_DRIVER_GEO_WMS_DRIVEROPTIONS

