#ifndef OSGEARTHFEATURES_FEATURE_GEOFEATURE_CURSOR_GEO
#define OSGEARTHFEATURES_FEATURE_GEOFEATURE_CURSOR_GEO 1

#include <osgEarthFeatures/Feature>
#include <osgEarthFeatures/FeatureSource>
#include <osgEarthFeatures/FeatureCursor>
#include <osgEarthFeatures/Filter>
#include <osgEarthSymbology/Query>
#include <osgEarth/Registry>
#include <algorithm>

#include <queue>
#include <vector>
//#include "stdafx.h"
#include "GeoFeature_SqliteApi.h"
#include "GEOFeatureUtils.h"
namespace GeoGlobe
{
	namespace GeoVector
	{
		
		using namespace osgEarth::Features;


		//该类的主要功能用于确定所有要素中当前要素
		class FeatureCursorGEO : public FeatureCursor
		{
		public:
			FeatureCursorGEO(
				GEOFeatureClass* geoFeaClass,
				const FeatureSource*     source,
				const FeatureProfile*    profile,
				const Symbology::Query&  query,
				float                    height,
				const FeatureFilterChain& filters,
				ProgressCallback*         progress
				);
		public: // FeatureCursor

			bool hasMore() const;//判断是否还有要素
			Feature* nextFeature();//获取下一个要素


			//判断点是否有效
			inline bool isPointValid( const osg::Vec3d& v, double thresh = 1e10 )
			{
				return (!v.isNaN() && osg::absolute( v.x() ) < thresh && osg::absolute( v.y() ) < thresh && osg::absolute( v.z() ) < thresh );
			}

            //判断几何要素是否有效
			inline bool isGeometryValid( osgEarth::Symbology::Geometry* geometry )
			{        
				if (!geometry) return false;

				if (!geometry->isValid()) return false;

				for (osgEarth::Symbology::Geometry::const_iterator i = geometry->begin(); i != geometry->end(); ++i)
				{
					if (!isPointValid( *i ))
					{
						return false;
					}
				}
				return true;
			}
		protected:
			virtual ~FeatureCursorGEO();
		private:
			void readChunk();    
			
		//private:
		public:
			GEOFeatureClass*                    _geoFeatureClass;//与本地要素文件相关联的要素类
			osg::ref_ptr<const FeatureSource>   _source;//要素图层的数据来源
			osg::ref_ptr<const FeatureProfile>  _profile;//该要素图层的基本信息概括
			Symbology::Query                    _query;//筛选要素的条件设定
			const FeatureFilterChain&            _filters;//根据该类可以对要素进行一系列的操作

			GEOGeometryH*                        _spatialFilter;//吉奥几何要素的操作类

			std::queue< osg::ref_ptr<Feature> > _queue;
			osg::ref_ptr<Feature>               _lastFeatureReturned;
			
			unsigned                            _chunkSize;
			unsigned                            _currentOidIndex;
			bool                                _bhasNext;
			std::vector<int>                     _queryOids;
			unsigned                             _querySize;
			unsigned                             _queryRasterSampleInterval;
			float                                _geoHeight;
		};

	}//geovector
}//geoglobe
#endif//OSGEARTHFEATURES_FEATURE_GEOFEATURE_CURSOR_GEO