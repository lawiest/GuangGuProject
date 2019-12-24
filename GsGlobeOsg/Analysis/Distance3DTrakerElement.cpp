#include <Analysis/Distance3DTrakerElement.h>
#include "osgViewer/Viewer"
#include "osgEarth/Geodata"
#include "tool/CodeTransformHelp.h"
#include "osgEarthSymbology/Style"
#include "osgEarthSymbology/Color" 
#include "tool/GGEarthManipulator.h"
#include "SphereElement.h"
#include "OsgCoreSingleton.h"
#include "FormHook.h"

Distance3DTrakerElement::Distance3DTrakerElement()
{
	m_mapNode = GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode();
	m_bIsFirst = true;
	m_bIsFinished = false;
	m_isDrag = false;
	m_Distance3D = 0;

	//旋转矩阵
	mt = new osg::MatrixTransform();
	displayGroup = new osg::Group();

	GLOBE_NAME::OsgCoreSingleton::Instance()->Root()->addChild(displayGroup);
}

Distance3DTrakerElement::~Distance3DTrakerElement()
{
}

double Distance3DTrakerElement::getDistance3D()
{
	return m_Distance3D;
}

vector<osg::Vec3d> Distance3DTrakerElement::getPickPoints()
{
	return m_PickPoints;
}

osg::Node* Distance3DTrakerElement::DisplayGroup()
{
	return displayGroup;
}

GeoGlobe::Annotation::PlaceNode* Distance3DTrakerElement::CreateLabel(osgEarth::MapNode* m_mapNode, osgEarth::GeoPoint mapPoint, std::string strText)
{
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->size() = 18.0f;
	//m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color() = osgEarth::Color::White;
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color() = osgEarth::Color::White;
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->font() = "msyh.ttc";
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	m_SymStyle.getOrCreate<osgEarth::IconSymbol>()->declutter() = false;
	m_SymStyle.getOrCreate<osgEarth::TextSymbol>()->declutter() = false;
	
	m_ptrLabelNode = new GeoGlobe::Annotation::PlaceNode(mapPoint, strText, m_SymStyle);
	m_ptrLabelNode->setDynamic(true);
	m_ptrLabelNode->setCullingActive(true);

	return m_ptrLabelNode;
}

bool Distance3DTrakerElement::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, osg::Vec3d& world)
{
	osgUtil::LineSegmentIntersector::Intersections intersections;
	view->computeIntersections(ea.getX(), ea.getY(), intersections);

	osg::Matrix _inverseMV;
	osg::ref_ptr<osg::Camera> cameraMaster = view->getCamera();
	_inverseMV.invert(cameraMaster->getViewMatrix());
	osg::Vec3d ptEye = osg::Vec3d(0.0, 0.0, 0.0) * _inverseMV;

	double dLen2Shortest = DBL_MAX;

	bool bFind = false;

	osgUtil::LineSegmentIntersector::Intersections::iterator hitr, hitrShortest;
	for (hitr = intersections.begin(); hitr != intersections.end(); hitr++)
	{
		// the geodes are identified by name.
		osg::NodePath nodePath = hitr->nodePath;
		for (int i = 0; i < nodePath.size(); i++)
		{
			osg::ref_ptr<osg::Node> node = nodePath[i];
			if (node == NULL)
				continue;
			//if (node->getName() == "3DMeasureEnable")
			{
				osg::Vec3d p = hitr->getWorldIntersectPoint();
				//hitr->nodePath();
				double dLenTmp = (ptEye.x() - p.x())*(ptEye.x() - p.x())
					+ (ptEye.y() - p.y())*(ptEye.y() - p.y())
					+ (ptEye.z() - p.z())*(ptEye.z() - p.z());

				if (dLen2Shortest > dLenTmp)
				{
					dLen2Shortest = dLenTmp;
					hitrShortest = hitr;
					bFind = true;
				}
			}
		}
	}
	if (bFind == true)
	{
		world = hitrShortest->getWorldIntersectPoint();
		//m_ptrRootGroup->addChild(CreateNode(world));
	}
	return bFind;
}

osg::Node* Distance3DTrakerElement::createLine(osg::Vec3d lbegin, osg::Vec3d lend, osg::Vec3d offset)
{
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
	osg::ref_ptr<osg::Geode> Line3D = new osg::Geode;

	Line3D->addDrawable(geom.get());

	geom->setUseDisplayList(false);
	geom->setUseVertexBufferObjects(true);

	osg::ref_ptr<osg::Vec3Array> m_ptrVerts = new osg::Vec3Array();
	m_ptrVerts->reserve(2);
	geom->setVertexArray(m_ptrVerts);
	geom->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	geom->getOrCreateStateSet()->setRenderBinDetails(11, "RenderBin");

	//颜色初始化
	osg::Vec4Array* colors = new osg::Vec4Array();
	colors->reserve(2);
	geom->setColorArray(colors);
	colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	//顶点初始化
	osg::Vec3d dv0 = lbegin - offset;
	osg::Vec3d dv1 = lend - offset;

	m_ptrVerts->push_back(dv0);
	m_ptrVerts->push_back(dv1);

	geom->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, m_ptrVerts->size()));
	Line3D->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	//Remove all children from this group
	//Line3D->removeChildren(2, Line3D->getNumChildren());

	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
	mt->setMatrix(osg::Matrix::translate(m_PickPoints[0]));
	mt->addChild(Line3D);

	return mt.release();
}


bool Distance3DTrakerElement::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (m_bIsFinished)
		return false;

	osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
	
	int size;
	std::ostringstream s1;
	osgEarth::GeoPoint mapPoint;

	if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		if (m_isDrag)
		{
			m_isDrag = false;
			return true;
		}

		if (!pick(view, ea, world))
		{
			return false;
			m_isDrag = false;
		}

		//线的起点
		if (m_bIsFirst)
		{		
			m_PickPoints.push_back(world);
			size = m_PickPoints.size();

			RubberBand = createLine(m_PickPoints[m_PickPoints.size() - 1], world, m_PickPoints[0]);
			displayGroup->addChild(RubberBand);

			std::string strText = CodeHelp::String_To_UTF8("起点");
			mapPoint.fromWorld(m_mapNode->getTerrain()->getSRS(), world);
			displayGroup->addChild(CreateLabel(m_mapNode, mapPoint, strText));

			m_isDrag = false;
			m_bIsFirst = false;
			return false;
		}
		else
		{
			m_PickPoints.push_back(world);
			size = m_PickPoints.size();
				
			//画线，使用相对位置，再偏移至绝对位置，数值小，线不抖
			osg::Node* ptrLine3D = createLine(m_PickPoints[size - 2], m_PickPoints[size - 1], m_PickPoints[0]);
			displayGroup->addChild(ptrLine3D);

			//计算三维距离
			m_Distance3D += (m_PickPoints[size - 2]- m_PickPoints[size - 1]).length();
			
			OnFinish(m_Distance3D);

			//添加标签,设置有效位		
			s1 << setiosflags(ios::fixed) << setprecision(2) << m_Distance3D << "米" << "\n";
			std::string strText = CodeHelp::String_To_UTF8(s1.str());
			mapPoint.fromWorld(m_mapNode->getTerrain()->getSRS(), world);
			displayGroup->addChild(CreateLabel(m_mapNode, mapPoint, strText));

			//删除橡皮筋对象
			displayGroup->removeChild(RubberBand);
			m_isDrag = false;
		}		
		m_isDrag = false;
	}
	else if(ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
	{
		if (!m_bIsFirst)
		{
			if (!pick(view, ea, world))
				return false;
			
			displayGroup->removeChild(RubberBand);
			RubberBand = createLine(m_PickPoints[m_PickPoints.size() - 1], world, m_PickPoints[0]);
			displayGroup->addChild(RubberBand);
		}
	}
	else if(ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK)
	{
		m_isDrag = false;
		m_bIsFinished = true;
		displayGroup->removeChild(RubberBand);
	}
	else if (ea.getEventType() == osgGA::GUIEventAdapter::DRAG)
	{
		m_isDrag = true;
	}
	return false;
}
