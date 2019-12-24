#pragma once
#include <Layer.h>
#include <geodatabase.h> 
#include <Feature.h>
#include <canvas.h>

GLOBE_NS

enum ShadowPos
{
	DROP_SHADOW_BOTTOM_RIGHT = 0,
	DROP_SHADOW_CENTER_RIGHT,
	DROP_SHADOW_TOP_RIGHT,
	DROP_SHADOW_BOTTOM_CENTER,
	DROP_SHADOW_TOP_CENTER,
	DROP_SHADOW_BOTTOM_LEFT,
	DROP_SHADOW_CENTER_LEFT,
	DROP_SHADOW_TOP_LEFT,
	OUTLINE,
	NONE
};

/*
道路样式类
nsize：字大小
color：字颜色
shadowColor：阴影颜色
eShaowPos：阴影位置
dbOffset：阴影偏移量
strFontFile：字体，默认会读取C:\Windows\Fonts目录下的字体
*/
class GS_API RoadTextStyle :public GeoStar::Utility::GsRefObject
{
public:
	RoadTextStyle(int nsize = 16,
				KERNEL_NAME::GsColor color = KERNEL_NAME::GsColor(255, 255, 255, 255),
				KERNEL_NAME::GsColor shadowColor = KERNEL_NAME::GsColor(0.0, 0.0, 0.0, 110),
				ShadowPos eShaowPos = OUTLINE,
				double dbOffset = 0.07,
				std::string strFontFile = "msyh.ttc")
	{
		m_nsize = nsize;
		m_color = color;
		m_shadowColor = shadowColor;
		m_strFontFile = strFontFile;
		m_eShadowPos = eShaowPos;
		m_dbOffset = dbOffset;
	}

	int m_nsize;
	KERNEL_NAME::GsColor m_color;
	KERNEL_NAME::GsColor m_shadowColor;
	ShadowPos m_eShadowPos;
	double m_dbOffset;
	std::string m_strFontFile;
};
GS_SMARTER_PTR(RoadTextStyle);

class GS_API WFSLayer :public Layer
{
public:
	//常规加载，只支持wgs84坐标系的数据
	WFSLayer(std::string strUrl, std::string strTypeName, double dbheight = 0);
	//可支持地方坐标系的数据
	WFSLayer(std::string strUrl, std::string strTypeName, GsRawPoint3D originPoint);
	//专为道路设计
	WFSLayer(std::string strUrl, std::string strTypeName, RoadTextStyle* roadTextStyle);
	WFSLayer(KERNEL_NAME::GsTileClass* ptrTileClass);
	
	void ReadWFSMetaDataFromServer(std::string strUrl, std::string strTypeName);
	std::string ReadFeatureType(std::string strUrl, std::string strTypeName);

	void StartPick();
	void EndPick();

	static std::vector<std::string> getTypeNameList(std::string strUrl);

	virtual ~WFSLayer();

	virtual void Visible(bool bVisible);
	virtual bool Visible() const; 
	int UID();

	//UTILITY_NAME::GsDelegate<void(Feature*)>& OnPicked();

protected:
	//std::vector<std::string> m_vecTypeList;
	bool m_bIsPick;

};

GS_SMARTER_PTR(WFSLayer);

GLOBE_ENDNS


