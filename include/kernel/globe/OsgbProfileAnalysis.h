#pragma once
#include <AnalysisBase.h>

GLOBE_NS

class Layer;

//基于倾斜数据剖面分析
class GS_API OsgbProfileAnalysis :public AnalysisBase
{
public:
	/*
	ptrLayer：倾斜图层
	bAutoCalc：是否自动计算，默认为true，如果设为false，当画完线时不会计算，此时需要手动调用Calculate方法
	dbStep：插值密度，默认为一百，该值越大插点越密、结果越准确、耗时越久
	*/
	OsgbProfileAnalysis(Layer* ptrLayer, bool bAutoCalc = true, double dbStep = 100.0);

	/*
	dbStep：插值密度、该值越大插点越密、结果越准确、耗时越久
	*/
	void Calculate(double dbStep = 100.0);

	virtual ~OsgbProfileAnalysis();

	//绘制结束事件
	UTILITY_NAME::GsDelegate<void()>& OnDrawFinish();
	//计算完成
	UTILITY_NAME::GsDelegate<void(const std::vector<double>&, const std::vector<double>&)>& OnCalcFinish();

private:

};

GS_SMARTER_PTR(OsgbProfileAnalysis);

GLOBE_ENDNS