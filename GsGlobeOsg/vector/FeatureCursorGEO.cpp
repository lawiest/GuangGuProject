#include "stdafx.h"
#include <math.h>
#include "FeatureCursorGEO.h"
#include "GEOFeatureOptions.h"
#define LC "[FeatureCursorGEO]"
#define GEO_SCOPED_LOCK GDAL_SCOPED_LOCK


using namespace osgEarth::Features;
using namespace osgEarth::Symbology;

FeatureCursorGEO::FeatureCursorGEO(
	GEOFeatureClass* geoFeaClass,
	const FeatureSource*     source,
	const FeatureProfile*    profile,
	const Symbology::Query&  query,
	float                    height,
	const FeatureFilterChain& filters,
	ProgressCallback*         progress) :
	FeatureCursor(progress),
    _geoFeatureClass  (geoFeaClass),
    _source           ( source ),
	_profile          ( profile ),
	_query            ( query ),
	_filters          ( filters ),
	_chunkSize        ( 500 ),
	_currentOidIndex        (0),
	_queryRasterSampleInterval         (0),
	_geoHeight        (height),
	_bhasNext        (false),
	_spatialFilter(0L)
{
	/////////////////////////////////初始化
	GEO_SCOPED_LOCK;
	
	std::string expr="select oid,xmin,ymin,xmax,ymax from dataindex";
	//绘制真矢量处理
	if (query.expression().isSet())
	{
		expr = query.expression().value();
		std::stringstream buf;
		buf<<"select oid,xmin,ymin,xmax,ymax from dataindex where "<<expr;
		expr=buf.str();
	//	expr="select oid,xmin,ymin,xmax,ymax from dataindex where oid<10";
		if (query.bounds().isSet())
		{
			_geoFeatureClass->excuteSeletSqlandQuerytoGetOID(*query.bounds(),expr,_queryOids);
			if (_queryOids.size()>0)
			{
				_bhasNext=true;
			}
		}
	}
	////绘制栅格处理
	//if (query.tileKey().isSet())
	//{
	//	_queryRasterSampleInterval=(*_query.tileKey()).getLOD();
	//	if (_queryRasterSampleInterval>6)
	//	{
	//		_geoFeatureClass->excuteSeletSqlandQuerytoGetOID((*query.tileKey()).getExtent().bounds(),expr,_queryOids);
	//		if (_queryOids.size()>0)
	//		{
	//			_bhasNext=true;
	//	    }
	//	}
	//}
	//if (query.tileKey().isSet())
	//{
	//	_queryRasterSampleInterval=(*_query.tileKey()).getLOD();
	//	
	//	if (_geoFeatureClass->getFeatureCount()<1000)
	//	{

	//	}
	//	else
	//	{
	//		std::stringstream buf;
	//		if (_queryRasterSampleInterval>=4 && _queryRasterSampleInterval<=14)//0-3层不显示,4-15分级渲染,4,5,6共用；7，8，9；10，11，12；
	//		{
	//			int temall;
	//			if (_queryRasterSampleInterval>=4 && _queryRasterSampleInterval<=6)
	//			{
	//				temall=4;
	//			}
	//			if (_queryRasterSampleInterval>=7 && _queryRasterSampleInterval<=9)
	//			{
	//				temall=7;
	//			}
	//			if (_queryRasterSampleInterval>=10 && _queryRasterSampleInterval<=14)
	//			{
	//				temall=10;
	//			}
	//			for(int i=1;i<=temall;i++)
	//			{
	//				//int tem=(int)1000*pow(0.8,i);
	//				int tem=(int)500*pow(0.8,i);
	//				if (i==1)
	//				{
	//					buf<<" where oid%"<<tem<<"==0";
	//				}
	//				else
	//				{
	//					buf<<" or oid%"<<tem<<"==0";
	//				}
	//				
	//			}
	//			expr=expr+buf.str();
	//			
	//		    _geoFeatureClass->excuteSeletSqlandQuerytoGetOID((*query.tileKey()).getExtent().bounds(),expr,_queryOids);
	//		    if (_queryOids.size()>0)
	//			{
	//				_bhasNext=true;
	//			}
	//			
	//		}
	//		else if(_queryRasterSampleInterval>=15)//16层
	//		{
	//			_geoFeatureClass->excuteSeletSqlandQuerytoGetOID((*query.tileKey()).getExtent().bounds(),expr,_queryOids);
	//			if (_queryOids.size()>0)
	//			{
	//				_bhasNext=true;
	//			}
	//		}
	//		else
	//		{
	//			//nothing
	//		}
	//	}
	//}
	
	else
	{
		for (int i=1;i<=_geoFeatureClass->getFeatureCount()-1;i++)
		{
			_queryOids.push_back(i);
		}
		if (_queryOids.size()>0)
		{
			_bhasNext=true;
		}
	}
    _querySize=_queryOids.size();
	
	readChunk();

}

FeatureCursorGEO::~FeatureCursorGEO()
{
	GEO_SCOPED_LOCK;
}

bool
	FeatureCursorGEO::hasMore() const
{
	return  _queue.size() > 0||_bhasNext ;
}

Feature*
	FeatureCursorGEO::nextFeature()
{
	if ( !hasMore() )
		return 0L;

	if ( _queue.size() == 0 && _bhasNext )
	{
		readChunk();
		if (_queue.size()==0)
		{
			OE_WARN<<LC<<"NO VALID FEATURE"<<std::endl;
			//return NULL;
		}
	}

	// do this in order to hold a reference to the feature we return, so the caller
	// doesn't have to. This lets us avoid requiring the caller to use a ref_ptr when 
	// simply iterating over the cursor, making the cursor move conventient to use.
	
		_lastFeatureReturned = _queue.front();
		_queue.pop();

		return _lastFeatureReturned.get();
	
}


// reads a chunk of features into a memory cache; do this for performance
// and to avoid needing the OGR Mutex every time
void
	FeatureCursorGEO::readChunk()
{
	FeatureList preProcessList;

	//GEO_SCOPED_LOCK;

	if (_bhasNext)
	{
		for(_currentOidIndex; _currentOidIndex<_querySize; _currentOidIndex=_currentOidIndex+1)
		{
			GEOFeatureH handle;
			_geoFeatureClass->createGEOFeatureH(_queryOids[_currentOidIndex],&handle);

			if ( handle.valid() )
			{
				osg::ref_ptr<Feature> f = GEOFeatureUtils::createFeature( &handle, _profile->getSRS(),_geoHeight );
				if ( f.valid() && !_source->isBlacklisted(f->getFID()) )
				{
					if (isGeometryValid( f->getGeometry() ) )
					{
						_queue.push( f );

						if ( _filters.size() > 0 )
							preProcessList.push_back( f.release() );
					}
					else
					{
						OE_INFO << LC << "Skipping feature with invalid geometry: " << f->getGeoJSON() << std::endl;
					}
				} 
			}

		}
		_bhasNext=false;
		
	}
	// preprocess the features using the filter list:
	if ( preProcessList.size() > 0 )
	{
		FilterContext cx;
		cx.setProfile( _profile.get() );

		for(FeatureFilterChain::const_iterator i = _filters.begin(); i != _filters.end(); ++i )
		{
			FeatureFilter* filter = i->get();
			cx = filter->push( preProcessList, cx );
		}
	}
}




