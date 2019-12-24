#pragma once
#include "AnalysisBase.h"
#include <GsReference.h>
#include <geometry.h>

GLOBE_NS

class Element;
// 编辑工具，激活使用可以选中对象并进行编辑
class GS_API EditElmentAnalysis :public AnalysisBase
{
public:
	EditElmentAnalysis();

	virtual ~EditElmentAnalysis();


	 void SetEnable(bool enable);
	 void SetOpenMouseMove(bool enable);
	 bool Enable();
	 // 设置选中并编辑的element 
	 void SetSelectElment(Element* edit,bool isEvent);

	UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Element*, GLOBE_NAME::Element*)>& OnSelectEvent();

	UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Element*)>&OnEditElementEvent();
	// 结束编辑的消息事件
	UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Element*)>&OnEndEditElementEvent();
	// 删除element 对象的过程
	UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Element*)>& OnDeleteElementEvent();
private:

};

GS_SMARTER_PTR(EditElmentAnalysis);

GLOBE_ENDNS