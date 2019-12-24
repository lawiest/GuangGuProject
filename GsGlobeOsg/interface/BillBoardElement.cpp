#include "BillBoardElement.h"
#include "OsgCoreSingleton.h"
#include "innerimp.h"
#include <placename/PlaceNode.h>
#include <tool/CodeTransformHelp.h>
#include <strstream>
#include <tool/BillBoardPicker.h>
#include <osgViewer/Viewer>
#include <osgEarth/ObjectIndex>
#include <osgEarth/Registry>

GLOBE_NS

class BillBoardUpdateCallback : public osg::NodeCallback
{
public:
	BillBoardUpdateCallback(){}

	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> ptrPlaceName = dynamic_cast<GeoGlobe::Annotation::PlaceNode*>(node);

		if (NULL == ptrPlaceName)
			return;

		std::string strPtr = ptrPlaceName->getName();
		long long longPtr;
		std::strstream ss;
		ss << strPtr;
		ss >> longPtr;

		BillBoardElementPtr ptrBillbord = reinterpret_cast<BillBoardElement*>(longPtr);

		if (NULL != ptrBillbord)
			ptrBillbord->OnUpdate(ptrBillbord.p);
	}
protected:
};

GeoGlobe::BillBoardPicker* BillBoardElement::m_Picker = NULL;
bool BillBoardElement::m_bIsPick = false;

UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Feature*)>& BillBoardElement::OnPicked()
{
	if (m_Picker)
		return m_Picker->OnPicked;
}

void BillBoardElement::StartPick()
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

void BillBoardElement::EndPick()
{
	if (m_bIsPick)
	{
		if (m_Picker)
			m_Picker->EndPick();

		m_bIsPick = false;
	}
}

BillBoardElement::BillBoardElement(double x, double y, double z, std::string strText, std::string strIcon, int nMinLod, int nMaxLod)
{
	m_dLongitude = x;
	m_dLatitude = y;
	m_dHeigh = z;
	m_bUpdateCallBack = false;
	m_iocnAlignment = ALIGN_BOTTOM;
	m_strText = strText;
	m_strIcon = strIcon;
	m_strText = CodeHelp::String_To_UTF8(strText);
	CreateLabel(nMinLod, nMaxLod);
	m_ElementType = eBillboardElement;
}

BillBoardElement::~BillBoardElement()
{

}

void BillBoardElement::setPosition(double x, double y, double z)
{
	osgEarth::GeoPoint postion = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getPosition();
	postion.x() = x;
	postion.y() = y;
	postion.z() = z;
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setPosition(postion);

	OnPositionChange(KERNEL_NAME::GsRawPoint3D(x, y, z));
}

void BillBoardElement::getPosition(double& x, double& y, double& z)
{
	osgEarth::GeoPoint postion = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getPosition();

	x = postion.x();
	y = postion.y();
	z = postion.z();
}

void BillBoardElement::CreateLabel(int dbMinLod, int dbMaxLod)
{
	const osgEarth::SpatialReference* geoSRS = OsgCoreSingleton::Instance()->MapNode()->getMapSRS()->getGeographicSRS();
	osgEarth::Style style;
	//设置文字
	style.getOrCreate<osgEarth::IconSymbol>()->url()->setLiteral(m_strIcon);
	style.getOrCreate<osgEarth::IconSymbol>()->declutter() = true;
	style.getOrCreate<osgEarth::TextSymbol>()->declutter() = true;
	style.getOrCreate<osgEarth::TextSymbol>()->size() = 18.0f;
	style.getOrCreate<osgEarth::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	style.getOrCreate<osgEarth::TextSymbol>()->font() = "msyh.ttc";

	osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> ptrPlaceName = new  GeoGlobe::Annotation::PlaceNode(osgEarth::GeoPoint(geoSRS, m_dLongitude, m_dLatitude,m_dHeigh), m_strText, style);
	ptrPlaceName->setDynamic(true);
	/*拾取BillBoard等屏幕元素需要使用osgearth自带的RTT技术，必须加这句话相当于注册*/
	osgEarth::ObjectID id = osgEarth::Registry::objectIndex()->tagNode(ptrPlaceName.get(), ptrPlaceName.get());
	m_ptrElementHandle = new ReferenceImp<GeoGlobe::Annotation::PlaceNode >(ptrPlaceName);

	osg::ref_ptr<osg::LOD> ptrLOD = new osg::LOD();
	ptrLOD->addChild(ptrPlaceName, dbMinLod, dbMaxLod);
	m_ptrLodElement = new ReferenceImp<osg::LOD>(ptrLOD);
}

void BillBoardElement::IconAlignment(Alignment alignment)
{
	osgEarth::Style style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getStyle();
	m_iocnAlignment = alignment;
	if (alignment == ALIGN_BOTTOM)
	{
		//style.getOrCreate<osgEarth::IconSymbol>()->alignment() = osgEarth::Symbology::IconSymbol::ALIGN_CENTER_BOTTOM;
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
		style.getOrCreate<osgEarth::TextSymbol>()->alignment() = osgEarth::Symbology::TextSymbol::ALIGN_LEFT_CENTER;
	}
	else if (alignment == ALIGN_LEFT)
	{
		//style.getOrCreate<osgEarth::IconSymbol>()->alignment() = osgEarth::Symbology::IconSymbol::ALIGN_RIGHT_CENTER;
		style.getOrCreate<osgEarth::TextSymbol>()->alignment() = osgEarth::Symbology::TextSymbol::ALIGN_RIGHT_CENTER;
	}

	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setStyle(style);
}

Alignment BillBoardElement::IconAlignment()
{
	return m_iocnAlignment;
}

void BillBoardElement::CoverAutoHide(bool bAutoHide)
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getStyle();
	style.getOrCreate<osgEarth::IconSymbol>()->declutter() = bAutoHide;
	style.getOrCreate<osgEarth::TextSymbol>()->declutter() = bAutoHide;

	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setStyle(style);
}

bool BillBoardElement::CoverAutoHide()
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getStyle();
	return style.getOrCreate<osgEarth::IconSymbol>()->declutter().get();
}

void BillBoardElement::TextSize(double dbSize)
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getStyle();
	style.getOrCreate<osgEarth::TextSymbol>()->size() = dbSize;

	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setStyle(style);
}

double BillBoardElement::TextSize()
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getStyle();
	return style.getOrCreate<osgEarth::TextSymbol>()->size().get().eval();
}

void BillBoardElement::SetAnimatorAction(AnimatorAction * action)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setAnimatorAction(action);
}

AnimatorAction * BillBoardElement::GetAnimatorAction()
{
	return m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getAnimatorAction();
}


void BillBoardElement::TextColor(int r, int g, int b, int a)
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getStyle();
	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color() = osgEarth::Symbology::Color(r / 255.0, g / 255.0, b / 255.0, a);

	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setStyle(style);
}

void BillBoardElement::TextColor(KERNEL_NAME::GsColor color)
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getStyle();
	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color() = osgEarth::Symbology::Color(color.RedF(), color.GreenF(), color.BlueF());

	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setStyle(style);
}

void BillBoardElement::TextHaloColor(int r, int g, int b, int a)
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getStyle();
	style.getOrCreate<osgEarth::TextSymbol>()->halo() = osgEarth::Color(r / 255.0, g / 255.0, b / 255.0, a);

	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setStyle(style);
}

KERNEL_NAME::GsColor BillBoardElement::TextColor()
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getStyle();
	osgEarth::Symbology::Color color = style.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color();
	return KERNEL_NAME::GsColor::FromARGBF(color.r(), color.g(), color.b(), color.a());
}

void BillBoardElement::Dynamic(bool bDynamic)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setDynamic(bDynamic);
}

void BillBoardElement::setShadowColor(const KERNEL_NAME::GsColor& color)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setShadowColor(osg::Vec4(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f));
}

const KERNEL_NAME::GsColor& BillBoardElement::getShadowColor()
{
	osg::Vec4 color = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getShadowColor();
	return KERNEL_NAME::GsColor(color.r() * 255, color.g() * 255, color.b() * 255, color.a() * 255);
}

void BillBoardElement::setShadowOffset(const double& dbShadowOffset)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setShadowOffset(dbShadowOffset);
}

const double& BillBoardElement::getShadowOffset()
{
	return m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getShadowOffset();
}

void BillBoardElement::setShadowPos(const ShadowStyle& m_eShadowPos)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setShadowPos((osgText::Text::BackdropType)m_eShadowPos);
}

const ShadowStyle& BillBoardElement::getShadowPos()
{
	return (ShadowStyle)m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getShadowPos();
}
// 改变lod 
void BillBoardElement::setLod(int minLod, int maxLod)
{
	m_ptrLodElement->CastTo<osg::ref_ptr<osg::LOD>>()->setRange(0, minLod, maxLod);
}

std::string BillBoardElement::Icon()
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getStyle();
	return style.getOrCreate<osgEarth::IconSymbol>()->url().get().eval();
}

void BillBoardElement::Icon(std::string strIcon)
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getStyle();
	style.getOrCreate<osgEarth::IconSymbol>()->url()->setLiteral(strIcon);
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setStyle(style);
}

void BillBoardElement::Font(std::string strFont)
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getStyle();
	style.getOrCreate<osgEarth::TextSymbol>()->font() =strFont;
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setStyle(style);
}

std::string BillBoardElement::Font()
{
	osgEarth::Style  style = m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getStyle();
	return style.getOrCreate<osgEarth::TextSymbol>()->font().value();
}

void BillBoardElement::Text(std::string strText)
{
	m_strText = CodeHelp::String_To_UTF8(strText);
	m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setText(m_strText);
}

std::string BillBoardElement::Text()
{
	return m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getText();
}

void BillBoardElement::setValue(std::string key, std::string value)
{
	if (m_ptrElementHandle)
		m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setUserValue(key, value);
}

void BillBoardElement::getValue(std::string& key, std::string& value)
{
	if (m_ptrElementHandle)
		m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->getUserValue(key, value);
}

bool BillBoardElement::Visible()
{
	return m_bVisible;
}

void BillBoardElement::Visible(bool bVisible)
{
	m_bVisible = bVisible;

	if (m_bVisible)
		m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setNodeMask(1);
	else
		m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setNodeMask(0);
}

UTILITY_NAME::GsDelegate<void(Element*)>& BillBoardElement::Updata()
{
	if (!m_bUpdateCallBack)
	{
		long long  longPtr = (long long)(long long *)this;
		std::string strPtr;
		std::strstream ss;
		ss << longPtr;
		ss >> strPtr;

		m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setName(strPtr.c_str());
		m_ptrElementHandle->CastTo<osg::ref_ptr<GeoGlobe::Annotation::PlaceNode> >()->setUpdateCallback(new BillBoardUpdateCallback());
		m_bUpdateCallBack = true;
	}

	return OnUpdate;
}


GLOBE_ENDNS