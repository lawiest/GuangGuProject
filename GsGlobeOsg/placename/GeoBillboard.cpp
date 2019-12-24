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

#include <PlaceName/GeoBillboard.h>
#include <osgEarthAnnotation/AnnotationUtils>
#include <osgEarthAnnotation/AnnotationRegistry>
#include <osgEarthAnnotation/GeoPositionNode>
#include <osgEarthFeatures/BuildTextFilter>
#include <osgEarthFeatures/LabelSource>
#include <osgEarth/Utils>
#include <osgEarth/Registry>
#include <osgEarth/ShaderGenerator>
#include <osgEarth/GeoMath>
#include <osgEarth/ScreenSpaceLayout>
#include <osg/Depth>
#include <osg/BlendFunc>
#include <osgText/Text>
#include <Tool/GlobalSetting.h>
#include <osgEarth/Shaders>
#include <osgEarth/VirtualProgram>
#include <osgEarth/ShaderGenerator>
#include <OsgCoreSingleton.h>
#include <osg/ImageStream>
#include <osgEarth/Lighting>

#define LC "[GeoBillboard] "

using namespace osgEarth::Annotation;
using namespace osgEarth::Features;
using namespace osgEarth::Symbology;
using namespace osgEarth;
 
static const char *billboardVertSource = {
	"// microshader - colors a fragment based on its position\n"
	"varying vec2 m_coord;\n"
	"void main(void)\n"
	"{\n"
	"   m_coord= gl_MultiTexCoord0.xy;\n"
	"   gl_Position = gl_ModelViewProjectionMatrix *gl_Vertex;\n"
	"}\n"
};

static const char * fBilboardShaderStr = {
	//"precision highp float; \n"
	"uniform sampler2D sampler2d; \n"
	"uniform  vec4 geoBill_color;  \n"
	"varying  vec2 m_coord; \n"
	"void main() \n"
	"{ \n"
	"   vec4 out_color = texture2D(sampler2d,m_coord) ;  \n"
	"   gl_FragColor =vec4(1.0,1.0,1.0,geoBill_color.a)*out_color;\n"
	"} \n"
};

namespace
{
	const char* iconVS =
		"#version " GLSL_VERSION_STR "\n"
		"out vec2 oe_GeoBillboard_texcoord; \n"
		"void oe_GeoBillboard_icon_VS(inout vec4 vertex) \n"
		"{ \n"
		"    oe_GeoBillboard_texcoord = gl_MultiTexCoord0.st; \n"
		"} \n";

	const char* iconFS =
		"#version " GLSL_VERSION_STR "\n"
		"in vec2 oe_GeoBillboard_texcoord; \n"
		"uniform sampler2D oe_GeoBillboard_tex; \n"
		"void oe_GeoBillboard_icon_FS(inout vec4 color) \n"
		"{ \n"
		"    color = texture(oe_GeoBillboard_tex, oe_GeoBillboard_texcoord); \n"
		"} \n";
}

osg::observer_ptr<osg::StateSet> GeoGlobe::PlaceName::GeoBillboard::s_geodeStateSet;
osg::observer_ptr<osg::StateSet> GeoGlobe::PlaceName::GeoBillboard::s_imageStateSet;

GeoGlobe::PlaceName::GeoBillboard::GeoBillboard(
                     const GeoPoint& position,
                     const std::string& text,
                     const osgEarth::Style& style,
					 const bool bShowAlways, 
					 int textoffSet) :
	GeoPositionNode(),
	_text(text),
	_style(style),
	_geode(0L),
	_labelRotationRad(0.),
	_followFixedCourse(false),
	_bShowAlways(bShowAlways),
	m_TextoffSet(textoffSet)
{
	setPosition(position);
	////点的初始化
	//_SphereElement = new SphereElement(mapNode);
	//getPositionAttitudeTransform()->addChild(_SphereElement);
	//_SphereElement->setPosition(position);

	//_style.getOrCreate<osgEarth::IconSymbol>()->declutter() = true;
	//_style.getOrCreate<osgEarth::IconSymbol>()->occlusionCull() = false;
	//_style.getOrCreate<osgEarth::TextSymbol>()->declutter() = true;
	//_style.getOrCreate<osgEarth::TextSymbol>()->occlusionCull() = false;
	_boxfill = NULL;
	_boxOutLine = NULL;
	_bVisible = true;
	_ptrText = NULL;
	_imageGeom = NULL;
	_program = NULL;
	_ptrRTTProgram = NULL;

	m_vecBackgrandColor = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_vecOutLineColor = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_vecLineColor = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_textColor = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_shadowColor = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_dbShadowOffset = 0.0f;
	m_eShadowPos = (osgText::Text::BackdropType)(-100);
	_bIsIconChange = false;

	//折线角度
	m_PolylineAngle = osg::DegreesToRadians(90.0);
	//底部线
	m_dbBottomLen = 50;
	//折线长度
	m_dbPolylineLen = 40;
	construct();
    init();
}

void GeoGlobe::PlaceName::GeoBillboard::construct()
{
	_geode = 0L;
	_labelRotationRad = 0.0f;
	_followFixedCourse = false;

	// This class makes its own shaders
	ShaderGenerator::setIgnoreHint(this, true);

	// Construct the shared state sets
	if (s_geodeStateSet.lock(_geodeStateSet) == false)
	{
		static Threading::Mutex s_mutex;
		Threading::ScopedMutexLock lock(s_mutex);

		if (s_geodeStateSet.lock(_geodeStateSet) == false)
		{
			s_geodeStateSet = _geodeStateSet = new osg::StateSet();

			// draw in the screen-space bin
			ScreenSpaceLayout::activate(_geodeStateSet.get());

			// completely disable depth buffer
			_geodeStateSet->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false), 1);

			// Disable lighting for place nodes by default
			_geodeStateSet->setDefine(OE_LIGHTING_DEFINE, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
		}
	}

	if (s_imageStateSet.lock(_imageStateSet) == false)
	{
		static Threading::Mutex s_mutex;
		Threading::ScopedMutexLock lock(s_mutex);

		if (s_imageStateSet.lock(_imageStateSet) == false)
		{
			s_imageStateSet = _imageStateSet = new osg::StateSet();
			VirtualProgram* vp = VirtualProgram::getOrCreate(_imageStateSet.get());
			vp->setName("GeoBillboard::imageStateSet");
			vp->setFunction("oe_GeoBillboard_icon_VS", iconVS, ShaderComp::LOCATION_VERTEX_MODEL);
			vp->setFunction("oe_GeoBillboard_icon_FS", iconFS, ShaderComp::LOCATION_FRAGMENT_COLORING);
			_imageStateSet->addUniform(new osg::Uniform("oe_GeoBillboard_tex", 0));
		}
	}
}

osgEarth::Symbology::TextSymbol* GeoGlobe::PlaceName::GeoBillboard::CorrectTextSymbol(TextSymbol* textSymbol)
{
	osgEarth::Symbology::TextSymbol* ptrTextSym = (osgEarth::Symbology::TextSymbol*)textSymbol->clone(osg::CopyOp::DEEP_COPY_ALL);
	switch (ptrTextSym->alignment().get())
	{
	case TextSymbol::ALIGN_LEFT_CENTER:
		ptrTextSym->alignment() = TextSymbol::ALIGN_RIGHT_CENTER;
		break;
	case TextSymbol::ALIGN_RIGHT_CENTER:
		ptrTextSym->alignment() = TextSymbol::ALIGN_LEFT_CENTER;
		break;
	case TextSymbol::ALIGN_CENTER_TOP:
		ptrTextSym->alignment() = TextSymbol::ALIGN_CENTER_BOTTOM;
		break;
	case TextSymbol::ALIGN_CENTER_BOTTOM:
		ptrTextSym->alignment() = TextSymbol::ALIGN_CENTER_TOP;
		break;
	default:
		break;
	}

	return ptrTextSym;
}

const std::string& UNIFORM_IS_TEXT()
{
	static std::string s = "oeAnno_isText";
	return s;
}

osg::Drawable* create2DOutline(const osg::BoundingBox& box, float padding, const osg::Vec4& color)
{
	osg::Geometry* geom = new osg::Geometry();
	geom->setUseVertexBufferObjects(true);

	osg::Vec3Array* v = new osg::Vec3Array();
	v->reserve(4);
	v->push_back(osg::Vec3(box.xMin() - padding, box.yMin() - padding, 0));
	v->push_back(osg::Vec3(box.xMax() + padding, box.yMin() - padding, 0));
	v->push_back(osg::Vec3(box.xMax() + padding, box.yMax() + padding, 0));
	v->push_back(osg::Vec3(box.xMin() - padding, box.yMax() + padding, 0));
	geom->setVertexArray(v);
	if (v->getVertexBufferObject())
		v->getVertexBufferObject()->setUsage(GL_STATIC_DRAW_ARB);

	osg::DrawElementsUByte* b = new osg::DrawElementsUByte(GL_LINE_LOOP);
	b->reserve(4);
	b->push_back(0); b->push_back(1); b->push_back(2); b->push_back(3);
	geom->addPrimitiveSet(b);

	osg::Vec4Array* c = new osg::Vec4Array(1);
	(*c)[0] = color;
	geom->setColorArray(c);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	static osg::ref_ptr<osg::Uniform> s_isNotTextUniform = new osg::Uniform(osg::Uniform::BOOL, UNIFORM_IS_TEXT());
	s_isNotTextUniform->set(false);
	geom->getOrCreateStateSet()->addUniform(s_isNotTextUniform.get());

	return geom;
}

osg::Drawable* create2DQuad(const osg::BoundingBox& box, float padding, const osg::Vec4& color)
{
	osg::Geometry* geom = new osg::Geometry();
	geom->setUseVertexBufferObjects(true);

	osg::Vec3Array* v = new osg::Vec3Array();
	v->reserve(4);
	v->push_back(osg::Vec3(box.xMin() - padding, box.yMin() - padding, 0));
	v->push_back(osg::Vec3(box.xMax() + padding, box.yMin() - padding, 0));
	v->push_back(osg::Vec3(box.xMax() + padding, box.yMax() + padding, 0));
	v->push_back(osg::Vec3(box.xMin() - padding, box.yMax() + padding, 0));
	geom->setVertexArray(v);
	if (v->getVertexBufferObject())
		v->getVertexBufferObject()->setUsage(GL_STATIC_DRAW_ARB);

	osg::DrawElementsUByte* b = new osg::DrawElementsUByte(GL_TRIANGLES);
	b->reserve(6);
	b->push_back(0); b->push_back(1); b->push_back(2);
	b->push_back(2); b->push_back(3); b->push_back(0);
	geom->addPrimitiveSet(b);

	osg::Vec4Array* c = new osg::Vec4Array(1);
	(*c)[0] = color;
	geom->setColorArray(c);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);

	// add the static "isText=true" uniform; this is a hint for the annotation shaders
	// if they get installed.
	static osg::ref_ptr<osg::Uniform> s_isTextUniform = new osg::Uniform(osg::Uniform::BOOL, UNIFORM_IS_TEXT());
	s_isTextUniform->set(false);
	geom->getOrCreateStateSet()->addUniform(s_isTextUniform.get());

	return geom;
}

void GeoGlobe::PlaceName::GeoBillboard::init()
{
	osg::Group* pat = getPositionAttitudeTransform();
	pat->removeChildren(0, pat->getNumChildren());

	_geode = new osg::Group();
	_geode->setCullingActive(false);
	_geode->setStateSet(_geodeStateSet.get());
	osg::Drawable* text = 0L;

	_geode->setComputeBoundingSphereCallback(new ControlPointCallback());

	// If there's no explicit text, look to the text symbol for content.
	if ( _text.empty() && _style.has<TextSymbol>() )
	{
		_text = _style.get<TextSymbol>()->content()->eval();
	}

	const TextSymbol* symbol = _style.get<TextSymbol>();

	// Handle the rotation if any
	if (symbol && symbol->onScreenRotation().isSet())
	{
		_labelRotationRad = osg::DegreesToRadians(symbol->onScreenRotation()->eval());
	}

	// In case of a label must follow a course on map, we project a point from the position
	// with the given bearing. Then during culling phase we compute both points on the screen
	// and then we can deduce the screen rotation
	// may be optimized...
	else if (symbol && symbol->geographicCourse().isSet())
	{
		_followFixedCourse = true;
		_labelRotationRad = osg::DegreesToRadians(symbol->geographicCourse()->eval());
	}


	osg::ref_ptr<const InstanceSymbol> instance = _style.get<InstanceSymbol>();

	const IconSymbol* icon = NULL;
	std::string strIcon = "";

	if (instance.valid())
	{
		icon = instance->asIcon();
		if (icon)
			strIcon = icon->url()->eval();
	}

	if (strIcon != _iconUrl)
		_bIsIconChange = true;
	else
		_bIsIconChange = false;

	if (!_image.valid() || strIcon != _iconUrl)
	{
		URI imageURI;

		if ( icon )
		{
			if ( icon->url().isSet() )
			{
				imageURI = URI( icon->url()->eval(), icon->url()->uriContext() );
				_iconUrl = icon->url()->eval();
			}
		}

		if ( !imageURI.empty() )
		{
			_image = imageURI.getImage( _dbOptions.get() );
			osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(_image.get());
			if (imagestream)
			{
				imagestream->play();
			}
		}
	}

	osg::BoundingBox imageBox(0, 0, 0, 0, 0, 0);

	// found an image; now format it:
	if ( _image.get() )
	{
		// Scale the icon if necessary
		double scale = 1.0;
		if ( icon && icon->scale().isSet() )
		{
			scale = icon->scale()->eval();
		}

		double s = scale * _image->s();
		double t = scale * _image->t();

		// this offset anchors the image at the bottom
		osg::Vec2s offset;
		if ( !icon || !icon->alignment().isSet() )
		{	
			// default to bottom center
			offset.set(0.0, t / 2.0);
		}
		else
		{	// default to bottom center
			switch (icon->alignment().value())
			{
			case IconSymbol::ALIGN_LEFT_TOP:
				offset.set((s / 2.0), -(t / 2.0));
				break;
			case IconSymbol::ALIGN_LEFT_CENTER:
				offset.set((s / 2.0), 0.0);
				break;
			case IconSymbol::ALIGN_LEFT_BOTTOM:
				offset.set((s / 2.0), (t / 2.0));
				break;
			case IconSymbol::ALIGN_CENTER_TOP:
				offset.set(0.0, -(t / 2.0));
				break;
			case IconSymbol::ALIGN_CENTER_CENTER:
				offset.set(0.0, 0.0);
				break;
			case IconSymbol::ALIGN_CENTER_BOTTOM:
			default:
				offset.set(0.0, (t / 2.0));
				break;
			case IconSymbol::ALIGN_RIGHT_TOP:
				offset.set(-(s / 2.0), -(t / 2.0));
				break;
			case IconSymbol::ALIGN_RIGHT_CENTER:
				offset.set(-(s / 2.0), 0.0);
				break;
			case IconSymbol::ALIGN_RIGHT_BOTTOM:
				offset.set(-(s / 2.0), (t / 2.0));
				break;
			}
		}

		// Apply a rotation to the marker if requested:
		double heading = 0.0;
		if ( icon && icon->heading().isSet() )
		{
			heading = osg::DegreesToRadians( icon->heading()->eval() );
		}

		_lineGeometry = new osg::Geometry;
		_lineGeometry->setUseDisplayList(false);
		_lineGeometry->setUseVertexBufferObjects(true);

		osg::Vec3Array* verts = new osg::Vec3Array();
		//verts->reserve(3);
		_lineGeometry->setVertexArray(verts);

		//颜色初始化
		osg::Vec4Array* colors = new osg::Vec4Array();
		//colors->reserve(2);
		_lineGeometry->setColorArray(colors);

		if (m_vecLineColor != osg::Vec4d(0.0f, 0.0f, 0.0f, 0.0f))
		{
			colors->push_back(m_vecLineColor);
		}
		else
		{
			m_vecLineColor = osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
			colors->push_back(m_vecLineColor);
		}

		//colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		_lineGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
		float fPadding = 6.0f;

		//顶点初始化
		double thirdX = cos(m_PolylineAngle)*(m_dbPolylineLen);
		double thirdY = sin(m_PolylineAngle)*(m_dbPolylineLen)+m_dbBottomLen;
		verts->push_back(osg::Vec3(0, 0.0, 0.0f));
		verts->push_back(osg::Vec3f(0, m_dbBottomLen, 0.0f));
		verts->push_back(osg::Vec3f(thirdX, thirdY, 0.0f));

		_lineGeometry->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP, 0, verts->size()));
		_lineGeometry->setCullingActive(false);
		_geode->addChild(_lineGeometry);
		
		offset.x() = thirdX;
		offset.y() = thirdY;

		if (fabs(m_PolylineAngle - osg::DegreesToRadians(90.0)) < FLT_EPSILON)
		{
			offset.y() += t / 2.0 - fPadding;
		}

		//if (_imageGeom == NULL)
		{
			_imageGeom = AnnotationUtils::createImageGeometry(_image.get(), offset, 0, heading, scale);
			
			if (_imageGeom)
			{
				_geode->addChild(_imageGeom);
				_imageGeom->getOrCreateStateSet()->merge(*_imageStateSet.get());
				imageBox = _imageGeom->getBoundingBox();;
				_imageGeom->setCullingActive(false);
			}
		}
		//else
		//{
		//	osg::Vec3Array* verts = (osg::Vec3Array*)_imageGeom->getVertexArray();

		//	float s = scale * _image->s();
		//	float t = scale * _image->t();

		//	float x0 = (float)offset.x() - s / 2.0;
		//	float y0 = (float)offset.y() - t / 2.0;

		//	(*verts)[0].set(x0, y0, 0);
		//	(*verts)[1].set(x0 + s, y0, 0);
		//	(*verts)[2].set(x0 + s, y0 + t, 0);
		//	(*verts)[3].set(x0, y0 + t, 0);

		//	_geode->addDrawable(_imageGeom);

 	//		_imageGeom->dirtyDisplayList();
 	//		_imageGeom->dirtyBound();
		//	verts->dirty();

		//	imageBox = osgEarth::Utils::getBoundingBox(_imageGeom);
		//	_imageGeom->setCullingActive(false);

		//	if (_bIsIconChange)
		//	{
		//		osg::StateSet* ptrState = _imageGeom->getOrCreateStateSet();
		//		osg::Texture2D* texture = dynamic_cast<osg::Texture2D*>(ptrState->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
		//		texture->setImage(_image);
		//	}
		//}
		
		if (_image.valid())
		{
			TextSymbol* textSymbol = _style.getOrCreate<TextSymbol>();
			if (!textSymbol->alignment().isSet())
				textSymbol->alignment() = textSymbol->ALIGN_RIGHT_CENTER;
		}
		TextSymbol* textSymbol = _style.getOrCreate<TextSymbol>();

		if (textSymbol)
			m_textColor = textSymbol->fill()->color();

		if (!_text.empty()&&_text!="")
		{
				osg::ref_ptr<osg::Drawable> textTemp = AnnotationUtils::createTextDrawable(_text,_style.get<TextSymbol>(),osg::Vec3(0, 0, 0));
				double dblY = textTemp->getBoundingBox().yMax() - textTemp->getBoundingBox().yMin();
				double dblX = textTemp->getBoundingBox().xMax() - textTemp->getBoundingBox().xMin();
				double dblLength = (textTemp->getBoundingBox().xMax() - textTemp->getBoundingBox().xMin()) * 0.3f;
				//为文字在中间显示设计，如果m_TextoffSet不等于-100就表示文字居中
				//m_TextoffSet值本身为在垂直方向上的偏移
				if (m_TextoffSet == -100)
				{
					double dbSpace = 6;

					text = AnnotationUtils::createTextDrawable(_text, _style.get<TextSymbol>(), osg::Vec3(thirdX, thirdY, 0));

					_ptrText = dynamic_cast<osgText::Text*>(text);
					_ptrText->setAlignment(osgText::TextBase::LEFT_CENTER);
					_ptrText->setCullingActive(false);

					if (m_shadowColor == osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f))
					{
						m_shadowColor = osg::Vec4(0.0, 0.0, 0.0, 0.5);
						_ptrText->setBackdropColor(m_shadowColor);
					}
					else
					{
						_ptrText->setBackdropColor(m_shadowColor);
					}
			
					if (fabs(m_dbShadowOffset) > -0.0001 && fabs(m_dbShadowOffset) < 0.0001)
					{
						m_dbShadowOffset = 0.07;
						_ptrText->setBackdropOffset(m_dbShadowOffset);
					}
					else
					{
						_ptrText->setBackdropOffset(m_dbShadowOffset);
					}

					if (m_eShadowPos == (osgText::Text::BackdropType) (-100))
					{
						m_eShadowPos = osgText::Text::BackdropType::OUTLINE;
						_ptrText->setBackdropType(m_eShadowPos);
					}
					else
					{
						_ptrText->setBackdropType(m_eShadowPos);
					}

					double anchorX = 0.0;
					double anchorY = 0.0;
					switch (textSymbol->alignment().get())
					{
					case TextSymbol::ALIGN_LEFT_CENTER:
						anchorX = thirdX - dblX - s / 2;
						anchorY = thirdY;
						break;
					case TextSymbol::ALIGN_RIGHT_CENTER:
						anchorX = thirdX + s / 2;
						anchorY = thirdY;
						break;
					case TextSymbol::ALIGN_CENTER_TOP:
						anchorX = thirdX - dblX / 2;
						anchorY = thirdY + dblY / 2 + s / 2;
						break;
					case TextSymbol::ALIGN_CENTER_BOTTOM:
						anchorX = thirdX - dblX / 2;
						anchorY = thirdY - dblY / 2 - s / 2;
						break;
					default:
						break;
					}


					if (fabs(m_PolylineAngle - osg::DegreesToRadians(90.0)) < FLT_EPSILON)
					{
						anchorY += t / 2.0 /*- fPadding*/;
					}

					_ptrText->setPosition(osg::Vec3(anchorX, anchorY, 0));
				}
				else
				{
					double textY = thirdY + m_TextoffSet;
					if (fabs(m_PolylineAngle - osg::DegreesToRadians(90.0)) < FLT_EPSILON)
					{
						textY += t / 2.0 - fPadding;
					}

					text = AnnotationUtils::createTextDrawable(_text, _style.get<TextSymbol>(), osg::Vec3(thirdX, textY, 0.0f));
					_ptrText = dynamic_cast<osgText::Text*>(text);
					_ptrText->setCullingActive(false);
					_ptrText->setAlignment(osgText::TextBase::CENTER_CENTER);
				}
		}
		
	}
	else
	{
		//初始化几何
		_lineGeometry = new osg::Geometry;
		_lineGeometry->setUseDisplayList(false);
		_lineGeometry->setUseVertexBufferObjects(true);

		osg::Vec3Array* verts = new osg::Vec3Array();
		//verts->reserve(2);
		_lineGeometry->setVertexArray(verts);

		//颜色初始化
		osg::Vec4Array* colors = new osg::Vec4Array();
		//colors->reserve( 2 );
		_lineGeometry->setColorArray(colors);
		//colors->push_back( osg::Vec4(1.0f,1.0f,1.0f,1.0f) );

		if (m_vecLineColor != osg::Vec4d(0.0f, 0.0f, 0.0f, 0.0f))
		{
			colors->push_back(m_vecLineColor);
		}
		else
		{
			m_vecLineColor = osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
			colors->push_back(m_vecLineColor);
		}

		_lineGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

		double thirdX = cos(m_PolylineAngle)*(m_dbPolylineLen);
		double thirdY = sin(m_PolylineAngle)*(m_dbPolylineLen)+m_dbBottomLen;
		//顶点初始化
		verts->push_back(osg::Vec3(0, 0.0, 0.0f));
		verts->push_back(osg::Vec3f(0, m_dbBottomLen, 0.0f));
		verts->push_back(osg::Vec3f(thirdX, thirdY, 0.0f));
		//double dblMidX = (text->getBoundingBox().xMin() + text->getBoundingBox().xMax()) * 0.5f;
		//verts->push_back(osg::Vec3(dblMidX, text->getBoundingBox().yMin() - fPadding, 0.0f));
		//verts->push_back(osg::Vec3(dblMidX, text->getBoundingBox().yMin() - fPadding * 2.0 - dblLength, 0.0f));

		_lineGeometry->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP, 0, verts->size()));
		_lineGeometry->setCullingActive(false);
		_geode->addChild(_lineGeometry);

		osg::ref_ptr<osg::Drawable> textTemp = AnnotationUtils::createTextDrawable(
			_text,
			_style.get<TextSymbol>(),
			osg::Vec3(0, 0, 0));
		double dblY = textTemp->getBoundingBox().yMax() - textTemp->getBoundingBox().yMin();
		double dblX = textTemp->getBoundingBox().xMax() - textTemp->getBoundingBox().xMin();
		double dblLength = (textTemp->getBoundingBox().xMax() - textTemp->getBoundingBox().xMin()) * 0.3f;

		TextSymbol* textSymbol = _style.getOrCreate<TextSymbol>();

		if (textSymbol)
		{
			if (!textSymbol->alignment().isSet())
				textSymbol->alignment() = textSymbol->ALIGN_CENTER_TOP;
			m_textColor = textSymbol->fill()->color();
		}

		double dbSpace = 6;

		text = AnnotationUtils::createTextDrawable(_text, _style.get<TextSymbol>(), osg::Vec3(thirdX, thirdY, 1));

		_ptrText = dynamic_cast<osgText::Text*>(text);
		_ptrText->setAlignment(osgText::TextBase::LEFT_CENTER);
		_ptrText->setCullingActive(false);
		if (m_shadowColor == osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f))
		{
			m_shadowColor = osg::Vec4(0.0, 0.0, 0.0, 0.5);
			_ptrText->setBackdropColor(m_shadowColor);
		}
		else
		{
			_ptrText->setBackdropColor(m_shadowColor);
		}

		if (fabs(m_dbShadowOffset) < 0.0001)
		{
			m_dbShadowOffset = 0.07;
			_ptrText->setBackdropOffset(m_dbShadowOffset);
		}
		else
		{
			_ptrText->setBackdropOffset(m_dbShadowOffset);
		}

		if (m_eShadowPos == (osgText::Text::BackdropType) (-100))
		{
			m_eShadowPos = osgText::Text::BackdropType::OUTLINE;
			_ptrText->setBackdropType(m_eShadowPos);
		}
		else
		{
			_ptrText->setBackdropType(m_eShadowPos);
		}

		double anchorX = 0.0;
		double anchorY = 0.0;
		switch (textSymbol->alignment().get())
		{
		case TextSymbol::ALIGN_LEFT_CENTER:
			anchorX = thirdX - dblX - dbSpace;
			anchorY = thirdY;
			break;
		case TextSymbol::ALIGN_RIGHT_CENTER:
			anchorX = thirdX + dbSpace;
			anchorY = thirdY;
			break;
		case TextSymbol::ALIGN_CENTER_TOP:
			anchorX = thirdX - dblX / 2;
			anchorY = thirdY + dblY / 2 + dbSpace;
			break;
		case TextSymbol::ALIGN_CENTER_BOTTOM:
			anchorX = thirdX - dblX / 2;
			anchorY = thirdY - dblY / 2 - dbSpace;
			break;
		default:
			break;
		}

		_ptrText->setPosition(osg::Vec3(anchorX, anchorY, 0));

		osg::Vec4 backgrandColor;
		if (m_vecBackgrandColor != osg::Vec4(0.0, 0.0, 0.0, 0.0))
		{
			backgrandColor = m_vecBackgrandColor;
		}
		else
		{
			backgrandColor = osg::Vec4(1.0f, 1.0f, 1.0f, 0.3f);
			m_vecBackgrandColor = backgrandColor;
		}

		_boxfill = create2DQuad(text->getBoundingBox(), dbSpace, m_vecBackgrandColor);


		if (_boxfill)
		{	
			_boxfill->setCullingActive(false);
			_geode->addChild(_boxfill);
		}

		osg::Vec4 outLineColor;
		if (m_vecOutLineColor != osg::Vec4d(0.0f, 0.0f, 0.0f, 0.0f))
		{
			outLineColor = m_vecOutLineColor;
		}
		else
		{
			outLineColor = osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
			m_vecOutLineColor = outLineColor;
		}

		_boxOutLine = create2DOutline(text->getBoundingBox(), dbSpace - 0.1f, outLineColor);
		_boxOutLine->setCullingActive(false);
		if (_boxOutLine)
		{
			_geode->addChild(_boxOutLine);
		}
	}

	_geode->setCullingActive(false);
	//_geode->getOrCreateStateSet()->setAttributeAndModes(new osg::Program, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	_geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	if (text)
	{
		_geode->addChild(text);
	}

	//osg::StateSet* stateSet = _geode->getOrCreateStateSet();

	//设置透明渲染元
	//stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	//设置渲染级别,防止开启alpha融合后会被其他节点遮住
	//stateSet->setRenderBinDetails(11, "RenderBin");
	//stateSet->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false), 1);

	getPositionAttitudeTransform()->addChild(_geode);

	applyStyle( _style );

	setPriority(getPriority());

	if (_dynamic)
		setDynamic(_dynamic);

	updateLayoutData();
}


void GeoGlobe::PlaceName::GeoBillboard::onAnimationStop(GLOBE_NAME::ActionState state)
{
	if (state == GLOBE_NAME::Stop)
	{
		setDrawableColor(_boxfill, m_vecBackgrandColor);
		setDrawableColor(_lineGeometry, m_vecLineColor);
		setDrawableColor(_boxOutLine, m_vecOutLineColor);

		if (m_colorUniform)
			m_colorUniform->set(osg::Vec4(1.0, 1.0, 1.0, 1.0));

		if (_ptrText)
		{
			_ptrText->setColor(m_textColor);
			_ptrText->setBackdropColor(m_shadowColor);
		}

		if (_imageGeom)
		{
			osg::StateSet* ss = _imageGeom->getOrCreateStateSet();
			if (ss)
				ss->setAttribute(_ptrRTTProgram);
		}
	}
}

void GeoGlobe::PlaceName::GeoBillboard::CancelHightLight()
{
	_geode->setStateSet(_stateSet);
}

void GeoGlobe::PlaceName::GeoBillboard::HightLight()
{
	//osg::StateSet* ptrStateSet = _imageGeom->getOrCreateStateSet();

	////记录原来的状态
	//_stateSet = dynamic_cast<osg::StateSet*> (ptrStateSet->clone(osg::CopyOp::DEEP_COPY_ALL));

	////关闭灯光
	//ptrStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	////打开混合融合模式
	//ptrStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	//ptrStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	//ptrStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	//osg::BlendColor* bc = new osg::BlendColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.5));
	////BlendFunc：定义一种混合方式
	//osg::BlendFunc*bf = new osg::BlendFunc();
	////属性设置，开关打开
	//ptrStateSet->setAttributeAndModes(bf, osg::StateAttribute::ON);
	//ptrStateSet->setAttributeAndModes(bc, osg::StateAttribute::ON);
	//bf->setSource(osg::BlendFunc::CONSTANT_ALPHA);
	//bf->setDestination(osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
	////bc->setConstantColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.5));
}

void GeoGlobe::PlaceName::GeoBillboard::setShadowColor(const osg::Vec4& color)
{
	if (_ptrText)
	{
		_ptrText->setBackdropColor(color);
		m_shadowColor = color;
	}
}

osg::Vec4 GeoGlobe::PlaceName::GeoBillboard::getShadowColor()
{
	if (_ptrText)
		return _ptrText->getBackdropColor();

	return osg::Vec4(1.0, 0.0, 0.0, 1.0);
}

void GeoGlobe::PlaceName::GeoBillboard::setShadowOffset(const double& dbShadowOffset)
{
	if (_ptrText)
	{
		_ptrText->setBackdropOffset(dbShadowOffset);
		m_dbShadowOffset = dbShadowOffset;
	}
}
double GeoGlobe::PlaceName::GeoBillboard::getShadowOffset()
{
	return m_dbShadowOffset;
}

void GeoGlobe::PlaceName::GeoBillboard::setShadowPos(const osgText::Text::BackdropType& eShadowPos)
{
	if (_ptrText)
	{
		_ptrText->setBackdropType(eShadowPos);
		m_eShadowPos = eShadowPos;
	}
}

osgText::Text::BackdropType GeoGlobe::PlaceName::GeoBillboard::getShadowPos()
{
	if (_ptrText)
		return _ptrText->getBackdropType();

	return m_eShadowPos;
}

void  GeoGlobe::PlaceName::GeoBillboard::setDrawableColor(osg::Drawable* ptrDraw, const osg::Vec4& color)
{
	if (ptrDraw)
	{
		osg::Vec4Array* c = dynamic_cast<osg::Vec4Array*>(ptrDraw->asGeometry()->getColorArray());
		if (c)
			(*c)[0] = color;
	}
}

osg::Vec4  GeoGlobe::PlaceName::GeoBillboard::getDrawableColor(osg::Drawable* ptrDraw)
{
	if (ptrDraw)
	{
		osg::Vec4Array* c = dynamic_cast<osg::Vec4Array*>(ptrDraw->asGeometry()->getColorArray());
		if (c)
			return (*c)[0];
	}

	return osg::Vec4(1.0, 0.0, 0.0, 1.0);
}

void  GeoGlobe::PlaceName::GeoBillboard::setOutLineColor(const osg::Vec4& color)
{
	setDrawableColor(_boxOutLine, color);
	m_vecOutLineColor = color;
}

osg::Vec4  GeoGlobe::PlaceName::GeoBillboard::getOutLineColor()
{
	return getDrawableColor(_boxOutLine);
}

void  GeoGlobe::PlaceName::GeoBillboard::setLineColor(const osg::Vec4& color)
{
	setDrawableColor(_lineGeometry, color);
	m_vecLineColor = color;
}

osg::Vec4  GeoGlobe::PlaceName::GeoBillboard::getLineColor()
{
	return getDrawableColor(_lineGeometry);
}

void GeoGlobe::PlaceName::GeoBillboard::setTextColor(const osg::Vec4& color)
{
	if (_ptrText)
	{
		//_ptrText->setColor(osg::Vec4(1.0, 0.0, 0.0, 1.0));
		_ptrText->setColor(color);
		_ptrText->dirtyDisplayList();
		m_textColor = color;
	}
}

osg::Vec4 GeoGlobe::PlaceName::GeoBillboard::getTextColor()
{
	return m_textColor;
}

void GeoGlobe::PlaceName::GeoBillboard::setBackGrandColor(const osg::Vec4& color)
{
	setDrawableColor(_boxfill, color);
	m_vecBackgrandColor = color;
}

osg::Vec4 GeoGlobe::PlaceName::GeoBillboard::getBackGrandColor()
{
	return getDrawableColor(_boxfill);
}

void GeoGlobe::PlaceName::GeoBillboard::setPolylineAngle(const double& dbAngle)
{
	m_PolylineAngle = osg::DegreesToRadians(dbAngle);
	init();
}

double GeoGlobe::PlaceName::GeoBillboard::getPolylineAngel()
{
	return osg::RadiansToDegrees( m_PolylineAngle);
}

void GeoGlobe::PlaceName::GeoBillboard::setBottomLen(const double& dbBottomLen)
{
	m_dbBottomLen = dbBottomLen;
	init();
}

double GeoGlobe::PlaceName::GeoBillboard::getBottomLen()
{
	return m_dbBottomLen;
}

void GeoGlobe::PlaceName::GeoBillboard::setPolylineLen(const double& dbPolylineLen)
{
	m_dbPolylineLen = dbPolylineLen;
	init();
}

double GeoGlobe::PlaceName::GeoBillboard::getPolylineLen()
{
	return m_dbPolylineLen;
}

void GeoGlobe::PlaceName::GeoBillboard::setAnimatorAction(GLOBE_NAME::AnimatorAction * animatorAction)
{
	if (animatorAction)
	{
		m_ptrAnimatorAction = animatorAction;
		m_ptrAnimatorAction->OnAnimatorEvent.Add(this, &GeoGlobe::PlaceName::GeoBillboard::onAnimationStop);

		if (_program == NULL && _imageGeom != NULL)
		{
			//这个Shader名字不能一样，不然会共用而应用到所有的GeoBillboard
			static int nIndex = 0;
			std::string strName = "GeobillboardShader" + std::to_string(nIndex++);

			//创建shader
			osg::StateSet* ss = _imageGeom->getOrCreateStateSet();
			_program = new osg::Program;
			_program->setName(strName);
			_program->addShader(new osg::Shader(osg::Shader::VERTEX, billboardVertSource));
			_program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fBilboardShaderStr));

			//记录之前的着色器
			_ptrRTTProgram = (osgEarth::VirtualProgram*)ss->getAttribute(osg::StateAttribute::PROGRAM);

			//设置着色器程序
			ss->setAttributeAndModes(_program, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
			// 开启着色器透明
			ss->setMode(GL_BLEND, osg::StateAttribute::ON);
			ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
			ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
			ss->setAttribute(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA), osg::StateAttribute::ON);
			//ss->setRenderBinDetails(12, "RenderBin");//设置渲染顺序
			m_colorUniform = new osg::Uniform("geoBill_color", osg::Vec4(1, 1, 1, 1));
			ss->addUniform(m_colorUniform);
			//
		}
		else if (_program != NULL&&_imageGeom != NULL)
		{
			osg::StateSet* ss = _imageGeom->getOrCreateStateSet();
			ss->setAttribute(_program);
		}
	}
}

GLOBE_NAME::AnimatorAction * GeoGlobe::PlaceName::GeoBillboard::getAnimatorAction()
{
	return m_ptrAnimatorAction;
}

void GeoGlobe::PlaceName::GeoBillboard::traverse(osg::NodeVisitor& nv)
{
	if (nv.getTraversalMode() == osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
	{
 		if (_bVisible)
		{
			GLOBE_NAME::PointAnimatorAction* pointaciton = dynamic_cast<GLOBE_NAME::PointAnimatorAction*>(m_ptrAnimatorAction.p);
			if (pointaciton&&pointaciton->isStart())
			{
				osgViewer::View* ptrView = GLOBE_NAME::OsgCoreSingleton::Instance()->OsgViewer();
				double time = ptrView->getFrameStamp()->getSimulationTime() * 1000;
				float a = pointaciton->Update(time);

				// 透明动画
				if (pointaciton->getAphla() && pointaciton->isStart())
				{
					// 直接更改颜色值的透明值
					float aphla = pointaciton->getAphla()->GetValue(a);
					osg::Vec4 tempBackgrandColor = m_vecBackgrandColor;
					osg::Vec4 tempLineColor = m_vecLineColor;
					osg::Vec4 tempOutLineColor = m_vecOutLineColor;
					osg::Vec4 tempTextColor = m_textColor;
					osg::Vec4 tempShadowColor = m_shadowColor;

					if (m_colorUniform)
						m_colorUniform->set(osg::Vec4(1.0,1.0,1.0, aphla));
					tempBackgrandColor.a() = aphla;
					tempLineColor.a() = aphla;
					tempOutLineColor.a() = aphla;
					tempTextColor.a() = aphla;
					setDrawableColor(_boxfill, tempBackgrandColor);
					setDrawableColor(_lineGeometry, tempLineColor);
					setDrawableColor(_boxOutLine, tempOutLineColor);

					if (_ptrText)
					{
						osg::Vec4 textColor = _ptrText->getColor();
						osg::Vec4 shadowColor = _ptrText->getBackdropColor();
						textColor.a() = aphla;
						//闪的时候强制将阴影alpha设为0，不然阴影色一起闪效果不好
						shadowColor.a() = 0;
						_ptrText->setColor(textColor);
						_ptrText->setBackdropColor(shadowColor);
					}
				}
			}

			Group::traverse(nv);
		}
	}
	else
	{
		Group::traverse(nv);
	}
}

void
GeoGlobe::PlaceName::GeoBillboard::dirty()
{
	GeoPositionNode::dirty();
	updateLayoutData();
}

void
GeoGlobe::PlaceName::GeoBillboard::setPriority(float value)
{
	GeoPositionNode::setPriority(value);
	updateLayoutData();
}

void
GeoGlobe::PlaceName::GeoBillboard::updateLayoutData()
{
	if (!_dataLayout.valid())
	{
		_dataLayout = new ScreenSpaceLayoutData();
	}

	// re-apply annotation drawable-level stuff as neccesary.
	for (unsigned i = 0; i < _geode->getNumChildren(); ++i)
	{
		_geode->getChild(i)->setUserData(_dataLayout.get());
	}

	_dataLayout->setPriority(getPriority());

	GeoPoint location = getPosition();
	location.makeGeographic();
	double latRad;
	double longRad;
	GeoMath::destination(osg::DegreesToRadians(location.y()),
		osg::DegreesToRadians(location.x()),
		_labelRotationRad,
		2500.,
		latRad,
		longRad);

	_geoPointProj.set(osgEarth::SpatialReference::get("wgs84"),
		osg::RadiansToDegrees(longRad),
		osg::RadiansToDegrees(latRad),
		0,
		osgEarth::ALTMODE_ABSOLUTE);

	_geoPointLoc.set(osgEarth::SpatialReference::get("wgs84"),
		//location.getSRS(),
		location.x(),
		location.y(),
		0,
		osgEarth::ALTMODE_ABSOLUTE);

	const TextSymbol* ts = getStyle().get<TextSymbol>();
	if (ts)
	{
		_dataLayout->setPixelOffset(ts->pixelOffset().get());

		if (_followFixedCourse)
		{
			osg::Vec3d p0, p1;
			_geoPointLoc.toWorld(p0);
			_geoPointProj.toWorld(p1);
			_dataLayout->setAnchorPoint(p0);
			_dataLayout->setProjPoint(p1);
		}
	}
}

void
GeoGlobe::PlaceName::GeoBillboard::setText( const std::string& text )
{
    if ( !_dynamic )
    {
        OE_WARN << LC << "Illegal state: cannot change a LabelNode that is not dynamic" << std::endl;
        return;
    }

    _text = text;

	if (_image)
	{
		for (unsigned i = 0; i < _geode->getNumChildren(); ++i)
		{
			osgText::Text* d = dynamic_cast<osgText::Text*>(_geode->getChild(i));
			if (d)
			{
				TextSymbol* symbol = _style.getOrCreate<TextSymbol>();
				osgText::String::Encoding text_encoding = osgText::String::ENCODING_UNDEFINED;
				if (symbol && symbol->encoding().isSet())
				{
					text_encoding = AnnotationUtils::convertTextSymbolEncoding(symbol->encoding().value());
				}

				d->setText(text, text_encoding);
				break;
			}
		}
	}
	else
	{
		init();
	}
}

void GeoGlobe::PlaceName::GeoBillboard::setIconImage(osg::Image* image)
{
	// changing the icon requires a complete rebuild.
	_image = image;
	init();
}

void GeoGlobe::PlaceName::GeoBillboard::setVisible(const bool& bVisible)
{
	_bVisible = bVisible;
}

void
GeoGlobe::PlaceName::GeoBillboard::setStyle(const Style& style)
{
    // changing the style requires a complete rebuild.
    _style = style;
    init();
}


void
GeoGlobe::PlaceName::GeoBillboard::setAnnotationData( AnnotationData* data )
{
	//GeoPositionNode::setAnnotationData( data );
	
    // override this method so we can attach the anno data to the drawables.
    for(unsigned i=0; i<_geode->getNumChildren(); ++i)
    {
        _geode->getChild(i)->setUserData( data );
    }
}


void
GeoGlobe::PlaceName::GeoBillboard::setDynamic( bool value )
{
    GeoPositionNode::setDynamic( value );
    
    for(unsigned i=0; i<_geode->getNumChildren(); ++i)
    {
        _geode->getChild(i)->setDataVariance( 
            value ? osg::Object::DYNAMIC : osg::Object::STATIC );
    }
}

void GeoGlobe::PlaceName::GeoBillboard::setPosition( const GeoPoint& pos )
{
	//_SphereElement->setPosition(pos);
	GeoPositionNode::setPosition(pos);
}
