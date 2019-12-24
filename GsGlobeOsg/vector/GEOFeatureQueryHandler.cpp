#include "stdafx.h"
#include "GEOFeatureQueryHandler.h"

#include <osgEarthFeatures/Feature>
//#include <osgEarthUtil/FeatureQueryTool>
//#include <osgEarth/Pickers>
#include <osgEarth/Registry>
#include <osgEarth/Capabilities>
#include <osgViewer/View>

#include <osgUtil/LineSegmentIntersector>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osg/Depth>
#include <vector>
#define LC "[GEOFeatureQueryTool] "
//#define GEOSQLITE_SCOPED_LOCK GDAL_SCOPED_LOCK

using namespace osgEarth::Features;
using namespace osgEarth::Util;
using namespace osgEarth::Util::Controls;
using namespace GeoGlobe::GeoVector;
//#undef OE_DEBUG
//#define OE_DEBUG OE_INFO

//-----------------------------------------------------------------------

GEOFeatureQueryTool::GEOFeatureQueryTool(
	std::string xmlPath,
	osg::Group* root,
	MapNode*mapNode,
	GEOFeatureQueryTool::Callback* callback) :
_mapNode( mapNode ),
_root(root),
_dis(2.0)
{
	init(xmlPath);
	if ( callback )
		addCallback( callback );
}

void
	GEOFeatureQueryTool::addCallback( GEOFeatureQueryTool::Callback* cb )
{
	if ( cb )
		_callbacks.push_back( cb );
}

void
	GEOFeatureQueryTool::setMapNode( MapNode* mapNode )
{
	_mapNode = mapNode;
}

bool
	GEOFeatureQueryTool::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
	bool handled = false;
	bool attempt;


	attempt =
		ea.getEventType() == osgGA::GUIEventAdapter::RELEASE &&
		_mouseDown && 
		fabs(ea.getX()-_mouseDownX) <= 3.0 && 
		fabs(ea.getY()-_mouseDownY) <= 3.0;


	if ( attempt && getMapNode() )
	{
		_worldPoints->clear();
		_llhPoints->clear();

		GEOVectorHighlightCommom::removeHighlightGeode(_root,"highlightGeode");

		osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
		getQueryPtToPoly(view,osg::Vec2f(ea.getX(),ea.getY()),_worldPoints,_llhPoints);
		
		////////////////////////////////get the attributetables
		QueryInSqlite(_llhPoints,_attributeTables);
		if (_attributeTables.size()>0)
		{
			Callback::EventArgs args;
			args._ea = &ea;
			args._aa = &aa;
			

			for( Callbacks::iterator i = _callbacks.begin(); i != _callbacks.end();i++ )
			{
				if ( i->valid() )
				{
					i->get()->onHit(_attributeTables,args);
				}
				else
				{
					i = _callbacks.erase( i );
				}
			}
			handled = true;
		}
		if ( !handled )
		{
			OE_DEBUG << LC << "miss" << std::endl;

			Callback::EventArgs args;
			args._ea = &ea;
			args._aa = &aa;

			for( Callbacks::iterator i = _callbacks.begin(); i != _callbacks.end(); )
			{
				if ( i->valid() )
				{
					i->get()->onMiss( args );
					++i;
				}
				else
				{
					i = _callbacks.erase( i );
				}
			}
		}

		_mouseDown = false;
	}

	// unmodified left mouse click
	else if (
		ea.getEventType()  == osgGA::GUIEventAdapter::PUSH &&
		ea.getModKeyMask() == 0 &&
		ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		_mouseDown = true;
		_mouseDownX = ea.getX();
		_mouseDownY = ea.getY();
	}

	return handled;
}

bool 
	GEOFeatureQueryTool::init(std::string xmlPath)
{
	_mouseDown=false;
	_worldPoints = new osg::Vec3dArray;
	_llhPoints = new osg::Vec3dArray;

	_geoFeaClasses=NULL;
	if (!osgDB::fileExists(xmlPath))
	{
		OE_NOTICE << "XML not exists " << std::endl;
		return false;
	}

	std::ifstream in(xmlPath.c_str());
	Config doc;
	doc.fromXML(in);
	if (doc.empty()||doc.children().empty())
	{
		OE_NOTICE << "Failed to get XML " << std::endl;
		return false;
	}

	/////////read the geoMap,create the std::map of datasource and style
	Config geoMap = doc.child("geomapfile").child("geomap");

	std::vector<std::string> layerSourceNames;
	Config geoMapLayers = geoMap.child("layers");
	for(ConfigSet::const_iterator i=geoMapLayers.children().begin();i!=geoMapLayers.children().end();++i)
	{
		Config geoFeatureLayer = *i;
		layerSourceNames.push_back(geoFeatureLayer.child("layersourcename").value());
	}
	///////////////////////////
	_dataCount=layerSourceNames.size();
	_geoFeaClasses = new GEOFeatureClass[_dataCount];
	////////////////////////////
	std::string dataRootPath = osgDB::getFilePath(xmlPath);

	Config fdataSources = doc.child("geomapfile").child("geodatasources");
	for (ConfigSet::const_iterator i=fdataSources.children().begin();i!=fdataSources.children().end();++i)
	{
		const osgEarth::Config& dataSource=*i;
		std::string name = dataSource.child("name").value();
		for(int j=0;j<layerSourceNames.size();j++)
		{
			if(name == layerSourceNames[j])
			{
				std::string dname = dataSource.child("dataname").value();
				std::string dataSourcePath = dataRootPath+"/"+dname+".fcs";
				(_geoFeaClasses+j)->init(dataSourcePath);
				break;
			}
		}
	}
	return true;
}

void 
	GEOFeatureQueryTool::ptWintoWorld(osgViewer::View* view,osg::Vec2f& mousePoint,osg::Vec3d& worldPoint)
{
	osg::Vec2f pt = mousePoint;
	if (pt.x()<0)
	{
		pt.x()=0;
	}
	if (pt.y()<0)
	{
		pt.y()=0;
	}
	osgUtil::LineSegmentIntersector::Intersections hits;
	if (view->computeIntersections(pt.x(),pt.y(),hits))
	{
		worldPoint=hits.begin()->getWorldIntersectPoint();
	}
	return;
}
void 
	GEOFeatureQueryTool::ptWorldtoLLH(osgEarth::SpatialReference*srs,osg::Vec3d& worldPoint,osg::Vec3d& llhPoint)
{
	GeoPoint tempt;
	tempt.fromWorld(srs,worldPoint);
	llhPoint=tempt.vec3d();
}
void 
	GEOFeatureQueryTool::getQueryPtToPoly(osgViewer::View* view,osg::Vec2f& mousePoints,osg::Vec3dArray* worldPoints,osg::Vec3dArray*llhPoints)
{
	osg::Vec3d wtem;
	ptWintoWorld(view,osg::Vec2f(mousePoints.x()-_dis,mousePoints.y()-_dis),wtem);
	worldPoints->push_back(wtem);
	ptWintoWorld(view,osg::Vec2f(mousePoints.x()-_dis,mousePoints.y()+_dis),wtem);
	worldPoints->push_back(wtem);
	ptWintoWorld(view,osg::Vec2f(mousePoints.x()+_dis,mousePoints.y()+_dis),wtem);
	worldPoints->push_back(wtem);
	ptWintoWorld(view,osg::Vec2f(mousePoints.x()+_dis,mousePoints.y()-_dis),wtem);
	worldPoints->push_back(wtem);
	
	if (worldPoints->size()>1)
	{
		for (osg::Vec3dArray::iterator itr = worldPoints->begin();
			itr!=worldPoints->end();
			itr++)
		{
			osg::Vec3d& pt=*(itr);
			GeoPoint tempt;
			tempt.fromWorld(_mapNode->getMapSRS(),pt);
			llhPoints->push_back(tempt.vec3d());
		}
		osg::Vec3d pt = *(llhPoints->rbegin());
		if (*(llhPoints->begin())!=pt)
		{
			llhPoints->push_back(*(llhPoints->begin()));
		}
	}
	return;
}
void 
	GEOFeatureQueryTool::getBoundBox(osg::Vec3dArray* llhPoints,osgEarth::Bounds& bds)
{
	double xmin=DBL_MAX;
	double ymin=DBL_MAX;
	double xmax=-DBL_MAX;
	double ymax=-DBL_MAX;
	for (osg::Vec3dArray::iterator itr=llhPoints->begin();
		itr!=llhPoints->end();
		itr++)
	{
		osg::Vec3d& pt=*(itr);
		if (pt.x()<xmin)
		{
			xmin=pt.x();
		}
		if (pt.y()<ymin)
		{
			ymin=pt.y();
		}
		if (pt.x()>xmax)
		{
			xmax=pt.x();
		}
		if (pt.y()>ymax)
		{
			ymax=pt.y();
		}
	}
	bds.set(xmin, ymin, -DBL_MAX, xmax, ymax, DBL_MAX);
	return;
}

void 
	GEOFeatureQueryTool::QueryInSqlite(osg::Vec3dArray* llhPoints,std::vector<AttributeTable>& attributeTables)
{
	attributeTables.clear();

	///////////////////////////////////////////////////////////////
	for (int dataindex=0;dataindex<_dataCount;dataindex++)
	{
		//////////////////////////////////////////////////get the filedinfo
		Config fields = (_geoFeaClasses+dataindex)->_geoFeaMetatable.FiledsInfo.child("fields");

		///////////////////////////////////////////get the query feature oid
		std::vector<int> oids;
		getIntersectVectors(_llhPoints,oids,_geoFeaClasses+dataindex);
		highlightVector(oids,_geoFeaClasses+dataindex,_root,_mapNode.get());
		for(std::vector<int>::iterator i=oids.begin();i!=oids.end();i++)
		{
			int oid=*i;
			AttributeTable attr;
			createAttributeTable(fields,(_geoFeaClasses+dataindex),oid,attr);
			attributeTables.push_back(attr);
		}//for every oid
	}//for every sqlite
}
void 
	GEOFeatureQueryTool::createAttributeTable(Config fields,GEOFeatureClass* gfc,int oid,AttributeTable& attr)
{

	AttributeValue& a=attr["layer"];
	a.first = AttributeType::ATTRTYPE_STRING;
	a.second.stringValue = osgDB::getNameLessAllExtensions(osgDB::getSimpleFileName(gfc->getdbPath()));
	a.second.set = true;

	AttributeValue& at=attr["feature_oid"];
	at.first = AttributeType::ATTRTYPE_INT;
	at.second.intValue = oid;
	at.second.set = true;

	for (ConfigSet::const_iterator cfitr=fields.children().begin();cfitr!=fields.children().end();cfitr++)
	{
		Config field = *cfitr;
		std::string fieldname = field.child("name").value();
		AttributeValue& a=attr[fieldname];
		if(field.child("type").value()=="integer"||field.child("type").value()=="smallint")
		{
			a.first=AttributeType::ATTRTYPE_INT;
			a.second.intValue = gfc->excuteFieldSQLint(fieldname,oid);
			a.second.set = true;
		}
		if(field.child("type").value()=="text")
		{
			a.first=AttributeType::ATTRTYPE_STRING;
			a.second.stringValue = gfc->excuteFieldSQLstring(fieldname,oid);
			a.second.set = true;
		}
		if(field.child("type").value()=="real")
		{
			a.first=AttributeType::ATTRTYPE_DOUBLE;
			a.second.doubleValue = gfc->excuteFieldSQLdouble(fieldname,oid);
			a.second.set = true;
		}

	}//for every field
}
void 
	GEOFeatureQueryTool::getIntersectVectors(osg::Vec3dArray* llhPoints,std::vector<int>& oids,GEOFeatureClass* geoFeaClass)
{
	
	osgEarth::Bounds bds;
	getBoundBox(llhPoints,bds);

	geoFeaClass->excuteSeletSqlandQuerytoGetOID(bds,
		"select oid,xmin,ymin,xmax,ymax from dataindex",
		oids);//不论什么类型，先获得相交的oid

	for (std::vector<int>::iterator itr=oids.begin();itr!=oids.end();)
	{
		bool inter=false;
		int oid = *itr;
		GEOFeatureH gfeature;
		geoFeaClass->createGEOFeatureH(oid,&gfeature);
		GEOGeometryH* geomHandle = gfeature.getGeomH();
		if (gfeature.valid())
		{
			//get the vertexarray,convert the coordinates
			osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray;

			int ptNum = geomHandle->getNumPoints();
			for (int i=0;i<ptNum;i++)
			{
				/*GeoPoint geopt(_mapNode->getMapSRS(),osg::Vec3d(geomHandle->getGeoPoint(i),0.0));
				osg::Vec3d worldPoints;
				geopt.toWorld(worldPoints);*/
				vertices->push_back(osg::Vec3d(geomHandle->getGeoPoint(i),0.0));
			}
			
			////////judge
			geoGEOMETRYTYPE geoType = (geoGEOMETRYTYPE)geomHandle->getGeomType();

			if (geoType == GEO_GEOMETRY_POLYGON)//对于面而言，判断包围盒相互包围，若包围盒相交再做线的相交测试
			{
				if (geoFeaClass->bOidContainObjectBound(oid,bds))//（包围盒）要素包围查询多边形,查询多边形有一个点在要素内，即选中
				{
					for (osg::Vec3dArray::iterator i=llhPoints->begin();
						i!=llhPoints->end();
						i++)
					{
						if (GEOPointInRingTest::piontInAtRing(*i,vertices))
						{
							inter = true;
							break;
						}
					}
				}
				else if (geoFeaClass->bOidInObjectBound(oid,bds))//（包围盒）查询多边形包围要素,要素有一个点在多边形内，即相交，若没有，则一定不相交
				{
					for (osg::Vec3dArray::iterator i=vertices->begin();
						i!=vertices->end();
						i++)
					{
						if (GEOPointInRingTest::piontInAtRing(*i,llhPoints))
						{
							inter = true;
							break;
						}
					}
					
				}
				else//包围盒只是相交但不包含的情况
				{
					int numParts = geomHandle->getNumInfo();
					for (int i=0;i<numParts;i++)
					{
						if (GEOLineIntersectTest::hasLineIntersect(llhPoints,vertices))
						{
							inter = true;
							break;
						}
					}	
				}
				
			}
			else if (geoType == GEO_GEOMETRY_PATH 
				||geoType==GEO_GEOMETRY_LINE
				||geoType == GEO_GEOMETRY_POLYLINE
				||geoType == GEO_GEOMETRY_RING)//线与面相交判断
			{
				int numParts = geomHandle->getNumInfo();
				if (GEOLineIntersectTest::hasLineIntersect(llhPoints,vertices))//相交测试，若相交则一定相交
				{
					inter=true;
				}
				else//不相交，有线要素包含在查询多边形中的情况，判断线要素是否有点在多边形内
				{
					for (osg::Vec3dArray::iterator i=vertices->begin();
						i!=vertices->end();
						i++)
					{
						if (GEOPointInRingTest::piontInAtRing(*i,_llhPoints))
						{
							inter = true;
							break;
						}
					}
				}
				
			}
			else if ( geoType == GEO_GEOMETRY_POINT)//判断点是否在多边形内，垂线法
			{
				for (osg::Vec3dArray::iterator i=vertices->begin();
					i!=vertices->end();
					i++)
				{
					if (GEOPointInRingTest::piontInAtRing(*i,_llhPoints))
					{
						inter = true;
						break;
					}
				}
			}
			else if (
				geoType == GEO_GEOMETRY_COLLECTION ||
				geoType == GEO_GEOMETRY_MULTIPOINT )
			{

			}

		}//if (gfeature.valid())
		if (!inter)
			itr=oids.erase(itr);
		else
			itr++;
	}//for oid,every picked vector
}

void 
	GEOFeatureQueryTool::highlightVector(std::vector<int>& oids,GEOFeatureClass* geoFeaClass,osg::Group* root,MapNode* mapNode)
{
	GEOVectorFCSAttributeQuery::highlightLocalVector(oids,geoFeaClass,root,mapNode);
}
//////////////////////---

//////////////////////////////////////////////////////////////////////////////////////////readoutcallback
GEOFeatureReadoutCallback::GEOFeatureReadoutCallback(Container* container )
{
	_grid = new Grid();
	_grid->setBackColor( Color(Color::Black,0.7f) );
	container->addControl( _grid );
}

void
	GEOFeatureReadoutCallback::onHit( std::vector<AttributeTable> tables,const EventArgs& args)
{
	clear();
	unsigned n=0;
	
	for (std::vector<AttributeTable>::const_iterator itr=tables.begin();itr!=tables.end();itr++)
	{
		AttributeTable table = *itr;
		unsigned r=0;
		for (AttributeTable::const_iterator i = table.begin(); i != table.end(); ++i)
		{
			_grid->setControl( n, r, new LabelControl(i->first,14.0f, Color::Red) );
			_grid->setControl( n+1, r, new LabelControl(i->second.getString(),14.0f,Color::White) );
			r++;
		}
		n=n+2;
	}
	_grid->setVisible( true );
	
	args._aa->requestRedraw();
}

void
	GEOFeatureReadoutCallback::onMiss( const EventArgs& args )
{
	clear();
	args._aa->requestRedraw();
}

void
	GEOFeatureReadoutCallback::clear()
{
	_grid->clearControls();
	_grid->setVisible( false );
}
