#ifndef GEOGLOBE_RADARTRAKER3DELEMENT_H
#define GEOGLOBE_RADARTRAKER3DELEMENT_H 1

#include <osgEarth/GeoData>
#include <osgEarth/MapNodeObserver>
#include <osg/ShapeDrawable>
#include <osgManipulator/Projector>
#include <osgViewer/Viewer>
#include <osgEarthAnnotation/LabelNode>
#include <Analysis/SphereElement.h>
#include <AnalysisCallBack.h>

namespace GeoGlobe
{

	class RadarTraker3DElement : public osg::Group, public osgEarth::MapNodeObserver
	{
	public:
		/**
		* Callback that is fired when the position changes
		*/
		struct PositionChangedCallback : public osg::Referenced
		{
		public:
			virtual void onPositionChanged(const RadarTraker3DElement* sender, const osgEarth::GeoPoint& position) {};
			virtual ~PositionChangedCallback() { }
		};

		typedef std::list< osg::ref_ptr<PositionChangedCallback> > PositionChangedCallbackList;

		enum DragMode
		{
			DRAGMODE_HORIZONTAL,
			DRAGMODE_VERTICAL
		};

		void StartAnalysis(osgEarth::MapNode* mapNode, const DragMode& defaultMode);

		RadarTraker3DElement();
		RadarTraker3DElement(osgEarth::MapNode* mapNode, const DragMode& defaultMode = DRAGMODE_HORIZONTAL);
		virtual ~RadarTraker3DElement();

		const osgEarth::GeoPoint& getPosition() const;
		void setPosition(const osgEarth::GeoPoint& position, bool fireEvents = true);

		void setDefaultDragMode(const DragMode& mode) { _defaultMode = mode; }
		DragMode& getDefaultDragMode() { return _defaultMode; }

		void setColor(const osg::Vec4f& color);

		void addPositionChangedCallback(PositionChangedCallback* callback);
		void removePositionChangedCallback(PositionChangedCallback* callback);

		virtual void traverse(osg::NodeVisitor& nv);

		bool getLocationAt(osgViewer::View* view, double x, double y, double &lon, double &lat, double &h);

		const osgEarth::GeoPoint& getStart() const;
		const osg::Vec3d& getStartWorld() const;
		void setStart(const osgEarth::GeoPoint& start);
		const osgEarth::GeoPoint& getEnd() const;
		const osg::Vec3d& getEndWorld() const;
		void setEnd(const osgEarth::GeoPoint& end);
		const osgEarth::GeoPoint& getScreenStart() const;
		const osgEarth::GeoPoint& getScreenEnd() const;
// 		void AddAnalysisCallBack(GLOBE_NAME::AnalysisCallBack* ptrCallBack);

		bool IsFinished();

	public: // MapNodeObserver
		virtual void setMapNode(osgEarth::MapNode* mapNode);
		virtual osgEarth::MapNode* getMapNode() { return _mapNode.get(); }

	protected:
		bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
		void firePositionChanged();



	private:
		osg::observer_ptr< osgEarth::MapNode > _mapNode;
		osgEarth::GeoPoint _position;
		PositionChangedCallbackList _callbacks;

		osg::ref_ptr<  osgManipulator::LineProjector >  _projector;
		osgManipulator::PointerInfo  _pointer;
		osg::Vec3d _startProjectedPoint;
		DragMode _defaultMode;
		bool _Finished;
		int _nStep;

		osgEarth::GeoPoint _start;
		osgEarth::GeoPoint _screenStart;
		osgEarth::GeoPoint _end;
		osgEarth::GeoPoint _screenEnd;
		osg::Vec3d _startWorld;
		osg::Vec3d _endWorld;
		GLOBE_NAME::AnalysisCallBackPtr m_ptrAnalysisCallBack;

		//点 
		osg::ref_ptr<GeoGlobe::SphereElement> _SphereElement;

		//高度或者距离的Label
		osg::ref_ptr<osgEarth::Annotation::LabelNode> m_ptrLabelNode;
		osgEarth::Symbology::Style m_SymStyle;
	};

} // namespace GeoGlobe

#endif