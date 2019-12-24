#pragma once

#include <utility.h> 

GLOBE_NS

class GS_API AnalysisCallBack :public GeoStar::Utility::GsRefObject
{
public:
	AnalysisCallBack();
	
	virtual void OnFinish(double dbResult) {};
	virtual ~AnalysisCallBack();
};

GS_SMARTER_PTR(AnalysisCallBack);

GLOBE_ENDNS


