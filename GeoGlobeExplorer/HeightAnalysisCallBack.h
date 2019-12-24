#pragma once
#include <AnalysisCallBack.h>

class HeightAnalysisCallBack : public GLOBE_NAME::AnalysisCallBack
{
public:
	HeightAnalysisCallBack();
	~HeightAnalysisCallBack();

	virtual void OnFinish(double dbResult);

protected:
	double m_dbResult;
};

GS_SMARTER_PTR(HeightAnalysisCallBack);