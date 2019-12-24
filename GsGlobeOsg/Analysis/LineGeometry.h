#pragma once
#include <osg/Geometry>
#include "utility.h"
GLOBE_NS
class AnimatorAction;
GLOBE_ENDNS
namespace osg {

	class LineGeometry :public Geometry
	{
	public:
		LineGeometry();
		// 线的混合模式
		LineGeometry(float width,float imageH,osg::Image* lineImage,int mixMod, int textureType = 3);
		/** Copy constructor using CopyOp to manage deep vs shallow copy. */
		LineGeometry(const LineGeometry& geometry, const CopyOp& copyop = CopyOp::SHALLOW_COPY);
		//
		virtual void drawImplementation(RenderInfo& renderInfo) const;
		void SetLineLength(double length);
		void SetLineShape(osg::Array* vertexPoints, osg::Array* vertexDir, osg::Array* textcoord, osg::UShortArray* trangles);
		// 线的缩放的系数
		void SetLineScale(float lineScale);
		float GetLineScale();
		float getLineWidth();//获取线的宽度
		void setLineWidth(float w);
		float getLineImageHeight();//线的贴图纹理
		void setLineColor(const osg::Vec4& color);
		osg::Vec4 getLineColor();

		float getFixLod();
		void setFixLod(float fixLod);

		void setAnimatorAction(GLOBE_NAME::AnimatorAction* animatorAction);
		GLOBE_NAME::AnimatorAction* getAnimatorAction();

	protected:
		void BuildStats(int mixMod);
	protected:
		float m_lineWidth;
		float m_ImageHeigth;
		float m_LineWidthScale;
		float m_lineLenght;//线的总长度
		double m_LinePics;//
		float m_FixLod;//固定显示
		osg::ref_ptr<Image> m_lineImage;
		osg::StateSet * m_CurrStateSet;
		osg::ref_ptr<osg::Uniform> m_uniWidth;
		osg::ref_ptr<osg::Uniform> m_uniTextCoord;
		osg::ref_ptr<osg::Uniform> m_uniColor;//颜色的变量
		osg::ref_ptr<osg::Uniform> m_uPrive;// 绘制线的百分比
		float m_lineColorAphla;
		int m_TextureType;
		UTILITY_NAME::GsSmarterPtr<GLOBE_NAME::AnimatorAction> m_ptrAnimatorAction;
	};
}

