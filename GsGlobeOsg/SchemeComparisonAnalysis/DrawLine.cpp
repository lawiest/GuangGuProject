
#include "SchemeComparisonAnalysis/DrawLine.h"

CDrawLine::CDrawLine(osgEarth::MapNode* m_MapNode)
{
	_DLineWidth=5.0;
	_DLineColorArr=new osg::Vec4Array;
	_DLineColorArr->push_back( osg::Vec4( 0.f, 0.f, 1.f, 1.f ) ); 
	_DLinebindcolortype=osg::Geometry::BIND_PER_VERTEX;
	_DLightOnOrOff=false;

	_dMapNode=m_MapNode;
	_dMapSRS=m_MapNode->getMapSRS();
	_dGeoSRS=m_MapNode->getMapSRS()->getGeographicSRS();
}

CDrawLine::CDrawLine(osg::Vec3d starLLPt,osg::Vec3d endLLPt,osgEarth::MapNode* m_MapNode)
{
	 _DLineWidth=5.0;
	 _DLineColorArr=new osg::Vec4Array;
	 _DLineColorArr->push_back( osg::Vec4( 0.f, 0.0f, 1.f, 1.f ) ); 
	 _DLinebindcolortype=osg::Geometry::BIND_PER_VERTEX;
	 _DLightOnOrOff=false;

	_dMapNode=m_MapNode;
	_dMapSRS=m_MapNode->getMapSRS();
	_dGeoSRS=m_MapNode->getMapSRS()->getGeographicSRS();

	get2WorldPt(starLLPt,endLLPt);
}

CDrawLine::~CDrawLine(void)
{
}

void CDrawLine::get2WorldPt(osg::Vec3d starLLPt,osg::Vec3d endLLPt)
{
	//位置
	osgEarth::GeoPoint mapstarPtPos=osgEarth::GeoPoint(_dGeoSRS,starLLPt.x(),starLLPt.y(),starLLPt.z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_dMapSRS);
	osg::Vec3d worldMapstarPt;
	mapstarPtPos.toWorld(worldMapstarPt);//经纬度转换为世界坐标
	_starPt=worldMapstarPt;

	osgEarth::GeoPoint mapendPtPos=osgEarth::GeoPoint(_dGeoSRS,endLLPt.x(),endLLPt.y(),endLLPt.z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_dMapSRS);
	osg::Vec3d worldMapendPt;
	mapendPtPos.toWorld(worldMapendPt);//经纬度转换为世界坐标

	_endPt=worldMapendPt;


}

void CDrawLine::drawLine()
{
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;//申请一个几何体
	//设置线宽
	osg::ref_ptr <osg::LineWidth> LineSize = new osg::LineWidth; 
	LineSize ->setWidth (_DLineWidth) ;
	geom->getOrCreateStateSet()->setAttributeAndModes(LineSize.get (),osg::StateAttribute::ON);

	if (!_DLightOnOrOff)
	{
		geom->getOrCreateStateSet()->setMode(GL_LIGHTING ,osg::StateAttribute::OFF);
	}


	osg::ref_ptr<osg::Vec3Array> va=new osg::Vec3Array;
	va->push_back(_starPt);
	va->push_back(_endPt);
	geom->setVertexArray(va);
	//定义颜色数组 
	
	geom->setColorArray( _DLineColorArr.get() ); 

	geom->setColorBinding( _DLinebindcolortype );//设置颜色绑定方式为逐点绑定 BIND_PER_VERTEX
	
	geom->addPrimitiveSet(new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0,2) );//连线
	
	this->addDrawable(geom);
}


