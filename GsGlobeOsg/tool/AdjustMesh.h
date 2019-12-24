#ifndef OSGEARTHUTIL_ADJUSTMESH_H
#define OSGEARTHUTIL_ADJUSTMESH_H 1

#include <osgEarthUtil/Common>
#include <osgEarth/MapNode>
#include <osgEarth/MapNodeObserver>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthSymbology/Style>
#include <osg/Group>
#include <osgGA/GUIEventHandler>
#include <osgViewer/View>
#include <osgViewer/Viewer>
#include  <osgEarthUtil/EarthManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgEarthUtil/ObjectLocator>

class AdjustMeshHandler:public osgGA::GUIEventHandler
{
public:

	bool _mouseDown ;
	float _mouseDownX ;
	float _mouseDownY ;
	osg::Node::NodeMask _intersectionMask;
	osg::ref_ptr<osgEarth::MapNode> _mapNode;
	osgEarth::Util::ObjectLocatorNode* _locator;
	std::vector<std::string> _meshPaths;
	std::string              _xmlPath;
	osg::ref_ptr<osg::Group>  _root;
	bool _insert;


	AdjustMeshHandler(std::vector<std::string> meshPaths,std::string xmlPath,osg::ref_ptr<osgEarth::MapNode> mapNode,osg::ref_ptr<osg::Group> root)
	{
		_meshPaths=meshPaths;
		_mapNode=mapNode;
		_intersectionMask=0xffffffff;
		_insert=false;
		_xmlPath=xmlPath;
		_root=root;
		InitMesh();
		
	}

	void InitMesh()
	{
		_locator = new osgEarth::Util::ObjectLocatorNode( _mapNode->getMap() );
		_locator->getLocator()->setPosition(osg::Vec3d(0,  0, 0 ) );  

		for (int i=0;i<_meshPaths.size();i++)
		{

			osg::MatrixTransform* mt = new osg::MatrixTransform();
			mt->setMatrix(osg::Matrixd::scale(1,1,1));
			osg::Node* model=osgDB::readNodeFile(_meshPaths[i]);
			mt->addChild(model);
			_locator->addChild( mt );
		}
		_root->addChild(_locator);
		//_mapNode->addChild(_locator);
	}

	bool getLocationAt(osgViewer::View* view, double x, double y, double &lon, double &lat,double &z)
	{
		osgUtil::LineSegmentIntersector::Intersections results;            
		if ( view->computeIntersections( x, y, results, _intersectionMask ) )
		{
			// find the first hit under the mouse:
			osgUtil::LineSegmentIntersector::Intersection first = *(results.begin());
			osg::Vec3d point = first.getWorldIntersectPoint();

			double lat_rad, lon_rad, height;       
			_mapNode->getMap()->getProfile()->getSRS()->getEllipsoid()->convertXYZToLatLongHeight( 
				point.x(), point.y(), point.z(), lat_rad, lon_rad, height );

			lat = osg::RadiansToDegrees( lat_rad );
			lon = osg::RadiansToDegrees( lon_rad );
			z= height;    
			return true;
		}
		return false;
	}

	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
	{

		osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView()); 

		if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN && ea.getKey() == osgGA::GUIEventAdapter::KEY_Control_L)
		{  
			_insert=true;
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::KEYUP && ea.getKey() == osgGA::GUIEventAdapter::KEY_Control_L)
		{
			_insert=false;
		}


		if (_insert&&ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{        
			_mouseDown = true;
			_mouseDownX = ea.getX();
			_mouseDownY = ea.getY();
		}
		else if (_insert&&ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{ 
			if (osg::equivalent(ea.getX(), _mouseDownX) && osg::equivalent(ea.getY(), _mouseDownY))
			{
				double lon, lat,height;
				if (getLocationAt(view, ea.getX(), ea.getY(), lon, lat,height))
				{
					osg::Vec3d local= _locator->getLocator()->getPosition();
					_locator->getLocator()->setPosition(osg::Vec3d(lon,lat,height));
				}
			}
		}

		switch (ea.getEventType())
		{

		case  osgGA::GUIEventAdapter::KEYDOWN:
			{


				switch(ea.getKey())
				{
				case osgGA::GUIEventAdapter::KEY_Left:
					{
						//ResetMapNode(_mapNode);
						//setImageLayerVisible(_mapNode,0,false);
						osg::Vec3d local= _locator->getLocator()->getPosition();
						_locator->getLocator()->setPosition(osg::Vec3d(local.x()-0.00001,local.y(),local.z()));
						return true;
					}
				case  osgGA::GUIEventAdapter::KEY_Right:
					{
						osg::Vec3d local= _locator->getLocator()->getPosition();
						_locator->getLocator()->setPosition(osg::Vec3d(local.x()+0.00001,local.y(),local.z()));
						return true;
					}
				case osgGA::GUIEventAdapter::KEY_Down:
					{
						//ResetMapNode(_mapNode);
						//setImageLayerVisible(_mapNode,0,false);
						osg::Vec3d local= _locator->getLocator()->getPosition();
						_locator->getLocator()->setPosition(osg::Vec3d(local.x(),local.y()-0.00001,local.z()));
						return true;
					}
				case  osgGA::GUIEventAdapter::KEY_Up:
					{
						osg::Vec3d local= _locator->getLocator()->getPosition();
						_locator->getLocator()->setPosition(osg::Vec3d(local.x(),local.y()+0.00001,local.z()));
						return true;
					}
				case  osgGA::GUIEventAdapter::KEY_Page_Up:
					{
						osg::Vec3d orientation= _locator->getLocator()->getOrientation();

						//_locator->getLocator()->setPosition(osg::Vec3d(local.x()+0.0001,local.y(),0));
						_locator->getLocator()->setOrientation(osg::Vec3d(orientation.x()+0.1,orientation.y(),orientation.z()));
						return true;
					}
				case  osgGA::GUIEventAdapter::KEY_Page_Down:
					{
						osg::Vec3d orientation= _locator->getLocator()->getOrientation();

						//_locator->getLocator()->setPosition(osg::Vec3d(local.x()+0.0001,local.y(),0));
						_locator->getLocator()->setOrientation(osg::Vec3d(orientation.x()-0.1,orientation.y(),orientation.z()));
						return true;
					}
				case  osgGA::GUIEventAdapter::KEY_W:
					{
						osg::Vec3d orientation= _locator->getLocator()->getOrientation();
						_locator->getLocator()->setOrientation(osg::Vec3d(orientation.x(),orientation.y()+0.1,orientation.z()));
						return true;
					}
				case  osgGA::GUIEventAdapter::KEY_S:
					{
						osg::Vec3d orientation= _locator->getLocator()->getOrientation();

						//_locator->getLocator()->setPosition(osg::Vec3d(local.x()+0.0001,local.y(),0));
						_locator->getLocator()->setOrientation(osg::Vec3d(orientation.x(),orientation.y()-0.1,orientation.z()));
						return true;
					}
				case  osgGA::GUIEventAdapter::KEY_A:
					{
						osg::Vec3d orientation= _locator->getLocator()->getOrientation();
						_locator->getLocator()->setOrientation(osg::Vec3d(orientation.x(),orientation.y(),orientation.z()+0.1));
						return true;
					}
				case  osgGA::GUIEventAdapter::KEY_D:
					{
						osg::Vec3d orientation= _locator->getLocator()->getOrientation();

						//_locator->getLocator()->setPosition(osg::Vec3d(local.x()+0.0001,local.y(),0));
						_locator->getLocator()->setOrientation(osg::Vec3d(orientation.x(),orientation.y(),orientation.z()-0.1));
						return true;
					}
				case  osgGA::GUIEventAdapter::KEY_Minus:
					{
						osg::Vec3d local= _locator->getLocator()->getPosition();
						_locator->getLocator()->setPosition(osg::Vec3d(local.x(),local.y(),local.z()-1));
						return true;
					}
				case  osgGA::GUIEventAdapter::KEY_Plus:
					{
						osg::Vec3d local= _locator->getLocator()->getPosition();
						_locator->getLocator()->setPosition(osg::Vec3d(local.x(),local.y(),local.z()+1));
						return true;
					}
				case  osgGA::GUIEventAdapter::KEY_Return:
					{

						osg::Vec3d position= _locator->getLocator()->getPosition();
						osg::Vec3d orientation= _locator->getLocator()->getOrientation();
						writeToXml(_xmlPath,_meshPaths,position,orientation);
						return true;
					}


				}

			}
		}
		return false;
	}


	void writeToXml(std::string xmlName,std::vector<std::string> meshNames,osg::Vec3d position,osg::Vec3d orientation)
	{

		//Create the root XML document
		osg::ref_ptr<XmlDocument> doc = new XmlDocument();

		doc->setName( "MeshMeta" );


		doc->getAttrs()["lon"] = toString(toString<double>(position.x() ));
		doc->getAttrs()["lat"] = toString(toString<double>(position.y()  ));
		doc->getAttrs()["height"] = toString(toString<double>(position.z()  ));
		doc->getAttrs()["x_orient"] = toString(toString<double>(orientation.x()  ));
		doc->getAttrs()["y_orient"] = toString(toString<double>(orientation.y()  ));
		doc->getAttrs()["z_orient"] = toString(toString<double>(orientation.z()  ));

		osg::ref_ptr<XmlElement> meshSet = new XmlElement( "MeshSet" );
		doc->getChildren().push_back(meshSet.get() );
		for (int i=0;i<meshNames.size();i++)
		{
			osg::ref_ptr<XmlElement> mesh = new XmlElement( "Mesh" );
			mesh->getAttrs()["meshpath"] = meshNames[i];
			meshSet->getChildren().push_back(mesh.get() );

		}

		std::string path = osgDB::getFilePath(xmlName);

		if (!osgDB::fileExists(path) && !osgDB::makeDirectory(path))
		{
			OE_WARN << "Couldn't create path " << std::endl;
		}
		//std::locale::global(std::locale("Chinese-simplified"));
		std::ofstream out(xmlName.c_str());
		//	out<<"»ÆÎâÃÉ";
		//doc->store(xmlName.c_str());
		doc->store(out);

	}


};
#endif