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

#include <placename/PlaceNode.h>
#include <osgEarthAnnotation/AnnotationUtils>
#include <osgEarthAnnotation/AnnotationRegistry>
#include <osgEarthAnnotation/BboxDrawable>
#include <osgEarthFeatures/BuildTextFilter>
#include <osgEarthFeatures/LabelSource>
#include <osgEarth/Utils>
#include <osgEarth/Registry>
#include <osgEarth/ShaderGenerator>
#include <osgEarth/GeoMath>
#include <osgEarth/ScreenSpaceLayout>
#include <osgEarth/Lighting>
#include <osg/Depth>
#include <osgText/Text>
#include <OsgCoreSingleton.h>
#include <osgViewer/viewer>
#include <osg/BlendFunc>
#include <osg/ImageStream>

#define LC "[PlaceNode] "

using namespace osgEarth;
using namespace osgEarth::Annotation;
using namespace GeoGlobe::Annotation;
using namespace osgEarth::Features;
using namespace osgEarth::Symbology;

static const char *placenameVertSource = {
	"// microshader - colors a fragment based on its position\n"
	"varying vec2 m_coord;\n"
	"void main(void)\n"
	"{\n"
	"   m_coord= gl_MultiTexCoord0.xy;\n"
	"    gl_Position =   gl_ModelViewProjectionMatrix *gl_Vertex;\n"
	"}\n"
};

static const char * fPlaceNameShaderStr = {
	//"precision highp float; \n"
	"uniform sampler2D sampler2d; \n"
	"uniform  vec4 place_color;  \n"
	"varying  vec2 m_coord; \n"
	"void main() \n"
	"{ \n"
	"   vec4 out_color = texture2D(sampler2d,m_coord) ;  \n"
	"  gl_FragColor =vec4(1.0,1.0,1.0,place_color.a)*out_color;\n"
	"} \n"
};

namespace
{
	const char* iconVS =
		"#version " GLSL_VERSION_STR "\n"
		"out vec2 oe_PlaceNode_texcoord; \n"
		"void oe_PlaceNode_icon_VS(inout vec4 vertex) \n"
		"{ \n"
		"    oe_PlaceNode_texcoord = gl_MultiTexCoord0.st; \n"
		"} \n";

	const char* iconFS =
		"#version " GLSL_VERSION_STR "\n"
		"in vec2 oe_PlaceNode_texcoord; \n"
		"uniform sampler2D oe_PlaceNode_tex; \n"
		"void oe_PlaceNode_icon_FS(inout vec4 color) \n"
		"{ \n"
		"    color = texture(oe_PlaceNode_tex, oe_PlaceNode_texcoord); \n"
		"} \n";
}

osg::observer_ptr<osg::StateSet> GeoGlobe::Annotation::PlaceNode::s_geodeStateSet;
osg::observer_ptr<osg::StateSet> GeoGlobe::Annotation::PlaceNode::s_imageStateSet;

GeoGlobe::Annotation::PlaceNode::PlaceNode(
	const GeoPoint&    position,
	osg::Image*        image,
	const std::string& text,
	const Style&       style) :

	GeoPositionNode(),
	_image(image),
	_text(text),
	_style(style),
	_geode(0L),
	_labelRotationRad(0.),
	_followFixedCourse(false)
{	
	setPosition(position);
	m_textColor = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_shadowColor = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_dbShadowOffset = 0.0f;
	m_eShadowPos = (osgText::Text::BackdropType)(-100);
	_imageGeom = NULL;
	_program = NULL;
	_ptrRTTProgram = NULL;
	construct();
	init();
}

GeoGlobe::Annotation::PlaceNode::PlaceNode(
	const GeoPoint&    position,
	const std::string& text,
	const Style&       style) :

	GeoPositionNode(),
	_text(text),
	_style(style),
	_geode(0L),
	_labelRotationRad(0.),
	_followFixedCourse(false)
{
	setPosition(position);
	m_textColor = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_shadowColor = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_dbShadowOffset = 0.0f;
	m_eShadowPos = (osgText::Text::BackdropType)(-100);
	_imageGeom = NULL;
	_program = NULL;
	_ptrRTTProgram = NULL;
	construct();
	init();
}

GeoGlobe::Annotation::PlaceNode::PlaceNode(
	const GeoPoint&       position,
	const Style&          style,
	const osgDB::Options* dbOptions) :
	GeoPositionNode(),
	_style(style),
	_dbOptions(dbOptions),
	_labelRotationRad(0.),
	_followFixedCourse(false)
{
	setPosition(position);
	m_textColor = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_shadowColor = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
	m_dbShadowOffset = 0.0f;
	m_eShadowPos = (osgText::Text::BackdropType)(-100);
	_ptrText = NULL;
	_imageGeom = NULL;
	_program = NULL;
	_ptrRTTProgram = NULL;
	construct();
	init();
}

void GeoGlobe::Annotation::PlaceNode::construct()
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
			vp->setName("PlaceNode::imageStateSet");
			vp->setFunction("oe_PlaceNode_icon_VS", iconVS, ShaderComp::LOCATION_VERTEX_MODEL);
			vp->setFunction("oe_PlaceNode_icon_FS", iconFS, ShaderComp::LOCATION_FRAGMENT_COLORING);
			_imageStateSet->addUniform(new osg::Uniform("oe_PlaceNode_tex", 0));
		}
	}
}

void GeoGlobe::Annotation::PlaceNode::init()
{
	osg::Group* pat = getPositionAttitudeTransform();
	pat->removeChildren(0, pat->getNumChildren());

	_geode = new osg::Group();
	_geode->setCullingActive(false);
	_geode->setStateSet(_geodeStateSet.get());

	// ensure that (0,0,0) is the bounding sphere control/center point.
	// useful for things like horizon culling.
	_geode->setComputeBoundingSphereCallback(new ControlPointCallback());

	osg::Drawable* text = 0L;

	const TextSymbol* symbol = _style.get<TextSymbol>();

	// If there's no explicit text, look to the text symbol for content.
	if (_text.empty() && symbol)
	{
		_text = symbol->content()->eval();
	}

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

	std::string strIcon = "";
	const IconSymbol* icon = NULL;
	if (instance)
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

		if (icon)
		{
			if (icon->url().isSet())
			{
				imageURI = icon->url()->evalURI();
				_iconUrl = icon->url()->eval();
			}
			else if (icon->getImage())
			{
				_image = icon->getImage();
			}
		}

		if (!imageURI.empty())
		{
			_image = imageURI.getImage(_dbOptions.get());
			osg::ImageStream* imagestream = dynamic_cast<osg::ImageStream*>(_image.get());
			if (imagestream)
			{
				imagestream->play();
			}
		}
	}

	osg::BoundingBox imageBox(0, 0, 0, 0, 0, 0);

	// found an image; now format it:
	if (_image.get())
	{
		// Scale the icon if necessary
		double scale = 1.0;
		if (icon && icon->scale().isSet())
		{
			scale = icon->scale()->eval();
		}

		double s = scale * _image->s();
		double t = scale * _image->t();

		// this offset anchors the image at the bottom
		osg::Vec2s offset;
		if (!icon || !icon->alignment().isSet())
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
		if (icon && icon->heading().isSet())
		{
			heading = osg::DegreesToRadians(icon->heading()->eval());
		}

		//We must actually rotate the geometry itself and not use a MatrixTransform b/c the 
		//decluttering doesn't respect Transforms above the drawable.
		//if (_imageGeom == NULL)
		{
			_imageGeom = AnnotationUtils::createImageGeometry(_image.get(), offset, 0, heading, scale);

			if (_imageGeom)
			{
				_geode->addChild(_imageGeom);
				_imageGeom->getOrCreateStateSet()->merge(*_imageStateSet.get());
				imageBox = _imageGeom->getBoundingBox();
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
		//	_imageGeom->dirtyBound();

		//	_geode->addChild(_imageGeom);
		//	imageBox = _imageGeom->getBoundingBox();
		//	_imageGeom->setCullingActive(false);

		//	if (_bIsIconChange)
		//	{
		//		osg::StateSet* ptrState = _imageGeom->getOrCreateStateSet();
		//		osg::Texture2D* texture = dynamic_cast<osg::Texture2D*>(ptrState->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
		//		texture->setImage(_image);
		//	}
		//}
	}

	TextSymbol* textSymbol = NULL;

	if (_image.valid())
	{
		textSymbol = _style.getOrCreate<TextSymbol>();
		if (!textSymbol->alignment().isSet())
			textSymbol->alignment() = textSymbol->ALIGN_LEFT_CENTER;
	}

	if (textSymbol)
		m_textColor = textSymbol->fill()->color();

	text = AnnotationUtils::createTextDrawable(
		_text,
		_style.get<TextSymbol>(),
		imageBox);

	const BBoxSymbol* bboxsymbol = _style.get<BBoxSymbol>();
	if (bboxsymbol && text)
	{
		osg::Drawable* bboxGeom = new BboxDrawable(text->getBoundingBox(), *bboxsymbol);
		_geode->addChild(bboxGeom);
	}

	if (text)
	{
		_geode->addChild(text);
		_ptrText = dynamic_cast<osgText::Text*>(text);

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
	}
	_geode->setCullingActive(false);
	//_geode->getOrCreateStateSet()->setAttributeAndModes(new osg::Program, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
	_geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	getPositionAttitudeTransform()->addChild(_geode);

	// for clamping and occlusion culling    
	//OE_WARN << LC << "PlaceNode::applyStyle: " << _style.getConfig().toJSON(true) << std::endl;
	setDefaultLighting(false);
	applyStyle(_style);

	//setLightingIfNotSet(false);

	//// generate shaders:
	//Registry::shaderGenerator().run(
	//	this,
	//	"Globe.PlaceNode",
	//	Registry::stateSetCache());

	setPriority(getPriority());

	if (_dynamic)
		setDynamic(_dynamic);

	updateLayoutData();
}

void GeoGlobe::Annotation::PlaceNode::setShadowColor(const osg::Vec4& color)
{
	if (_ptrText)
	{
		_ptrText->setBackdropColor(color);
		m_shadowColor = color;
	}
}

const osg::Vec4& GeoGlobe::Annotation::PlaceNode::getShadowColor()
{
	if (_ptrText)
		return _ptrText->getBackdropColor();
}

void GeoGlobe::Annotation::PlaceNode::setShadowOffset(const double& dbShadowOffset)
{
	if (_ptrText)
	{
		_ptrText->setBackdropOffset(dbShadowOffset);
		m_dbShadowOffset = dbShadowOffset;
	}
}
const double& GeoGlobe::Annotation::PlaceNode::getShadowOffset()
{
	return m_dbShadowOffset;
}

void GeoGlobe::Annotation::PlaceNode::setShadowPos(const osgText::Text::BackdropType& eShadowPos)
{
	if (_ptrText)
	{
		_ptrText->setBackdropType(eShadowPos);
		m_eShadowPos = eShadowPos;
	}
}

const osgText::Text::BackdropType& GeoGlobe::Annotation::PlaceNode::getShadowPos()
{
	if (_ptrText)
		return _ptrText->getBackdropType();
}

void GeoGlobe::Annotation::PlaceNode::onAnimationStop(GLOBE_NAME::ActionState state)
{
	if (state == GLOBE_NAME::Stop)
	{
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

void GeoGlobe::Annotation::PlaceNode::setAnimatorAction(GLOBE_NAME::AnimatorAction * animatorAction)
{
	m_ptrAnimatorAction = animatorAction;
	m_ptrAnimatorAction->OnAnimatorEvent.Add(this, &PlaceNode::onAnimationStop);

	if (_program == NULL && _imageGeom != NULL)
	{
		//这个Shader名字不能一样，不然会共用而应用到所有的placename
		static int nIndex = 0;
		std::string strName = "PlaceNameShader" + std::to_string(nIndex++);

		//创建shader
		osg::StateSet* ss = _imageGeom->getOrCreateStateSet();
		_program = new osg::Program;
		_program->setName(strName);
		_program->addShader(new osg::Shader(osg::Shader::VERTEX, placenameVertSource));
		_program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fPlaceNameShaderStr));

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
		m_colorUniform = new osg::Uniform("place_color", osg::Vec4(1, 1, 1, 1));
		ss->addUniform(m_colorUniform);
		//
	}
	else if (_program != NULL&&_imageGeom != NULL)
	{
		osg::StateSet* ss = _imageGeom->getOrCreateStateSet();
		if (ss)
			ss->setAttribute(_program);
	}
}

GLOBE_NAME::AnimatorAction * GeoGlobe::Annotation::PlaceNode::getAnimatorAction()
{
	return m_ptrAnimatorAction;
}


void GeoGlobe::Annotation::PlaceNode::traverse(osg::NodeVisitor& nv)
{
	if (nv.getTraversalMode() == osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
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

				if (m_colorUniform)
					m_colorUniform->set(osg::Vec4(1.0, 1.0, 1.0, aphla));

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
	else
	{
		Group::traverse(nv);
	}
}


void GeoGlobe::Annotation::PlaceNode::dirty()
{
	GeoPositionNode::dirty();
	updateLayoutData();
}

void GeoGlobe::Annotation::PlaceNode::setPriority(float value)
{
	GeoPositionNode::setPriority(value);
	updateLayoutData();
}

void GeoGlobe::Annotation::PlaceNode::updateLayoutData()
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

void GeoGlobe::Annotation::PlaceNode::setText(const std::string& text)
{
	if (!_dynamic)
	{
		OE_WARN << LC << "Illegal state: cannot change a LabelNode that is not dynamic" << std::endl;
		return;
	}

	_text = text;

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


void GeoGlobe::Annotation::PlaceNode::setStyle(const Style& style)
{
	// changing the style requires a complete rebuild.
	_style = style;
	init();
}


void GeoGlobe::Annotation::PlaceNode::setIconImage(osg::Image* image)
{
	// changing the icon requires a complete rebuild.
	_image = image;
	init();
}


void GeoGlobe::Annotation::PlaceNode::setDynamic(bool value)
{
	GeoPositionNode::setDynamic(value);

	for (unsigned i = 0; i < _geode->getNumChildren(); ++i)
	{
		_geode->getChild(i)->setDataVariance(
			value ? osg::Object::DYNAMIC : osg::Object::STATIC);
	}
}

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
//
//#include <placename/PlaceNode.h>
//#include <osgEarthAnnotation/AnnotationUtils>
//#include <osgEarthAnnotation/AnnotationRegistry>
//#include <osgEarthAnnotation/BboxDrawable>
//#include <osgEarth/Utils>
//#include <osgEarth/GeoMath>
//#include <osgEarth/ScreenSpaceLayout>
//#include <osgEarth/Lighting>
//#include <osgEarth/Shaders>
//#include <osgEarth/VirtualProgram>
//#include <osgEarth/ShaderGenerator>
//
//#include <osg/Depth>
//#include <osgText/Text>
//
//#define LC "[PlaceNode] "
//
//using namespace osgEarth;
//using namespace GeoGlobe::Annotation;
//using namespace osgEarth::Symbology;
//
//namespace
//{
//	const char* iconVS =
//		"#version " GLSL_VERSION_STR "\n"
//		"out vec2 oe_PlaceNode_texcoord; \n"
//		"void oe_PlaceNode_icon_VS(inout vec4 vertex) \n"
//		"{ \n"
//		"    oe_PlaceNode_texcoord = gl_MultiTexCoord0.st; \n"
//		"} \n";
//
//	const char* iconFS =
//		"#version " GLSL_VERSION_STR "\n"
//		"in vec2 oe_PlaceNode_texcoord; \n"
//		"uniform sampler2D oe_PlaceNode_tex; \n"
//		"void oe_PlaceNode_icon_FS(inout vec4 color) \n"
//		"{ \n"
//		"    color = texture(oe_PlaceNode_tex, oe_PlaceNode_texcoord); \n"
//		"} \n";
//}
//
//osg::observer_ptr<osg::StateSet> PlaceNode::s_geodeStateSet;
//osg::observer_ptr<osg::StateSet> PlaceNode::s_imageStateSet;
//
//PlaceNode::PlaceNode() :
//	GeoPositionNode()
//{
//	construct();
//}
//
//PlaceNode::PlaceNode(const std::string& text,
//	const Style& style,
//	osg::Image* image) :
//	GeoPositionNode()
//{
//	construct();
//
//	_text = text;
//	_image = image;
//	_style = style;
//
//	m_textColor = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
//	m_shadowColor = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
//	m_dbShadowOffset = 0.0f;
//	m_eShadowPos = (osgText::Text::BackdropType)(-100);
//
//	compile();
//}
//
//PlaceNode::PlaceNode(const GeoPoint& position,
//	const std::string& text,
//	const Style& style,
//	osg::Image* image) :
//	GeoPositionNode()
//{
//	construct();
//
//	_text = text;
//	_image = image;
//	_style = style;
//
//	m_textColor = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
//	m_shadowColor = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
//	m_dbShadowOffset = 0.0f;
//	m_eShadowPos = (osgText::Text::BackdropType)(-100);
//
//	setPosition(position);
//
//	compile();
//}
//
//void
//PlaceNode::construct()
//{
//	_geode = 0L;
//	_labelRotationRad = 0.0f;
//	_followFixedCourse = false;
//	_imageDrawable = 0L;
//	_bboxDrawable = 0L;
//	_textDrawable = 0L;
//
//	// This class makes its own shaders
//	ShaderGenerator::setIgnoreHint(this, true);
//
//	// Construct the shared state sets
//	if (s_geodeStateSet.lock(_geodeStateSet) == false)
//	{
//		static Threading::Mutex s_mutex;
//		Threading::ScopedMutexLock lock(s_mutex);
//
//		if (s_geodeStateSet.lock(_geodeStateSet) == false)
//		{
//			s_geodeStateSet = _geodeStateSet = new osg::StateSet();
//
//			// draw in the screen-space bin
//			ScreenSpaceLayout::activate(_geodeStateSet.get());
//
//			// completely disable depth buffer
//			_geodeStateSet->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false), 1);
//
//			// Disable lighting for place nodes by default
//			_geodeStateSet->setDefine(OE_LIGHTING_DEFINE, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
//		}
//	}
//
//	if (s_imageStateSet.lock(_imageStateSet) == false)
//	{
//		static Threading::Mutex s_mutex;
//		Threading::ScopedMutexLock lock(s_mutex);
//
//		if (s_imageStateSet.lock(_imageStateSet) == false)
//		{
//			s_imageStateSet = _imageStateSet = new osg::StateSet();
//			VirtualProgram* vp = VirtualProgram::getOrCreate(_imageStateSet.get());
//			vp->setName("PlaceNode::imageStateSet");
//			vp->setFunction("oe_PlaceNode_icon_VS", iconVS, ShaderComp::LOCATION_VERTEX_MODEL);
//			vp->setFunction("oe_PlaceNode_icon_FS", iconFS, ShaderComp::LOCATION_FRAGMENT_COLORING);
//			_imageStateSet->addUniform(new osg::Uniform("oe_PlaceNode_tex", 0));
//		}
//	}
//}
//
//void
//PlaceNode::compile()
//{
//	osg::Group* pat = getPositionAttitudeTransform();
//	pat->removeChildren(0, pat->getNumChildren());
//
//	_geode = new osg::Group();
//	_geode->setCullingActive(false);
//	_geode->setStateSet(_geodeStateSet.get());
//
//	// ensure that (0,0,0) is the bounding sphere control/center point.
//	// useful for things like horizon culling.
//	_geode->setComputeBoundingSphereCallback(new ControlPointCallback());
//
//	getPositionAttitudeTransform()->addChild(_geode);
//
//	_imageDrawable = 0L;
//	_bboxDrawable = 0L;
//	_textDrawable = 0L;
//
//	const TextSymbol* symbol = _style.get<TextSymbol>();
//
//	// If there's no explicit text, look to the text symbol for content.
//	if (_text.empty() && symbol)
//	{
//		_text = symbol->content()->eval();
//	}
//
//	// Handle the rotation if any
//	if (symbol && symbol->onScreenRotation().isSet())
//	{
//		_labelRotationRad = osg::DegreesToRadians(symbol->onScreenRotation()->eval());
//	}
//
//	// In case of a label must follow a course on map, we project a point from the position
//	// with the given bearing. Then during culling phase we compute both points on the screen
//	// and then we can deduce the screen rotation
//	// may be optimized...
//	else if (symbol && symbol->geographicCourse().isSet())
//	{
//		_followFixedCourse = true;
//		_labelRotationRad = osg::DegreesToRadians(symbol->geographicCourse()->eval());
//	}
//
//	osg::ref_ptr<const InstanceSymbol> instance = _style.get<InstanceSymbol>();
//
//	const IconSymbol* icon = 0;
//	if (instance.valid())
//	{
//		icon = instance->asIcon();
//	}
//
//	if (!_image.valid())
//	{
//		URI imageURI;
//
//		if (icon)
//		{
//			if (icon->url().isSet())
//			{
//				imageURI = icon->url()->evalURI();
//			}
//			else if (icon->getImage())
//			{
//				_image = icon->getImage();
//			}
//		}
//
//		if (!imageURI.empty())
//		{
//			_image = imageURI.getImage(_readOptions.get());
//		}
//	}
//
//	osg::BoundingBox imageBox(0, 0, 0, 0, 0, 0);
//
//	// found an image; now format it:
//	if (_image.get())
//	{
//		// Scale the icon if necessary
//		double scale = 1.0;
//		if (icon && icon->scale().isSet())
//		{
//			scale = icon->scale()->eval();
//		}
//
//		double s = scale * _image->s();
//		double t = scale * _image->t();
//
//		// this offset anchors the image at the bottom
//		osg::Vec2s offset;
//		if (!icon || !icon->alignment().isSet())
//		{
//			// default to bottom center
//			offset.set(0.0, t / 2.0);
//		}
//		else
//		{	// default to bottom center
//			switch (icon->alignment().value())
//			{
//			case IconSymbol::ALIGN_LEFT_TOP:
//				offset.set((s / 2.0), -(t / 2.0));
//				break;
//			case IconSymbol::ALIGN_LEFT_CENTER:
//				offset.set((s / 2.0), 0.0);
//				break;
//			case IconSymbol::ALIGN_LEFT_BOTTOM:
//				offset.set((s / 2.0), (t / 2.0));
//				break;
//			case IconSymbol::ALIGN_CENTER_TOP:
//				offset.set(0.0, -(t / 2.0));
//				break;
//			case IconSymbol::ALIGN_CENTER_CENTER:
//				offset.set(0.0, 0.0);
//				break;
//			case IconSymbol::ALIGN_CENTER_BOTTOM:
//			default:
//				offset.set(0.0, (t / 2.0));
//				break;
//			case IconSymbol::ALIGN_RIGHT_TOP:
//				offset.set(-(s / 2.0), -(t / 2.0));
//				break;
//			case IconSymbol::ALIGN_RIGHT_CENTER:
//				offset.set(-(s / 2.0), 0.0);
//				break;
//			case IconSymbol::ALIGN_RIGHT_BOTTOM:
//				offset.set(-(s / 2.0), (t / 2.0));
//				break;
//			}
//		}
//
//		// Apply a rotation to the marker if requested:
//		double heading = 0.0;
//		if (icon && icon->heading().isSet())
//		{
//			heading = osg::DegreesToRadians(icon->heading()->eval());
//		}
//
//		//We must actually rotate the geometry itself and not use a MatrixTransform b/c the 
//		//decluttering doesn't respect Transforms above the drawable.        
//		_imageDrawable = AnnotationUtils::createImageGeometry(_image.get(), offset, 0, heading, scale);
//		if (_imageDrawable)
//		{
//			// todo: optimize this better:
//			_imageDrawable->getOrCreateStateSet()->merge(*_imageStateSet.get());
//			_geode->addChild(_imageDrawable);
//			imageBox = _imageDrawable->getBoundingBox();
//		}
//	}
//
//	if (_image.valid())
//	{
//		TextSymbol* textSymbol = _style.getOrCreate<TextSymbol>();
//		if (!textSymbol->alignment().isSet())
//			textSymbol->alignment() = textSymbol->ALIGN_LEFT_CENTER;
//	}
//
//	_textDrawable = AnnotationUtils::createTextDrawable(
//		_text,
//		_style.get<TextSymbol>(),
//		imageBox);
//
//	const BBoxSymbol* bboxsymbol = _style.get<BBoxSymbol>();
//	if (bboxsymbol && _textDrawable)
//	{
//		_bboxDrawable = new BboxDrawable(_textDrawable->getBoundingBox(), *bboxsymbol);
//		_geode->addChild(_bboxDrawable);
//	}
//
//	if (_textDrawable)
//	{
//		_geode->addChild(_textDrawable);
//	}
//
//#if 0 // test a drop line
//	LineDrawable* line = new LineDrawable(GL_LINES);
//	line->pushVertex(osg::Vec3(0, 0, 0));
//	line->pushVertex(osg::Vec3(0, 0, -100000));
//	line->finish();
//	getPositionAttitudeTransform()->addChild(line);
//#endif
//
//	setDefaultLighting(false);
//
//	applyStyle(_style);
//
//	setPriority(getPriority());
//
//	if (_dynamic)
//		setDynamic(_dynamic);
//
//	updateLayoutData();
//}
//
//void
//PlaceNode::dirty()
//{
//	GeoPositionNode::dirty();
//	updateLayoutData();
//}
//
//void
//PlaceNode::setPriority(float value)
//{
//	GeoPositionNode::setPriority(value);
//	updateLayoutData();
//}
//
//void
//PlaceNode::updateLayoutData()
//{
//	if (!_dataLayout.valid())
//	{
//		_dataLayout = new ScreenSpaceLayoutData();
//	}
//
//	// re-apply annotation drawable-level stuff as neccesary.
//	if (_imageDrawable)
//		_imageDrawable->setUserData(_dataLayout.get());
//
//	if (_bboxDrawable)
//		_bboxDrawable->setUserData(_dataLayout.get());
//
//	if (_textDrawable)
//		_textDrawable->setUserData(_dataLayout.get());
//
//	_dataLayout->setPriority(getPriority());
//
//	GeoPoint location = getPosition();
//	location.makeGeographic();
//	double latRad;
//	double longRad;
//	GeoMath::destination(osg::DegreesToRadians(location.y()),
//		osg::DegreesToRadians(location.x()),
//		_labelRotationRad,
//		2500.,
//		latRad,
//		longRad);
//
//	_geoPointProj.set(osgEarth::SpatialReference::get("wgs84"),
//		osg::RadiansToDegrees(longRad),
//		osg::RadiansToDegrees(latRad),
//		0,
//		osgEarth::ALTMODE_ABSOLUTE);
//
//	_geoPointLoc.set(osgEarth::SpatialReference::get("wgs84"),
//		//location.getSRS(),
//		location.x(),
//		location.y(),
//		0,
//		osgEarth::ALTMODE_ABSOLUTE);
//
//	const TextSymbol* ts = getStyle().get<TextSymbol>();
//	if (ts)
//	{
//		_dataLayout->setPixelOffset(ts->pixelOffset().get());
//
//		if (_followFixedCourse)
//		{
//			osg::Vec3d p0, p1;
//			_geoPointLoc.toWorld(p0);
//			_geoPointProj.toWorld(p1);
//			_dataLayout->setAnchorPoint(p0);
//			_dataLayout->setProjPoint(p1);
//		}
//	}
//}
//
//void
//PlaceNode::setText(const std::string& text)
//{
//	if (!_dynamic && !_geode)
//	{
//		OE_WARN << LC << "Illegal state: cannot change a LabelNode that is not dynamic" << std::endl;
//		return;
//	}
//
//	_text = text;
//
//	if (_textDrawable)
//	{
//		TextSymbol* symbol = _style.getOrCreate<TextSymbol>();
//		osgText::String::Encoding text_encoding = osgText::String::ENCODING_UNDEFINED;
//		if (symbol && symbol->encoding().isSet())
//		{
//			text_encoding = AnnotationUtils::convertTextSymbolEncoding(symbol->encoding().value());
//		}
//
//		_textDrawable->setText(text, text_encoding);
//	}
//}
//
//void
//PlaceNode::setStyle(const Style& style)
//{
//	// changing the style requires a complete rebuild.
//	_style = style;
//	compile();
//}
//
//void
//PlaceNode::setStyle(const Style& style, const osgDB::Options* readOptions)
//{
//	// changing the style requires a complete rebuild.
//	_style = style;
//	_readOptions = readOptions;
//	compile();
//}
//
//void GeoGlobe::Annotation::PlaceNode::setShadowColor(const osg::Vec4& color)
//{
//	if (_textDrawable)
//	{
//		_textDrawable->setBackdropColor(color);
//		m_shadowColor = color;
//	}
//}
//
//const osg::Vec4& GeoGlobe::Annotation::PlaceNode::getShadowColor()
//{
//	if (_textDrawable)
//		return _textDrawable->getBackdropColor();
//}
//
//void GeoGlobe::Annotation::PlaceNode::setShadowOffset(const double& dbShadowOffset)
//{
//	if (_textDrawable)
//	{
//		_textDrawable->setBackdropOffset(dbShadowOffset);
//		m_dbShadowOffset = dbShadowOffset;
//	}
//}
//const double& GeoGlobe::Annotation::PlaceNode::getShadowOffset()
//{
//	return m_dbShadowOffset;
//}
//
//void GeoGlobe::Annotation::PlaceNode::setShadowPos(const osgText::Text::BackdropType& eShadowPos)
//{
//	if (_textDrawable)
//	{
//		_textDrawable->setBackdropType(eShadowPos);
//		m_eShadowPos = eShadowPos;
//	}
//}
//
//const osgText::Text::BackdropType& GeoGlobe::Annotation::PlaceNode::getShadowPos()
//{
//	if (_textDrawable)
//		return _textDrawable->getBackdropType();
//}
//
//void
//PlaceNode::setIconImage(osg::Image* image)
//{
//	if (_image != image)
//	{
//		_image = image;
//		if (_imageDrawable)
//		{
//			osg::Texture2D* texture = dynamic_cast<osg::Texture2D*>(_imageDrawable->getStateSet()->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
//			if (texture)
//			{
//				texture->setImage(_image);
//			}
//		}
//		else
//		{
//			compile();
//		}
//	}
//}
//
//void
//PlaceNode::setDynamic(bool value)
//{
//	GeoPositionNode::setDynamic(value);
//
//	osg::Object::DataVariance dv = value ? osg::Object::DYNAMIC : osg::Object::STATIC;
//
//	if (_textDrawable)
//		_textDrawable->setDataVariance(dv);
//
//	if (_bboxDrawable)
//		_bboxDrawable->setDataVariance(dv);
//
//	if (_imageDrawable)
//		_imageDrawable->setDataVariance(dv);
//}
//
////-------------------------------------------------------------------
//
//OSGEARTH_REGISTER_ANNOTATION(place, GeoGlobe::Annotation::PlaceNode);
//
//PlaceNode::PlaceNode(const Config&         conf,
//	const osgDB::Options* readOptions) :
//	GeoPositionNode(conf, readOptions),
//	_readOptions(readOptions)
//{
//	construct();
//
//	conf.get("style", _style);
//	conf.get("text", _text);
//
//	optional<URI> imageURI;
//	conf.get("icon", imageURI);
//	if (imageURI.isSet())
//	{
//		_image = imageURI->getImage();
//		if (_image.valid())
//			_image->setFileName(imageURI->base());
//	}
//
//	compile();
//}
//
//void
//PlaceNode::setConfig(const Config& conf)
//{
//	GeoPositionNode::setConfig(conf);
//
//	conf.get("style", _style);
//	conf.get("text", _text);
//
//	optional<URI> imageURI;
//	conf.get("icon", imageURI);
//	if (imageURI.isSet())
//	{
//		_image = imageURI->getImage();
//		if (_image.valid())
//			_image->setFileName(imageURI->base());
//	}
//
//	//init();
//}
//
//Config
//PlaceNode::getConfig() const
//{
//	Config conf("place");
//	conf.set("text", _text);
//	conf.set("style", _style);
//	if (_image.valid()) {
//		if (!_image->getFileName().empty())
//			conf.set("icon", _image->getFileName());
//		else if (!_image->getName().empty())
//			conf.set("icon", _image->getName());
//	}
//
//	return conf;
//}
//
//
//#undef  LC
//#define LC "[PlaceNode Serializer] "
//
//#include <osgDB/ObjectWrapper>
//#include <osgDB/InputStream>
//#include <osgDB/OutputStream>
//
//namespace osgEarth {
//	namespace Serializers {
//		namespace PlaceNode
//		{
//			// functions
//			static bool checkConfig(const GeoGlobe::Annotation::PlaceNode& node)
//			{
//				return true;
//			}
//
//			static bool readConfig(osgDB::InputStream& is, GeoGlobe::Annotation::PlaceNode& node)
//			{
//				std::string json;
//				is >> json;
//				Config conf;
//				conf.fromJSON(json);
//				node.setConfig(conf);
//				return true;
//			}
//
//			static bool writeConfig(osgDB::OutputStream& os, const GeoGlobe::Annotation::PlaceNode& node)
//			{
//				os << node.getConfig().toJSON(false) << std::endl;
//				return true;
//			}
//
//			REGISTER_OBJECT_WRAPPER(
//				PlaceNode,
//				new GeoGlobe::Annotation::PlaceNode,
//				GeoGlobe::Annotation::PlaceNode,
//				"osg::Object osg::Node osg::Group osgEarth::Annotation::AnnotationNode osgEarth::Annotation::GeoPositionNode osgEarth::Annotation::PlaceNode")
//			{
//				ADD_USER_SERIALIZER(Config);
//			}
//		}
//	}
//}
