#ifndef GEOGLOBE_TOOL_MEMCACHE
#define GEOGLOBE_TOOL_MEMCACHE 1

#include <osg/Referenced>
#include <osgEarth/ThreadingUtils>
#include <osg/Texture2D>
//#include <Model/Model.h>
//#include <Pipe/PipeTile.h>

#include "Tool\cityhash\city.h"
using namespace std;

namespace GeoGlobe
{
	namespace Tool
	{

		class MemCache: public osg::Referenced
		{
		public:
			osgEarth::Threading::ReadWriteMutex _cacheMutex;
			osgEarth::Threading::Mutex _Mutex;
			unsigned _pruneCount;
			typedef std::map<unsigned long long, osg::ref_ptr<osg::Object> > ObjectCache;
			ObjectCache _objectCache;
			const int m_nMax;

			osgEarth::Threading::ReadWriteMutex _LodModelcacheMutex;
			ObjectCache _LodModelCache;
			unsigned _pruneLodModelCount;
			osgEarth::Threading::Mutex _LodModelMutex;
			int m_nLodModelMax;

			osgEarth::Threading::ReadWriteMutex _GeometrycacheMutex;
			ObjectCache _GeometryCache;
			unsigned _pruneGeometryCount;
			osgEarth::Threading::Mutex _GeometryMutex;
			int m_nGeometryMax;

			osgEarth::Threading::ReadWriteMutex _TexturecacheMutex;
			ObjectCache _TextureCache;
			unsigned _pruneTextureCount;
			osgEarth::Threading::Mutex _TextureMutex;
			int m_nTextureMax;


			MemCache();
			void prune();
	
			void pruneIfNecessary();

			osg::ref_ptr<osg::Texture2D> getTexture(std::string texName,bool& find);

			osg::ref_ptr<osg::StateSet> getStateSet(std::string statesetName,bool& find);

			void addObjectToCache(std::string objectName,osg::Object* object);
			osg::ref_ptr<osg::Object> getObjectFromCache(std::string objectName);

			void pruneLodModelIfNecessary();

			void addLodMdelToCache(std::string objectName,osg::Object* object);
			osg::ref_ptr<osg::Object> getLodMdelFromCache(std::string objectName);

			void pruneTextureIfNecessary();
			void addTextureToCache(std::string objectName,osg::Object* object);

			osg::ref_ptr<osg::Object> getTextureFromCache(std::string objectName);

			void pruneGeometryIfNecessary();

			void addGeometryToCache(std::string objectName,osg::Object* object);

			osg::ref_ptr<osg::Object> getGeometryFromCache(std::string objectName);

		};

	}
}


#endif
