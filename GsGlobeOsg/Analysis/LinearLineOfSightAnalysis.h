#ifndef LINEARLINEOFSIGHTEDITOR_H
#define LINEARLINEOFSIGHTEDITOR_H 1

#include <osgEarthUtil/LineOfSight>
#include <osgEarthUtil/LinearLineOfSight>
#include <osgEarth/MapNode>
#include <osgEarth/MapNodeObserver>
#include <osgEarth/Terrain>
#include <osgEarth/GeoData>

#include <sstream>
#include <string>
#include <Analysis/LineTraker3DElement.h>
#include <PlaceName/GeoBillboard.h>
#include <tool/CodeTransformHelp.h>


using namespace osgEarth::Util;

namespace GeoGlobe
{
	class LOSDraggerCallback : public GeoGlobe::LineTraker3DElement::PositionChangedCallback
	{
	public:
		LOSDraggerCallback(LinearLineOfSightNode* los, bool start):
		  _los(los),
			  _start(start)
		  {      
		  }

		  virtual void onPositionChanged(const GeoGlobe::LineTraker3DElement* sender, const osgEarth::GeoPoint& position)
		  {   
			  if ( _start )
				  _los->setStart( position );
			  else
				  _los->setEnd( position );          
		  }


		  LinearLineOfSightNode* _los;
		  bool _start;
	};

	class LinearLineOfSightAnalysis : public osg::Group, public MapNodeObserver
    {
    public:
        LinearLineOfSightAnalysis(MapNode* mapNode);           
        virtual ~LinearLineOfSightAnalysis();

		virtual void setMapNode( MapNode* mapNode );
		virtual MapNode* getMapNode();

		virtual void traverse(osg::NodeVisitor& nv); 

		bool IsFinished();

	protected:
		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
		bool getLocationAt(osgViewer::View* view, double x, double y, double &lon, double &lat, double &h);
		osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> CreateBillboard(GeoPoint geoPt, std::string strContent);
		bool UpdateHitBillboard();

    private:
		osg::ref_ptr< MapNode > _mapNode;
        osg::ref_ptr< LinearLineOfSightNode > _los;
		osg::ref_ptr< GeoGlobe::LineTraker3DElement> _startLineTraker;
		osg::ref_ptr< GeoGlobe::LineTraker3DElement> _endLineTraker;

		osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> _starBillboard;
		osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> _endBillboard;
		osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> _hitBillboard;

		int _nStep;
		bool m_bFinished;
    };

} // namespace GeoGlobe

#endif