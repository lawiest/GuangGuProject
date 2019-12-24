#include "GeoBillBoardElement.h"
#include "OsgCoreSingleton.h"
#include "innerimp.h"
#include <tool/CodeTransformHelp.h>
#include <strstream>
#include <placename/GeoBillboard.h>
#include <osgEarth/ObjectIndex>
#include <osgEarth/Registry>
#include <tool/BillBoardPicker.h>
#include <Feature.h>

GLOBE_NS


class GeoBillBoardUpdateCallback : public osg::NodeCallback
{
public:
	GeoBillBoardUpdateCallback()
	{
	}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> ptrPlaceName = dynamic_cast<GeoGlobe::PlaceName::GeoBillboard*>(node);

		if (NULL == ptrPlaceName)
			return;

		std::string strPtr = ptrPlaceName->getName();
		long long longPtr;
		std::strstream ss;
		ss << strPtr;
		ss >> longPtr;

		GeoBillBoardElementPtr ptrBillbord = reinterpret_cast<GeoBillBoardElement*>(longPtr);

		if (NULL != ptrBillbord)
			ptrBillbord->OnUpdate(ptrBillbord.p);
	}
protected:
};

GeoBillBoardElement::GeoBillBoardElement(double x, double y, double z, std::string strText, std::string strIcon, int nTextCenterOffset, int nMinLod, int nMaxLod)
{
	m_strIcon = strIcon;
	m_dLongitude = x;
	m_dLatitude = y;
	m_dHeigh = z;
	m_bUpdateCallBack = false;
	m_strText = CodeHelp::String_To_UTF8(strText);
	CreateGeoBillboard(nMinLod, nMaxLod, nTextCenterOffset);
	m_ElementType = eGeoBillBoardElement;
}

GeoBillBoardElement::~GeoBillBoardElement()
{

}

void GeoBillBoardElement::setPosition(double x, double y, double z)
{
	osgEarth::GeoPoint postion = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getPosition();
	postion.x() = x;
	postion.y() = y;
	postion.z() = z;
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setPosition(postion);
	OnPositionChange(KERNEL_NAME::GsRawPoint3D(x,y,z));
}

void GeoBillBoardElement::getPosition(double& x, double& y, double& z)
{
	osgEarth::GeoPoint postion = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getPosition();
	x = postion.x();
	y = postion.y();
	z = postion.z();
}

void GeoBillBoardElement::CreateGeoBillboard(int nMinLod, int nMaxLod, int nTextCenterOffset)
{
	osgEarth::Symbology::Style style;
	osg::ref_ptr<osg::Node> geoBillBord;
	if (m_strIcon != "")
	{
		style.getOrCreate<osgEarth::IconSymbol>()->url()->setLiteral(m_strIcon);
	}
	else
	{
		//style.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color() = osgEarth::Color::White;
		//style.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color() = osgEarth::Color::Red;
	}

	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->size() = 18.0f;

	//style.getOrCreate<osgEarth::IconSymbol>()->declutter() = false;
	//style.getOrCreate<osgEarth::TextSymbol>()->declutter() = false;
	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->font() = "msyh.ttc";
	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;


	osgEarth::GeoPoint geoPos(OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), m_dLongitude,m_dLatitude,m_dHeigh);
	geoBillBord = new GeoGlobe::PlaceName::GeoBillboard(geoPos, m_strText, style, true, nTextCenterOffset);
	geoBillBord->setCullingActive(false);
	dynamic_cast<GeoGlobe::PlaceName::GeoBillboard*>(geoBillBord.get())->setDynamic(true);
	//geoBillBord->setDynamic(true);


	/*拾取BillBoard等屏幕元素需要使用osgearth自带的RTT技术，必须加这句话相当于注册*/
	osgEarth::ObjectID id = osgEarth::Registry::objectIndex()->tagNode(geoBillBord.get(), geoBillBord.get());

	m_ptrElementHandle = new ReferenceImp<osg::Node>(geoBillBord);
	osg::ref_ptr<osg::LOD> ptrLOD = new osg::LOD();
	ptrLOD->addChild(geoBillBord, nMinLod, nMaxLod);
	m_ptrLodElement = new ReferenceImp<osg::LOD>(ptrLOD);
}

void GeoBillBoardElement::CoverAutoHide(bool bAutoHide)
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getStyle();
	style.getOrCreate<osgEarth::IconSymbol>()->declutter() = bAutoHide;
	style.getOrCreate<osgEarth::TextSymbol>()->declutter() = bAutoHide;

	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setStyle(style);
}

bool GeoBillBoardElement::CoverAutoHide()
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getStyle();
	return style.getOrCreate<osgEarth::IconSymbol>()->declutter().get();
}

void GeoBillBoardElement::TextAlignment(const Alignment& alignment)
{
	osgEarth::Style style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getStyle();
	m_textAlignment = alignment;
	if (alignment == ALIGN_BOTTOM)
	{
		//style.getOrCreate<osgEarth::IconSymbol>()->alignment() = osgEarth::Symbology::IconSymbol::ALIGN_CENTER_TOP;
		style.getOrCreate<osgEarth::TextSymbol>()->alignment() = osgEarth::Symbology::TextSymbol::ALIGN_CENTER_BOTTOM;
	}
	else if (alignment == ALIGN_TOP)
	{
		//style.getOrCreate<osgEarth::IconSymbol>()->alignment() = osgEarth::Symbology::IconSymbol::ALIGN_CENTER_BOTTOM;
		style.getOrCreate<osgEarth::TextSymbol>()->alignment() = osgEarth::Symbology::TextSymbol::ALIGN_CENTER_TOP;
	}
	else if (alignment == ALIGN_RIGHT)
	{
		//style.getOrCreate<osgEarth::IconSymbol>()->alignment() = osgEarth::Symbology::IconSymbol::ALIGN_LEFT_CENTER;
		style.getOrCreate<osgEarth::TextSymbol>()->alignment() = osgEarth::Symbology::TextSymbol::ALIGN_RIGHT_CENTER;
	}
	else if (alignment == ALIGN_LEFT)
	{
		//style.getOrCreate<osgEarth::IconSymbol>()->alignment() = osgEarth::Symbology::IconSymbol::ALIGN_RIGHT_CENTER;
		style.getOrCreate<osgEarth::TextSymbol>()->alignment() = osgEarth::Symbology::TextSymbol::ALIGN_LEFT_CENTER;
	}
	else if (alignment == ALIGN_CENTER)
	{
		style.getOrCreate<osgEarth::TextSymbol>()->alignment() = osgEarth::Symbology::TextSymbol::ALIGN_CENTER_CENTER;
	}

	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setStyle(style);
}

Alignment GeoBillBoardElement::TextAlignment()
{
	return m_textAlignment;
}

void GeoBillBoardElement::TextSize(double dbSize)
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getStyle();
	style.getOrCreate<osgEarth::TextSymbol>()->size() = dbSize;

	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setStyle(style);
}

double GeoBillBoardElement::TextSize()
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getStyle();
	return style.getOrCreate<osgEarth::TextSymbol>()->size().get().eval();
}

void GeoBillBoardElement::TextColor(int r, int g, int b, int a)
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getStyle();
	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color() = osgEarth::Symbology::Color(r / 255.0, g / 255.0, b / 255.0, a);

	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setStyle(style);
}

void GeoBillBoardElement::TextColor(KERNEL_NAME::GsColor color)
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getStyle();
	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color() = osgEarth::Symbology::Color(color.RedF(), color.GreenF(), color.BlueF());

	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setStyle(style);
	//m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setTextColor(osg::Vec4(color.R/255.0,color.G/255.0,color.B/255.0f,color.A/255.0f));
}

void GeoBillBoardElement::HightLight()
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->HightLight();
}

void GeoBillBoardElement::CancelHightLight()
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->CancelHightLight();
}

void GeoBillBoardElement::SetAnimatorAction(AnimatorAction * action)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setAnimatorAction(action);
}

AnimatorAction * GeoBillBoardElement::GetAnimatorAction()
{
	return m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getAnimatorAction();
}

void GeoBillBoardElement::TextHaloColor(int r, int g, int b, int a)
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getStyle();
	style.getOrCreate<osgEarth::TextSymbol>()->halo() = osgEarth::Color(r / 255.0, g / 255.0, b / 255.0, a);

	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setStyle(style);
}

KERNEL_NAME::GsColor GeoBillBoardElement::TextColor()
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getStyle();
	osgEarth::Symbology::Color color = style.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color();
	return KERNEL_NAME::GsColor::FromARGBF(color.r(), color.g(), color.b(), color.a());
}

void GeoBillBoardElement::Dynamic(bool bDynamic)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setDynamic(bDynamic);
}

void GeoBillBoardElement::setLod(int minLod, int maxLod)
{
	m_ptrLodElement->CastTo<osg::ref_ptr<osg::LOD>>()->setRange(0, minLod, maxLod);
}

void GeoBillBoardElement::Text(std::string strText)
{
	m_strText = CodeHelp::String_To_UTF8(strText);
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setText(m_strText);
}

std::string GeoBillBoardElement::Text()
{
	return m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getText();
}

void GeoBillBoardElement::Font(std::string strFont)
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getStyle();
	style.getOrCreate<osgEarth::TextSymbol>()->font() = strFont;
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setStyle(style);
}

std::string GeoBillBoardElement::Icon()
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getStyle();
	return style.getOrCreate<osgEarth::IconSymbol>()->url().get().eval();
}

void GeoBillBoardElement::setImage(unsigned char* ptrData, int nWidth, int nHeight)
{
	osg::ref_ptr<osg::Image>image = new osg::Image;
	unsigned char*pRGBData = NULL;

	image->setImage(nWidth, nHeight, 1, 4, GL_RGBA, GL_UNSIGNED_BYTE, pRGBData, osg::Image::USE_NEW_DELETE);
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setIconImage(image);
}

void GeoBillBoardElement::Icon(std::string strIcon)
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getStyle();
	style.getOrCreate<osgEarth::IconSymbol>()->url()->setLiteral(strIcon);
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setStyle(style);
}

void GeoBillBoardElement::setBackGrandColor(const KERNEL_NAME::GsColor& color)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setBackGrandColor(osg::Vec4(color.R/255.0f, color.G/255.0f, color.B/255.0f, color.A/255.0f));
}

KERNEL_NAME::GsColor GeoBillBoardElement::getBackGrandColor()
{
	osg::Vec4 color = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getBackGrandColor();
	return KERNEL_NAME::GsColor(color.r() * 255, color.g() * 255, color.b() * 255, color.a() * 255);
}

void GeoBillBoardElement::setOutLineColor(const KERNEL_NAME::GsColor& color)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setOutLineColor(osg::Vec4(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f));
}

KERNEL_NAME::GsColor GeoBillBoardElement::getOutLineColor()
{
	osg::Vec4 color = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getOutLineColor();
	return KERNEL_NAME::GsColor(color.r() * 255, color.g() * 255, color.b() * 255, color.a() * 255);
}

void GeoBillBoardElement::setLineColor(const KERNEL_NAME::GsColor& color)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setLineColor(osg::Vec4(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f));
}

KERNEL_NAME::GsColor GeoBillBoardElement::getLineColor()
{
	osg::Vec4 color = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getLineColor();
	return KERNEL_NAME::GsColor(color.r() * 255, color.g() * 255, color.b() * 255, color.a() * 255);
}


//设置折线角度
void GeoBillBoardElement::setPolylineAngle(const double& dbAngle)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setPolylineAngle(dbAngle);
}

//设置折线角度
double GeoBillBoardElement::getPolylineAngel()
{
	return m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getPolylineAngel();
}

//设置底部线垂线长度
void GeoBillBoardElement::setBottomLen(const double& dbBottomLen)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setBottomLen(dbBottomLen);
}

//获得底部线垂线长度
double GeoBillBoardElement::getBottomLen()
{
	return m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getBottomLen();
}

//设置折线长度
void GeoBillBoardElement::setPolylineLen(const double& dbPolylineLen)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setPolylineLen(dbPolylineLen);
}

//获得折线长度
double GeoBillBoardElement::getPolylineLen()
{
	return m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getPolylineLen();
}

void GeoBillBoardElement::setShadowColor(const KERNEL_NAME::GsColor& color)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setShadowColor(osg::Vec4(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f));
}

KERNEL_NAME::GsColor GeoBillBoardElement::getShadowColor()
{
	osg::Vec4 color = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getShadowColor();
	return KERNEL_NAME::GsColor(color.r() * 255, color.g() * 255, color.b() * 255, color.a() * 255);
}

void GeoBillBoardElement::setShadowOffset(const double& dbShadowOffset)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setShadowOffset(dbShadowOffset);
}

double GeoBillBoardElement::getShadowOffset()
{
	return m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getShadowOffset();
}

void GeoBillBoardElement::setShadowPos(const ShadowStyle& m_eShadowPos)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setShadowPos((osgText::Text::BackdropType)m_eShadowPos);
}

ShadowStyle GeoBillBoardElement::getShadowPos()
{
	return (ShadowStyle)m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getShadowPos();
}

std::string GeoBillBoardElement::Font()
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getStyle();
	return style.getOrCreate<osgEarth::TextSymbol>()->font().value();
}

bool GeoBillBoardElement::Visible()
{
	return m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getVisible();
	//return m_bVisible;
}

void GeoBillBoardElement::Visible(bool bVisible)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setVisible(bVisible);

	//m_bVisible = bVisible;

	//if (m_bVisible)
	//	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setNodeMask(1);
	//else
	//	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setNodeMask(0);
}

void GeoBillBoardElement::setValue(std::string key, std::string value)
{
	if (m_ptrElementHandle)
		m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setUserValue(key, value);
}

void GeoBillBoardElement::getValue(std::string& key, std::string& value)
{
	if (m_ptrElementHandle)
		m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->getUserValue(key, value);
}

GeoGlobe::BillBoardPicker* GeoBillBoardElement::m_Picker = NULL;
bool GeoBillBoardElement::m_bIsPick = false;

UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Feature*)>& GeoBillBoardElement::OnPicked()
{
	if (m_Picker)
		return m_Picker->OnPicked;
}

void GeoBillBoardElement::StartPick()
{
	if (!m_bIsPick)
	{
		if (!m_Picker)
			m_Picker = new GeoGlobe::BillBoardPicker();
		else
			m_Picker->StartPick();

		m_bIsPick = true;
	}
}

void GeoBillBoardElement::EndPick()
{
	if (m_bIsPick)
	{
		if (m_Picker)
			m_Picker->EndPick();

		m_bIsPick = false;
	}
}

UTILITY_NAME::GsDelegate<void(Element*)>& GeoBillBoardElement::Updata()
{
	if (!m_bUpdateCallBack)
	{
		long long  longPtr = (long long)(long long *)this;
		std::string strPtr;
		std::strstream ss;
		ss << longPtr;
		ss >> strPtr;

		m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setName(strPtr.c_str());
		m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> >()->setUpdateCallback(new GeoBillBoardUpdateCallback());
		m_bUpdateCallBack = true;
	}

	return OnUpdate;
}


GLOBE_ENDNS