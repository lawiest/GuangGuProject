#include "Analysis/BirdsEyeElement.h"
#include <osgDB/ReaderWriter>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/LineWidth>
using namespace osgEarth;

GeoGlobe::BirdsEyeElement::BirdsEyeElement(GGEarthManipulator* earthpulator,std::string strImgPath,double dblXmin,double dblXmax, double dblYmin,double dblYmax)
{
	m_bVisible = true;

	setNumChildrenRequiringEventTraversal( 1 );
	m_earthpulator = earthpulator;

	// set the projection matrix
	setProjectionMatrix(osg::Matrix::ortho2D(0,300,0,200));
	// set the view matrix
	setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	setViewMatrix(osg::Matrix::identity());
	// only clear the depth buffer
	setClearMask(GL_DEPTH_BUFFER_BIT);
	// draw subgraph after main camera view.
	setRenderOrder(osg::Camera::POST_RENDER);
	// we don't want the camera to grab event focus from the viewers main camera(s).
	setAllowEventFocus(false);

	// 增加HUD相机的渲染内容，即map
	osg::ref_ptr<osg::Geode> pGeode = new osg::Geode;

	// 创建纹理
	osg::ref_ptr<osg::Geometry> pGeometry = new osg::Geometry;
	{
		// 顶点
		osg::ref_ptr<osg::Vec3Array> verArray = new osg::Vec3Array;
		verArray->push_back(osg::Vec3(0, 0, -1));
		verArray->push_back(osg::Vec3(300, 0, -1));
		verArray->push_back(osg::Vec3(300, 200, -1));
		verArray->push_back(osg::Vec3(0, 200, -1));
		pGeometry->setVertexArray(verArray);
		// 纹理坐标
		osg::ref_ptr<osg::Vec2Array> textArray = new osg::Vec2Array;
		textArray->push_back(osg::Vec2(0, 0));
		textArray->push_back(osg::Vec2(1, 0));
		textArray->push_back(osg::Vec2(1, 1));
		textArray->push_back(osg::Vec2(0, 1));
		pGeometry->setTexCoordArray(0, textArray);
		// 法线，指向用户，即y轴
		osg::ref_ptr < osg::Vec3Array> normal = new osg::Vec3Array;
		normal->push_back(osg::Vec3(0, 1, 0));
		pGeometry->setNormalArray(normal);
		pGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
		// 指定绘制方式
		pGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
		// 纹理图片
		osg::ref_ptr<osg::Image> image = osgDB::readImageFile(strImgPath);
		if (image)
		{
			osg::ref_ptr<osg::Texture2D> text2D = new osg::Texture2D;
			text2D->setImage(image);
			text2D->setDataVariance(osg::Object::DYNAMIC);

			pGeometry->getOrCreateStateSet()->setTextureAttributeAndModes(
				0, text2D, osg::StateAttribute::ON);
		}
	}
	pGeode->addDrawable(pGeometry);
	addChild(pGeode);

	//边框
	osg::ref_ptr<osg::Geode>pGeodeBorder = new osg::Geode;
	osg::ref_ptr<osg::Vec3Array> verBorder = new osg::Vec3Array;
	osg::ref_ptr<osg::Geometry> pGeoBorder= new osg::Geometry;
	{
		verBorder->push_back(osg::Vec3(1.0, 1.0, 0));
		verBorder->push_back(osg::Vec3(300 - 1.0, 1.0, 0));
		verBorder->push_back(osg::Vec3(300 - 1.0, 200 - 1.0, 0));
		verBorder->push_back(osg::Vec3(1.0, 200 - 1.0, 0));
	}
	pGeoBorder->setVertexArray(verBorder);
	osg::ref_ptr<osg::Vec3Array> normalBorder = new osg::Vec3Array;
	normalBorder->push_back(osg::Vec3(0, 0, 1));
	pGeoBorder->setNormalArray(normalBorder, osg::Array::BIND_OVERALL);
	osg::ref_ptr<osg::Vec4Array> colorBorder = new osg::Vec4Array;
	colorBorder->push_back(osg::Vec4(0.0, 0.0, 0.5, 1));
	pGeoBorder->setColorArray(colorBorder, osg::Array::BIND_OVERALL);
	pGeoBorder->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, 4));

	pGeodeBorder->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	pGeodeBorder->getOrCreateStateSet()->setAttribute(new osg::LineWidth(1), osg::StateAttribute::ON);
	pGeodeBorder->addDrawable(pGeoBorder);
	addChild(pGeodeBorder);

	//十字丝
	m_pGeodeCross = new osg::Geode;
	m_pVertexCross = new osg::Vec3dArray;
	osg::ref_ptr<osg::Geometry> pGeometryCross = new osg::Geometry;
	// 使用VBO，每帧实时的修改数据
	pGeometryCross->setUseVertexBufferObjects(true);
	pGeometryCross->setVertexArray(m_pVertexCross);
	osg::ref_ptr<osg::Vec3Array> normalCross = new osg::Vec3Array;
	normalCross->push_back(osg::Vec3(0, 0, 1));
	pGeometryCross->setNormalArray(normalCross, osg::Array::BIND_OVERALL);
	osg::ref_ptr<osg::Vec4Array> colorCross = new osg::Vec4Array;
	colorCross->push_back(osg::Vec4(1.0, 1.0, 1.0, 1));
	pGeometryCross->setColorArray(colorCross, osg::Array::BIND_OVERALL);
	pGeometryCross->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 4));

	m_pGeodeCross->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	m_pGeodeCross->getOrCreateStateSet()->setAttribute(new osg::LineWidth(0.5), osg::StateAttribute::ON);
	m_pGeodeCross->addDrawable(pGeometryCross);
	addChild(m_pGeodeCross);

	//十字丝中心方框
	m_pGeodeCrossQuad = new osg::Geode;
	m_pVertexCrossQuad = new osg::Vec3dArray;
	osg::ref_ptr<osg::Geometry> pGeometryCrossQuad = new osg::Geometry;
	// 使用VBO，每帧实时的修改数据
	pGeometryCrossQuad->setUseVertexBufferObjects(true);
	pGeometryCrossQuad->setVertexArray(m_pVertexCrossQuad);
	osg::ref_ptr<osg::Vec3Array> normalQuad = new osg::Vec3Array;
	normalQuad->push_back(osg::Vec3(0, 0, 1));
	pGeometryCrossQuad->setNormalArray(normalQuad, osg::Array::BIND_OVERALL);
	osg::ref_ptr<osg::Vec4Array> colorCrossQuad = new osg::Vec4Array;
	colorCrossQuad->push_back(osg::Vec4(1.0, 1.0, 0.0, 1));
	pGeometryCrossQuad->setColorArray(colorCrossQuad, osg::Array::BIND_OVERALL);
	pGeometryCrossQuad->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, 4));

	m_pGeodeCrossQuad->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	m_pGeodeCrossQuad->getOrCreateStateSet()->setAttribute(new osg::LineWidth(0.5), osg::StateAttribute::ON);
	m_pGeodeCrossQuad->addDrawable(pGeometryCrossQuad);
	addChild(m_pGeodeCrossQuad);
}
GeoGlobe::BirdsEyeElement:: ~BirdsEyeElement()
{

}

void GeoGlobe::BirdsEyeElement::traverse(osg::NodeVisitor& nv)
{
	if (!m_bVisible)
		return;
	
	if (nv.getVisitorType() == osg::NodeVisitor::EVENT_VISITOR)
	{
		osgGA::EventVisitor* ev = static_cast<osgGA::EventVisitor*>(&nv);
		for(osgGA::EventQueue::Events::iterator itr = ev->getEvents().begin();
			itr != ev->getEvents().end();
			++itr)
		{
			osgGA::GUIEventAdapter* ea = dynamic_cast<osgGA::GUIEventAdapter*>(itr->get());
			if ( ea && handle(*ea, *(ev->getActionAdapter())))
				ea->setHandled(true);
		}
	}
	osg::Camera::traverse(nv);
}

bool GeoGlobe::BirdsEyeElement::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if ( ea.getEventType() == osgGA::GUIEventAdapter::FRAME && m_earthpulator.valid())
	{
		Viewpoint vpoint = m_earthpulator->getViewpoint();
		osg::Vec3d mapPos = osg::Vec3d(vpoint.focalPoint()->x(),vpoint.focalPoint()->y(),0);

		//经纬度是-180~180，-90~90，保证算出的是0~360和0~180
		double x = (mapPos.x() + 180.0) * 300 / 360.0;
		double y = (mapPos.y() + 90.0) * 200 / 180.0;

		// 更新缓冲区数据
		m_pVertexCross->clear();
		int nCross = 500;
		m_pVertexCross->push_back(osg::Vec3d(x, y - nCross, 0));
		m_pVertexCross->push_back(osg::Vec3d(x, y + nCross, 0));
		m_pVertexCross->push_back(osg::Vec3d(x - nCross, y, 0));
		m_pVertexCross->push_back(osg::Vec3d(x + nCross, y, 0));
		m_pVertexCross->dirty();

		// 更新缓冲区数据
		m_pVertexCrossQuad->clear();
		int nCrossQuad = 10;
		m_pVertexCrossQuad->push_back(osg::Vec3d(x + nCrossQuad, y - nCrossQuad, 0));
		m_pVertexCrossQuad->push_back(osg::Vec3d(x + nCrossQuad, y + nCrossQuad, 0));
		m_pVertexCrossQuad->push_back(osg::Vec3d(x - nCrossQuad, y + nCrossQuad, 0));
		m_pVertexCrossQuad->push_back(osg::Vec3d(x - nCrossQuad, y - nCrossQuad, 0));
		m_pVertexCrossQuad->dirty();
	}
	return false;
}

void GeoGlobe::BirdsEyeElement::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;
}

GeoGlobe::ImageNode::ImageNode(const char * strImgFile)
{
	setNumChildrenRequiringEventTraversal( 1 );
	osg::ref_ptr<osg::Image> image = osgDB::readImageFile(strImgFile);
	//int n3 = image->get
	// set the projection matrix
	setProjectionMatrix(osg::Matrix::ortho2D(0,image->s(),0,image->t()));
	//setProjectionMatrix(osg::Matrix::ortho2D(400,700,300,500));
	// set the view matrix
	setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	setViewMatrix(osg::Matrix::identity());
	// only clear the depth buffer
	setClearMask(GL_DEPTH_BUFFER_BIT);
	// draw subgraph after main camera view.
	setRenderOrder(osg::Camera::POST_RENDER);
	// we don't want the camera to grab event focus from the viewers main camera(s).
	setAllowEventFocus(false);

	// 增加HUD相机的渲染内容，即map
	osg::ref_ptr<osg::Geode> pGeode = new osg::Geode;

	// 创建纹理
	osg::ref_ptr<osg::Geometry> pGeometry = new osg::Geometry;
	{
		// 顶点
		osg::ref_ptr<osg::Vec3Array> verArray = new osg::Vec3Array;
		verArray->push_back(osg::Vec3(0, 0, -1));
		verArray->push_back(osg::Vec3(image->s(), 0, -1));
		verArray->push_back(osg::Vec3(image->s(), image->t(), -1));
		verArray->push_back(osg::Vec3(0, image->t(), -1));
		/*verArray->push_back(osg::Vec3(400, 300, -1));
		verArray->push_back(osg::Vec3(700, 300, -1));
		verArray->push_back(osg::Vec3(700, 500, -1));
		verArray->push_back(osg::Vec3(400, 500, -1));*/
		pGeometry->setVertexArray(verArray);
		// 纹理坐标
		osg::ref_ptr<osg::Vec2Array> textArray = new osg::Vec2Array;
		textArray->push_back(osg::Vec2(0, 0));
		textArray->push_back(osg::Vec2(1, 0));
		textArray->push_back(osg::Vec2(1, 1));
		textArray->push_back(osg::Vec2(0, 1));
		pGeometry->setTexCoordArray(0, textArray);
		// 法线，指向用户，即y轴
		osg::ref_ptr < osg::Vec3Array> normal = new osg::Vec3Array;
		normal->push_back(osg::Vec3(0, 1, 0));
		pGeometry->setNormalArray(normal);
		pGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
		// 指定绘制方式
		pGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
		// 纹理图片
		
		if (image)
		{
			osg::ref_ptr<osg::Texture2D> text2D = new osg::Texture2D;
			text2D->setImage(image);
			text2D->setDataVariance(osg::Object::DYNAMIC);

			pGeometry->getOrCreateStateSet()->setTextureAttributeAndModes(
				0, text2D, osg::StateAttribute::ON);
		}
	}
	pGeode->addDrawable(pGeometry);
	addChild(pGeode);
}