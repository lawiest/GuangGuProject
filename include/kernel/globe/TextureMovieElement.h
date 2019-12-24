
#pragma once
#include "Element.h"
#include <GsReference.h>

GLOBE_NS

enum TexturePlane
{
	XYPlane,
	XZPlane
};

/*
纹理视频元素，可在这张纹理上播放视频(只支持本地)
*/
class GS_API TextureMovieElement :public Element
{
public:
	/*
	strURL：视频路径
	position：视频位置(经纬度)
	dbScale：缩放值(此值越大视频尺寸越小)
	plane：指定平面(默认在XY平面，如果换成XZ平面会竖起来)
	*/
	TextureMovieElement(std::string strURL, GsRawPoint3D position, double dbScale = 60.0, TexturePlane plane = XYPlane);
	const TexturePlane& getTexturePlane();
	
	//设置位置。单位为经纬度和高度
	void setPosition(GsRawPoint3D pos);
	GsRawPoint3D getPosition();

	/*
	gsAxle：旋转轴，一般情况下只需要z轴旋转(即GsRawPoint3D(0.0, 0.0, 1.0))
	dbDegree：旋转角度(角度制,如45度)
	*/
	void Rotate(GsRawPoint3D gsAxle,double dbDegree);

	virtual~TextureMovieElement();

private:

	GsReferencePtr m_ptrTransNode;
	TexturePlane m_eTexturePlane;
};

GS_SMARTER_PTR(TextureMovieElement);

GLOBE_ENDNS