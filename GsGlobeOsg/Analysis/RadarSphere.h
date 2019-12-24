#ifndef RADARSPHERE_H
#define RADARSPHERE_H 1

#include <osgDB/ReadFile>
#include <osgearth/MapNode>

namespace GeoGlobe
{
	class RadarSphere : public osg::Geometry
{
public:

	RadarSphere( int iNumPartsLongtitude_half , int iNumPartsLatitude_half , osg::Vec3d& world, osg::ref_ptr<  osgEarth::MapNode > mapNode);

	void InitVertices(osg::Vec3d& world);//初始化顶点

	void AddFaces(osg::Node* node);//添加面

	void setRadius(float r);

protected:

	double m_dRadius;//半径

	int m_iNumPartsLongtitude_half;//经线分割的块数(半数)

	int m_iNumPartsLatitude_half;//纬线分割的块数(半数)
	
	osg::ref_ptr< osg::Vec3Array > m_pVertices;//圆面上的各点

	osg::Vec3d m_center;

	osg::ref_ptr< osgEarth::MapNode > m_mapNode;

	osg::ref_ptr<osg::Geode> pGeode;

	
};

} // namespace GeoGlobe

#endif