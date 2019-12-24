#include "stdafx.h"
#include "GEOWFSQueryHandler.h"

#include <osgEarthFeatures/Feature>
//#include <osgEarthUtil/FeatureQueryTool> 
#include <osgEarth/Registry>
#include <osgEarth/Capabilities>
#include <osgViewer/View>

#include <osgUtil/LineSegmentIntersector>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osg/Depth>
#include <vector>
#include <iostream>
#include <osgEarth/XmlUtils>

#define LC "[GEOWFSFeatureQueryTool] "
//#define GEOSQLITE_SCOPED_LOCK GDAL_SCOPED_LOCK

using namespace osgEarth::Features;
using namespace osgEarth::Util;
using namespace osgEarth::Util::Controls;
using namespace GeoGlobe::GeoVector;
//#undef OE_DEBUG
//#define OE_DEBUG OE_INFO

//-----------------------------------------------------------------------

GEOWFSFeatureQueryTool::GEOWFSFeatureQueryTool(
	const GeoGlobe::GeoVector::GEOWFSFeatureOptions& options,
	MapNode*mapNode,
	GEOWFSFeatureQueryTool::Callback* callback) :
_mapNode( mapNode )
{
	_wfsUrl = options.url().get().full();
	_typeName = options.typeName().get();
	_xmlBBoxPathRoot =  options.cachePath().get() + "/" +_typeName;

	if ( callback )
		addCallback( callback );
}

void
	GEOWFSFeatureQueryTool::addCallback( GEOWFSFeatureQueryTool::Callback* cb )
{
	if ( cb )
		_callbacks.push_back( cb );
}

void
	GEOWFSFeatureQueryTool::setMapNode( MapNode* mapNode )
{
	_mapNode = mapNode;
}

bool
	GEOWFSFeatureQueryTool::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
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
		osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
		osg::Vec3d world1,world2;
		GeoPoint mapPoint1,mapPoint2;
		osgUtil::LineSegmentIntersector::Intersections hits;
		if (view->computeIntersections(ea.getX(),ea.getY(),hits))
		{
			world1 = hits.begin()->getWorldIntersectPoint();
			mapPoint1.fromWorld(_mapNode->getMapSRS(),world1);
		}
		if (view->computeIntersections(ea.getX()+100.0,ea.getY()+100.0,hits))
		{
			world2 = hits.begin()->getWorldIntersectPoint();
			mapPoint2.fromWorld(_mapNode->getMapSRS(),world2);
		}
		////////////////////////////////get the attributetables
		double xmin,xmax,ymin,ymax;
		xmin=mapPoint1.x();
		xmax=mapPoint2.x();
		ymin=mapPoint1.y();
		ymax=mapPoint2.y();

		if (xmin>xmax)
		{
			double tem;
			tem=xmax;
			xmax=xmin;
			xmin=tem;
		}
		if (ymin>ymax)
		{
			double tem;
			tem=ymax;
			ymax=ymin;
			ymin=tem;
		}

		if (getBBoxXML(xmin,ymin,xmax,ymax))
		{
			AttributeTable attr;
			createAttributeTable(attr);
			_attributeTables.push_back(attr);
		}
		
		if (_attributeTables.size()>0)
		{
			Callback::EventArgs args;
			args._ea = &ea;
			args._aa = &aa;
			args._worldPoint = world1;

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

void GEOWFSFeatureQueryTool::createAttributeTable(AttributeTable& attr)
{
	AttributeValue& a=attr["layer"];
	a.first = ATTRTYPE_STRING;
	a.second.stringValue = _typeName;
	a.second.set = true;

	

	std::ifstream fin(_xmlBBoxPath.c_str(),std::ios::binary);
	if (!fin.eof())
	{
		osg::ref_ptr<XmlDocument> doc = XmlDocument::load(fin);
		osg::ref_ptr<XmlElement> wfsFeatureCollection = static_cast<XmlElement*>(doc->getChildren()[0].get());

		XmlElement* e_featureMember=wfsFeatureCollection->getSubElement("gml:featuremember");
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

	}//if (!fin.eof())
}

bool GEOWFSFeatureQueryTool::getBBoxXML(double xmin,double ymin,double xmax,double ymax)
{
	std::ostringstream buf;
	//buf<<"BBox-"<<xmin<<"-"<<ymin<<"-"<<xmax<<"-"<<ymax<<".xml";
	buf<<"BBox-"<<124.15<<"-"<<41.9229<<"-"<<124.219<<"-"<<41.9739<<".xml";
	_xmlBBoxPath=_xmlBBoxPathRoot+buf.str();

	if (!osgDB::fileExists(_xmlBBoxPath))
	{
		ReadResult rr=URI(createBBoxURL(xmin,ymin,xmax,ymax)).readString();
		if (rr.succeeded())
		{
			saveResponse(rr.getString(),_xmlBBoxPath);
			return true;
		}
		else
		{
			return false;
		}
	}

	if (osgDB::fileExists(_xmlBBoxPath))
	{
		return true;
	}

	return false;
}

std::string GEOWFSFeatureQueryTool::createBBoxURL(double xmin,double ymin,double xmax,double ymax)
{
	//http://192.168.42.55:9010/wfs_dl/wfs?VERSION=1.0.0&REQUEST=GetFeature&TypeName=五万主要公路线&BBOX=124.822,41.329,124.823,41.330
	std::string url;
	std::ostringstream buf;
	buf<<_wfsUrl
		<<"?"
		<<"VERSION=1.0.0"
		<<"&REQUEST=GetFeature"
		<<"&TypeName="<<_typeName
		<<"&BBox="<<xmin<<","<<ymin<<","<<xmax<<","<<ymax;
	url=buf.str();
	return url;
}
void GEOWFSFeatureQueryTool::saveResponse(const std::string buffer, const std::string& filename)
{
	if (!osgDB::fileExists(osgDB::getFilePath(filename)) )
		osgDB::makeDirectoryForFile(filename);
	std::ofstream fout;
	fout.open(filename.c_str(), std::ios::out | std::ios::binary);        
	fout.write(buffer.c_str(), buffer.size());        
	fout.close();
}
//////////////////////////////////////////////////////////////////////////////////////////readoutcallback
GEOWFSFeatureReadoutCallback::GEOWFSFeatureReadoutCallback(Container* container )
{
	_grid = new Grid();
	_grid->setBackColor( Color(Color::Black,0.7f) );
	container->addControl( _grid );
}

void
	GEOWFSFeatureReadoutCallback::onHit( std::vector<AttributeTable> tables,const EventArgs& args)
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
	GEOWFSFeatureReadoutCallback::onMiss( const EventArgs& args )
{
	clear();
	args._aa->requestRedraw();
}

void
	GEOWFSFeatureReadoutCallback::clear()
{
	_grid->clearControls();
	_grid->setVisible( false );
}
