
#include "TextElement.h"
#include "OsgCoreSingleton.h"
#include "innerimp.h"
#include <osgText/Font>
#include <osgText/Text>
#include "Analysis/MatrixTransformVisible.h"
#include <osg/Depth>
#include <osgEarth/GeoTransform>
#include <osgearth/MapNode>
#include <tool/CodeTransformHelp.h>

GLOBE_NS

TextElement::TextElement(double x, double y, double z, std::string strText, int nMinLod, int nMaxLod)
{
	osg::ref_ptr<osgText::Font> ptrfont = osgText::readFontFile("fonts/msyh.ttc");
	m_dbAngle = 0;
	m_fSize = 5.0f;
	osg::ref_ptr<osgText::Text> ptrText = new osgText::Text;
	m_ptrTextRef = new ReferenceImp<osgText::Text>(ptrText);
	//osg::ref_ptr<osgText::Font> font = osgText::readFontFile(strFont);
	ptrText->setFont(ptrfont.get());
	ptrText->setCharacterSize(m_fSize);
	//ptrText->setColor(color);
	ptrText->setBackdropColor(osg::Vec4(0.0, 0.0, 0.0, 0.5));
	ptrText->setBackdropOffset(0.07);
	ptrText->setBackdropType(osgText::Text::BackdropType::OUTLINE);

	ptrText->setAxisAlignment(osgText::TextBase::XY_PLANE);
	ptrText->setAlignment(osgText::TextBase::CENTER_CENTER);
	ptrText->setPosition(osg::Vec3(0.0f, 0.0f, 0.0f));
	std::string str = CodeHelp::String_To_UTF8(strText);
	ptrText->setText(str, osgText::String::ENCODING_UTF8);
	ptrText->setName("TextELement");
	//Ò¶½Úµã
	osg::ref_ptr<osg::Geode> textGeode = new osg::Geode;
	textGeode->addDrawable(ptrText);

	osg::ref_ptr<GeoGlobe::MatrixTransformVisible> ptrMat  = new GeoGlobe::MatrixTransformVisible();
	m_ptrTransRef = new ReferenceImp<GeoGlobe::MatrixTransformVisible>(ptrMat);
	ptrMat->addChild(textGeode.get());
	ptrMat->setName("TextELement");
	textGeode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	textGeode->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false));
	textGeode->getOrCreateStateSet()->setRenderBinDetails(12, "RenderBin");
	textGeode->setName("TextELement");
	osg::ref_ptr<osgEarth::GeoTransform> ptrLocator = new osgEarth::GeoTransform();
	ptrLocator->addChild(ptrMat);
	osgEarth::GeoPoint point = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), osg::Vec3(x, y, z));
	ptrLocator->setPosition(point);
	m_ptrLocatorRef = new ReferenceImp<osgEarth::GeoTransform>(ptrLocator);
	ptrLocator->setName("TextELement");
	osg::ref_ptr<osg::LOD> ptrLOD = new osg::LOD();
	m_ptrElementHandle = new ReferenceImp<osg::LOD >(ptrLOD);
	ptrLOD->addChild(ptrLocator, nMinLod, nMaxLod);
	ptrLOD->setName("TextELement");
	m_ElementType = eTextElement;
}

void TextElement::setText(std::string strText)
{
	std::string str = CodeHelp::String_To_UTF8(strText);
	m_ptrTextRef->CastTo<osg::ref_ptr<osgText::Text> >()->setText(str, osgText::String::ENCODING_UTF8);
	m_ptrTextRef->CastTo<osg::ref_ptr<osgText::Text> >()->dirtyDisplayList();
}

std::string TextElement::getText()
{
	return m_ptrTextRef->CastTo<osg::ref_ptr<osgText::Text> >()->getText().createUTF8EncodedString();
}

void TextElement::setPosition(double x, double y, double z)
{
	osgEarth::GeoPoint point = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), osg::Vec3d(x, y, z));
	m_ptrLocatorRef->CastTo<osg::ref_ptr <osgEarth::GeoTransform> >()->setPosition(point);
}

KERNEL_NAME::GsRawPoint3D TextElement::getPosition()
{
	osgEarth::GeoPoint vec3 = m_ptrLocatorRef->CastTo<osg::ref_ptr <osgEarth::GeoTransform> >()->getPosition();
	return KERNEL_NAME::GsRawPoint3D(vec3.x(), vec3.y(), vec3.z());
}

void TextElement::getPosition(double & x, double & y, double & z)
{
	osgEarth::GeoPoint vec3 = m_ptrLocatorRef->CastTo<osg::ref_ptr <osgEarth::GeoTransform> >()->getPosition();
	x = vec3.x();
	y = vec3.y();
	z = vec3.z();
}

bool TextElement::Visible()
{
	return m_ptrTransRef->CastTo<osg::ref_ptr<GeoGlobe::MatrixTransformVisible>>()->Visible();
}

void TextElement::Visible(bool bVisible)
{
	m_ptrTransRef->CastTo<osg::ref_ptr<GeoGlobe::MatrixTransformVisible>>()->setVisible(bVisible);
}

void TextElement::setColor(int r, int g, int b, int a )
{
	m_ptrTextRef->CastTo<osg::ref_ptr<osgText::Text>>()->setColor(osg::Vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f));
	m_ptrTextRef->CastTo<osg::ref_ptr<osgText::Text>>()->dirtyDisplayList();
}

KERNEL_NAME::GsColor TextElement::getColor()
{
	osg::Vec4 vec4 = m_ptrTextRef->CastTo<osg::ref_ptr<osgText::Text> >()->getColor();
	return KERNEL_NAME::GsColor(vec4.r()*255, vec4.g()*255, vec4.b()*255, vec4.a()*255);
}

void TextElement::Rotate(GsRawPoint3D gsAxle, double dbDegree)
{
	if (m_dbAngle == dbDegree)
		return;
	double angle = osg::DegreesToRadians(dbDegree);
	m_ptrTransRef->CastTo<osg::ref_ptr<osg::MatrixTransform> >()->setMatrix(osg::Matrix::rotate(angle, 0, 0, 1));
	m_dbAngle = dbDegree;
}

double TextElement::getdbAngle()
{
	return m_dbAngle;
}

float TextElement::getTextSize()
{
	return m_fSize;
}

void TextElement::setTextSize(float fSize)
{
	m_ptrTextRef->CastTo<osg::ref_ptr<osgText::Text> >()->setCharacterSize(fSize);
	m_fSize = fSize;
	m_ptrTextRef->CastTo<osg::ref_ptr<osgText::Text> >()->dirtyDisplayList();
}

TextElement::~TextElement()
{
	
}

GLOBE_ENDNS