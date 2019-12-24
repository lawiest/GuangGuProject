
#include "StreamMovieElement.h"
#include "OsgCoreSingleton.h"
#include "innerimp.h"
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ImageSequence>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgEarth/GeoTransform>
#include <osgEarth/MapNode>

GLOBE_NS

extern "C"
{
	#include "libavcodec/avcodec.h"
	#include "libswscale/swscale.h"
}


//图片内存转换YUV->bmp 宽高为1920*1080
bool YV12ToBGR24_FFmpeg(unsigned char* pYUV, unsigned char* pBGR24, int width, int height)
{
	if (width < 1 || height < 1 || pYUV == NULL || pBGR24 == NULL)
		return false;
	//int srcNumBytes,dstNumBytes;
	//uint8_t *pSrc,*pDst;
	AVPicture pFrameYUV, pFrameBGR;

	//pFrameYUV = avpicture_alloc();
	//srcNumBytes = avpicture_get_size(PIX_FMT_YUV420P,width,height);
	//pSrc = (uint8_t *)malloc(sizeof(uint8_t) * srcNumBytes);
	avpicture_fill(&pFrameYUV, pYUV, AV_PIX_FMT_YUV420P, width, height);

	//U,V互换
	uint8_t * ptmp = pFrameYUV.data[1];
	pFrameYUV.data[1] = pFrameYUV.data[2];
	pFrameYUV.data[2] = ptmp;

	//pFrameBGR = avcodec_alloc_frame();
	//dstNumBytes = avpicture_get_size(PIX_FMT_BGR24,width,height);
	//pDst = (uint8_t *)malloc(sizeof(uint8_t) * dstNumBytes);
	avpicture_fill(&pFrameBGR, pBGR24, AV_PIX_FMT_BGR24, width, height);

	struct SwsContext* imgCtx = NULL;
	imgCtx = sws_getContext(width, height, AV_PIX_FMT_YUV420P, width, height, AV_PIX_FMT_BGR24, SWS_BILINEAR, 0, 0, 0);

	if (imgCtx != NULL) 
	{
		sws_scale(imgCtx, pFrameYUV.data, pFrameYUV.linesize, 0, height, pFrameBGR.data, pFrameBGR.linesize);
		if (imgCtx) 
		{
			sws_freeContext(imgCtx);
			imgCtx = NULL;
		}
		return true;
	}
	else 
	{
		sws_freeContext(imgCtx);
		imgCtx = NULL;
		return false;
	}
}

osg::StateSet* StreamMovieElement::createState()
{
	osg::ref_ptr<osg::ImageSequence> imageSequence = new osg::ImageSequence;
	imageSequence->setLoopingMode(osg::ImageStream::NO_LOOPING);
	imageSequence->setMode(osg::ImageSequence::PAGE_AND_DISCARD_USED_IMAGES);
	imageSequence->setLength(6 * 1 / 30.0);

	// start the image sequence playing
	imageSequence->play();

	m_ptrImageSequence = new  ReferenceImp<osg::ImageSequence>(imageSequence);

#if 1
	osg::Texture2D* texture = new osg::Texture2D;
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	texture->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT);
	texture->setResizeNonPowerOfTwoHint(false);
	texture->setImage(imageSequence.get());
	//texture->setTextureSize(512,512);
#else
	osg::TextureRectangle* texture = new osg::TextureRectangle;
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	texture->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT);
	texture->setImage(imageSequence.get());
	//texture->setTextureSize(512,512);
#endif

	// create the StateSet to store the texture data
	osg::StateSet* stateset = new osg::StateSet;
	stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
	stateset->setAttributeAndModes(new osg::Program, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	return stateset;
}

void StreamMovieElement::setRectangle(int nWidth, int nHeight)
{
	osg::ref_ptr<osg::MatrixTransform> ptrMat = m_ptrTransNode->CastTo<osg::ref_ptr<osg::MatrixTransform> >();
	if (ptrMat == NULL)
		return;
 	osg::Geode* ptrGeode = dynamic_cast<osg::Geode*>(ptrMat->getChild(0));
	if (ptrGeode == NULL)
		return;
	osg::Geometry* ptrGeo = ptrGeode->getDrawable(0)->asGeometry();
	if (ptrGeode == NULL)
		return;

	osg::Vec3 corner = osg::Vec3(0.0f - nWidth / m_dbFactor / 2.0, 0.0f - nHeight / m_dbFactor / 2.0, 0.0);
	osg::Vec3 widthVec = osg::Vec3(nWidth / m_dbFactor, 0.0f, 0.0);
	osg::Vec3 heightVec = osg::Vec3(0.0f, nHeight / m_dbFactor, 0.0f);

	osg::Vec3Array* ptrVertex =dynamic_cast<osg::Vec3Array*>(ptrGeo->getVertexArray());
	if (ptrVertex == NULL)
		return;

	(*ptrVertex)[0] = corner + heightVec;
	(*ptrVertex)[1] = corner;
	(*ptrVertex)[2] = corner + widthVec;
	(*ptrVertex)[3] = corner + widthVec + heightVec;

	ptrGeo->dirtyDisplayList();
	ptrGeo->dirtyBound();
}

StreamMovieElement::StreamMovieElement(GsRawPoint3D position, int nWidth, int nHeight, double dbFactor)
{
	m_bVisible = true;
	m_dbFactor = dbFactor;
	m_xRot = m_yRot = m_zRot = 0;

	// create the geometry of the model, just a simple 2d quad right now.
	osg::Geode* geode = new osg::Geode;
	osg::Geometry* ptrGeometry = osg::createTexturedQuadGeometry(osg::Vec3(0.0f- nWidth / dbFactor/2.0, 0.0f- nHeight / dbFactor/2.0, 0.0), 
																osg::Vec3(nWidth/dbFactor, 0.0f, 0.0), 
																osg::Vec3(0.0f, nHeight / dbFactor, 0.0f),0.f,0.f);
	osg::Vec2Array* ptrTextCoord =dynamic_cast<osg::Vec2Array*>(ptrGeometry->getTexCoordArray(0));
	(*ptrTextCoord)[3].set(1, 0);
	(*ptrTextCoord)[2].set(1, 1);
	(*ptrTextCoord)[1].set(0, 1);
	(*ptrTextCoord)[0].set(0, 0);

	geode->addDrawable(ptrGeometry);

	geode->setStateSet(createState());

	osg::ref_ptr<osgEarth::GeoTransform> ptrLocatorMovie = new osgEarth::GeoTransform();
	osgEarth::GeoPoint point = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), osg::Vec3d(position.X, position.Y, position.Z - 500));
	ptrLocatorMovie->setPosition(point);
	m_ptrElementHandle = new ReferenceImp<osgEarth::GeoTransform>(ptrLocatorMovie);

	osg::ref_ptr<osg::MatrixTransform> ptrMat = new osg::MatrixTransform();
	ptrMat->addChild(geode);

	ptrLocatorMovie->addChild(ptrMat);

	OsgCoreSingleton::Instance()->MapNode()->addChild(ptrLocatorMovie);

	m_ptrTransNode = new ReferenceImp<osg::MatrixTransform>(ptrMat);

	m_ElementType = eStreamMovieElement;
}

void StreamMovieElement::AddImage(unsigned char* data, int nWidth, int nHeight)
{
	osg::ref_ptr<osg::Image>image = new osg::Image;
	unsigned char*pRGBData;
	pRGBData = new unsigned char[nWidth * nHeight * 3];
	YV12ToBGR24_FFmpeg(data, pRGBData, nWidth, nHeight);
	//Mode为USE_NEW_DELETE时应该会把指针释放，会把传入的pRGBData释放
	image->setImage(nWidth, nHeight, 1, 3, GL_RGB, GL_UNSIGNED_BYTE, pRGBData, osg::Image::USE_NEW_DELETE);

	m_ptrImageSequence->CastTo<osg::ref_ptr<osg::ImageSequence> >()->addImage(image);
}

void StreamMovieElement::setPosition(GsRawPoint3D pos)
{
	osgEarth::GeoPoint point = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), osg::Vec3d(pos.X, pos.Y, pos.Z));
	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::GeoTransform> >()->setPosition(point);
}

GsRawPoint3D StreamMovieElement::getPosition()
{
	osgEarth::GeoPoint vecPos = m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::GeoTransform> >()->getPosition();

	return GsRawPoint3D(vecPos.x(), vecPos.y(), vecPos.z());
}

bool StreamMovieElement::Visible()
{
	return m_bVisible;
}

void StreamMovieElement::Visible(bool bVisible)
{
	m_bVisible = bVisible;

	//if (m_bVisible)
	//	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Util::ObjectLocatorNode>>()->setNodeMask(1);
	//else
	//	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::Util::ObjectLocatorNode>>()->setNodeMask(0);
}

void StreamMovieElement::Rotate(GsRawPoint3D gsAxle, double dbDegree)
{
	if (gsAxle.X != 0)
		m_xRot = dbDegree;
	if (gsAxle.Y != 0)
		m_yRot = dbDegree;
	if (gsAxle.Z != 0)
		m_zRot = dbDegree;
	osg::ref_ptr<osg::MatrixTransform> ptrMat = m_ptrTransNode->CastTo<osg::ref_ptr<osg::MatrixTransform> >();
	ptrMat->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(m_xRot), osg::Vec3d(1, 0, 0), osg::DegreesToRadians(m_yRot), osg::Vec3d(0, 1, 0), osg::DegreesToRadians(m_zRot), osg::Vec3d(0, 0, 1)));
}


StreamMovieElement::~StreamMovieElement()
{

}

GLOBE_ENDNS
