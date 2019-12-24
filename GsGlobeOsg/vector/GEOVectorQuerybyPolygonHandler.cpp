#include "GEOVectorQuerybyPolygonHandler.h"
#include <osgEarthSymbology/Geometry>



using namespace osgEarth::Features;
using namespace osgEarth::Util;
using namespace osgEarth::Util::Controls;
using namespace GeoGlobe::GeoVector;

#define LC "[VectorQueryTool] "

//////////////////////-----------------------------------------------------------
GeoVecFCSQuerybyPolyHandler::GeoVecFCSQuerybyPolyHandler(std::string xmlPath,osg::Group* root,MapNode* mapNode,
	VectorQueryCallback* callbackToAdd) : 
GEOVectorQuerybyPolygonHandler(root,mapNode,callbackToAdd),
	_xmlPath(xmlPath),
	_xmlValid(false)
{
	if (init(_xmlPath))
	{
		_xmlValid = true;
	}
}
bool
    GeoVecFCSQuerybyPolyHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	bool handled = GEOVectorQuerybyPolygonHandler::handle(ea,aa);
	if (!_xmlValid)
	{
		return false;
	}
	if (
		ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK &&
		ea.getModKeyMask() == 0 &&
		ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON
		)
	{
		_dbmouseDown=true;
		if (_worldPoints->size()<1)
		{
			return false;
		}
		getQueryPolygon(_worldPoints,_llhPoints);//此处第一个点与最后一个点相同
		QueryInSqlite(_llhPoints,_attributeTables);
		if (_attributeTables.size()>0)
		{
			VectorQueryCallback::EventArgs args;
			args._ea = &ea;
			args._aa = &aa;
			
			for( VectorQueryCallbacks::iterator i = _vqcallbacks.begin(); i != _vqcallbacks.end();i++ )
			{
				if ( i->valid() )
				{
					//i->get()->onHit(_attributeTables,args);
				}
				else
				{
					i = _vqcallbacks.erase( i );
				}
			}
			handled = true;
		}
		if ( !handled )
		{
			OE_DEBUG << LC << "miss" << std::endl;

			VectorQueryCallback::EventArgs args;
			args._ea = &ea;
			args._aa = &aa;

			for( VectorQueryCallbacks::iterator i = _vqcallbacks.begin(); i != _vqcallbacks.end(); )
			{
				if ( i->valid() )
				{
					i->get()->onMiss( args );
					++i;
				}
				else
				{
					i = _vqcallbacks.erase( i );
				}
			}
		}
	}//double click
	
	return handled;
}

bool 
	GeoVecFCSQuerybyPolyHandler::init(std::string xmlPath)
{
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
	GeoVecFCSQuerybyPolyHandler::QueryInSqlite(osg::Vec3dArray* llhPoints,std::vector<AttributeTable>& attributeTables)
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
	GeoVecFCSQuerybyPolyHandler::createAttributeTable(Config fields,GEOFeatureClass* gfc,int oid,AttributeTable& attr)
{

	AttributeValue& a=attr["layer"];
	a.first = ATTRTYPE_STRING;
	a.second.stringValue = osgDB::getNameLessAllExtensions(osgDB::getSimpleFileName(gfc->getdbPath()));
	a.second.set = true;

	AttributeValue& at=attr["feature_oid"];
	at.first = ATTRTYPE_INT;
	at.second.intValue = oid;
	at.second.set = true;

	for (ConfigSet::const_iterator cfitr=fields.children().begin();cfitr!=fields.children().end();cfitr++)
	{
		Config field = *cfitr;
		std::string fieldname = field.child("name").value();
		AttributeValue& a=attr[fieldname];
		if(field.child("type").value()=="integer"||field.child("type").value()=="smallint")
		{
			a.first=ATTRTYPE_INT;
			a.second.intValue = gfc->excuteFieldSQLint(fieldname,oid);
			a.second.set = true;
		}
		if(field.child("type").value()=="text")
		{
			a.first=ATTRTYPE_STRING;
			a.second.stringValue = gfc->excuteFieldSQLstring(fieldname,oid);
			a.second.set = true;
		}
		if(field.child("type").value()=="real")
		{
			a.first=ATTRTYPE_DOUBLE;
			a.second.doubleValue = gfc->excuteFieldSQLdouble(fieldname,oid);
			a.second.set = true;
		}

	}//for every field
}
void 
	GeoVecFCSQuerybyPolyHandler::getIntersectVectors(osg::Vec3dArray* llhPoints,std::vector<int>& oids,GEOFeatureClass* geoFeaClass)
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
	GeoVecFCSQuerybyPolyHandler::highlightVector(std::vector<int>& oids,GEOFeatureClass* geoFeaClass,osg::Group* root,MapNode* mapNode)
{
	GEOVectorFCSAttributeQuery::highlightLocalVector(oids,geoFeaClass,root,mapNode);
}
//////////////////////-----------------------------------------------------------
GeoVecFCSQuerybyPointHandler::GeoVecFCSQuerybyPointHandler(std::string xmlPath,osg::Group* root,MapNode* mapNode,
	VectorQueryCallback* callbackToAdd) : 
GeoVecFCSQuerybyPolyHandler(xmlPath,root,mapNode,callbackToAdd),
	_dis(2.0)
{
	
}
bool
	GeoVecFCSQuerybyPointHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	
		bool handled = false;
		bool attempt;

		attempt =
			ea.getEventType() == osgGA::GUIEventAdapter::RELEASE &&
			_lmouseDown && 
			fabs(ea.getX()-_mouseDownX) <= 3.0 && 
			fabs(ea.getY()-_mouseDownY) <= 3.0;

		if ( attempt && getMapNode() )
		{
			_worldPoints->clear();
			_llhPoints->clear();

			GEOVectorHighlightCommom::removeHighlightGeode(_root,"highlightGeode");

			
			osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
			MousepstoWorldps(view,osg::Vec2f(ea.getX()-_dis,ea.getY()-_dis),_worldPoints);
			MousepstoWorldps(view,osg::Vec2f(ea.getX()-_dis,ea.getY()+_dis),_worldPoints);
			MousepstoWorldps(view,osg::Vec2f(ea.getX()+_dis,ea.getY()+_dis),_worldPoints);
			MousepstoWorldps(view,osg::Vec2f(ea.getX()+_dis,ea.getY()-_dis),_worldPoints);
			
			
			getQueryPolygon(_worldPoints,_llhPoints);//此处第一个点与最后一个点相同
			QueryInSqlite(_llhPoints,_attributeTables);
			////////////////////////////////get the attributetables
			
			if (_attributeTables.size()>0)
			{
				VectorQueryCallback::EventArgs args;
				args._ea = &ea;
				args._aa = &aa;

				for( VectorQueryCallbacks::iterator i = _vqcallbacks.begin(); i != _vqcallbacks.end();i++ )
				{
					if ( i->valid() )
					{
						i->get()->onHit(_attributeTables,args);
					}
					else
					{
						i = _vqcallbacks.erase( i );
					}
				}
				handled = true;
			}
			if ( !handled )
			{
				OE_DEBUG << LC << "miss" << std::endl;

				VectorQueryCallback::EventArgs args;
				args._ea = &ea;
				args._aa = &aa;

				for( VectorQueryCallbacks::iterator i = _vqcallbacks.begin(); i != _vqcallbacks.end(); )
				{
					if ( i->valid() )
					{
						i->get()->onMiss( args );
						++i;
					}
					else
					{
						i = _vqcallbacks.erase( i );
					}
				}
			}

			_lmouseDown = false;
		}
		else if (
			ea.getEventType()  == osgGA::GUIEventAdapter::PUSH &&
			ea.getModKeyMask() == 0 &&
			ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			_lmouseDown = true;
			_mouseDownX = ea.getX();
			_mouseDownY = ea.getY();
		}

		return handled;
	
};

/////////////////////////////////////-------------------------------------------------------------------
GeoVecWFSQuerybyPolyHandler::GeoVecWFSQuerybyPolyHandler(const GeoGlobe::GeoVector::GEOWFSFeatureOptions& options,
	osg::Group* root,MapNode* mapNode,VectorQueryCallback* callbackToAdd) : 
    GEOVectorQuerybyPolygonHandler(root,mapNode,callbackToAdd)
{
	_gmlFeaClass=NULL;
	_wfsUrl = options.url().get().full();
	_typeName = options.typeName().get();
	_xmlqcPath =  options.cachePath().get() + "/" +_typeName + "querycache.xml";
}

bool
	GeoVecWFSQuerybyPolyHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	bool handled = GEOVectorQuerybyPolygonHandler::handle(ea,aa);
	
	if (
		ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK &&
		ea.getModKeyMask() == 0 &&
		ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON
		)
	{
		_dbmouseDown=true;
		if (_worldPoints->size()<1)
		{
			return false;
		}

		_worldPoints->push_back(*_worldPoints->begin());
		_worldPoints->dirty();

		getQueryPolygon(_worldPoints,_llhPoints);

		//_xmlqcPath="E:/OSG/data/cache/vector/wfsaq.xml";
		//if(getQueryResponseXML(_llhPoints,_xmlqcPath))
		//{
			_xmlqcPath="E:/OSG/data/cache/vector/BOU2_4M_Lquerycache.xml";
			std::ifstream fin(_xmlqcPath.c_str());
			//std::ifstream fin(xmlqcPath.c_str(),ios::binary);
			osg::ref_ptr<XmlDocument> doc = XmlDocument::load(fin);
			if (!doc)
			{
				return false;
			}
			osg::ref_ptr<XmlElement> e_root = static_cast<XmlElement*>(doc->getChildren()[0].get());
			if (!e_root)
			{
				return false;
			}
			_gmlFeaClass = new GEOWFSFeatureClass(e_root);
			highlightVector(_gmlFeaClass);
			AttributeTable attr;
			createAttributeTable(_gmlFeaClass,attr);
			_attributeTables.push_back(attr);
		//}
		
		if (_attributeTables.size()>0)
		{
			VectorQueryCallback::EventArgs args;
			args._ea = &ea;
			args._aa = &aa;

			for( VectorQueryCallbacks::iterator i = _vqcallbacks.begin(); i != _vqcallbacks.end();i++ )
			{
				if ( i->valid() )
				{
					i->get()->onHit(_attributeTables,args);
				}
				else
				{
					i = _vqcallbacks.erase( i );
				}
			}
			handled = true;
		}
		if ( !handled )
		{
			OE_DEBUG << LC << "miss" << std::endl;

			VectorQueryCallback::EventArgs args;
			args._ea = &ea;
			args._aa = &aa;

			for( VectorQueryCallbacks::iterator i = _vqcallbacks.begin(); i != _vqcallbacks.end(); )
			{
				if ( i->valid() )
				{
					i->get()->onMiss( args );
					++i;
				}
				else
				{
					i = _vqcallbacks.erase( i );
				}
			}
		}
	}//double click

	return handled;
}
bool 
	GeoVecWFSQuerybyPolyHandler::getQueryResponseXML(osg::Vec3dArray* llhPoints,std::string& xmlqcPath)
{
	remove(xmlqcPath.c_str());
	if (!osgDB::fileExists(xmlqcPath))
	{
		ReadResult rr = URI(createWFSQueryURL(llhPoints)).readString();
		if (rr.succeeded())
		{
			saveResponse(rr.getString(),xmlqcPath);
			return true;
		}
			
		else
			return false;
	}	
	else
	   return true;
}
std::string
	GeoVecWFSQuerybyPolyHandler::createWFSQueryURL(osg::Vec3dArray* llhPoints)
{
	std::string url;
	std::ostringstream buf;
	buf<<_wfsUrl
		<<"?"
		<<"VERSION=1.0.0"
		<<"&REQUEST=GetFeature"
		<<"&TypeName="<<_typeName
		<<"&Filter="<<createOGCFilter(llhPoints);
	url=buf.str();
	return url;
}
std::string
	GeoVecWFSQuerybyPolyHandler::createOGCFilter(osg::Vec3dArray* llhPoints)
{
	std::stringstream buf;
	//<ogc:Filter xmlns:ogc="http://www.opengis.net/ogc"><osg:Intersects>;
	buf<<"<ogc:Filter xmlns:ogc="
		<<"\""
		<<"http://www.opengis.net/ogc"
		<<"\""
		<<"><ogc:Intersects>";
	//point search
	if((*(llhPoints->begin())==llhPoints->back())&&(llhPoints->size()==2))
	{
		buf<<"<gml:Point xmlns:gml="
			<<"\""
			<<"http://www.opengis.net/gml"
			<<"\""
			<<"><gml:coordinates>"
			<<llhPoints->back().x()
			<<","
			<<llhPoints->back().y()
			<<"</gml:coordinates></gml:Point></ogc:Intersects></ogc:Filter>";
	}
	//polygon search
	/*<ogc:Filter xmlns:ogc="http://www.opengis.net/ogc">
		<ogc:Intersects>
		<ogc:PropertyName>geometry</ogc:PropertyName>
		<gml:Polygon xmlns:gml="http://www.opengis.net/gml">
		<gml:outerBoundaryIs>
		<gml:LinearRing>
		<gml:coordinates decimal="." cs="," ts=" ">123.567,41.364 123.567,42.364 124.567,42.364 124.567,41.364 123.567,41.364</gml:coordinates>
		</gml:LinearRing>
		</gml:outerBoundaryIs>
		</gml:Polygon>
		</ogc:Intersects>
		</ogc:Filter>*/
	if((*(llhPoints->begin())==llhPoints->back())&&(llhPoints->size()>2))
	{
		buf<<"<ogc:PropertyName>geometry</ogc:PropertyName>"
			<<"<gml:Polygon xmlns:gml="
			<<"\""
			<<"http://www.opengis.net/gml"
			<<"\""
            <<"><gml:outerBoundaryIs><gml:LinearRing>"
		   <<"<gml:coordinates decimal="
			<<"\""
			<<"."
			<<"\""
			<<" cs="
			<<"\""
			<<","
			<<"\""
			<<" ts="
			<<"\""
			<<" "
			<<"\""
			<<">";
//			<<"<gml:coordinates decimal="." cs="," ts=" ">";
		for (osg::Vec3dArray::iterator itr=llhPoints->begin();
			itr!=llhPoints->end();
			itr++)
		{
			osg::Vec3d& pt=*itr;
			if (itr==llhPoints->begin())
			{
				buf<<pt.x()<<","<<pt.y();
			}
			else
			{
				buf<<" "<<pt.x()<<","<<pt.y();
			}	
		}
		buf<<"</gml:coordinates></gml:LinearRing></gml:outerBoundaryIs></gml:Polygon></ogc:Intersects></ogc:Filter>";
	}
	return buf.str();
}
void 
	GeoVecWFSQuerybyPolyHandler::saveResponse(const std::string& buffer, const std::string& filename)
{
	if (!osgDB::fileExists(osgDB::getFilePath(filename)) )
		osgDB::makeDirectoryForFile(filename);
	std::ofstream fout;
	fout.open(filename.c_str(), std::ios::out | std::ios::binary);        
	fout.write(buffer.c_str(), buffer.size());        
	fout.close();
}
void 
	GeoVecWFSQuerybyPolyHandler::highlightVector(GEOWFSFeatureClass* gmlFeaClass)
{
	GEOVectorWFSAttributeQuery::highlightWFSVector(gmlFeaClass,_root,_mapNode.get());
}
void 
	GeoVecWFSQuerybyPolyHandler::createAttributeTable(GEOWFSFeatureClass* gmlFeaClass,osgEarth::Features::AttributeTable& attr)
{
	if (!gmlFeaClass)
	{
		return;
	}
	AttributeValue& a=attr["layer"];
	a.first = ATTRTYPE_STRING;
	a.second.stringValue = _typeName;
	a.second.set = true;

	XmlElement* e_featureMember=gmlFeaClass->getRoot()->getSubElement("gml:featuremember");
	if (e_featureMember==NULL)
	{
		return;
	}
	XmlElement* e_feature = static_cast<XmlElement*>(e_featureMember->getChildren()[0].get());
	if (e_feature==NULL)
	{
		return;
	}
	int fid =atoi(e_feature->getSubElementText("oid").c_str());

	AttributeValue& at=attr["feature_oid"];
	at.first = ATTRTYPE_INT;
	at.second.intValue = fid;
	at.second.set = true;

	XmlNodeList featureAttrs=e_feature->getChildren();
	for (XmlNodeList::const_iterator itr=featureAttrs.begin()+2;itr!=featureAttrs.end();itr++)
	{
		XmlElement* e = (XmlElement*)itr->get();
		AttributeValue& a=attr[e->getName()];
		a.first = ATTRTYPE_STRING;
		a.second.stringValue = e->getText();
		a.second.set = true;
	}//for xmlnode

}

/////////////////////--------------------------------------------------------------

VectorReadoutCallback::VectorReadoutCallback(Container* container )
{
	_grid = new Grid();
	_grid->setBackColor( Color(Color::Black,0.7f) );
	container->addControl( _grid );
}

void
	VectorReadoutCallback::onHit( std::vector<AttributeTable>& tables,const EventArgs& args)
{
	clear();
	unsigned n=0;

	for (std::vector<AttributeTable>::const_iterator itr=tables.begin();itr!=tables.end();itr++)
	{
		AttributeTable table = *itr;

		AttributeValue& findlayer = table.find("layer")->second;
		std::string layername=findlayer.getString();
		_grid->setControl( n, 0, new LabelControl("layer",14.0f, Color::Red) );
		_grid->setControl( n+1, 0, new LabelControl(layername,14.0f,Color::White) );

		AttributeValue& findoid = table.find("feature_oid")->second;
		std::string foid=findoid.getString();
		_grid->setControl( n, 1, new LabelControl("feature_oid",14.0f, Color::Red) );
		_grid->setControl( n+1, 1, new LabelControl(foid,14.0f,Color::White) );

		unsigned r=2;
		for (AttributeTable::const_iterator i = table.begin(); i != table.end(); ++i)
		{
			if (i->first!="layer"&&i->first!="feature_oid")
			{
				_grid->setControl( n, r, new LabelControl(i->first,14.0f, Color::Red) );
				_grid->setControl( n+1, r, new LabelControl(i->second.getString(),14.0f,Color::White) );
				r++;
			}
		}
		n=n+2;
	}
	_grid->setVisible( true );

	args._aa->requestRedraw();
}

void
	VectorReadoutCallback::onMiss( const EventArgs& args )
{
	clear();
	args._aa->requestRedraw();
}

void
	VectorReadoutCallback::clear()
{
	_grid->clearControls();
	_grid->setVisible( false );
}
