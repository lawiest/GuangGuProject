#ifndef _POLYGONSEGEMENTINTERSECTOR_H_
#define _POLYGONSEGEMENTINTERSECTOR_H_ 1

#include <SchemeComparisonAnalysis/PolyInter/IntersectionVisitorCMF.h>
#include <osgEarth/GeoData>


class PolygonCMFSegmentIntersector : public Intersector
{
	public:
		PolygonCMFSegmentIntersector(std::vector<osg::Vec3d>& vecGeoPts);

		struct Intersection
		{
			Intersection():
				ratio(-1.0),
				primitiveIndex(0) {}

			bool operator < (const Intersection& rhs) const { return ratio < rhs.ratio; }

			typedef std::vector<unsigned int>   IndexList;
			typedef std::vector<double>         RatioList;

			double                          ratio;
			osg::NodePath                   nodePath;
			osg::ref_ptr<osg::Drawable>     drawable;
			osg::ref_ptr<osg::RefMatrix>    matrix;
			IndexList                       indexList;
			RatioList                       ratioList;
			unsigned int                    primitiveIndex;      
		};

		typedef std::multiset<Intersection> Intersections;

		inline void insertIntersection(const Intersection& intersection) { getIntersections().insert(intersection); }

		inline Intersections& getIntersections() { return _parent ? _parent->_intersections : _intersections; }

		inline Intersection getFirstIntersection() { Intersections& intersections = getIntersections(); return intersections.empty() ? Intersection() : *(intersections.begin()); }

	public:

		virtual Intersector* clone(IntersectionVisitorCMF& iv);

		virtual bool enter(const osg::Node& node);

		virtual void leave();

		virtual void intersect(IntersectionVisitorCMF& iv, osg::Node* node);

		virtual void reset();

		virtual bool containsIntersections() { return !getIntersections().empty(); }


	protected:

		bool intersects(const osg::BoundingSphere& bs);

		PolygonCMFSegmentIntersector* _parent;

		Intersections _intersections;

		std::vector<osg::Vec3d> _vecGeoPoint;

		//两个多边形求交
		bool PolygonClip(const std::vector<osg::Vec3d> &poly1,const std::vector<osg::Vec3d> &poly2);
		bool GetCrossPoint(const osg::Vec3d &p1,const osg::Vec3d &p2,const osg::Vec3d &q1,const osg::Vec3d &q2,long &x,long &y);
		bool IsRectCross(const osg::Vec3d &p1,const osg::Vec3d &p2,const osg::Vec3d &q1,const osg::Vec3d &q2);
		bool IsLineSegmentCross(const osg::Vec3d &pFirst1,const osg::Vec3d &pFirst2,const osg::Vec3d &pSecond1,const osg::Vec3d &pSecond2);
		bool IsPointInPolygon(const std::vector<osg::Vec3d> &poly,const osg::Vec3d &pt);

};

#endif