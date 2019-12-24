

#ifndef CT3D_DRIVER
#define CT3D_DRIVER 1

#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <Model/CT3D_3DGeometry.h>

#include <Model/CT3D_3DTriangleMesh.h>
#include <Model/Feature.h>
#include <Model/CT3D_3DSurface.h>
#include <Model/CT3D_3DSolid.h>
#include <Model/CT3D_3DGeometry.h>
#include <Model/CT3D_3DGroup.h>
#include <Model/CT3D_3DTriangleMesh.h>
#include <Model/Buffer.h>
#include "Model/CT3D_Vertex3d.h"
#include "Model/CT3D_Vertex3f.h"
#include "Model/CT3D_ShadedVertex3d.h"
#include "Model/CT3D_ShadedVertex3f.h"
#include "Model/CT3D_TexturedVertex3d.h"
#include "Model/CT3D_TexturedVertex3f.h"
#include "Model/CT3D_ThirdVersatileVertex3d.h"
#include "Model/CT3D_ThirdVersatileVertex3dWithoutRGB.h"
#include "Model/CT3D_ThirdVersatileVertex3fWithoutRGB.h"
#include "Model/CT3D_ThirdVersatileVertex3f.h"
#include "Model/CT3D_NormaledVertex3d.h"
#include "Model/CT3D_NormaledVertex3f.h"
#include "Model/CT3D_TextureDriver.h"
#include <osg/TexEnvCombine>
#include <Tool/IDConverter.h>
#include <Tool/SQLiteOP.h>
#include <Model/LodModel.h>

const UID64 Invalid_ID = 0;

struct OSGMATERIAL
{
	UID64  materialID;
	osg::Material* material;
	//std::vector<osg::Texture2D*> textureList;
	std::map<int,osg::Texture2D*> textureMap;
	//std::map<int,osg::Texture2D*> textureList;
};

typedef std::map<int,/*std::vector<OSGMATERIAL>*/std::vector<osg::ref_ptr<CMaterial> > > LOD_MATERIAL_MAP;//Ҫ�ش���LOD��Ϣ�б�

class CT3DDriver : public osgDB::ReaderWriter
{
public:
	const osgEarth::SpatialReference* _srs;
	CT3DDriver()
	{ 
	/*	ProfileOptions profileOp;
		profileOp.srsString()="+proj=longlat +a=6378137 +b=6378137  +no_defs";
		profileOp.bounds()=osgEarth::Bounds(-180,-90,180,90);
		profileOp.numTilesWideAtLod0()=2;
		profileOp.numTilesHighAtLod0()=1;*/
		//_srs = osgEarth::SpatialReference::create()
		//ProfileOptions profileOp;
		//profileOp.srsString()="+proj=longlat +a=6378137 +b=6378137  +no_defs";
		//profileOp.bounds()=osgEarth::Bounds(-180,-90,180,90);
		//profileOp.numTilesWideAtLod0()=2;
		//profileOp.numTilesHighAtLod0()=1;
		// osgEarth::Profile::create(profileOp);
		_srs =/*osgEarth::Profile::create(*//*profileOp)->getSRS()*/ osgEarth::SpatialReference::create( "epsg:4326", "" );
		supportsExtension("CT3D","user defined format");
	}



	virtual const char* className()
	{
		return "User Defined Mesh Engine";
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


	CMaterial* createMaterials(Buffer& buf,LOD_MATERIAL_MAP& mapMaterial) const
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
				//if (NULL == pMaterial)
				//	return NULL;

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

	bool addMaterial(int nLod, CMaterial* pMaterial,LOD_MATERIAL_MAP& mapMaterial) const
	{
		//std::vector<osg::Texture2D*> textureList=
		//OSGMATERIAL osgMaterial;
		//osgMaterial.textureMap=createTexture(pMaterial->getTextureID(),pSQLiteTexOP);


		LOD_MATERIAL_MAP::iterator it = mapMaterial.find(nLod);
		if (it != mapMaterial.end())
		{
			it->second.push_back(pMaterial);
			return true;
		}

		std::vector<osg::ref_ptr<CMaterial> > vec;
		vec.push_back(pMaterial);
		mapMaterial.insert(LOD_MATERIAL_MAP::value_type(nLod,vec));
		return true;

	}


	bool addTexture(int nLod, UID64 textID,LodModel* lodModel) const
	{
		return false;

		std::map<int,std::vector<unsigned long long> >::iterator it = lodModel->_lodTexture.find(nLod);
		if (it != lodModel->_lodTexture.end())
		{
			it->second.push_back(textID);
			return true;
		}

		std::vector<unsigned long long> vec;
		vec.push_back(textID);
		lodModel->_lodTexture.insert(std::map<int,std::vector<unsigned long long> >::value_type(nLod,vec));
		return true;

	}

//#include <osg/TexEnvCombine>
	void setStateSet(CMaterial* pMaterial,osg::StateSet* stateset,std::string DBPath,LodModel* lodModel) const
	{


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
				osg::ref_ptr<osg::Texture2D> texture;
				if (Invalid_ID != nTexId)
				{
					nTexId = IDConverter::MaterialLodIDToMatID(nTexId, 0);
					texture=createTexture(nTexId,DBPath);
					//texture->setUnRefImageDataAfterApply(false);

				}
				addTexture(0, nTexId,lodModel);
				//if(texture==NULL)
				//	int a=0;
				//if(texture!=NULL)
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
							osg::ref_ptr<osg::Texture2D> texture=createTexture(nTexId,DBPath);
							addTexture(0, nTexId,lodModel);
							/*		if (!texture)
							continue;*/
						
							
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

	osg::ref_ptr<osg::Texture2D> createTexture(UID64 nTexId,std::string DBPath) const
	{
		//osg::Texture2D* texture;
		//std::string DBPath="C:/Users/Administrator/Desktop/jinyang/jianzhu/NoDefine";
		std::stringstream ct3dTexName;
		ct3dTexName<<DBPath<<"."<<nTexId<<".ct3d_tex";
		//osg::Image* image2=osgDB::readImageFile(ct3dTexName.str());
		//Buffer texBuffer;
		//if(!pSQLiteTexOP->GetTextureByTexKey(nTexId,texBuffer))
		//	return NULL;

		//CMaterial pMaterial;

		//if (!pMaterial.readBuffer(texBuffer)||0!= pMaterial.getMaterialType())
		//	return NULL;

		//std::string string;
		//string.assign((char*)texBuffer.getCurPtr(),texBuffer.size() - texBuffer.getPos() + 1);
		//std::stringstream stream;
		//stream<<string;

		//std::string ext=osgDB::getFileExtension(pMaterial.getMaterialName());
		//osg::ref_ptr<osgDB::ReaderWriter> rw = osgDB::Registry::instance()->getReaderWriterForExtension( ext );
		//osg::ref_ptr<osg::Image> image = rw->readImage(stream).getImage();/*osgDB::readImageFile("C:/Users/Administrator/Desktop/jctz0113.tga")*/;

		//if (!image)
		//	return NULL;
		

		
	//	osgDB::Options *options=osgDB::Registry::instance()->getOptions();
	//	osg::ref_ptr<osg::Image> image=osgDB::readImageFile("C:/Users/Administrator/Desktop/QQ��ͼ20140717164707.png"/*,options*/);
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

	//std::map<int,osg::Texture2D*> createTexture(TexIDList texIDList,CSQLiteOP *pSQLiteTexOP) const
	//{
	//	std::map<int,osg::Texture2D*> textureMap;

	//	//texIDList.
	//	for (int i=0;i<texIDList.size();i++)
	//	{
	//		/*		if (i>1)
	//		break;*/
	//		Buffer texBuffer;
	//		if(!pSQLiteTexOP->GetTextureByTexKey(texIDList[i],texBuffer))
	//			continue;

	//		CMaterial pMaterial;

	//		if (!pMaterial.readBuffer(texBuffer)||0!= pMaterial.getMaterialType())
	//			continue;

	//		std::string string;
	//		string.assign((char*)texBuffer.getCurPtr(),texBuffer.size() - texBuffer.getPos() + 1);
	//		std::stringstream stream;
	//		stream<<string;

	//		std::string ext=osgDB::getFileExtension(pMaterial.getMaterialName());
	//		osg::ref_ptr<osgDB::ReaderWriter> rw = osgDB::Registry::instance()->getReaderWriterForExtension( ext );
	//		osg::ref_ptr<osg::Image> image = rw->readImage(stream).getImage();
	//		std::string fileName=std::string("C:/Users/Administrator/Desktop/dds/")+"temp."+ext;
	//		rw->writeImage(*image,fileName);

	//		if (!image->valid())
	//			continue;
	//		osg::Texture2D* texture = new osg::Texture2D( image/*osgDB::readImageFile("C:/Users/Administrator/Desktop/jinci_museum_obj/jchd0111.jpg")*/ );
	//		

	//		texture->setWrap(osg::Texture2D::WRAP_R, osg::Texture::REPEAT);
	//		texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture::REPEAT);
	//		texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture::REPEAT);

 //         //  textureMap.insert(make_pair())
	//		//textureMap.push_back(texture);
	//	}


	//	return textureMap;
	//}


	osg::Geometry*  convertTriangleMesh(C3DTriangleMesh* c3dTriangleMesh,Location location) const
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
				osg::Vec3d focalPoint;
				_srs->transformToWorld(osg::Vec3d(location.dLongitude,location.dLatitude,location.dElevation),focalPoint);
				for (int i=0;i<lVertNum;i++)
				{
					osg::Vec3d worldCoord;
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-focalPoint.x(),worldCoord.y()-focalPoint.y(),worldCoord.z()-focalPoint.z()));
					normals->push_back(osg::Vec3d(pList[i].nx,pList[i].ny,pList[i].nz));
					texcoords0->push_back(osg::Vec2d(pList[i].tu,1.0-pList[i].tv));
					colors->push_back(osg::Vec4d(1.0f,1.0f,1.0f,1.0f));
				}
				break;

			}

		case VERTEXTYPE_V3dT2dT2dN3dC3f:	// for SecondVersatileVertex3f
			{
				SecondVersatileVertex3d* pList=(SecondVersatileVertex3d*)c3dTriangleMesh->m_vVertexList;
				long lVertNum=(c3dTriangleMesh)->GetVerticesNum();

				//_srs = osgEarth::SpatialReference::create( "epsg:4326", "" );
				osg::Vec3d focalPoint;
				_srs->transformToWorld(osg::Vec3d(location.dLongitude,location.dLatitude,location.dElevation),focalPoint);
				for (int i=0;i<lVertNum;i++)
				{
					osg::Vec3d worldCoord;
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-focalPoint.x(),worldCoord.y()-focalPoint.y(),worldCoord.z()-focalPoint.z()));
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
			osg::Vec3d focalPoint;
			_srs->transformToWorld(osg::Vec3d(location.dLongitude,location.dLatitude,location.dElevation),focalPoint);
			for (int i=0;i<lVertNum;i++)
			{
				osg::Vec3d worldCoord;
				_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
				vertices->push_back(osg::Vec3d(worldCoord.x()-focalPoint.x(),worldCoord.y()-focalPoint.y(),worldCoord.z()-focalPoint.z()));
				normals->push_back(osg::Vec3d(pList[i].nx,pList[i].ny,pList[i].nz));
				colors->push_back(osg::Vec4d(1.0f,1.0f,1.0f,1.0f));
				texcoords0->push_back(osg::Vec2d(pList[i].tu,1.0-pList[i].tv));
				texcoords1->push_back(osg::Vec2d(pList[i].tu2,1.0-pList[i].tv2));
				texcoords2->push_back(osg::Vec2d(pList[i].tu3,1.0-pList[i].tv3));

			}
			break;
			}

		default:
			{
//				std::cout<<"default";
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

		osg::Material *mat = new osg::Material();
		mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
		mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
		mat->setShininess(osg::Material::FRONT_AND_BACK, 1.2f);
		osgGeometry->getOrCreateStateSet()->setAttribute(mat);

		return osgGeometry;


	}


	bool addStateSet(int nLod, osg::StateSet* stateset,LodModel* lodModel) const
	{


		std::map<int,std::vector<osg::ref_ptr<osg::StateSet> > >::iterator it = lodModel->_lodStateSet.find(nLod);
		if (it != lodModel->_lodStateSet.end())
		{
			it->second.push_back(stateset);
			return true;
		}

		std::vector<osg::ref_ptr<osg::StateSet> > vec;
		vec.push_back(stateset);
		lodModel->_lodStateSet.insert(std::make_pair(nLod,vec));
		return true;

	}


	osg::Drawable* C3DSurfaceToOSGDrawable(C3DSurface* c3dSurface,int surfaceID,Location location,LOD_MATERIAL_MAP mapMaterial,std::string DBPath,osg::Geode* geode) const
	{
		byte byType=c3dSurface->getTypeID();
		if(byType == GEOTYPE_3D_TRIANGLEMESH)	
		{
			//c3dSurface->getMatID()
			//void* vVertexList;
			VersatileVertex3d* pList=(VersatileVertex3d*)((C3DTriangleMesh*)c3dSurface)->m_vVertexList;
			long lVertNum=((C3DTriangleMesh*)c3dSurface)->GetVerticesNum();

			osg::Geometry* osgGeometry=convertTriangleMesh(((C3DTriangleMesh*)c3dSurface),location);
			//osg::Vec3Array* vertices = new osg::Vec3Array ;
			//osg::Vec3Array* normals = new osg::Vec3Array ;
			//osg::Vec4Array* colors = new osg::Vec4Array ;
			//osg::Vec2Array* texcoords = new osg::Vec2Array ;
			////_srs = osgEarth::SpatialReference::create( "epsg:4326", "" );
			//osg::Vec3d focalPoint;
			//_srs->transformToWorld(osg::Vec3d(location.dLongitude,location.dLatitude,location.dElevation),focalPoint);
			//for (int i=0;i<lVertNum;i++)
			//{
			//	osg::Vec3d worldCoord;
			//	_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
			//	vertices->push_back(osg::Vec3d(worldCoord.x()-focalPoint.x(),worldCoord.y()-focalPoint.y(),worldCoord.z()-focalPoint.z()));
			//	normals->push_back(osg::Vec3d(pList[i].nx,pList[i].ny,pList[i].nz));
			//	texcoords->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
			//	//	colors->push_back(osg::Vec4d(pList[i].r,pList[i].g,pList[i].b,1.0f));

			//}

			//osgGeometry->setVertexArray(vertices);
			//osgGeometry->setNormalArray(normals, osg::Array::BIND_PER_VERTEX);
			//osgGeometry->setTexCoordArray(0,texcoords);
			//	osgGeometry->setColorArray(colors,osg::Array::BIND_PER_VERTEX);

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
			//stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);

			if (mapMaterial.size()!=0)
			{



				//OSGMATERIAL osgMaterial=mapMaterial.begin()->second.at(surfaceID);
				CMaterial* pMaterial=(mapMaterial.begin())->second.at(surfaceID);

	/*			if (pMaterial==NULL)
					int a=0;*/
			

	//			setStateSet(pMaterial,stateset,DBPath,(LodModel*)geode);
				//addStateSet(0,stateset,(LodModel*)geode);
				//stateset->s
				//for (int i=0;i<osgMaterial.textureList.size();i++)
				//{

				//	stateset->setTextureAttributeAndModes( i, osgMaterial.textureList[i],osg::StateAttribute::ON );
				////	break;

				//}

			}
			geode->addDrawable(osgGeometry);
			return osgGeometry;

		} 
	}

	osg::Node* C3DGeometryToOSGNode(C3DGeometry * c3dGeometry,Location location,LOD_MATERIAL_MAP mapMaterial,std::string DBPath,osg::Geode* geode) const
	{
		if(c3dGeometry==NULL)
			return NULL;

		byte byType=c3dGeometry->getTypeID();
		if (byType == GEOTYPE_3D_GROUP)
		{
			//osg::Group* osgGroup=new osg::Group;

			_GeometryList c3dGeometryList;
			((C3DGroup *)c3dGeometry)->GetCollection(c3dGeometryList);


			for(int i=0;i<c3dGeometryList.size();i++)
			{
				C3DGeometry * c3dGeometry=c3dGeometryList[i].second;
				/*osgGroup->addChild(*/C3DGeometryToOSGNode(c3dGeometry,location,mapMaterial,DBPath,geode)/*)*/;
			}

			return /*(osg::Node*)osgGroup*/NULL;
		}
		else if(byType == GEOTYPE_3D_SOLID)
		{
			//osg::Geode* osgGeode=new osg::Geode;
			_3DSurfaceList c3dSurfaceList=((C3DSolid *)c3dGeometry)->get3DSurfaceList();

			for(int i=0;i<c3dSurfaceList.size();i++)
			{
				
				C3DSurface* c3dSurface=c3dSurfaceList[i].second;
				C3DSurfaceToOSGDrawable(c3dSurface,i,location,mapMaterial,DBPath,geode);
				//osgGeode->addChild();
				//osgGeode->addDrawable(C3DSurfaceToOSGDrawable(c3dSurface,i,location,mapMaterial,DBPath,geode));
			//	return osgGeode;
			}

			return /*osgGeode*/NULL;
		}


	}

	void parseFileName(std::string uri,double& lon,double& lat,double& elevation,unsigned long long& geoKey) const
	{
		std::vector<std::string> vecStr;

		std::string lessExtension;
		std::string ext=osgDB::getFileExtension(uri);
		//vecStr->push_back(ext);
		lessExtension=osgDB::getNameLessExtension(uri);

		for (int i=0;i<7;i++)
		{
			ext=osgDB::getFileExtension(lessExtension);
			vecStr.push_back(ext);
			lessExtension=osgDB::getNameLessExtension(lessExtension);
		}

		
		elevation= atof(std::string(vecStr[1]+"."+vecStr[0]).c_str());
		lat= atof(std::string(vecStr[3]+"."+vecStr[2]).c_str());
		lon= atof(std::string(vecStr[5]+"."+vecStr[4]).c_str());
		
		std::stringstream tempss;
		tempss<<vecStr[6].c_str();
		
		tempss>>geoKey;






		
	}

	//osg::Geode _geode;

	virtual ReadResult readNode(const std::string& uri, const Options* options) const
	{
		std::string ext = osgDB::getFileExtension(uri);

		

		if ( acceptsExtension(ext) )
		{
			Location location;
			unsigned long long geoKey;
			parseFileName(uri,location.dLongitude,location.dLatitude,location.dElevation,geoKey);
			//std::string lessExtension = osgDB::getNameLessExtension(uri);
			//std::stringstream tempss;
			//tempss<<osgDB::getFileExtension(lessExtension).c_str();
			//unsigned long long geoKey;
			//tempss>>geoKey;
			

			

			std::string DBPath=osgDB::getNameLessAllExtensions(uri);
			//std::string fcDBPath=DBPath+".fc";
			std::string geoDBPath=DBPath+".geo";
		//	std::string texDBPath=DBPath+".tex";
			//CSQLiteOP* pSQLiteFCOP=new CSQLiteOP();
			//pSQLiteFCOP->OpenDataBase(fcDBPath.c_str());
			CSQLiteOP pSQLiteGeoOP;
			pSQLiteGeoOP.OpenDataBase(geoDBPath.c_str());
			//CSQLiteOP* pSQLiteTexOP=new CSQLiteOP();
			//pSQLiteTexOP->OpenDataBase(texDBPath.c_str());



			//Buffer   featureBuffer;
			//pSQLiteFCOP->GetLodByFeatureKey(featureKey, featureBuffer);
			//CFeature feature;
			//feature.readBuffer(featureBuffer);

			//osg::ref_ptr<osg::LOD> lodModel=new osg::LOD;
			//GeoPoint geoPoint(
			//	_srs, 
			//	feature.getLocation().dLongitude, 
			//	feature.getLocation().dLatitude, 
			//	feature.getLocation().dElevation,
			//	ALTMODE_ABSOLUTE );
			//osg::Matrixd matrix;
			//geoPoint.createLocalToWorld( matrix );
			//osg::ref_ptr<osg::MatrixTransform>mt = new osg::MatrixTransform;
			//mt->setMatrix(matrix);
			//mt->addChild(lodModel );

	/*		for (int j=0;j<feature.getLOD_INFOList().size();j++)
			{

		    unsigned long long geoKey=feature.getLOD_INFOList()[j].unGeometryID;*/
			Buffer   geoBuffer;
			pSQLiteGeoOP.GetGeometryByGeoKey(geoKey,geoBuffer);
			C3DGeometry* c3dGeometry=createGeometry(geoBuffer);
			//
			LOD_MATERIAL_MAP mapMaterial;
			createMaterials(geoBuffer,mapMaterial);
		//	osg::ref_ptr<osg::Node> node=osgDB::readNodeFile("C:/Users/Administrator/Desktop/data/tree.ive");
			osg::ref_ptr<LodModel> node=new LodModel;
			 node->_dbName=DBPath;
			//osg::ref_ptr<osg::Geode> node=new osg::Geode;
			/*osg::ref_ptr<osg::Node> node=*/C3DGeometryToOSGNode(/*c3dGroup*/c3dGeometry,location,mapMaterial,DBPath,node);
			//lodModel->addChild(node,feature.getLOD_INFOList()[j].dSwitchIn,feature.getLOD_INFOList()[j].dSwitchOut);
			//lodModel->addChild(node,j*10,(j+1)*10);
		/*	}*/
			//pSQLiteFCOP->CloseDataBase();
			node->_lodMaterial=mapMaterial;
			pSQLiteGeoOP.CloseDataBase();
			delete c3dGeometry;
		//	LOD_MATERIAL_MAP::iterator itr=mapMaterial.begin();
		//	//typedef std::map<int,/*std::vector<OSGMATERIAL>*/std::vector<CMaterial*> >
		//while(itr!=mapMaterial.end())
		//{
		//	std::vector<CMaterial*> vm=itr->second;
		//	for(int i=0;i<vm.size();i++)
		//	{
		//		if (vm[i])
		//		delete vm[i];
		//		
		//		//delete itr->second[i];
		//	}
		//	itr++;
		//	//delete (itr->second);
		//	//mapMaterial.erase(*itr);
		//}

			//delete pSQLiteGeoOP;
		//	pSQLiteTexOP->CloseDataBase();

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

REGISTER_OSGPLUGIN(ct3d, CT3DDriver)

#endif // OSGEARTH_ENGINE_MP_OPTIONS
