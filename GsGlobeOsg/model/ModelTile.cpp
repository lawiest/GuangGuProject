#include <Model/ModelTile.h>
#include <osg/PagedLOD>
#include <osg/CullStack>
#include <osg/Notify>
#include <Tool/SQLiteOP.h>
#include <algorithm>
#include <Model/LodModel.h>
#include <osgEarth/GeoData>
using namespace GeoGlobe::Model;


ModelTile::ModelTile( ModelTileOptions* options):osg::Group()
{
	_elevation=0;
	_options=options;

	int level=options->_tileKey.getLevelOfDetail()+1;
	int col=options->_tileKey.getTileX();
	int row=options->_tileKey.getTileY();

	_level = level;
	_row = row;
	_col = col;

	//从本地或服务器中读取瓦片数据
	if(options->_serviceUrl=="")
		createFeaturesFromLocal(level,col,row);
	else
		createFeaturesFromService(level,col,row);

	FEATURE_MAP::iterator itr=_features.begin();
	//根据解析瓦片数据得到的Feature生成相应的Lod模型
	while(itr!=_features.end())
	{
		_elevation+=itr->second->getLocation().dElevation;
		//std::string featureName=createLodModelName(options->_dataName,itr->first);
		std::string lodModelName=options->createLodModelName(itr->first);/*createLodModelName(options->_dataName,itr->first);*/
		osg::ref_ptr<LodModel> lodModel;
		//从缓存中读取LOD模型
		osg::ref_ptr<osg::Object> obj=GeoGlobe::Tool::GlobalSetting::instance()->_memCache->getLodMdelFromCache(lodModelName);
		lodModel=(LodModel*)( obj.get());
		//如果没有则生成LOD模型
		if (!lodModel.valid())
		{
			osg::ref_ptr<LodModelOptions> lodModelOptions=new LodModelOptions(_options,itr->second.get());

			lodModel=new LodModel(/*_options*/lodModelOptions);

			GeoGlobe::Tool::GlobalSetting::instance()->_memCache->addLodMdelToCache(lodModelName,lodModel);
		}

		addChild(lodModel);
		itr++;
	}

	//计算模型瓦片的平均高度
	if (_features.size()!=0)
	{
		_elevation=_elevation/_features.size();
	}
	
	_features.clear();
}

////向瓦片中添加模型
//bool ModelTile::addChild( Node *child )
//{
//	if (LOD::addChild(child))
//	{
//		this->setNumChildrenWithCullingDisabled(0);
//		return true;
//	}
//	return false;
//}

//判断模型中是否有数据
bool ModelTile::hasRealData()
{
	if ( _children.size()==0)
	{
		return false;
	}
	return true;
}

////遍历模型瓦片
void ModelTile::traverse(osg::NodeVisitor& nv)
{ 
	if (nv.getTraversalMode()==NodeVisitor::TRAVERSE_ALL_CHILDREN)
		return;

	for(NodeList::iterator itr=_children.begin();itr!=_children.end();++itr)
	{
		(*itr)->accept(nv);
	}
}

//解析Buffer将其生成为feature，直接抄的geoglobe中的代码
void ModelTile::Buffer2Features(FEATURE_MAP &vtFeatures, char *pData, int nSize) 
{
	//__android_log_print(ANDROID_LOG_INFO, "ModelTile::Buffer2Features %s", "start");
	if (! pData || nSize <= 0)
		return;

	//获取版本号
	enumModelVersion eModeType = GetModelTileDataType(pData, nSize);
	switch (eModeType)
	{
		case eModelVersionGMDL500:
		case eModelVersionGMDL501:
			//对应CFeature50类，暂时不实现
			return;
		case eModelVersionGMDL502:	
		case eModelVersionGMDL503:	//USEGMDL503
		case eModelVersionGMDL504:	//Mixed 502&503
		case eModelVersionGMDL507:	//USEGMDL503
			{
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
					osg::ref_ptr<CFeature> tempFeature = new CFeature;

					if(tempFeature->readBufferForService(buff))
					{
						pos += nMetaDataLen;
						vtFeatures.insert(make_pair(tempFeature->getFeatureID(),tempFeature));
					}
					else
					{
						vtFeatures.clear();
						return;
					}
				}
			}

		case eModelVersionKMZ:
		case eModelVersionG3D:
		default:
			return;
	}
	//__android_log_print(ANDROID_LOG_INFO, "ModelTile::Buffer2Features %s", "start");
}

#if 0
void ModelTile::createFeaturesFromService(int& level,int& col,int& row)
{
	_features.clear();

	char *pTileBuff=NULL;
	int nSize;
	//if(_options->_modelCache->getTileDataFromCache(level,col,row,&pTileBuff,nSize))
	//{
	//	Buffer2Features(_features,pTileBuff,nSize);
	//}
	//std::string modelTileFile=Stringify() <<"ModelTile/"<<level<<"/"<<col<<"/"<<row<<".modeltile";
	//modelTileFile+=_options->_cachePath+"/";
	std::string modelTileFile=_options->createModelTileName(level,col,row);
	
	//std::string tileData;
	char* data;
	int   dataLen=0;
	if(_options->_cache->readDataFromCache(modelTileFile,dataLen,&data))
	{
		if (dataLen!=0)
		{
			//Buffer2Features(_features,const_cast<char *>(tileData.c_str()),tileData.size());
			Buffer2Features(_features,data,dataLen);
		}

		delete []data;
	}
	else
	{
		std::string tileUrl=creatModelTileURL(level,col,row);
		Download dl;
		//dl.downloadData(tileUrl);
		
		//std::string	tileData=osgEarth::URI(tileUrl.c_str()).getString();

	`	//__android_log_print(ANDROID_LOG_INFO, "ModelTile::createFeaturesFromService", "dlSize=%d", dl.size);
		if (dl.downloadData(tileUrl))
		{
			_options->_cache->writeDataToCache(modelTileFile,dl.size,const_cast<char *>(dl.data));
		}
		Buffer2Features(_features,const_cast<char *>(dl.data),dl.size);
	}

	//if (tileData.size()!=0)
	//{
	//	Buffer2Features(_features,const_cast<char *>(tileData.c_str()),tileData.size());
	//}

	//if (_features.size()==0&&_options->_serviceUrl!="")
	//{

	//	std::string tileUrl=creatModelTileURL(level,col,row);
	//	cout<<tileUrl.c_str()<<"   begin  "<<endl;
	//	tileData=osgEarth::URI(tileUrl.c_str()).getString();
	//	cout<<tileUrl.c_str()<<"   end  "<<endl;
	//	//	FEATURE_MAP features;
	//	if (tileData.size()!=0)
	//	{
	//		Buffer2Features(_features,const_cast<char *>(tileData.c_str()),tileData.size());
	//	}

	//	if (_features.size()!=0)
	//	{
	//		//_options->_modelCache->writeTileDataToCache(level,col,row,tileData);
	//		_options->_modelCache->writeDataToCache(modelTileFile,tileData);
	//		//writeTileDataToCache(level,col,row,tileData);
	//	}

	//}
}
#endif

#if 1
//从服务得到数据构建feature
void ModelTile::createFeaturesFromService(int& level,int& col,int& row)
{
	_features.clear();

	char* data;
	int   dataLen=0;
	//判断缓存中是否存在瓦片数据
	if(_options->_cache->_tileCache.GetTileDataByTileKey(level,col,row,&data,dataLen))
	{
		if (dataLen!=0)
		{
			//解析Buffer
			Buffer2Features(_features,data,dataLen);
		}

		delete []data;
	}
	else
	{
		//不存在的话先下载再解析
		std::string tileUrl=creatModelTileURL(level,col,row);
		Download dl;

		if (dl.downloadData(tileUrl))
		{
			std::string modelTileFile=_options->createModelTileName(level,col,row);
			//下载后的数据写入本地缓存
			_options->_cache->writeDataToCache(modelTileFile,dl.size,const_cast<char *>(dl.data));
			_options->_cache->commitCache("");
		}

		Buffer2Features(_features,const_cast<char *>(dl.data),dl.size);
	}
}
#endif

//从本地得到数据构建feature
void ModelTile::createFeaturesFromLocal(int& level,int& col,int& row)
{
	_features.clear();
	char *pTileBuff=NULL;
	int nSize;
	//从本地数据库中取出数据库
	if(_options->getTileDataFromDB(level,col,row,&pTileBuff,nSize))
	{
		//将数据块解析为feature
		Buffer2Features(_features,pTileBuff,nSize);
	}

	if (pTileBuff)
	{
		delete []pTileBuff;
	}
}

//构造读取瓦片服务的URL
std::string ModelTile::creatModelTileURL(int level,int col,int row)
{
	std::string tileUrl=_options->_serviceUrl;
	std::stringstream buf;
	buf<<"/GetTile?l="<<level<<"&x="<<col<<"&y="<<row;
	tileUrl+=buf.str();
	/*strTile="http://192.168.42.178:9010/YINGXIANG/DataServer?l=17&x=104307&y=23027";*/
	return tileUrl;

}

GeoGlobe::enumModelVersion ModelTile::GetModelTileDataType(char *pTileBuf, int nSize)
{
	//如果瓦片数据为空，或者数据长度小于6字节,返回
	if ( NULL == pTileBuf || nSize < 6 )
		return eModelVersionUnknownType;

	char szTag[5] = {0};
	memcpy(szTag, pTileBuf, 4);
	char szVersion[3] = {0};
	memcpy(szVersion, pTileBuf+4, 2);

	//5.0 5.1
	if ( stricmp(szTag, "GMDL") == 0 )
	{
		if (stricmp(szVersion, "05") == 0)
			return eModelVersionGMDL500;

		//则认为是5.1
		if (stricmp(szVersion, "51") == 0)
			return eModelVersionGMDL501;

		//则认为是5.2
		if (stricmp(szVersion, "52") == 0)
			return eModelVersionGMDL502;

		//则认为是5.3
		if (stricmp(szVersion, "53") == 0)
			return eModelVersionGMDL503;

		//则认为是5.4
		if (stricmp(szVersion, "54") == 0)
			return eModelVersionGMDL504;

		//则认为是5.2
		if (stricmp(szVersion, "55") == 0)
			return eModelVersionGMDL505;

		//则认为是5.3
		if (stricmp(szVersion, "56") == 0)
			return eModelVersionGMDL506;

		//则认为是5.4
		if (stricmp(szVersion, "57") == 0)
			return eModelVersionGMDL507;

		return eModelVersionUnknownType;
	}

	//KMZ
	if ((pTileBuf[0] == 0x50) && (pTileBuf[1] == 0x4b) && (pTileBuf[2] == 0x03)&& (pTileBuf[3] == 0x04)&& (pTileBuf[4] == 0x14))
		return eModelVersionKMZ;

	return eModelVersionG3D;
}
