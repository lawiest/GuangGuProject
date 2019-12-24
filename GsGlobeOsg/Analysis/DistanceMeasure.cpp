/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
 * Copyright 2016 Pelican Mapping
 * http://osgearth.org
 *
 * osgEarth is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

//#include <osgEarthUtil/MeasureTool>
#include <osgEarth/GeoMath>
#include <osgEarth/GLUtils>
#include <osgEarthFeatures/Feature>
#include <osgEarthAnnotation/FeatureNode>
#include <Analysis/DistanceMeasure.h>
#include <tool/CodeTransformHelp.h>
#include <osgEarthUtil/Controls>
#define LC "[DistanceMeasure] "

using namespace osgEarth;
using namespace osgEarth::Util;
using namespace osgEarth::Symbology;
using namespace osgEarth::Features;
using namespace osgEarth::Annotation;
using namespace osgEarth::Util::Controls;
//#define SHOW_EXTENT 1


GeoGlobe::DistanceMeasureHandler::DistanceMeasureHandler(osgEarth::MapNode* mapNode ):
_mouseDown         (false),
//_group             (group),
_gotFirstLocation  (false),
_lastPointTemporary(false),
_Finished          (false),
_geoInterpolation  (GEOINTERP_GREAT_CIRCLE),
_mouseButton       (osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON),
_isPath            (false),

_intersectionMask  (0xffffffff)
{
	m_ptrGroup = new osg::Group();
    setMapNode( mapNode );

	setIsPath(true);
	
}


GeoGlobe::DistanceMeasureHandler::~DistanceMeasureHandler()
{
	_mapNode->removeChild(m_ptrGroup);
    setMapNode( 0L );
}


void
GeoGlobe::DistanceMeasureHandler::setMapNode( MapNode* mapNode )
{
	MapNode* oldMapNode = getMapNode();

	if (oldMapNode != mapNode)
	{
		if (oldMapNode)
		{
			oldMapNode->removeChild(m_ptrGroup.get());
		}

		_mapNode = mapNode;

		if (mapNode)
		{
			mapNode->addChild(m_ptrGroup);
		}

		rebuild();
	}
}


void
GeoGlobe::DistanceMeasureHandler::rebuild()
{
	if (m_ptrGroup.valid() && _featureNode.valid())
	{
		m_ptrGroup->removeChild(_featureNode.get());
		_featureNode = 0L;
	}

    if ( !getMapNode() )
        return;

    if ( getMapNode()->getMapSRS()->isProjected() )
    {
        OE_WARN << LC << "Sorry, DistanceMeasure does not yet support projected maps" << std::endl;
        return;
    }


    // Define the path feature:
    _feature = new Feature(new LineString(), getMapNode()->getMapSRS());
    _feature->geoInterp() = _geoInterpolation;

    // clamp to the terrain skin as it pages in
    AltitudeSymbol* alt = _feature->style()->getOrCreate<AltitudeSymbol>();
    alt->clamping() = alt->CLAMP_TO_TERRAIN;
    alt->technique() = alt->TECHNIQUE_GPU;

    // offset to mitigate Z fighting
    RenderSymbol* render = _feature->style()->getOrCreate<RenderSymbol>();
    render->depthOffset()->enabled() = true;
    render->depthOffset()->automatic() = true;

    // define a style for the line
    LineSymbol* ls = _feature->style()->getOrCreate<LineSymbol>();
    ls->stroke()->color() = Color::Yellow;
    ls->stroke()->width() = 2.0f;
    ls->stroke()->widthUnits() = Units::PIXELS;
    ls->tessellation() = 150;

    _featureNode = new FeatureNode(_feature.get() );
	//_featureNode->setMapNode(getMapNode());
    //_featureNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	GLUtils::setLighting(_featureNode->getOrCreateStateSet(), osg::StateAttribute::OFF);
    //_featureNode->setClusterCulling(false);

    m_ptrGroup->addChild (_featureNode.get() );

#ifdef SHOW_EXTENT

    // Define the extent feature:
    _extentFeature = new Feature( new Polygon(), getMapNode()->getMapSRS() );
    _extentFeature->geoInterp() = GEOINTERP_RHUMB_LINE;
    _extentFeature->style()->add( alt );
    LineSymbol* extentLine = _extentFeature->style()->getOrCreate<LineSymbol>();
    extentLine->stroke()->color() = Color::Cyan;
    extentLine->stroke()->width() = 2.0f;
    extentLine->tessellation() = 20;

    _extentFeatureNode = new FeatureNode( getMapNode(), _extentFeature.get() );
    
    _group->addChild( _extentFeatureNode.get() );
#endif
}

osg::Group* GeoGlobe::DistanceMeasureHandler::DisplayGroup()
{
	return m_ptrGroup;
}

bool
GeoGlobe::DistanceMeasureHandler::getIsPath() const
{
    return _isPath;
}

void GeoGlobe::DistanceMeasureHandler::setIsPath( bool bPath )
{
   if (_isPath != bPath)
    {
        _isPath = bPath;
        _Finished = false;
       // clear();                    
        _gotFirstLocation = false;
    }

	
}


GeoInterpolation
GeoGlobe::DistanceMeasureHandler::getGeoInterpolation() const
{
    return _geoInterpolation;
}

void
GeoGlobe::DistanceMeasureHandler::setGeoInterpolation( GeoInterpolation geoInterpolation )
{
    if (_geoInterpolation != geoInterpolation)
    {
        _geoInterpolation = geoInterpolation;
        _feature->geoInterp() = _geoInterpolation;
        _featureNode->init();
        fireDistanceChanged();
    }
}

void
GeoGlobe::DistanceMeasureHandler::setLineStyle( const Style& style )
{
     _feature->style() = style;
     _featureNode->init();
}

bool GeoGlobe::DistanceMeasureHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{    
	if (_Finished /*&& ea.getHandled()*/)
		return false;

	osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());                
    if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() == _mouseButton)
    {        
        _mouseDown = true;
        _mouseDownX = ea.getX();
        _mouseDownY = ea.getY();
	//	m_ptrGroup->removeChild(m_ptrLabelNode);
		
    }
    else if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() == _mouseButton)
    {        
        float eps = 1.0f;
        _mouseDown = false;
        if (osg::equivalent(ea.getX(), _mouseDownX, eps) && osg::equivalent(ea.getY(), _mouseDownY, eps))
        {
            double lon, lat;
            if (getLocationAt(view, ea.getX(), ea.getY(), lon, lat))
            {
                if (!_gotFirstLocation)
                {
                    _Finished = false;
                  //  clear();                    
                    _gotFirstLocation = true;
                    _feature->getGeometry()->push_back( osg::Vec3d( lon, lat, 0 ) );
                }
                else
                {
                    if (_lastPointTemporary)
                    {
                        _feature->getGeometry()->back() = osg::Vec3d( lon, lat, 0 );
                        _lastPointTemporary = false;
                    }
                    else
                    {                     
                        _feature->getGeometry()->push_back( osg::Vec3d( lon, lat, 0 ) );
                    }

					_start = GeoPoint(getMapNode()->getMapSRS(), lon, lat, ALTMODE_ABSOLUTE);

                    _featureNode->init();


					if (_geoInterpolation == GEOINTERP_GREAT_CIRCLE)
					{
						distance = GeoMath::distance(_feature->getGeometry()->asVector());
					}
				/*	else if (_geoInterpolation == GEOINTERP_RHUMB_LINE)
					{
						distance = GeoMath::rhumbDistance(_feature->getGeometry()->asVector());
					}*/
					for (MeasureToolEventHandlerList::const_iterator i = _eventHandlers.begin(); i != _eventHandlers.end(); ++i)
					{
						i->get()->onDistanceChanged(this, distance);
					}

					std::ostringstream s1;
					ostringstream osm;
					osm.precision(2);           // 浮点数转换限制  
					osm.setf(std::ios::fixed); // 将浮点数的位数限定为小数点之后的位数  
					if (distance > 1000)
					{
						osm << distance / 1e3;
						s1  << osm.str() << "千米" << "\n";
					}
					else
					{
						osm << distance;
						s1 << osm.str() << "米" << "\n";
					}
					
					OnFinish(distance);

					std::string na = CodeHelp::String_To_UTF8(s1.str());
					m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->size() = 18.0f;
					//m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color() = Color::White;
					m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color() = Color::White;
					m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->font() = "msyh.ttc";
					m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
					m_ptrLabelNode = new GeoGlobe::Annotation::PlaceNode(_start, na, m_SymStyle);
					m_ptrLabelNode->setDynamic(true);
					m_ptrLabelNode->setCullingActive(true);
					m_ptrGroup->addChild(m_ptrLabelNode);

                  //  _gotFirstLocation = false;
                  //  _Finished = true;
                    if (_Finished || !_isPath) {
                        _gotFirstLocation = false;
                    }

#ifdef SHOW_EXTENT
                    const GeoExtent ex( _feature->getSRS(), _feature->getGeometry()->getBounds() );
                    //OE_INFO << "extent = " << ex.toString() << std::endl;
                    Geometry* eg = _extentFeature->getGeometry();
                    osg::Vec3d fc = ex.getCentroid();
                    eg->clear();
                    eg->push_back( ex.west(), ex.south() );
                    if ( ex.width() >= 180.0 )
                        eg->push_back( fc.x(), ex.south() );
                    eg->push_back( ex.east(), ex.south() );
                    eg->push_back( ex.east(), ex.north() );
                    if ( ex.width() >= 180.0 )
                        eg->push_back( fc.x(), ex.north() );
                    eg->push_back( ex.west(), ex.north() );
                    _extentFeatureNode->init();
#endif

                    fireDistanceChanged();
                    aa.requestRedraw();
                }
            }
        }
    }  
    else if (ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK) {        
        if (_gotFirstLocation)
        {
            //_gotFirstLocation = false;
            _Finished = true;    
            aa.requestRedraw();

            return true;
        }
    }
    else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
    {
        if (_gotFirstLocation)
        {
            double lon, lat;
            if (getLocationAt(view, ea.getX(), ea.getY(), lon, lat))
            {
                if (!_lastPointTemporary)
                {
                    _feature->getGeometry()->push_back( osg::Vec3d( lon, lat, 0 ) );                 
                    _lastPointTemporary = true;
                }                        
                else
                {
                    _feature->getGeometry()->back() = osg::Vec3d( lon, lat, 0 );
                }
                _featureNode->init();
                fireDistanceChanged();

                aa.requestRedraw();

            }
        }
    }    
    return false;
}

bool GeoGlobe::DistanceMeasureHandler::getLocationAt(osgViewer::View* view, double x, double y, double &lon, double &lat)
{
    osgUtil::LineSegmentIntersector::Intersections results;            
    if ( getMapNode() &&  view->computeIntersections( x, y, results, _intersectionMask ) )
    {
        // find the first hit under the mouse:
        osgUtil::LineSegmentIntersector::Intersection first = *(results.begin());
        osg::Vec3d point = first.getWorldIntersectPoint();

        double lat_rad, lon_rad, height;       
        getMapNode()->getMap()->getProfile()->getSRS()->getEllipsoid()->convertXYZToLatLongHeight( 
            point.x(), point.y(), point.z(), lat_rad, lon_rad, height );

        lat = osg::RadiansToDegrees( lat_rad );
        lon = osg::RadiansToDegrees( lon_rad );
        return true;
    }
    return false;
}

void GeoGlobe::DistanceMeasureHandler::clear()
{
    //Clear the locations    
    _feature->getGeometry()->clear();
    //_features->dirty();
    _featureNode->init();

#ifdef SHOW_EXTENT
    _extentFeature->getGeometry()->clear();
    _extentFeatureNode->init();
#endif

    fireDistanceChanged();

    _gotFirstLocation = false; 
    _lastPointTemporary = false; 
}

void GeoGlobe::DistanceMeasureHandler::setMouseButton(int mouseButton)
{
    _mouseButton = mouseButton;
}

int GeoGlobe::DistanceMeasureHandler::getMouseButton() const
{
    return _mouseButton;
}


void GeoGlobe::DistanceMeasureHandler::addEventHandler(DistanceMeasureEventHandler* handler )
{
    _eventHandlers.push_back( handler );
}


void GeoGlobe::DistanceMeasureHandler::fireDistanceChanged()
{
    double distance = 0;
    if (_geoInterpolation == GEOINTERP_GREAT_CIRCLE)
    {
        distance = GeoMath::distance(_feature->getGeometry()->asVector());
    }
    else if (_geoInterpolation == GEOINTERP_RHUMB_LINE) 
    {
        distance = GeoMath::rhumbDistance(_feature->getGeometry()->asVector());
    }
    for (MeasureToolEventHandlerList::const_iterator i = _eventHandlers.begin(); i != _eventHandlers.end(); ++i)
    {
        i->get()->onDistanceChanged( this, distance );
    }
}


void GeoGlobe::DistanceMeasureHandler::addDistanceMeasureEventHandler(DistanceMeasureEventHandler* callback)
{
	_eventHandlers.push_back(callback);
}

void GeoGlobe::DistanceMeasureHandler::removeDistanceMeasureEventHandler(DistanceMeasureEventHandler* callback)
{
	MeasureToolEventHandlerList::iterator i = std::find(_eventHandlers.begin(), _eventHandlers.end(), callback);
	if (i != _eventHandlers.end())
	{
		_eventHandlers.erase(i);
	}
}
