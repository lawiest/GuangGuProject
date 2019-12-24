
#pragma once
#include <utility.h>
#include <globe\GsReference.h>
#include <geometry.h>

GLOBE_NS
class AnimatorAction;

//
// 线的混合模式
enum LinePictureMixType
{
	// 使用原色绘制线型
	SRC = 1,
	//使用颜色绘制线型
	COLOR,
	// 取红色值混合计算
	RRR
};
//

enum Alignment
{
	ALIGN_NONE, ALIGN_LEFT, ALIGN_RIGHT, ALIGN_TOP, ALIGN_BOTTOM, ALIGN_CENTER
};

enum ShadowStyle
{
	SHADOW_BOTTOM_RIGHT = 0,
	SHADOW_CENTER_RIGHT,
	SHADOW_TOP_RIGHT,
	SHADOW_BOTTOM_CENTER,
	SHADOW_TOP_CENTER,
	SHADOW_BOTTOM_LEFT,
	SHADOW_CENTER_LEFT,
	SHADOW_TOP_LEFT,
	SHADOW_OUTLINE,
	SHADOW_NONE
};

enum ElementGeoType {
	Point = (1 << 10),
	Line=1<<11,
	Polygon=1<<12
};
enum ElementType
{
	eLonAndLatElement ,
	eCompassElement,
	eOverlayElement,
	eGroupElement,
	eBillboardElement =1 | ElementGeoType::Point,// 点类型的添加到之后
	eModelElement,
	eGeoBillBoardElement,
	eTextureMovieElement,
	eStreamMovieElement,
	ePointSymbolElement,
	ePointSymbolElement3D,
	eTextElement,
	eSquareElement,
	eLineSymbolElement = 1 | ElementGeoType::Line,// 线类型的添加这个之后
	elineSymbolElement3D,
	eLinePictureElement3D,
	ePolygonSymbolElement = 1 | ElementGeoType::Polygon,// 面类型的添加这个之后
};

class GS_API Element :public GeoStar::Utility::GsRefObject
{
public:
	Element();
	virtual ~Element();

	GsReference* Handle()
	{
		return m_ptrElementHandle.p;
	}

	ElementType Type();
	std::string Name();
	void setName(const std::string& name);
	std::string UID();
	void setUID(const std::string& uid);
	void setAtrribute(const std::string&name, const std::string &value);
	std::string getAttribute(const std::string&name);
	std::map<std::string, std::string>& getAttributes();

	virtual void Build() {};
	virtual void setPosition(double x, double y, double z) {};
	virtual void getPosition(double& x, double& y, double& z) {};

	virtual bool Visible() { return false; };
	virtual void Visible(bool bVisible) {};

	double Longitude();
	double Latitude();
	double Height();

	UTILITY_NAME::GsDelegate<void(Element*)> OnUpdate;
	virtual UTILITY_NAME::GsDelegate<void(Element*)>& Updata() {return OnUpdate; };
	UTILITY_NAME::GsDelegate<void(KERNEL_NAME::GsRawPoint3D)> OnPositionChange;

	friend class ElementBox;
	friend class OverlayElement;

protected:	
	
	GsReferencePtr m_ptrElementHandle;
	ElementType m_ElementType;

	std::map<std::string, std::string> m_mapAttribute;
	std::string m_strName;
	std::string m_UID;

	double m_dLongitude;
	double m_dLatitude;
	double m_dHeigh;

	double m_xMin;
	double m_yMin;
	double m_xMax;
	double m_yMax;
};

GS_SMARTER_PTR(Element);

GLOBE_ENDNS