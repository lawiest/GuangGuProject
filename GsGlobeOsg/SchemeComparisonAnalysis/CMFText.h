#pragma once
#include "SchemeComparisonAnalysis/CoEarth.h"
#include "SchemeComparisonAnalysis/CoreOSG.h"

class CCMFText :public osg::Geode
{
public:
	CCMFText(osgEarth::MapNode* m_MapNode);
	~CCMFText(void);


    CCMFText(const char*  textContent,osg::Vec3d textPositionPt,osgEarth::MapNode* m_MapNode);

	void DrawText();

	void setCMFTextFont(string fonturl){_TextFont=osgText::readFontFile(fonturl);}

	void setCMFTextWorldPosition(osg::Vec3d position);

	osg::Vec3d getPosWorldPt(){return _Textposition;}

	void setCMFTextColor(osg::Vec4d textColor){_TextColor=textColor;}
	osg::Vec4d getCMFTextColor(){return _TextColor;}


	void setCMFCharacterSize(float CharacterSize){_CMFCharacterSize=CharacterSize;}
	float setCMFCharacterSize(){return _CMFCharacterSize;}

	void setCMFAxisAlignment(osgText::Text::AlignmentType alignmentType){_CMFAlignmentType=alignmentType;}
	void setCMFAlignmentType(osgText::Text::AxisAlignment axisAlignment){_CMFAxisAlignment=axisAlignment;}

    wchar_t* TextToWidthChar(const char* mstring);

public:
    const char* _CMFTextContent;

	float _CMFCharacterSize;
	osgText::Text::AlignmentType _CMFAlignmentType;
	osgText::Text::AxisAlignment _CMFAxisAlignment;

	osg::Vec3d _Textposition;

	osg::Vec4d _TextColor;

	osg::ref_ptr<osgText::Font> _TextFont;

	bool _AutoRotateToScreenSwitch;
	bool _BackdropSwitch;//描边
	osg::Vec4d _BackdropColor;
	bool _DrawModeSwitch;//边框

	bool _LightOnOrOff;

	osgEarth::MapNode* _tMapNode;
	const osgEarth::SpatialReference* _tMapSRS;
	const osgEarth::SpatialReference* _tGeoSRS;
};

