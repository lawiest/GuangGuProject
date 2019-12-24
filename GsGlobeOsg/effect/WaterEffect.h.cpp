#include "WaterEffect.h"
#include <osgDB/ReadFile>
#include <osg/BlendFunc>
#include <osgDB/WriteFile>
#include <osg/ClipPlane>
#include <osg/ClipNode>
#include <osgUtil/SmoothingVisitor>

using namespace GeoGlobe;
using namespace Effect;

WaterTransform::WaterTransform(WaterEffect* pWater)
	:m_pWater(pWater)
{

}

WaterTransform::~WaterTransform()
{

}

void WaterTransform::traverse(osg::NodeVisitor& nv)
{
	if(m_pWater)
	{
		m_pWater->Update();
	}

	osg::MatrixTransform::traverse(nv);
}

////////////////////////////////////////////////////////////////////////////////
WaterEffect::WaterEffect()
	:m_surfaceObject(0)
	,m_reflectionTexture(0)
	,m_refractionTexture(0)
	,m_vertexShader(0)
	,m_fragmentShader(0)
	,m_program(0)
	,m_reflectionCamera(0)
	,m_pCamera(0)
	,m_visible(true)
{
	Initialize();
}

void WaterEffect::Initialize()
{
	m_vertexShader = new osg::Shader( osg::Shader::VERTEX );
	m_fragmentShader = new osg::Shader( osg::Shader::FRAGMENT );

	m_program = new osg::Program();
	m_program->addShader( m_vertexShader );
	m_program->addShader( m_fragmentShader );

	m_transform = new WaterTransform(this);
	if(m_transform)
	{
		m_transform->setNodeMask(0x1);
	}

	m_surfaceObject =  new osg::Geode(); 
	if(m_surfaceObject)
	{
		m_surfaceObject->setNodeMask(0x1);
	}

	m_reflectionTexture = new osg::Texture2D(new osg::Image);
	if(m_reflectionTexture)
	{
		m_reflectionTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
		m_reflectionTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
		m_reflectionTexture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
		m_reflectionTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
		m_reflectionTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
		m_reflectionTexture->setInternalFormat(GL_RGBA);
	}

	m_refractionTexture = new osg::Texture2D(new osg::Image);
	if(m_refractionTexture)
	{
		m_refractionTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		m_refractionTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
		m_refractionTexture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
		m_refractionTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
		m_refractionTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
		m_refractionTexture->setInternalFormat(GL_RGBA);
	}
}

WaterEffect::~WaterEffect()
{

}

void WaterEffect::SetRTTCamera(int width, int height)
{
	if(nullptr == m_reflectionCamera)
	{
		m_reflectionCamera = new osg::Camera;  
	}
	m_reflectionTexture->getImage()->asImage()->allocateImage(width,height,1,GL_RGBA,GL_UNSIGNED_BYTE, 1); 
	m_reflectionCamera->setProjectionMatrixAsPerspective(45.0f, static_cast<double>(width)/static_cast<double>(height), 0.01f, 10000.0f);
	m_reflectionCamera->setClearColor(osg::Vec4(0.2,0.3,0.58,1.0));  
	m_reflectionCamera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);  
	m_reflectionCamera->setRenderOrder(osg::Camera::PRE_RENDER,0);  
	m_reflectionCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF_INHERIT_VIEWPOINT);  
	m_reflectionCamera->setViewport(0, 0,width, height);  
	m_reflectionCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);  
	m_reflectionCamera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	m_reflectionCamera->attach(osg::Camera::COLOR_BUFFER0, m_reflectionTexture);
}

void WaterEffect::SetRefractionCamera(int width, int height)
{
	if(nullptr == m_refractionCamera)
	{
		m_refractionCamera = new osg::Camera;  
	}

	m_refractionTexture->getImage()->asImage()->allocateImage(width,height,1,GL_RGBA,GL_UNSIGNED_BYTE, 1); 
	m_refractionCamera->setViewport(0, 0,width, height);  
	m_refractionCamera->setClearColor(osg::Vec4(0.2,0.3,0.58,1.0));  
	m_refractionCamera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);  
	m_refractionCamera->setRenderOrder(osg::Camera::PRE_RENDER,1);  
	m_refractionCamera->setReferenceFrame(osg::Camera::ABSOLUTE_RF_INHERIT_VIEWPOINT);  
	m_refractionCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);  
	m_refractionCamera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	m_refractionCamera->attach(osg::Camera::COLOR_BUFFER1, m_refractionTexture);
}

void WaterEffect::SetWaterSurfaceObject(osg::ref_ptr<osg::Geometry> surface)
{
	if(surface&&m_surfaceObject)
	{
		m_surfaceObject->addDrawable(surface.get());
		m_transform->addChild(m_surfaceObject.get());
	}
}

osg::Texture2D* WaterEffect::CreateTexture( const char* filename)
{
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setImage( osgDB::readImageFile(filename) );
	texture->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
	texture->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
	texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
	texture->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
	return texture.release();
}

void WaterEffect::SetPosition(const osg::Matrix& matrix)
{
	if(m_transform && m_surfaceObject)
	{
		m_transform->setMatrix(matrix);
		m_worldMatrix = matrix;

		int index = 0;
		osg::Vec3d p[3];

		osg::Geometry* pGeo =  dynamic_cast<osg::Geometry*>(m_surfaceObject->getDrawable(0));
		osg::Vec3Array* verArray = (osg::Vec3Array*)(pGeo->getVertexArray());
		for(osg::Vec3Array::iterator iter = verArray->begin(); iter<verArray->begin() +3 ; iter++)
		{
			p[index++] = osg::Vec3d( iter->x(),iter->y(),iter->z());
		}

		m_orgiVertices[0] = p[0] * matrix;
		m_orgiVertices[1] = p[1] * matrix;
		m_orgiVertices[2] = p[2] * matrix;

		osg::Vec3d normal = (m_orgiVertices[0] -  m_orgiVertices[1]) ^ (m_orgiVertices[2] - m_orgiVertices[1]);
		normal.normalize();
		double distance = -normal * m_orgiVertices[1];
		m_plane = osg::Vec4d(normal, distance);
	}
}

void WaterEffect::LoadShader()
{
	if(m_surfaceObject&&m_vertexShader&&m_fragmentShader&&m_program)
	{
		osg::StateSet* stateset = m_transform->getOrCreateStateSet();

		bool sucess = false;
		if(m_vertexShader->loadShaderSourceFromFile("../resource/effect/water/shaders/water.vert"))
		{
			sucess = m_fragmentShader->loadShaderSourceFromFile("../resource/effect/water/shaders/water.frag");
		}

		if(sucess)
		{
			stateset->setAttributeAndModes( m_program.get(), osg::StateAttribute::ON);
		}
	}
}

void WaterEffect::SetLigthPosition(const osg::Vec3f& lightPos)
{
	m_lightPosition = lightPos;
}

bool WaterEffect::PreRender(osg::Group* renderObjects)
{
	if(!renderObjects || !m_pCamera || !m_program || !m_vertexShader || !m_fragmentShader || !m_surfaceObject)
	{
		return false;
	}
	LoadShader();

	SetRTTCamera(1024,1024);
	SetRefractionCamera(1024,1024);
	//osg::ref_ptr<osg::ClipPlane> clipPlane = new osg::ClipPlane;
	//clipPlane->setClipPlane(m_plane);
	//clipPlane->setClipPlaneNum(0);

	//osg::ref_ptr<osg::ClipNode> clipNode = new osg::ClipNode;
	//clipNode->addClipPlane(clipPlane.get());
	//clipNode->addChild(renderObjects);
	//m_reflectionCamera->addChild(renderObjects);
	//m_refractionCamera->addChild(renderObjects);


	osg::StateSet* pStateset = m_transform->getOrCreateStateSet();
	if(!pStateset)
	{
		return false;
	}

	pStateset->setMode(GL_BLEND, osg::StateAttribute::OFF); 
	pStateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	pStateset->setTextureAttributeAndModes( 0, CreateTexture("../resource/effect/water/image/bottom.jpg"));
	pStateset->setTextureAttributeAndModes( 1, CreateTexture("../resource/effect/water/image/negx.bmp") );
	pStateset->setTextureAttributeAndModes( 2,  CreateTexture("../resource/effect/water/image/tangent.jpg") );
	pStateset->setTextureAttributeAndModes( 3, CreateTexture("../resource/effect/water/image/normal.jpg") );

	pStateset->addUniform( new osg::Uniform("reflection", 0) );
	pStateset->addUniform( new osg::Uniform("defaultTex", 1) );
	pStateset->addUniform( new osg::Uniform("refraction", 2) );
	pStateset->addUniform( new osg::Uniform("normalTex", 3) );
	pStateset->addUniform(new osg::Uniform("lightPos", osg::Vec3f(m_lightPosition.x(),m_lightPosition.y(),m_lightPosition.z())));  
	pStateset->addUniform(new osg::Uniform("osg_FrameTime", float(0.0)));
	pStateset->addUniform(new osg::Uniform("matReflectionModelViewProject", osg::Matrix::identity())); 
	 return true;
}

void WaterEffect::AttachCamera(osg::Camera* pCamera)
{
	if(pCamera)
	{
		m_pCamera = pCamera;
	}
}

void WaterEffect::SetVisible(bool visible)
{
	if(m_transform&&m_surfaceObject)
	{
		if(visible)
		{
			m_transform->setNodeMask(0x1);
			m_surfaceObject->setNodeMask(0x1);
		}
		else
		{
			m_transform->setNodeMask(0x0);
			m_surfaceObject->setNodeMask(0x0);
		}
		m_visible = visible;
	}
}

void WaterEffect::Update()
{
	if(!m_visible || !m_pCamera || !m_reflectionCamera)
	{
		return;
	}

	//lost the device
	//m_reflectionCamera->setViewport(0,0,m_pCamera->getViewport()->width(), m_pCamera->getViewport()->height());
	//m_reflectionCamera->setProjectionMatrix(m_pCamera->getProjectionMatrix());

	float time = m_pCamera->getView()->getFrameStamp()->getSimulationTime();
	m_transform->getOrCreateStateSet()->getUniform("osg_FrameTime")->set(time);

	//rtt相机镜像投影;
	osg::Vec3d eye, center, up;  
	m_pCamera->getViewMatrixAsLookAt(eye, center, up);

	//利用反射矩阵来进行反射计算
	 osg::Matrix mirror = osg::Matrix (1-2*m_plane.x()*m_plane.x(),		-2*m_plane.x()* m_plane.y(),			-2*m_plane.x()*m_plane.z(),			0,  
		 -2* m_plane.y()*m_plane.x(),		1 - 2* m_plane.y()* m_plane.y(),    -2* m_plane.y()*m_plane.z(),			0, 
		 -2*m_plane.z()*m_plane.x(),		-2*m_plane.z()* m_plane.y(),         1-2*m_plane.z()*m_plane.z(),			0,
		 -2* m_plane.w()*m_plane.x(),      -2* m_plane.w()* m_plane.y(),         -2* m_plane.w()*m_plane.z(),         1);

	 //第二种算法，直接用向量和法线来求解反射向量和位置
	// osg::Vec3d normal = osg::Vec3d(m_plane.x(),m_plane.y(),m_plane.z());
	// osg::Vec3d c = m_worldMatrix.getTrans();
	// osg::Vec3d eyedir = eye - c;
	// double d = eyedir.length();

	// eyedir.normalize(); 
	// eyedir = eyedir - normal*(eyedir*normal)*2.0;
	// eyedir.normalize();
	// osg::Vec3d newEye = c + eyedir*d;

	// osg::Vec3d cendir = center - c;
	// d = cendir.length();

	// cendir.normalize(); 
	// cendir = cendir - normal*(cendir*normal)*2.0;
	// cendir.normalize();
	// osg::Vec3d newCenter = c + cendir*d;

	// osg::Vec3d newUp = up -  normal*(up*normal)*2.0;
	// newUp.normalize();
	//osg::Matrixd reflectView = osg::Matrixd::lookAt(newEye,newCenter,newUp);

	 osg::Vec3d refEye = eye * mirror;
	 osg::Vec3d refCenter = center * mirror;
	 osg::Vec3d refup =(center+up) * mirror;

	 refup -= refCenter;
	 refup.normalize();

	 osg::Matrixd reflectView = osg::Matrixd::lookAt(refEye,refCenter,refup);
	m_reflectionCamera->setViewMatrix(reflectView);

	m_refractionCamera->setViewMatrix(m_pCamera->getViewMatrix());
	m_refractionCamera->setProjectionMatrix(m_pCamera->getProjectionMatrix());
	m_transform->getOrCreateStateSet()->getUniform("matReflectionModelViewProject")->set(reflectView * m_reflectionCamera->getProjectionMatrix());  
}

osg::ref_ptr<osg::Geometry> WaterEffect::CreateSimpleSurfacePlane(float length, float width)
{
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;  
	osg::ref_ptr<osg::Vec3Array> vArr = new osg::Vec3Array;  
	vArr->push_back(osg::Vec3(-length/2.0,-width/2.0,0.0));  
	vArr->push_back(osg::Vec3(-length/2.0,width/2.0,0.0));  
	vArr->push_back(osg::Vec3(length/2.0,width/2.0,0.0));  
	vArr->push_back(osg::Vec3(length/2.0,-width/2.0,0.0));  
	geom->setVertexArray(vArr);  

	osg::ref_ptr<osg::Vec2Array> texArr = new osg::Vec2Array;  
	texArr->push_back(osg::Vec2(0.0,0.0));  
	texArr->push_back(osg::Vec2(0.0,1.0));  
	texArr->push_back(osg::Vec2(1.0,1.0));  
	texArr->push_back(osg::Vec2(1.0,0.0));  
	geom->setTexCoordArray(0,texArr);  

	osg::ref_ptr<osg::Vec3Array> nArr = new osg::Vec3Array;   
	nArr->push_back(osg::Vec3d(0.0,0.0,1.0));  
	geom->setNormalArray(nArr);  
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));  
	return geom;
}