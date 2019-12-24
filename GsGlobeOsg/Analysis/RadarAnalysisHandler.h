#ifndef RADARANALYSISHANDLER_H
#define RADARANALYSISHANDLER_H 1

#include <osgEarth/MapNode>
#include <osgEarth/MapNodeObserver>
#include <osgEarth/Terrain>
#include <osgEarth/GeoData>
#include <Analysis/RadarTraker3DElement.h>
#include "Analysis/FormHook.h"
#include <osgEarthAnnotation/AnnotationUtils>
#include <Analysis/RadarSphere.h>

using namespace osgEarth::Util;

namespace GeoGlobe
{
	class RadarAnalysisHandler : public osg::Group, public MapNodeObserver
	{
	public:
		RadarAnalysisHandler(MapNode* mapNode, GeoGlobe::IConsultationWindow* consultationWnd = NULL);
		virtual ~RadarAnalysisHandler();

		virtual void setMapNode(MapNode* mapNode);
		virtual MapNode* getMapNode();

		virtual void traverse(osg::NodeVisitor& nv);

		bool IsFinished();

	protected:
		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
		bool getLocationAt(osgViewer::View* view, double x, double y, double &lon, double &lat, double &h);		
		osg::Geode* compute_line(osg::Node* node, osg::Vec3d radarCenter);
	
		void VertexExtractor(osg::ref_ptr<osg::Geode> geode);
	private:
		osg::ref_ptr< MapNode > _mapNode;
		osg::ref_ptr< GeoGlobe::RadarTraker3DElement> _startPointTraker;
		osg::Vec3d _startWorld;	
		osg::Vec3d _radarCenter;

		RadarSphere* r_sphere;				
		GeoGlobe::IConsultationWindow* m_consultationWnd;
		bool m_bFinished;
		int _nStep;
	
		osg::ref_ptr<osg::Geode> pGeode;
		osg::ref_ptr<osg::Vec3Array> extracted_verts;

	};

} // namespace GeoGlobe

#endif