#include <Model/ReadModelFromSQLite.h>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/LOD>
#include <osg/PagedLOD>

ReadModelFromSQLite::ReadModelFromSQLite(std::string FCDBPath)
{
	
 //std::string FCDBPath="C:/Users/Administrator/Desktop/tile/jincitest_model/jincitest_sql.rel";
   std::string DBDirectory=osgDB::getFilePath(FCDBPath)+"/";
   std::string DBName=osgDB::getStrippedName(FCDBPath);

   _strFCDBPath=FCDBPath;
   _strGeoDBPath=DBDirectory+DBName+".geo";
   _strTexDBPath=DBDirectory+DBName+".tex";
   _srs = osgEarth::SpatialReference::create( "epsg:4326", "" );
}

osg::Node* ReadModelFromSQLite::getOsgNode()
{
	//osg::PagedLOD* root;
	//root=new osg::PagedLOD;
	osg::Group* root;
	root=new osg::Group;


	_pSQLiteFCOP=new CSQLiteOP();
	_pSQLiteFCOP->OpenDataBase(_strFCDBPath.c_str());
	_pSQLiteGeoOP=new CSQLiteOP();
	_pSQLiteGeoOP->OpenDataBase(_strGeoDBPath.c_str());
	_pSQLiteTexOP=new CSQLiteOP();
	_pSQLiteTexOP->OpenDataBase(_strTexDBPath.c_str());

	vector<unsigned long long> vecFKeys;
	 _pSQLiteFCOP->GetFeatureKeysFromFC(FEATURECLASS_TABLE_NAME,vecFKeys);
	//// std::vector<CFeature> vecFeatures;
	 for(int i=0;i</*vecFKeys.size()*/50;i++)
	 {
		 osg::PagedLOD* LodModel;
		 LodModel=new osg::PagedLOD;
		 
		 Buffer   buffer;
		 _pSQLiteFCOP->GetLodByFeatureKey(vecFKeys[i], buffer);
		 CFeature feature;
		 feature.readBuffer(buffer);
		 std::vector<unsigned long long> vecGeoKeys;
		 for (int j=0;j<feature.getLOD_INFOList().size();j++)
		 {

		//	 unsigned char* varData=NULL;
			 Buffer   geoBuffer;
			 unsigned long long GeoKey=feature.getLOD_INFOList()[j].unGeometryID;
			 _pSQLiteGeoOP->GetGeometryByGeoKey(GeoKey,geoBuffer);
			// C3DGroup c3dGroup;
			 //c3dGroup.readBuffer(geoBuffer);
			 C3DGeometry* c3dGeometry=createGeometry(geoBuffer);
			 MATERIAL_MAP mapMaterial;
			 createMaterials(geoBuffer,mapMaterial);
	

			
		
			
	        osg::Node* child=C3DGeometryToOSGNode(/*c3dGroup*/c3dGeometry,feature.getLocation(),mapMaterial);
			
			//root->addChild(child,)
		//	root->addChild(child,1,1);
			LodModel->addChild(child,feature.getLOD_INFOList()[j].dSwitchIn,feature.getLOD_INFOList()[j].dSwitchOut);



			GeoPoint geoPoint(
				_srs, 
				feature.getLocation().dLongitude, 
				feature.getLocation().dLatitude, 
				feature.getLocation().dElevation,
				ALTMODE_ABSOLUTE );
			osg::Matrixd matrix;
			geoPoint.createLocalToWorld( matrix );


			osg::MatrixTransform* mt = new osg::MatrixTransform;
			mt->setMatrix(/*osg::Matrixd::scale(10,10,10)**/matrix);
			//   osg::MatrixTransform* mt = new osg::MatrixTransform;
			//mt->setMatrix(osg::Matrixd::translate(lon*100000,lat*100000,altitude));
			mt->addChild( LodModel );


			root->addChild(mt);


		 }

//		 _mapMaterial.clear();
	/*	 CSQLiteOP *SQLiteGeoOP;
		 SQLiteGeoOP=new CSQLiteOP();
		 SQLiteGeoOP->GetKeysFromKeyValueTable(GEOMETRY_TABLE_NAME,vecGeoKeys);*/
		// vecFeatures.push_back(feature);
		// return root;
	 }

	return root;
}



osg::Drawable* ReadModelFromSQLite::C3DSurfaceToOSGDrawable(C3DSurface* c3dSurface,int surfaceID,Location location,MATERIAL_MAP mapMaterial)
{
	byte byType=c3dSurface->getTypeID();
	if(byType == GEOTYPE_3D_TRIANGLEMESH)	
	{
		//c3dSurface->getMatID()
		//void* vVertexList;
		VersatileVertex3d* pList=(VersatileVertex3d*)((C3DTriangleMesh*)c3dSurface)->m_vVertexList;
		long lVertNum=((C3DTriangleMesh*)c3dSurface)->GetVerticesNum();

		osg::Geometry* osgGeometry=new osg::Geometry;
		osg::Vec3Array* vertices = new osg::Vec3Array ;
		osg::Vec3Array* normals = new osg::Vec3Array ;
		osg::Vec4Array* colors = new osg::Vec4Array ;
		osg::Vec2Array* texcoords = new osg::Vec2Array ;
		//_srs = osgEarth::SpatialReference::create( "epsg:4326", "" );
		osg::Vec3d focalPoint;
		_srs->transformToWorld(osg::Vec3d(location.dLongitude,location.dLatitude,location.dElevation),focalPoint);
		for (int i=0;i<lVertNum;i++)
		{
			osg::Vec3d worldCoord;
			_srs->transformToWorld(osg::Vec3d(pList[i].x,pList[i].y,pList[i].z),worldCoord);
			vertices->push_back(osg::Vec3d(worldCoord.x()-focalPoint.x(),worldCoord.y()-focalPoint.y(),worldCoord.z()-focalPoint.z()));
			normals->push_back(osg::Vec3d(pList[i].nx,pList[i].ny,pList[i].nz));
			texcoords->push_back(osg::Vec2d(pList[i].tu,pList[i].tv));
		//	colors->push_back(osg::Vec4d(pList[i].r,pList[i].g,pList[i].b,1.0f));

		}

		osgGeometry->setVertexArray(vertices);
		osgGeometry->setNormalArray(normals, osg::Array::BIND_PER_VERTEX);
		osgGeometry->setTexCoordArray(0,texcoords);
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
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);

		if (mapMaterial.size()!=0)
		{

		
		
		CMaterial* cMaterial=mapMaterial.begin()->second.at(surfaceID);
		TexIDList texIDList=cMaterial->getTextureID();
	
		for (int i=0;i<texIDList.size();i++)
		{
	/*		if (i>1)
			break;*/
			Buffer texBuffer;
			if(!_pSQLiteTexOP->GetTextureByTexKey(texIDList[i],texBuffer))
				continue;

				CMaterial pMaterial;
				
				if (!pMaterial.readBuffer(texBuffer)||0!= pMaterial.getMaterialType())
					continue;

				std::string string;
				string.assign((char*)texBuffer.getCurPtr(),texBuffer.size() - texBuffer.getPos() + 1);
				std::stringstream stream;
				stream<<string;
				
				std::string ext=osgDB::getFileExtension(pMaterial.getMaterialName());
				osg::ref_ptr<osgDB::ReaderWriter> rw = osgDB::Registry::instance()->getReaderWriterForExtension( ext );
		        osgDB::ReaderWriter::ReadResult rr = rw->readImage(stream);
				osg::Texture2D* texture = new osg::Texture2D( rr.getImage()/*osgDB::readImageFile("C:/Users/Administrator/Desktop/jinci_museum_obj/jchd0111.jpg")*/ );

				texture->setWrap(osg::Texture2D::WRAP_R, osg::Texture::REPEAT);
				texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture::REPEAT);
				texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture::REPEAT);
				stateset->setTextureAttributeAndModes( i, texture,osg::StateAttribute::ON );
				/*			FILE *fimage=fopen("C:/Users/Administrator/Desktop/temp.jpg","wb");
				if (fimage)
				{
				fwrite(texBuffer.getCurPtr(),sizeof(unsigned char),texBuffer.size() - texBuffer.getPos() + 1,fimage);
				fclose(fimage);
				}*/


			//0����������,�������0����
		/*	if (0 != pMaterial.getMaterialType())
				return NULL;*/

		}
		//unsigned long long matID= ((C3DTriangleMesh*)c3dSurface)->getMatID();
		//Buffer texbuffer;
		//_pSQLiteTexOP->GetTextureByTexKey(matID,texbuffer);

		//osgGeometry->setv
		/*&([0]);*/

		//osgGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,lVertNum));
		}
		return osgGeometry;

	} 
}

osg::Node* ReadModelFromSQLite::C3DGeometryToOSGNode(C3DGeometry * c3dGeometry,Location location,MATERIAL_MAP mapMaterial)
{
	byte byType=c3dGeometry->getTypeID();
	if (byType == GEOTYPE_3D_GROUP)
	{
		osg::Group* osgGroup=new osg::Group;

		_GeometryList c3dGeometryList;
		((C3DGroup *)c3dGeometry)->GetCollection(c3dGeometryList);


		for(int i=0;i<c3dGeometryList.size();i++)
		{
			C3DGeometry * c3dGeometry=c3dGeometryList[i].second;
			osgGroup->addChild(C3DGeometryToOSGNode(c3dGeometry,location,mapMaterial));
		}

		return (osg::Node*)osgGroup;
	}
	else if(byType == GEOTYPE_3D_SOLID)
	{
		osg::Geode* osgGeode=new osg::Geode;
		_3DSurfaceList c3dSurfaceList=((C3DSolid *)c3dGeometry)->get3DSurfaceList();

		for(int i=0;i<c3dSurfaceList.size();i++)
		{
			C3DSurface* c3dSurface=c3dSurfaceList[i].second;
			//osgGeode->addChild();
			osgGeode->addDrawable(C3DSurfaceToOSGDrawable(c3dSurface,i,location,mapMaterial));
		}

		return osgGeode;
	}


}


C3DGeometry* ReadModelFromSQLite::createGeometry(Buffer& buff)
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

CMaterial* ReadModelFromSQLite::createMaterials(Buffer& buf,MATERIAL_MAP& mapMaterial)
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
		char nLodLevel = *((char*)buf.getCurPtr());
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


	//		lodMaterialList.insert(make_pair(nLodLevel,pMaterial));
			//CMaterialWrap *pMtl = m_pModelFactory->GetMaterial(nMatID);
			//if (NULL == pMtl)
			//{
			//	//�����������ݵ�buf
			//	Buffer tempBuf(buf.getCurPtr(), nSize);
			//	pMtl = m_pModelFactory->CreateMaterial(nMatID, tempBuf);
			//}

			//if (NULL == pMtl)
			//	return;

			//if (nLodLevel == m_nLodLvl)
			//	pGeometryWrap->SetMaterialByIndex(i, pMtl);

			//m_pFeatureWrap->AddMaterial(nLodLevel, pMtl);
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

bool ReadModelFromSQLite::addMaterial(int nLod, CMaterial* pMaterial,MATERIAL_MAP& mapMaterial)
{

	std::map<int, std::vector<CMaterial*> >::iterator it = mapMaterial.find(nLod);
	if (it != mapMaterial.end())
	{
		it->second.push_back(pMaterial);
		return true;
	}

	std::vector<CMaterial*> vec;
	vec.push_back(pMaterial);
	mapMaterial.insert(std::map<int, std::vector<CMaterial*> >::value_type(nLod,vec));
	return true;

}
