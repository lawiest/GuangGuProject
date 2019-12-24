/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
* Copyright 2016 Pelican Mapping
* http://osgearth.org
*
* osgEarth is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#ifndef GEOGLOBE_ANNOTATION_PLACE_NODE_H
#define GEOGLOBE_ANNOTATION_PLACE_NODE_H 1

#include <osgEarthAnnotation/GeoPositionNode>
#include <osgEarthSymbology/Style>
#include <osgEarth/ScreenSpaceLayout>
#include <Animator.h>
#include <osgEarth/VirtualProgram>

namespace GeoGlobe {
	namespace Annotation
	{
		using namespace osgEarth;
		using namespace osgEarth::Symbology;

		/**
		* A PlaceNode combines an 2D icon, a label, support for mouse interaction
		* and a popup control (eventually)
		*/
		class  PlaceNode : public osgEarth::Annotation::GeoPositionNode
		{
		public:


			/**
			* Constructs a new place node
			*
			* @param mapNode   MapNode that helps position this annotation
			* @param position  Initial location of the annotation
			* @param iconImage Image of the place icon
			* @param labelText Text to place next to the icon
			* @param style     Optional style settings.
			*/
			PlaceNode(
				const GeoPoint&    position,
				osg::Image*        iconImage,
				const std::string& labelText,
				const Style&       style = Style());

			/**
			* Constructs a new place node. You can specify an icon marker by
			* adding a IconSymbol to the Style.
			*
			* @param mapNode   MapNode that helps position this annotation
			* @param position  Initial location of the annotation
			* @param labelText Text to place next to the icon
			* @param style     Optional style settings.
			*/
			PlaceNode(
				const GeoPoint&    position,
				const std::string& labelText,
				const Style&       style = Style());

			/**
			* Constuct a new place node entirely from symbology
			*
			* @param mapNode   MapNode that helps position this annotation
			* @param position  Initial location of the annotation
			* @param style     Style settings.
			* @param dbOptions I/O options for embedded resource loading
			*/
			PlaceNode(
				const GeoPoint&       position,
				const Style&          style,
				const osgDB::Options* dbOptions);

			/**
			* Deserializes a place node from Config data. (internal method)
			*/
			PlaceNode(
				const Config&         conf,
				const osgDB::Options* dbOptions);

			/**
			* Image to use for the icon
			*/
			void setIconImage(osg::Image* image);
			osg::Image* getIconImage() const { return _image.get(); }

			/**
			* Text label content
			*/
			void setText(const std::string& text);
			virtual const std::string& getText() const { return _text; }

			/**
			* Style (for text and placement)
			*/
			void setStyle(const Style& style);
			const Style& getStyle() const { return _style; }

			void setShadowColor(const osg::Vec4& color);
			const osg::Vec4& getShadowColor();

			void setShadowOffset(const double& dbShadowOffset);
			const double& getShadowOffset();

			void setShadowPos(const osgText::Text::BackdropType& m_eShadowPos);
			const osgText::Text::BackdropType& getShadowPos();

			void setAnimatorAction(GLOBE_NAME::AnimatorAction* animatorAction);
			// 获取动画属性部分
			GLOBE_NAME::AnimatorAction* getAnimatorAction();
			void onAnimationStop(GLOBE_NAME::ActionState state);

		public: // GeoPositionNode override

			virtual void setPriority(float value);
			virtual void setDynamic(bool value);

			virtual void dirty();

			virtual void traverse(osg::NodeVisitor& nv);

		protected:

			virtual ~PlaceNode() { }

			osg::ref_ptr<osg::Image>           _image;
			std::string						   _iconUrl;
			bool							   _bIsIconChange;
			std::string                        _text;
			Style                              _style;
			class osg::Group*                  _geode;
			osg::ref_ptr<const osgDB::Options> _dbOptions;
			osg::ref_ptr<ScreenSpaceLayoutData> _dataLayout;
			osgText::Text* _ptrText;
			osg::ref_ptr<osg::Geometry> _imageGeom;
			osg::Program* _program;
			osgEarth::VirtualProgram*  _ptrRTTProgram;

			/** rotation of the label **/
			float                    _labelRotationRad;
			bool                     _followFixedCourse;
			GeoPoint                 _geoPointLoc;
			GeoPoint                 _geoPointProj;

			osg::Vec4 m_textColor;
			osg::Vec4 m_shadowColor;
			osgText::Text::BackdropType m_eShadowPos;
			double m_dbShadowOffset;

			osg::ref_ptr<osg::Uniform> m_colorUniform;
			UTILITY_NAME::GsSmarterPtr<GLOBE_NAME::AnimatorAction> m_ptrAnimatorAction;

			void init();
			void construct();
			void updateLayoutData();

		private:
			PlaceNode(const PlaceNode& rhs, const osg::CopyOp& op = osg::CopyOp::DEEP_COPY_ALL) : GeoPositionNode(rhs, op) { }
			static osg::observer_ptr<osg::StateSet> s_geodeStateSet;
			static osg::observer_ptr<osg::StateSet> s_imageStateSet;

			osg::ref_ptr<osg::StateSet> _geodeStateSet, _imageStateSet;
		};

	}
} // namespace osgEarth::Annotation

#endif //OSGEARTH_ANNOTATION_PLACE_NODE_H

///* -*-c++-*- */
///* osgEarth - Geospatial SDK for OpenSceneGraph
//* Copyright 2018 Pelican Mapping
//* http://osgearth.org
//*
//* osgEarth is free software; you can redistribute it and/or modify
//* it under the terms of the GNU Lesser General Public License as published by
//* the Free Software Foundation; either version 2 of the License, or
//* (at your option) any later version.
//*
//* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//* IN THE SOFTWARE.
//*
//* You should have received a copy of the GNU Lesser General Public License
//* along with this program.  If not, see <http://www.gnu.org/licenses/>
//*/
//#ifndef GEOGLOBE_ANNOTATION_PLACE_NODE_H
//#define GEOGLOBE_ANNOTATION_PLACE_NODE_H 1
//
//#include <osgEarthAnnotation/GeoPositionNode>
//#include <osgEarthSymbology/Style>
//#include <osgEarth/ScreenSpaceLayout>
//
//namespace GeoGlobe {
//	namespace Annotation
//	{
//		using namespace osgEarth;
//		using namespace osgEarth::Annotation;
//		using namespace osgEarth::Symbology;
//
//		/**
//		* A PlaceNode combines an 2D icon, a label, support for mouse interaction
//		* and a popup control (eventually)
//		*/
//		class PlaceNode : public GeoPositionNode
//		{
//		public:
//			META_AnnotationNode(osgEarthAnnotation, PlaceNode);
//
//			//! Constructs a new empty place node.
//			PlaceNode();
//
//			//! Construct new node with text content and default styling.
//			PlaceNode(const std::string& text,
//				const Style&       style = Style(),
//				osg::Image*        image = 0L);
//
//			//! Construct new node with position, text content and default styling.
//			PlaceNode(const GeoPoint&    position,
//				const std::string& text = "",
//				const Style&       style = Style(),
//				osg::Image*        image = 0L);
//
//			//! Deserialize a place node.
//			PlaceNode(
//				const Config& conf,
//				const osgDB::Options* readOptions);
//
//			/**
//			* Image to use for the icon
//			*/
//			void setIconImage(osg::Image* image);
//			osg::Image* getIconImage() const { return _image.get(); }
//
//			/**
//			* Text label content
//			*/
//			void setText(const std::string& text);
//			virtual const std::string& getText() const { return _text; }
//
//			/**
//			* Style (for text and placement)
//			*/
//			void setStyle(const Style& style);
//			void setStyle(const Style& style, const osgDB::Options* readOptions);
//			const Style& getStyle() const { return _style; }
//
//			void setShadowColor(const osg::Vec4& color);
//			const osg::Vec4& getShadowColor();
//
//			void setShadowOffset(const double& dbShadowOffset);
//			const double& getShadowOffset();
//
//			void setShadowPos(const osgText::Text::BackdropType& m_eShadowPos);
//			const osgText::Text::BackdropType& getShadowPos();
//
//		public: // GeoPositionNode override
//
//			virtual void setPriority(float value);
//			virtual void setDynamic(bool value);
//
//			virtual Config getConfig() const;
//
//			virtual void dirty();
//
//		protected: // AnnotationNode override
//
//			virtual bool supportsRenderBinDetails() const { return false; }
//
//		protected:
//
//			virtual ~PlaceNode() { }
//
//			osg::ref_ptr<osg::Image>            _image;
//			std::string                         _text;
//			Style                               _style;
//			osg::Group*                         _geode;
//			osg::Geometry*                      _imageDrawable;
//			osg::Drawable*                      _bboxDrawable;
//			osgText::Text*                      _textDrawable;
//			osg::ref_ptr<const osgDB::Options>  _readOptions;
//			osg::ref_ptr<ScreenSpaceLayoutData> _dataLayout;
//
//			/** rotation of the label **/
//			float                    _labelRotationRad;
//			bool                     _followFixedCourse;
//			GeoPoint                 _geoPointLoc;
//			GeoPoint                 _geoPointProj;
//
//			//void init();
//			void updateLayoutData();
//			void construct();
//			void compile();
//
//		public:
//
//			void setConfig(const Config& conf);
//
//		private:
//
//			PlaceNode(const PlaceNode& rhs, const osg::CopyOp& op = osg::CopyOp::DEEP_COPY_ALL) : GeoPositionNode(rhs, op) { }
//
//		private:
//			static osg::observer_ptr<osg::StateSet> s_geodeStateSet;
//			static osg::observer_ptr<osg::StateSet> s_imageStateSet;
//
//			osg::ref_ptr<osg::StateSet> _geodeStateSet, _imageStateSet;
//
//			osg::Vec4 m_textColor;
//			osg::Vec4 m_shadowColor;
//			osgText::Text::BackdropType m_eShadowPos;
//			double m_dbShadowOffset;
//		};
//
//	}
//} // namespace osgEarth::Annotation
//
//#endif //OSGEARTH_ANNOTATION_PLACE_NODE_H
