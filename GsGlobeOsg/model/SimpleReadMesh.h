#include <Model/AddMeshCallback.h>
#ifndef EXTENSION_SIMPLEREADMESH_H
#define EXTENSION_SIMPLEREADMESH_H 1


osg::Group* simpleReadMesh()
{
	osg::Group* meshGroup=new osg::Group;

	std::FILE* fp;
	fp=std::fopen("C:/Users/Administrator/Desktop/meshTest/location.txt","r");
	char s[200];
	fgets(s,1024,fp);


	fgets(s,1024,fp);
	std::vector<std::string> strVector=splitEx(s, ",");
	std::string meshPath="C:/Users/Administrator/Desktop/ive_L0/"+strVector[0]+".ive";
	double lon;
	sscanf(strVector[1].c_str(),"%lf",&lon);
	double lat;
	sscanf(strVector[2].c_str(),"%lf",&lat);
	double altitude;
	sscanf(strVector[3].c_str(),"%lf",&altitude);
	osg::ref_ptr<osg::Node> pageLod=osgDB::readNodeFile(meshPath);
	osg::ref_ptr<osgEarth::SpatialReference> srs = SpatialReference::create( "epsg:4326", "" );
	GeoPoint geoPoint(
		srs, 
		lon, 
		lat, 
		altitude+20,
		ALTMODE_ABSOLUTE );
	osg::Matrixd matrix;
	geoPoint.createLocalToWorld( matrix );


	osg::MatrixTransform* mt = new osg::MatrixTransform;
	mt->setMatrix(/*osg::Matrixd::scale(10,10,10)**/matrix);
	//   osg::MatrixTransform* mt = new osg::MatrixTransform;
	//mt->setMatrix(osg::Matrixd::translate(lon*100000,lat*100000,altitude));
	mt->addChild( pageLod );

	meshGroup->addChild(mt);


	while(fgets(s,1024,fp))
	{

		std::vector<std::string> strVector=splitEx(s, ",");
		//std::string meshPath="C:/Users/Administrator/Desktop/jinci_museum/"+strVector[0]+".obj";
		std::string meshPath="C:/Users/Administrator/Desktop/ive_L0/"+strVector[0]+".ive";
		double lon;
		sscanf(strVector[1].c_str(),"%lf",&lon);
		double lat;
		sscanf(strVector[2].c_str(),"%lf",&lat);
		double altitude;
		sscanf(strVector[3].c_str(),"%lf",&altitude);
		double radius;
		sscanf(strVector[4].c_str(),"%lf",&radius);
		//	osg::ref_ptr<osg::Node> pageLod=osgDB::readNodeFile(meshPath);


		osg::ref_ptr<osg::PagedLOD> pageLod=new osg::PagedLOD();
		pageLod->setRange(0,0,radius*1000);
		pageLod->setFileName(0,meshPath);
		GeoPoint geoPoint(
			srs, 
			lon, 
			lat, 
			altitude+20,
			ALTMODE_ABSOLUTE );
		osg::Matrixd matrix;
		geoPoint.createLocalToWorld( matrix );
		osg::MatrixTransform* mt = new osg::MatrixTransform;
		mt->setMatrix(/*osg::Matrixd::scale(10,10,10)**/matrix);
		mt->addChild( pageLod );

		meshGroup->addChild(mt);


	}
	osg::ref_ptr<osg::StateSet> stateset=meshGroup->getOrCreateStateSet();
	//stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	osg::Material* m = new osg::Material();
	m->setAmbient(m->FRONT_AND_BACK, osg::Vec4(1,1,1,1));
	m->setDiffuse(m->FRONT_AND_BACK, osg::Vec4(1,1,1,1));
	m->setSpecular(m->FRONT_AND_BACK, osg::Vec4(0.1,0.1,0.1,1));
	m->setEmission(m->FRONT_AND_BACK, osg::Vec4(1,1,1,1));
	m->setShininess(m->FRONT_AND_BACK, 32.0);
	stateset->setAttributeAndModes(m, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
	//stateset->setAttributeAndModes( 
	//	new osg::CullFace(), 
	//	osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
	return	meshGroup;
}


#endif 