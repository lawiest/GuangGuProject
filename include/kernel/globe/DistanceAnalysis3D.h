#pragma once
#include "AnalysisBase.h"

GLOBE_NS


class GS_API DistanceAnalysis3D :public AnalysisBase
{
public:
	DistanceAnalysis3D();

	virtual ~DistanceAnalysis3D();

	UTILITY_NAME::GsDelegate<void(double)>& OnFinish();

private:

};

GS_SMARTER_PTR(DistanceAnalysis3D);

GLOBE_ENDNS