#ifndef OSGEARTH_DRIVER_GEOWFS_RASTERIZER_DRIVEROPTIONS
#define OSGEARTH_DRIVER_GEOWFS_RASTERIZER_DRIVEROPTIONS 1

#include <osgEarth/Common>
#include <osgEarthFeatures/FeatureTileSource>
//#include <osgEarthSymbology/AGG.h>
namespace GeoGlobe { namespace GeoVector
{
    
    using namespace osgEarth::Features;

     class GEOWFSRasterizerOptions : public FeatureTileSourceOptions // NO EXPORT; header only
    {
    public:
        /** 
         * Whether to downsample line features to that they are no higher resolution than
         * the target image resolution. Defaults to true, but you can disable this (for a possible
         * performance increase) if you know your data to be of a relatively low resolution.
         * (Default = true)
         */
        osgEarth::optional<bool>& optimizeLineSampling() { return _optimizeLineSampling; }
        const osgEarth::optional<bool>& optimizeLineSampling() const { return _optimizeLineSampling; }

        /** 
         * Set the gamma parameter applied on the AggLite rasterizer : allow to control geometry antialiasing
         * (Default = 1.3)
         */
        osgEarth::optional<double>& gamma() { return _gamma; }
        const osgEarth::optional<double>& gamma() const { return _gamma; }

    public:
        GEOWFSRasterizerOptions( const TileSourceOptions& options =TileSourceOptions() )
            : FeatureTileSourceOptions( options ),
              _optimizeLineSampling   ( true ),
              _gamma                  ( 1.3 )
        {
            setDriver( "geowfsrasterizer" );
            fromConfig( _conf );
        }

        /** dtor */
        virtual ~GEOWFSRasterizerOptions() { }

    public:
        Config getConfig() const {
            Config conf = FeatureTileSourceOptions::getConfig();
            conf.updateIfSet("optimize_line_sampling", _optimizeLineSampling);
            conf.updateIfSet("gamma", _gamma );
            return conf;
        }

    protected:
        void mergeConfig( const osgEarth::Config& conf ) {
            FeatureTileSourceOptions::mergeConfig( conf );
            fromConfig(conf);
        }

    private:
        void fromConfig( const osgEarth::Config& conf ) {
            conf.getIfSet( "optimize_line_sampling", _optimizeLineSampling );
            conf.getIfSet( "gamma", _gamma );
        }

        optional<bool> _optimizeLineSampling;
        optional<double> _gamma;
    };

} } // namespace geoglobe::geovector

#endif // OSGEARTH_DRIVER_GEOWFS_RASTERIZER_DRIVEROPTIONS