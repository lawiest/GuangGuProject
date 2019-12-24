#pragma once
#include <utility.h>
#include <GsReference.h>

GLOBE_NS

enum AnalysisType
{
	eHeighAnalysis,
	eDistanceAnalysis,
	eAreaAnalysis,
	eOPInRangeAnalysis,
	eLineOfSightAnalysis,
	eMultipleLineOfSightAnalysis,
	eRadarAnalysis,
	eAreaAnalysis3D,
	eDistanceAnalysis3D,
	eMarathonAnalysis,
	eTerrainProfileAnalysis,
	eDrawElementTool,
	eEditElmentAnalysis,
	eVolumeAnalysis,
	eOsgbProfileAnalysis
};

class GS_API AnalysisBase :public GeoStar::Utility::GsRefObject
{
public:
	AnalysisBase();
	virtual ~AnalysisBase();

	GsReference* Handle()
	{
		return m_ptrAnalysisHandle.p;
	}

	AnalysisType Type();

protected:
	GsReferencePtr m_ptrAnalysisHandle;
	AnalysisType m_AnalysisType;

};

GS_SMARTER_PTR(AnalysisBase);

GLOBE_ENDNS

