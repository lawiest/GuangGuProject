#include <string.h>
#include <Tool/SQLiteOP.h>
#include <Model/CFeature.h>
#include <Model/CT3D_3DSurface.h>
#include <Model/CT3D_3DSolid.h>
#include <Model/CT3D_3DGeometry.h>
#include <Model/CT3D_3DGroup.h>
#include <Model/CT3D_3DTriangleMesh.h>
#include <Model/CT3D_VersatileVertex3d.h>
#include <osg/Node>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <Model/Material.h>
#include <osgEarth/SpatialReference>
#include <osgEarthAnnotation/PlaceNode>


using namespace osgEarth::Annotation;

#ifndef _MODEL_ReadModelFromSQLite_H_
#define _MODEL_ReadModelFromSQLite_H_


typedef std::map<int,std::vector<CMaterial*>> MATERIAL_MAP;//要素带的LOD信息列表

class ReadModelFromSQLite
{
	

public:
	ReadModelFromSQLite(std::string FCDBPath);
	osg::Node* getOsgNode();
private:
//	CSQLiteOP *SQLiteRelOP;
	CSQLiteOP *_pSQLiteFCOP;
	CSQLiteOP *_pSQLiteGeoOP;
	CSQLiteOP *_pSQLiteTexOP;
	std::string _strFCDBPath;
	std::string _strGeoDBPath;
	std::string _strTexDBPath;
	osg::Node* C3DGeometryToOSGNode(C3DGeometry * c3dGeometry,Location location,MATERIAL_MAP mapMaterial);
	osg::Drawable* C3DSurfaceToOSGDrawable(C3DSurface* c3dSurface,int surfaceID,Location location,MATERIAL_MAP mapMaterial);
	C3DGeometry* createGeometry(Buffer& buff);
	CMaterial* createMaterials(Buffer& buff,MATERIAL_MAP& mapMaterial);
	bool addMaterial(int nLod, CMaterial*,MATERIAL_MAP& mapMaterial);

	C3DGroup c3dGroup;
	std::map<unsigned long long,CMaterial*> materialList;
	 
	osg::ref_ptr<osgEarth::SpatialReference> _srs;
};

#endif 