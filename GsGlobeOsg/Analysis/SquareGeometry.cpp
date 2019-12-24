#include "SquareGeometry.h"
#include <osg\BlendFunc>
#include <osg\Texture2D>
#include <osgViewer\View>
#include <Animator.h>

namespace osg
{
	//
	static const char *squareVertSource = {
		"// microshader - colors a fragment based on its position\n"
		"varying vec2 m_coord;\n"
		"void main(void)\n"
		"{\n"
		"   m_coord= gl_MultiTexCoord0.xy;\n"
		"    gl_Position =   gl_ModelViewProjectionMatrix *gl_Vertex;\n"
		"}\n"
	};
	//
	static const char* squareRoatVertSource = {
		 "uniform  mat2 pos_rota;\n"
		 "varying  vec2 m_coord; \n"
		 "void main(void)  \n"
		 "{  \n"
		"   m_coord= gl_MultiTexCoord0.xy;\n"
		 "vec2 temp = pos_rota * gl_Vertex.xy;\n"
		 " gl_Position = gl_ModelViewProjectionMatrix * vec4(temp,0,1);  \n"
		 "}  \n"
	};
	// 圆环扩散效果
	static const char *squareCircleFragSource = {
		"uniform float u_distance;\n"// 半径
		"uniform vec4 u_color;\n"// 颜色效果
		"varying vec2 m_coord;\n"
		"void main(void)\n"
		"{\n"
		 "float r = distance(vec2(0.5,0.5),m_coord);\n"
		 "float a = smoothstep(u_distance, u_distance-0.02,r);\n"
		" a *= smoothstep(u_distance-0.35, u_distance,r);\n"
		 "if(r>0.49)a*= smoothstep(0.5, 0.49,r);\n"
		"    gl_FragColor =vec4(1.0,1.0,1.0,a)*u_color;// clamp( color, 0.0, 1.0 );\n"
		"}\n"
	};
	// 点图片的发光shader
	static const char* squarefShaderStr = { 
		 "uniform sampler2D sampler2d; \n"
		 "uniform vec4 u_color;\n"
		 "varying  vec2 m_coord; \n"
		 "void main()   \n"
		 "{ \n"
		 "vec4 texture=texture2D(sampler2d,m_coord);\n"
		 "  gl_FragColor =  vec4(u_color.rgb + texture.rrr, (texture.r + texture.b) * u_color.a) ; \n"
		 "}  \n" 
	};
	// 显示绘制图片的的过程
	static const char* squarePicShaderStr = {
		"uniform sampler2D sampler2d; \n"
		"uniform vec4 u_color;\n"
		"varying  vec2 m_coord; \n"
		"void main()   \n"
		"{ \n"
		"vec4 texture=texture2D(sampler2d,m_coord);\n"
		"  gl_FragColor =  vec4(texture.rgb,u_color.a*texture.a); \n"
		"}  \n"
	};
	//
	SquareGeometry::SquareGeometry(float w, float h):SquareGeometry(w,h,nullptr,1)
	{
	}
	SquareGeometry::SquareGeometry(float w, float h, osg::Image * lineImage, int mode)
	{
		m_width = w;
		m_height = h;
		m_Image = lineImage;
		m_SquareMode = mode;
		build();
		setUseDisplayList(false);
		m_lineColorAphla = 1;
	}
	SquareGeometry::~SquareGeometry()
	{
	}
	void SquareGeometry::drawImplementation(RenderInfo & renderInfo) const
	{
		osgViewer::View* osgView = dynamic_cast<osgViewer::View*>(renderInfo.getView());

		if (osgView)
		{
			GLOBE_NAME::PointAnimatorActionPtr pointaciton = dynamic_cast<GLOBE_NAME::PointAnimatorAction*>(m_ptrAnimatorAction.p);
			if (pointaciton)
			{
				double time = osgView->getFrameStamp()->getSimulationTime() * 1000;
				float a = pointaciton->Update(time);
				bool isStart = pointaciton->isStart();
				// 透明动画
				if (isStart&&pointaciton->getAphla())
				{
					// 直接更改颜色值的透明值
					(*m_uniColor->getFloatArray())[3] = m_lineColorAphla*pointaciton->getAphla()->GetValue(a);
					m_uniColor->dirty();
				}
				//缩放动画
				if (isStart&&pointaciton->getScale())
				{
					m_uCirle->set(pointaciton->getScale()->GetValue(a));
				}
				// 旋转动画
			}
		}
		osg::Geometry::drawImplementation(renderInfo);
	}
	void SquareGeometry::setColor(const osg::Vec4 & color)
	{
		m_uniColor->set(color);
		m_lineColorAphla = color.a();
	}
	osg::Vec4 SquareGeometry::getColor()
	{
		osg::Vec4 color;
		m_uniColor->get(color);
		color.a() = m_lineColorAphla;
		return color;
	}
	float SquareGeometry::getWidth()
	{
		return m_width;
	}
	float SquareGeometry::getHeight()
	{
		return m_height;
	}
	void SquareGeometry::setAnimatorAction(GLOBE_NAME::AnimatorAction * animatorAction)
	{
		m_ptrAnimatorAction = animatorAction;
	}
	GLOBE_NAME::AnimatorAction * SquareGeometry::getAnimatorAction()
	{
		return m_ptrAnimatorAction;
	}
	// 编译生成
	void SquareGeometry::build()
	{
		osg::Vec3 vleftbuootm(-m_width*0.5f, -m_height*0.5f, 0);
		osg::Vec3 vwidth(m_width, 0, 0);
		osg::Vec3 vheight(0, m_height, 0);
		// 顶点坐标添加
		osg::Vec3Array* vertext = new osg::Vec3Array(4);
		(*vertext)[0] = vleftbuootm;
		(*vertext)[1] = vleftbuootm+vwidth;
		(*vertext)[2] = vleftbuootm+vheight;
		(*vertext)[3] = vleftbuootm+vwidth+vheight;
		// 纹理坐标
		osg::Vec2Array* tcoords = new osg::Vec2Array(4);
		(*tcoords)[0].set(0.0f, 0.0f);
		(*tcoords)[1].set(1.0f, 0.0f);
		(*tcoords)[2].set(0.0f, 1.0f);
		(*tcoords)[3].set(1.0f, 1.0f);
		//
		setVertexArray(vertext);//顶点坐标
		setTexCoordArray(0,tcoords);// 纹理坐标
		//着色器设置
		osg::StateSet* ss = getOrCreateStateSet();
		osg::Program* program = new osg::Program;
		program->setName("squareLineShader");
		const char* verterx = m_SquareMode == 1 ? squareVertSource : squareRoatVertSource;
		program->addShader(new osg::Shader(osg::Shader::VERTEX, verterx));
		const char* fShader = m_SquareMode == 1 ? squareCircleFragSource: squarefShaderStr;
		program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fShader));
		//设置着色器程序
		ss->setAttributeAndModes(program, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
		// 开启着色器透明
		ss->setMode(GL_BLEND, osg::StateAttribute::ON);
		ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
		ss->setAttribute(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA), osg::StateAttribute::ON);
		ss->setRenderBinDetails(11, "RenderBin");//设置渲染顺序
		//添加着色器上的变量
		// 颜色值
		m_uniColor = new osg::Uniform("u_color", osg::Vec4(1, 0, 1, 1));
		m_uCirle = new osg::Uniform("u_distance", 0.2f);
		m_uniRota = new osg::Uniform("pos_rota", osg::Matrix2(1,0,0,1));
		//
		ss->addUniform(m_uniColor);
		if (m_SquareMode == 1)
			ss->addUniform(m_uCirle);
		else
			ss->addUniform(m_uniRota);
		// 纹理
		if(m_Image)
		{
			osg::Texture2D* texture = new osg::Texture2D;
			texture->setImage(m_Image);
			texture->setWrap(osg::Texture::WRAP_S, osg::Texture::WrapMode::REPEAT);
			texture->setWrap(osg::Texture::WRAP_T, osg::Texture::WrapMode::REPEAT);
			ss->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
		}
		addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::Mode::TRIANGLE_STRIP, 0, 4));
	}
}