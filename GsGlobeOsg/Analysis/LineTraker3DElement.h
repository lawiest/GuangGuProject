/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
* Copyright 2008-2014 Pelican Mapping
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
#ifndef GEOGLOBE_LINETRAKER3DELEMENT_H
#define GEOGLOBE_LINETRAKER3DELEMENT_H 1

#include <osgEarth/GeoData>
#include <osgEarth/MapNodeObserver>
#include <osg/ShapeDrawable>
#include <osgManipulator/Projector>
#include <osgViewer/Viewer>
#include <osgEarthAnnotation/LabelNode>
#include <Analysis/SphereElement.h>
#include <AnalysisCallBack.h>
#include <event.h>

namespace GeoGlobe
{

    class LineTraker3DElement : public osg::Group, public osgEarth::MapNodeObserver
    {
    public:
        /**
        * Callback that is fired when the position changes
        */
        struct PositionChangedCallback : public osg::Referenced
        {
        public:
            virtual void onPositionChanged(const LineTraker3DElement* sender, const osgEarth::GeoPoint& position) {};
            virtual ~PositionChangedCallback() { }
        };

        typedef std::list< osg::ref_ptr<PositionChangedCallback> > PositionChangedCallbackList;

		enum DragMode
		{
			DRAGMODE_HORIZONTAL,
			DRAGMODE_VERTICAL
		};

		void StartAnalysis(osgEarth::MapNode* mapNode, const DragMode& defaultMode);

		LineTraker3DElement();
        LineTraker3DElement(osgEarth::MapNode* mapNode,const DragMode& defaultMode=DRAGMODE_HORIZONTAL );
        virtual ~LineTraker3DElement();

        const osgEarth::GeoPoint& getPosition() const;
        void setPosition( const osgEarth::GeoPoint& position, bool fireEvents=true);

        void setDefaultDragMode(const DragMode& mode) { _defaultMode = mode; }
        DragMode& getDefaultDragMode() { return _defaultMode; }

        void setColor( const osg::Vec4f& color );

        void addPositionChangedCallback( PositionChangedCallback* callback );
        void removePositionChangedCallback( PositionChangedCallback* callback );

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
		//void AddAnalysisCallBack(GLOBE_NAME::AnalysisCallBack* ptrCallBack);

		bool IsFinished();

		UTILITY_NAME::GsDelegate<void (double)> OnFinish;

    public: // MapNodeObserver
        virtual void setMapNode(osgEarth::MapNode* mapNode );
        virtual osgEarth::MapNode* getMapNode() { return _mapNode.get(); }

    protected:
        bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
        void firePositionChanged();
		void draw();
		void CreateLabel();

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
		//GLOBE_NAME::AnalysisCallBackPtr m_ptrAnalysisCallBack;

		//点 
		osg::ref_ptr<GeoGlobe::SphereElement> _SphereElement;

		//高度或者距离的Label
		osg::ref_ptr<osgEarth::Annotation::LabelNode> m_ptrLabelNode;
		osgEarth::Symbology::Style m_SymStyle;
    };

} // namespace GeoGlobe

#endif