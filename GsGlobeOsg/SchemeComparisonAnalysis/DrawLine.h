#ifndef _DRAWLINE_H_
#define _DRAWLINE_H_

#pragma once

#include "SchemeComparisonAnalysis/CoEarth.h"
#include "SchemeComparisonAnalysis/CoreOSG.h"

class CDrawLine :public osg::Geode
{
public:
	CDrawLine(osgEarth::MapNode* m_MapNode);

	CDrawLine(osg::Vec3d starLLPt,osg::Vec3d endLLPt,osgEarth::MapNode* m_MapNode);


	void drawLine();

	void get2WorldPt(osg::Vec3d starPt,osg::Vec3d endPt);

	void setDLineColor(osg::ref_ptr<osg::Vec4Array> lineColorArr){_DLineColorArr=lineColorArr;}
	osg::ref_ptr<osg::Vec4Array> getDLineColor(){return _DLineColorArr;}

	void setDLineColorBindingType(osg::Geometry::AttributeBinding bindcolortype){_DLinebindcolortype=bindcolortype;}

	void setDLineLightMode(bool lightOnOrOff){_DLightOnOrOff=lightOnOrOff;}

	void setDLineWidth(float lineWidth){_DLineWidth=lineWidth;}
	float getDLineWidth(){return _DLineWidth;}

	~CDrawLine(void);

	
public:
	float _DLineWidth;
	osg::ref_ptr<osg::Vec4Array> _DLineColorArr;
	osg::Geometry::AttributeBinding _DLinebindcolortype;

	bool _DLightOnOrOff;

	osg::Vec3d _starPt;
	osg::Vec3d _endPt;

	osgEarth::MapNode* _dMapNode;
	const osgEarth::SpatialReference* _dMapSRS;
	const osgEarth::SpatialReference* _dGeoSRS;
};

#endif

