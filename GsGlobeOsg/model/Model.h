#pragma once
#ifndef GEOGLOBE_MODEL_MODEL
#define GEOGLOBE_MODEL_MODEL 1


//#include <Model/CT3D_TextureDriver.h>
#include <Model/CProcessCallback.h>

#include <Tool/IDConverter.h>
#include <Tool/SQLiteOP.h>
#include <osgEarth/SpatialReference>
#include <osgDB/ReadFile>
#include <osg/Object>
#include <osg/CopyOp>
#include <osg/TexEnvCombine>
#include <osg/MatrixTransform>
#include <osgDB/WriteFile>
#include <Model/Material.h>
#include <Model/ModelOptions.h>
#include <osgEarth/ImageUtils>
#include <osgUtil/Optimizer>
#include <Tool/GlobalSetting.h>
#include <osgUtil/Simplifier>
#include <Model/ModelBaseResource.h>
#include "Tool\cityhash\city.h"
#include "GrowBufferFactory.h"

#ifndef M_PI
	#define M_PI       3.14159265358979323846
#endif

typedef unsigned long long ULONGLONG;

using namespace osg;
using namespace CT3D;
static CGrowBufferFactory g_GrowBufferFactory;
//bool g_ModelRegistryShader = false;

namespace GeoGlobe
{
	namespace Model
	{
		const UID64 Invalid_ID = 0;
		typedef std::vector<osg::ref_ptr<CMaterial> > C3D_MATERIALS;
		typedef std::map<int,std::vector<osg::ref_ptr<CMaterial> > > LOD_C3D_MATERIALS;//Ҫ�ش���LOD��Ϣ�б�

		class CSphereHelp
		{
		public:
			static double ToDegree(double d)
			{
				return d * 180.0 / M_PI;
			}
			static double ToRadian (double d)
			{
				return d * M_PI / 180.0;
			} 
			static bool ToSphere(osg::Vec3d pt,double dblRadius,osg::Vec3d* ptRet)
			{
				//缺省为地球球心
				ptRet->set(0.0,0.0,0.0);
				double dblOutx,dblOuty,dblOutz;
				dblOutz = 0 - dblRadius;
				if(0 == pt.x() && 0 == pt.y() && 0 == pt.z())
					return true;

				//有X，Y，Z，坐标计算出设备点到中心的距离。
				double rho = sqrt(pt.x() * pt.x() + pt.y() * pt.y() + pt.z() * pt.z());

				//Z高度就等于该距离减去地球半径。
				dblOutz = rho - dblRadius;

				//tan(geo) = dy/dx
				dblOutx  = atan2( pt.y(),pt.x());
				dblOutx  = ToDegree(dblOutx);

				dblOuty= asin(pt.z() / rho);
				dblOuty = ToDegree(dblOuty);

				ptRet->set(dblOutx,dblOuty,dblOutz);
				return true;
			}

			static bool ToDKE(osg::Vec3d pt,double dblRadius,osg::Vec3d* ptRet)
			{
				double dblOutx,dblOuty,dblOutz;
				double dblInx,dblIny,dblInz;
				dblInx = pt.x();
				dblIny = pt.y();
				dblInz = pt.z();

				//实际的高程加上地球的半径
				dblInz += dblRadius;
				//X和Y弧度。
				dblIny = ToRadian(dblIny);
				dblInx  = ToRadian(dblInx);

				//Z坐标
				dblOutz = dblInz * sin(dblIny);

				//由维度值计算出坐标点所在Z平面上的长度。
				double dblZLen  = dblInz * cos(dblIny);

				//计算出x和y
				dblOutx = dblZLen * cos(dblInx);
				dblOuty = dblZLen * sin(dblInx);

				ptRet->set(dblOutx,dblOuty,dblOutz);
				return true;
			}

		};

		class Geometry :public osg::Geode
		{
		public:
			const osgEarth::SpatialReference* _srs;
			osg::Vec3d _centerPos;
			LOD_C3D_MATERIALS _lodMaterials;
			int _lod;
			ModelOptions* _options;
			bool _hasRealData;
			GeoGlobe::enumModelVersion _eVersion;
			GeoGlobe::enumVertexCoordType _eVertexCoordType;
			std::map<int, UID64> _mapTexturePkgID;

			//60版本模型特有结构
			vector<D3dxMaterialParam> _vecD3dxMaterialParam;
			unsigned long* _pAdjacencyTable;

			//
			bool _bSetTexture;

			bool hasTexture();


			bool hasRealData();

			bool uncompressGeoData(Buffer& geoBuf);

			bool GetModelGeoDataBuffer(ModelOptions* options,C3DGeometry** c3dGeometry);


			void createGeometryFromLocal();

#if 0
			void createGeometryFromService();
#endif

#if 1
			void createGeometryFromService();
#endif
			Geometry(ModelOptions* options);

			std::string creatGeometryURL(ULONGLONG geoKey);

			bool parseC3DGeometry(C3DGeometry* c3dGeometry);

			void parseC3DSurface(C3DSurface* c3dSurface);

			osg::ref_ptr<osg::Geometry>  createTriangleMesh(C3DTriangleMesh* c3dTriangleMesh) const;
			

			osg::ref_ptr<osg::Geometry>  createTriangleMesh(C3DTriangleMesh* c3dTriangleMesh, D3dxMaterialParam D3dMParam) const;

			double ToDegree(double d);
			bool ToSphere(osg::Vec3d pt,double dblRadius,osg::Vec3d& ptRet);
			C3DGeometry* createC3DGeometry(Buffer& buff);

			bool createMaterials(Buffer& buf);

			bool addMaterial(int nLod, CMaterial* pMaterial);


			void  getMaterials(C3D_MATERIALS& materials,int lod);

			bool setTextures(std::vector<osg::ref_ptr<osg::StateSet> >& textures,int lod);

		};


		class Model : public  osg::MatrixTransform
		{
		public:
			osg::ref_ptr<Geometry>                    _geometry;
			std::vector<osg::ref_ptr<osg::StateSet> > _textures;
			osg::ref_ptr<ModelOptions>               _options;
			bool                                     _init;
			std::multimap<int,int>                   _invalidTexsList;
			std::map<UID64,osg::ref_ptr<osg::Image>> _mapTexturePkgImage;


			Model(ModelOptions* options);

			~Model();

			bool parseTexturePkg(UID64 nTexturePkgID);

			bool initModel(bool first=false);

#if 1

			osg::ref_ptr<Geometry> createGeometry(ULONGLONG& geoKey);

#if 0
			bool parseC3DMaterials(C3D_MATERIALS& c3dMaterials);
#endif 

			bool parseC3DMaterials(C3D_MATERIALS& c3dMaterials);


			osg::Image* createImageFromService(UID64& nTexId);

			osg::ref_ptr<osg::Texture2D> createTexture(UID64 nTexId);
#endif


#if 0
			osg::ref_ptr<Geometry> createGeometry(ULONGLONG& geoKey);

			bool parseC3DMaterials(C3D_MATERIALS& c3dMaterials);

			osg::Image* createImageFromService(UID64& nTexId);
#endif
			osg::Image* createImageFromLocal(UID64& nTexId);

			enum ImageType{  
				jpg,  
				bmp,  
				png,
				tga,
				nothing  
			};  

			ImageType image_type(const char* imageData);


			osg::Image* getImageFromBuffer(Buffer& texBuf,UID64& nTexId);

			osg::Image* createImage(UID64& nTexId);

			std::string createTextureURL(UID64 nTexId);


			osg::ref_ptr<osg::Texture2D> createTexture(UID64 nTexId,bool& firstLoad);

			osg::ref_ptr<osg::Texture2D> createTexture2(UID64 nTexId);

			void traverse(NodeVisitor& nv);

		};
	}

}
#endif
