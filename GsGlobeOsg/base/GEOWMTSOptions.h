#ifndef OSGEARTH_DRIVER_GEO_WMTS_DRIVEROPTIONS
#define OSGEARTH_DRIVER_GEO_WMTS_DRIVEROPTIONS 1

#include <osgEarth/Common>
#include <osgEarth/TileSource>
#include "Base/SERVICE.h"

namespace GeoGlobe 
{
	

	class GEOWMTSOptions : public osgEarth::TileSourceOptions // NO EXPORT; header only
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

		osgEarth::optional<std::string>& wmtsFormat() { return _wmtsFormat; }
		const osgEarth::optional<std::string>& wmtsFormat() const { return _wmtsFormat; }

		osgEarth::optional<std::string>& wmtsVersion() { return _wmtsVersion; }
		const osgEarth::optional<std::string>& wmtsVersion() const { return _wmtsVersion; }

		osgEarth::optional<std::string>& srs() { return _srs; }
		const osgEarth::optional<std::string>& srs() const { return _srs; }

		osgEarth::optional<std::string>& crs() { return _crs; }
		const osgEarth::optional<std::string>& crs() const { return _crs; }


		osgEarth::optional<std::string>& cachePath() { return _cachePath; }
		const osgEarth::optional<std::string>& cachePath() const { return _cachePath; }
	public:
		GEOWMTSOptions( const TileSourceOptions& opt =TileSourceOptions() ) : TileSourceOptions( opt ),
			_wmtsVersion( "1.0.0" )
		{
			setDriver( "geowmts" );
			fromConfig( _conf );
		}

		/** dtor */
		virtual ~GEOWMTSOptions() { }

	public:
		osgEarth::Config getConfig() const {
			osgEarth::Config conf = TileSourceOptions::getConfig();
			conf.set("url", _url);
			conf.set("capabilities_url", _capabilitiesUrl);
			conf.set("tile_service_url", _tileServiceUrl);
			conf.set("layers", _layers);
			conf.set("style", _style);
			conf.set("format", _format);
			conf.set("wmts_format", _wmtsFormat);
			conf.set("wmts_version", _wmtsVersion);
			conf.set("srs", _srs);
			conf.set("crs", _crs);
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
			conf.get("wmts_format", _wmtsFormat);
			conf.get("wmts_version", _wmtsVersion);
			conf.get("srs", _srs);
			conf.get("crs", _crs);
			conf.get("cachePath", _cachePath);
		}

		osgEarth::optional<osgEarth::URI>         _url;
		osgEarth::optional<osgEarth::URI>         _capabilitiesUrl;
		osgEarth::optional<osgEarth::URI>         _tileServiceUrl;
		osgEarth::optional<std::string> _layers;
		osgEarth::optional<std::string> _style;
		osgEarth::optional<std::string> _format;
		osgEarth::optional<std::string> _wmtsFormat;
		osgEarth::optional<std::string> _wmtsVersion;
		osgEarth::optional<std::string> _srs;
		osgEarth::optional<std::string> _crs;
		osgEarth::optional<std::string> _cachePath;
	public:
		void getLayerExtent(double& west,double& south,double& east,double& north)
		{
			double minx,miny,maxx,maxy;
			minx=-180;
			miny=-90;
			maxx=180;
			maxy=90;
			char sep = url()->full().find_first_of('?') == std::string::npos? '?' : '&';
			osg::ref_ptr<osgEarth::Util::WMTSCapabilities> capabilities=NULL;
			osgEarth::URI capUrl = capabilitiesUrl().value();
			if ( capUrl.empty() )
			{
				capUrl = osgEarth::URI(
					url()->full() + 
					sep + 
					std::string("SERVICE=WMTS") +
					std::string("&VERSION=") + wmtsVersion().value() +
					std::string("&REQUEST=GetCapabilities") );
			}
			if (cachePath().isSet()&&layers().isSet())
			{
				std::string capCachePath = cachePath().value()+"/"+"Cap-"+layers().value()+".xml";

				capabilities = osgEarth::Util::WMTSCapabilitiesReader::read(capUrl.full(),capCachePath);
			}
			else
				capabilities = osgEarth::Util::WMTSCapabilitiesReader::read(capUrl.full());

			if ( !capabilities.valid() )
			{
				return ;
			}

			if (layers().isSet())//ture,judge at first
			{
				for(int i=0;i<capabilities->getLayer().size();i++)
				{
					if (layers() == capabilities->getLayerAt(i).Title)
					{
						minx = capabilities->getLayerAt(i).wgs84bound.west;
						maxx = capabilities->getLayerAt(i).wgs84bound.east;
						miny = capabilities->getLayerAt(i).wgs84bound.south;
						maxy = capabilities->getLayerAt(i).wgs84bound.north;
					}
				}
			}
			west=minx;
			south=miny;
			east=maxx;
			north=maxy;
			return;
		}
		void getLayerCenter(double& centerx,double& centery)
		{
			double minx,miny,maxx,maxy;
			minx=-180;
			miny=-90;
			maxx=180;
			maxy=90;
			getLayerExtent(minx,miny,maxx,maxy);
			centerx=(minx+maxx)/2;
			centery=(miny+maxy)/2;
			return;
		}

	};

} // namespace GeoGlobe

#endif // OSGEARTH_DRIVER_GEO_WMTS_DRIVEROPTIONS

