#ifndef MODEL_PAGER
#define MODEL_PAGER 1
#include <map>
#include <set>
#include <osg/Node>
#include <Model/CT3DDriver.h>
#include <Tool/SQLiteOP.h>
#include <Model/Feature.h>
#include <osgViewer/Viewer>
#include <osgEarth/Map>
#include <osgEarthUtil/EarthManipulator>
#include <Tool/CommonFunction.h>
#include <Tool/IDConverter.h>
#include <Model/GeometryWrap.h>
#include <Model/LodModel.h>
#include <android/log.h>


using namespace osg;



typedef std::multimap<double,osg::ref_ptr<CFeature> > SORT_FEATURE_MAP;
typedef std::map<unsigned long long, osg::ref_ptr<CFeature> > FEATURE_MAP;
typedef std::map<unsigned long long,osg::ref_ptr<osg::Image> > IMAGE_MAP;


struct ModelTile
{
	unsigned long long tileKey;
	FEATURE_MAP features;
};
typedef std::map<int,ModelTile> TREE_MAP;

typedef std::pair<osg::ref_ptr<osg::Node>, double >          NodeTimeStampPair;
typedef std::map<unsigned long long, NodeTimeStampPair >                NODE_CACHE;
typedef std::pair<osg::ref_ptr<osg::Image>, double >          ImageTimeStampPair;
typedef std::map<unsigned long long, ImageTimeStampPair >                IMAGE_CACHE;

typedef std::pair<osg::ref_ptr<CGeometryWrap>, double >          ModelTimeStampPair;
typedef std::map<unsigned long long, ModelTimeStampPair >                   MODEL_CACHE;



struct DataRequest
{
	unsigned long long geoKey;
	std::string dataName;
	Location location;
	int       curLOD;
};
typedef std::list< DataRequest >              RequestList;



//class LoadModelThread:public OpenThreads::Thread,public osg::Referenced
//{
//public:
//	bool              _done;
//	OpenThreads::Mutex _cacheMutex;
//	OpenThreads::Mutex _requestMutex;
//	OpenThreads::Block _block;
//	osg::ref_ptr<osgViewer::Viewer> _viewer;
//
//	NODE_CACHE _nodeCache;
//	IMAGE_CACHE _imageCache;
//	RequestList _requestList;
//	double _expiryDelay;
//	std::list<unsigned long long> _textureRequestList;
//
//
//	MODEL_CACHE _modelCache;
//
//	void block()
//	{
//		_block.reset();
//		_block.block();
//
//	}
//
//	void release()
//	{
//		_block.release();
//
//	}
//
//	LoadModelThread(osgViewer::Viewer* viewer)
//	{
//
//		_viewer=viewer;
//		_done=false;
//		_expiryDelay=10;
//	}
//	~LoadModelThread(){cancel();}
//
//	osg::ref_ptr<osg::Node>/*osg::ref_ptr<CGeometryWrap>*/ getModel(unsigned long long geoKey)
//	{
//		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);
//		NODE_CACHE::iterator itr = _nodeCache.find(geoKey);
//		if (itr!=_nodeCache.end()) 
//		{
//			return  itr->second.first;
//			
//		}
//
//		//MODEL_CACHE::iterator itr = _modelCache.find(geoKey);
//		//if (itr!=_modelCache.end()) 
//		//{
//		//	return  itr->second.first;
//
//		//}
//
//		return NULL;
//
//	}
//
//	osg::ref_ptr<osg::Image> getImage(unsigned long long texKey)
//	{
//		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);
//		IMAGE_CACHE::iterator itr = _imageCache.find(texKey);
//		if (itr!=_imageCache.end()) 
//		{
//			return  itr->second.first;
//
//		}
//
//		return NULL;
//
//	}
//
//	//void addRequest(DataRequest dataRequest)
//	//{
//	//	OpenThreads::Mutex _requestMutex;
//	//	_requestList.push_front(dataRequest);
//	//}
//	void updateRequest(RequestList requestList)
//	{
//		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_requestMutex);
//		//_requestList.swap()
//		_requestList=requestList;
//	}
//
//	bool getFirstRequest(DataRequest& dataRequest)
//	{
//		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_requestMutex);
//		if (_requestList.size()>0)
//	   {
//		dataRequest=_requestList.front();
//		
//		_requestList.pop_front();
//		return true;
//		}
//		return false;
//		//.push_front(dataRequest);
//	}
//
//	bool isRequestListEmpty()
//	{
//		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_requestMutex);
//		if (_requestList.size()>0)
//			return false;
//		else
//			return true;
//	}
//
//
//	void removeExpiredNodesInCache(const osg::FrameStamp& frameStamp)
//	{
//		double expiryTime = frameStamp.getReferenceTime() - _expiryDelay;
//
//		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);
//
//		// Remove expired entries from object cache
//		NODE_CACHE::iterator itr = _nodeCache.begin();
//		while(itr != _nodeCache.end())
//		{
//			if (itr->second.second<=expiryTime)
//			{
//				_nodeCache.erase(itr++);
//			}
//			else
//			{
//				++itr;
//			}
//		}
//
//		IMAGE_CACHE::iterator itr2 = _imageCache.begin();
//		while(itr2 != _imageCache.end())
//		{
//			if (itr2->second.second<=expiryTime)
//			{
//				_imageCache.erase(itr2++);
//			}
//			else
//			{
//				++itr2;
//			}
//		}
//
//	/*	MODEL_CACHE::iterator itr = _modelCache.begin();
//		while(itr != _modelCache.end())
//		{
//			if (itr->second.second<=expiryTime)
//			{
//				_modelCache.erase(itr++);
//			}
//			else
//			{
//				++itr;
//			}
//		}
//
//		IMAGE_CACHE::iterator itr2 = _imageCache.begin();
//		while(itr2 != _imageCache.end())
//		{
//			if (itr2->second.second<=expiryTime)
//			{
//				_imageCache.erase(itr2++);
//			}
//			else
//			{
//				++itr2;
//			}
//		}*/
//
//	
//
//
//	}
//
//	void updateTimeStampOfNodesInCacheWithExternalReferences(const osg::FrameStamp& frameStamp)
//	{
//		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);
//
//		// look for objects with external references and update their time stamp.
//		for(NODE_CACHE::iterator itr=_nodeCache.begin();
//			itr!=_nodeCache.end();
//			++itr)
//		{
//			// if ref count is greater the 1 the object has an external reference.
//			if (itr->second.first->referenceCount()>1)
//			{
//				// so update it time stamp.
//				itr->second.second = frameStamp.getReferenceTime();
//			}
//		}
//
//		//for(MODEL_CACHE::iterator itr=_modelCache.begin();
//		//	itr!=_modelCache.end();
//		//	++itr)
//		//{
//		//	// if ref count is greater the 1 the object has an external reference.
//		//	if (itr->second.first->referenceCount()>1)
//		//	{
//		//		// so update it time stamp.
//		//		itr->second.second = frameStamp.getReferenceTime();
//		//	}
//		//}
//
//		for(IMAGE_CACHE::iterator itr=_imageCache.begin();
//			itr!=_imageCache.end();
//			++itr)
//		{
//			// if ref count is greater the 1 the object has an external reference.
//			if (itr->second.first->referenceCount()>1)
//			{
//				// so update it time stamp.
//				itr->second.second = frameStamp.getReferenceTime();
//			}
//		}
//
//	}
//
//	void addTextureRequest(CGeometryWrap* gw,int lod)
//	{
//		std::map<int,std::vector<unsigned long long>>::iterator itr=gw->_lodTexture.find(lod);
//		if (itr==gw->_lodTexture.end())
//		return;
//
//	    std::vector<unsigned long long> textures=itr->second;
//		for (int i=0;i<textures.size();i++)
//		{
//			_textureRequestList.push_back(textures[i]);
//		}
//		 //while(itr!=gw->_vecTexture.end())
//		 //{
//			// _textureRequestList.push_back(itr->first);
//			// itr++;
//		 //}
//	}
//
//	void addTextureRequest(osg::Node* node,int lod)
//	{
//		LodModel* lodModel=(LodModel* )node;
//		std::map<int,std::vector<unsigned long long>>::iterator itr=lodModel->_lodTexture.find(lod);
//		if (itr==lodModel->_lodTexture.end())
//			return;
//
//		std::vector<unsigned long long> textures=itr->second;
//		for (int i=0;i<textures.size();i++)
//		{
//			_textureRequestList.push_back(textures[i]);
//		}
//
//	}
//
//	void prepareGeometryData()
//	{
//		CSQLiteOP pSQLiteGeoOP;
//		pSQLiteGeoOP.OpenDataBase("C:/Users/Administrator/Desktop/jinyang/jianzhu/NoDefine.geo");
//		do 
//		{
//
//		   DataRequest dataRequest;
//		   bool succeed=getFirstRequest(dataRequest);
//	       if(!succeed)
//			 break;
//		   NODE_CACHE::iterator modelItr=_nodeCache.find(dataRequest.geoKey);
//		   if(modelItr==_nodeCache.end())
//		   {
//
//			//Buffer   geoBuffer;
//			//pSQLiteGeoOP.GetGeometryByGeoKey(dataRequest.geoKey,geoBuffer);
//	//		CGeometryWrap* gw=new CGeometryWrap(geoBuffer,dataRequest.location);
//	//		addTextureRequest(gw,dataRequest.curLOD);
//			osg::Node* node=osgDB::readNodeFile(dataRequest.dataName);
//			//osg::Node* node=NULL;
//			LodModel* lodModel=(LodModel*)node;
//			prepareMaterial(lodModel,dataRequest.curLOD);
//
//
//			_viewer->getFrameStamp()->getReferenceTime();
//			//addTextureRequest(node,dataRequest.curLOD);
//			//addTextureRequest(gw,dataRequest.curLOD);
//			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);
//			double time=_viewer->getFrameStamp()->getReferenceTime();
//			_nodeCache.insert(make_pair(dataRequest.geoKey,make_pair(node,time)));
//		/*	_modelCache.insert(make_pair(dataRequest.geoKey,make_pair(gw,time)));*/
//			
//			//microSleep(50000L);
//		   }
//		   //else
//		   //{
//			  // addTextureRequest(modelItr->second.first,dataRequest.curLOD);
//			  // 
//		   //}
//		 microSleep(10000L);
//		} while (1);
//
//		pSQLiteGeoOP.CloseDataBase();
//	}
//
//	void PrepareTextureData()
//	{
//		
//		CSQLiteOP SQLiteTexOP;
//		SQLiteTexOP.OpenDataBase("C:/Users/Administrator/Desktop/jinyang/jianzhu/NoDefine.tex");
//		std::list<unsigned long long>::iterator itr=_textureRequestList.begin();
//		while(itr!=_textureRequestList.end())
//		{
//
//			
//			IMAGE_CACHE::iterator imageItr=_imageCache.find(*itr);
//			if (imageItr==_imageCache.end())
//			{
//				
//				
//				Buffer texBuffer;
//				if(SQLiteTexOP.GetTextureByTexKey(*itr,texBuffer))
//				{
//					CMaterial pMaterial;
//
//					if (!pMaterial.readBuffer(texBuffer)||0!= pMaterial.getMaterialType())
//						continue;;
//
//					std::string string;
//					string.assign((char*)texBuffer.getCurPtr(),texBuffer.size() - texBuffer.getPos() + 1);
//					std::stringstream stream;
//					stream<<string;
//
//					std::string ext=osgDB::getFileExtension(pMaterial.getMaterialName());
//					osg::ref_ptr<osgDB::ReaderWriter> rw = osgDB::Registry::instance()->getReaderWriterForExtension( ext );
//					osg::ref_ptr<osg::Image> image = rw->readImage(stream).getImage();
//					if (image!=NULL)
//					{
//						double time=_viewer->getFrameStamp()->getReferenceTime();
//						OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);
//
//						_imageCache.insert(make_pair(*itr,make_pair(image,time)));
//						//_imageCache.insert()
//					}
//					
//
//				}
//				
//			}
//			//microSleep(50000L);
//			itr++;
//		}
//		SQLiteTexOP.CloseDataBase();
//		_textureRequestList.clear();
//	}
//
//
//	osg::Texture2D* createTexture(UID64 nTexId) const
//	{
//		//osg::Texture2D* texture;
//		std::string DBPath="C:/Users/Administrator/Desktop/jinyang/jianzhu/NoDefine";
//		std::stringstream ct3dTexName;
//		ct3dTexName<<DBPath<<"."<<nTexId<<".ct3d_tex";
//	
//		osg::ref_ptr<osg::Image> image=osgDB::readImageFile(ct3dTexName.str()/*,options*/);
//		//	osg::ref_ptr<osg::Image> image;
//		//osg::ref_ptr<osg::Image> image=osgDB::readImageFile("C:/Users/Administrator/Desktop/jinyang/jianzhu//NoDefine.1970410736473887.ct3d_tex");
//		if(image)
//		{
//			osg::Texture2D* texture = new osg::Texture2D(image );
//
//
//			texture->setWrap(osg::Texture2D::WRAP_R, osg::Texture::REPEAT);
//			texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture::REPEAT);
//			texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture::REPEAT);
//			return texture;
//		}
//		return NULL;
//	}
//
//
//
//	void prepareMaterial(LodModel* lodModel,int lod)
//	{
//		
//		if (lodModel->isStateSetExist(lod))
//		             return;
//
//		//if (!lodModel->isMaterialExist(lod))
//		//	return;
//		
//		std::vector<osg::ref_ptr<CMaterial>> vecMaterial;
//			if(!lodModel->getMaterial(lod,vecMaterial))
//				return ;
//
//		for (int i=0;i<vecMaterial.size();i++)
//		{
//		
//		osg::StateSet* stateset=new osg::StateSet;
//		
//		int nMatType = vecMaterial[i]->getMaterialType();
//		switch(nMatType)
//		{
//		case 0:
//			break;
//		case 1:
//			{
//				UID64 nTexId = vecMaterial[i]->getSimpleMat().nTextureID;
//				osg::Texture2D* texture;
//				if (Invalid_ID != nTexId)
//				{
//					nTexId = IDConverter::MaterialLodIDToMatID(nTexId, 0);
//					texture=createTexture(nTexId);
//
//				}
//				
//				stateset->setTextureAttributeAndModes(0, texture,osg::StateAttribute::PROTECTED|osg::StateAttribute::ON );
//
//				stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
//				stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
//				osg::TexEnvCombine *te = new osg::TexEnvCombine; 
//				te->setCombine_RGB(osg::TexEnvCombine::REPLACE);
//				stateset->setTextureAttributeAndModes(0, te, osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
//	
//
//			}
//			break;
//		case 2:
//			{
//			ComMat_PARAM commat_param = vecMaterial[i]->getComPlexMat();
//				if (commat_param.pComm_item != NULL)
//				{
//					for(int i = 0; i < commat_param.nItemNum; i++)
//					{
//						COMMAT_ITEM& commat_item =  commat_param.pComm_item[i];
//						for(int j = 0; j < 1/*commat_item.nItemNum*/; j++)
//						{
//							TEXTURE_ITEM& texture_item = commat_item.pComm_item[j];
//							UID64 nTexId = texture_item.nTextureID;
//							
//							osg::Texture2D* texture=createTexture(nTexId);
//						
//						
//							
//						stateset->setTextureAttributeAndModes(texture_item.nTexIndex, texture,osg::StateAttribute::PROTECTED|osg::StateAttribute::ON );
//					//		stateset->setTextureAttributeAndModes(1, NULL,osg::StateAttribute::OVERRIDE|osg::StateAttribute::PROTECTED|osg::StateAttribute::OFF );
//						/*	if  ( j==0&&texture->getImage()->isImageTranslucent())
//							{*/
//								OSG_INFO<<"Found transparent image"<<std::endl;
//								stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
//								stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
//							/*}*/
//
//							osg::TexEnvCombine *te = new osg::TexEnvCombine;    
//
//							if(texture_item.nColorBlendMode.nOperation==2)
//							te->setCombine_RGB(osg::TexEnvCombine::REPLACE);
//							else if(texture_item.nColorBlendMode.nOperation==4)
//							te->setCombine_RGB(osg::TexEnvCombine::MODULATE);
//
//							te->setSource0_RGB(osg::TexEnvCombine::TEXTURE);
//							te->setOperand0_RGB(osg::TexEnvCombine::SRC_COLOR);
//							te->setSource1_RGB(osg::TexEnvCombine::PREVIOUS);
//							te->setOperand1_RGB(osg::TexEnvCombine::SRC_COLOR);
//
//							//if(texture_item.nAlphaBlendMode.nOperation==2)
//								te->setCombine_Alpha(osg::TexEnvCombine::REPLACE);
//		/*					else if(texture_item.nAlphaBlendMode.nOperation==4)
//								te->setCombine_Alpha(osg::TexEnvCombine::MODULATE);*/
//					
//							te->setSource0_Alpha(osg::TexEnvCombine::TEXTURE);
//							te->setOperand0_Alpha(osg::TexEnvCombine::SRC_ALPHA);
//							te->setSource1_Alpha(osg::TexEnvCombine::PREVIOUS);
//							te->setOperand1_Alpha(osg::TexEnvCombine::SRC_ALPHA);
//
//							if(j!=0 && texture_item.nAlphaBlendMode.nOperation==3)
//							{
//								te->setConstantColor(osg::Vec4(0,0,0,0));
//								te->setCombine_Alpha(osg::TexEnvCombine::ADD);
//								te->setSource0_Alpha(osg::TexEnvCombine::CONSTANT);
//								te->setOperand0_Alpha(osg::TexEnvCombine::SRC_ALPHA);
//								te->setSource1_Alpha(osg::TexEnvCombine::PREVIOUS);
//								te->setOperand1_Alpha(osg::TexEnvCombine::SRC_ALPHA);
//							}
//
//
//
//
//
//
//							stateset->setTextureAttributeAndModes(texture_item.nTexIndex, te, osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
//							//stateset->setm
//							
//						}
//					}
//				}
//			}
//			break;
//		default:
//			break;
//		}
//
//		lodModel->addStateSet(lod,stateset);
//	}
//	}
//
//	virtual void run()
//	{
//		do 
//		{
//			if(isRequestListEmpty())
//				block();
//			prepareGeometryData();
//			
//		//	PrepareTextureData();
//			//osg::Timer_t beforeCullTick = osg::Timer::instance()->tick();
//	
//			//DataRequest dataRequest;
//			//bool succeed=getFirstRequest(dataRequest);
//			//if (!succeed)
//			//	block();
//			////else if(_nodeCache.find(dataRequest.geoKey)==_nodeCache.end())
//			////{
//			////	osg::Node* node=osgDB::readNodeFile(dataRequest.dataName);
//			////	//_viewer->getFrameStamp()->getReferenceTime();
//			////	double time=_viewer->getFrameStamp()->getReferenceTime();
//			////	_nodeCache.insert(make_pair(dataRequest.geoKey,make_pair(node,time)));
//			////	//microSleep(50000L);
//			////}
//
//			//else if(_modelCache.find(dataRequest.geoKey)==_modelCache.end())
//			//{
//			//	CSQLiteOP pSQLiteGeoOP;
//			//	pSQLiteGeoOP.OpenDataBase("C:/Users/Administrator/Desktop/jinyang/jianzhu/NoDefine.geo");
//			//	Buffer   geoBuffer;
//			//	pSQLiteGeoOP.GetGeometryByGeoKey(dataRequest.geoKey,geoBuffer);
//			//	CGeometryWrap* gw=new CGeometryWrap(geoBuffer,dataRequest.location);
//			//	addTextureRequest(gw);
//			//	//osg::Node* node=osgDB::readNodeFile(dataRequest.dataName);
//			//	//_viewer->getFrameStamp()->getReferenceTime();
//			//	double time=_viewer->getFrameStamp()->getReferenceTime();
//			//	_modelCache.insert(make_pair(dataRequest.geoKey,make_pair(gw,time)));
//			//	//microSleep(50000L);
//			//}
//
//
//
//		} while (!_done);
//	}
//
//	virtual int cancel()
//	{
//		release();
//		_done=true;
//		while(isRunning())
//		{
//			//OpenThreads::Thread::YieldCurrentThread();
//			microSleep(100000L);
//		}
//		Thread::cancel();
//
//		return 0;
//	}
//
//
//
//
//};



class ModelPager:public  osg::Referenced
{
public:

	RequestList _requestList;
	osg::ref_ptr<osgViewer::Viewer> _viewer;
	NODE_CACHE _nodeCache;
	IMAGE_CACHE _imageCache;
	OpenThreads::Mutex _cacheMutex;
	OpenThreads::Mutex _requestMutex;
	double _expiryDelay;


	ModelPager(osgViewer::Viewer* viewer)
	{
//		__android_log_print(ANDROID_LOG_INFO, "ModelPager", "getIN");
		_viewer=viewer;
		_expiryDelay=2;
	}

	~ModelPager()
	{

		for (int i=0;i<_loadModelThreads.size();i++)
		{
			_loadModelThreads[i]->cancel();
		}
	}

	void setUpThreads(int numThreads)
	{
		_loadModelThreads.clear();
		int numProcessors = OpenThreads::GetNumberOfProcessors();
		for (int i=0;i<numThreads;i++)
		{
			LoadModelThread2* thread = new LoadModelThread2(this, _viewer);
			 thread->setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_MIN);
			 
			 thread->setProcessorAffinity((i+1) % numProcessors);
			_loadModelThreads.push_back(thread);
			// updateThread->setProcessorAffinity(3);
		}

		for (int i=0;i<_loadModelThreads.size();i++)
		{
			_loadModelThreads[i]->startThread();
		}

	}

	void updateRequest(RequestList requestList)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_requestMutex);
		//_requestList.swap()
		_requestList=requestList;
	}

	bool isRequestListEmpty()
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_requestMutex);
		if (_requestList.size()>0)
			return false;
		else
			return true;
	}

	bool getFirstRequest(DataRequest& dataRequest)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_requestMutex);
		if (_requestList.size()>0)
		{
			dataRequest=_requestList.front();

			_requestList.pop_front();
			return true;
		}
		return false;
		
	}

	osg::ref_ptr<osg::Node> getModel(unsigned long long geoKey)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);
		NODE_CACHE::iterator itr = _nodeCache.find(geoKey);
		if (itr!=_nodeCache.end()) 
		{
			return  itr->second.first;

		}

		return NULL;

	}
	std::vector<osg::ref_ptr<osg::Node> > _test;
	bool addModel(unsigned long long geoKey,osg::Node* model)
	{
		_viewer->getFrameStamp()->getReferenceTime();

		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);
		double time=_viewer->getFrameStamp()->getReferenceTime();
		if(_nodeCache.find(geoKey)!=_nodeCache.end())
			int a=0;
		_nodeCache.insert(make_pair(geoKey,make_pair(model,time)));
		//_test.push_back(model);
		return true;
	}

	void removeExpiredNodesInCache(const osg::FrameStamp& frameStamp)
	{
//		__android_log_print(ANDROID_LOG_INFO, "removeExpired", "entered");
		double expiryTime = frameStamp.getReferenceTime() - _expiryDelay;

		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);
		//_test.clear();
		//std::vector<osg::ref_ptr<osg::Node>>::iterator vir=_test.begin();
		//while(vir!=_test.end())
		//{
		//	_test.erase(vir++);
		//	
		//}
	
		
		//_nodeCache.erase();
		// Remove expired entries from object cache
//		__android_log_print(ANDROID_LOG_INFO, "removeExpired", "nodecache size %d", _nodeCache.size());
		NODE_CACHE::iterator itr = _nodeCache.begin();
		while(itr != _nodeCache.end())
		{

			if (itr->second.second<=expiryTime)
			{
				_nodeCache.erase(itr++);
//				__android_log_print(ANDROID_LOG_INFO, "removeExpired", "succeed");
			}
			else
			{
				++itr;
			}
		}

	}

	void updateTimeStampOfNodesInCacheWithExternalReferences(const osg::FrameStamp& frameStamp)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_cacheMutex);
//		__android_log_print(ANDROID_LOG_INFO, "UpdateStamp", "nodecache size %d", _nodeCache.size());
		// look for objects with external references and update their time stamp.
		for(NODE_CACHE::iterator itr=_nodeCache.begin();
			itr!=_nodeCache.end();
			++itr)
		{
			// if ref count is greater the 1 the object has an external reference.
			if (itr->second.first->referenceCount()>1)
			{
				// so update it time stamp.
				itr->second.second = frameStamp.getReferenceTime();
			}
		}

	}

	void releaseThreads()
	{
		for (int i=0;i<_loadModelThreads.size();i++)
		{
			_loadModelThreads[i]->release();
		}
	}

	class LoadModelThread2:public OpenThreads::Thread,public osg::Referenced
{
public:
	bool              _done;
	OpenThreads::Block _block;
	osg::ref_ptr<osgViewer::Viewer> _viewer;
	osg::ref_ptr<ModelPager>       _modelPaper;
	
	
	double _expiryDelay;


	void block()
	{
		_block.reset();
		_block.block();

	}

	void release()
	{
		_block.release();

	}

	LoadModelThread2(ModelPager* modelPager,osgViewer::Viewer* viewer)
	{
		_modelPaper=modelPager;
		_viewer=viewer;
		_done=false;
		_expiryDelay=10;
		
	}
	~LoadModelThread2(){cancel();}


	void prepareGeometryData()
	{
		//CSQLiteOP pSQLiteGeoOP;
		//std::string geoDBName=_dbName+".geo";
		//pSQLiteGeoOP.OpenDataBase(geoDBName.c_str());

		   DataRequest dataRequest;
		   bool succeed=_modelPaper->getFirstRequest(dataRequest);
	       if(!succeed)
			 return;
		   NODE_CACHE::iterator modelItr=_modelPaper->_nodeCache.find(dataRequest.geoKey);
		   if(modelItr==_modelPaper->_nodeCache.end())
		   {

	       // osg::ref_ptr<osg::Node> node=osgDB::readNodeFile("C:/Users/Administrator/Desktop/03/maps/jcj21.obj");
			osg::ref_ptr<osg::Node> node=osgDB::readNodeFile(dataRequest.dataName);
		//	 std::cout<<"  geo  "<<std::endl;
			////osg::Node* node=NULL;
			LodModel* lodModel=(LodModel*)(node.get());
			/* std::cout<<"  tex1  "<<std::endl;*/
			prepareMaterial(lodModel,dataRequest.curLOD);
		//	 std::cout<<"  tex  "<<std::endl;

			_viewer->getFrameStamp()->getReferenceTime();

			_modelPaper->addModel(dataRequest.geoKey,node);
	

		   }



	//	pSQLiteGeoOP.CloseDataBase();
	}


	osg::ref_ptr<osg::Texture2D> createTexture(UID64 nTexId,std::string dbPath) const
	{
		//osg::Texture2D* texture;
		//std::string texDBPath=dbPath+"C:/Users/Administrator/Desktop/jinyang/jianzhu/NoDefine";
		std::stringstream ct3dTexName;
		ct3dTexName<<dbPath.c_str()<<"."<<nTexId<<".ct3d_tex";
		//ct3dTexName<<texDBPath.c_str()<<"."<<nTexId<<".ct3d_tex";
	
		osg::ref_ptr<osg::Image> image=osgDB::readImageFile(ct3dTexName.str()/*,options*/);
		//	osg::ref_ptr<osg::Image> image;
		//osg::ref_ptr<osg::Image> image=osgDB::readImageFile("C:/Users/Administrator/Desktop/jinyang/jianzhu//NoDefine.1970410736473887.ct3d_tex");
		if(image)
		{
			osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image );


			texture->setWrap(osg::Texture2D::WRAP_R, osg::Texture::REPEAT);
			texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture::REPEAT);
			texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture::REPEAT);
			return texture;
		}
		return NULL;
	}


	void prepareMaterial(LodModel* lodModel,int lod)
	{
		
		if (lodModel->isStateSetExist(lod))
		             return;

		//if (!lodModel->isMaterialExist(lod))
		//	return;
		
		std::vector<osg::ref_ptr<CMaterial> > vecMaterial;
			if(!lodModel->getMaterial(lod,vecMaterial))
				return ;

		for (int i=0;i<vecMaterial.size();i++)
		{
		
		osg::ref_ptr<osg::StateSet> stateset=new osg::StateSet;
		
		int nMatType = vecMaterial[i]->getMaterialType();
		switch(nMatType)
		{
		case 0:
			break;
		case 1:
			{
				UID64 nTexId = vecMaterial[i]->getSimpleMat().nTextureID;
				osg::ref_ptr<osg::Texture2D> texture;
				if (Invalid_ID != nTexId)
				{
					nTexId = IDConverter::MaterialLodIDToMatID(nTexId, 0);
					texture=createTexture(nTexId,lodModel->_dbName);

				}
				
				stateset->setTextureAttributeAndModes(0, texture,osg::StateAttribute::PROTECTED|osg::StateAttribute::ON );

				//stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
				//stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				osg::ref_ptr<osg::TexEnvCombine> te = new osg::TexEnvCombine; 
				te->setCombine_RGB(osg::TexEnvCombine::REPLACE);
				stateset->setTextureAttributeAndModes(0, te, osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
	

			}
			break;
		case 2:
			{
			ComMat_PARAM commat_param = vecMaterial[i]->getComPlexMat();
				if (commat_param.pComm_item != NULL)
				{
					for(int i = 0; i < commat_param.nItemNum; i++)
					{
						COMMAT_ITEM& commat_item =  commat_param.pComm_item[i];
						for(int j = 0; j < commat_item.nItemNum; j++)
						{
							TEXTURE_ITEM& texture_item = commat_item.pComm_item[j];
							UID64 nTexId = texture_item.nTextureID;
							
							osg::ref_ptr<osg::Texture2D> texture=createTexture(nTexId,lodModel->_dbName);
						if (texture==NULL)
						{
							/* texture=createTexture(nTexId);*/
							break;
						}
					      //return;
						
							
						stateset->setTextureAttributeAndModes(texture_item.nTexIndex, texture,osg::StateAttribute::PROTECTED|osg::StateAttribute::ON );
					//		stateset->setTextureAttributeAndModes(1, NULL,osg::StateAttribute::OVERRIDE|osg::StateAttribute::PROTECTED|osg::StateAttribute::OFF );
			/*				if  ( j==0&&texture->getImage()->isImageTranslucent())
							{
								OSG_INFO<<"Found transparent image"<<std::endl;
								stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
								stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
							}*/

							osg::ref_ptr<osg::TexEnvCombine> te = new osg::TexEnvCombine;    

							if(texture_item.nColorBlendMode.nOperation==2)
							te->setCombine_RGB(osg::TexEnvCombine::REPLACE);
							else if(texture_item.nColorBlendMode.nOperation==4)
							te->setCombine_RGB(osg::TexEnvCombine::MODULATE);

							te->setSource0_RGB(osg::TexEnvCombine::TEXTURE);
							te->setOperand0_RGB(osg::TexEnvCombine::SRC_COLOR);
							te->setSource1_RGB(osg::TexEnvCombine::PREVIOUS);
							te->setOperand1_RGB(osg::TexEnvCombine::SRC_COLOR);

							//if(texture_item.nAlphaBlendMode.nOperation==2)
								te->setCombine_Alpha(osg::TexEnvCombine::REPLACE);
		/*					else if(texture_item.nAlphaBlendMode.nOperation==4)
								te->setCombine_Alpha(osg::TexEnvCombine::MODULATE);*/
					
							te->setSource0_Alpha(osg::TexEnvCombine::TEXTURE);
							te->setOperand0_Alpha(osg::TexEnvCombine::SRC_ALPHA);
							te->setSource1_Alpha(osg::TexEnvCombine::PREVIOUS);
							te->setOperand1_Alpha(osg::TexEnvCombine::SRC_ALPHA);

							if(j!=0 && texture_item.nAlphaBlendMode.nOperation==3)
							{
								te->setConstantColor(osg::Vec4(0,0,0,0));
								te->setCombine_Alpha(osg::TexEnvCombine::ADD);
								te->setSource0_Alpha(osg::TexEnvCombine::CONSTANT);
								te->setOperand0_Alpha(osg::TexEnvCombine::SRC_ALPHA);
								te->setSource1_Alpha(osg::TexEnvCombine::PREVIOUS);
								te->setOperand1_Alpha(osg::TexEnvCombine::SRC_ALPHA);
							}






							stateset->setTextureAttributeAndModes(texture_item.nTexIndex, te, osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
							//stateset->setm
							
						}
					}
				}
			}
			break;
		default:
			break;
		}

		lodModel->addStateSet(lod,stateset);
	}
	}

	virtual void run()
	{
		do 
		{
			if(_modelPaper->isRequestListEmpty())
				block();
			prepareGeometryData();
			microSleep(10000L);

		} while (!_done);
	}

	virtual int cancel()
	{
		release();
		_done=true;
		while(isRunning())
		{
			//OpenThreads::Thread::YieldCurrentThread();
			microSleep(100000L);
		}
		Thread::cancel();

		return 0;
	}




};
	std::vector< osg::ref_ptr<LoadModelThread2> > _loadModelThreads;
};


class ModelDataSet
{
public:
	ModelDataSet(std::string gmdxFile)
	{
		//parseTile();
		_topLevel=0;
		_bottomLevel=24;
		_west=-180;
		_east=180;
		_south=-90;
		_north=90;
		//std::string gmdxPath=osgDB::getFilePath(gmdxFile);
		_dataSetFileName=gmdxFile+"DataSet1.tile";
		_modelGeometrysFiles=gmdxFile+"NoDefine.geo";
		_modelTextureFiles=gmdxFile+"NoDefine.tex";
		//__android_log_print(ANDROID_LOG_INFO, "ModelDataset", "dataSetFileName: %s", _dataSetFileName.c_str());
	}

	void parseTile()
	{
		/*std::string  spath=*(this->rootPath())+*(this->sqlitePath());*/
		//std::string  spath=*(this->sqlitePath());
		sqlite3 *db;
		sqlite3_open(_dataSetFileName.c_str(),&db);
		//__android_log_print(ANDROID_LOG_INFO, "ModelDataset", "openSucceed");
		long long minlevel,maxlevel;
		sqlite3_stmt* stmt;

		std::string sqlminlev;
		sqlminlev="select itemdata from TileMeta where itemname='ValidTopLevel'";

		if (SQLITE_OK==sqlite3_prepare(db,sqlminlev.c_str(),-1,&stmt,0))
		{
			while(sqlite3_step(stmt)==SQLITE_ROW)
			{
				minlevel=sqlite3_column_int(stmt,0);
				//__android_log_print(ANDROID_LOG_INFO, "ModelDataset", "sqlitestep1 Succeed");
			}
		}
		sqlite3_finalize(stmt);

		std::string sqlmaxlev;
		sqlmaxlev="select itemdata from TileMeta where itemname='ValidBottomLevel'";

		if (SQLITE_OK==sqlite3_prepare(db,sqlmaxlev.c_str(),-1,&stmt,0))
		{
			while(sqlite3_step(stmt)==SQLITE_ROW)
			{
				maxlevel=sqlite3_column_int(stmt,0);
				//__android_log_print(ANDROID_LOG_INFO, "ModelDataset", "sqlitestep2 Succeed");
			}
		}

		sqlite3_finalize(stmt);

		_topLevel=(unsigned int)minlevel;
		_bottomLevel=(unsigned int)maxlevel;
		//__android_log_print(ANDROID_LOG_INFO, "ModelDataset", "minlevel:%d maxlevel:%d", _topLevel, _bottomLevel);

		sqlite3_close(db);
	}

	bool isInclude(int level,double east,double west,double north,double south)
	{
		if (level < _topLevel) return false;
		if (level > _bottomLevel) return false;
		if (east < _west) return false;
		if (west > _east) return false;
		if (north < _south) return false;
		if (south > _north) return false;

		return true;
		//return _y < rhs._y;
	}

	std::string _dataSetFileName;
	double _west;
	double _east;
	double _south;
	double _north;
	unsigned int _topLevel;
	unsigned int _bottomLevel;
	std::string _relationshipFile;

	std::string _featureClassesFiles;
	std::string _modelGeometrysFiles;
	std::string _modelTextureFiles;

};


class UpdateModelThread2:public OpenThreads::Thread,public osg::Referenced
{
public:

	OpenThreads::Mutex _mutex;
	bool               _done;
	osg::ref_ptr<osgViewer::Viewer> _viewer;
	const osgEarth::SpatialReference* _srs;
	osg::ref_ptr<ModelPager> _modelPager;
	TREE_MAP   _tileTree;
	FEATURE_MAP      _features;
	SORT_FEATURE_MAP _sortFeatures;
	int _numRender;
	int _numUpdate;
	osgEarth::Viewpoint _vp;
	osg::Vec3d         _focalPoint;
	osg::Vec3d         _eyePoint;

	RequestList        _oldRequestList;
	std::vector<ModelDataSet>      _modelDataSets;

	UpdateModelThread2(osgViewer::Viewer* Viewer,ModelPager* modelPager,int numRender):_done(false)
	{
		//__android_log_print(ANDROID_LOG_INFO, "UpdateModelThread", "getIN");
		_viewer=Viewer;

		//ProfileOptions profileOp;
		//profileOp.srsString()="+proj=longlat +a=6378137 +b=6378137  +no_defs";
		//profileOp.bounds()=osgEarth::Bounds(-180,-90,180,90);
		//profileOp.numTilesWideAtLod0()=2;
		//profileOp.numTilesHighAtLod0()=1;
		//osgEarth::Profile::create(profileOp);
		//_srs =osgEarth::Profile::create(profileOp)->getSRS();
		_srs=osgEarth::SpatialReference::create( "epsg:4326", "" );
		_modelPager=modelPager;
		_numRender=numRender;
		_numUpdate=2000;
	}

	bool updateViewPoint()
	{
		EarthManipulator* manip = (EarthManipulator*)_viewer->getCameraManipulator();
		osgEarth::Viewpoint vp=manip->getViewpoint();
		if(vp.getFocalPoint().z()>3000)
			return false;
	
		if(vp.getRange()==_vp.getRange()&&vp.getFocalPoint()==_vp.getFocalPoint())
		{

			return false;
		}
		_vp=vp;
		
		osg::Vec3d center=vp.getFocalPoint();
		_srs->getEllipsoid()->convertLatLongHeightToXYZ(
			osg::DegreesToRadians( center.y() ),
			osg::DegreesToRadians( center.x() ),
			center.z(),
			_focalPoint.x(), _focalPoint.y(), _focalPoint.z() );


		osg::ref_ptr<osg::Camera> cameraMaster=_viewer->getCamera();
		osg::Matrix inverseMV;
		inverseMV.invert(cameraMaster->getViewMatrix());
		_eyePoint=osg::Vec3(0,0,0)*inverseMV;

		return true;
	}

	void updateTileTree()
	{

		//EarthManipulator* manip = (EarthManipulator*)_viewer->getCameraManipulator();
		//osgEarth::Viewpoint vp=manip->getViewpoint();
		double distance=_vp.getRange();
		double maxDistance=_srs->getEllipsoid()->getRadiusEquator()*3;
		double two_level=maxDistance/distance;
		int level=log(two_level)/log(2.0f);

		//__android_log_print(ANDROID_LOG_INFO, "UpdateTileTree", "LEVEL:%d", level);
		//LEVEL_MAP levels;

		do 
		{
			double tileSize=180*pow(2.0,-level);
			int col=getColFromLongitude(_vp.getFocalPoint().x(),tileSize);
			int row=getRowFromLatitude(_vp.getFocalPoint().y(), tileSize);
			unsigned long long tileKey = (unsigned long long)(CCommonFunction::TileCoding(level,row,col));

			//TILE_TREE::iterator treeItr=_tileTree2.find(level);
			//if (treeItr==_tileTree2.end())
			//{
			//	FEATURE_MAP features=getFeaturesFromTile(level,row,col);
			//	
			//	ModelTile    modelTile;
			//	modelTile.tileKey=tileKey;
			//	modelTile.features=features;
			//	_tileTree.insert(make_pair(level,modelTile));
			//	continue;
			//}
			//else
			//{
			//	if(treeItr->second.tileKey!=tileKey)
			//	{
			//		treeItr->second.features.clear();
			//		FEATURE_MAP features=getFeaturesFromTile(level,row,col);
			//		ModelTile    modelTile;
			//		modelTile.tileKey=tileKey;
			//		modelTile.features=features;
			//		treeItr->second=modelTile;
			//	}
			//}


			TREE_MAP::iterator treeItr=_tileTree.find(level);

			if (treeItr==_tileTree.end())
			{
				//__android_log_print(ANDROID_LOG_INFO, "UpdateTileTree", "Add New");
				FEATURE_MAP features;//=getFeaturesFromTile(level,row,col);
				ModelTile    modelTile;
				modelTile.tileKey=tileKey;
				modelTile.features=features;
				_tileTree.insert(make_pair(level,modelTile));
				continue;
			}
			else
			{
				if(treeItr->second.tileKey!=tileKey)
				{
					//__android_log_print(ANDROID_LOG_INFO, "UpdateFeature", "Key crushed");
					treeItr->second.features.clear();
					FEATURE_MAP features;//=getFeaturesFromTile(level,row,col);
					ModelTile    modelTile;
					modelTile.tileKey=tileKey;
					modelTile.features=features;
					treeItr->second=modelTile;
				}
			}

			level--;
		} while (level>=0);
		//__android_log_print(ANDROID_LOG_INFO, "UpdateTileTree", "before microSleep");
		 microSleep(100000L);
		//__android_log_print(ANDROID_LOG_INFO, "UpdateTileTree", "microSleep DONE");

	}

	bool ifRequestEqual(RequestList requestList)
	{
		if(_oldRequestList.size()!=requestList.size())
			return false;

		RequestList::iterator oldItr=_oldRequestList.begin();
		RequestList::iterator newItr=requestList.begin();

		while(oldItr!=_oldRequestList.end())
		{
			if(oldItr->geoKey!=newItr->geoKey||oldItr->curLOD!=newItr->curLOD) 
				return false;

			oldItr++;
		}

		return true;
	}

	void updateFeatures()
	{
		_features.clear();
		TREE_MAP::reverse_iterator rTreeIter=_tileTree.rbegin();

		while (rTreeIter!=_tileTree.rend())
		{
			ModelTile tile=rTreeIter->second;
			
			FEATURE_MAP features=tile.features;
			FEATURE_MAP::iterator featureIter=features.begin();
			while(featureIter!=features.end())
			{
				_features.insert(make_pair(featureIter->first,featureIter->second));
				featureIter++;
			}
			if (_features.size()>_numUpdate)
			{
				break;
			}

			rTreeIter++;
		}

		//__android_log_print(ANDROID_LOG_INFO, "UpdateFeature", "NUM:%d", _features.size());
		 microSleep(100000L);
		//__android_log_print(ANDROID_LOG_INFO, "UpdateFeature", "microSleep done");
	}

	void sortFeatures()
	{
		//__android_log_print(ANDROID_LOG_INFO, "sortFeatures", "entered");
		SORT_FEATURE_MAP sortFeatures;
		
		FEATURE_MAP::iterator feaIter=_features.begin();
		while(feaIter!=_features.end())
		{
			CFeature* feature=feaIter->second;
			Location location=feature->getLocation();

			double distance=computerDistance(location.worldPosition,_focalPoint);
		
			sortFeatures.insert(make_pair(distance,feature));
			//_oldFeaturesMap.find(iter->first);
			feaIter++;
		}

		RequestList requestList;
		SORT_FEATURE_MAP::iterator sortIter=sortFeatures.begin();
		int loadNum=0;
		while(sortIter!=sortFeatures.end()&&loadNum<_numRender)
		{
			CFeature* feature=sortIter->second;
			Location location=feature->getLocation();
			double distance=computerDistance(location.worldPosition,_eyePoint);
			feature->computeCurLOD(distance);

			if (feature->_curLOD!=-1)	
			{
				for (int i=0;i<=feature->_curLOD;i++)
				{
					
					DataRequest dataRequest=createRequestByFeature(feature,i);
					requestList.push_back(dataRequest);
				}

				loadNum++;
			}

			sortIter++;
		}

		_modelPager->updateRequest(requestList);
		if (!ifRequestEqual(requestList)&&requestList.size()!=0)
		{
			_modelPager->releaseThreads();
			_oldRequestList=requestList;
		}

		//microSleep(100000L);
		//__android_log_print(ANDROID_LOG_INFO, "sortFeature", "NUM:%d", sortFeatures.size());
		 OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
		_sortFeatures.clear();
		//__android_log_print(ANDROID_LOG_INFO, "sortFeature", "clear");
		_sortFeatures=sortFeatures;
		 OpenThreads::ScopedLock<OpenThreads::Mutex> unlock(_mutex);
		//__android_log_print(ANDROID_LOG_INFO, "_sortFeature", "NUM:%d", _sortFeatures.size());
		microSleep(100000L);
	}

	void getSortFeatures(SORT_FEATURE_MAP& sortFeatures)
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
		sortFeatures=_sortFeatures;
	}

	DataRequest createRequestByFeature(CFeature *feature,int lod)
	{
		DataRequest dataRequest;
		//std::string DBPath="C:/Users/Administrator/Desktop/jinyang/jianzhu/NoDefine";
		double centerLon,centerLat,centerElevation;
		centerLon=feature->getLocation().dLongitude;
		centerLat=feature->getLocation().dLatitude;
		centerElevation=feature->getLocation().dElevation;
		
		char strLon[100],strLat[100],strElevation[100];
		sprintf(strLon,"%.14lf",centerLon);
		sprintf(strLat,"%.14lf",centerLat);
		sprintf(strElevation,"%.14lf",centerElevation);

		
		unsigned long long geoKey=feature->getLOD_INFOList()[lod].unGeometryID;
		std::stringstream ct3dGeoName;
		ct3dGeoName<<feature->_dbName.c_str()<<"."<<geoKey<<"."<<strLon<<"."<<strLat<<"."<<strElevation<<".ct3d";
		//ct3dGeoName<<DBPath.c_str()<<"."<<geoKey<<"."<<strLon<<"."<<strLat<<"."<<strElevation<<".ct3d";

		dataRequest.geoKey=geoKey;
		dataRequest.dataName=ct3dGeoName.str();
		dataRequest.location=feature->m_location;
		dataRequest.curLOD=lod;
		return dataRequest;
	}

	inline virtual osg::Vec3 getViewPoint()
	{

		osg::Vec3d ptEye;
		EarthManipulator* manip = (EarthManipulator*)_viewer->getCameraManipulator();
		osgEarth::Viewpoint vp=manip->getViewpoint();
		osg::Vec3d center=vp.getFocalPoint();

		_srs->getEllipsoid()->convertLatLongHeightToXYZ(osg::DegreesToRadians( center.y() ),osg::DegreesToRadians( center.x() ),center.z(),ptEye.x(), ptEye.y(), ptEye.z() );

		return ptEye;
	}

	inline double computerDistance(osg::Vec3 pos1,osg::Vec3 pos2)
	{
		osg::Vec3 distance=pos1-pos2;
		return distance.length();

	}

	~UpdateModelThread2(){cancel();}

	void Buffer2Features(FEATURE_MAP &vtFeatures, char *pData, int nSize,std::string dbName) const
	{

		if (! pData || nSize <= 0)
		{
			//__android_log_print(ANDROID_LOG_INFO, "UpdateTileTree", "Got sqlTile Unvalid!");
			return;
		}

		int pos = 0;
		pos += 6;

		while(pos != nSize-9)
		{
			//����
			byte curMetaType = 0;
			memcpy((char *)&curMetaType, pData + pos, sizeof(byte));
			pos += 1;
			//����
			int nMetaDataLen = 0;
			memcpy((char *)&nMetaDataLen,pData + pos,sizeof(int));
			pos += 4;
			if (pos + nMetaDataLen > nSize)
			{
				break;
			}
			//������תΪFeature
			Buffer buff(pData + pos,nMetaDataLen);
			CFeature *tempFeature = new CFeature;
			tempFeature->readBufferForService(buff);
			pos += nMetaDataLen;
			//�����ɵ�Feature�ŵ�������

			Location location=tempFeature->getLocation();
			double centerLon=location.dLongitude;
			double centerLat=location.dLatitude;
			double centerElevation=location.dElevation;
			osg::Vec3d position;
			_srs->transformToWorld(osg::Vec3d(centerLon,centerLat,centerElevation),position);
			location.worldPosition=position;
			tempFeature->setLocation(location);
			tempFeature->_dbName=dbName;
			vtFeatures.insert(make_pair(tempFeature->getFeatureID(),tempFeature));
			//delete
			//vtFeatures.push_back(tempFeature);
		}
	}

	int getRowFromLatitude(double latitude, double tileSize)
	{
		double tmp=abs(fabs(latitude-(90))/tileSize);
		return tmp/*+0.5*/;
	}

	int getColFromLongitude(double longitude, double tileSize)
	{
		double tmp=fabs(longitude-(-180))/tileSize;
		return tmp/*+0.5*/;
	}

	FEATURE_MAP getFeaturesFromTile(int level,int row,int col)
	{
		/*ModelDataSet modelDataSet;*/
		for (int i=0;i<_modelDataSets.size();i++)
		{
			double tileSize=180*pow(2.0,-(level+1));
			double minX=-180+col*tileSize;
			double maxX=minX+tileSize;
			double maxY=90-row*tileSize;
			double minY=maxY-tileSize;

			//std::string DBPath="C:/Users/Administrator/Desktop/jinyang/jianzhu/";
			//std::string DBName=DBPath+"DataSetjianhzu"+".tile";
			//std::string dbName=DBPath+"NoDefine";
			std::string DBName;
			std::string dbName;

			if(_modelDataSets[i].isInclude(level,maxX,minX,maxY,minY))
			{
				DBName=_modelDataSets[i]._dataSetFileName;
				dbName=_modelDataSets[i]._modelGeometrysFiles;
			}else
			{
				continue;
			}
				
			//int col=getColFromLongitude(_vp.getFocalPoint().x(),tileSize);
			//int row=getRowFromLatitude(_vp.getFocalPoint().y(), tileSize);

			FEATURE_MAP features;
		
			//std::string dbName="C:/Users/Administrator/Desktop/jinyang/jianzhu/NoDefine";
			//std::string DBPath="C:/Users/Administrator/Desktop/SXJC/";
			//std::string DBName=DBPath+"DataSet1.tile";
			//std::string dbName=DBPath+"NoDefine";
			CSQLiteOP* pSQLiteTileOP=new CSQLiteOP();
			pSQLiteTileOP->OpenDataBase(DBName.c_str());

			char *pTileBuff=NULL;
			int nSize;
			if(pSQLiteTileOP->GetTileDataByTileKey(level+1,row,col,&pTileBuff,nSize))
			{
				//__android_log_print(ANDROID_LOG_INFO, "UpdateTileTree", "Got sqlTileData");
				Buffer2Features(features,pTileBuff,nSize,dbName);
			}

			pSQLiteTileOP->CloseDataBase();
			delete pSQLiteTileOP;
			return features;
		}
	}
	
	virtual int cancel()
	{
		_done=true;
		while(isRunning())
			microSleep(100000L);

		Thread::cancel();
		return 0;
	}

	virtual void run()
	{
		do 
		{
			//__android_log_print(ANDROID_LOG_INFO, "updateThread", "do cycle entered");

			osg::Timer_t beforeSortTick = osg::Timer::instance()->tick();
			if(updateViewPoint())
			{
				//__android_log_print(ANDROID_LOG_INFO, "UpdateThread", "Update Entered");
				//updateTileTree();
				//updateFeatures();
				//sortFeatures();
				//__android_log_print(ANDROID_LOG_INFO, "UpdateThread", "Update Entered2");
			}

			osg::Timer_t afterSortTick = osg::Timer::instance()->tick();
			osg::Timer_t beforeUpdateTick = osg::Timer::instance()->tick();

			//_loadThread->prepareGeometryData();
			//_loadThread->PrepareTextureData();
			osg::Timer_t afterUpdateTick = osg::Timer::instance()->tick();
			 
			//std::cout<<"  sortThread:  "<<osg::Timer::instance()->delta_s(beforeSortTick, afterSortTick)<<"  updateThread:  "<<osg::Timer::instance()->delta_s(beforeUpdateTick, afterUpdateTick)<<std::endl;
	
			microSleep(500000L);

		} while (!_done);
	}
};


class UpdateModelCallback2:public osg::NodeCallback
{
public:
	//std::set<unsigned long long> _oldFeatureSet;
	osg::ref_ptr<UpdateModelThread2> _updateThread;
	osg::ref_ptr<ModelPager>         _modelPager;
	osg::ref_ptr<osgViewer::Viewer>   _viewer;

	int                 _renderNum;
	UpdateModelCallback2(UpdateModelThread2* updateThread,ModelPager* modelPager,osgViewer::Viewer* viewer,int renderNum)
	{
		_updateThread=updateThread;
		_modelPager=modelPager;
		_viewer=viewer;
		_renderNum=renderNum;
	}

	osg::ref_ptr<osg::Node> createNodeByFeature(osg::ref_ptr<CFeature> feature)
	{
		//return NULL;
		const osgEarth::SpatialReference* _srs = osgEarth::SpatialReference::create( "epsg:4326", "" );
		if (feature->_curLOD!=-1)
		{
			for (int i=feature->_curLOD;i>-1;i--)
			{
				unsigned long long geoKey=feature->getLOD_INFOList()[i].unGeometryID;
				osg::ref_ptr<osg::Node> model=_modelPager->getModel(geoKey);
				//	osg::ref_ptr<osg::Node> model=osgDB::readNodeFile("C:/Users/Administrator/Desktop/03/maps/jcj21.obj");
				if (model)
				{
					if(((LodModel*)model.get())->setStateSet(i))
					{
					Location location=feature->getLocation();
					osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
					osgEarth::GeoPoint gpoint;
					gpoint.fromWorld(_srs, location.worldPosition);
					gpoint.z() += 18.0f;
					osg::Vec3d position;
					_srs->transformToWorld(osg::Vec3d(gpoint.x(), gpoint.y(), gpoint.z()),position);
					mt->setMatrix(osg::Matrixd::translate(position));
					mt->addChild(model);
					return mt;
					}
				}
			}
		}
		return NULL;
	}

	virtual void operator()(osg::Node* node,osg::NodeVisitor* nv)
	{
		//__android_log_print(ANDROID_LOG_INFO, "UpdateCallBack", "cycle");

		_modelPager->updateTimeStampOfNodesInCacheWithExternalReferences(*(_viewer->getFrameStamp()));
		_modelPager->removeExpiredNodesInCache(*(_viewer->getFrameStamp()));

		int size=node->asGroup()->getNumChildren();
		//__android_log_print(ANDROID_LOG_INFO, "UpdateCallBack", "deleteNUM: %d", size);
		node->asGroup()->removeChildren(0,size);

		//__android_log_print(ANDROID_LOG_INFO, "ModelManager", "operater1");
		SORT_FEATURE_MAP sortFeatures;
		_updateThread->getSortFeatures(sortFeatures);
		SORT_FEATURE_MAP::iterator sortIter=sortFeatures.begin();
		int numModel=0;

		while(sortIter!=sortFeatures.end())
		{
			osg::ref_ptr<CFeature> feature=sortIter->second;
			if (feature==NULL)
				continue;
		
			osg::ref_ptr<osg::Node> model=createNodeByFeature(feature);
			
			if (model)
			{
				//__android_log_print(ANDROID_LOG_INFO, "ModelManager", "operator2");
				node->asGroup()->addChild(model);
				numModel++;
			}

			if(numModel>_renderNum)
				break;

			sortIter++;
		}
	}	
};







class ModelManager:public osg::Referenced
{
	//osg::ref_ptr<UpdateModelThread> updateThread;
	osg::ref_ptr<UpdateModelThread2> _updateThread;
	//osg::ref_ptr<LoadModelThread>   loadThread;
	//osg::ref_ptr<UpdateModelCallback> updateCallback;
	osg::ref_ptr<UpdateModelCallback2> _updateCallback;
	osg::ref_ptr<ModelPager>    _modelPager;
	osg::ref_ptr<osg::Group>       _modelGroup;
	std::vector<ModelDataSet>      _modelDataSets;
	//int                            _renderNum;
	//NODE_MAP         modelRes;
public:
	ModelManager(osgViewer::Viewer* viewer, osg::Group* modelGroup, int renderNum, std::vector<std::string> gmdxFiles)
	{
		//__android_log_print(ANDROID_LOG_INFO, "ModelManager", "getIN");
		_modelGroup=modelGroup;
		_modelPager=new ModelPager(viewer);
		_updateThread=new UpdateModelThread2(viewer,_modelPager,renderNum);

		for (int i=0;i<gmdxFiles.size();i++)
		{
			ModelDataSet  modelDataSet=ModelDataSet(gmdxFiles[i]);
			_updateThread->_modelDataSets.push_back(modelDataSet);
		}

		//__android_log_print(ANDROID_LOG_INFO, "updateThread", "datasetNum: %d", _updateThread->_modelDataSets.size());
		//_updateCallback=new UpdateModelCallback2(_updateThread,_modelPager,viewer,renderNum);
		//_modelGroup->addUpdateCallback(_updateCallback);
		//__android_log_print(ANDROID_LOG_INFO, "ModelManager", "childNUM: %d", _modelGroup->getNumChildren());
		_updateThread->setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_MIN);
		_updateThread->start();
		_modelPager->setUpThreads(2);

		//const osgEarth::SpatialReference* _srs = osgEarth::SpatialReference::create( "epsg:4326", "" );
		//osg::Vec3d position;
		//_srs->transformToWorld(osg::Vec3d(112.43670,37.70727,336.44998),position);
		//std::string modelPath = "/storage/emulated/0/osgEarth/DATA/SXJC/NoDefine.geo.1973735141671031.112.43670672066136.37.70727852584686.805.62605000000008.ct3d";
		//osg::Node* node=osgDB::readNodeFile(modelPath);
		//LodModel* lodModel=(LodModel*)(node);
		//{
		//if(!lodModel)
		//	__android_log_print(ANDROID_LOG_INFO, "LoadModel", "is NULL");
		//}
		//
		//prepareMaterial(lodModel,0);
		//lodModel->setStateSet(0);
		//lodModel->getOrCreateStateSet()->setAttributeAndModes(new osg::Program,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
		//osg::MatrixTransform* mt = new osg::MatrixTransform;
		//mt->setMatrix(osg::Matrixd::translate(position));
		//std::string path = "/storage/emulated/0/osgEarth/DATA/Model/jcj01.obj";
		//osg::ref_ptr<osg::Node> node1 = osgDB::readNodeFile(path);
		//osg::ref_ptr<osg::Node> node1 = createQuad();
		//__android_log_print(ANDROID_LOG_INFO, "ModelManager", "Main1");
		//if(!node1)
		//{
		//	__android_log_print(ANDROID_LOG_INFO, "ModelManager", "NULL1");
		//}
		//mt->addChild(lodModel);
		//modelGroup->addChild(mt);

	}

	~ModelManager()
	{
		//updateThread->cancel();
		//loadThread->cancel();
		_updateThread->cancel();
		//loadThread->cancel();
		//loadThread=new LoadModelThread();
	}

	//osg::Texture2D* createTexture(UID64 nTexId,std::string dbPath) const
	//{
	//	//osg::Texture2D* texture;
	//	//std::string texDBPath=dbPath+"C:/Users/Administrator/Desktop/jinyang/jianzhu/NoDefine";
	//	std::stringstream ct3dTexName;
	//	ct3dTexName<<dbPath.c_str()<<"."<<nTexId<<".ct3d_tex";
	//	const std::string& str1 = ct3dTexName.str();
	//	__android_log_print(ANDROID_LOG_INFO, "texDBPath", "is:%s", str1.c_str());
	//	//ct3dTexName<<texDBPath.c_str()<<"."<<nTexId<<".ct3d_tex";
	//	//std::string temptexpath = "/storage/emulated/0/osgEarth/DATA/Model/11.dds";
	//	osg::Image* image=osgDB::readImageFile(ct3dTexName.str()/*,options*/);
	//	//	osg::ref_ptr<osg::Image> image;
	//	//osg::ref_ptr<osg::Image> image=osgDB::readImageFile("C:/Users/Administrator/Desktop/jinyang/jianzhu//NoDefine.1970410736473887.ct3d_tex");
	//	if(image)
	//	{
	//		__android_log_print(ANDROID_LOG_INFO, "readImageFile", "in");
	//		osg::Texture2D* texture = new osg::Texture2D(image );
	//
	//
	//		texture->setWrap(osg::Texture2D::WRAP_R, osg::Texture::REPEAT);
	//		texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture::REPEAT);
	//		texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture::REPEAT);
	//		__android_log_print(ANDROID_LOG_INFO, "readImageFile", "out");
	//		if(!texture)
	//			__android_log_print(ANDROID_LOG_INFO, "texture", "is NULL");
	//		return texture;
	//	}
	//	else
	//	{
	//		__android_log_print(ANDROID_LOG_INFO, "readImageFile", "return NULL");
	//	}
	//	return NULL;
	//	}
	//
	//		void prepareMaterial(LodModel* lodModel,int lod)
	//		{
	//
	//			if (lodModel->isStateSetExist(lod))
	//			             return;
	//
	//			//if (!lodModel->isMaterialExist(lod))
	//			//	return;
	//
	//			std::vector<osg::ref_ptr<CMaterial> > vecMaterial;
	//				if(!lodModel->getMaterial(lod,vecMaterial))
	//					return ;
	//
	//			for (int i=0;i<vecMaterial.size();i++)
	//			{
	//
	//			osg::StateSet* stateset=new osg::StateSet;
	//
	//			int nMatType = vecMaterial[i]->getMaterialType();
	//			switch(nMatType)
	//			{
	//			case 0:
	//				break;
	//			case 1:
	//				{
	//					UID64 nTexId = vecMaterial[i]->getSimpleMat().nTextureID;
	//					osg::Texture2D* texture;
	//					if (Invalid_ID != nTexId)
	//					{
	//						nTexId = IDConverter::MaterialLodIDToMatID(nTexId, 0);
	//						texture=createTexture(nTexId,lodModel->_dbName);
	//						__android_log_print(ANDROID_LOG_INFO, "LoadModelDBName", "is: %s", (lodModel->_dbName).c_str());
	//					}
	//
	//					stateset->setTextureAttributeAndModes(0, texture,osg::StateAttribute::PROTECTED|osg::StateAttribute::ON );
	//
	//					//stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	//					//stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	////					osg::ref_ptr<osg::TexEnvCombine> te = new osg::TexEnvCombine;
	////					te->setCombine_RGB(osg::TexEnvCombine::REPLACE);
	////					stateset->setTextureAttributeAndModes(0, te, osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
	//
	//
	//				}
	//				break;
	//			case 2:
	//				{
	//				ComMat_PARAM commat_param = vecMaterial[i]->getComPlexMat();
	//					if (commat_param.pComm_item != NULL)
	//					{
	//						for(int i = 0; i < commat_param.nItemNum; i++)
	//						{
	//							COMMAT_ITEM& commat_item =  commat_param.pComm_item[i];
	//							for(int j = 0; j < commat_item.nItemNum; j++)
	//							{
	//								TEXTURE_ITEM& texture_item = commat_item.pComm_item[j];
	//								UID64 nTexId = texture_item.nTextureID;
	//
	//								osg::Texture2D* texture=createTexture(nTexId,lodModel->_dbName);
	//								__android_log_print(ANDROID_LOG_INFO, "LoadModelDBName2", "is: %s", (lodModel->_dbName).c_str());
	//							if (texture==NULL)
	//							{
	//								/* texture=createTexture(nTexId);*/
	//								break;
	//							}
	//						      //return;
	//
	//
	//							stateset->setTextureAttributeAndModes(texture_item.nTexIndex, texture,osg::StateAttribute::PROTECTED|osg::StateAttribute::ON );
	//						//		stateset->setTextureAttributeAndModes(1, NULL,osg::StateAttribute::OVERRIDE|osg::StateAttribute::PROTECTED|osg::StateAttribute::OFF );
	//				/*				if  ( j==0&&texture->getImage()->isImageTranslucent())
	//								{
	//									OSG_INFO<<"Found transparent image"<<std::endl;
	//									stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	//									stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	//								}*/
	//
	//								osg::TexEnvCombine* te = new osg::TexEnvCombine;
	//
	//								if(texture_item.nColorBlendMode.nOperation==2)
	//								te->setCombine_RGB(osg::TexEnvCombine::REPLACE);
	//								else if(texture_item.nColorBlendMode.nOperation==4)
	//								te->setCombine_RGB(osg::TexEnvCombine::MODULATE);
	//
	//								te->setSource0_RGB(osg::TexEnvCombine::TEXTURE);
	//								te->setOperand0_RGB(osg::TexEnvCombine::SRC_COLOR);
	//								te->setSource1_RGB(osg::TexEnvCombine::PREVIOUS);
	//								te->setOperand1_RGB(osg::TexEnvCombine::SRC_COLOR);
	//
	//								//if(texture_item.nAlphaBlendMode.nOperation==2)
	//									te->setCombine_Alpha(osg::TexEnvCombine::REPLACE);
	//			/*					else if(texture_item.nAlphaBlendMode.nOperation==4)
	//									te->setCombine_Alpha(osg::TexEnvCombine::MODULATE);*/
	//
	//								te->setSource0_Alpha(osg::TexEnvCombine::TEXTURE);
	//								te->setOperand0_Alpha(osg::TexEnvCombine::SRC_ALPHA);
	//								te->setSource1_Alpha(osg::TexEnvCombine::PREVIOUS);
	//								te->setOperand1_Alpha(osg::TexEnvCombine::SRC_ALPHA);
	//
	//								if(j!=0 && texture_item.nAlphaBlendMode.nOperation==3)
	//								{
	//									te->setConstantColor(osg::Vec4(0,0,0,0));
	//									te->setCombine_Alpha(osg::TexEnvCombine::ADD);
	//									te->setSource0_Alpha(osg::TexEnvCombine::CONSTANT);
	//									te->setOperand0_Alpha(osg::TexEnvCombine::SRC_ALPHA);
	//									te->setSource1_Alpha(osg::TexEnvCombine::PREVIOUS);
	//									te->setOperand1_Alpha(osg::TexEnvCombine::SRC_ALPHA);
	//								}
	//
	//
	//
	//
	//
	//
	//								stateset->setTextureAttributeAndModes(texture_item.nTexIndex, te, osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
	//								//stateset->setm
	//
	//							}
	//						}
	//					}
	//				}
	//				break;
	//			default:
	//				break;
	//			}
	//
	//			lodModel->addStateSet(lod,stateset);
	//		}
	//		}

};


#endif












