#include "AreaAnalysis.h"
#include "innerimp.h"
#include "Analysis/PolygonTrakerElement3D.h"
#include "Analysis/AreaTrakerElement.h"
#include "OsgCoreSingleton.h"

GLOBE_NS

AreaAnalysis::AreaAnalysis()
{
	osg::ref_ptr<GeoGlobe::AreaTrakerElement> ptrAreaAnalysis = new GeoGlobe::AreaTrakerElement();
	m_ptrAnalysisHandle = new ReferenceImp<GeoGlobe::AreaTrakerElement>(ptrAreaAnalysis);
	m_AnalysisType = eAreaAnalysis;
}

UTILITY_NAME::GsDelegate<void(double)>& AreaAnalysis::OnFinish()
{
	return Handle()->CastTo<osg::ref_ptr<GeoGlobe::AreaTrakerElement> >()->OnFinish;
}

AreaAnalysis::~AreaAnalysis()
{

}

GLOBE_ENDNS