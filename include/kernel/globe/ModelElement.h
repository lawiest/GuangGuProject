
#pragma once

#include "Element.h"
#include <GsReference.h>

GLOBE_NS

class GS_API ModelElement :public Element
{
public:
	ModelElement(std::string strUrl, double x, double y, double z);

	virtual~ModelElement();
	virtual bool Visible() ;
	virtual void Visible(bool bVisible);
	//设置位置
	virtual	void setPosition(double x, double y, double z);
	virtual void getPosition(double& x, double& y, double& z)override;
	std::string getModelUrl();
	//获得位置
	GsRawPoint3D getPosition();
	//绕着gsAxle轴旋转dbAngle角度
	void Rotate(GsRawPoint3D gsAxle, double dbAngle);
	double getdbAngle();
	//缩放原始大小*dbScale倍
	void Scale(double dbScale);
	double getdbScale();

	void setTranslate(double x, double y, double z);

private:
	GsReferencePtr m_ptrMatrix;
	GsReferencePtr m_ptrModelNode;
	double m_dbAngle;
	double m_dbScale;
	std::string m_modelUrl;
	GsRawPoint3D m_rotaAxle;
	bool m_bVisible;
};

GS_SMARTER_PTR(ModelElement);

GLOBE_ENDNS