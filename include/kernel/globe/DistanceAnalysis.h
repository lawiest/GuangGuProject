#pragma once
#include <AnalysisBase.h>

GLOBE_NS

class AnalysisCallBack;

class GS_API DistanceAnalysis :public AnalysisBase
{
public:
	DistanceAnalysis();

	virtual ~DistanceAnalysis();

	void Path(bool bPath);
	bool Path() const;

	UTILITY_NAME::GsDelegate<void(double)>& OnFinish();

private:

};

GS_SMARTER_PTR(DistanceAnalysis);

GLOBE_ENDNS
