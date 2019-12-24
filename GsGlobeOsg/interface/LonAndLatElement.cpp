
#include "LonAndLatElement.h"
#include "OsgCoreSingleton.h"
#include "innerimp.h"
#include <osgEarthUtil/Controls>
#include <tool/CodeTransformHelp.h>
#include"Analysis/QueryElevationElement.h"

GLOBE_NS

using namespace osgEarth::Util::Controls;

osg::ref_ptr<QueryElevationElement> ptrQueryElvation;

struct MyClickHandler : public ControlEventHandler
{
	MyClickHandler(LonAndLatElement* ptrLonAndLatEle)
	{
		m_ptrLonAndLatEle = ptrLonAndLatEle;
	}

	void onClick(Control* control, const osg::Vec2f& pos, int mouseButtonMask)
	{
		LabelControl* lable = dynamic_cast<LabelControl*>(control);

		if (m_ptrLonAndLatEle)
			m_ptrLonAndLatEle->OnClicked()(m_ptrLonAndLatEle);
	}

	LonAndLatElementPtr m_ptrLonAndLatEle;
};

LonAndLatElement::LonAndLatElement()
{
	m_ElementType = eLonAndLatElement;
}

void LonAndLatElement::setText(std::string strText)
{
	osg::ref_ptr<LabelControl> ptrLabel = m_ptrElementHandle->CastTo<osg::ref_ptr<LabelControl>>();

	if (ptrLabel)
		ptrLabel->setText(strText);
}

std::string LonAndLatElement::getText()
{
	osg::ref_ptr<LabelControl> ptrLabel = m_ptrElementHandle->CastTo<osg::ref_ptr<LabelControl>>();

	if (ptrLabel)
		return ptrLabel->text();

	return "error!";
}

void LonAndLatElement::Visible(bool bVisible)
{
	osg::ref_ptr<LabelControl> ptrLabel = m_ptrElementHandle->CastTo<osg::ref_ptr<LabelControl>>();

	if (ptrLabel)
		ptrLabel->setVisible(bVisible);
}

bool LonAndLatElement::Visible()
{
	osg::ref_ptr<LabelControl> ptrLabel = m_ptrElementHandle->CastTo<osg::ref_ptr<LabelControl>>();

	if (ptrLabel)
		return ptrLabel->visible();

	return false;
}

LonAndLatElement::~LonAndLatElement()
{
	
}

void LonAndLatElement::creatGeoInfoBox()
{
	osg::ref_ptr<ControlCanvas> ptrControlCanvas = ControlCanvas::getOrCreate(OsgCoreSingleton::Instance()->OsgViewer());

	HBox* bottom = new HBox();
	bottom->setBackColor(1, 0, 0, 0);
	bottom->setMargin(0);
	bottom->setChildSpacing(0);
	bottom->setVertAlign(Control::ALIGN_BOTTOM);
	bottom->setHorizAlign(Control::ALIGN_LEFT);
	osg::ref_ptr<LabelControl> ptrLongitudeLabel = new LabelControl();
	m_ptrElementHandle = new ReferenceImp<LabelControl>(ptrLongitudeLabel);

	osgText::Font* font = osgText::readFontFile("msyh.ttc");
	ptrLongitudeLabel->setFont(font);
	ptrLongitudeLabel->setFontSize(15.0);
	ptrLongitudeLabel->setEncoding(osgText::String::ENCODING_UTF8);
	ptrLongitudeLabel->setText(CodeHelp::String_To_UTF8("¾­¶È:"));
	ptrLongitudeLabel->setMargin(0);
	bottom->addControl(ptrLongitudeLabel.get());
	ptrLongitudeLabel->setActiveColor(0.0f, 0.0f, 0.0f, 0.5f);
	ptrLongitudeLabel->addEventHandler(new MyClickHandler(this));
	ptrControlCanvas->addControl(bottom);
	ptrQueryElvation = new QueryElevationElement(this);
	OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(ptrQueryElvation);
}

void LonAndLatElement::Build()
{
	creatGeoInfoBox();
}

UTILITY_NAME::GsDelegate<void(Element*)>& LonAndLatElement::OnClicked()
{
	return Clicked;
}

UTILITY_NAME::GsDelegate<void()>& LonAndLatElement::Change()
{
	return ptrQueryElvation->Change;
}

UTILITY_NAME::GsDelegate<void(KERNEL_NAME::GsRawPoint3D point)>& LonAndLatElement::MouseMovePoint()
{
	return ptrQueryElvation->MouseMovePoint;
}

GLOBE_ENDNS