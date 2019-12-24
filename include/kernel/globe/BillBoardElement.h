#pragma once
#include "Element.h"
#include <canvas.h>
#include <event.h>
#include <Feature.h>

namespace GeoGlobe
{
	class BillBoardPicker;
}

GLOBE_NS

/*
提供一个基本billboard类
该billboard只包含一个图标和文字，并且他们的相对位置可以设置
*/
class GS_API BillBoardElement :public Element
{
public:
	/*
	x,y,z：位置
	strText：显示文本
	strIcon：显示图标
	nMinLod：最小可见距离（物体离相机）
	nMaxLod：最大可见距离（物体离相机）
	*/
	BillBoardElement(double x, double y, double z, std::string strText, std::string strIcon = "", int nMinLod = 0, int nMaxLod = INT_MAX);
	
	virtual void setPosition(double x, double y, double z);
	virtual void getPosition(double& x, double& y, double& z);

	/*保存用户数据*/
	void setValue(std::string key,std::string value);
	/*取出用户数据*/
	void getValue(std::string& key, std::string& value);

	virtual ~BillBoardElement();
	
	//设置图标
	void Icon(std::string strIcon);
	//获取图标url
	std::string Icon();
	//图标位置
	void IconAlignment(Alignment alignment);
	//获取图标位置
	Alignment IconAlignment();

	void Text(std::string strText);
	std::string Text();

	//首先系统中要有这个字体然后这样写：simhei.ttf
	void Font(std::string strFont);
	std::string Font();
	
	void CoverAutoHide(bool bAutoHide);
	bool CoverAutoHide();

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

	//设置阴影颜色
	void setShadowColor(const KERNEL_NAME::GsColor& color);
	//得到阴影颜色
	const KERNEL_NAME::GsColor& getShadowColor();
	//设置阴影偏移
	void setShadowOffset(const double& dbShadowOffset);
	//得到阴影偏移
	const double& getShadowOffset();
	//设置阴影位置
	void setShadowPos(const ShadowStyle& m_eShadowPos);
	//得到阴影位置
	const ShadowStyle& getShadowPos();
	// 设置lod
	void setLod(int minLod=0, int maxLod=INT_MAX);

	// 设置动画执行者
	void SetAnimatorAction(AnimatorAction* action);
	// 线的动画执行者
	AnimatorAction* GetAnimatorAction();

	/*开启拾取*/
	static void StartPick();
	/*结束拾取*/
	static void EndPick();

	//void setText(double x, double y, double z);

	bool Visible();
	void Visible(bool bVisible);

	GsReference* LodHandle()
	{
		return m_ptrLodElement.p;
	}

	/*被拾取委托*/
	static UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Feature*)>& OnPicked();
	/*更新委托*/
	UTILITY_NAME::GsDelegate<void(Element*)>& Updata();

protected:

	void CreateLabel(int dbMinLod , int dbMaxLod );

	static GeoGlobe::BillBoardPicker * m_Picker;
	static bool m_bIsPick;

	std::string m_strText;
	std::string m_strIcon;


	GsReferencePtr m_ptrLodElement;
	bool m_bVisible;
	bool m_bUpdateCallBack;
	Alignment m_iocnAlignment;
};

GS_SMARTER_PTR(BillBoardElement);

GLOBE_ENDNS
