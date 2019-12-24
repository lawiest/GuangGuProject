#ifndef OSGEARTH_DRIVER_GEO_RASTERIZER_DRIVEROPTIONS
#define OSGEARTH_DRIVER_GEO_RASTERIZER_DRIVEROPTIONS 1

#include <osgEarth/Common>
#include <osgEarthFeatures/FeatureTileSource>

namespace GeoGlobe { namespace GeoVector
{
    
    using namespace osgEarth::Features;

    //栅格化矢量要素图层的参数类
    class GEORasterizerOptions : public FeatureTileSourceOptions // NO EXPORT; header only
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
        GEORasterizerOptions( const TileSourceOptions& options =TileSourceOptions() )
            : FeatureTileSourceOptions( options ),
              _optimizeLineSampling   ( true ),
              _gamma                  ( 1.3 )
        {
            setDriver( "georasterizer" );//设置读取该栅格化矢量要素图层的驱动器名称
            fromConfig( _conf );
        }

        /** dtor */
        virtual ~GEORasterizerOptions() { }

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

        //与栅格化相关的参数
        optional<bool> _optimizeLineSampling;
        optional<double> _gamma;
    };

} } // namespace osgEarth::Drivers

#endif // OSGEARTH_DRIVER_GEO_RASTERIZER_DRIVEROPTIONS