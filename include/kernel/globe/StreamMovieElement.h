#pragma once
#include "Element.h"
#include <GsReference.h>

namespace osg
{
	class StateSet;
}

GLOBE_NS

/*该类提供了一个播放视频流的基本元素*/
class GS_API  StreamMovieElement :public Element
{
public:
	/*
	position：该视频流播放的位置
	nWidth：视频宽
	nHeight：视频高
	dbFactor：缩放因子，视频尺寸会等比例根据该值缩放(该值越大，视频播放尺寸越小)
	*/
	StreamMovieElement(GsRawPoint3D position, int nWidth, int nHeight, double dbFactor);
	virtual~StreamMovieElement();

	//设置位置。单位为经纬度和高度
	void setPosition(GsRawPoint3D pos);
	GsRawPoint3D getPosition();

	bool Visible();
	void Visible(bool bVisible);

	/*
	gsAxle：旋转轴，一般情况下只需要z轴旋转(即GsRawPoint3D(0.0, 0.0, 1.0))
	dbDegree：旋转角度(角度制,如45度)
	*/
	void Rotate(GsRawPoint3D gsAxle, double dbDegree);

	void setRectangle(int nWidth, int nHeight);

	/*
	data：图片内存块(目前只支持YUV格式,理论上可以支持所有常见格式(其他格式暂未考虑)，使用ffmpeg解码)
	nWidth：图片宽
	nHeight：图片高
	*/
	void AddImage(unsigned char* data ,int nWidth, int nHeight);

private:

	osg::StateSet* createState();

	double m_dbFactor;
	bool m_bVisible;
	GsReferencePtr m_ptrTransNode;
	GsReferencePtr m_ptrImageSequence;
	double m_xRot;// 姿态旋转的
	double m_yRot;
	double m_zRot;
};

GS_SMARTER_PTR(StreamMovieElement);

GLOBE_ENDNS