#pragma once
#include <osgEarth/Viewpoint>
#include <osgEarth/Geodata>
#include "model\LodModel.h"
#include "SchemeComparisonAnalysis/ModelFc.h"
class PolygonCMFTrakerElement3D;

namespace GeoGlobe
{
	class PolygonTrakerElement3D;

	class IMainWindow
	{
	public:
		virtual void setModelNode(osg::ref_ptr<GeoGlobe::Model::LodModel> modelnode) = 0;
		virtual void setRelocationData(std::map<int64, osg::ref_ptr<GeoGlobe::Model::LodModel>> mapModel, osg::Group* group, GeoGlobe::PolygonTrakerElement3D* Ele, std::map<int64, osg::ref_ptr<osg::StateSet>> mapState) = 0;
		virtual void setCMFRelocationData(std::map<int64, osg::ref_ptr<CModelF>> mapModel, osg::Group* group, PolygonCMFTrakerElement3D* cmfEle, std::map<int64, osg::ref_ptr<osg::StateSet>> mapState) = 0;

	};
	class IConsultationWindow
	{
        public:
			virtual void AddModelHightLightStep(unsigned long long nkey) = 0;
			virtual bool IsPickUpFinished() = 0;
            virtual void AddLocateStep(osgEarth::Viewpoint vp) = 0;
            virtual bool IsClosed() = 0;
			virtual void ObserverPoints(osgEarth::GeoPoint screenStart, osgEarth::GeoPoint screenEnd) = 0;

	};
} // namespace GeoGlobe
 
