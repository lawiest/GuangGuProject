#ifndef GEOGLOBE__TERRAINPROFILEGRAPH_H
#define GEOGLOBE__TERRAINPROFILEGRAPH_H 1

#include <osg/Notify>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osgEarth/MapNode>
#include <osgEarthUtil/TerrainProfile>
#include <osgEarthFeatures/Feature>
#include <osgEarthAnnotation/FeatureNode>
#include <osgearth/MapNodeObserver>
#include <AnalysisCallBack.h>

using namespace osgEarth::Util;

namespace GeoGlobe
{
	/// \brief 剖面分析类型
	class TerrainProfileGraph :  public osgGA::GUIEventHandler
	{ 
	public:

		TerrainProfileGraph(osgEarth::MapNode* mapNode);
		virtual ~TerrainProfileGraph();

		virtual void setMapNode(osgEarth::MapNode* mapNode);
		virtual osgEarth::MapNode* getMapNode() { return _mapNode.get(); }
		
		bool TerrainProfileGraph::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);	

		void compute();

		UTILITY_NAME::GsDelegate<void(std::list<double>&,std::list<double>&)> OnFinish;

		osg::Node* DisplayGroup();

		bool getLocationAt(osgViewer::View* view, double x, double y, double &lon, double &lat);

		void setIntersectionMask(osg::Node::NodeMask intersectionMask) { _intersectionMask = intersectionMask; }
		osg::Node::NodeMask getIntersectionMask() const { return _intersectionMask; }

		std::list <double> ElevationVec();
		std::list <double> DistanceVec();

		double MaxElevation();
		double MinElevation();
		double MaxDistance();

		void rebuild();

	private:
		osg::ref_ptr< osgEarth::MapNode > _mapNode;

		osg::ref_ptr< osgEarth::Annotation::FeatureNode > _featureNode;
		osg::ref_ptr< osgEarth::Features::Feature >  _feature;
		osg::ref_ptr<osg::Group> displayGroup;
		bool _startValid, _temporaryEndValid;
		osg::Vec3d _start;
		osg::Vec3d _end;

		std::list <double> elevationNum;
		std::list <double> distanceNum;

		bool _bFinished;

		double minElevation, maxElevation;
		double totalDistance;

		osg::Node::NodeMask _intersectionMask;
		osgEarth::GeoInterpolation _geoInterpolation;

	};

} // namespace GeoGlobe
#endif