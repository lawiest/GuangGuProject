
#include "MemCache.h"
using namespace std;

using namespace osgEarth;


		

GeoGlobe::Tool::MemCache::MemCache():m_nMax(300)
{
	_pruneCount = 0;

	_pruneLodModelCount = 0;
	_pruneGeometryCount = 0;
	_pruneTextureCount = 0;

	m_nLodModelMax = 10000;
	m_nTextureMax = 10000;
	m_nGeometryMax = 10000;
}

void GeoGlobe::Tool::MemCache::prune()
{
	// assume an exclusive mutex is taken.
	//Threading::ScopedWriteLock exclusive( _cacheMutex );
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_Mutex);
	unsigned delete_count = 0;

	for( ObjectCache::iterator i = _objectCache.begin(); i != _objectCache.end(); )
	{
		if ( i->second.get()->referenceCount() == 1 )
		{
			// do not call releaseGLObjects since the attrs themselves might still be shared
			_objectCache.erase( i++ );
			delete_count++;
		}
		else
		{
			++i;
		}
	}

	_pruneCount = _pruneCount - delete_count;
	//_objectCache.clear();

	//cout<<
	//OE_DEBUG << LC << "Pruned " << sa_count << " attributes, " << ss_count << " statesets" << std::endl;
}
	
void GeoGlobe::Tool::MemCache::pruneIfNecessary()
{
	// assume an exclusve mutex is taken
	if ( _pruneCount++ >= m_nMax )
	{
		//_pruneCount = 0;
		prune();
	}
}

osg::ref_ptr<osg::Texture2D> GeoGlobe::Tool::MemCache::getTexture(std::string texName,bool& find)
{
	Threading::ScopedWriteLock exclusive( _cacheMutex );
	find=true;
				
	osg::ref_ptr<osg::Texture2D> tex;
	unsigned long long nHash = CityHash64(texName.c_str(), texName.size());
	ObjectCache::iterator itr = _objectCache.find(nHash);
	if (itr!=_objectCache.end()) 
	{
		tex=(osg::Texture2D*)(itr->second.get());
		return tex;
	}

	//not found //
	pruneIfNecessary();
	find=false;
	tex=new osg::Texture2D;
	tex->setName(texName);
	tex->setWrap(osg::Texture2D::WRAP_R, osg::Texture::REPEAT);
	tex->setWrap(osg::Texture2D::WRAP_S, osg::Texture::REPEAT);
	tex->setWrap(osg::Texture2D::WRAP_T, osg::Texture::REPEAT);
	_objectCache.insert(make_pair(nHash,tex.get()));

	return tex;
				
}

osg::ref_ptr<osg::StateSet> GeoGlobe::Tool::MemCache::getStateSet(std::string statesetName,bool& find)
{
	Threading::ScopedWriteLock exclusive( _cacheMutex );
	find=true;

	osg::ref_ptr<osg::StateSet> stateset;
	unsigned long long nHash = CityHash64(statesetName.c_str(), statesetName.size());
	ObjectCache::iterator itr = _objectCache.find(nHash);
	if (itr!=_objectCache.end()) 
	{
		stateset=(osg::StateSet*)(itr->second.get());
		return stateset;
	}

	//not found //
	pruneIfNecessary();
	find=false;
	stateset=new osg::StateSet;
	stateset->setName(statesetName);
/*	tex->setWrap(osg::Texture2D::WRAP_R, osg::Texture::REPEAT);
	tex->setWrap(osg::Texture2D::WRAP_S, osg::Texture::REPEAT);
	tex->setWrap(osg::Texture2D::WRAP_T, osg::Texture::REPEAT);*/
				
	_objectCache.insert(make_pair(nHash,stateset.get()));

	return stateset;

}

void GeoGlobe::Tool::MemCache::addObjectToCache(std::string objectName,osg::Object* object)
{
	Threading::ScopedWriteLock exclusive( _cacheMutex );
	pruneIfNecessary();
	unsigned long long nHash = CityHash64(objectName.c_str(), objectName.size());
	_objectCache.insert(make_pair(nHash,object));
}

osg::ref_ptr<osg::Object> GeoGlobe::Tool::MemCache::getObjectFromCache(std::string objectName)
{
	Threading::ScopedReadLock shared( _cacheMutex );
	unsigned long long nHash = CityHash64(objectName.c_str(), objectName.size());
	ObjectCache::iterator itr = _objectCache.find(nHash);
	if (itr!=_objectCache.end()) return itr->second;
	else return NULL;

}

void GeoGlobe::Tool::MemCache::pruneLodModelIfNecessary()
{
	if (_pruneLodModelCount++ < m_nLodModelMax )
		return;

	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_LodModelMutex);
	unsigned delete_count = 0;

	for( ObjectCache::iterator i = _LodModelCache.begin(); i != _LodModelCache.end(); )
	{
		if ( i->second.get()->referenceCount() == 1 )
		{
			_LodModelCache.erase( i++ );
			delete_count++;
		}
		else
		{
			++i;
		}
	}

	_pruneLodModelCount = _pruneLodModelCount - delete_count;
}

void GeoGlobe::Tool::MemCache::addLodMdelToCache(std::string objectName,osg::Object* object)
{
	Threading::ScopedWriteLock exclusive( _LodModelcacheMutex );
	pruneLodModelIfNecessary();
	unsigned long long nHash = CityHash64(objectName.c_str(), objectName.size());
	_LodModelCache.insert(make_pair(nHash,object));
}

osg::ref_ptr<osg::Object> GeoGlobe::Tool::MemCache::getLodMdelFromCache(std::string objectName)
{
	Threading::ScopedReadLock shared( _LodModelcacheMutex );
	unsigned long long nHash = CityHash64(objectName.c_str(), objectName.size());
	ObjectCache::iterator itr = _LodModelCache.find(nHash);
	if (itr!=_LodModelCache.end()) return itr->second;
	else return NULL;
}

void GeoGlobe::Tool::MemCache::pruneTextureIfNecessary()
{
	if ( _pruneTextureCount++ < m_nTextureMax )
		return;

	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_TextureMutex);
	unsigned delete_count = 0;

	for( ObjectCache::iterator i = _TextureCache.begin(); i != _TextureCache.end(); )
	{
		if ( i->second.get()->referenceCount() == 1 )
		{
			_TextureCache.erase( i++ );
			delete_count++;
		}
		else
		{
			++i;
		}
	}

	_pruneTextureCount = _pruneTextureCount - delete_count;
}

void GeoGlobe::Tool::MemCache::addTextureToCache(std::string objectName,osg::Object* object)
{
	Threading::ScopedWriteLock exclusive( _TexturecacheMutex );
	pruneTextureIfNecessary();
	unsigned long long nHash = CityHash64(objectName.c_str(), objectName.size());
	_TextureCache.insert(make_pair(nHash,object));
}

osg::ref_ptr<osg::Object> GeoGlobe::Tool::MemCache::getTextureFromCache(std::string objectName)
{
	Threading::ScopedReadLock shared( _TexturecacheMutex );
	unsigned long long nHash = CityHash64(objectName.c_str(), objectName.size());
	ObjectCache::iterator itr = _TextureCache.find(nHash);
	if (itr!=_TextureCache.end()) return itr->second;
	else return NULL;
}

void GeoGlobe::Tool::MemCache::pruneGeometryIfNecessary()
{
	if ( _pruneGeometryCount++ < m_nGeometryMax )
		return;

	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_GeometryMutex);
	unsigned delete_count = 0;

	for( ObjectCache::iterator i = _GeometryCache.begin(); i != _GeometryCache.end(); )
	{
		if ( i->second.get()->referenceCount() == 1 )
		{
			_GeometryCache.erase( i++ );
			delete_count++;
		}
		else
		{
			++i;
		}
	}

	_pruneGeometryCount = _pruneGeometryCount - delete_count;
}

void GeoGlobe::Tool::MemCache::addGeometryToCache(std::string objectName,osg::Object* object)
{
	Threading::ScopedWriteLock exclusive( _GeometrycacheMutex );
	pruneGeometryIfNecessary();
	unsigned long long nHash = CityHash64(objectName.c_str(), objectName.size());
	_GeometryCache.insert(make_pair(nHash,object));
}

osg::ref_ptr<osg::Object> GeoGlobe::Tool::MemCache::getGeometryFromCache(std::string objectName)
{
	Threading::ScopedReadLock shared( _GeometrycacheMutex );
	unsigned long long nHash = CityHash64(objectName.c_str(), objectName.size());
	ObjectCache::iterator itr = _GeometryCache.find(nHash);
	if (itr!=_GeometryCache.end()) return itr->second;
	else return NULL;
}

