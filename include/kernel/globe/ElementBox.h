#pragma once

#include <utility.h>
#include <GsReference.h>
#include "Element.h"

namespace osg
{
	class Node;
}
GLOBE_NS

class GS_API ElementBox :public GeoStar::Utility::GsRefObject
{
public:
	ElementBox();
	virtual ~ElementBox();

	void AddElement(Element * ptrElement);
	void RemoveElement(Element * ptrElement);

	void RemoveAllElement();
	// 内部调用外部请勿调用
	Element * findByNode(osg::Node* node);

	Element* LonAndLatElement();
	Element* CompassElement();

private:
	
	std::vector<ElementPtr> m_vecElement;	

};

GS_SMARTER_PTR(ElementBox);

GLOBE_ENDNS