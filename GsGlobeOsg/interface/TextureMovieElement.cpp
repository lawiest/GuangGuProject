#include "TextureMovieElement.h"
#include "OsgCoreSingleton.h"
#include "innerimp.h"
#include <osgEarth/GeoTransform>
#include <osg/ImageStream>
#include <osg/TextureRectangle>
#include <osg/Texture2D>
#include <osgearth/MapNode>

osg::Geometry* myCreateTexturedQuadGeometry(const osg::Vec3& pos, float width, float height, osg::Image* image, bool useTextureRectangle, bool xyPlane, bool option_flip)
{
	bool flip = image->getOrigin() == osg::Image::TOP_LEFT;
	if (option_flip) flip = !flip;

	if (useTextureRectangle)
	{
		osg::Geometry* pictureQuad = osg::createTexturedQuadGeometry(pos,
			osg::Vec3(width, 0.0f, 0.0f),
			xyPlane ? osg::Vec3(0.0f, height, 0.0f) : osg::Vec3(0.0f, 0.0f, height),
			0.0f, flip ? image->t() : 0.0, image->s(), flip ? 0.0 : image->t());

		osg::TextureRectangle* texture = new osg::TextureRectangle(image);
		
		texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);


		pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0,
			texture,
			osg::StateAttribute::ON);

		return pictureQuad;
	}
	else
	{
		osg::Geometry* pictureQuad = osg::createTexturedQuadGeometry(pos,
			osg::Vec3(width, 0.0f, 0.0f),
			xyPlane ? osg::Vec3(0.0f, height, 0.0f) : osg::Vec3(0.0f, 0.0f, height),
			0.0f, flip ? 1.0f : 0.0f, 1.0f, flip ? 0.0f : 1.0f);

		osg::Texture2D* texture = new osg::Texture2D(image);
		texture->setResizeNonPowerOfTwoHint(false);
		texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
		texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);


		pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0,
			texture,
			osg::StateAttribute::ON);

		return pictureQuad;
	}
}

GLOBE_NS

const TexturePlane& TextureMovieElement::getTexturePlane()
{
	return m_eTexturePlane;
}

TextureMovieElement::TextureMovieElement(std::string strURL,GsRawPoint3D position, double dbScale, TexturePlane plane)
{
	m_eTexturePlane = plane;
	

	std::string strDLLName = osgDB::Registry::instance()->createLibraryNameForExtension("ffmpeg");
	osgDB::Registry::instance()->loadLibrary(strDLLName);

	osg::Vec3 pos(0.0f, 0.0f, 0.0f);
	osg::Vec3 topleft = pos;
	osg::Vec3 bottomright = pos;

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::StateSet* stateset = geode->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	osg::ref_ptr<osg::Image> image = osgDB::readRefImageFile(strURL);
	osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(image.get());

	if (imagestream)
	{
		osg::ImageStream::AudioStreams& audioStreams = imagestream->getAudioStreams();
		if (!audioStreams.empty())
		{
			osg::AudioStream* audioStream = audioStreams[0].get();
			osg::notify(osg::NOTICE) << "AudioStream read [" << audioStream->getName() << "]" << std::endl;
#if USE_SDL || USE_SDL2
			if (numAudioStreamsEnabled == 0)
			{
				audioStream->setAudioSink(new SDLAudioSink(audioStream));

				++numAudioStreamsEnabled;
			}
#endif
		}
		imagestream->play();
	}

	if (image)
	{
		osg::notify(osg::NOTICE) << "image->s()" << image->s() << " image-t()=" << image->t() << " aspectRatio=" << image->getPixelAspectRatio() << std::endl;

		float width = image->s() * image->getPixelAspectRatio();
		float height = image->t();

		bool bXYPlane;
		if (plane == XYPlane)
			bXYPlane = true;
		else
			bXYPlane = false;

		osg::ref_ptr<osg::Drawable> drawable = myCreateTexturedQuadGeometry(pos, width / dbScale, height / dbScale, image.get(), true, bXYPlane, false);
		//°ë¶ÎÊÇ·ñ°ëÍ¸Ã÷
		if (image->isImageTranslucent())
		{
			osg::notify(osg::NOTICE) << "Transparent movie, enabling blending." << std::endl;

			drawable->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
			drawable->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		}

		geode->addDrawable(drawable.get());
		geode->getOrCreateStateSet()->setAttributeAndModes(new osg::Program, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
		geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	}

	osg::ref_ptr<osgEarth::GeoTransform> ptrLocatorMovie = new osgEarth::GeoTransform();
	osgEarth::GeoPoint point = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), osg::Vec3d(position.X, position.Y, position.Z));
	ptrLocatorMovie->setPosition(point);
	m_ptrElementHandle = new ReferenceImp<osgEarth::GeoTransform>(ptrLocatorMovie);
	//double x, y, z;
	//OsgCoreSingleton::Instance()->MapNode()->getMapSRS()->getEllipsoid()->convertLatLongHeightToXYZ(osg::DegreesToRadians(vecOut.y()), osg::DegreesToRadians(vecOut.x()), vecOut.z(), x, y, z);

	osg::ref_ptr<osg::MatrixTransform> ptrMat = new osg::MatrixTransform();
	ptrMat->addChild(geode);
	//ptrMat->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(-45.0), osg::Vec3(0.0, 0.0, 1.0))/**osg::Matrix::translate(osg::Vec3d(x, y, z+50))*/);
	ptrLocatorMovie->addChild(ptrMat);

	m_ptrTransNode = new ReferenceImp<osg::MatrixTransform>(ptrMat);

	m_ElementType = eTextureMovieElement;
}

void TextureMovieElement::setPosition(GsRawPoint3D pos)
{
	osgEarth::GeoPoint point = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), osg::Vec3d(pos.X, pos.Y, pos.Z));
	m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::GeoTransform> >()->setPosition(point);
}

GsRawPoint3D TextureMovieElement::getPosition()
{
	osgEarth::GeoPoint vecPos = m_ptrElementHandle->CastTo<osg::ref_ptr<osgEarth::GeoTransform> >()->getPosition();
	
	return GsRawPoint3D(vecPos.x(), vecPos.y(), vecPos.z());
}

void TextureMovieElement::Rotate(GsRawPoint3D gsAxle, double dbDegree)
{
	osg::ref_ptr<osg::MatrixTransform> ptrMat = m_ptrTransNode->CastTo<osg::ref_ptr<osg::MatrixTransform> >();
	ptrMat->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(dbDegree), osg::Vec3(gsAxle.X, gsAxle.Y, gsAxle.Z)));
}

TextureMovieElement::~TextureMovieElement()
{

}

GLOBE_ENDNS
