
#include "model.h"
#include <osg/Texture2D>
#include <osg/Node>
#include <osg/Geode>
#include <Model/CFeature.h>
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
#include <osgEarth/Registry>
#include <zlib.h>

using namespace GeoGlobe::Model;
		

bool GeoGlobe::Model::Geometry::hasTexture()
{
	return _bSetTexture;
}

bool GeoGlobe::Model::Geometry::hasRealData()
{
	if(getNumDrawables()>0&&_hasRealData)
		return true;
	else
		return false;
}

bool GeoGlobe::Model::Geometry::uncompressGeoData(Buffer& geoBuf)
{
	//return false;
	unsigned long uncompressDataLen = *((int*)geoBuf.getCurPtr());
	unsigned char* uncompressData=new unsigned char[uncompressDataLen];
	geoBuf.setPos(4);

	if(0!=uncompress((unsigned char*)uncompressData, &uncompressDataLen, (unsigned char*)geoBuf.getCurPtr(), geoBuf.size()-4))
		return false;

	geoBuf.clear();
	geoBuf.set(uncompressData+5,uncompressDataLen-5);
	//delete uncompressData;
	return true;
}

bool GeoGlobe::Model::Geometry::GetModelGeoDataBuffer(ModelOptions* options,C3DGeometry** c3dGeometry)
{
	char* compressData=NULL;
	int compressDataLen=0;
	Buffer geoBuf;
	if(!_options->GetGeoDataFromDB(_options->_geoKey,compressDataLen,&compressData))
	{
		if (compressData)
			delete []compressData;

		return false;
	}

	unsigned char* uncompressData = NULL;
	GeoStar::Utility::GsGrowByteBuffer* tempGrowBuffer;
	unsigned long uncompressDataLen = *((int*)compressData);
	//uncompressData=new unsigned char[uncompressDataLen];
	tempGrowBuffer = g_GrowBufferFactory.Instance(uncompressDataLen);

	if (tempGrowBuffer == NULL || tempGrowBuffer->RealSize() < uncompressDataLen || tempGrowBuffer->BufferHead() == NULL)
	{
		if (compressData)
			delete []compressData;

		if (tempGrowBuffer)
			g_GrowBufferFactory.Recycle(tempGrowBuffer);

		return false;
	}

	uncompressData = (unsigned char*)tempGrowBuffer->BufferHead();
	if(0!=uncompress((unsigned char*)uncompressData, &uncompressDataLen, (unsigned char*)compressData+4, compressDataLen-4))
	{
		if (compressData)
			delete []compressData;

		if (tempGrowBuffer)
			g_GrowBufferFactory.Recycle(tempGrowBuffer);
		return false;
	}

    unsigned char szVersionSize = uncompressData[0];
	int nMixedHeadSize = 0;
	if(szVersionSize == 6)
	{
		char szVersion[8] = {0};
		char* pHeadBuffer = (char*)uncompressData;
		memcpy(szVersion, pHeadBuffer+1, szVersionSize);

		if (stricmp(szVersion, "GMDL52") == 0)
		{
			_eVertexCoordType = eVertexCoordTypeSpherical;
			_eVersion = eModelVersionGMDL502;
		}
		else if (stricmp(szVersion, "GMDL53") == 0)
		{
			_eVertexCoordType = eVertexCoordTypeLocal;
			_eVersion = eModelVersionGMDL503;
		}
		else if (stricmp(szVersion, "GMDL55") == 0)
		{
			_eVertexCoordType = eVertexCoordTypeSpherical;
			_eVersion = eModelVersionGMDL505;
		}
		else if (stricmp(szVersion, "GMDL56") == 0)
		{
			_eVertexCoordType = eVertexCoordTypeLocal;
			_eVersion = eModelVersionGMDL506;
		}
		else
		{
			if (compressData)
				delete []compressData;

			if (tempGrowBuffer)
				g_GrowBufferFactory.Recycle(tempGrowBuffer);

			return false;
		}

		nMixedHeadSize = szVersionSize + 1;
	}
	//浅拷贝
	geoBuf.set(uncompressData + 5 + nMixedHeadSize,uncompressDataLen - 5 - nMixedHeadSize);
	if (geoBuf.size() != uncompressDataLen - 5 - nMixedHeadSize)
	{
		if (compressData)
			delete []compressData;

		if (tempGrowBuffer)
			g_GrowBufferFactory.Recycle(tempGrowBuffer);

		return false;
	}

	*c3dGeometry=createC3DGeometry(geoBuf);
	_hasRealData=createMaterials(geoBuf);

	if (compressData)
		delete []compressData;

	if (tempGrowBuffer)
		g_GrowBufferFactory.Recycle(tempGrowBuffer);

	return true;
}


void GeoGlobe::Model::Geometry::createGeometryFromLocal()
{
	C3DGeometry* c3dGeometry = NULL;
	if (!GetModelGeoDataBuffer(_options,&c3dGeometry))
		return;

	//处理几何
	std::vector<C3DTriangleMesh*> vecRef;
	CProecssGetNode<C3DTriangleMesh> getRefNodel(&vecRef);
	CProcessGeometry processGoem(c3dGeometry, &getRefNodel);
	processGoem.DoSplite();
				
	if (_eVersion == eModelVersionGMDL503 || _eVersion == eModelVersionGMDL506)
	{
		for (int i=0;i< processGoem.m_vecD3dxMaterialParam.size();i++)
		{
			D3dxMaterialParam mParam = processGoem.m_vecD3dxMaterialParam[i];
			osg::ref_ptr<osg::Geometry> mesh = createTriangleMesh(vecRef[0],mParam);
			addDrawable(mesh);
		}
	}
	else
	{
		for (int i=0;i< vecRef.size();i++)
		{
			osg::ref_ptr<osg::Geometry> mesh = createTriangleMesh(vecRef[i]);
			addDrawable(mesh);
		}
	}

	if (c3dGeometry)
		delete c3dGeometry;
}

#if 0
void GeoGlobe::Model::Geometry::createGeometryFromService()
{
	char* compressData=NULL;
	int compressDataLen=0;
	std::string geoData;
	std::string geoFile=_options->createGeometryName(_options->_geoKey);

	if(!_options->_cache->readDataFromCache(geoFile,compressDataLen,&compressData)/*_options->_modelCache->GetGeoDataFromCache(_options->_geoKey,compressDataLen,&compressData)*/)
	{
		std::string geoURL=creatGeometryURL(_options->_geoKey);
		//bool a=osgEarth::Registry::instance()->isBlacklisted(geoURL);
		if (osgEarth::Registry::instance()->isBlacklisted(geoURL))
			return;

		//geoData=osgEarth::URI(geoURL.c_str()).getString();

		Download dl;


		//			__android_log_print(ANDROID_LOG_INFO, "createGeometryFromService", "featureSize=%d",1);
		if (!dl.downloadData(geoURL))
		{
			//osgEarth::Registry::inblacklist("");
			osgEarth::Registry::instance()->blacklist(geoURL);
			return;
		}
		else
		{
			_options->_cache->writeDataToCache(geoFile,dl.size,const_cast<char *>(dl.data));
			/*_options->_modelCache->readDataFromCache(geoFile,geoData2);
			geoData3=URI(geoFile).readString().getString();*/

		}

		compressData=const_cast<char *>(dl.data)+6;
		compressDataLen=dl.size-6-9;

		unsigned long uncompressDataLen = *((int*)/*tempBuf.getCurPtr())*/compressData);
		unsigned char* uncompressData=new unsigned char[uncompressDataLen];

		if(0==uncompress((unsigned char*)uncompressData, &uncompressDataLen, (unsigned char*)compressData+4, compressDataLen-4))
		{
			Buffer geoBuf=Buffer(uncompressData+5,uncompressDataLen-5);

			C3DGeometry* c3dGeometry=createC3DGeometry(geoBuf);
			_hasRealData=createMaterials(geoBuf);


			parseC3DGeometry(c3dGeometry);

			if (c3dGeometry)
				delete c3dGeometry;
		}
		delete []uncompressData;
		//					delete compressData;
	}
	else
	{
		unsigned long uncompressDataLen = *((int*)/*tempBuf.getCurPtr())*/(compressData+6));
		unsigned char* uncompressData=new unsigned char[uncompressDataLen];

		if(0==uncompress((unsigned char*)uncompressData, &uncompressDataLen, (unsigned char*)compressData+4+6, compressDataLen-4-6-9))
		{
			Buffer geoBuf=Buffer(uncompressData+5,uncompressDataLen-5);

			C3DGeometry* c3dGeometry=createC3DGeometry(geoBuf);
			_hasRealData=createMaterials(geoBuf);


			parseC3DGeometry(c3dGeometry);

			if (c3dGeometry)
				delete c3dGeometry;
		}
		delete []uncompressData;
		delete []compressData;
	}
}
#endif

#if 1
void GeoGlobe::Model::Geometry::createGeometryFromService()
{
	char* compressData=NULL;
	int compressDataLen=0;
	std::string geoData;
	std::string geoFile=_options->createGeometryName(_options->_geoKey);

	if(!_options->_cache->_geoCache.QuerySingleKeyValueTable("ModelGeometry",_options->_geoKey,compressDataLen,&compressData))
	{
		std::string geoURL=creatGeometryURL(_options->_geoKey);
		//bool a=osgEarth::Registry::instance()->isBlacklisted(geoURL);
		if (osgEarth::Registry::instance()->isBlacklisted(geoURL))
			return;

		//geoData=osgEarth::URI(geoURL.c_str()).getString();

		Download dl;

		//__android_log_print(ANDROID_LOG_INFO, "createGeometryFromService", "featureSize=%d",1);
		if (!dl.downloadData(geoURL))
		{
			//osgEarth::Registry::inblacklist("");
			osgEarth::Registry::instance()->blacklist(geoURL);
			return;
		}
		else
		{
			_options->_cache->writeDataToCache(geoFile,dl.size,const_cast<char *>(dl.data));
			/*_options->_modelCache->readDataFromCache(geoFile,geoData2);
			geoData3=URI(geoFile).readString().getString();*/
		}

		compressData=const_cast<char *>(dl.data)+6;
		compressDataLen=dl.size-6-9;

		unsigned long uncompressDataLen = *((int*)/*tempBuf.getCurPtr())*/compressData);
		unsigned char* uncompressData=new unsigned char[uncompressDataLen];

		if(0==uncompress((unsigned char*)uncompressData, &uncompressDataLen, (unsigned char*)compressData+4, compressDataLen-4))
		{
            unsigned char szVersionSize = uncompressData[0];
			int nMixedHeadSize = 0;
			if(szVersionSize == 6)
			{
				char szVersion[8] = {0};
				char* pHeadBuffer = (char*)uncompressData;
				memcpy(szVersion, pHeadBuffer+1, szVersionSize);

				if (stricmp(szVersion, "GMDL52") == 0)
				{
					_eVertexCoordType = eVertexCoordTypeSpherical;
					_eVersion = eModelVersionGMDL502;
				}
				else if (stricmp(szVersion, "GMDL53") == 0)
				{
					_eVertexCoordType = eVertexCoordTypeLocal;
					_eVersion = eModelVersionGMDL503;
				}
				else if (stricmp(szVersion, "GMDL55") == 0)
				{
					_eVertexCoordType = eVertexCoordTypeSpherical;
					_eVersion = eModelVersionGMDL505;
				}
				else if (stricmp(szVersion, "GMDL56") == 0)
				{
					_eVertexCoordType = eVertexCoordTypeLocal;
					_eVersion = eModelVersionGMDL506;
				}
				else
				{
					if (compressData)
						delete []compressData;

					return ;
				}

				nMixedHeadSize = szVersionSize + 1;
			}

			Buffer geoBuf=Buffer(uncompressData + 5 + nMixedHeadSize,uncompressDataLen - 5 -nMixedHeadSize);

			C3DGeometry* c3dGeometry=createC3DGeometry(geoBuf);
			_hasRealData=createMaterials(geoBuf);

			parseC3DGeometry(c3dGeometry);

			if (c3dGeometry)
				delete c3dGeometry;
		}
		delete []uncompressData;
		//delete compressData;

	}
	else
	{
		unsigned long uncompressDataLen = *((int*)/*tempBuf.getCurPtr())*/(compressData+6));
		unsigned char* uncompressData=new unsigned char[uncompressDataLen];

		if(0==uncompress((unsigned char*)uncompressData, &uncompressDataLen, (unsigned char*)compressData+4+6, compressDataLen-4-6-9))
		{
            unsigned char szVersionSize = uncompressData[0];
			int nMixedHeadSize = 0;
			if(szVersionSize == 6)
			{
				char szVersion[8] = {0};
				char* pHeadBuffer = (char*)uncompressData;
				memcpy(szVersion, pHeadBuffer+1, szVersionSize);

				if (stricmp(szVersion, "GMDL52") == 0)
				{
					_eVertexCoordType = eVertexCoordTypeSpherical;
					_eVersion = eModelVersionGMDL502;
				}
				else if (stricmp(szVersion, "GMDL53") == 0)
				{
					_eVertexCoordType = eVertexCoordTypeLocal;
					_eVersion = eModelVersionGMDL503;
				}
				else if (stricmp(szVersion, "GMDL55") == 0)
				{
					_eVertexCoordType = eVertexCoordTypeSpherical;
					_eVersion = eModelVersionGMDL505;
				}
				else if (stricmp(szVersion, "GMDL56") == 0)
				{
					_eVertexCoordType = eVertexCoordTypeLocal;
					_eVersion = eModelVersionGMDL506;
				}
				else
				{
					if (compressData)
						delete []compressData;

					return ;
				}

				nMixedHeadSize = szVersionSize + 1;
			}

			Buffer geoBuf=Buffer(uncompressData + 5 + nMixedHeadSize,uncompressDataLen - 5 - nMixedHeadSize);

			C3DGeometry* c3dGeometry=createC3DGeometry(geoBuf);
			_hasRealData=createMaterials(geoBuf);

			parseC3DGeometry(c3dGeometry);

			if (c3dGeometry)
				delete c3dGeometry;
		}

		delete []uncompressData;
		delete []compressData;
	}
}
#endif
GeoGlobe::Model::Geometry::Geometry(ModelOptions* options)
{
	_bSetTexture = false;
	_options=options;
	_srs =options->_srs;
	_centerPos=options->_centerPos;
	_lod=-1;
	_eVersion = eModelVersionGMDL502;
	_eVertexCoordType = eVertexCoordTypeSpherical;

	if (_options->_serviceUrl=="")
	{
		createGeometryFromLocal();
	}
	else
	{
		createGeometryFromService();
	}
}

std::string GeoGlobe::Model::Geometry::creatGeometryURL(ULONGLONG geoKey)
{
	//http://172.17.1.224:8088/shanxi37dom/services/tile/GetModelData?ModelID=10:20
	std::string geoURL=_options->_serviceUrl;
	std::stringstream buf;
	buf<<"/GetModelData?ModelID="<<geoKey;
	geoURL+=buf.str();
	return geoURL;
}

bool GeoGlobe::Model::Geometry::parseC3DGeometry(C3DGeometry* c3dGeometry)
{
	if(c3dGeometry==NULL)
		return false;

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
	}
	else if(byType == GEOTYPE_3D_SOLID)
	{
		_3DSurfaceList c3dSurfaceList=((C3DSolid *)c3dGeometry)->get3DSurfaceList();

		for(int i=0;i<c3dSurfaceList.size();i++)
		{

			C3DSurface* c3dSurface=c3dSurfaceList[i].second;
			parseC3DSurface(c3dSurface);
		}
	}

	return true;
}

void GeoGlobe::Model::Geometry::parseC3DSurface(C3DSurface* c3dSurface)
{
	byte byType=c3dSurface->getTypeID();
	if(byType == GEOTYPE_3D_TRIANGLEMESH)	
	{
		VersatileVertex3d* pList=(VersatileVertex3d*)((C3DTriangleMesh*)c3dSurface)->m_vVertexList;
		long lVertNum=((C3DTriangleMesh*)c3dSurface)->GetVerticesNum();
		osg::ref_ptr<osg::Geometry> mesh=createTriangleMesh(((C3DTriangleMesh*)c3dSurface));
		addDrawable(mesh);
	} 

}
osg::ref_ptr<osg::Geometry>  GeoGlobe::Model::Geometry::createTriangleMesh(C3DTriangleMesh* c3dTriangleMesh) const
{
	osg::ref_ptr<osg::Geometry> mesh=new osg::Geometry;
	mesh->setUseDisplayList(true);

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array ;
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array ;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array ;
	osg::ref_ptr<osg::Vec2Array> texcoords0 = new osg::Vec2Array ;
	osg::ref_ptr<osg::Vec2Array> texcoords1 = new osg::Vec2Array ;
	osg::ref_ptr<osg::Vec2Array> texcoords2 = new osg::Vec2Array ;

	int nVertexType=c3dTriangleMesh->GetVertexType();
	switch (nVertexType)
	{
	case VERTEXTYPE_V3f:			// for Vertex3f
		{
			Vertex3f* pList = (Vertex3f*)c3dTriangleMesh->m_vVertexList;
			break;
		}
	case VERTEXTYPE_V3d:
		{
			Vertex3d* pList = (Vertex3d*)c3dTriangleMesh->m_vVertexList;
			break;
		}
	case VERTEXTYPE_V3fT2f:
		{
			TexturedVertex3f* pList = (TexturedVertex3f*)c3dTriangleMesh->m_vVertexList;

			long lVertNum=(c3dTriangleMesh)->GetVerticesNum();
			for (int i=0;i<lVertNum;i++)
			{
				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[i].x,pList[i].y,pList[i].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[i].x-_centerPos.x(),pList[i].y-_centerPos.y(),pList[i].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
			}

			break;
		}
	case VERTEXTYPE_V3dT2d:
		{
			VersatileVertex3d* pList = (VersatileVertex3d*)c3dTriangleMesh->m_vVertexList;

			long lVertNum=(c3dTriangleMesh)->GetVerticesNum();
			for (int i=0;i<lVertNum;i++)
			{
				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[i].x,pList[i].y,pList[i].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[i].x-_centerPos.x(),pList[i].y-_centerPos.y(),pList[i].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
			}

			break;
		}
	case VERTEXTYPE_V3fN3fC3f:
		{
			ShadedVertex3f* pList = (ShadedVertex3f*)c3dTriangleMesh->m_vVertexList;
			break;
		}
	case VERTEXTYPE_V3dN3dC3f:
		{
			ShadedVertex3d* pList = (ShadedVertex3d*)c3dTriangleMesh->m_vVertexList;
			break;
		}
	case VERTEXTYPE_V3fT2fN3fC3f:			// for Vertex3f
		{
			VersatileVertex3f* pList = (VersatileVertex3f*)c3dTriangleMesh->m_vVertexList;
			long lVertNum=(c3dTriangleMesh)->GetVerticesNum();
			for (int i=0;i<lVertNum;i++)
			{
				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[i].x,pList[i].y,pList[i].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[i].x-_centerPos.x(),pList[i].y-_centerPos.y(),pList[i].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
			}

			break;
		}
	case VERTEXTYPE_V3dT2dN3dC3f:
		{
			VersatileVertex3d* pList = (VersatileVertex3d*)c3dTriangleMesh->m_vVertexList;
			long lVertNum=(c3dTriangleMesh)->GetVerticesNum();
			for (int i=0;i<lVertNum;i++)
			{
				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[i].x,pList[i].y,pList[i].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[i].x-_centerPos.x(),pList[i].y-_centerPos.y(),pList[i].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
			}

			break;
		}
	case VERTEXTYPE_V3fT2fT2fN3fC3f:
		{
			SecondVersatileVertex3f* pList = (SecondVersatileVertex3f*)c3dTriangleMesh->m_vVertexList;
			long lVertNum=(c3dTriangleMesh)->GetVerticesNum();
			for (int i=0;i<lVertNum;i++)
			{
				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[i].x,pList[i].y,pList[i].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[i].x-_centerPos.x(),pList[i].y-_centerPos.y(),pList[i].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
				texcoords1->push_back(osg::Vec2d(pList[i].tu2,pList[i].tv2));
			}
			break;
		}
	case VERTEXTYPE_V3dT2dT2dN3dC3f:
		{
			SecondVersatileVertex3d* pList=(SecondVersatileVertex3d*)c3dTriangleMesh->m_vVertexList;
			long lVertNum=(c3dTriangleMesh)->GetVerticesNum();
			for (int i=0;i<lVertNum;i++)
			{
				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[i].x,pList[i].y,pList[i].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[i].x-_centerPos.x(),pList[i].y-_centerPos.y(),pList[i].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
				texcoords1->push_back(osg::Vec2d(pList[i].tu2,pList[i].tv2));
			}

			break;
		}
	case VERTEXTYPE_V3fT2fT2fT2fN3fC3f:
		{
			ThirdVersatileVertex3f* pList = (ThirdVersatileVertex3f*)c3dTriangleMesh->m_vVertexList;
			long lVertNum=(c3dTriangleMesh)->GetVerticesNum();
			for (int i=0;i<lVertNum;i++)
			{
				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[i].x,pList[i].y,pList[i].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[i].x-_centerPos.x(),pList[i].y-_centerPos.y(),pList[i].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
				texcoords1->push_back(osg::Vec2d(pList[i].tu2,pList[i].tv2));
				texcoords2->push_back(osg::Vec2d(pList[i].tu3,pList[i].tv3));
			}

			break;
		}
	case VERTEXTYPE_V3dT2dT2dT2dN3dC3f:
		{
			ThirdVersatileVertex3d* pList=(ThirdVersatileVertex3d*)c3dTriangleMesh->m_vVertexList;
			long lVertNum=(c3dTriangleMesh)->GetVerticesNum();
			for (int i=0;i<lVertNum;i++)
			{
				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[i].x,pList[i].y,pList[i].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[i].x-_centerPos.x(),pList[i].y-_centerPos.y(),pList[i].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
				texcoords1->push_back(osg::Vec2d(pList[i].tu2,pList[i].tv2));
				texcoords2->push_back(osg::Vec2d(pList[i].tu3,pList[i].tv3));
			}

			break;
		}
	case VERTEXTYPE_V3dT2dN3dC3f_WithoutRGB:
		{
			VersatileVertex3dWithoutRGB* pList=(VersatileVertex3dWithoutRGB*)c3dTriangleMesh->m_vVertexList;
			long lVertNum=(c3dTriangleMesh)->GetVerticesNum();
			for (int i=0;i<lVertNum;i++)
			{
				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[i].x,pList[i].y,pList[i].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[i].x -_centerPos.x(),pList[i].y -_centerPos.y(),pList[i].z -_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
			}

			break;
		}
	case VERTEXTYPE_V3dT2dT2dN3dC3f_WithoutRGB:
		{
			SecondVersatileVertex3dWithoutRGB* pList = (SecondVersatileVertex3dWithoutRGB*)c3dTriangleMesh->m_vVertexList;
			long lVertNum=(c3dTriangleMesh)->GetVerticesNum();
			for (int i=0;i<lVertNum;i++)
			{
				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[i].x,pList[i].y,pList[i].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[i].x-_centerPos.x(),pList[i].y-_centerPos.y(),pList[i].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
				texcoords1->push_back(osg::Vec2d(pList[i].tu2,pList[i].tv2));
			}

			break;
		}
	case VERTEXTYPE_V3dT2dT2dT2dN3dC3f_WithoutRGB:
		{
			ThirdVersatileVertex3dWithoutRGB* pList = (ThirdVersatileVertex3dWithoutRGB*)c3dTriangleMesh->m_vVertexList;
			long lVertNum=(c3dTriangleMesh)->GetVerticesNum();
			for (int i=0;i<lVertNum;i++)
			{
				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[i].x,pList[i].y,pList[i].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[i].x-_centerPos.x(),pList[i].y-_centerPos.y(),pList[i].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
				texcoords1->push_back(osg::Vec2d(pList[i].tu2,pList[i].tv2));
				texcoords2->push_back(osg::Vec2d(pList[i].tu3,pList[i].tv3));
			}

			break;
		}
	case VERTEXTYPE_V3fT2fN3fC3f_WithoutRGB:
		{
			VersatileVertex3fWithoutRGB* pList = (VersatileVertex3fWithoutRGB*)c3dTriangleMesh->m_vVertexList;

			long lVertNum=(c3dTriangleMesh)->GetVerticesNum();
			for (int i=0;i<lVertNum;i++)
			{
				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[i].x,pList[i].y,pList[i].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[i].x -_centerPos.x(),pList[i].y -_centerPos.y(),pList[i].z -_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
			}
			break;
		}
	case VERTEXTYPE_V3fT2fT2fN3fC3f_WithoutRGB:
		{
			SecondVersatileVertex3fWithoutRGB* pList = (SecondVersatileVertex3fWithoutRGB*)c3dTriangleMesh->m_vVertexList;
			long lVertNum=(c3dTriangleMesh)->GetVerticesNum();
			for (int i=0;i<lVertNum;i++)
			{
				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[i].x,pList[i].y,pList[i].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[i].x-_centerPos.x(),pList[i].y-_centerPos.y(),pList[i].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
				texcoords1->push_back(osg::Vec2d(pList[i].tu2,pList[i].tv2));
			}

			break;
		}
	case VERTEXTYPE_V3fT2fT2fT2fN3fC3f_WithoutRGB:
		{
			ThirdVersatileVertex3fWithoutRGB* pList = (ThirdVersatileVertex3fWithoutRGB*)c3dTriangleMesh->m_vVertexList;
			long lVertNum=(c3dTriangleMesh)->GetVerticesNum();
			for (int i=0;i<lVertNum;i++)
			{
				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[i].x,pList[i].y,pList[i].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[i].x-_centerPos.x(),pList[i].y-_centerPos.y(),pList[i].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
				texcoords1->push_back(osg::Vec2d(pList[i].tu2,pList[i].tv2));
				texcoords2->push_back(osg::Vec2d(pList[i].tu3,pList[i].tv3));
			}

			break;
		}
	case VERTEXTYPE_V3fN3f:
		{
			NormaledVertex3f* pList = (NormaledVertex3f*)c3dTriangleMesh->m_vVertexList;
			break;
		}
	case VERTEXTYPE_V3dN3d:
		{
			NormaledVertex3d* pList = (NormaledVertex3d*)c3dTriangleMesh->m_vVertexList;
			break;
		}
	case VERTEXTYPE_End:
		{
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
	if(texcoords0->size())
		mesh->setTexCoordArray(0,texcoords0);
	if(texcoords1->size())
		mesh->setTexCoordArray(1,texcoords1);
	if(texcoords2->size())
		mesh->setTexCoordArray(2,texcoords2);
	colors->push_back(osg::Vec4d(1.0f,1.0f,1.0f,1.0f));
	mesh->setColorArray(colors,osg::Array::BIND_OVERALL);

	long *lTriangleList;
	long lTriListSize;
	c3dTriangleMesh->GetTriangles(lTriangleList,lTriListSize);
	if (lTriListSize>0)
	{
		osg::ref_ptr<osg::DrawElementsUInt> triangle=new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,0);
		if (triangle == NULL)
			return NULL;

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
		osg::ref_ptr<osg::DrawElementsUInt> strip=new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP,0);
		if (strip == NULL)
			return NULL;

		for(int i=0;i<lStripListSize;i++)
		{
			strip->push_back(lStripList[i]);
		}
		mesh->addPrimitiveSet(strip);
	}

	return mesh.release();
}

osg::ref_ptr<osg::Geometry>  GeoGlobe::Model::Geometry::createTriangleMesh(C3DTriangleMesh* c3dTriangleMesh, D3dxMaterialParam D3dMParam) const
{
	unsigned long nVertexStart = D3dMParam.nVertexStart;
	unsigned long nVertexCount = D3dMParam.nVertexCount;
	unsigned long nFaceStart = D3dMParam.nFaceStart;
	unsigned long nFaceCount = D3dMParam.nFaceCount;

	osg::ref_ptr<osg::Geometry> mesh=new osg::Geometry;
	mesh->setUseDisplayList(true);

	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array ;
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array ;
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array ;
	osg::ref_ptr<osg::Vec2Array> texcoords0 = new osg::Vec2Array ;
	osg::ref_ptr<osg::Vec2Array> texcoords1 = new osg::Vec2Array ;
	osg::ref_ptr<osg::Vec2Array> texcoords2 = new osg::Vec2Array ;

	int nVertexType=c3dTriangleMesh->GetVertexType();
	switch (nVertexType)
	{
	case VERTEXTYPE_V3f:			// for Vertex3f
		{
			Vertex3f* pList = (Vertex3f*)c3dTriangleMesh->m_vVertexList;
			break;
		}
	case VERTEXTYPE_V3d:
		{
			Vertex3d* pList = (Vertex3d*)c3dTriangleMesh->m_vVertexList;
			break;
		}
	case VERTEXTYPE_V3fT2f:
		{
			TexturedVertex3f* pList = (TexturedVertex3f*)c3dTriangleMesh->m_vVertexList;
						
			long *lTriangleList;
			long lTriListSize;
			c3dTriangleMesh->GetTriangles(lTriangleList,lTriListSize);

			for(int i = nFaceStart * 3;i< (nFaceStart + nFaceCount) * 3;i++)
			{
				long nIndex = lTriangleList[i];

				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[nIndex].x-_centerPos.x(),pList[nIndex].y-_centerPos.y(),pList[nIndex].z-_centerPos.z()));
				}
				texcoords0->push_back(osg::Vec2d(pList[nIndex].tu,pList[nIndex].tv));
			}

			break;
		}
	case VERTEXTYPE_V3dT2d:
		{
			VersatileVertex3d* pList = (VersatileVertex3d*)c3dTriangleMesh->m_vVertexList;

			long *lTriangleList;
			long lTriListSize;
			c3dTriangleMesh->GetTriangles(lTriangleList,lTriListSize);

			for(int i = nFaceStart * 3;i< (nFaceStart + nFaceCount) * 3;i++)
			{
				long nIndex = lTriangleList[i];

				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[nIndex].x-_centerPos.x(),pList[nIndex].y-_centerPos.y(),pList[nIndex].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[nIndex].tu,pList[nIndex].tv));
			}

			break;
		}
	case VERTEXTYPE_V3fN3fC3f:
		{
			ShadedVertex3f* pList = (ShadedVertex3f*)c3dTriangleMesh->m_vVertexList;
			break;
		}
	case VERTEXTYPE_V3dN3dC3f:
		{
			ShadedVertex3d* pList = (ShadedVertex3d*)c3dTriangleMesh->m_vVertexList;
			break;
		}
	case VERTEXTYPE_V3fT2fN3fC3f:			// for Vertex3f
		{
			VersatileVertex3f* pList = (VersatileVertex3f*)c3dTriangleMesh->m_vVertexList;
			long *lTriangleList;
			long lTriListSize;
			c3dTriangleMesh->GetTriangles(lTriangleList,lTriListSize);

			for(int i = nFaceStart * 3;i< (nFaceStart + nFaceCount) * 3;i++)
			{
				long nIndex = lTriangleList[i];

				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[nIndex].x-_centerPos.x(),pList[nIndex].y-_centerPos.y(),pList[nIndex].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[nIndex].tu,pList[nIndex].tv));
			}

			break;
		}
	case VERTEXTYPE_V3dT2dN3dC3f:
		{
			VersatileVertex3d* pList = (VersatileVertex3d*)c3dTriangleMesh->m_vVertexList;
			long *lTriangleList;
			long lTriListSize;
			c3dTriangleMesh->GetTriangles(lTriangleList,lTriListSize);

			for(int i = nFaceStart * 3;i< (nFaceStart + nFaceCount) * 3;i++)
			{
				long nIndex = lTriangleList[i];

				osg::Vec3d worldCoord;
				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					_srs->transformToWorld(osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[nIndex].x-_centerPos.x(),pList[nIndex].y-_centerPos.y(),pList[nIndex].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[nIndex].tu,pList[nIndex].tv));
			}

			break;
		}
	case VERTEXTYPE_V3fT2fT2fN3fC3f:
		{
			SecondVersatileVertex3f* pList = (SecondVersatileVertex3f*)c3dTriangleMesh->m_vVertexList;
			long *lTriangleList;
			long lTriListSize;
			c3dTriangleMesh->GetTriangles(lTriangleList,lTriListSize);

			for(int i = nFaceStart * 3;i< (nFaceStart + nFaceCount) * 3;i++)
			{
				long nIndex = lTriangleList[i];

				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					osg::Vec3d worldCoord;
					_srs->transformToWorld(osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[nIndex].x-_centerPos.x(),pList[nIndex].y-_centerPos.y(),pList[nIndex].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[nIndex].tu,pList[nIndex].tv));
				texcoords1->push_back(osg::Vec2d(pList[nIndex].tu2,pList[nIndex].tv2));
			}

			break;
		}
	case VERTEXTYPE_V3dT2dT2dN3dC3f:
		{
			SecondVersatileVertex3d* pList=(SecondVersatileVertex3d*)c3dTriangleMesh->m_vVertexList;
			long *lTriangleList;
			long lTriListSize;
			c3dTriangleMesh->GetTriangles(lTriangleList,lTriListSize);

			for(int i = nFaceStart * 3;i< (nFaceStart + nFaceCount) * 3;i++)
			{
				long nIndex = lTriangleList[i];

				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					osg::Vec3d worldCoord;
					_srs->transformToWorld(osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[nIndex].x-_centerPos.x(),pList[nIndex].y-_centerPos.y(),pList[nIndex].z-_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[nIndex].tu,pList[nIndex].tv));
				texcoords1->push_back(osg::Vec2d(pList[nIndex].tu2,pList[nIndex].tv2));
			}
			break;
		}
	case VERTEXTYPE_V3fT2fT2fT2fN3fC3f:
		{
			ThirdVersatileVertex3f* pList = (ThirdVersatileVertex3f*)c3dTriangleMesh->m_vVertexList;
			long *lTriangleList;
			long lTriListSize;
			c3dTriangleMesh->GetTriangles(lTriangleList,lTriListSize);

			for(int i = nFaceStart * 3;i< (nFaceStart + nFaceCount) * 3;i++)
			{
				long nIndex = lTriangleList[i];

				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					osg::Vec3d worldCoord;
					_srs->transformToWorld(osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[nIndex].x -_centerPos.x(),pList[nIndex].y -_centerPos.y(),pList[nIndex].z -_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[nIndex].tu,pList[nIndex].tv));
				texcoords1->push_back(osg::Vec2d(pList[nIndex].tu2,pList[nIndex].tv2));
				texcoords2->push_back(osg::Vec2d(pList[nIndex].tu3,pList[nIndex].tv3));
			}

			break;
		}
	case VERTEXTYPE_V3dT2dT2dT2dN3dC3f:
		{
			ThirdVersatileVertex3d* pList=(ThirdVersatileVertex3d*)c3dTriangleMesh->m_vVertexList;
			long *lTriangleList;
			long lTriListSize;
			c3dTriangleMesh->GetTriangles(lTriangleList,lTriListSize);

			for(int i = nFaceStart * 3;i< (nFaceStart + nFaceCount) * 3;i++)
			{
				long nIndex = lTriangleList[i];

				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					osg::Vec3d worldCoord;
					_srs->transformToWorld(osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[nIndex].x -_centerPos.x(),pList[nIndex].y -_centerPos.y(),pList[nIndex].z -_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[nIndex].tu,pList[nIndex].tv));
				texcoords1->push_back(osg::Vec2d(pList[nIndex].tu2,pList[nIndex].tv2));
				texcoords2->push_back(osg::Vec2d(pList[nIndex].tu3,pList[nIndex].tv3));
			}

			break;
		}
	case VERTEXTYPE_V3dT2dN3dC3f_WithoutRGB:
		{
			VersatileVertex3dWithoutRGB* pList=(VersatileVertex3dWithoutRGB*)c3dTriangleMesh->m_vVertexList;
			long *lTriangleList;
			long lTriListSize;
			c3dTriangleMesh->GetTriangles(lTriangleList,lTriListSize);

			for(int i = nFaceStart * 3;i< (nFaceStart + nFaceCount) * 3;i++)
			{
				long nIndex = lTriangleList[i];

				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					osg::Vec3d worldCoord;
					_srs->transformToWorld(osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z);
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[nIndex].x -_centerPos.x(),pList[nIndex].y -_centerPos.y(),pList[nIndex].z -_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[nIndex].tu,pList[nIndex].tv));
			}

			break;
		}
	case VERTEXTYPE_V3dT2dT2dN3dC3f_WithoutRGB:
		{
			SecondVersatileVertex3dWithoutRGB* pList = (SecondVersatileVertex3dWithoutRGB*)c3dTriangleMesh->m_vVertexList;
			long *lTriangleList;
			long lTriListSize;
			c3dTriangleMesh->GetTriangles(lTriangleList,lTriListSize);

			for(int i = nFaceStart * 3;i< (nFaceStart + nFaceCount) * 3;i++)
			{
				long nIndex = lTriangleList[i];

				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					osg::Vec3d worldCoord;
					_srs->transformToWorld(osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[nIndex].x -_centerPos.x(),pList[nIndex].y -_centerPos.y(),pList[nIndex].z -_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[nIndex].tu,pList[nIndex].tv));
				texcoords1->push_back(osg::Vec2d(pList[nIndex].tu2,pList[nIndex].tv2));
			}

			break;
		}
	case VERTEXTYPE_V3dT2dT2dT2dN3dC3f_WithoutRGB:
		{
			ThirdVersatileVertex3dWithoutRGB* pList = (ThirdVersatileVertex3dWithoutRGB*)c3dTriangleMesh->m_vVertexList;
			long *lTriangleList;
			long lTriListSize;
			c3dTriangleMesh->GetTriangles(lTriangleList,lTriListSize);

			for(int i = nFaceStart * 3;i< (nFaceStart + nFaceCount) * 3;i++)
			{
				long nIndex = lTriangleList[i];

				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					osg::Vec3d worldCoord;
					_srs->transformToWorld(osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[nIndex].x -_centerPos.x(),pList[nIndex].y -_centerPos.y(),pList[nIndex].z -_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[nIndex].tu,pList[nIndex].tv));
				texcoords1->push_back(osg::Vec2d(pList[nIndex].tu2,pList[nIndex].tv2));
				texcoords2->push_back(osg::Vec2d(pList[nIndex].tu3,pList[nIndex].tv3));
			}

			break;
		}
	case VERTEXTYPE_V3fT2fN3fC3f_WithoutRGB:
		{
			VersatileVertex3fWithoutRGB* pList = (VersatileVertex3fWithoutRGB*)c3dTriangleMesh->m_vVertexList;

			long *lTriangleList;
			long lTriListSize;
			c3dTriangleMesh->GetTriangles(lTriangleList,lTriListSize);

			for(int i = nFaceStart * 3;i< (nFaceStart + nFaceCount) * 3;i++)
			{
				long nIndex = lTriangleList[i];

				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					osg::Vec3d worldCoord;
					_srs->transformToWorld(osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[nIndex].x -_centerPos.x(),pList[nIndex].y -_centerPos.y(),pList[nIndex].z -_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[nIndex].tu,pList[nIndex].tv));
			}
			break;
		}
	case VERTEXTYPE_V3fT2fT2fN3fC3f_WithoutRGB:
		{
			SecondVersatileVertex3fWithoutRGB* pList = (SecondVersatileVertex3fWithoutRGB*)c3dTriangleMesh->m_vVertexList;
			long *lTriangleList;
			long lTriListSize;
			c3dTriangleMesh->GetTriangles(lTriangleList,lTriListSize);

			for(int i = nFaceStart * 3;i< (nFaceStart + nFaceCount) * 3;i++)
			{
				long nIndex = lTriangleList[i];

				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					osg::Vec3d worldCoord;
					_srs->transformToWorld(osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[nIndex].x -_centerPos.x(),pList[nIndex].y -_centerPos.y(),pList[nIndex].z -_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[nIndex].tu,pList[nIndex].tv));
				texcoords1->push_back(osg::Vec2d(pList[nIndex].tu2,pList[nIndex].tv2));
			}

			break;
		}
	case VERTEXTYPE_V3fT2fT2fT2fN3fC3f_WithoutRGB:
		{
			ThirdVersatileVertex3fWithoutRGB* pList = (ThirdVersatileVertex3fWithoutRGB*)c3dTriangleMesh->m_vVertexList;
						
			long *lTriangleList;
			long lTriListSize;
			c3dTriangleMesh->GetTriangles(lTriangleList,lTriListSize);

			for(int i = nFaceStart * 3;i< (nFaceStart + nFaceCount) * 3;i++)
			{
				long nIndex = lTriangleList[i];

				if (_eVertexCoordType == eVertexCoordTypeSpherical)
				{
					osg::Vec3d worldCoord;
					_srs->transformToWorld(osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z),worldCoord);
					vertices->push_back(osg::Vec3d(worldCoord.x()-_centerPos.x(),worldCoord.y()-_centerPos.y(),worldCoord.z()-_centerPos.z()));
				}
				else if (_eVertexCoordType == eVertexCoordTypeLocal)
				{
					osg::Vec3d vecR =  osg::Vec3d(pList[nIndex].x,pList[nIndex].y,pList[nIndex].z) ;
					vertices->push_back(vecR);
				}
				else
				{
					vertices->push_back(osg::Vec3d(pList[nIndex].x -_centerPos.x(),pList[nIndex].y -_centerPos.y(),pList[nIndex].z -_centerPos.z()));
				}
				//normals->push_back(osg::Vec3d(pList[nIndex].nx,pList[nIndex].ny,pList[nIndex].nz));
				texcoords0->push_back(osg::Vec2d(pList[nIndex].tu,pList[nIndex].tv));
				texcoords1->push_back(osg::Vec2d(pList[nIndex].tu2,pList[nIndex].tv2));
				texcoords2->push_back(osg::Vec2d(pList[nIndex].tu3,pList[nIndex].tv3));
			}

			break;
		}
	case VERTEXTYPE_V3fN3f:
		{
			NormaledVertex3f* pList = (NormaledVertex3f*)c3dTriangleMesh->m_vVertexList;
			break;
		}
	case VERTEXTYPE_V3dN3d:
		{
			NormaledVertex3d* pList = (NormaledVertex3d*)c3dTriangleMesh->m_vVertexList;
			break;
		}
	case VERTEXTYPE_End:
		{
			break;
		}
	default:
		{
			std::cout<<"default";
			break;
		}
	}

	if(vertices->size())
		mesh->setVertexArray(vertices);
	if(normals->size())
		mesh->setNormalArray(normals, osg::Array::BIND_PER_VERTEX);
	if(texcoords0->size())
		mesh->setTexCoordArray(0,texcoords0);
	if(texcoords1->size())
		mesh->setTexCoordArray(1,texcoords1);
	if(texcoords2->size())
		mesh->setTexCoordArray(2,texcoords2);
	colors->push_back(osg::Vec4d(1.0f,1.0f,1.0f,1.0f));
	mesh->setColorArray(colors,osg::Array::BIND_OVERALL);

	osg::ref_ptr<osg::DrawElementsUInt> triangle=new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES,0);
	if (triangle == NULL)
		return NULL;

	for(int i=0;i< nFaceCount * 3 ;i++)
	{
		triangle->push_back(i);
	}
	mesh->addPrimitiveSet(triangle);

	return mesh.release();
}

double GeoGlobe::Model::Geometry::ToDegree(double d)
{
	return d * 180.0 / M_PI;
}
bool GeoGlobe::Model::Geometry::ToSphere(osg::Vec3d pt,double dblRadius,osg::Vec3d& ptRet)
{
	//缺省为地球球心
	//ptRet->set(0.0,0.0,0.0);
	double dblx,dbly,dblz;
	dblz = 0 - dblRadius;
	if(0 == pt.x() && 0 == pt.y() && 0 == pt.z())
		return true;

	//有X，Y，Z，坐标计算出设备点到中心的距离。
	double rho = sqrt(pt.x() * pt.x() + pt.y() * pt.y() + pt.z() * pt.z());

	//Z高度就等于该距离减去地球半径。
	dblz = rho - dblRadius;

	//tan(geo) = dy/dx
	dblx  = atan2( pt.y(),pt.x());
	dblx  = ToDegree(dblx);

	dbly= asin(pt.z() / rho);
	dbly = ToDegree(dbly);

	//ptRet->set(dblx,dbly,dblz);

	return true;
}

C3DGeometry* GeoGlobe::Model::Geometry::createC3DGeometry(Buffer& buff) 
{

	//��9���ֽ�ΪType
	byte byGeomType = *((byte*)buff.getCurPtr() + 8);

	//���ɶ�Ӧ���͵�geometry
	C3DGeometry* pGeometry = (C3DGeometry*)CGeometry::CreateGeometry(byGeomType);
	if (pGeometry!=NULL&&!pGeometry->readBuffer(buff)) 
	{
		SafeDelete(pGeometry);
		return NULL;
	}

	return pGeometry;
}

bool GeoGlobe::Model::Geometry::createMaterials(Buffer& buf) 
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

			osg::ref_ptr<CMaterial> pMaterial = new CMaterial();
			if (NULL == pMaterial)
				return NULL;

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

		if (_eVersion >= eModelVersionGMDL505)
		{
			//读取纹理包的ID
			UID64 nTexturePkgID = *((UID64*)buf.getCurPtr());
			buf.setPos(buf.getPos()+8);
			_mapTexturePkgID.insert(std::map<int, UID64>::value_type(nLodLevel,nTexturePkgID));
		}
	}

	return true;
}

bool GeoGlobe::Model::Geometry::addMaterial(int nLod, CMaterial* pMaterial) 
{
	LOD_C3D_MATERIALS::iterator it = _lodMaterials.find(nLod);
	if (it != _lodMaterials.end())
	{
		it->second.push_back(pMaterial);
		return true;
	}

	std::vector<osg::ref_ptr<CMaterial> > vec;
	vec.push_back(pMaterial);
	_lodMaterials.insert(LOD_C3D_MATERIALS::value_type(nLod,vec));
	return true;
}


void  GeoGlobe::Model::Geometry::getMaterials(C3D_MATERIALS& materials,int lod)
{
	LOD_C3D_MATERIALS::iterator it = _lodMaterials.find(lod);
	if (it != _lodMaterials.end())
	{
		materials=_lodMaterials.find(lod)->second;
	}
}


bool GeoGlobe::Model::Geometry::setTextures(std::vector<osg::ref_ptr<osg::StateSet> >& textures,int lod)
{
	if (lod==_lod)
		return true;
	if(textures.size()<getNumDrawables() )
		return false;

	for (int i=0;i<getNumDrawables();i++)
	{
		getDrawable(i)->setStateSet(textures[i]);
	}

	_lod=lod;
	_bSetTexture = true;
	return true;
}




GeoGlobe::Model::Model::Model(ModelOptions* options)
{
	_options=options;
	osg::ref_ptr<Geometry> geometry=createGeometry(_options->_geoKey);

	if (geometry.valid())
	{
		_geometry=geometry;

		if (_geometry->hasTexture() == false)
		{
			//如果有打包纹理，解析打包纹理
			std::map<int, UID64>::iterator it = geometry->_mapTexturePkgID.find(_options->_lod);
			if (it != geometry->_mapTexturePkgID.end())
			{
				UID64 nTexturePkgID = it->second;
				parseTexturePkg(nTexturePkgID);
			}

			C3D_MATERIALS materials;
			_geometry->getMaterials(materials,_options->_lod);
			//解析材质纹理
			parseC3DMaterials(materials);

			//设置Textures
			_geometry->setTextures(_textures,_options->_lod);
		}
		this->addChild(_geometry);

		//注册数据，shader才能对其产生作用
		if (_options->_bShaderGenerator)
		{
			Registry::shaderGenerator().run(_geometry);
		}

		//如果是局部顶点坐标，则需要进行以下处理
		if (_geometry->_eVertexCoordType == eVertexCoordTypeLocal)
		{
			osg::Vec3d poiRotate = osg::Vec3d(CSphereHelp::ToRadian(_options->_feature->getLocation().dblX),CSphereHelp::ToRadian(_options->_feature->getLocation().dblY),0);
			osg::ref_ptr<osg::MatrixTransform> trans=new osg::MatrixTransform();
			trans->setMatrix(osg::Matrix::rotate(-poiRotate.y() + M_PI * 0.5,1,0,0) * osg::Matrix::rotate(poiRotate.x() - M_PI * 1.5,0,0,1));
			osg::Matrix vertialGroundMatrix = trans->getMatrix();

			setMatrix(vertialGroundMatrix * osg::Matrix::translate(_options->_centerPos) );
		}
		else
			setMatrix(osg::Matrix::translate(_options->_centerPos) );
	}

	if (_options->_serviceUrl!="")
		_options->_cache->commitCache("");

	_mapTexturePkgImage.clear();
}

GeoGlobe::Model::Model::~Model()
{
	_mapTexturePkgImage.clear();
}

bool GeoGlobe::Model::Model::parseTexturePkg(UID64 nTexturePkgID)
{
	int   dataLen=0;
	char* temp=NULL;
	if(_options->GetTexDataFromDB(nTexturePkgID,dataLen,&temp))
	{
		Buffer buf;
		buf.set(temp + 5,dataLen - 5);
		/// 读取纹理块头
		CT3D::Material nMaterial;
		//--------------------------------------------------------------
		buf.read((char*)&nMaterial.m_bVersionLength,sizeof(byte));
		if( nMaterial.m_bVersionLength == 6 )
		{
			char * sMaterialVersion = new char[nMaterial.m_bVersionLength +1];
			if (sMaterialVersion == NULL)
			{
				delete []temp;
				return false;
			}
			sMaterialVersion[nMaterial.m_bVersionLength] = '\0';
			buf.read(sMaterialVersion,nMaterial.m_bVersionLength);
			string strVersion(sMaterialVersion);
			if(strVersion.find("GMDL") != string::npos)
			{
				nMaterial.m_sMaterialVersion = strVersion;
			}
			else
			{
				buf.setPos(buf.getPos() - 1 - nMaterial.m_bVersionLength);
			}
			if ( sMaterialVersion )
			{
				delete []sMaterialVersion;
				sMaterialVersion = 0;
			}
		}
		else
		{
			buf.setPos(buf.getPos() - 1);
		}
		//--------------------------------------------------------------
		//读取材质ID
		buf.read((char*)&nMaterial.unMaterialID,sizeof(UINT64));

		//读取材质类型
		buf.read((char*)&nMaterial.byMaterialType,sizeof(byte));

		//读取材质名称长度
		buf.read((char*)&nMaterial.nMaterialNameLen,sizeof(int));

		if(nMaterial.nMaterialNameLen > sizeof(nMaterial.caMaterialName)/sizeof(char))
		{
			delete []temp;
			return false;
		}

		//读取材质名称
		buf.read(nMaterial.caMaterialName,nMaterial.nMaterialNameLen);

		//读取材质数据体长度
		buf.read((char*)&nMaterial.nMaterialDataLen,sizeof(int));

		//纹理包中纹理的个数
		int nTextureCount = *((int*)buf.getCurPtr());
		buf.setPos(buf.getPos()+4);

		for(int nTex = 0; nTex < nTextureCount; nTex++)
		{
			UID64 texID =  *((UID64*)buf.getCurPtr());
			buf.setPos(buf.getPos()+8);

			int nSize =  *((int*)buf.getCurPtr());
			buf.setPos(buf.getPos()+4);

			//构建材质数据的buf
			Buffer tempBuf(buf.getCurPtr(), nSize);
			buf.setPos(buf.getPos()+nSize);

			//解析纹理
			osg::ref_ptr<osg::Image> image = getImageFromBuffer(tempBuf,texID);
			if (!image.valid())
			{
				delete []temp;
				return false;
			}

			_mapTexturePkgImage[texID]=image;
		} 
	}

	if (temp)
		delete []temp;

	return true;
}

bool GeoGlobe::Model::Model::initModel(bool first)
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
					++oitr;
				}
			}
		}
	}
	else
	{
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

#if 1

osg::ref_ptr<GeoGlobe::Model::Geometry> GeoGlobe::Model::Model::createGeometry(ULONGLONG& geoKey)
{
	//	 OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_options->_shared->_geoMutex);

	std::string geometryName=_options->createGeometryName(geoKey);
	osg::ref_ptr<Geometry> geometry;
	osg::ref_ptr<osg::Object> obj=GeoGlobe::Tool::GlobalSetting::instance()->_memCache->getGeometryFromCache(geometryName);
	geometry=(Geometry*)(obj.get());

	if (!geometry.valid())
	{
		geometry=new Geometry( _options);

		if (/*false&&*/geometry.valid()&&geometry->hasRealData())
		{

			GeoGlobe::Tool::GlobalSetting::instance()->_memCache->addGeometryToCache(geometryName,geometry);
			return geometry.get();
		}
	}
	else
		return geometry.get();
	return NULL;
}

#if 0
bool GeoGlobe::Model::Model::parseC3DMaterials(C3D_MATERIALS& c3dMaterials)
{
	for (int i=0;i<c3dMaterials.size();i++)
	{
		if (c3dMaterials[i]==NULL)
			return false;

		osg::ref_ptr<osg::StateSet> stateset;
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
					std::string matName=_options->createTextureName(nTexId);
					bool find=false;
					stateset=GeoGlobe::Tool::GlobalSetting::instance()->_memCache->getStateSet(matName,find);
					if (!find)
					{
						texture=createTexture(nTexId);
						if (texture->getImage()->isImageTranslucent())
						{
							stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
							stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
						}
						stateset->setTextureAttributeAndModes(0, texture,osg::StateAttribute::PROTECTED|osg::StateAttribute::ON );
					}
				}
			}
			break;
		case 2:
			{
				ComMat_PARAM commat_param = c3dMaterials[i]->getComPlexMat();
				if (commat_param.pComm_item != NULL)
				{
					for(int i = 0; i < commat_param.nItemNum; i++)
					{
						COMMAT_ITEM& commat_item =  commat_param.pComm_item[i];

						int texNum=_options->_setting->_multTex?commat_item.nItemNum:1;

						for(int j = 0; j < texNum; j++)
						{
							TEXTURE_ITEM& texture_item = commat_item.pComm_item[j];
							UID64 nTexId = texture_item.nTextureID;

							osg::ref_ptr<osg::Texture2D> texture;
							std::string matName=_options->createTextureName(nTexId);
							bool find=false;
							stateset=GeoGlobe::Tool::GlobalSetting::instance()->_memCache->getStateSet(matName,find);
							if (!find)
							{
								texture=createTexture(nTexId);
								if (texture->getImage()->isImageTranslucent())
								{
									stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
									stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
								}
								stateset->setTextureAttributeAndModes(texture_item.nTexIndex, texture,osg::StateAttribute::PROTECTED|osg::StateAttribute::ON );
							}
						}
					}
				}
			}
			break;
		default:
			break;
		}

		if (stateset.valid())
		{
			stateset->setTextureAttributeAndModes(/*texture_item.nTexIndex*/1, NULL,osg::StateAttribute::PROTECTED|osg::StateAttribute::OFF );
			_textures.push_back(stateset);
		}
	}
	//addStateSet(lod,vec);
	return true;
}
#endif 

bool GeoGlobe::Model::Model::parseC3DMaterials(C3D_MATERIALS& c3dMaterials)
{
	for (int i=0;i<c3dMaterials.size();i++)
	{
		if (c3dMaterials[i]==NULL)
			return false;

		osg::ref_ptr<osg::StateSet> stateset=new osg::StateSet;
		if (!stateset.valid())
			return false;

		int nMatType = c3dMaterials[i]->getMaterialType();
		switch(nMatType)
		{
			case 0:
				break;
			case 1:
				{
					UID64 nTexId = c3dMaterials[i]->getSimpleMat().nTextureID;
					//osg::ref_ptr<osg::Texture2D> texture;
					if (Invalid_ID != nTexId)
					{
						nTexId = IDConverter::MaterialLodIDToMatID(nTexId, 0);
						std::string matName=_options->createTextureName(nTexId);
						GeoGlobe::Tool::MemCache* memCache=GeoGlobe::Tool::GlobalSetting::instance()->_memCache;
						osg::ref_ptr<osg::Texture2D> texture=(osg::Texture2D*)(memCache->getTextureFromCache(matName).get());

						if(texture==NULL)
						{
							texture=createTexture(nTexId);
							if (!texture.valid())
								return false;

							memCache->addTextureToCache(matName,texture);
						}

						if (texture)
						{
							if (texture->getName()=="Alpha")
							{
								stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
								stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
							}
							stateset->setTextureAttributeAndModes(0, texture,osg::StateAttribute::PROTECTED|osg::StateAttribute::ON );
						}
					}
				}
				break;
			case 2:
				{
					ComMat_PARAM commat_param = c3dMaterials[i]->getComPlexMat();
					if (commat_param.pComm_item != NULL)
					{
						for(int m = 0; m < commat_param.nItemNum; m++)
						{
							COMMAT_ITEM& commat_item =  commat_param.pComm_item[m];

							int texNum=_options->_setting->_multTex?commat_item.nItemNum:1;

							for(int n = 0; n < texNum; n++)
							{
								TEXTURE_ITEM& texture_item = commat_item.pComm_item[n];
								UID64 nTexId = texture_item.nTextureID;					
								GeoGlobe::Tool::MemCache* memCache=GeoGlobe::Tool::GlobalSetting::instance()->_memCache;
								std::string matName=_options->createTextureName(nTexId);
								osg::ref_ptr<osg::Texture2D> texture=(osg::Texture2D*)memCache->getTextureFromCache(matName).get();

								if(texture==NULL)
								{
									texture=createTexture(nTexId);
									if (!texture.valid())
										return false;

									memCache->addTextureToCache(matName,texture);
								}

								if (texture)
								{
									if (texture->getName()=="Alpha")
									{
										stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
										stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
									}
								}
								//北京模型数据有部分区域Index不对，导致此处崩溃，暂时作保护
								if (texture_item.nTexIndex > 1000)
									break;
								stateset->setTextureAttributeAndModes(texture_item.nTexIndex, texture,osg::StateAttribute::PROTECTED|osg::StateAttribute::ON );
							}
						}
					}
				}
				break;
			default:
				break;
		}
		_textures.push_back(stateset);
	}
	return true;
}


osg::Image* GeoGlobe::Model::Model::createImageFromService(UID64& nTexId)
{
	//return NULL;
	if (_options->_serviceUrl!="")
	{
		//_drawables.clear();
		//_lodMaterials.clear();
		int   dataLen=0;
		char* temp=NULL;

		char* data;

		/*			std::string textureFile=Stringify() <<"Texture/"<<nTexId<<".texture";
		textureFile+=_options->_cachePath+"/";*/


		if(!_options->_cache->_texCache.QuerySingleKeyValueTable("ModelTexture",nTexId,dataLen,&data))
		{
			std::string texData;
			std::string texURL=createTextureURL(nTexId);
			if (osgEarth::Registry::instance()->isBlacklisted(texURL))
				return NULL;


			Download dl;

			//texData=osgEarth::URI(texURL.c_str()).getString();
			//dataLen=texData.size();
			if (dl.downloadData(texURL))
			{
				//_options->_cache->writeDataToCache(textureFile,texData.);
				std::string textureFile=_options->createTextureName(nTexId);
				_options->_cache->writeDataToCache(textureFile,dl.size,const_cast<char *>(dl.data));
				/*temp=(char*)(texData.c_str());
				_options->_modelCache->writeTexDataToCache(nTexId, temp, dataLen);*/
			}
			else
			{
				osgEarth::Registry::instance()->blacklist(texURL);
				return NULL;
			}


			Buffer texBuf= Buffer((char*)(dl.data) + 7 + 4, dl.size - 11 - 9);
			return getImageFromBuffer(texBuf,nTexId);
		}
		else
		{
			Buffer texBuf= Buffer(data+ 7 + 4, dataLen - 11 - 9);
			osg::ref_ptr<osg::Image> image=getImageFromBuffer(texBuf,nTexId);
			delete []data;
			if (image)
				return image.release();
			else
				return NULL;

		}
	}
	return NULL;
}

osg::ref_ptr<osg::Texture2D> GeoGlobe::Model::Model::createTexture(UID64 nTexId)
{
	osg::ref_ptr<osg::Texture2D> texture=new osg::Texture2D;
	if (!texture.valid())
		return NULL;

	texture->setWrap(osg::Texture2D::WRAP_R, osg::Texture::REPEAT);
	texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture::REPEAT);
	texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture::REPEAT);
	osg::ref_ptr<osg::Image> image;
	image=createImage(nTexId);

	if(image)
	{
		texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
		texture->setImage(image);

		if(image->isImageTranslucent())
		{
			texture->setName("Alpha");
		}
		texture->setUnRefImageDataAfterApply( true );
		//OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_options->_shared->_texMutex);
	}
	else
	{
		image=ImageUtils::createEmptyImage();
		texture->setImage(image);
		cout<<"invalid image"<<endl;
	}
	return texture;
}
#endif


#if 0
osg::ref_ptr<Geometry> GeoGlobe::Model::Model::createGeometry(ULONGLONG& geoKey)
{
	//	 OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_options->_shared->_geoMutex);

	std::string geometryName=_options->createGeometryName(geoKey);
	osg::ref_ptr<Geometry> geometry;
	/*	geometry=(Geometry*) osgDB::Registry::instance()->getFromObjectCache(geometryName);*/

	osg::ref_ptr<osg::Object> obj=( osgDB::Registry::instance()->getRefFromObjectCache(geometryName));
	geometry=(Geometry*)(obj.get());

	if (!geometry.valid())
	{
		geometry=new Geometry( _options);

		if (/*false&&*/geometry.valid()&&geometry->hasRealData())
		{


			osg::ref_ptr<Geometry> a;
			a=(Geometry*) osgDB::Registry::instance()->getFromObjectCache(geometryName);
			if (a.valid())
			{
				cout<<"find geo repeat"<<endl;
			}
			//			osgDB::Registry::instance()->addEntryToObjectCache(geometryName,_geometry);*/

			osgDB::Registry::instance()->addEntryToObjectCache(geometryName,geometry);
			return geometry.get();
		}
	}
	else
		return geometry.get();
	return NULL;
}

bool GeoGlobe::Model::Model::parseC3DMaterials(C3D_MATERIALS& c3dMaterials)
{
	for (int i=0;i<c3dMaterials.size();i++)
	{
		osg::ref_ptr<osg::StateSet> stateset=new osg::StateSet;

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
						}
					}
				}
			}
			break;
		default:
			break;
		}
		_textures.push_back(stateset);
	}
	//addStateSet(lod,vec);
	return true;
}

osg::ref_ptr<osg::Texture2D> GeoGlobe::Model::Model::createTexture(UID64 nTexId)
{
	//__android_log_print(ANDROID_LOG_INFO, "Model::createTexture  %s", "start");
	bool  firstLoad=false;
	osg::ref_ptr<osg::Texture2D> texture=createTexture(nTexId,firstLoad);
	if (firstLoad)
	{
		osg::ref_ptr<osg::Image> image;
		image=createImage(nTexId);
		if(image)
		{
			texture->setUnRefImageDataAfterApply(true);
			texture->setImage(image);
			image->setName(texture->getName());
			if (ImageUtils::isPowerOfTwo( image ) &&
				!(!image->isMipmap() && ImageUtils::isCompressed(image)) )
			{
				texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
			}
		}
		else
		{
			image=ImageUtils::createEmptyImage();
			texture->setImage(image);
			cout<<"invalid image"<<endl;
		}
	}
	//__android_log_print(ANDROID_LOG_INFO, "Model::createTexture  %s", "end");
	return texture;
}

osg::Image* GeoGlobe::Model::Model::createImageFromService(UID64& nTexId)
{
	//return NULL;
	if (_options->_serviceUrl!="")
	{
		//_drawables.clear();
		//_lodMaterials.clear();
		int   dataLen=0;
		char* temp=NULL;

		char* data;

		/*			std::string textureFile=Stringify() <<"Texture/"<<nTexId<<".texture";
		textureFile+=_options->_cachePath+"/";*/
		std::string textureFile=_options->createTextureName(nTexId);

		if(!_options->_cache->readDataFromCache(textureFile,dataLen,&data))
		{
			std::string texData;
			std::string texURL=createTextureURL(nTexId);
			if (osgEarth::Registry::instance()->isBlacklisted(texURL))
				return NULL;


			Download dl;

			//texData=osgEarth::URI(texURL.c_str()).getString();
			//dataLen=texData.size();
			if (dl.downloadData(texURL))
			{
				//_options->_cache->writeDataToCache(textureFile,texData.);
				_options->_cache->writeDataToCache(textureFile,dl.size,const_cast<char *>(dl.data));
				/*temp=(char*)(texData.c_str());
				_options->_modelCache->writeTexDataToCache(nTexId, temp, dataLen);*/
			}
			else
			{
				osgEarth::Registry::instance()->blacklist(texURL);
				return NULL;
			}


			Buffer texBuf= Buffer((char*)(dl.data) + 7 + 4, dl.size - 11 - 9);
			return getImageFromBuffer(texBuf,nTexId);
		}
		else
		{
			Buffer texBuf= Buffer(data+ 7 + 4, dataLen - 11 - 9);
			osg::ref_ptr<osg::Image> image=getImageFromBuffer(texBuf,nTexId);
			delete []data;
			if (image)
				return image.release();
			else
				return NULL;
		}
	}
	return NULL;
}
#endif
osg::Image* GeoGlobe::Model::Model::createImageFromLocal(UID64& nTexId)
{
	std::map<UID64,osg::ref_ptr<osg::Image>>::iterator it = _mapTexturePkgImage.find(nTexId);
	if (it != _mapTexturePkgImage.end())
	{
		return it->second.release();
	}
	int   dataLen=0;
	char* temp=NULL;
	if(_options->GetTexDataFromDB(nTexId,dataLen,&temp))
	{
		//texBuffer.set(texData+5,nRealDataSize-5,true);
		Buffer texBuf= Buffer(temp + 5, dataLen - 5/*,true*/);
		//delete temp;
		osg::ref_ptr<osg::Image> image = getImageFromBuffer(texBuf,nTexId);
		delete []temp;
		return image.release();
	}

	if (temp)
	{
		delete []temp;
	}
	return NULL;
}

			

GeoGlobe::Model::Model::ImageType GeoGlobe::Model::Model::image_type(const char* imageData)  
{  
	unsigned char png_type[9] = {0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A,'/0'};
	unsigned char tga_type[12] = {0,0,2,0,0,0,0,0,0,0,0,0};
	unsigned char file_head[9];  

	for (int i=0;i<8;++i)  
	{  
		file_head[i] = imageData[i];  
	}  

	file_head[8] = '/0';  

	switch (file_head[0])  
	{  
	case 0xff:  
		if (file_head[1]==0xd8)  
			return jpg;//jpeg  
	case 0x42:  
		if (file_head[1]==0x4D)  
			return bmp;//bmp  
	case 0x89:  
		if (file_head[1]==png_type[1] && file_head[2]==png_type[2] && file_head[3]==png_type[3] && file_head[4]==png_type[4]&&  
			file_head[5]==png_type[5] && file_head[6]==png_type[6] && file_head[7]==png_type[7])  
			return png;//png
	case 0:  
		if (memcmp(tga_type,imageData,sizeof(12))==0)
			return tga;//tga
		return tga;
	default:  
		return nothing;  
	} 

	return nothing;  
}  


osg::Image* GeoGlobe::Model::Model::getImageFromBuffer(Buffer& texBuf,UID64& nTexId)
{
	//	return NULL;
	CMaterial pMaterial;
	if (!pMaterial.readBuffer(texBuf)||0!= pMaterial.getMaterialType())
		return NULL;

	if (0 != pMaterial.getMaterialType())
		return NULL;

	UID64 nTextureIndexFromID = nTexId >> 56;
	std::string string;
	if (nTextureIndexFromID != 1)
	{
		string.assign((char*)texBuf.getCurPtr(),texBuf.size() - texBuf.getPos());
	}
	else
	{
		string.assign((char*)texBuf.getCurPtr(),texBuf.size() - texBuf.getPos() + 1);
	}
	std::stringstream stream;
	stream<<string;
	ImageType imtype = image_type((char*)texBuf.getCurPtr());
	std::string ext=osgDB::getFileExtension(pMaterial.getMaterialName());
	osg::ref_ptr<osgDB::ReaderWriter> rw = osgDB::Registry::instance()->getReaderWriterForExtension( ext );
	if(!rw.valid())
	{
		switch (imtype)
		{
		case jpg:
			{
				std::string strext=osgDB::getFileExtension("0.JPG");
				rw = osgDB::Registry::instance()->getReaderWriterForExtension( strext );
				break;
			}
		case bmp:
			{
				std::string strext=osgDB::getFileExtension("0.BMP");
				rw = osgDB::Registry::instance()->getReaderWriterForExtension( strext );
				break;
			}
		case png:
			{
				std::string strext=osgDB::getFileExtension("0.PNG");
				rw = osgDB::Registry::instance()->getReaderWriterForExtension( strext );
				break;
			}
		case tga:
			{
				std::string strext=osgDB::getFileExtension("0.tga");
				rw = osgDB::Registry::instance()->getReaderWriterForExtension( strext );
				break;
			}
		default:  
				break;  
		}
	}

	if(!rw.valid())
		return NULL;

	osg::ref_ptr<osgDB::Options> op=new osgDB::Options;
	if (!op.valid())
		return NULL;

	op->setOptionString("dds_flip");
	osg::ref_ptr<osg::Image> image = rw->readImage(stream,op).getImage();
	if(image.valid())
		return image.release();
	else
	{
	//__android_log_print(ANDROID_LOG_ERROR, "getImageFromBuffer", "image FAIL");
		return NULL;
	}
}

osg::Image* GeoGlobe::Model::Model::createImage(UID64& nTexId)
{
	//return NULL;
	//__android_log_print(ANDROID_LOG_INFO, "Model::createImage", "start");
	if(_options->_serviceUrl=="")
	{
		return createImageFromLocal(nTexId);
	}
	else
	{
		return createImageFromService(nTexId);
	}
	//__android_log_print(ANDROID_LOG_INFO, "Model::createImage", "end");

}

std::string GeoGlobe::Model::Model::createTextureURL(UID64 nTexId)
{

	//http://172.17.1.224:8088/shanxiModel/services/tile/GetTexture?Texture=1.jpg:12345

	std::string texURL=_options->_serviceUrl;
	std::stringstream buf;
	buf<<"/GetTexture?Texture="<<nTexId;
	texURL+=buf.str();
	return texURL;
	//return "";
}


osg::ref_ptr<osg::Texture2D> GeoGlobe::Model::Model::createTexture(UID64 nTexId,bool& firstLoad)
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_options->_shared->_texMutex);
	firstLoad = false;
	std::string textureName=_options->createTextureName(nTexId);
	osg::ref_ptr<osg::Object> obj=( osgDB::Registry::instance()->getRefFromObjectCache(textureName));

	osg::ref_ptr<osg::Texture2D> texture=(osg::Texture2D*)(obj.get());
	if (!texture.valid())
	{
		texture = new osg::Texture2D();
		texture->setName(textureName);
		texture->setWrap(osg::Texture2D::WRAP_R, osg::Texture::REPEAT);
		texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture::REPEAT);
		texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture::REPEAT);
		osgDB::Registry::instance()->addEntryToObjectCache(textureName,texture.get());
		firstLoad=true;


	}
	//OpenThreads::ScopedLock<OpenThreads::Mutex> unlock(_options->_shared->_texMutex);
	return texture;
}

osg::ref_ptr<osg::Texture2D> GeoGlobe::Model::Model::createTexture2(UID64 nTexId)
{
	//	 OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_options->_shared->_texMutex);

	//			return NULL;
	//osg::Texture2D* texture;
	//std::string texDBPath=dbPath+"C:/Users/Administrator/Desktop/jinyang/jianzhu/NoDefine";
	/*		std::stringstream ct3dTexName;
	ct3dTexName<<_options->_texDBName.c_str()<<"."<<nTexId<<".ct3d_tex";*/
	//ct3dTexName<<texDBPath.c_str()<<"."<<nTexId<<".ct3d_tex";
	//std::string textureName=createTextureName(_options->_modelCache->_tileDBName,nTexId);
	std::string textureName=_options->createTextureName(nTexId);
	//	osg::ref_ptr<osg::Texture2D> texture=(osg::Texture2D*)( osgDB::Registry::instance()->getFromObjectCache(textureName));
	osg::ref_ptr<osg::Object> obj=( osgDB::Registry::instance()->getRefFromObjectCache(textureName));
	osg::ref_ptr<osg::Texture2D> texture=(osg::Texture2D*)(obj.get());
	//osg::ref_ptr<osg::Image> image=(osg::Image*)( osgDB::Registry::instance()->getFromObjectCache(textureName));
	if (!texture.valid())
	{
		texture = new osg::Texture2D();
		//osgDB::Registry::instance()->addEntryToObjectCache(textureName,texture);
		//osg::ref_ptr<LodModelOptions> lodModelOptions=new LodModelOptions(_options,itr->second.get());
		osg::ref_ptr<osg::Image> image;
		image=createImage(nTexId);
		if(image)
		{
			//osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image );
			texture->setImage(image);
			texture->setWrap(osg::Texture2D::WRAP_R, osg::Texture::REPEAT);
			texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture::REPEAT);
			texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture::REPEAT);
			//OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_options->_shared->_texMutex);
			/*					osg::ref_ptr<osg::Texture2D> texture=(osg::Texture2D*)( osgDB::Registry::instance()->getFromObjectCache(textureName));
			if(!texture.valid())*/
			/*cout<<"find tex repeat"<<endl;*/
			osgDB::Registry::instance()->addEntryToObjectCache(textureName,texture);
			//OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_options->_shared->_texMutex);
			//return texture;
		}
		else
		{
			image=ImageUtils::createEmptyImage();
			texture->setImage(image);
			cout<<"invalid image"<<endl;
		}
		//image=new LodModel(/*_options*/lodModelOptions);


		////
	}
	//else
	//{
	//	int a=0;
	//}
	return texture;
	//osg::ref_ptr<osgDB::ReaderWriter> reader = osgDB::Registry::instance()->getReaderWriterForExtension("ct3d_tex");
	//osg::ref_ptr<osgDB::Options> options=new osgDB::Options;
	//options->setObjectCacheHint(osgDB::Options::CACHE_IMAGES);
	//	modelTile=(ModelTile2*)(reader->readObject(ct3dTexName.str(),options).getObject());


	//osg::ref_ptr<osg::Image> image=osgDB::readImageFile(ct3dTexName.str(),options);

	/*		if(image)
	{
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image );


	texture->setWrap(osg::Texture2D::WRAP_R, osg::Texture::REPEAT);
	texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture::REPEAT);
	texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture::REPEAT);
	return texture;
	}*/
	//return NULL;
}

void GeoGlobe::Model::Model::traverse(NodeVisitor& nv)
{
	if (nv.getTraversalMode() == nv.TRAVERSE_ALL_CHILDREN )
		return;

	/*	if (nv.getVisitorType() == nv.CULL_VISITOR  )
	{*/
	//return;
	//initModel();
	//不用在每次traverse的时候设置Textures，只需要在初始化的时候设置即可
	if(_geometry!=NULL/*&&_geometry->setTextures(_textures,_options->_lod)*/)	
	{
		//					osgUtil::Optimizer optimizer;
		//					optimizer.optimize(_geometry, osgUtil::Optimizer::MERGE_GEOMETRY);
		/*			if (selected)
		{
		_geometry->setSelect();
		}
		else
		{
		_geometry->setTextures(_textures,_options->_lod);
		}*/

		//_geometry->setSelect();

		_geometry->accept(nv);
	}
	/*}*/
	/*		if(_geometry!=NULL&&_geometry->setSelect())
	_geometry->accept(nv);*/
	//_progress->update( nv.getFrameStamp()->getFrameNumber() );
	/*	}
	else
	{
	if(_geometry!=NULL)
	_geometry->accept(nv);
	}*/

}

