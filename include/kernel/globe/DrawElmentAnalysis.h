#pragma once
#include "AnalysisBase.h"
#include <GsReference.h>
#include <geometry.h>
GLOBE_NS

// 绘制类型
enum DrawElementType
{
	// 绘制点
	DRAWPOINT=1,
	// 绘制线
	DRAWLINE,
	// 绘制面
	DRAWPOLYGON,
	// 绘制矩形
	DRAWRECT
};

class GS_API DrawElmentAnalysis :public AnalysisBase
{
public:
	DrawElmentAnalysis();

	virtual ~DrawElmentAnalysis();


	 void SetEnable(bool enable);
	 void SetMonitorMouseMove(bool enable);
	 bool Enable();

	/// \brief 获取绘制多段线需要的最小点的个数
	/// \return 返回绘制多段线需要的最小点个数
	int MinPointCount();

	/// \brief 设置绘制多段线需要最小点个数
	/// \param min 输入的值
	void MinPointCount(int min);
	/// \brief 获取需要的最大点个数
	///
	int MaxPointCount();
	/// \brief 设置绘制需要的最大值
	/// \param max 输入的值
	void MaxPointCount(int max);

	UTILITY_NAME::GsDelegate<void(const KERNEL_NAME::GsRawPoint3D&)> &OnMouseDoubleClickEvent();
	UTILITY_NAME::GsDelegate<void(const std::vector<KERNEL_NAME::GsRawPoint3D>& )> &OnDrawFinishEvent();

private:

};

GS_SMARTER_PTR(DrawElmentAnalysis);


//

class GS_API DrawRectElmentAnalysis :public DrawElmentAnalysis
{
public:
	DrawRectElmentAnalysis();
	virtual ~DrawRectElmentAnalysis();
	// 获取绘制类型
	DrawElementType GetDrawType();
	// 设置绘制类型
	void SetDrawType(DrawElementType type);

	//判断键盘是否按下
	bool isKeyPress(int key);
};

GS_SMARTER_PTR(DrawRectElmentAnalysis);
GLOBE_ENDNS