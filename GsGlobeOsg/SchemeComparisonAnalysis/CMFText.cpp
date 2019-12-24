#include "SchemeComparisonAnalysis/CMFText.h"


CCMFText::CCMFText(osgEarth::MapNode* m_MapNode)
{
	_LightOnOrOff=false;

	_tMapNode=m_MapNode;
	_tMapSRS=m_MapNode->getMapSRS();
	_tGeoSRS=m_MapNode->getMapSRS()->getGeographicSRS();
}

CCMFText::CCMFText(const char* textContent,osg::Vec3d textPositionPt,osgEarth::MapNode* m_MapNode)
{

	_CMFTextContent=textContent;

	_CMFCharacterSize=5.0f;
	_CMFAlignmentType=osgText::Text::CENTER_TOP;
	_CMFAxisAlignment=osgText::Text::SCREEN;

	_TextColor=osg::Vec4(1.0,0.0,0.0,1.0);

	_TextFont=osgText::readFontFile("../CmfData/GB2312.ttf");

	_AutoRotateToScreenSwitch=false;
	_BackdropSwitch=false;//描边
	_BackdropColor=osg::Vec4(1.0,0.0,0.0,1.0);
	_DrawModeSwitch=false;//边框

	_LightOnOrOff=false;

	_tMapNode=m_MapNode;
	_tMapSRS=m_MapNode->getMapSRS();
	_tGeoSRS=m_MapNode->getMapSRS()->getGeographicSRS();

	setCMFTextWorldPosition(textPositionPt);
}

CCMFText::~CCMFText(void)
{
}

void CCMFText::setCMFTextWorldPosition(osg::Vec3d position)
{
	//位置
	osgEarth::GeoPoint mapstarPtPos=osgEarth::GeoPoint(_tGeoSRS,position.x(),position.y(),position.z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_tMapSRS);
	osg::Vec3d worldMapposition;
	mapstarPtPos.toWorld(worldMapposition);//经纬度转换为世界坐标
	_Textposition=worldMapposition;
}

wchar_t* CCMFText::TextToWidthChar(const char* mstring)
{
	int requiredSize=mbstowcs(NULL,mstring,0);
	wchar_t* wtext=new wchar_t(requiredSize+1);
	mbstowcs(wtext,mstring,requiredSize+1);
	return wtext;
}

void CCMFText::DrawText()
{
	osg::ref_ptr<osgText::Text> text = new osgText::Text;

	osg::ref_ptr<osgText::Font> font = new osgText::Font();
	//读取字体
	font = osgText::readFontFile("C:\\Users\\CK\\Desktop\\GB2312.ttf");

	// 设置字体文件
	text->setFont(_TextFont.get());
	// 设置文字信息
	text->setText( TextToWidthChar(_CMFTextContent) );
	// 设置字体大小
	text->setCharacterSize( _CMFCharacterSize );
	text->setAutoRotateToScreen(_AutoRotateToScreenSwitch);
	//设置字体颜色
	text->setColor( _TextColor );
	// 设置显示的位置
	text->setPosition( _Textposition );
	// 设置对齐方式
	text->setAlignment( _CMFAlignmentType );
	// 设置旋转方式
	text->setAxisAlignment( _CMFAxisAlignment);

	if (_BackdropSwitch)
	{
		text->setBackdropType(osgText::Text::OUTLINE);//对文字进行描边
		text->setBackdropColor(_BackdropColor);//描边颜色
	}
	if (_DrawModeSwitch)
	{
		text->setDrawMode(osgText::Text::TEXT | osgText::Text::BOUNDINGBOX);//添加文字边框   
	}
	if (_LightOnOrOff==false)
	{
		text->getOrCreateStateSet()->setMode(GL_LIGHTING ,osg::StateAttribute::OFF);
	}
	
	this->addDrawable(text.get());

}






