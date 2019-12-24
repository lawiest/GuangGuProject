#pragma once
#include "Element.h"
#include <GsReference.h>
#include "geometry.h"
#include "canvas.h"

GLOBE_NS


enum TextureType
{
	//贴图
	REAPTE=3,
	TO_EDGE=4
};

class GS_API LinePictureElement3D : public Element
{
public:
	LinePictureElement3D(float w,float pictureH,const char* image);
	LinePictureElement3D(float w, float pictureH, const char* image,LinePictureMixType type,TextureType textureType=REAPTE);
	virtual~LinePictureElement3D();
	// 设置点
	void setPoints(const std::vector<KERNEL_NAME::GsRawPoint3D>& points);
	// 添加点
	void AddPoint(KERNEL_NAME::GsRawPoint3D& point);
	KERNEL_NAME::GsRawPoint3D GetPoint(int index);
	int PointSize();
	std::vector<KERNEL_NAME::GsRawPoint3D> GetPoints();
	float GetLineScale();
	void SetLineScale(float);
	// 线的填充图片路径
	std::string GetImageUrl();
	// 获取线宽
	float GetLineWidth();
	// 设置线宽
	void SetLineWidth(float width);
	// 获取线图片的高度
	float GetImageHeight();
	//是否可见
	virtual bool Visible();
	// 设置visible 可见
	virtual void Visible(bool bVisible);
	//
	void SetLineColor(const KERNEL_NAME::GsColor& color);
	KERNEL_NAME::GsColor GetLineColor();
	// 设置
	void SetFixLod(float fixlod);
	// lod 的模式情况
	float GetFixLod();
	// 获取混合样式的过程
	// 若为COLOR 模式则可以设置线的颜色过程
	LinePictureMixType GetPictureMixType();
	// 设置动画执行者
	void SetAnimatorAction(AnimatorAction* action);
	// 线的动画执行者
	AnimatorAction* GetAnimatorAction();

protected:
	void reBuild();
private:
	GsReferencePtr m_ptrLinesGeom;
	GsReferencePtr m_ptrNodeLineGeom;
	GsReferencePtr m_ptrTranForm;
	GsReferencePtr m_ptrGeode;
	std::vector<KERNEL_NAME::GsRawPoint3D> m_linePoints;
	std::string m_imageUrl;
	LinePictureMixType m_MixType;

};

GS_SMARTER_PTR(LinePictureElement3D);

GLOBE_ENDNS