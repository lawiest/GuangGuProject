#include <SchemeComparisonAnalysis/PolyInter/PolygonCMFSegmentIntersector.h>

#include <osg/Geometry>
#include <osg/Notify>
#include <osg/io_utils>
#include <osg/TriangleFunctor>
#include <osg/KdTree>
#include <osg/Timer>
#include <osg/TexMat>

#include "SchemeComparisonAnalysis/ModelFc.h"
#include <model/Model.h>
#include <model/LodModel.h>

PolygonCMFSegmentIntersector::PolygonCMFSegmentIntersector(std::vector<osg::Vec3d>& vecGeoPts):
_parent(0)
{
	_vecGeoPoint.insert(_vecGeoPoint.end(),vecGeoPts.begin(),vecGeoPts.end());
}

Intersector* PolygonCMFSegmentIntersector::clone(IntersectionVisitorCMF& iv)
{
	osg::ref_ptr<PolygonCMFSegmentIntersector> lsi = new PolygonCMFSegmentIntersector(_vecGeoPoint);
	lsi->_parent = this;
	return lsi.release();
}

bool PolygonCMFSegmentIntersector::enter(const osg::Node& node)
{
	return true;
}

void PolygonCMFSegmentIntersector::leave()
{
}

void PolygonCMFSegmentIntersector::intersect(IntersectionVisitorCMF& iv, osg::Node* node)
{
	CModelF* model = dynamic_cast<CModelF*>(node);
	if (model == NULL)
		return;

	std::vector<osg::Vec3d> vecModelBoxPt;
	vecModelBoxPt.push_back(osg::Vec3d(*model->_Xmin,*model->_Ymin,0.0));
	vecModelBoxPt.push_back(osg::Vec3d(*model->_Xmax,*model->_Ymin,0.0));
	vecModelBoxPt.push_back(osg::Vec3d(*model->_Xmax,*model->_Ymax,0.0));
	vecModelBoxPt.push_back(osg::Vec3d(*model->_Xmin,*model->_Ymax,0.0));

	double x = model->getBound().center().x();
	double y = model->getBound().center().y();

	osg::Vec3d ptModelCenter = osg::Vec3d(x,y,0);
	if (IsPointInPolygon(_vecGeoPoint,ptModelCenter) || PolygonClip(_vecGeoPoint,vecModelBoxPt))//根据模型经纬度范围与多边形区域求交，确定是否在内部
	{
		Intersection hit;
		hit.nodePath = iv.getNodePath();
		insertIntersection(hit);
	}
}

void PolygonCMFSegmentIntersector::reset()
{
	Intersector::reset();

	_intersections.clear();
}

bool PolygonCMFSegmentIntersector::intersects(const osg::BoundingSphere& bs)
{
	return true;
}

bool PolygonCMFSegmentIntersector::PolygonClip(const std::vector<osg::Vec3d> &poly1,const std::vector<osg::Vec3d> &poly2)
{
	if (poly1.size() < 3 || poly2.size() < 3)
		return false;

	//long x,y;
	//for (int i=0;i<poly1.size();i++)
	//{
	//	int poly1_next_idx = (i + 1) % poly1.size();
	//	for(int j=0;j<poly2.size();j++)
	//	{
	//		int poly2_next_idx = (j + 1) % poly2.size();

	//		if (GetCrossPoint(poly1[i],poly1[poly1_next_idx],
	//			poly2[j],poly2[poly2_next_idx],
	//			x,y))
	//		{
	//			return true;
	//		}
	//	}
	//}

	//计算多边形内部点
	int nPtNum1 = 0;
	for(int i = 0;i < poly1.size();i++)
	{
		if (IsPointInPolygon(poly2,poly1[i]))
		{
			nPtNum1++;
		}
	}
	if(nPtNum1 == poly1.size())
		return true;

	int nPtNum2 = 0;
	for (int i = 0;i < poly2.size();i++)
	{
		if (IsPointInPolygon(poly1,poly2[i]))
		{
			nPtNum2++;
		}
	}

	if(nPtNum2 == poly2.size())
		return true;

	return false;
}

bool PolygonCMFSegmentIntersector::GetCrossPoint(const osg::Vec3d &p1,const osg::Vec3d &p2,const osg::Vec3d &q1,const osg::Vec3d &q2,long &x,long &y)
{
	if(IsRectCross(p1,p2,q1,q2))
	{
		if (IsLineSegmentCross(p1,p2,q1,q2))
		{
			//求交点
			long tmpLeft,tmpRight;
			tmpLeft = (q2.x() - q1.x()) * (p1.y() - p2.y()) - (p2.x() - p1.x()) * (q1.y() - q2.y());
			tmpRight = (p1.y() - q1.y()) * (p2.x() - p1.x()) * (q2.x() - q1.x()) + q1.x() * (q2.y() - q1.y()) * (p2.x() - p1.x()) - p1.x() * (p2.y() - p1.y()) * (q2.x() - q1.x());

			x = (int)((double)tmpRight/(double)tmpLeft);

			tmpLeft = (p1.x() - p2.x()) * (q2.y() - q1.y()) - (p2.y() - p1.y()) * (q1.x() - q2.x());
			tmpRight = p2.y() * (p1.x() - p2.x()) * (q2.y() - q1.y()) + (q2.x()- p2.x()) * (q2.y() - q1.y()) * (p1.y() - p2.y()) - q2.y() * (q1.x() - q2.x()) * (p2.y() - p1.y()); 
			y = (int)((double)tmpRight/(double)tmpLeft);
			return true;
		}
	}
	return false;
}

bool PolygonCMFSegmentIntersector::IsRectCross(const osg::Vec3d &p1,const osg::Vec3d &p2,const osg::Vec3d &q1,const osg::Vec3d &q2)
{
	bool ret = min(p1.x(),p2.x()) <= max(q1.x(),q2.x())&&
		min(q1.x(),q2.x()) <= max(p1.x(),p2.x())&&
		min(p1.y(),p2.y()) <= max(q1.y(),q2.y())&&
		min(q1.y(),q2.y()) <= max(p1.y(),p2.y());

	return ret;
}

bool PolygonCMFSegmentIntersector::IsLineSegmentCross(const osg::Vec3d &pFirst1,const osg::Vec3d &pFirst2,const osg::Vec3d &pSecond1,const osg::Vec3d &pSecond2)
{
	long line1,line2;
	line1 = pFirst1.x() * (pSecond1.y() - pFirst2.y()) +
		pFirst2.x() * (pFirst1.y() - pSecond1.y()) +
		pSecond1.x() * (pFirst2.y() - pFirst1.y());
	line2 = pFirst1.x() * (pSecond2.y() - pFirst2.y()) +
		pFirst2.x() * (pFirst1.y() - pSecond2.y()) + 
		pSecond2.x() * (pFirst2.y() - pFirst1.y());
	if (((line1 ^ line2) >= 0) && !(line1 == 0 && line2 == 0))
		return false;

	line1 = pSecond1.x() * (pFirst1.y() - pSecond2.y()) +
		pSecond2.x() * (pSecond1.y() - pFirst1.y()) +
		pFirst1.x() * (pSecond2.y() - pSecond1.y());
	line2 = pSecond1.x() * (pFirst2.y() - pSecond2.y()) + 
		pSecond2.x() * (pSecond1.y() - pFirst2.y()) +
		pFirst2.x() * (pSecond2.y() - pSecond1.y());
	if (((line1 ^ line2) >= 0) && !(line1 == 0 && line2 == 0))
		return false;
	return true;
}

bool PolygonCMFSegmentIntersector::IsPointInPolygon(const std::vector<osg::Vec3d> &poly,const osg::Vec3d &pt)
{
	int i,j;
	bool c = false;
	for (i = 0,j = poly.size() - 1;i < poly.size();j = i++)
	{
		if ((((poly[i].y() <= pt.y()) && (pt.y() < poly[j].y())) 
			||((poly[j].y() <= pt.y()) && (pt.y() < poly[i].y()))) && (pt.x() < (poly[j].x() - poly[i].x()) * (pt.y() - poly[i].y())/(poly[j].y() - poly[i].y()) + poly[i].x()))
		{
			c = !c;
		}
	}
	return c;
}
