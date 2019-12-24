#ifndef OBSERVERPOINTSINRANGEANALYSIS_H
#define OBSERVERPOINTSINRANGEANALYSIS_H 1

#include <osgEarthUtil/RadialLineOfSight>
#include <osgEarth/MapNode>
#include <osgEarth/MapNodeObserver>
#include <osgEarth/Terrain>
#include <osgEarth/GeoData>
#include <Analysis/LineTraker3DElement.h>
#include "Analysis/FormHook.h"

using namespace osgEarth::Util;
 

namespace GeoGlobe
{
	class ObserverPointsInRangeAnalysis : public osg::Group, public MapNodeObserver
    {
    public:
        ObserverPointsInRangeAnalysis(MapNode* mapNode, GeoGlobe::IConsultationWindow* consultationWnd = NULL);
        virtual ~ObserverPointsInRangeAnalysis();

		virtual void setMapNode( MapNode* mapNode );
		virtual MapNode* getMapNode();

		virtual void traverse(osg::NodeVisitor& nv);

		bool IsFinished();

	protected:
		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
		bool getLocationAt(osgViewer::View* view, double x, double y, double &lon, double &lat, double &h);
		void CreateBillboard(GeoPoint geoPt);

    private:
		osg::ref_ptr< MapNode > _mapNode;
		osg::ref_ptr< GeoGlobe::LineTraker3DElement> _startLineTraker;
		osg::ref_ptr< RadialLineOfSightNode > _rlos;		
		int _nStep;
		osg::Vec3d _startWorld;

		GeoGlobe::IConsultationWindow* m_consultationWnd;
		bool m_bFinished;
    };

} // namespace GeoGlobe

#endif