#pragma once
#include "Element.h"
#include <canvas.h>
#include <GsReference.h>

GLOBE_NS

class GS_API SquareElement : public Element
{
public:
	SquareElement(float w,float h);
	SquareElement(float w, float h, const char* image, LinePictureMixType type=RRR);
	virtual~SquareElement();
	//
	void setColor(const KERNEL_NAME::GsColor& color);
	KERNEL_NAME::GsColor getColor();

	void setPosition(double x, double y, double z);
	KERNEL_NAME::GsRawPoint3D getPosition();
	virtual void getPosition(double& x, double& y, double& z)override;

	virtual bool Visible();
	virtual void Visible(bool bVisible);
	// 设置动画执行者
	void SetAnimatorAction(AnimatorAction* action);
	// 线的动画执行者
	AnimatorAction* GetAnimatorAction();
protected:
	void Rebuild();
private:
	GsReferencePtr m_ptrSquaRef;
	GsReferencePtr m_ptrTransRef;
	GsReferencePtr m_ptrLocatorRef;
};

GS_SMARTER_PTR(SquareElement);

GLOBE_ENDNS

