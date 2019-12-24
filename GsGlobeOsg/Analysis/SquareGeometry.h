#pragma once
#include <osg/Geometry>
#include "utility.h"

GLOBE_NS
class AnimatorAction;
GLOBE_ENDNS

namespace osg
{
	//
	class SquareGeometry :public osg::Geometry
	{
	public:
		// 颜色模式
		SquareGeometry(float w, float h);
		// 图片模式
		SquareGeometry(float w, float h, osg::Image* lineImage,int mode=2);
		~SquareGeometry();
		
		virtual void drawImplementation(RenderInfo& renderInfo) const;
		//设置颜色
		void setColor(const osg::Vec4& color);
		// 获取颜色
		osg::Vec4 getColor();
		float getWidth();
		float getHeight();
		//设置动画属性部分
		void setAnimatorAction(GLOBE_NAME::AnimatorAction* animatorAction);
		// 获取动画属性部分
		GLOBE_NAME::AnimatorAction* getAnimatorAction();
	protected:
		void build();

	protected:
		float m_width;
		float m_height;
		osg::ref_ptr<Image> m_Image;
		osg::ref_ptr<osg::Uniform> m_uniColor;//颜色的变量
		osg::ref_ptr<osg::Uniform> m_uCirle;//半径
		osg::ref_ptr<osg::Uniform> m_uniRota;// 平面旋转和缩放参数
		int m_SquareMode;// 模式 圆环扩散效果，点发光效果，原始图片渲染
		float m_lineColorAphla;
		UTILITY_NAME::GsSmarterPtr<GLOBE_NAME::AnimatorAction> m_ptrAnimatorAction;
	};
}


