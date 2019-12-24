#ifndef GEOGLOBE_PIPE_PIPETILE
#define GEOGLOBE_PIPE_PIPETILE 1
#include <osg/LOD>
#include <Pipe/PipeOptions.h>
#include <Pipe/PipeTileParse.h>
#include <Pipe/PipeFeature.h>
#include <Pipe/PipeLineModel.h>
#include <Pipe/PipeModel.h>

namespace GeoGlobe
{
	namespace Pipe
	{

		typedef unsigned long long  ULONGLONG;
	//	typedef std::map<ULONGLONG,osg::ref_ptr<CFeature>> FEATURE_MAP;

		class PipeTile : public osg::Group
		{
		public:



			double _altitude;
		//	FEATURE_MAP _features;

			osg::ref_ptr<PipeTileOptions> _options;

			vector<CPipePointFeature> _pointFeatures;
			vector<CPipeLineFeature>  _lineFeatures;

			PipeTile()
			{

			}

			PipeTile(PipeTileOptions* options)
			{
				_altitude=0;
				_options=options;
				//_srs=_options->getMapNode()->getMapSRS();
				//getFeaturesFromTile(tileKey.getLevelOfDetail()+1,tileKey.getTileX(),tileKey.getTileY());
				int level=options->_tileKey.getLevelOfDetail()+1;
				int col=options->_tileKey.getTileX();
				int row=options->_tileKey.getTileY();


			

				if(options->_serviceUrl=="")
					createFeaturesFromLocal(level,col,row);
				else
					createFeaturesFromService(level,col,row);


				for (int i=0;i<_pointFeatures.size();i++)
				{
					
	
					_altitude+=_pointFeatures[i].GeoPoint().z();
					osg::ref_ptr<PipePointModelOptions> options=new PipePointModelOptions(&_pointFeatures[i],_options);


					//if (options->_featureID==216301362311332172)
					//{
					//	int a=0;
					//}
					std::string pipeName=_options->createPipeName(options->_featureID,0);
//					__android_log_print(ANDROID_LOG_ERROR, "pipeTile", "pipePointName: %s",pipeName.c_str());
					osg::ref_ptr<osg::Object> obj=_options->_memCache->getObjectFromCache(pipeName);
//					if(obj.valid())
//						__android_log_print(ANDROID_LOG_ERROR, "pipeTile", "pipePoint obj Got");
//					else
//						__android_log_print(ANDROID_LOG_ERROR, "pipeTile", "pipePoint obj not Got");
					osg::ref_ptr<PipeModel> pipePoint=(PipeModel*)(obj.get());
					if (!pipePoint.valid())
					{
						pipePoint=new PipeModel;
						pipePoint->setCenter(options->_modelCart);
						pipePoint->setRange(0,0,300);
						pipePoint->setRadius(1);
						std::string pipeModelName=_options->createPipeModelName(options->_modelGeometryID);
						pipePoint->setFileName(0,pipeModelName);
						pipePoint->setDatabaseOptions(options);
						_options->_memCache->addObjectToCache(pipeName,pipePoint);
					}
					/*		else
					{
					cout<<"repeat    "<<pipeName<<"     "<<_options->_tileKey.str()<<endl;
					}*/
					
					//PagedLOD*  pipePoint=new PagedLOD;
					//PipeModel*
					addChild(pipePoint);
					cout<<getBound().radius()<<endl;

				}

				for (int i=0;i<_lineFeatures.size();i++)
				{


	

					osg::ref_ptr<PipeLineModelOptions> options=new PipeLineModelOptions(&_lineFeatures[i],_options);
					_altitude+=options->_modelGeo.z();



					std::string pipeName=_options->createPipeName(options->_featureID,0);
					//std::string pipeModelName=_options->createPipeModelName(options->_modelGeometryID);
			/*		if (options->_featureID==216301362311332172)
					{
						int a=0;
					}*/
//					__android_log_print(ANDROID_LOG_ERROR, "pipeTile", "pipeLineName: %s",pipeName.c_str());
					osg::ref_ptr<osg::Object> obj=_options->_memCache->getObjectFromCache(pipeName);
//					if(obj.valid())
//						__android_log_print(ANDROID_LOG_ERROR, "pipeTile", "pipeLine obj Got");
//					else
//						__android_log_print(ANDROID_LOG_ERROR, "pipeTile", "pipeLine obj not Got");
					osg::ref_ptr<PipeModel> pipeLine=(PipeModel*)(obj.get());
					if (!pipeLine.valid())
					{
						pipeLine=new PipeModel;
						pipeLine->setCenter(options->_modelCart);
						pipeLine->setRange(0,0,1000);
					//	pipeLine->setInitialBound(options->_aabbBox);
						pipeLine->setRadius(options->_aabbBox.radius());
						std::string pipeModelName=_options->createPipeModelName(options->_modelGeometryID);
						pipeLine->setFileName(0,pipeModelName);
						pipeLine->setDatabaseOptions(options);
						_options->_memCache->addObjectToCache(pipeName,pipeLine);
					}



					

					//PipeModel* pipeLine=new PipeModel;
					////PagedLOD*  pipeline=new PagedLOD;
					//pipeLine->setCenter(options->_modelCart);
					//pipeLine->setRange(0,0,600);
					//pipeLine->setFileName(0,pipeModelName);
					//pipeLine->setDatabaseOptions(options);
					
					addChild(pipeLine);
					cout<<getBound().radius()<<endl;

					
				}
				//osgEarth::GeoPoint gp=options->_tileKey.getExtent().getBoundingGeoCircle().getCenter();
		
				
		        getBound();

				//if (_lineFeatures.size()!=0)
				//{
				//	//addChild(new PlaceNode(options->_mapNode,_options->_tileKey.getExtent().getBoundingGeoCircle().getCenter(),_options->_tileKey.str()));
				//	osg::Sphere* sphere=new osg::Sphere(getBound().center(),getBound().radius());
				//	osg::Geode* geode=new osg::Geode;
				//	geode->addDrawable(new osg::ShapeDrawable(sphere));
				//	addChild(geode);
				//}
				//	cout<<getBound().radius()<<endl;
				_altitude=_altitude/(_lineFeatures.size()+_pointFeatures.size());
				//gp.z()=_altitude;
				//PlaceNode* pn=new PlaceNode(options->_mapNode,gp,options->_tileKey.str());
				//addChild(pn);
				_lineFeatures.clear();
				_pointFeatures.clear();

			}

			bool addChild( Node *child )
			{
				if (Group::addChild(child))
				{
					this->setNumChildrenWithCullingDisabled(0);

					////if (_children.size()>_rangeList.size())
					////{
					////	float maxRange = !_rangeList.empty() ? _rangeList.back().second : 0.0f;

					////	_rangeList.resize(_children.size(),MinMaxPair(maxRange,maxRange));
					////}

					return true;
				}
				return false;
			}

//从服务器中获取管线Feature
			void createFeaturesFromService(int& level,int& col,int& row)
			{
				//_features.clear();


		/*		char *pTileBuff=NULL;
				int nSize;*/
				////if(_options->_modelCache->getTileDataFromCache(level,col,row,&pTileBuff,nSize))
				////{
				////	Buffer2Features(_features,pTileBuff,nSize);
				////}
				////	std::string modelTileFile=Stringify() <<"ModelTile/"<<level<<"/"<<col<<"/"<<row<<".modeltile";
				////modelTileFile+=_options->_cachePath+"/";
				std::string pipeTileName=_options->createPipeTileName(level,col,row);

				////std::string tileData;
				char* data;
				int   dataLen;
				if(_options->_cache->readDataFromCache(pipeTileName,dataLen,&data))
				{
					if (dataLen!=0)
					{
						//Buffer2Features(_features,const_cast<char *>(tileData.c_str()),tileData.size());
						//Buffer2Features(_features,data,dataLen);
						//Buffer2Features(const_cast<char *>(dl.data+33),dl.size-33);
						Buffer2Features(data+33,dataLen-33);
					}

					delete []data;
				}
				else
				{
					std::string tileUrl=creatPipeTileURL(level,col,row);
					Download dl;
					if(!dl.downloadData(tileUrl))
						return;
				//	//   std::string	tileData=osgEarth::URI(tileUrl.c_str()).getString();


		/*			if (dl.size!=0)
					{
						
					}*/
					//Buffer2Features(_features,const_cast<char *>(dl.data),dl.size);
						/*	CSQLiteOP tileDB;
					tileDB.OpenDataBase("C:/Users/Administrator/Desktop/js/js.tile");

					char* pTileBuff;
					int nSize;


					if(tileDB.GetTileDataByTileKey(level,col,row,&pTileBuff,nSize))*/
			/*		for(int i=0;i<100;i++)
					{
						char* a=const_cast<char *>(dl.data+28+i);
						int c=0;
					}*/
					
					if (dl.size>28)
					{
						_options->_cache->writeDataToCache(pipeTileName,dl.size,const_cast<char *>(dl.data));
						_options->_cache->commitCache("");
						Buffer2Features(const_cast<char *>(dl.data+33),dl.size-33);
					}
					

				}


			}


			std::string creatPipeTileURL(int level,int col,int row)
			{

				//ex: http://192.168.42.168:7001/PLS/pls?SERVICE=PLS&VERSION=1.0.0&REQUEST=GetTiles&LAYER=pipetest&LXYS=17,110475,17548
				//unsigned int lev,r,c;
				//lev=key.getLevelOfDetail()+1;
				//key.getTileXY(c,r);
				std::string tileUrl=_options->_serviceUrl;
				//std::string strTile = _options->_serviceUrl->full();
				/*int pos=strTile.find("/service/tile");
				int n=strTile.size()-pos+1;
				strTile.replace(pos,n,"/DataServer");*/
				std::stringstream buf;


				buf<<tileUrl<<"?SERVICE=PLS&VERSION=1.0.0&REQUEST=GetTiles&LAYER="<<_options->_layerName<<"&LXYS="<<level<<","<<col<<","<<row;


				/*buf<<"/DataServer?l="<<key_L<<"&x="<<key_Y<<"&y="<<key_X;*/
				//buf<<"/GetTile?l="<<level<<"&x="<<col<<"&y="<<row;
				/*buf<<"/DataServer?l="<<17<<"&x="<<104307<<"&y="<<23027;*/
				//tileUrl+=buf.str();
				/*strTile="http://192.168.42.178:9010/YINGXIANG/DataServer?l=17&x=104307&y=23027";*/
				return buf.str();

			}
//从本地数据库中获取管线Feature
			void createFeaturesFromLocal(int& level,int& col,int& row)
			{
				//_features.clear();

				char *pTileBuff=NULL;
				int nSize;

				/*			CSQLiteOP tileDB;
				tileDB.OpenDataBase("C:/Users/Administrator/Desktop/js/js.tile");
				tileDB.GetTileDataByTileKey(level,col,row,&pTileBuff,nSize);*/
				if(!_options->getTileDataFromDB(level,col,row,&pTileBuff,nSize))
					return;

			//	int nVersionByteSize = sizeof(unsigned short);
			//	unsigned short nVersion = *((unsigned short*)(pTileBuff));



			//	CPBPipeTile pbPipeTile;
			////	std::cout<<"parse start"<<endl;
			//	pbPipeTile.Serialize((const unsigned char*)pTileBuff+nVersionByteSize,nSize-nVersionByteSize);
			////	std::cout<<"parse end"<<endl;


			//	vector<CPipePointFeature> vPointFeature;
			//	vector<CPipeLineFeature> vLineFeature;
			//	for (int i=0;i<pbPipeTile.PipeCount();i++)
			//	{
			//		CPipePointFeature pointFeature;
			//		CPipeLineFeature lineFeaturea;
			//		if (pbPipeTile.GetPipeLineFeature(lineFeaturea,i))
			//		{
			//			_lineFeatures.push_back(lineFeaturea);
			//			continue;
			//		}
			//		if (pbPipeTile.GetPipePointFeature(pointFeature,i))
			//		{
			//			_pointFeatures.push_back(pointFeature);
			//			continue;
			//		}
			//	}
				Buffer2Features(pTileBuff,nSize);

				if (pTileBuff)
				{
					delete []pTileBuff;
				}

			}

			void Buffer2Features(char *pTileBuff,int nSize)
			{

				int nVersionByteSize = sizeof(unsigned short);
				unsigned short nVersion = *((unsigned short*)(pTileBuff));



				CPBPipeTile pbPipeTile;
					//std::cout<<"parse start"<<endl;
				pbPipeTile.Serialize((const unsigned char*)pTileBuff+nVersionByteSize,nSize-nVersionByteSize);
					//std::cout<<"parse end"<<endl;



				//vector<CPipeLineFeature> vLineFeature;
				//for (int i=0;i<pbPipeTile.TilePtr()->pipes_size()/*pbPipeTile.PipeCount()*/;i++)
				//{
				//	CPipeLineFeature lineFeature;
				//	if (!pbPipeTile.GetPipeLineFeature(lineFeature,i))
				//		continue;

				//	vLineFeature.push_back(lineFeature);
				//}


				//if(_options->_tileKey.str()=="18/424778/83906")
				//	int a=0;

				vector<CPipePointFeature> vPointFeature;
				vector<CPipeLineFeature> vLineFeature;
				for (int i=0;i<pbPipeTile.PipeCount();i++)
				{
					CPipePointFeature pointFeature;
					CPipeLineFeature lineFeaturea;
					if (pbPipeTile.GetPipeLineFeature(lineFeaturea,i))
					{
						_lineFeatures.push_back(lineFeaturea);
						continue;
					}
					if (pbPipeTile.GetPipePointFeature(pointFeature,i))
					{
						_pointFeatures.push_back(pointFeature);
						continue;
					}
					//else
					//{
					//	pbPipeTile.GetPipeLineFeature(lineFeaturea,i);
					//	pbPipeTile.GetPipePointFeature(pointFeature,i);
					//	int a=0;
					//}
				}
			}

			bool hasRealData()
			{
				if ( _children.size()==0)
				{
					return false;
				}
				return true;
			}

			void traverse(NodeVisitor& nv)
			{
				//if(_options->_tileKey.str()!="18/424778/83906")
				//	return;

				if (nv.getTraversalMode()==NodeVisitor::TRAVERSE_ALL_CHILDREN)
				{
					return;
				}
				//if(_options->_tileKey.str()=="16/106191/20976")
				//	cout<<"begin"<<endl;
				//	
				//			if (nv.getVisitorType()==osg::NodeVisitor::CULL_VISITOR)
				//{
				//cout<<_options->_tileKey.str()<<"   "<<_children.size()<<"   "<<getBound().radius()<<endl;
				//}
				
				std::for_each(_children.begin(),_children.end(),NodeAcceptOp(nv));

	
			}


		};
	}
}


#endif
