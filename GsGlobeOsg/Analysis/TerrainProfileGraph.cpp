#include "Analysis/TerrainProfileGraph.h"

#include <osg/Notify>
#include <osgGA/StateSetManipulator>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/TerrainProfile>
#include <osgEarth/GeoMath>
#include <osgEarth/Registry>
#include <osgEarth/GLUtils>
#include "OsgCoreSingleton.h"

using namespace osgEarth;
using namespace osgEarth::Util;
using namespace osgEarth::Symbology;
using namespace osgEarth::Features;
using namespace osgEarth::Annotation;


GeoGlobe::TerrainProfileGraph::TerrainProfileGraph(osgEarth::MapNode* mapNode)
{
	_startValid = false;
	_temporaryEndValid = false;
	_bFinished = false;

	_intersectionMask = 0xffffffff;

	_geoInterpolation = GEOINTERP_GREAT_CIRCLE;

	displayGroup = new osg::Group();

	setMapNode(mapNode);

	mapNode->addChild(displayGroup);

}

GeoGlobe::TerrainProfileGraph::~TerrainProfileGraph()
{
}

void GeoGlobe::TerrainProfileGraph::setMapNode(MapNode* mapNode)
{
	MapNode* oldMapNode = getMapNode();
	if (oldMapNode != mapNode)
	{
		_mapNode = mapNode;
		rebuild();
	}
}

void GeoGlobe::TerrainProfileGraph::rebuild()
{
	if (displayGroup.valid() && _featureNode.valid())
	{
		displayGroup->removeChild(_featureNode.get());
		_featureNode = 0L;
	}

	if (!getMapNode())
		return;

	if (getMapNode()->getMapSRS()->isProjected())
		return;

	// Define the path feature:
	_feature = new Feature(new LineString(), getMapNode()->getMapSRS());
	_feature->geoInterp() = _geoInterpolation;

	// clamp to the terrain skin as it pages in
	AltitudeSymbol* alt = _feature->style()->getOrCreate<AltitudeSymbol>();
	alt->clamping() = alt->CLAMP_TO_TERRAIN;
	alt->technique() = alt->TECHNIQUE_GPU;
	
	// define a style for the line
	LineSymbol* ls = _feature->style()->getOrCreate<LineSymbol>();
	ls->stroke()->color() = Color::Yellow;
	ls->stroke()->width() = 2.0f;
	ls->stroke()->widthUnits() = Units::PIXELS;
	ls->tessellation() = 150;

	_featureNode = new FeatureNode(_feature.get());
	//_featureNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	GLUtils::setLighting(_featureNode->getOrCreateStateSet(), osg::StateAttribute::OFF);

	displayGroup->addChild(_featureNode.get());
}

bool GeoGlobe::TerrainProfileGraph::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (_bFinished)
		return false;

	osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());

	if (ea.getEventType() == ea.PUSH && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		osg::Vec3d world;
		if (_mapNode->getTerrain()->getWorldCoordsUnderMouse(aa.asView(), ea.getX(), ea.getY(), world))
		{
			GeoPoint mapPoint;
			mapPoint.fromWorld(_mapNode->getMapSRS(), world);

			double lon, lat;
			if (getLocationAt(view, ea.getX(), ea.getY(), lon, lat))
			{
				if (!_startValid)
				{
					_startValid = true;
					_start = mapPoint.vec3d();
					_feature->getGeometry()->push_back(osg::Vec3d(lon, lat, 0));
				}
				else
				{
					if (_temporaryEndValid)
					{
						_feature->getGeometry()->back() = osg::Vec3d(lon, lat, 0);
						_temporaryEndValid = false;
					}
					else
					{
						_feature->getGeometry()->push_back(osg::Vec3d(lon, lat, 0));
					}

					_end = mapPoint.vec3d();

					compute();
					_startValid = false;
					_bFinished = true;
					//_temporaryEndValid = true;
				}
				_featureNode->dirty();
			}
		}
	}	
	else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
	{
		if (_startValid)
		{
			double lon, lat;
			if (getLocationAt(view, ea.getX(), ea.getY(), lon, lat))
			{
				if (!_temporaryEndValid)
				{
					_feature->getGeometry()->push_back(osg::Vec3d(lon, lat, 0));
					_temporaryEndValid = true;
				}
				else
				{
					_feature->getGeometry()->back() = osg::Vec3d(lon, lat, 0);
				}
				_featureNode->dirty();
				//aa.requestRedraw();
			}
		}
	}
	return false;
}

bool GeoGlobe::TerrainProfileGraph::getLocationAt(osgViewer::View* view, double x, double y, double &lon, double &lat)
{
	osgUtil::LineSegmentIntersector::Intersections results;
	if (getMapNode() && view->computeIntersections(x, y, results, _intersectionMask))
	{
		// find the first hit under the mouse:
		osgUtil::LineSegmentIntersector::Intersection first = *(results.begin());
		osg::Vec3d point = first.getWorldIntersectPoint();

		double lat_rad, lon_rad, height;
		getMapNode()->getMap()->getProfile()->getSRS()->getEllipsoid()->convertXYZToLatLongHeight(
			point.x(), point.y(), point.z(), lat_rad, lon_rad, height);

		lat = osg::RadiansToDegrees(lat_rad);
		lon = osg::RadiansToDegrees(lon_rad);
		return true;
	}
	return false;
}


void GeoGlobe::TerrainProfileGraph::compute()
{
	osg::ref_ptr<TerrainProfileCalculator > _profileCalculator = new TerrainProfileCalculator(_mapNode,
		GeoPoint(_mapNode->getMapSRS(), _start.x(), _start.y()),
		GeoPoint(_mapNode->getMapSRS(), _end.x(), _end.y()));


	osgEarth::Util::TerrainProfile _profile = _profileCalculator->getProfile();
	
	_profile.getElevationRanges(minElevation, maxElevation);
	double elevationRange = maxElevation - minElevation;

	totalDistance = _profile.getTotalDistance();

	for (unsigned int i = 0; i < _profile.getNumElevations(); i++)
	{
		distanceNum.push_back(_profile.getDistance(i));
		elevationNum.push_back(_profile.getElevation(i));
	}

	OnFinish(distanceNum, elevationNum);
}

double GeoGlobe::TerrainProfileGraph::MaxElevation()
{
	return maxElevation;
}

double GeoGlobe::TerrainProfileGraph::MinElevation()
{
	return minElevation;
}

double GeoGlobe::TerrainProfileGraph::MaxDistance()
{
	return totalDistance;
}

osg::Node* GeoGlobe::TerrainProfileGraph::DisplayGroup()
{
	return displayGroup;
}


std::list <double> GeoGlobe::TerrainProfileGraph::DistanceVec()
{
	return distanceNum;
}
	
std::list <double> GeoGlobe::TerrainProfileGraph::ElevationVec()
{
	return elevationNum;
}