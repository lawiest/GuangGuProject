#include "RadarSphere.h"
#include <osgUtil/SmoothingVisitor>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>

GeoGlobe::RadarSphere::RadarSphere(int iNumPartsLongtitude_half, int iNumPartsLatitude_half, osg::Vec3d& world, osg::ref_ptr< osgEarth::MapNode > mapNode)
	: m_iNumPartsLongtitude_half(iNumPartsLongtitude_half),
	m_iNumPartsLatitude_half(iNumPartsLatitude_half)
{
	m_center = world;
	m_mapNode = mapNode;
}


void GeoGlobe::RadarSphere::setRadius(float r)
{
	m_dRadius = r;
	InitVertices(m_center);//初始化顶点
	AddFaces(m_mapNode);//添加面
}


void GeoGlobe::RadarSphere::InitVertices(osg::Vec3d& world)
{
	float x = world.x();
	float y = world.y();
	float z = world.z();

	osg::Vec3d north = osg::Vec3();

	osg::Vec3d up = world - osg::Vec3d(0.0, 0.0, 0.0);
 	up.normalize();	
 	osg::Vec3d zVec = osg::Vec3d(0.0, 0.0, 1.0);
 	double cosa = zVec * up;
 	double radianS = acos(cosa);
	osg::Vec3d m_pnormal = zVec ^ up;
 	
	m_pVertices = new osg::Vec3Array((m_iNumPartsLongtitude_half * 2 + 1)*((m_iNumPartsLatitude_half + 1) * 2 - 1));


	double dDeltaAngleV = osg::PI / (2 * m_iNumPartsLatitude_half);  //纬度之间的角度

	double dDeltaAngle = osg::PI / m_iNumPartsLongtitude_half;  //经度间夹角

	double dAngle, dAngleV = 0, dLatitudeRadius;  //角度,高度,纬度半径

	for (int i = 0; i < (m_iNumPartsLatitude_half) * 2; ++i, dAngleV += dDeltaAngleV)
	{
		dLatitudeRadius = abs(m_dRadius* sin(dAngleV));
		dAngle = 0;
		
		for (int k = 0; k < m_iNumPartsLongtitude_half * 2 + 1; ++k, dAngle += dDeltaAngle)
		{
			if (((i*(m_iNumPartsLongtitude_half * 2 + 1) + k) + 1) % (m_iNumPartsLongtitude_half * 2 + 1) == 0)
			{
				(*m_pVertices)[i*(m_iNumPartsLongtitude_half * 2 + 1) + k] = (*m_pVertices)[i*(m_iNumPartsLongtitude_half * 2 + 1) + k - m_iNumPartsLongtitude_half * 2];
			}
			else
			{
				osg::Vec3d m_pVerticesOrigin;
				m_pVerticesOrigin = osg::Vec3(dLatitudeRadius*cos(dAngle),
					                          dLatitudeRadius*sin(dAngle),
					                          m_dRadius * cos(dAngleV));

				osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();								
				
				mt->setMatrix(osg::Matrix::rotate(radianS, m_pnormal) *
					          osg::Matrix::translate(x, y, z));
				m_pVerticesOrigin = m_pVerticesOrigin * mt->getMatrix();				 	

				(*m_pVertices)[i*(m_iNumPartsLongtitude_half * 2 + 1) + k].set(m_pVerticesOrigin);
			}
		}
	}

	//初始化最后一条经线上的点

	for (int i = 0; i < m_iNumPartsLongtitude_half * 2 + 1; ++i)
	{
		osg::Vec3d m_pVerticesOrigin;
		m_pVerticesOrigin = osg::Vec3(0, 0, m_dRadius * cos(dAngleV));

		osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();

		mt->setMatrix(osg::Matrix::rotate(radianS, m_pnormal) *
			osg::Matrix::translate(x, y, z));
		m_pVerticesOrigin = m_pVerticesOrigin * mt->getMatrix();

		if (i == m_iNumPartsLongtitude_half * 2)
		{	
			(*m_pVertices)[(m_iNumPartsLongtitude_half * 2 + 1)*((m_iNumPartsLatitude_half + 1) * 2 - 2) + i].set(m_pVerticesOrigin);
		}
		else
		{
			(*m_pVertices)[(m_iNumPartsLongtitude_half * 2 + 1)*((m_iNumPartsLatitude_half + 1) * 2 - 2) + i].set(m_pVerticesOrigin);
		}
	}
}


void GeoGlobe::RadarSphere::AddFaces(osg::Node* node)
{
	pGeode = new osg::Geode();	
	
	this->setVertexArray(m_pVertices.get());

	int iNumLongtitude = m_iNumPartsLongtitude_half * 2;   //number of points in a latitude
	int iNumLatitude = m_iNumPartsLatitude_half * 2;       //latitude num(except the 2 poles)
	
	for (int k = 0; k < iNumLatitude; ++k)
	{
		int i;

		for (i = 0; i < iNumLongtitude; ++i)
		{
			if (((iNumLongtitude + 1) * k + i + 1) % (m_iNumPartsLongtitude_half * 2 + 1) != 0)
			{
				osg::ref_ptr<osg::DrawElementsUInt>pQuads = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);

				pQuads->push_back((iNumLongtitude + 1) * k + i);
				pQuads->push_back((iNumLongtitude + 1) * k + i + 1);
				pQuads->push_back((iNumLongtitude + 1) * k + i + 1 + (iNumLongtitude + 1));
				pQuads->push_back((iNumLongtitude + 1) * k + i + (iNumLongtitude + 1));

				addPrimitiveSet(pQuads.get());
			}
		}
	}


	osgUtil::SmoothingVisitor smv;
	smv.smooth(*this);

	//set normals
	osg::ref_ptr< osg::Vec3Array > pNormals= new osg::Vec3Array();
	for ( int i= 0; i< m_pVertices->size(); ++i)
	{
	 //each point is equal to a vector,or normal.Because a normal of a point on the ball surface points 
	 //to the point from the center(here is (0,0,0),namely origin point),we only need normalize it.
	 osg::Vec3 pt= (*m_pVertices)[i];
	 pt.normalize();
	 pNormals->push_back(pt);
	}
	
	this->setNormalArray( pNormals);
	this->setNormalBinding( osg::Geometry::BIND_PER_VERTEX);
}

