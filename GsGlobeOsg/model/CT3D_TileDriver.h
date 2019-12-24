#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <Tool/SQLiteOP.h>
#include <Model/Buffer.h>
#include <Model/Material.h>

#ifndef CT3D_TILEDRIVER
#define CT3D_TILEDRIVER 1

class CT3D_TileDriver : public osgDB::ReaderWriter
{
public:
	CT3D_TileDriver() { supportsExtension("ct3d_tile","user defined format");}

	virtual const char* className()
	{
		return "User Defined Tile Engine";
	}


	void parseFileName(std::string uri,int& level,int& row,int& col) const
	{
		std::vector<std::string> vecStr;

		std::string lessExtension;
		std::string ext;
		//vecStr->push_back(ext);
		lessExtension=osgDB::getNameLessExtension(uri);

		//std::string temp=osgDB::getFileExtension(lessExtension);
		//std::stringstream tempss;
		//tempss<<temp;
		//tempss>>texKey;
		for (int i=0;i<3;i++)
		{
			ext=osgDB::getFileExtension(lessExtension);
			vecStr.push_back(ext);
			lessExtension=osgDB::getNameLessExtension(lessExtension);
		}


		level= atoi(std::string(vecStr[2]).c_str());
		row= atoi(std::string(vecStr[1]).c_str());
		col= atoi(std::string(vecStr[0]).c_str());

		//std::stringstream tempss;
		//tempss<<vecStr[6].c_str();
		//;
		//tempss>>geoKey;



	}


	void Buffer2Features(std::vector<CFeature *> &vtFeatures, char *pData, int nSize) const
	{


		vtFeatures.clear();
		if (! pData || nSize <= 0) return;

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
			vtFeatures.push_back(tempFeature);
		}
	}



	virtual ReadResult readNode(const std::string& uri, const Options* options) const
	{
		std::string ext = osgDB::getFileExtension(uri);
		


		if ( acceptsExtension(ext) )
		{
			int level,row,col;
			parseFileName(uri,level,row,col);
			std::string DBPath=osgDB::getFilePath(uri);

			std::string DBName=osgDB::getNameLessAllExtensions(uri);
			std::string tileDBPath=DBName+".tile";
			CSQLiteOP* pSQLiteTileOP=new CSQLiteOP();
			pSQLiteTileOP->OpenDataBase(tileDBPath.c_str());

			char *pTileBuff=NULL;
			int nSize;
			pSQLiteTileOP->GetTileDataByTileKey(level,row,col,&pTileBuff,nSize);
			std::vector<CFeature*> features;
			Buffer2Features(features,pTileBuff,nSize);

			osg::ref_ptr<osg::Group> node=new osg::Group;
			osg::ref_ptr<osgEarth::SpatialReference> srs=osgEarth::SpatialReference::create( "epsg:4326", "" );
			for(int i=/*features.size()/2*/0;i</*features.size()*/10;i++)
			{
				osg::LOD*  LodModel;
				LodModel=new osg::LOD();
				//osg::PagedLOD* LodModel;
				//LodModel=new osg::PagedLOD;
				double centerLon,centerLat,centerElevation;
				char strLon[100],strLat[100],strElevation[100];
				centerLon=features[i]->getLocation().dLongitude;
				centerLat=features[i]->getLocation().dLatitude;
				centerElevation=features[i]->getLocation().dElevation;
				sprintf(strLon,"%.14lf",centerLon);
				sprintf(strLat,"%.14lf",centerLat);
				sprintf(strElevation,"%.14lf",centerElevation);

				for (int j=1;j<features[i]->getLOD_INFOList().size();j++)
				{

					if(j>1)
						break;

					unsigned long long geoKey=features[i]->getLOD_INFOList()[j].unGeometryID;
					std::stringstream ct3dGeoName;
					
					//LodModel->setRange(0,0/*feature.getLOD_INFOList()[j].dSwitchIn*/,10000000000/*feature.getLOD_INFOList()[j].dSwitchOut*/);
					ct3dGeoName<<DBPath.c_str()<<"/NoDefine"<<"."<<geoKey<<"."<<strLon<<"."<<strLat<<"."<<strElevation<<".ct3d";
					//LodModel->setFileName(0,ct3dGeoName.str());
					LodModel->addChild(osgDB::readNodeFile(ct3dGeoName.str()),0,1000);
					//node->addChild(osgDB::readNodeFile(ct3dGeoName.str()));
				}

				osg::Vec3d focalPoint;
				srs->transformToWorld(osg::Vec3d(centerLon,centerLat,centerElevation),focalPoint);
				osg::MatrixTransform* mt = new osg::MatrixTransform;
				mt->setMatrix(osg::Matrixd::translate(focalPoint));
				mt->addChild(LodModel);
				if(i==0)
					mt->addChild(osgDB::readNodeFile("C:/Users/Administrator/Desktop/data/tree.ive"));

				node->addChild(mt);


			}

			

			pSQLiteTileOP->CloseDataBase();

			if (node.valid())
			{
				return ReadResult( node.get(), ReadResult::FILE_LOADED );
			}
			else
			{
				return ReadResult::FILE_NOT_FOUND;
			}
		}
		else
		{
			return ReadResult::FILE_NOT_HANDLED;
		}
	}
};

REGISTER_OSGPLUGIN(ct3d_tile, CT3D_TileDriver)

#endif // OSGEARTH_ENGINE_MP_OPTIONS
