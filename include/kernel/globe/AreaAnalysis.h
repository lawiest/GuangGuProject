
#pragma once
#include <AnalysisBase.h>

GLOBE_NS

class GS_API AreaAnalysis :public AnalysisBase
{
public:
	AreaAnalysis();

	virtual ~AreaAnalysis();

	UTILITY_NAME::GsDelegate<void(double)>& OnFinish();

private:

};

GS_SMARTER_PTR(AreaAnalysis);

GLOBE_ENDNS   //}}}


