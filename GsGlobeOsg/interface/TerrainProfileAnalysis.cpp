#include "TerrainProfileAnalysis.h"
#include "innerimp.h"
#include "Analysis/TerrainProfileGraph.h"
#include "OsgCoreSingleton.h"

GLOBE_NS

TerrainProfileAnalysis::TerrainProfileAnalysis()
{
	osg::ref_ptr<GeoGlobe::TerrainProfileGraph> ptrTerrainProfileAnalysis = new GeoGlobe::TerrainProfileGraph(OsgCoreSingleton::Instance()->MapNode());

	m_ptrAnalysisHandle = new ReferenceImp<GeoGlobe::TerrainProfileGraph>(ptrTerrainProfileAnalysis);
	ptrTerrainProfileAnalysis->OnFinish.Add(this, &TerrainProfileAnalysis::OnFinishListener);
	m_AnalysisType = eTerrainProfileAnalysis;
}

TerrainProfileAnalysis::~TerrainProfileAnalysis()
{

}

void TerrainProfileAnalysis::OnFinishListener(std::list<double>& DistanceVal, std::list<double>& ElevationVal)
{
	OnTerrainAnalysisEvents(DistanceVal, ElevationVal, this);
}

// std::vector<double> TerrainProfileAnalysis::GetDistanceVec()
// {
// 	return ptrTerrainProfileAnalysis->DistanceVec();
// }
// 
// std::vector<double> TerrainProfileAnalysis::GetElevationVec()
// {
// 	return ptrTerrainProfileAnalysis->ElevationVec();
// }

GLOBE_ENDNS