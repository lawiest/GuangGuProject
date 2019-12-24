#include "OPLayer.h"

#include<osg/MatrixTransform>
#include "model/CT3D_TexturedVertex3d.h"
#include <osgDB/FileUtils>
#include <osgEarth/XmlUtils>

//typedef long long __int64;

namespace GeoGlobe
{
	namespace OP
	{
		/*倾斜影像的解析,参照GeoGlobe*/
		OPModelInfo ModelInfo( char* pBuff,int nSize)
		{		
			int nCount = 0;
			OPModelInfo info;

			info.nModelID = *((__int64*)(pBuff));
			nCount += sizeof(__int64);

			info.nOsgbTileID = *((__int64*)(pBuff + nCount));
			nCount += sizeof(__int64);

			info.nLevel = *((int*)(pBuff + nCount));
			nCount += sizeof(int);

			info.nRow = *((int*)(pBuff + nCount));
			nCount += sizeof(int);

			info.nCol = *((int*)(pBuff + nCount));
	
			return info;
		}

		/*读取吉奥格式的数据集*/
		OPLayer::OPLayer(std::string gomdxFileName,const osgEarth::Profile* profile):TilePager( profile )
		{
			//读取吉奥自定义格式的倾斜影像数据

			_bInitialized  = false;
			_bInitialized = init(gomdxFileName);
			//__android_log_print(ANDROID_LOG_INFO, "OPLayer", "init2");
			build();
			//this->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::ON|osg::StateAttribute::PROTECTED);
		}

		/*读取osgb格式的数据集*/
		OPLayer::OPLayer(std::string osgbFileName,double lon,double lat,double elevation,const osgEarth::Profile* profile):TilePager( profile )
		{
			//读取osgb格式的倾斜影像数据
			_bInitialized  = false;
			osg::Node* op=osgDB::readNodeFile(osgbFileName);

			if(!op)
			{
				_bInitialized  = false;
				return;
			}
			else
				_bInitialized  = true;

			osg::Vec3d center=op->getBound().center();
			osg::Matrix vertialGroundMatrix;
			//mapNode->getMapSRS()->getEllipsoid()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians((21.9929027+21.9972180)/2),osg::DegreesToRadians((110.9657011+110.9703610)/2),0,vertialGroundMatrix);
			profile->getSRS()->getEllipsoid()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(lat),osg::DegreesToRadians(lon),elevation,vertialGroundMatrix);

			//对于osgb格式来说，只要设置它的旋转矩阵即可在虚拟地球上绘制
			osg::MatrixTransform* mt=new osg::MatrixTransform;
			mt->setMatrix(/*osg::Matrixd::translate(-center)**/vertialGroundMatrix);
			mt->addChild(op);
			this->addChild(mt);
			this->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::ON|osg::StateAttribute::PROTECTED);
			this->m_centerGeo = osg::Vec2d(lon, lat);
			this->m_west = lon-0.01;this->m_east = lon+0.01;
			this->m_south = lat-0.01;this->m_north = lat+0.01;
			//__android_log_print(ANDROID_LOG_INFO, "OPLayer", "NodeCenter:%f,%f,%f LAT:%f LON:%f",center.x(),center.y(),center.z(),lat,lon);
			osg::Vec3d _modelGeo;
			profile->getSRS()->transformToWorld(_modelGeo,center);
		}

		 /*数据库初始化*/
		bool OPLayer::init(std::string gomdxFileName)
		{
			osgEarth::ReadResult result = osgEarth::URI(gomdxFileName).readString();
			if (result.succeeded())
			{
				m_tileDB=new CSQLiteOP;
				m_geoDB=new  CSQLiteOP;

				std::string directory = gomdxFileName.substr(0,gomdxFileName.find_last_of('/'));
				std::string tileDBName;
				std::string geoDBName;

				std::string str = result.getString();
				std::stringstream in( str.c_str()  );

				osg::ref_ptr< osgEarth::XmlDocument > doc = osgEarth::XmlDocument::load( in );
				if (!doc.valid()) return false;
				osg::ref_ptr<osgEarth::XmlElement>  osgbmodeldataset= doc->getSubElement( "osgbmodeldataset" );

				osg::ref_ptr<osgEarth::XmlElement> datasetfile=osgbmodeldataset->getSubElement("datasetfile");
				tileDBName=datasetfile->getAttrs()["path"];
				tileDBName=directory+"/"+tileDBName.erase(0,tileDBName.find_last_of('/')+1);

				osg::ref_ptr<osgEarth::XmlElement>  geometryfile = osgbmodeldataset->getSubElement( "geometryfile" );
				geoDBName=geometryfile->getAttrs()["path"];
				geoDBName=directory+"/"+geoDBName.erase(0,geoDBName.find_last_of('/')+1);

				if(!m_tileDB->OpenDataBase(tileDBName.c_str())) return false;
				if(!m_geoDB->OpenDataBase(geoDBName.c_str())) return false;

				if(!readTileMeta(tileDBName)) return false;

				return true;
			}
			else
				return false;
		}

		/*读取数据集范围*/
		bool OPLayer::readTileMeta(std::string tileDBName)
		{
			if(!osgDB::fileExists(tileDBName))
				return false;

			sqlite3 *db;
			sqlite3_open(tileDBName.c_str(),&db);

			sqlite3_stmt* stmt;

			std::string sql;
			sql="select itemdata from TileMeta where itemname='ValidTopLevel'";

			if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
			{
				while(sqlite3_step(stmt)==SQLITE_ROW)
				{
					m_minLevel=sqlite3_column_int(stmt,0)-1;
				}
			}
			sqlite3_finalize(stmt);


			sql="select itemdata from TileMeta where itemname='ValidBottomLevel'";
			if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
			{
				while(sqlite3_step(stmt)==SQLITE_ROW)
				{
					m_maxLevel=sqlite3_column_int(stmt,0)-1;
				}
			}
			sqlite3_finalize(stmt);


			sql="select itemdata from TileMeta where itemname='West'";
			if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
			{
				while(sqlite3_step(stmt)==SQLITE_ROW)
				{
					m_west=sqlite3_column_double(stmt,0);
					//__android_log_print(ANDROID_LOG_ERROR, "PIPELAYER", "west:%f", _west);
				}
			}
			sqlite3_finalize(stmt);


			sql="select itemdata from TileMeta where itemname='East'";
			if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
			{
				while(sqlite3_step(stmt)==SQLITE_ROW)
				{
					m_east=sqlite3_column_double(stmt,0);
					//__android_log_print(ANDROID_LOG_ERROR, "PIPELAYER", "east:%f", _east);
				}
			}
			sqlite3_finalize(stmt);

			sql="select itemdata from TileMeta where itemname='South'";
			if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
			{
				while(sqlite3_step(stmt)==SQLITE_ROW)
				{
					m_south=sqlite3_column_double(stmt,0);
					//__android_log_print(ANDROID_LOG_ERROR, "PIPELAYER", "south:%f", _south);
				}
			}
			sqlite3_finalize(stmt);

			sql="select itemdata from TileMeta where itemname='North'";
			if (SQLITE_OK==sqlite3_prepare(db,sql.c_str(),-1,&stmt,0))
			{
				while(sqlite3_step(stmt)==SQLITE_ROW)
				{
					m_north=sqlite3_column_double(stmt,0);
			//		__android_log_print(ANDROID_LOG_ERROR, "PIPELAYER", "north:%f", _north);
				}
			}
			sqlite3_finalize(stmt);

			m_centerGeo = osg::Vec2d(0.5*(m_east+m_west), 0.5*(m_north+m_south));

			sqlite3_close(db);
			return true;
		}

		/*继承TilePager 根据key值生成瓦片*/
		osg::Node* OPLayer::createNode( const osgEarth::TileKey& key )
		{        

			std::cout<<key.getLOD()+1<<" "<<key.getTileX()<<" "<<key.getTileY()<<std::endl;
			osg::ref_ptr<osg::Group> tile=createOPTile(key.getLOD()+1,key.getTileX(),key.getTileY());
			if(tile.valid()&&tile->getNumChildren()>0)
				return tile.release();
			else
				return NULL;
		}

		/*构建几何数据*/
		osg::ref_ptr<osg::Geometry>  OPLayer::createOPGeometry(C3DOblique* c3dOblique)
		{
			osg::ref_ptr<osg::Geometry> mesh=new osg::Geometry;
			osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array ;
			osg::ref_ptr<osg::Vec2Array> texcoords0 = new osg::Vec2Array ;
			osg::Vec3d centerCart;
			m_profile->getSRS()->transformToWorld(c3dOblique->GetLocation(),centerCart);

			int nVertexType=c3dOblique->GetVertexType();
			switch (nVertexType)
			{
				//case VERTEXTYPE_V3f:			// for Vertex3f
				//	{
				//		break;
				//	}
				//case VERTEXTYPE_V3fN3fC3f:		// for ShadedVertex3f
				//	{
				//		break;
				//	}
				//case VERTEXTYPE_V3fT2fN3fC3f:	// for VersatileVertex3f
				//	{
				//		break;
				//	}
				//case VERTEXTYPE_V3d:				// for Vertex3d
				//	{
				//		break;
				//	}
				case VERTEXTYPE_V3dT2d:			// for TexturedVertex3d
					{
						TexturedVertex3d* pList;
			
						pList=(TexturedVertex3d*)(c3dOblique->GetVertices());
				
						for (int i=0;i<c3dOblique->GetVerticesNum();i++)
						{
							osg::Vec3d posCart;
							m_profile->getSRS()->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z+370),posCart);
							vertices->push_back(osg::Vec3d(posCart.x()-centerCart.x(),posCart.y()-centerCart.y(),posCart.z()-centerCart.z()));
						//	vertices->push_back(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z));
							texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
			
						}

						break;
					}
				//case VERTEXTYPE_V3dN3dC3f:// for ShadedVertex3d
				//	{
				//		
				//		break;
				//	}
			default:
				{
					std::cout<<"default";
				}
			}

			if(vertices->size())
				mesh->setVertexArray(vertices);
			if(texcoords0->size())
				mesh->setTexCoordArray(0,texcoords0);

			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array ;
			colors->push_back(osg::Vec4d(1.0f,1.0f,1.0f,1.0f));
			mesh->setColorArray(colors,osg::Array::BIND_OVERALL);

			long *lTriangleList;
			long lTriListSize;
			c3dOblique->GetTriangles(lTriangleList,lTriListSize);
			if (lTriListSize>0)
			{
				osg::DrawElementsUInt* triangle=new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,0);
				for(int i=0;i<lTriListSize;i++)
				{
					triangle->push_back(lTriangleList[i]);
				}
				mesh->addPrimitiveSet(triangle);
			}

			return mesh;
		}

		/*构建纹理数据*/
		osg::ref_ptr<osg::Texture2D> OPLayer::createOPTexture(C3DOblique* c3dOblique) 
		{
			void* pImageData;
			long lImagetNum;
			c3dOblique->GetImageData(pImageData,lImagetNum);
			//Buffer TextBuff;
			//TextBuff.set(pImageData,lImagetNum);

			std::string imgstr;
			imgstr.assign((char*)pImageData,lImagetNum);
			std::stringstream ss;
			ss<<imgstr;
	
			osg::ref_ptr<osgDB::ReaderWriter> rw = osgDB::Registry::instance()->getReaderWriterForExtension("jpg" );
			osg::ref_ptr<osg::Image> image = rw->readImage(ss).getImage();
			osg::ref_ptr<osg::Texture2D> texture=new osg::Texture2D;
			texture->setImage(image);
			return texture;
		}

		 /*构建瓦片数据*/
		osg::ref_ptr<osg::Group> OPLayer::createOPTile(int level,int col,int row)
		{

			osg::ref_ptr<osg::Group> opTile=new osg::Group;
			char *pTileBuff=NULL;
			int nSize;
			//tileDB->OpenDataBase("C:/Users/admin/Desktop/hhh/hhh.tile");
			if(!m_tileDB->GetTileDataByTileKey(level,col,row,&pTileBuff,nSize))
				return NULL;

			int nVersionByteSize = sizeof(unsigned char);
			unsigned char cVersion = *((unsigned char*)(pTileBuff));

			//Tile���任��־
			int nConverAllVertexFlagSize = sizeof(unsigned char);
			unsigned char cConverAllVertexFlag = *((unsigned char*)(pTileBuff + nVersionByteSize));

			//osgbģ����Ŀ
			int nOPModelNunSize = sizeof(unsigned short);
			unsigned short nOPModelNum = *((unsigned short*)(pTileBuff + nVersionByteSize + nConverAllVertexFlagSize));


			if (nOPModelNum == 0)
				return NULL;  

			//int nLevel,nRow,nCol;
			//pOPModelRenderStruct->GetLevelRowCol(&nLevel,&nRow,&nCol);

			long nSizeCount = nVersionByteSize + nConverAllVertexFlagSize + nOPModelNunSize;

			for (int i=0;i<nOPModelNum;i++)
			{
				long nOneOPSize = 28;

				OPModelInfo info = ModelInfo((pTileBuff + nSizeCount),nOneOPSize);
				int nSize;
				char *pGeoBuff=NULL;
				if(!m_geoDB->QuerySingleKeyValueTable("ModelGeometry",info.nModelID,nSize,&pGeoBuff))
				return NULL;

				long nModelDataSize = *((long*)(pGeoBuff));
				Buffer buff;
				buff.set((void*)(pGeoBuff + sizeof(long)), nModelDataSize);

				C3DOblique* c3dOblique = (C3DOblique*)CGeometry::CreateGeometry(GEOTYPE_3D_OBLIQUE);
				c3dOblique->readBuffer(buff);


				osg::ref_ptr<osg::Geometry> geometry = createOPGeometry(c3dOblique);
				osg::ref_ptr<osg::Texture2D> texture = createOPTexture(c3dOblique);
				osg::ref_ptr<osg::Geode> opModel=new osg::Geode;
				//geometry->getOrCreateStateSet()->setTextureAttribute(0,texture);
				geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture,osg::StateAttribute::PROTECTED|osg::StateAttribute::ON );
				opModel->addDrawable(geometry.get());
				//	this->addChild(opModel);
				osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
				osg::Vec3d centerCart;
				m_profile->getSRS()->transformToWorld(c3dOblique->GetLocation(),centerCart);
				mt->setMatrix(osg::Matrixd::translate(centerCart));
				mt->addChild(opModel.get());
	
				opTile->addChild(mt);
				nSizeCount += nOneOPSize;
				delete c3dOblique;
				delete []pGeoBuff;
			}

			delete []pTileBuff;
			return opTile;
		}

	}
}

