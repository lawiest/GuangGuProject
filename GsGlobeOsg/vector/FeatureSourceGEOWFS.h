#ifndef OSGEARTHFEATURES_FEATURE_GEOFEATURE_SOURCE_GEOWFS
#define OSGEARTHFEATURES_FEATURE_GEOFEATURE_SOURCE_GEOWFS 1

#include <osgEarth/Registry>
#include <osgEarth/FileUtils>
#include <osgEarth/StringUtils>
#include <osgEarth/GeoData>
#include <osgEarth/Bounds>
#include <osgEarth/Cache>
#include <osgEarthFeatures/FeatureSource>
#include <osgEarthFeatures/Filter>
#include <osgEarthFeatures/BufferFilter>
#include <osgEarthFeatures/ScaleFilter>
#include <osgEarthFeatures/GeometryUtils>
#include "tool/CodeTransformHelp.h"


#include <osg/Notify>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <list>


#include "GEOWFSFeatureOptions.h"
#include "GEOWFS.h"
#include "GeoFeature_XmlApi.h"
#define GEO_SCOPED_LOCK GDAL_SCOPED_LOCK
#define LC "[GEO FeatureSource] "

namespace GeoGlobe
{
	namespace GeoVector
	{
		
		using namespace osgEarth::Features;

		//吉奥WFS服务对应的要素类来源
		class GEOWFSFeatureSource : public FeatureSource
		{
		public:
					//构造函数，传入参数GEOWFSFeatureOptions，矢量抬高高度与透明度
			GEOWFSFeatureSource(const GEOWFSFeatureOptions& options);

			/** Destruct the object, cleaning up and OGR handles. */
			virtual ~GEOWFSFeatureSource();

			//FeatureSource的初始化类，在加载数据之前调用
			//virtual Status initialize(const osgDB::Options* dbOptions);
			virtual Status initialize(const osgDB::Options* readOptions);
			//创建要素的概括类，包括其数据范围，空间坐标系等
			const FeatureProfile* createFeatureProfile();

			FeatureProfile* getFeatureProfile();

			bool getFeatures(const std::string& buffer, FeatureList& features);
			bool getFeatures(const std::string& buffer, FeatureList& features, float height);
		    //根据要素图层的名称获取请求要素数据的url
			std::string createAllURL();
			//根据一定的经纬度范围获取要素的url
			std::string createBoundURL(osgEarth::Bounds bds);
			std::string createOidURL(int oid);
			void saveResponse(const std::string buffer, const std::string& filename);
			bool getFeatures(std::istream& buffer, FeatureList& features);
			bool getFeatures(std::istream& buffer, FeatureList& features, float height);
			bool getFeatures(std::istream& buffer, FeatureList& features, float transparent = 1.0, float height = 0.0);

			virtual FeatureCursor* createFeatureCursor(const Symbology::Query& query, ProgressCallback* progress);

			virtual bool supportsGetFeature() const
			{
				return false;
			}
			virtual Feature* getFeature(FeatureID fid);

			virtual bool isWritable() const;

			virtual const FeatureSchema& getSchema() const;

			virtual osgEarth::Symbology::Geometry::Type getGeometryType() const;


		private:
			const GEOWFSFeatureOptions         _options;  
		   // osg::ref_ptr< GEOWFSCapabilities > _capabilities;
			osg::ref_ptr< FeatureProfile >  _featureProfile;
			FeatureSchema                   _schema;
			osg::ref_ptr<CacheBin>          _cacheBin;
			osg::ref_ptr<const osgDB::Options>    _dbOptions;    
			std::string                     _xmlPath;
			std::string m_OriginSRS;

			osg::ref_ptr< GEOWFSCapabilities> _capabilities;

			float                           _geoHeight;
			float                           _transparent;
		};
	}
}
#endif