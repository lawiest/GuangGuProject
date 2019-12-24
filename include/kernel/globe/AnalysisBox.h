#pragma once
#include <utility.h>
#include "AnalysisBase.h"

namespace osg
{
	class Node;
}

namespace osgViewer
{
	class Viewer;
}

GLOBE_NS

class GS_API AnalysisBox :public GeoStar::Utility::GsRefObject
{
public:
	AnalysisBox();
	virtual ~AnalysisBox();

	void AddAnalysis(AnalysisBase* ptrAnalysis);

	void RemoveAnalysis(AnalysisBase* ptrAnalysis);

	void RemoveAllAnalysis();

	std::list<double> GetTerrainProfileElevation(AnalysisBase* ptrAnalysis);

	std::list<double> GetTerrainProfileDistance(AnalysisBase* ptrAnalysis);

	double GetMaxDistance(AnalysisBase* ptrAnalysis);

	double GetMinElevation(AnalysisBase* ptrAnalysis);

	double GetMaxElevation(AnalysisBase* ptrAnalysis);


private:
	std::vector<AnalysisBasePtr> m_vecAnalysis; 

	GsReferencePtr m_ptrTerrianProfile;

};

GS_SMARTER_PTR(AnalysisBox);

GLOBE_ENDNS
