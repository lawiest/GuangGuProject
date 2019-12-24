#pragma once
#include <AnalysisBase.h>
#include <event.h>

GLOBE_NS

class AnalysisCallBack;

class GS_API HeightAnalysis :public AnalysisBase
{
public:
	HeightAnalysis();
	
	virtual ~HeightAnalysis();

	UTILITY_NAME::GsDelegate<void(double)>& OnFinish();

private:

};

GS_SMARTER_PTR(HeightAnalysis);

GLOBE_ENDNS


