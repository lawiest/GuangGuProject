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
#ifndef _POLYGONTRAKERELEMENT3D_H_
#define _POLYGONTRAKERELEMENT3D_H_ 1

#include <osgEarth/GeoData>
#include <osgEarth/MapNodeObserver>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthAnnotation/Draggers>

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/BlendColor>

#include <SchemeComparisonAnalysis/PolyInter/PolygonCMFSegmentIntersector.h>
#include "SchemeComparisonAnalysis/ModelFc.h"
#include <model/LodModel.h>
#include "Analysis/FormHook.h"



class PolygonCMFTrakerElement3D : public osg::Group, public MapNodeObserver
{
public:
    PolygonCMFTrakerElement3D(osgEarth::MapNode* mapNode, GeoGlobe::IMainWindow* mainWindow, osgEarth::Annotation::FeatureNode* polygon=0L,bool bDrawDrager = true);
    virtual ~PolygonCMFTrakerElement3D();

	struct AddCMFPointsCompletedCallback : public virtual osg::Referenced
	{
	public:
		void completed(osgViewer::Viewer* viewer, PolygonCMFTrakerElement3D* Ele,osg::Group* group);

		std::map<int64,osg::ref_ptr<CModelF>> _mapModel;
		std::map<int64,osg::ref_ptr<osg::StateSet>> m_mapState;
	};

	void setAnimationCompletedCallback(AddCMFPointsCompletedCallback* acc) { _addpointsCompletedCallback = acc; }
	AddCMFPointsCompletedCallback* getAnimationCompletedCallback() { return _addpointsCompletedCallback.get(); }
		 
public:
	GeoGlobe::IMainWindow* m_MainWindow;
	// MapNodeObserver
	virtual void setMapNode( MapNode* mapNode );
	virtual MapNode* getMapNode();

	virtual void traverse(osg::NodeVisitor& nv); 

	osgEarth::Annotation::AnnotationNode* getAnnotation() { return _polyNode.get(); }

	void mapMouseClick(const osgEarth::GeoPoint& geoPoint, const osg::Vec3d& worldPoint,int button);
	void mapMouseMove(const osgEarth::GeoPoint& geoPoint);
	void addPoint(const osgEarth::GeoPoint& point, const osg::Vec3d& worldPoint);
	void addPoint(const osgEarth::GeoPoint& point);
	void movePoint(int index, const osgEarth::GeoPoint& position);

	void refreshFeatureNode(bool geometryOnly=false);
	void createPointDragger(int index, const osgEarth::GeoPoint& point);
	void clearDisplay();
	void resetValues();
	void updateDisplay();

private:
		bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
		void HightModel(CModelF* modelnode,int64 uKey);


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

	bool _bDrawDrager;

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

	osg::ref_ptr<AddCMFPointsCompletedCallback> _addpointsCompletedCallback;
	 

};

struct PointDraggerCallback : public osgEarth::Annotation::Dragger::PositionChangedCallback
{
	PointDraggerCallback(int index, PolygonCMFTrakerElement3D* ele)
		: _index(index), _Elememt(ele)
	{
	}

	void onPositionChanged(const osgEarth::Annotation::Dragger* sender, const osgEarth::GeoPoint& position)
	{
		_Elememt->movePoint(_index, position);
	}

	int _index;
	PolygonCMFTrakerElement3D* _Elememt;
};


#endif
