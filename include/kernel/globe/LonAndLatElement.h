#pragma once
#include "Element.h"
#include <GsReference.h>

GLOBE_NS

class GS_API LonAndLatElement : public Element
{
public:

	void setText(std::string strText);
	std::string getText();

	virtual void Visible(bool bVisible);
	virtual bool Visible();

	LonAndLatElement();
	virtual~LonAndLatElement();
	void creatGeoInfoBox();
	void Build();

	UTILITY_NAME::GsDelegate<void(Element*)>& OnClicked();
	UTILITY_NAME::GsDelegate<void()>& Change();
	UTILITY_NAME::GsDelegate<void(KERNEL_NAME::GsRawPoint3D point)>& MouseMovePoint();

protected:
	UTILITY_NAME::GsDelegate<void(Element*)> Clicked;
};
GS_SMARTER_PTR(LonAndLatElement);

GLOBE_ENDNS