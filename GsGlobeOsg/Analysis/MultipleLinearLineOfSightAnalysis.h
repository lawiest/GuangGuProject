#ifndef MULTIPLELINEARLINEOFSIGHTEDITOR_H
#define MULTIPLELINEARLINEOFSIGHTEDITOR_H 1

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


namespace GeoGlobe
{
	class MLOSDraggerCallback : public GeoGlobe::LineTraker3DElement::PositionChangedCallback
	{
	public:
		MLOSDraggerCallback(osgEarth::Util::LinearLineOfSightNode* los, bool start) :
			_los(los),
			_start(start)
		{
		}

		virtual void onPositionChanged(const GeoGlobe::LineTraker3DElement* sender, const osgEarth::GeoPoint& position)
		{
			if (_start)
				_los->setStart(position);
			else
				_los->setEnd(position);
		}


		osgEarth::Util::LinearLineOfSightNode* _los;
		bool _start;
	};

	class MultipleLinearLineOfSightAnalysis : public osg::Group, public osgEarth::Util::MapNodeObserver
	{
	public:
		MultipleLinearLineOfSightAnalysis(osgEarth::MapNode* mapNode);
		virtual ~MultipleLinearLineOfSightAnalysis();

		virtual void setMapNode(osgEarth::MapNode* mapNode);
		virtual osgEarth::MapNode* getMapNode();

		virtual void traverse(osg::NodeVisitor& nv);

		bool IsFinished();

	protected:
		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
		bool getLocationAt(osgViewer::View* view, double x, double y, double &lon, double &lat, double &h);
		osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> CreateBillboard(osgEarth::GeoPoint geoPt, std::string strContent);
		bool UpdateHitBillboard();

	private:
		osg::ref_ptr< osgEarth::MapNode > _mapNode;
		osg::ref_ptr< osgEarth::Util::LinearLineOfSightNode > _los;
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
