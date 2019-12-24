#ifndef GEOGLOBE_PIPE_PIPEPOINTMODEL
#define GEOGLOBE_PIPE_PIPEPOINTMODEL 1

#include <osg/MatrixTransform>
#include <Pipe/PipeOptions.h>
#include <Model/Model.h>
#include <osg/Math>


namespace GeoGlobe
{
	namespace Pipe
	{
		const UID64 Invalid_ID = 0;
		typedef std::vector<osg::ref_ptr<CMaterial> > C3D_MATERIALS;
		typedef std::map<int,C3D_MATERIALS>  LOD_C3D_MATERIALS;
/*管点模型的几何数据*/
		class Geometry :public osg::Geode
		{
		public:
/*空间参考用于旋转平移顶点*/
			const osgEarth::SpatialReference* _srs;
/*中心点经纬度坐标*/
			osg::Vec3d _centerPos;
//LOD材质
			LOD_C3D_MATERIALS _lodMaterials;
/*是否用矩阵进行旋转平移*/
			bool               _useVertialGroundMatrix;
/*模型的LOD级别*/
			int           _lod;
//管点模型属性
			PipePointModelOptions* _options;
/*是否有数据*/
			bool _hasRealData;

			bool hasRealData()
			{
				if(getDrawableList().size()>0&&_hasRealData)
					return true;
				else
					return false;
			}
//解压管点的几何数据
			bool uncompressGeoData(Buffer& geoBuf)
			{
				//return false;
				unsigned long uncompressDataLen = *((int*)geoBuf.getCurPtr());
				unsigned char* uncompressData=new unsigned char[uncompressDataLen];
				geoBuf.setPos(4);

				//int nRet = 
				if(0!=uncompress((unsigned char*)uncompressData, &uncompressDataLen, (unsigned char*)geoBuf.getCurPtr(), geoBuf.size()-4))
					return false;
				geoBuf.clear();
				geoBuf.set(uncompressData+5,uncompressDataLen-5);
				//delete uncompressData;
				return true;
			}
//从本地数据中构建几何体
			void createGeometryFromLocal()
			{
				char* compressData=NULL;
				int compressDataLen=0;
				if(_options->GetGeoDataFromDB(_options->_modelGeometryID,compressDataLen,&compressData))
				{
					unsigned long uncompressDataLen = *((int*)/*tempBuf.getCurPtr())*/compressData);
					unsigned char* uncompressData=new unsigned char[uncompressDataLen];

					if(0==uncompress((unsigned char*)uncompressData, &uncompressDataLen, (unsigned char*)compressData+4, compressDataLen-4))
					{
						Buffer geoBuf=Buffer(uncompressData+5,uncompressDataLen-5);

						C3DGeometry* c3dGeometry=createC3DGeometry(geoBuf);
						_hasRealData=createMaterials(geoBuf);

						int nPos = c3dGeometry->getObjName().find("FeaturePoint");
						if (nPos >=0 )
							_useVertialGroundMatrix = false;
						else
							_useVertialGroundMatrix = true;

						parseC3DGeometry(c3dGeometry);

						if (c3dGeometry)
							delete c3dGeometry;
					}
					delete []uncompressData;
				}

				if (compressData)
				{
					delete []compressData;
				}
			}
//从服务数据中构建几何体
			void createGeometryFromService()
			{

				char* compressData=NULL;
				int compressDataLen=0;
				std::string geoData;
				std::string geoFile=_options->createGeometryName(_options->_modelGeometryID);
				//是否已经下载到缓存中
				if(!_options->_cache->readDataFromCache(geoFile,compressDataLen,&compressData)/*_options->_modelCache->GetGeoDataFromCache(_options->_geoKey,compressDataLen,&compressData)*/)
				{
					std::string geoURL=creatGeometryURL(_options->_modelGeometryID);
					if (osgEarth::Registry::instance()->isBlacklisted(geoURL))
						return;
					Download dl;
					if (!dl.downloadData(geoURL))
					{
						//osgEarth::Registry::inblacklist("");
						osgEarth::Registry::instance()->blacklist(geoURL);
						return;
					}
					else
					{
						_options->_cache->writeDataToCache(geoFile,dl.size,const_cast<char *>(dl.data));
					}
						compressData=const_cast<char *>(dl.data+13)+6;
						compressDataLen=dl.size-6-9-13;

						unsigned long uncompressDataLen = *((int*)compressData);
						unsigned char* uncompressData=new unsigned char[uncompressDataLen];

						if(0==uncompress((unsigned char*)uncompressData, &uncompressDataLen, (unsigned char*)compressData+4, compressDataLen-4))
						{
							Buffer geoBuf=Buffer(uncompressData+5,uncompressDataLen-5);

							C3DGeometry* c3dGeometry=createC3DGeometry(geoBuf);
							_hasRealData=createMaterials(geoBuf);


							int nPos = c3dGeometry->getObjName().find("FeaturePoint");
							if (nPos >=0 )
								_useVertialGroundMatrix = false;
							else
								_useVertialGroundMatrix = true;

							parseC3DGeometry(c3dGeometry);

							if (c3dGeometry)
								delete c3dGeometry;
						}
						delete []uncompressData;
				}
				else
				{
					unsigned long uncompressDataLen = *((int*)(compressData+13+6));
					unsigned char* uncompressData=new unsigned char[uncompressDataLen];

					if(0==uncompress((unsigned char*)uncompressData, &uncompressDataLen, (unsigned char*)compressData+4+6+13, compressDataLen-4-6-9-13))
					{
						Buffer geoBuf=Buffer(uncompressData+5,uncompressDataLen-5);

						C3DGeometry* c3dGeometry=createC3DGeometry(geoBuf);
						_hasRealData=createMaterials(geoBuf);


						int nPos = c3dGeometry->getObjName().find("FeaturePoint");
						if (nPos >=0 )
							_useVertialGroundMatrix = false;
						else
							_useVertialGroundMatrix = true;

						parseC3DGeometry(c3dGeometry);

						if (c3dGeometry)
							delete c3dGeometry;
					}
					delete []uncompressData;
					delete []compressData;
				}
			}

			Geometry(PipePointModelOptions* options)
			{
				//std::string dbName;
				_options=options;
				_srs =options->_mapNode->getMapSRS();
				_centerPos=options->_modelCart;
				_lod=-1;

				//从数据库或服务器中读取管点的几何体
				if (_options->_serviceUrl=="")
				{
					createGeometryFromLocal();
				}
				else
				{
					createGeometryFromService();
				}


			}

			Geometry(){}
//生成服务URL
			std::string creatGeometryURL(UINT64 geoKey)
			{

				//http://192.168.42.147:9010/PLS/pls?SERVICE=PLS&VERSION=1.0.0&REQUEST=GetModelData&LAYER=pipetest&MODELID=483485810688

				std::string tileUrl=_options->_serviceUrl;
				std::stringstream buf;
				buf<<tileUrl<<"?SERVICE=PLS&VERSION=1.0.0&REQUEST=GetModelData&LAYER="<<_options->_layerName<<"&MODELID="<<geoKey;

				return buf.str();

			}
//解析GeoGlobe的C3DGeometry，详细步骤与模型部分一致
			bool parseC3DGeometry(C3DGeometry* c3dGeometry)
			{
				if(c3dGeometry==NULL)
					return false;

				byte byType=c3dGeometry->getTypeID();
				if (byType == GEOTYPE_3D_GROUP)
				{
					//osg::Group* osgGroup=new osg::Group;

					_GeometryList c3dGeometryList;
					((C3DGroup *)c3dGeometry)->GetCollection(c3dGeometryList);


					for(int i=0;i<c3dGeometryList.size();i++)
					{
						C3DGeometry * c3dGeometry=c3dGeometryList[i].second;
						/*osgGroup->addChild(*/parseC3DGeometry(c3dGeometry)/*)*/;
					}


				}
				else if(byType == GEOTYPE_3D_SOLID)
				{
					//osg::Geode* osgGeode=new osg::Geode;
					_3DSurfaceList c3dSurfaceList=((C3DSolid *)c3dGeometry)->get3DSurfaceList();

					for(int i=0;i<c3dSurfaceList.size();i++)
					{

						C3DSurface* c3dSurface=c3dSurfaceList[i].second;
						parseC3DSurface(c3dSurface);
					}
				}
				return true;
			}

//解析GeoGlobe的C3DSurface，详细步骤与模型部分一致
			void parseC3DSurface(C3DSurface* c3dSurface)
			{
				byte byType=c3dSurface->getTypeID();
				if(byType == GEOTYPE_3D_TRIANGLEMESH)	
				{
					VersatileVertex3d* pList=(VersatileVertex3d*)((C3DTriangleMesh*)c3dSurface)->m_vVertexList;
					long lVertNum=((C3DTriangleMesh*)c3dSurface)->GetVerticesNum();
					osg::Geometry* mesh=createTriangleMesh(((C3DTriangleMesh*)c3dSurface));
					addDrawable(mesh);
				} 

			}
//与Model中构建几何体一致
			osg::Geometry*  createTriangleMesh(C3DTriangleMesh* c3dTriangleMesh) const
			{
				osg::Geometry* mesh=new osg::Geometry;
				osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array ;
				osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array ;
				osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array ;
				osg::ref_ptr<osg::Vec2Array> texcoords0 = new osg::Vec2Array ;
				osg::ref_ptr<osg::Vec2Array> texcoords1 = new osg::Vec2Array ;
				osg::ref_ptr<osg::Vec2Array> texcoords2 = new osg::Vec2Array ;

				int nVertexType=c3dTriangleMesh->GetVertexType();
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
					//case VERTEXTYPE_V3dT2d:			// for TexturedVertex3d
					//	{
					//		break;
					//	}
					//case VERTEXTYPE_V3dN3dC3f:		// for ShadedVertex3d
					//	{
					//		
					//		break;
					//	}
				case VERTEXTYPE_V3dT2dN3dC3f:	// for VersatileVertex3d
					{
						VersatileVertex3d* pList=(VersatileVertex3d*)c3dTriangleMesh->m_vVertexList;
						long lVertNum=(c3dTriangleMesh)->GetVerticesNum();

						//_srs = osgEarth::SpatialReference::create( "epsg:4326", "" );
						//osg::Vec3d focalPoint;
						//_srs->transformToWorld(osg::Vec3d(location.dLongitude,location.dLatitude,location.dElevation),focalPoint);
						for (int i=0;i<lVertNum;i++)
						{
							osg::Vec3d worldCoord;
							vertices->push_back(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z));
							normals->push_back(osg::Vec3d(pList[i].nx,pList[i].ny,pList[i].nz));
							texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
							colors->push_back(osg::Vec4d(1.0f,1.0f,1.0f,1.0f));
						}
						break;

					}

				case VERTEXTYPE_V3dT2dT2dN3dC3f:	// for SecondVersatileVertex3f
					{
						SecondVersatileVertex3d* pList=(SecondVersatileVertex3d*)c3dTriangleMesh->m_vVertexList;
						long lVertNum=(c3dTriangleMesh)->GetVerticesNum();
						for (int i=0;i<lVertNum;i++)
						{
							osg::Vec3d worldCoord;
							_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
							vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
							normals->push_back(osg::Vec3d(pList[i].nx,pList[i].ny,pList[i].nz));
							colors->push_back(osg::Vec4d(1.0f,1.0f,1.0f,1.0f));
							texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
							texcoords1->push_back(osg::Vec2d(pList[i].tu2,pList[i].tv2));

						}
						break;
					}

				case VERTEXTYPE_V3dT2dT2dT2dN3dC3f:	// for ThirdVersatileVertex3f
					{
						ThirdVersatileVertex3d* pList=(ThirdVersatileVertex3d*)c3dTriangleMesh->m_vVertexList;
						long lVertNum=(c3dTriangleMesh)->GetVerticesNum();

						//_srs = osgEarth::SpatialReference::create( "epsg:4326", "" );

						for (int i=0;i<lVertNum;i++)
						{
							osg::Vec3d worldCoord;
							_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
							vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
							normals->push_back(osg::Vec3d(pList[i].nx,pList[i].ny,pList[i].nz));
							//colors->push_back(osg::Vec4d(pList[i].r,pList[i].g,pList[i].b,1.0f));
							colors->push_back(osg::Vec4d(1.0f,1.0f,1.0f,1.0f));
							texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
							texcoords1->push_back(osg::Vec2d(pList[i].tu2,pList[i].tv2));
							texcoords2->push_back(osg::Vec2d(pList[i].tu3,pList[i].tv3));
						}
						break;
					}

				default:
					{
						std::cout<<"default";
					}
				}

				if(vertices->size())
					mesh->setVertexArray(vertices);
				if(normals->size())
					mesh->setNormalArray(normals, osg::Array::BIND_PER_VERTEX);
				if(colors->size())
					mesh->setColorArray(colors,osg::Array::BIND_PER_VERTEX);
				if(texcoords0->size())
					mesh->setTexCoordArray(0,texcoords0);
				if(texcoords1->size())
					mesh->setTexCoordArray(1,texcoords1);
				if(texcoords2->size())
					mesh->setTexCoordArray(2,texcoords2);


				long *lTriangleList;
				long lTriListSize;
				c3dTriangleMesh->GetTriangles(lTriangleList,lTriListSize);
				if (lTriListSize>0)
				{
					osg::DrawElementsUInt* triangle=new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,0);
					for(int i=0;i<lTriListSize;i++)
					{
						triangle->push_back(lTriangleList[i]);
					}
					mesh->addPrimitiveSet(triangle);
				}


				long *lStripList;
				long lStripListSize;
				c3dTriangleMesh->GetStrips(lStripList,lStripListSize);
				if (lStripListSize>0)
				{
					osg::DrawElementsUInt* strip=new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP,0);
					for(int i=0;i<lStripListSize;i++)
					{
						strip->push_back(lStripList[i]);
					}
					mesh->addPrimitiveSet(strip);
				}

				return mesh;


			}
//构建C3DGeometry
			C3DGeometry* createC3DGeometry(Buffer& buff) 
			{


				byte byGeomType = *((byte*)buff.getCurPtr() + 8);
				C3DGeometry* pGeometry = (C3DGeometry*)CGeometry::CreateGeometry(byGeomType);
				if (pGeometry!=NULL&&!pGeometry->readBuffer(buff)) 
				{
					SafeDelete(pGeometry);
					return NULL;
				}

				return pGeometry;
			}
//解析材质
			bool createMaterials(Buffer& buf) 
			{
				if(NULL == buf.getCurPtr())
					return false;

				if ( (buf.getPos()+2) >= buf.size() )
					return false;

				//�����ڼ���������ʹ�ù�
				char nGeomUseCount = *((char*)buf.getCurPtr());
				buf.setPos(buf.getPos()+1);
				while (buf.getPos() < buf.size())
				{
					//��ǰlod���𼶱�
					int nLodLevel = *((char*)buf.getCurPtr());
					buf.setPos(buf.getPos()+1);

					//��ǰ������ʸ���?
					int nMatCount = *((int*)buf.getCurPtr());
					buf.setPos(buf.getPos()+4);
					for (int i=0;i<nMatCount; i++)
					{	
						//��ȡ���ݳ���
						int nSize = *((int*)buf.getCurPtr());

						//�������ݳ��� 4���ֽ�
						buf.setPos(buf.getPos()+4);

						//��ȡ���ʵ�ID
						UID64 nMatID = *((UID64*)buf.getCurPtr());

						CMaterial* pMaterial = new CMaterial();
						if (NULL == pMaterial)
							return false;

						Buffer tempBuf(buf.getCurPtr(), nSize);
						if (!pMaterial->readBuffer(tempBuf))
						{
							SafeDelete(pMaterial);
							return false;
						}

						addMaterial(nLodLevel, pMaterial);

						//������ǰ�Ĳ��ʵĳ���,׼��������һ��
						nSize += buf.getPos();
						if (nSize >= buf.size())
						{
							//OutputDebugString(_T("�������ʴ���\n"));
							buf.setPos(nSize);
							break;
						}

						buf.setPos(nSize);
					}

				}

				return true;
			}
//添加材质
			bool addMaterial(int nLod, CMaterial* pMaterial) 
			{
				LOD_C3D_MATERIALS::iterator it = _lodMaterials.find(nLod);
				if (it != _lodMaterials.end())
				{
					it->second.push_back(pMaterial);
					return true;
				}
				if (pMaterial==NULL)
				{
					int a=0;
				}

				std::vector<osg::ref_ptr<CMaterial> > vec;
				vec.push_back(pMaterial);
				_lodMaterials.insert(LOD_C3D_MATERIALS::value_type(nLod,vec));
				return true;

			}
//读取材质
			void  getMaterials(C3D_MATERIALS& materials,int lod)
			{
				LOD_C3D_MATERIALS::iterator it = _lodMaterials.find(lod);
				if (it != _lodMaterials.end())
				{
					materials=_lodMaterials.find(lod)->second;
				}

			}
//根据LOD级别设置纹理
			bool setTextures(std::vector<osg::ref_ptr<osg::StateSet> >& textures,int lod)
			{
				if (lod==_lod)
					return true;
				if(textures.size()<_drawables.size())
					return false;

				for (int i=0;i<_drawables.size();i++)
				{

					_drawables[i]->setStateSet(textures[i]);

				}

				_lod=lod;
				return true;
			}


		};


/*管点模型，与三维模型类似*/
		class PipePointModel : public  osg::MatrixTransform
		{
		public:
			//几何
			osg::ref_ptr<GeoGlobe::Pipe::Geometry>            _geometry;
			//通用属性
			osg::ref_ptr<PipePointModelOptions>                    _options;
			//纹理
			std::vector<osg::ref_ptr<osg::StateSet> > _textures;
			//是否初始化
			bool                                     _init;
			//无效纹理
			std::multimap<int,int>                         _invalidTexsList;

			PipePointModel(PipePointModelOptions* options)
			{

				_init=false;
				_options=options;
				_geometry=createGeometry(options->_modelGeometryID);

				addChild(_geometry);
				stringstream ss;
				ss<<options->_featureID;
				setName(ss.str());
			

				if(_geometry->hasRealData())
				{
					C3D_MATERIALS materials;
					_geometry->getMaterials(materials,0);
					parseC3DMaterials(materials);
					_geometry->setTextures(_textures,0);
					initModel(true);
				}

				double x,y,z;
				osg::Vec3d temp;
				if(_geometry->_useVertialGroundMatrix)
				{
					osg::Matrix vertialGroundMatrix;
					options->_mapNode->getMapSRS()->getEllipsoid()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(options->_modelGeo.y()),osg::DegreesToRadians(options->_modelGeo.x()),options->_modelGeo.z(),vertialGroundMatrix);
					setMatrix(/*_options->_featureMatrxi**//*osg::Matrix::scale(1000000000,1000000000,1000000000)**/vertialGroundMatrix );
				}
				else
					setMatrix(/*_options->_featureMatrxi**//*osg::Matrix::scale(1000000000,1000000000,1000000000)**/osg::Matrix::translate(options->_modelCart) );

				if (_options->_serviceUrl!="")
				{
					_options->_cache->commitCache("");
				}
				
				osg::Material* m = new osg::Material();
				m->setAmbient(m->FRONT_AND_BACK, osg::Vec4(0.4,0.4,0.4,1));
			    m->setDiffuse(m->FRONT_AND_BACK, osg::Vec4(1,1,1,1));
				m->setSpecular(m->FRONT_AND_BACK, osg::Vec4(1,1,1,1));
			//	m->setEmission(m->FRONT_AND_BACK, osg::Vec4(1,1,0,1));
				m->setShininess(m->FRONT_AND_BACK, 32.0);
				this->getOrCreateStateSet()->setAttributeAndModes(m, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
				this->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::ON|osg::StateAttribute::PROTECTED);
			}


//响应拾取高亮
			void select(bool select)
				{
					if (select)
					{
						osg::ref_ptr<osg::Material> m = new osg::Material();
						m->setAmbient(m->FRONT_AND_BACK, osg::Vec4d(1,1,0,1));
						m->setDiffuse(m->FRONT_AND_BACK, osg::Vec4d(1,1,0,1));
						m->setSpecular(m->FRONT_AND_BACK, osg::Vec4d(1,1,0,1));
						m->setEmission(m->FRONT_AND_BACK, osg::Vec4d(1,1,0,1));
						m->setShininess(m->FRONT_AND_BACK, 32.0);
						this->getOrCreateStateSet()->setAttributeAndModes(m, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
					}
					else
					{

						osg::ref_ptr<osg::Material> m = new osg::Material();
						m->setAmbient(m->FRONT_AND_BACK, osg::Vec4d(1,1,1,1));
						m->setDiffuse(m->FRONT_AND_BACK, osg::Vec4d(1,1,1,1));
						m->setSpecular(m->FRONT_AND_BACK, osg::Vec4d(1,1,1,1));

						m->setShininess(m->FRONT_AND_BACK, 32.0);
						this->getOrCreateStateSet()->setAttributeAndModes(m, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
					}
				}

//初始化模型
			bool initModel(bool first=false)
			{
				if (_init==true)
				{
					return true;
				}

				if (!first)
				{
					std::multimap<int,int>::iterator oitr = _invalidTexsList.begin();
					while(oitr != _invalidTexsList.end())
					{
						int i=oitr->first;
						int j=oitr->second;
						osg::Texture2D* texture = dynamic_cast<osg::Texture2D*>(_textures[i]->getTextureAttribute(j,osg::StateAttribute::TEXTURE));
						cout<<"invalidTex"<<texture->getName()<<endl;

						if (texture)
						{
							if (texture->getImage())
							{
								if (texture->getImage()->isImageTranslucent())
								{
									_textures[i]->setMode(GL_BLEND, osg::StateAttribute::ON);
									_textures[i]->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

								}
								_invalidTexsList.erase(oitr++);
							}
							else
							{
								//cout<<"find no image"<<endl;
								//return true;
								++oitr;
							}
						}
					}

				}
				else{

					for(int i=0;i<_textures.size();i++)
					{
						//osg::StateSet::TextureAttributeList tal=_textures[i]->getTextureAttributeList();
						for(unsigned int j=0;j<_textures[i]->getTextureAttributeList().size();++j)
						{
							osg::Texture2D* texture = dynamic_cast<osg::Texture2D*>(_textures[i]->getTextureAttribute(j,osg::StateAttribute::TEXTURE));
							if (texture)
							{
								if (texture->getImage())
								{
									if (texture->getImage()->isImageTranslucent())
									{
										_textures[i]->setMode(GL_BLEND, osg::StateAttribute::ON);
										_textures[i]->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
									}
								}
								else
								{
									_invalidTexsList.insert(make_pair(i,j));
								}


							}
						}
					}
				}

				if (_invalidTexsList.size()==0)
				{
					_init =true;
					return true;
				}
				else
				{
					_init=false;
					return false;
				}

			}
//从本地读取影像
			osg::Image* createImageFromLocal(UID64& nTexId)
			{

				int   dataLen=0;
				char* temp=NULL;
				if(_options->GetTexDataFromDB(nTexId,dataLen,&temp))
				{
					//texBuffer.set(texData+5,nRealDataSize-5,true);
					Buffer texBuf= Buffer(temp + 5, dataLen - 5/*,true*/);
					//delete temp;
					osg::ref_ptr<osg::Image> image = getImageFromBuffer(texBuf);
					delete []temp;
					return image.release();
				}

				if (temp)
				{
					delete []temp;
				}
				return NULL;
			}
//从二进制buffer中生成影像
			osg::Image* getImageFromBuffer(Buffer& texBuf)
			{
				//	return NULL;
				CMaterial pMaterial;
				if (!pMaterial.readBuffer(texBuf)||0!= pMaterial.getMaterialType())
					return NULL;

				std::string string;
				string.assign((char*)texBuf.getCurPtr(),texBuf.size() - texBuf.getPos() + 1);
				std::stringstream stream;
				stream<<string;
				/*		std::stringstream stream;
				stream<<texBuf.getCurPtr();*/
				std::string ext=osgDB::getFileExtension(pMaterial.getMaterialName());
				osg::ref_ptr<osgDB::ReaderWriter> rw = osgDB::Registry::instance()->getReaderWriterForExtension( ext );
				osg::ref_ptr<osgDB::Options> op=new osgDB::Options;
				op->setOptionString("dds_flip");
				osg::ref_ptr<osg::Image> image = rw->readImage(stream,op).getImage();
				return image.release();
			}
//生成纹理URL
			std::string createTextureURL(UID64 nTexId)
			{

				//http://192.168.42.147:9010/PLS/pls?SERVICE=PLS&VERSION=1.0.0&REQUEST=GetTexture&LAYER=pipetest&TEXTUREID=483485810688
	            std::string tileUrl=_options->_serviceUrl;
				std::stringstream buf;


				buf<<tileUrl<<"?SERVICE=PLS&VERSION=1.0.0&REQUEST=GetTexture&LAYER="<<_options->_layerName<<"&TEXTUREID="<<nTexId;

				return buf.str();
			}
//从服务读取影像
			osg::Image* createImageFromService(UID64& nTexId)
			{
					int   dataLen=0;
					char* temp=NULL;
					char* data;
					std::string textureFile=_options->createTextureName(nTexId);

					if(!_options->_cache->readDataFromCache(textureFile,dataLen,&data))
					{
						std::string texData;
						std::string texURL=createTextureURL(nTexId);
						if (osgEarth::Registry::instance()->isBlacklisted(texURL))
							return NULL;

						Download dl;

						if (dl.downloadData(texURL))
						{
							_options->_cache->writeDataToCache(textureFile,dl.size,const_cast<char *>(dl.data));
						}
						else
						{
							osgEarth::Registry::instance()->blacklist(texURL);
							return NULL;
						}
						Buffer texBuf= Buffer((char*)(dl.data) + 7 + 4+13, dl.size - 11 - 9-13);
						return getImageFromBuffer(texBuf);
					}
					else
					{
						Buffer texBuf= Buffer(data+ 7 + 4+13, dataLen - 11 - 9-13);
						osg::ref_ptr<osg::Image> image=getImageFromBuffer(texBuf);
						delete []data;
						if (image)
							return image.release();
						else 
							return NULL;
						
					}

				return NULL;
			}
//解析影像
			osg::Image* createImage(UID64& nTexId)
			{
				//return NULL;
				if(_options->_serviceUrl=="")
				{
					return createImageFromLocal(nTexId);
				}
				else
				{
					return createImageFromService(nTexId);
				}

			}
//构建纹理
			osg::ref_ptr<osg::Texture2D> createTexture(UID64 nTexId) 
			{
				bool  find;

				std::string texName=_options->createTextureName(nTexId);
				osg::ref_ptr<osg::Texture2D> texture=_options->_memCache->getTexture(texName,find);


				if (!find)
				{
					osg::ref_ptr<osg::Image> image;
					image=createImage(nTexId);
					if(image)
					{
						//osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image );
						texture->setImage(image);
						if (ImageUtils::isPowerOfTwo( image ) &&
							!(!image->isMipmap() && ImageUtils::isCompressed(image)) )
						{

							texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
						}
						image->setName(texture->getName());


					}
					else
					{
						image=ImageUtils::createOnePixelImage(osg::Vec4(0.827f, 0.831f, 0.835f,0.6f));
						texture->setImage(image);
						cout<<"invalid image"<<endl;
					}
				}

				return texture;

			}
//解析C3DMaterials
			bool parseC3DMaterials(C3D_MATERIALS& c3dMaterials)
			{
				//return false;
				for (int i=0;i<c3dMaterials.size();i++)
				{
					if (c3dMaterials[i]==NULL)
						return false;

					//UINT64 a=c3dMaterials[i]->getMaterialID();
					bool find=false;
					std::string matName=_options->createMaterialName(c3dMaterials[i]->getMaterialID());
					osg::ref_ptr<osg::StateSet> stateset=_options->_memCache->getStateSet(matName,find)/*new osg::StateSet*/;
					//	osg::ref_ptr<osg::StateSet> stateset=new osg::StateSet;

					if (!find)
					{
						int nMatType = c3dMaterials[i]->getMaterialType();
						switch(nMatType)
						{
						case 0:
							break;
						case 1:
							{
								UID64 nTexId = c3dMaterials[i]->getSimpleMat().nTextureID;
								osg::ref_ptr<osg::Texture2D> texture;
								if (Invalid_ID != nTexId)
								{
									nTexId = IDConverter::MaterialLodIDToMatID(nTexId, 0);
									texture=createTexture(nTexId);

								}
								if (texture==NULL)
									return false;

								stateset->setTextureAttributeAndModes(0, texture,osg::StateAttribute::PROTECTED|osg::StateAttribute::ON );



								/*	if(texture->getImage()->isImageTranslucent())
								{
								stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
								stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
								}*/

								break;
								osg::ref_ptr<osg::TexEnvCombine> te = new osg::TexEnvCombine; 
								te->setSource0_RGB(osg::TexEnvCombine::TEXTURE);
								te->setOperand0_RGB(osg::TexEnvCombine::SRC_COLOR);
								te->setSource1_RGB(osg::TexEnvCombine::PREVIOUS);
								te->setOperand1_RGB(osg::TexEnvCombine::SRC_COLOR);
								te->setCombine_RGB(osg::TexEnvCombine::REPLACE);
								te->setSource0_Alpha(osg::TexEnvCombine::TEXTURE);
								te->setOperand0_Alpha(osg::TexEnvCombine::SRC_ALPHA);
								te->setSource1_Alpha(osg::TexEnvCombine::PREVIOUS);
								te->setOperand1_Alpha(osg::TexEnvCombine::SRC_ALPHA);
								te->setCombine_Alpha(osg::TexEnvCombine::REPLACE);
								stateset->setTextureAttributeAndModes(0, te, osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);


							}
							break;
						case 2:
							{
								CT3D::ComMat_PARAM commat_param = c3dMaterials[i]->getComPlexMat();
								if (commat_param.pComm_item != NULL)
								{
									for(int i = 0; i < commat_param.nItemNum; i++)
									{
										CT3D::COMMAT_ITEM& commat_item =  commat_param.pComm_item[i];

										int texNum=_options->_setting->_multTex?commat_item.nItemNum:1;

										for(int j = 0; j < texNum; j++)
										{
											CT3D::TEXTURE_ITEM& texture_item = commat_item.pComm_item[j];
											UID64 nTexId = texture_item.nTextureID;

											osg::ref_ptr<osg::Texture2D> texture=createTexture(nTexId);


											if (texture==NULL)
												return false;

											//return;
											/*					if(texture->getImage()!=NULL&&texture->getImage()->isImageTranslucent())
											{
											stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
											stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
											}*/

											stateset->setTextureAttributeAndModes(texture_item.nTexIndex, texture,osg::StateAttribute::PROTECTED|osg::StateAttribute::ON );

											continue;
											osg::ref_ptr<osg::TexEnvCombine> te = new osg::TexEnvCombine;    

											if(texture_item.nColorBlendMode.nOperation==2)
												te->setCombine_RGB(osg::TexEnvCombine::MODULATE);
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
					}
					/*		else
					{
					cout<<"mat repeat"<<endl;
					}*/




					_textures.push_back(stateset);

				}
				//addStateSet(lod,vec);

				return true;
			}
//创建几何体
			osg::ref_ptr<Geometry> createGeometry(UINT64& geoKey)
			{
				std::string geometryName=_options->createPipeModelName(geoKey);
				osg::ref_ptr<Geometry> geometry;
				osg::ref_ptr<osg::Object> obj=( _options->_memCache->getObjectFromCache(geometryName));
				geometry=(Geometry*)(obj.get());

				if (!geometry.valid())
				{
					geometry=new Geometry( _options);
					_options->_memCache->addObjectToCache(geometryName,geometry);
					return geometry.get();
				}
				else
					return geometry.get();
			}

//节点遍历
			void traverse(NodeVisitor& nv)
			{
				if (nv.getTraversalMode() == nv.TRAVERSE_ALL_CHILDREN )
					return;
				std::multimap<std::string,bool>::iterator find=_options->_setting->_heightLightMap.find(getName());
				if (find!=_options->_setting->_heightLightMap.end())
				{
					select(find->second);
					_options->_setting->_heightLightMap.erase(find);
				}

				initModel();
				if(_geometry!=NULL&&_geometry->hasRealData())	
				{

					_geometry->accept(nv);
				}

			}

		};
	}

}


#endif
