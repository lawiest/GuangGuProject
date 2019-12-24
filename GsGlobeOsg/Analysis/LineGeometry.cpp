#include "LineGeometry.h"
#include <osg/BlendFunc>
#include <osg/Texture2D>
#include <osgViewer/View>
#include <Animator.h>

namespace osg 
{


	static const char * vDotLineShaderStr = {
		//	"precision highp float; \n"
			"uniform  vec2 tex_scale;  \n"
			"uniform float u_width;\n"
			"attribute  vec3 vertex;  \n"
			"varying  vec3 my_tex; \n"
		"void main(void)  \n"
		"{  \n"
		"vec3 dir =  vertex.xyz; \n"
		"gl_Position = gl_ModelViewProjectionMatrix*vec4(gl_Vertex.xyz+(u_width * dir), 1.0); \n"
		"my_tex =  vec3(gl_MultiTexCoord0.x,gl_MultiTexCoord0.y*tex_scale.x+tex_scale.y,gl_MultiTexCoord0.y); \n"
		"}  \n"
	};

	static const char * fDotLineShaderStr = {
		//"precision highp float; \n"
		"uniform sampler2D sampler2d; \n"
		"uniform  vec4 u_color;  \n"
		"uniform float u_previe;\n"
		//+"uniform mediump bool u_isPic; \n"
		"varying  vec3 my_tex; \n"
		"void main() \n"
		"{ \n"
		"   vec4 out_color = texture2D(sampler2d,my_tex.xy) ;  \n"
		"  gl_FragColor =vec4(1.0,1.0,1.0,u_color.a*step(my_tex.z,u_previe))*out_color;//vec4(0.2,0.9,0.2,out_color.a);//   u_color;\n"
		"} \n"
	};

	static const char* fDotLineColorShaderStr = {
		//"precision highp float; \n"
		"uniform sampler2D sampler2d; \n"
		"uniform  vec4 u_color;  \n"
		"uniform float u_previe;\n"
		"varying  vec3 my_tex; \n"
		"void main() \n"
		"{ \n"
		"   vec4 out_color = texture2D(sampler2d,my_tex.xy) ;  \n"
		"  gl_FragColor =  vec4(1.0,1.0,1.0,out_color.a*step(my_tex.z,u_previe)) * u_color;\n"
		"} \n"
	};
	// 图片模式
	static int PICTRUE_MODE = 1;
	//
	static int COLOR_MODE = 2;
	// 纹理贴图模式，重复贴图
	static int IMAGE_REPEAT = 3;
	// 纹理贴图 边缘颜色填充
	static int IMAGE_TO_EDGE = 4;
	const float DISTANCECAMERA = 382.97832107543945 * 5;

	LineGeometry::LineGeometry()
	{
	}

	LineGeometry::LineGeometry(float width, float imageH, osg::Image * lineImage,int mixMod,int textureType):osg::Geometry()
	
	{
		m_lineWidth = width/2.0f;
		m_ImageHeigth = imageH;
		m_lineImage = lineImage;
		m_LineWidthScale = 1;
		m_FixLod = 1;
		m_LinePics = 1;
		m_TextureType = textureType;
		BuildStats(mixMod);
		setUseDisplayList(false);
		m_lineColorAphla = 1;
	}

	LineGeometry::LineGeometry(const LineGeometry & geometry, const CopyOp & copyop):osg::Geometry(geometry, copyop)
	{

	}

	void LineGeometry::drawImplementation(RenderInfo & renderInfo) const
	{
		osgViewer::View* osgView = dynamic_cast<osgViewer::View*>(renderInfo.getView());
		
		if (osgView)
		{
			
			float scale =m_FixLod<0?0.04: osgView->getCameraManipulator()->getFusionDistanceValue() / DISTANCECAMERA;
			//线动画
			GLOBE_NAME::LineAnimatorActionPtr lineAction = dynamic_cast<GLOBE_NAME::LineAnimatorAction*>(m_ptrAnimatorAction.p);
			float textCoord = 0;
			float lineScale = 1;// 默认缩放值
			float prive = 1;
			if (lineAction)
			{
				double time = osgView->getFrameStamp()->getSimulationTime()*1000;
				float a = lineAction->Update(time);
				bool isStart = lineAction->isStart();
				// 纹理动画，箭头动向
				if (isStart&&lineAction->getTextCoord())
					textCoord = lineAction->getTextCoord()->GetValue(a);
				// 线宽动画
				if (isStart&& lineAction->getWidthScale())
					lineScale = lineAction->getWidthScale()->GetValue(a);
				// 透明动画
				if (isStart&& lineAction->getAphla())
				{
					// 直接更改颜色值的透明值
					(*m_uniColor->getFloatArray())[3] = m_lineColorAphla*lineAction->getAphla()->GetValue(a);
					m_uniColor->dirty();
				}
				else if(!isStart) {
					(*m_uniColor->getFloatArray())[3] = m_lineColorAphla;
					m_uniColor->dirty();
				}
				//百分比
				if (isStart&& lineAction->getPrevice())
					prive = lineAction->getPrevice()->GetValue(a);
				m_uPrive->set(prive);
			}
			//线宽根据缩放的计算
			m_uniWidth->set(m_lineWidth*scale*lineScale);
			//纹理坐标长度计算
			if (m_ImageHeigth > 1)
			{
				// 若边缘填充模式，
				if (m_TextureType == IMAGE_TO_EDGE)
				{
					textCoord *= (m_LinePics / scale);
					textCoord += 0.5f;
				}
				m_uniTextCoord->set(osg::Vec2(m_LinePics / scale, textCoord));
			}
		}
		osg::Geometry::drawImplementation(renderInfo);
	}

	void LineGeometry::SetLineLength(double length)
	{
		m_lineLenght = length;
		m_LinePics = length / m_ImageHeigth;
	}
	void LineGeometry::SetLineShape(osg::Array * vertexPoints, osg::Array * vertexDir, osg::Array * textcoord, osg::UShortArray* trangles)
	{
		setVertexArray(vertexPoints);
		setTexCoordArray(0, textcoord);//纹理坐标设置
		setVertexAttribArray(10, vertexDir, osg::Array::BIND_PER_VERTEX);
		//
		osg::ref_ptr<osg::DrawElementsUShort> tranglesPrimitives = new osg::DrawElementsUShort(osg::PrimitiveSet::Mode::TRIANGLES, trangles->begin(), trangles->end());
		if (getPrimitiveSetList().size() > 0)
		{
			setPrimitiveSet(0,tranglesPrimitives);
		}
		else {
			addPrimitiveSet(tranglesPrimitives);
		}

		
	}
	void LineGeometry::SetLineScale(float lineScale)
	{
		m_LineWidthScale = lineScale;
	}
	float LineGeometry::GetLineScale()
	{
		return m_LineWidthScale;
	}
	// 线宽
	float LineGeometry::getLineWidth()
	{
		return m_lineWidth*2;
	}
	void LineGeometry::setLineWidth(float w)
	{
		m_lineWidth = w/2.f;
	}
	float LineGeometry::getLineImageHeight()
	{
		return m_ImageHeigth;
	}
	// 线的颜色
	void LineGeometry::setLineColor(const osg::Vec4& color)
	{
		// 屏蔽颜色完全透明不可见的情况
		if (color.a() == 0)return;

		m_uniColor->set(color);
		m_lineColorAphla = color.a();
	}
	osg::Vec4 LineGeometry::getLineColor()
	{
		osg::Vec4 color;
		m_uniColor->get(color);
		color.a()=m_lineColorAphla;
		return color;
	}
	// 是否固定宽度
	float LineGeometry::getFixLod()
	{
		return m_FixLod;
	}
	void LineGeometry::setFixLod(float fixLod)
	{
		m_FixLod = fixLod;
	}
	void LineGeometry::setAnimatorAction(GLOBE_NAME::AnimatorAction * animatorAction)
	{
		m_ptrAnimatorAction = animatorAction;
	}
	GLOBE_NAME::AnimatorAction * LineGeometry::getAnimatorAction()
	{
		return m_ptrAnimatorAction;
	}
	void LineGeometry::BuildStats(int mixMod)
	{
		//着色器设置
		osg::StateSet* ss = getOrCreateStateSet();
		osg::Program* program = new osg::Program;
		program->setName("dotLineShader");
		program->addShader(new osg::Shader(osg::Shader::VERTEX, vDotLineShaderStr));
		const char* fShader = mixMod == PICTRUE_MODE ? fDotLineShaderStr : fDotLineColorShaderStr;
		program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fShader));
		program->addBindAttribLocation("vertex", 10);
		//设置着色器程序
		ss->setAttributeAndModes(program, osg::StateAttribute::ON);
		// 开启着色器透明
		ss->setMode(GL_BLEND, osg::StateAttribute::ON);
		ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF );
		osg::BlendFunc * aphlafunc = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		ss->setAttribute(aphlafunc, osg::StateAttribute::ON);
		ss->setRenderBinDetails(11, "RenderBin");
		//添加着色器上的变量
		m_uniWidth= new osg::Uniform("u_width", 0.25f);
		ss->addUniform(m_uniWidth);
		m_uniTextCoord = new osg::Uniform("tex_scale", osg::Vec2(1, 0));
		ss->addUniform(m_uniTextCoord);
		// 颜色值
		m_uniColor = new osg::Uniform("u_color", osg::Vec4(1, 1, 1, 1));
		//if (mixMod == COLOR_MODE)
		ss->addUniform(m_uniColor);
		//
		m_uPrive = new osg::Uniform("u_previe",1.f);
		ss->addUniform(m_uPrive);
		//
			osg::Texture2D* texture = new osg::Texture2D;
			texture->setImage(m_lineImage);
			osg::Texture::WrapMode model = m_TextureType == IMAGE_REPEAT ? osg::Texture::WrapMode::REPEAT : osg::Texture::WrapMode::CLAMP_TO_EDGE;
			texture->setWrap(osg::Texture::WRAP_S, model);
			texture->setWrap(osg::Texture::WRAP_T, model);
			if (m_TextureType == IMAGE_TO_EDGE)
			{
				texture->setFilter(osg::Texture::FilterParameter::MIN_FILTER, osg::Texture::FilterMode::LINEAR);
				texture->setFilter(osg::Texture::FilterParameter::MAG_FILTER, osg::Texture::FilterMode::LINEAR);
			}
			ss->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
			m_CurrStateSet = ss;
	}
}