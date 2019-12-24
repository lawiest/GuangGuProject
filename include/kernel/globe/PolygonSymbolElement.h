#pragma once
#include "Element.h"
#include <canvas.h>
#include <globe/GsReference.h>

GLOBE_NS
// 基本面类型
class GS_API PolygonSymbolElement : public Element
{
public:

	PolygonSymbolElement();
	virtual~PolygonSymbolElement();
	void AddPoint(double x, double y, double z);
	//点颜色，r[0-255],g[0-255],b[0-255],a[0-255]
	//void setLineColor(int r, int g, int b, int a = 255);
	void setLineColor(const KERNEL_NAME::GsColor& lineColor);
	// 获取线的颜色
	KERNEL_NAME::GsColor getLineColor();
	//面填充颜色，r[0-255],g[0-255],b[0-255],a[0-255]
	//void setFillColor(int r, int g, int b, int a = 255);
	void setFillColor(const KERNEL_NAME::GsColor& fillcolor);
	// 获取面填充颜色
	KERNEL_NAME::GsColor getFillColor();
	// 立面颜色设置
	void setFaceColor(const KERNEL_NAME::GsColor& faceColor);
	//获取立面颜色的
	KERNEL_NAME::GsColor getFaceColor();
	//获取立面高度,默认高度为0 则不显示立面
	void setFaceHeight(float height);
	//获取立面高度
	float getFaceHeight();
	// 开启是否遮盖的属性
	bool isDepthTest();
	// 设置是否遮挡的过程
	void setDepthTest(bool cover);
	//void ClearFillColor();
	// 设置线宽
	//void setLineSize(int nSize);
	virtual bool Visible();
	virtual void Visible(bool bVisible);
	// 设置线宽
	void setLineWidth(float width);
	//获取线宽
	float getLineWidth();
	
	// 获取点的坐标点
	const std::vector<KERNEL_NAME::GsRawPoint3D>& getPoints();
	// 设置一系列的坐标点
	void setPoints(const std::vector<KERNEL_NAME::GsRawPoint3D>& points);

protected:
	// 
	void reBuild();

protected:
	std::vector<KERNEL_NAME::GsRawPoint3D> m_Points;
	KERNEL_NAME::GsColor m_FillColor;
	KERNEL_NAME::GsColor m_LineColor;
	KERNEL_NAME::GsColor m_FaceColor;//立面颜色
	float m_LineWidth;
	float m_VerFaceHeight;//立面高度
	bool m_isDepthTest;// 是否开启遮挡
	GsReferencePtr m_ptrLinesGeom;
	GsReferencePtr m_ptrPolygonGeom;//面的
	GsReferencePtr m_ptrFacePolyGeom;//立面绘制的
	GsReferencePtr m_ptrGeode;
};

GS_SMARTER_PTR(PolygonSymbolElement);

GLOBE_ENDNS