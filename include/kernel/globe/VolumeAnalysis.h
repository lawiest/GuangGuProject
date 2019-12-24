#pragma once
#include <AnalysisBase.h>

GLOBE_NS

class Layer;

//基于倾斜数据体积测量分析，由于比较耗时内部使用多线程计算
class GS_API VolumeAnalysis :public AnalysisBase
{
public:
	/*
	ptrLayer：倾斜图层
	bAutoCalc：是否自动计算，默认为true，如果设为false，当画完线时不会计算，此时需要手动调用Calculate方法
	dbElevation：海拔高度，体积会以此海拔为基准计算，默认为0.0
	*/
	VolumeAnalysis(Layer* ptrLayer, bool bAutoCalc = true, double dbElevation = 0.0);

	/*
	dbElevation：海拔高度，体积会以此海拔为基准计算
	*/
	void Calculate(double dbElevation);

	virtual ~VolumeAnalysis();

	//绘制结束事件
	UTILITY_NAME::GsDelegate<void()>& OnDrawFinish();
	//计算完成
	UTILITY_NAME::GsDelegate<void(double)>& OnCalcFinish();

private:

};

GS_SMARTER_PTR(VolumeAnalysis);

GLOBE_ENDNS