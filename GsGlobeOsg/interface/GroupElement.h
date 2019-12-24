#pragma once
#include "Element.h"

namespace osg
{
	class Node;
}

GLOBE_NS

class GS_API GroupElement :public Element
{
public:
	GroupElement(int minLod=0, int maxLod= INT_MAX);
	virtual ~GroupElement();

	void AddElement(Element * ptrElement);
	void RemoveElement(Element * ptrElement);

	void RemoveAllElement();

	void SetLod(int minLod, int maxLod);
	int GetMinLod();
	int GetMaxLod();

	int ElementsSize();
	Element* GetElement(int index);

	virtual bool Visible();
	virtual void Visible(bool bVisible);
	// 内部调用外部请勿调用
	Element * findByNode(osg::Node* node);

private:

	std::vector<ElementPtr> m_vecElement;
	bool m_visible;
	int m_minLod;
	int m_maxLod;

};

GS_SMARTER_PTR(GroupElement);

GLOBE_ENDNS
