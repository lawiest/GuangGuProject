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
#ifndef GEOGLOBE_POLYGONTRAKERELEMENT3D_H
#define GEOGLOBE_POLYGONTRAKERELEMENT3D_H 1

#include <osgEarth/GeoData>
#include <osgEarth/MapNodeObserver>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osgEarthAnnotation/FeatureNode> 

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/BlendColor>
#include "osgEarthAnnotation\ImageOverlay"
#include <Analysis/PolygonSegmentIntersector.h>
#include <model/LodModel.h>
#include "Analysis/FormHook.h"
#include <event.h>

using namespace osgEarth::Annotation;


namespace GeoGlobe
{
    class PolygonTrakerElement3D : public osg::Group, public MapNodeObserver
    {
    public:
        PolygonTrakerElement3D(osgEarth::MapNode* mapNode, /*GeoGlobe::IMainWindow* mainWindow ,*/ osgEarth::Annotation::FeatureNode* polygon=0L, bool bDrawDrager = true);
        virtual ~PolygonTrakerElement3D();

		struct ModelQueryCompletedCallback : public virtual osg::Referenced
		{
		public:
			void completed(osgViewer::Viewer* viewer, PolygonTrakerElement3D* Ele,osg::Group* group);

			std::map<int64,osg::ref_ptr<GeoGlobe::Model::LodModel>> _mapModel;
			std::map<int64,osg::ref_ptr<osg::StateSet>> m_mapState;

			
		};

		struct AreaAnalysisCompletedCallback : public virtual osg::Referenced
		{
		public:
			void completed(osgViewer::Viewer* viewer, PolygonTrakerElement3D* Ele,osgEarth::MapNode* mapnode);

			double m_dblArea;
			osg::Vec3d m_centerPt;
		};

		struct ReginQueryCompletedCallback : public virtual osg::Referenced
		{
		public:
			ReginQueryCompletedCallback(long long qDate, int nDis, int nTime);
			void completed(osgViewer::Viewer* viewer, PolygonTrakerElement3D* Ele,osg::Group* group);
			ImageOverlay * ReginQuery();

		protected:
			osg::ref_ptr<osg::Group> m_ptrGroup;
		private:
			long long m_Date;

			osgEarth::Symbology::Polygon m_ScerrnPolygon;
			osgEarth::Symbology::Polygon m_GeoPolygon;
			MapNode *m_pMapNode;
			osgViewer::Viewer* m_pviewer;
			osgEarth::Bounds m_bounds;

			int m_nDis;
			int m_nTime;
		};

		//模型拾取
		void setModelQueryCompletedCallback(ModelQueryCompletedCallback* acc) { _addpointsCompletedCallback = acc; }
		ModelQueryCompletedCallback* getModelQueryCompletedCallback() { return _addpointsCompletedCallback.get(); }

		//面积量算
		void setAreaAnalysisCompletedCallback(AreaAnalysisCompletedCallback* acc) { _areaAnalysisCompletedCallback = acc; }
		AreaAnalysisCompletedCallback* getAreaAnalysisCompletedCallback() { return _areaAnalysisCompletedCallback.get(); }
		
		//区域分析
		/*void setReginQueryCompletedCallback(ReginQueryCompletedCallback *arc){_reginAnalysisCompletedCallback = arc;}
		ReginQueryCompletedCallback *getReginQueryCompletedCallback(){return _reginAnalysisCompletedCallback;}*/
	public:
		// MapNodeObserver
		virtual void setMapNode( MapNode* mapNode );
		virtual MapNode* getMapNode();

		virtual void traverse(osg::NodeVisitor& nv); 

		osgEarth::Annotation::AnnotationNode* getAnnotation() { return _polyNode.get(); }

		void mapMouseClick(const osgEarth::GeoPoint& geoPoint, const osg::Vec3d& worldPoint,int button);
		void mapMouseMove(const osgEarth::GeoPoint& point);
		void addPoint(const osgEarth::GeoPoint& point);
		void addPoint(const osgEarth::GeoPoint& point, const osg::Vec3d& worldPoint);
		void movePoint(int index, const osgEarth::GeoPoint& position);

		void refreshFeatureNode(bool geometryOnly=false);
		void createPointDragger(int index, const osgEarth::GeoPoint& point);
		void clearDisplay();
		void resetValues();
		void updateDisplay();
		bool IsFinished();

		void SetFillColor(const osg::Vec4f& fillcolor);
		void SetPathColor(const osg::Vec4f& pathcolor);

		UTILITY_NAME::GsDelegate<void (double)> OnFinish;

	private:
		 bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
		 void HightModel(GeoGlobe::Model::LodModel* modelnode,int64 uKey);

	private:
        osg::observer_ptr< MapNode > _mapNode;
		osg::Group* _draggers;
		osg::Vec4f _pathColor;
		osg::Vec4f _fillColor;
		osg::Vec3d _mousePoint;
		osg::ref_ptr<osgEarth::Symbology::Polygon> _polygon;
		osg::ref_ptr<osgEarth::Features::Feature> _polyFeature;
		osg::ref_ptr<osgEarth::Annotation::FeatureNode> _polyNode;
		bool _editing;
		osg::ref_ptr<osgEarth::Features::Feature> _inFeature;
		std::string _inName;
		std::string _inDescription;

		osg::ref_ptr<osgEarth::Annotation::FeatureNode> _featureNode;
		osgEarth::Symbology::Style _lineStyle;
		bool _drawLead;
		int _mouseDown;
		float _xDown, _yDown;
		osg::Vec3d _lastPoint;
		osg::Vec3d _currentPoint;
		bool _bFinished;
		bool _bDrawDrager;

		osg::ref_ptr<ModelQueryCompletedCallback> _addpointsCompletedCallback;
		osg::ref_ptr<AreaAnalysisCompletedCallback> _areaAnalysisCompletedCallback;
		osg::ref_ptr<ReginQueryCompletedCallback> _reginAnalysisCompletedCallback;

		GeoGlobe::IMainWindow * m_MainWindow;

	protected:
		//面积计算
		double m_areaReault;
		
	public:
		double CalcArea();
		//获取面积
		double GetArea(){return m_areaReault;};

		std::vector<osg::Vec3d> _vecGeoPoint;
		std::vector<osg::Vec3d> _vecWorldPoint;
		std::vector<osg::Vec3d> _vecScreenPoint;
    };

	struct PointDraggerCallback : public osgEarth::Annotation::Dragger::PositionChangedCallback
	{
		PointDraggerCallback(int index, PolygonTrakerElement3D* ele)
			: _index(index), _Elememt(ele)
		{
		}

		void onPositionChanged(const osgEarth::Annotation::Dragger* sender, const osgEarth::GeoPoint& position)
		{
			_Elememt->movePoint(_index, position);
		}

		int _index;
		PolygonTrakerElement3D* _Elememt;
	};

} // namespace GeoGlobe

#endif
