#ifndef _GEOMETRY_WRAP_H_
#define _GEOMETRY_WRAP_H_
#include <Model//ModelDef.h>
#include <osg/Node>
#include <osg/Geometry>
#include <Model/Buffer.h>

;

class CTriangleMeshInfo: public osg::Referenced
{
public:
	//	CTriangleMeshInfo(){}
	osg::ref_ptr<osg::Geometry> _drawable;
	////	std::map<int,osg::ref_ptr<CMaterial>> _lodMaterial;
	//	osg::ref_ptr<CMaterial> _material;
	//	UID64				 m_uMtlID;					//����ID
	//	//CMaterialWrap*		 m_pMaterial;				//����
	//	UID64				 m_uTexID;					//����ID
	//	//CTextureRes*		 m_pTextureRes;
	//	//enumTransparentState m_eTransparentState;
	//	float				 m_fMeshAlpha;				//���͸����

};

typedef std::map<int,std::vector<osg::ref_ptr<CMaterial> > > MATERIAL_LOD_MAP;//Ҫ�ش���LOD��Ϣ�б�
class CGeometryWrap : public osg::Referenced
{
public:
	CGeometryWrap(Buffer geoBuffer,Location location)
	{
		_location=location;
		C3DGeometry* c3dGeometry=createGeometry(geoBuffer);
		_srs = osgEarth::SpatialReference::create( "epsg:4326", "" );
		createMaterials(geoBuffer,_materialMap);
		parseC3DGeometry(c3dGeometry);
		parseMaterials();
		//createTriangleMesh(c3dGeometry);

		delete       c3dGeometry;
	}


	C3DGeometry* createGeometry(Buffer& buff) const
	{

		//��9���ֽ�ΪType
		byte byGeomType = *((byte*)buff.getCurPtr() + 8);

		//���ɶ�Ӧ���͵�geometry
		C3DGeometry* pGeometry = (C3DGeometry*)CGeometry::CreateGeometry(byGeomType);
		if (!pGeometry->readBuffer(buff)) 
		{
			SafeDelete(pGeometry);
			return NULL;
		}

		return pGeometry;
	}
	CMaterial* createMaterials(Buffer& buf,MATERIAL_LOD_MAP& mapMaterial) const
	{
		if(NULL == buf.getCurPtr())
			return NULL;

		if ( (buf.getPos()+2) >= buf.size() )
			return NULL;

		//�����ڼ���������ʹ�ù�
		char nGeomUseCount = *((char*)buf.getCurPtr());
		buf.setPos(buf.getPos()+1);
		while (buf.getPos() < buf.size())
		{
			//��ǰlod���𼶱�
			int nLodLevel = *((char*)buf.getCurPtr());
			buf.setPos(buf.getPos()+1);

			//��ǰ������ʸ���
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
					return NULL;

				Buffer tempBuf(buf.getCurPtr(), nSize);
				if (!pMaterial->readBuffer(tempBuf))
				{
					SafeDelete(pMaterial);
					return NULL;
				}

				addMaterial(nLodLevel, pMaterial,mapMaterial);

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

		return NULL;
	}
	bool addMaterial(int nLod, CMaterial* pMaterial,MATERIAL_LOD_MAP& mapMaterial) const
	{
		//std::vector<osg::Texture2D*> textureList=
		//OSGMATERIAL osgMaterial;
		//osgMaterial.textureMap=createTexture(pMaterial->getTextureID(),pSQLiteTexOP);


		MATERIAL_LOD_MAP::iterator it = mapMaterial.find(nLod);
		if (it != mapMaterial.end())
		{
			it->second.push_back(pMaterial);
			return true;
		}

		std::vector<osg::ref_ptr<CMaterial> > vec;
		vec.push_back(pMaterial);
		mapMaterial.insert(MATERIAL_LOD_MAP::value_type(nLod,vec));
		return true;

	}


	osg::Texture2D* createTexture(UID64 nTexId,std::string DBPath) const
	{

		std::stringstream ct3dTexName;
		ct3dTexName<<DBPath<<"."<<nTexId<<".ct3d_tex";

		osg::ref_ptr<osg::Image> image=osgDB::readImageFile(ct3dTexName.str()/*,options*/);
		//	osg::ref_ptr<osg::Image> image;
		//osg::ref_ptr<osg::Image> image=osgDB::readImageFile("C:/Users/Administrator/Desktop/jinyang/jianzhu//NoDefine.1970410736473887.ct3d_tex");
		if(image)
		{
			osg::Texture2D* texture = new osg::Texture2D(image );


			texture->setWrap(osg::Texture2D::WRAP_R, osg::Texture::REPEAT);
			texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture::REPEAT);
			texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture::REPEAT);
			return texture;
		}
		return NULL;
	}


void setStateSet(CMaterial* pMaterial,osg::StateSet* stateset ) const
	{
		std::string DBPath="C:/Users/Administrator/Desktop/jinyang/jianzhu/NoDefine.tex";

		//osg::Material* m = new osg::Material();
		//m->setAmbient(m->FRONT_AND_BACK, osg::Vec4(0.827,0.831,0.835,0.15));
		//m->setDiffuse(m->FRONT_AND_BACK, osg::Vec4(0.827,0.831,0.835,0.15));
		//m->setSpecular(m->FRONT_AND_BACK, osg::Vec4(0.827,0.831,0.835,0.15));
		//m->setEmission(m->FRONT_AND_BACK, osg::Vec4(0.827,0.831,0.835,0.15));
		//m->setShininess(m->FRONT_AND_BACK, 32.0);
		//stateset->setAttributeAndModes(m, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );


		int nMatType = pMaterial->getMaterialType();
		switch(nMatType)
		{
		case 0:
			break;
		case 1:
			{
				UID64 nTexId = pMaterial->getSimpleMat().nTextureID;
				osg::Texture2D* texture;
				if (Invalid_ID != nTexId)
				{
					nTexId = IDConverter::MaterialLodIDToMatID(nTexId, 0);
					texture=createTexture(nTexId,DBPath);

				}
				
				if(texture==NULL)
					int a=0;
				if(texture!=NULL)
				stateset->setTextureAttributeAndModes(0, texture,osg::StateAttribute::ON );

				stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
				stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				osg::TexEnvCombine *te = new osg::TexEnvCombine; 
				te->setCombine_RGB(osg::TexEnvCombine::REPLACE);
				stateset->setTextureAttributeAndModes(0, te, osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
				//te->setSource0_RGB(osg::TexEnvCombine::TEXTURE);
				//te->setOperand0_RGB(osg::TexEnvCombine::SRC_COLOR);
				//te->setSource1_RGB(osg::TexEnvCombine::PREVIOUS);
				//te->setOperand1_RGB(osg::TexEnvCombine::SRC_COLOR);

				//if(texture_item.nAlphaBlendMode.nOperation==2)
				//te->setCombine_Alpha(osg::TexEnvCombine::REPLACE);
				//pMaterial->getSimpleMat().

			}
			break;
		case 2:
			{
			ComMat_PARAM commat_param = pMaterial->getComPlexMat();
				if (commat_param.pComm_item != NULL)
				{
					for(int i = 0; i < commat_param.nItemNum; i++)
					{
						COMMAT_ITEM& commat_item =  commat_param.pComm_item[i];
						for(int j = 0; j < 1/*commat_item.nItemNum*/; j++)
						{
							TEXTURE_ITEM& texture_item = commat_item.pComm_item[j];
							UID64 nTexId = texture_item.nTextureID;
							  if ((nTexId >> 56) == 3)
								  int a=1;
							osg::Texture2D* texture=createTexture(nTexId,DBPath);
							if (!texture)
								continue;
						
							
							stateset->setTextureAttributeAndModes(texture_item.nTexIndex, texture,osg::StateAttribute::PROTECTED|osg::StateAttribute::ON );
					//		stateset->setTextureAttributeAndModes(1, NULL,osg::StateAttribute::OVERRIDE|osg::StateAttribute::PROTECTED|osg::StateAttribute::OFF );
						/*	if  ( j==0&&texture->getImage()->isImageTranslucent())
							{*/
								OSG_INFO<<"Found transparent image"<<std::endl;
								stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
								stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
							/*}*/

							osg::TexEnvCombine *te = new osg::TexEnvCombine;    

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
	}

	void parseC3DGeometry(C3DGeometry * c3dGeometry) 
	{
		if(c3dGeometry==NULL)
			return ;

		byte byType=c3dGeometry->getTypeID();
		if (byType == GEOTYPE_3D_GROUP)
		{


			_GeometryList c3dGeometryList;
			((C3DGroup *)c3dGeometry)->GetCollection(c3dGeometryList);


			for(int i=0;i<c3dGeometryList.size();i++)
			{
				C3DGeometry * c3dGeometry=c3dGeometryList[i].second;
				parseC3DGeometry(c3dGeometry);
			}

			return ;
		}
		else if(byType == GEOTYPE_3D_SOLID)
		{
			osg::Geode* osgGeode=new osg::Geode;
			_3DSurfaceList c3dSurfaceList=((C3DSolid *)c3dGeometry)->get3DSurfaceList();

			for(int i=0;i<c3dSurfaceList.size();i++)
			{
				C3DSurface* c3dSurface=c3dSurfaceList[i].second;
				//osgGeode->addChild();
				parseC3DSurface(c3dSurface,i);
				//	return osgGeode;
			}

			return ;
		}


	}
	void parseC3DSurface(C3DSurface* c3dSurface,int lod)
	{
		osg::ref_ptr<CTriangleMeshInfo> triangleMeshInfo=new CTriangleMeshInfo;
		//std::vector<osg::ref_ptr<CTriangleMeshInfo2> > c;

		//std::vector<osg::ref_ptr<CMaterial> > b;
		//CMaterial* a=new CMaterial;
		//b.push_back(a);
		//_vec.push_back(triangleMeshInfo);
		byte byType=c3dSurface->getTypeID();
		if(byType == GEOTYPE_3D_TRIANGLEMESH)	
		{

			VersatileVertex3d* pList=(VersatileVertex3d*)((C3DTriangleMesh*)c3dSurface)->m_vVertexList;
			long lVertNum=((C3DTriangleMesh*)c3dSurface)->GetVerticesNum();

			osg::Geometry* osgGeometry=convertTriangleMesh(((C3DTriangleMesh*)c3dSurface));


			long *lTriangleList;
			long lTriListSize;
			((C3DTriangleMesh*)c3dSurface)->GetTriangles(lTriangleList,lTriListSize);
			if (lTriListSize>0)
			{
				osg::DrawElementsUInt* triangle=new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,0);
				for(int i=0;i<lTriListSize;i++)
				{
					triangle->push_back(lTriangleList[i]);
				}
				osgGeometry->addPrimitiveSet(triangle);
			}


			long *lStripList;
			long lStripListSize;
			((C3DTriangleMesh*)c3dSurface)->GetStrips(lStripList,lStripListSize);
			if (lStripListSize>0)
			{
				osg::DrawElementsUInt* strip=new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP,0);
				for(int i=0;i<lStripListSize;i++)
				{
					strip->push_back(lStripList[i]);
				}
				osgGeometry->addPrimitiveSet(strip);
			}



			osg::ref_ptr< osg::StateSet > stateset = osgGeometry->getOrCreateStateSet();

			if (_materialMap.size()!=0)
			{


				CMaterial *pMaterial=(_materialMap.begin())->second.at(lod);

		


				setStateSet(pMaterial,stateset);

			}


			triangleMeshInfo->_drawable=osgGeometry;


			//return ;
			//	_vecMesh.push_back(true);
			_vecMesh.push_back(triangleMeshInfo);
		} 

	}
	osg::Geometry*  convertTriangleMesh(C3DTriangleMesh* c3dTriangleMesh) const
	{
		osg::Geometry* osgGeometry=new osg::Geometry;

		osgGeometry->setUseVertexBufferObjects(true);
		osgGeometry->setUseDisplayList(false);


		osg::Vec3Array* vertices = new osg::Vec3Array ;
		osg::Vec3Array* normals = new osg::Vec3Array ;
		osg::Vec4Array* colors = new osg::Vec4Array ;
		osg::Vec2Array* texcoords0 = new osg::Vec2Array ;
		osg::Vec2Array* texcoords1 = new osg::Vec2Array ;
		osg::Vec2Array* texcoords2 = new osg::Vec2Array ;

		int nVertexType=c3dTriangleMesh->GetVertexType();
		switch (nVertexType)
		{
		case VERTEXTYPE_V3f:			// for Vertex3f
		case VERTEXTYPE_V3fN3fC3f:		// for ShadedVertex3f
		case VERTEXTYPE_V3fT2fN3fC3f:	// for VersatileVertex3f
		case VERTEXTYPE_V3d:				// for Vertex3d
		case VERTEXTYPE_V3dT2d:			// for TexturedVertex3d
		case VERTEXTYPE_V3dN3dC3f:		// for ShadedVertex3d
		case VERTEXTYPE_V3dT2dN3dC3f:	// for VersatileVertex3d
			{
				VersatileVertex3d* pList=(VersatileVertex3d*)c3dTriangleMesh->m_vVertexList;
				long lVertNum=(c3dTriangleMesh)->GetVerticesNum();

				//_srs = osgEarth::SpatialReference::create( "epsg:4326", "" );
				osg::Vec3d focalPoint;
				_srs->transformToWorld(osg::Vec3d(_location.dLongitude,_location.dLatitude,_location.dElevation),focalPoint);
				for (int i=0;i<lVertNum;i++)
				{
					osg::Vec3d worldCoord;
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-focalPoint.x(),worldCoord.y()-focalPoint.y(),worldCoord.z()-focalPoint.z()));
					normals->push_back(osg::Vec3d(pList[i].nx,pList[i].ny,pList[i].nz));
					texcoords0->push_back(osg::Vec2d(pList[i].tu,1.0-pList[i].tv));
					colors->push_back(osg::Vec4d(pList[i].r,pList[i].g,pList[i].b,1.0f));
				}
				break;

			}

		case VERTEXTYPE_V3dT2dT2dN3dC3f:	// for SecondVersatileVertex3f
			{
				SecondVersatileVertex3d* pList=(SecondVersatileVertex3d*)c3dTriangleMesh->m_vVertexList;
				long lVertNum=(c3dTriangleMesh)->GetVerticesNum();

				//_srs = osgEarth::SpatialReference::create( "epsg:4326", "" );
				osg::Vec3d focalPoint;
				_srs->transformToWorld(osg::Vec3d(_location.dLongitude,_location.dLatitude,_location.dElevation),focalPoint);
				for (int i=0;i<lVertNum;i++)
				{
					osg::Vec3d worldCoord;
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-focalPoint.x(),worldCoord.y()-focalPoint.y(),worldCoord.z()-focalPoint.z()));
					normals->push_back(osg::Vec3d(pList[i].nx,pList[i].ny,pList[i].nz));
					colors->push_back(osg::Vec4d(pList[i].r,pList[i].g,pList[i].b,1.0f));
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
				osg::Vec3d focalPoint;
				_srs->transformToWorld(osg::Vec3d(_location.dLongitude,_location.dLatitude,_location.dElevation),focalPoint);
				for (int i=0;i<lVertNum;i++)
				{
					osg::Vec3d worldCoord;
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-focalPoint.x(),worldCoord.y()-focalPoint.y(),worldCoord.z()-focalPoint.z()));
					normals->push_back(osg::Vec3d(pList[i].nx,pList[i].ny,pList[i].nz));
					//colors->push_back(osg::Vec4d(pList[i].r,pList[i].g,pList[i].b,1.0f));
					texcoords0->push_back(osg::Vec2d(pList[i].tu,1.0-pList[i].tv));
					texcoords1->push_back(osg::Vec2d(pList[i].tu2,1.0-pList[i].tv2));
					texcoords2->push_back(osg::Vec2d(pList[i].tu3,1.0-pList[i].tv3));

				}
				break;
			}
		}

		if(vertices->size())
			osgGeometry->setVertexArray(vertices);
		if(normals->size())
			osgGeometry->setNormalArray(normals, osg::Array::BIND_PER_VERTEX);
		if(colors->size())
			osgGeometry->setColorArray(colors,osg::Array::BIND_PER_VERTEX);
		if(texcoords0->size())
			osgGeometry->setTexCoordArray(0,texcoords0);
		if(texcoords1->size())
			osgGeometry->setTexCoordArray(1,texcoords1);
		if(texcoords2->size())
			osgGeometry->setTexCoordArray(2,texcoords2);

		return osgGeometry;


	}

	bool addTexture(int nLod, UID64 textID) 
	{


		std::map<int,std::vector<ULONGLONG> >::iterator it = _lodTexture.find(nLod);
		if (it != _lodTexture.end())
		{
			it->second.push_back(textID);
			return true;
		}

		std::vector<ULONGLONG> vec;
		vec.push_back(textID);
		_lodTexture.insert(std::map<int,std::vector<ULONGLONG> >::value_type(nLod,vec));
		return true;

	}

	void parseMaterials()
	{
		MATERIAL_LOD_MAP::iterator itr= _materialMap.begin();
		
		while(itr!=_materialMap.end())
		{

			std::vector<osg::ref_ptr<CMaterial> > materials=itr->second;
			for (int i=0;i<materials.size();i++)
			{

				int nMatType =materials[i] ->getMaterialType();
				switch(nMatType)
				{
				case 0:
					break;
				case 1:
					{
						UID64 nTexId = materials[i]->getSimpleMat().nTextureID;

						if (Invalid_ID != nTexId)
						{
							nTexId = IDConverter::MaterialLodIDToMatID(nTexId, 0);
							//osg::Texture* texture=NULL;
							addTexture(itr->first,nTexId);
							//_lodTexture.insert(itr->first,make_pair(nTexId,texture));
						}

					}
					break;
				case 2:
					{
						ComMat_PARAM commat_param = materials[i]->getComPlexMat();
						if (commat_param.pComm_item != NULL)
						{
							for(int i = 0; i < commat_param.nItemNum; i++)
							{
								COMMAT_ITEM& commat_item =  commat_param.pComm_item[i];
								for(int j = 0; j < 1/*commat_item.nItemNum*/; j++)
								{
									TEXTURE_ITEM& texture_item = commat_item.pComm_item[j];
									UID64 nTexId = texture_item.nTextureID;
									addTexture(itr->first,nTexId);
								//	osg::Texture* texture=NULL;
								/*	_vecTexture.insert(make_pair(nTexId,texture));*/
									//	_vecTexture.insert(make_pair(nTexId,NULL));


								}
							}
						}
					}
					break;
				default:
					break;
				}
			}
			itr++;
		}
	}


	osg::ref_ptr<osg::Geode> _geode;
	std::vector<osg::ref_ptr<CTriangleMeshInfo> > _vecMesh;
	std::map<int,std::vector<ULONGLONG> >         _lodTexture;

	MATERIAL_LOD_MAP               _materialMap;
	Location                       _location;
	osg::ref_ptr<osgEarth::SpatialReference> _srs;
};


struct LodRange
{
	double switchIn;
	double switchOut;
};

class CFeatureWrap : public osg::Referenced
{
	CFeatureWrap(CFeature* feature)
	{
		for(int i=0;i<feature->getLOD_INFOList().size();i++)
		{
			LodRange lOdRange;
			lOdRange.switchIn=feature->getLOD_INFOList()[i].dSwitchIn;
			lOdRange.switchOut=feature->getLOD_INFOList()[i].dSwitchOut;
			_lodRanges.push_back(lOdRange);
		}
	}
	std::vector<LodRange>                     _lodRanges;
	std::map<int,osg::ref_ptr<CGeometryWrap> > _lodGeometry;
	int                                       _curLOD;
	Location                                  _location;
};


#endif
