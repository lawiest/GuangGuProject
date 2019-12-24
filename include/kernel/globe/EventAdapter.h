#pragma once
#include <utility.h>

GLOBE_NS

class Layer;
class Element;

/*事件适配器，通过事件机制来解决在主线程外加载数据崩溃问题
 *目前只针对图层和Element，可按需扩充
*/
class GS_API EventAdapter :public GeoStar::Utility::GsRefObject
{
public:
	EventAdapter();

	void AddLayer(Layer* ptrLayer);
	void AddElement(Element* ptrElement);

	virtual ~EventAdapter();

private:

};

GS_SMARTER_PTR(EventAdapter);

GLOBE_ENDNS
