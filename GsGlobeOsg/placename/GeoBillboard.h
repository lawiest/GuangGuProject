/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
* Copyright 2008-2014 Pelican Mapping
* http://osgearth.org
*
* osgEarth is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#ifndef GEOGLOBE_PLACENAME_GEOBILLBOARD
#define GEOGLOBE_PLACENAME_GEOBILLBOARD 1
 
#include <osgEarthSymbology/Style>

#include <Analysis/SphereElement.h>
#include <osgEarthAnnotation/GeoPositionNode>
#include <osgEarthAnnotation/AnnotationData>
#include <osgEarth/ScreenSpaceLayout>
#include <Animator.h>
#include <osgEarth/VirtualProgram>

namespace GeoGlobe { namespace PlaceName
{
    

	/*改写osgEarth自带的placeNode类，补充了图标部分
	 原来placenode图标不被共用，所以这里改写了*/
    class  GeoBillboard : public osgEarth::Annotation::GeoPositionNode
    {
    public:
        /**
         * Constructs a new place node. You can specify an icon marker by
         * adding a IconSymbol to the Style.
         *
         * @param mapNode   MapNode that helps position this annotation
         * @param position  Initial location of the annotation
         * @param labelText Text to place next to the icon
         * @param style     Optional style settings.
         */
        GeoBillboard(
            const osgEarth::GeoPoint&    position,
            const std::string& labelText,
            const osgEarth::Style&       style = osgEarth::Style(),
			const bool bShowAlways = false,
			int textoffSet = -100);


        /**
         * Text label content
         */
        void setText( const std::string& text );
        const std::string& getText() const { return _text; }

		void setIconImage(osg::Image* image);
		osg::Image* getIconImage() const { return _image.get(); }

		void setVisible(const bool& bVisible);
		const bool& getVisible() const { return _bVisible; }

		void setBackGrandColor(const osg::Vec4& color);
		osg::Vec4 getBackGrandColor();

		void setOutLineColor(const osg::Vec4& color);
		osg::Vec4 getOutLineColor();

		void setLineColor(const osg::Vec4& color);
		osg::Vec4 getLineColor();

		void setDrawableColor(osg::Drawable* ptrDraw, const osg::Vec4& color);
		osg::Vec4 getDrawableColor(osg::Drawable* ptrDraw);

		void setPolylineAngle(const double& dbAngle);
		double getPolylineAngel();

		void setBottomLen(const double& dbBottomLen);
		double getBottomLen();

		void setPolylineLen(const double& dbPolylineLen);
		double getPolylineLen();

		void setShadowColor(const osg::Vec4& color);
		osg::Vec4 getShadowColor();

		void setShadowOffset(const double& dbShadowOffset);
		double getShadowOffset();

		void setShadowPos(const osgText::Text::BackdropType& m_eShadowPos);
		osgText::Text::BackdropType getShadowPos();

		void setTextColor(const osg::Vec4& color);
		osg::Vec4 getTextColor();

		void HightLight();
		void CancelHightLight();

		void setAnimatorAction(GLOBE_NAME::AnimatorAction* animatorAction);
		// 获取动画属性部分
		GLOBE_NAME::AnimatorAction* getAnimatorAction();

		void onAnimationStop(GLOBE_NAME::ActionState state);

		osgEarth::Symbology::TextSymbol* CorrectTextSymbol(osgEarth::Symbology::TextSymbol* textSymbol);

        /**
         * Style (for text and placement)
         */
        void setStyle( const osgEarth::Style& style );
        const osgEarth::Style& getStyle() const { return _style; }

		/** Sets the node position */
		virtual void setPosition( const osgEarth::GeoPoint& pos );

    public: // OrthoNode override

        virtual void setAnnotationData(osgEarth::Annotation:: AnnotationData* data );
		virtual void setPriority(float value);
        virtual void setDynamic( bool value );
		virtual void dirty();

		osg::Geometry* createImageGeometry(osg::Image*       image,
			const osg::Vec2s& pixelOffset,
			unsigned          textureUnit,
			double            heading,
			double            scale);

		virtual void traverse(osg::NodeVisitor& nv );

    protected:

        virtual ~GeoBillboard() { }
		
        
    private:
        osg::ref_ptr<osg::Image>           _image;
		std::string						   _iconUrl;
		bool							   _bIsIconChange;
        std::string                        _text;
		osgEarth::Style                    _style;
        osg::Group*                  _geode;
        osg::ref_ptr<const osgDB::Options> _dbOptions;
		osg::ref_ptr<osgEarth::ScreenSpaceLayoutData> _dataLayout;

		/** rotation of the label **/
		float                    _labelRotationRad;
		bool                     _followFixedCourse;
		osgEarth::GeoPoint                 _geoPointLoc;
		osgEarth::GeoPoint                 _geoPointProj;

        void init();
		void construct();
		void updateLayoutData();
		//点
		osg::ref_ptr<GeoGlobe::SphereElement> _SphereElement;

		osg::Drawable* _boxfill;
		osg::Drawable* _boxOutLine;
		osg::Geometry* _lineGeometry;
		osgText::Text* _ptrText;
		osg::StateSet* _stateSet;
		osg::Texture2D* _texture;
		osg::ref_ptr<osg::Geometry> _imageGeom;
		osg::Program* _program;
		osgEarth::VirtualProgram*  _ptrRTTProgram;

		//折线角度
		double m_PolylineAngle;
		//底部线
		double m_dbBottomLen;
		//折线长度
		double m_dbPolylineLen;

		//是否最前端显示
		bool _bShowAlways;
		bool _bVisible;
		int m_TextoffSet;

		osg::Vec4 m_vecBackgrandColor;
		osg::Vec4 m_vecOutLineColor;
		osg::Vec4 m_vecLineColor;

		osg::ref_ptr<osg::Uniform> m_colorUniform;

		osg::Vec4 m_textColor;
		osg::Vec4 m_shadowColor;
		osgText::Text::BackdropType m_eShadowPos;
		double m_dbShadowOffset;
		UTILITY_NAME::GsSmarterPtr<GLOBE_NAME::AnimatorAction> m_ptrAnimatorAction;

	private:
		static osg::observer_ptr<osg::StateSet> s_geodeStateSet;
		static osg::observer_ptr<osg::StateSet> s_imageStateSet;

		osg::ref_ptr<osg::StateSet> _geodeStateSet, _imageStateSet;
    };

} } // namespace osgEarth::Annotation

#endif //OSGEARTH_ANNOTATION_PLACE_NODE_H
