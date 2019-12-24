#pragma once
#include "Element.h"
#include <canvas.h>
#include <event.h>

namespace GeoGlobe
{
	class BillBoardPicker;
}

GLOBE_NS

class Feature;
class AnimatorAction;

/*
提供多种样式billboard,在图标或文字正下面会有一根线支起来
提供图标和不提供图标表现不一样
*/
class GS_API GeoBillBoardElement :public Element
{
public:
	/*
	x,y,z：位置
	strText：显示文本(可以提供"\n"以回车)
	strIcon：显示图标(如果提供了该值，文字默认在图标右边并且有一根在图标正下方的线把它们支起来。如果没提供该值，线会在文字正下方，并且文字被一个白色透明框包起来)
	nTextCenterOffset：如果设置了此参数并且不为-100，则文字会在图标水平中央显示(默认文字在图标右侧)，值本身为文字在垂直方向的偏移用来调节位置
	nMinLod：最小可见距离（物体离相机）
	nMaxLod：最大可见距离（物体离相机）
	*/
	GeoBillBoardElement(double x, double y, double z, std::string strText, std::string strIcon = "", int nTextCenterOffset = -100, int nMinLod = 0, int nMaxLod = INT_MAX);

	virtual void setPosition(double x, double y, double z);
	virtual void getPosition(double& x, double& y, double& z);

	virtual ~GeoBillBoardElement();

	void Text(std::string strText);
	std::string Text();

	//设置图标
	void Icon(std::string strIcon);
	//获取图标url
	std::string Icon();
	
	//设置图标
	void setImage(unsigned char* ptrData,int nWidth,int nHeight);

	//默认会自动加载C:\Windows\Fonts目录下的字体,例如微软雅黑msyh.ttc只需要传入字体名即可(注意需要后缀名)如：youclass->Font("msyh.ttc")
	//也可以自己指定字体路径例如：youclass->Font("./Font/msyh.ttc")
	void Font(std::string strFont);
	std::string Font();

	void CoverAutoHide(bool bAutoHide);
	bool CoverAutoHide();

	//文字位置
	void TextAlignment(const Alignment& alignment);
	//获取图标位置
	Alignment TextAlignment();

	//设置背景颜色，只有在没给图标情况下生效
	void setBackGrandColor(const KERNEL_NAME::GsColor& color);
	KERNEL_NAME::GsColor getBackGrandColor();

	//设置边线颜色，只有在没给图标情况下生效
	void setOutLineColor(const KERNEL_NAME::GsColor& color);
	KERNEL_NAME::GsColor getOutLineColor();

	//设置线颜色
	void setLineColor(const KERNEL_NAME::GsColor& color);
	KERNEL_NAME::GsColor getLineColor();

	//设置折线角度,该角度为相对与水平方向逆时针夹角，如果设为90°则为垂直线
	void setPolylineAngle(const double& dbAngle);
	//获得折线角度
	double getPolylineAngel();

	//设置底部线垂线长度
	void setBottomLen(const double& dbBottomLen);
	//获得底部线垂线长度
	double getBottomLen();

	//设置折线长度
	void setPolylineLen(const double& dbPolylineLen);
	//获得折线长度
	double getPolylineLen();

	void HightLight();
	void CancelHightLight();

	// 设置动画执行者
	void SetAnimatorAction(AnimatorAction* action);
	// 获得动画执行者
	AnimatorAction* GetAnimatorAction();

	//设置阴影颜色
	void setShadowColor(const KERNEL_NAME::GsColor& color);
	//得到阴影颜色
	KERNEL_NAME::GsColor getShadowColor();
	//设置阴影偏移
	void setShadowOffset(const double& dbShadowOffset);
	//得到阴影偏移
	double getShadowOffset();
	//设置阴影位置
	void setShadowPos(const ShadowStyle& m_eShadowPos);
	//得到阴影位置
	ShadowStyle getShadowPos();

	//设置文本颜色，r[0-255],g[0-255],b[0-255],a[0-1]
	void TextColor(int r ,int g, int b, int a = 1.0);
	void TextColor(KERNEL_NAME::GsColor color);
	KERNEL_NAME::GsColor TextColor();

	void TextSize(double dbSize);
	double TextSize();

	//设置文本发光颜色，r[0-255],g[0-255],b[0-255],a[0-1]，注意！若文字size超过35时，此效果会异常
	void TextHaloColor(int r, int g, int b, int a = 1.0);
	//设置是否可以动态更新标牌文本
	void Dynamic(bool bDynamic);
	// 设置lod
	void setLod(int minLod = 0, int maxLod = INT_MAX);

	/*保存用户数据*/
	void setValue(std::string key, std::string value);
	/*取出用户数据*/
	void getValue(std::string& key, std::string& value);

	GsReference* LodHandle()
	{
		return m_ptrLodElement.p;
	}

	bool Visible();
	void Visible(bool bVisible);

	/*开启拾取*/
	static void StartPick();
	/*结束拾取*/
	static void EndPick();
	/*被拾取委托*/
	static UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Feature*)>& OnPicked();
	/*更新委托*/
	UTILITY_NAME::GsDelegate<void(Element*)>& Updata();

protected:

	void CreateGeoBillboard(int nMinLod, int nMaxLod, int nTextCenterOffset);

	std::string m_strText;
	std::string m_strIcon;

	static GeoGlobe::BillBoardPicker * m_Picker;
	static bool m_bIsPick;

	Alignment m_textAlignment;

	GsReferencePtr m_ptrLodElement;
	bool m_bVisible;
	bool m_bUpdateCallBack;
};

GS_SMARTER_PTR(GeoBillBoardElement);

GLOBE_ENDNS
