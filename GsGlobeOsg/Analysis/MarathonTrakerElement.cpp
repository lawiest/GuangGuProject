#include <Analysis/MarathonTrakerElement.h>
#include "osgViewer/Viewer"
#include "osgEarth/Geodata"
#include "tool/CodeTransformHelp.h"
#include "osgEarthSymbology/Style"
#include "osgEarthSymbology/Color" 
#include "tool/GGEarthManipulator.h"
#include "SphereElement.h"
#include "OsgCoreSingleton.h"
#include <osg/Depth>
#include <osg/LineWidth>

class PickSphere : public osgGA::GUIEventHandler
{
public:
	PickSphere(MarathonTrakerElement* ptrMarathon)
	{
		m_ptrCurSphere = NULL;
		m_bIsPickedAndMove = false;
		m_ptrMarathon = ptrMarathon;
	}
	virtual ~PickSphere(){}

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		if (m_ptrMarathon == NULL || !m_ptrMarathon->isFinish())
			return false;

		osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
		if (ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			//如果现在拾取到并且在改变位置
			if (m_bIsPickedAndMove)
			{
				setPosition(ea, aa);
				m_ptrCurSphere = NULL;
				m_bIsPickedAndMove = false;
			}
			else
			{
				osgUtil::LineSegmentIntersector::Intersections intersections;
				view->computeIntersections(ea.getX(), ea.getY(), intersections);

				osgUtil::LineSegmentIntersector::Intersections::iterator hitr;
				for (hitr = intersections.begin(); hitr != intersections.end(); hitr++)
				{
					osg::NodePath nodePath = hitr->nodePath;
					for (int i = 0; i < nodePath.size(); i++)
					{
						osg::ref_ptr<osg::Node> node = nodePath[i];
						m_ptrCurSphere = dynamic_cast<GeoGlobe::SphereElement*>(node.get());

						if (m_ptrCurSphere != NULL)
							return true;
					}
				}
			}
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
		{
			if (m_ptrCurSphere == NULL)
				return false;

			//根据鼠标改变位置
			setPosition(ea, aa);

			return true;
		}

		return false;
	}

	void setPosition(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
		osg::Vec3d world;

		if ( m_ptrMarathon->pick(view, ea, world))
		{		
			m_ptrMarathon->setPosition(m_ptrCurSphere, world);

			//记录是否已经失去到并且在改变位置
			m_bIsPickedAndMove = true;
		}
	}

private:
	osg::ref_ptr<GeoGlobe::SphereElement> m_ptrCurSphere;
	bool m_bIsPickedAndMove;
	osg::ref_ptr<MarathonTrakerElement> m_ptrMarathon;
};

PickSphere * m_pickShpere;

MarathonTrakerElement::MarathonTrakerElement()
{
	m_mapNode = GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode();
	m_bIsFirst = true;
	m_bIsFinished = false;
	m_Distance3D = 0;

	//旋转矩阵
	mt = new osg::MatrixTransform();
	displayGroup = new osg::Group();

	m_mapNode->addChild(displayGroup);
	m_pickShpere = new PickSphere(this);
	//GLOBE_NAME::OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(m_pickShpere);
}

MarathonTrakerElement::~MarathonTrakerElement()
{
	GLOBE_NAME::OsgCoreSingleton::Instance()->OsgViewer()->removeEventHandler(m_pickShpere);
}

double MarathonTrakerElement::getDistance3D()
{
	return m_Distance3D;
}

vector<osg::Vec3d> MarathonTrakerElement::getPickPoints()
{
	return m_vecPickPoints;
}

osg::Node* MarathonTrakerElement::DisplayGroup()
{
	return displayGroup;
}

osgEarth::Annotation::LabelNode* MarathonTrakerElement::CreateLabel(osgEarth::MapNode* m_mapNode, osgEarth::GeoPoint mapPoint, std::string na)
{
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->size() = 18.0f;
	//m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color() = osgEarth::Color::White;
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color() = osgEarth::Color::White;
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->font() = "msyh.ttc";
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	m_SymStyle.getOrCreate<osgEarth::IconSymbol>()->declutter() = false;
	m_SymStyle.getOrCreate<osgEarth::TextSymbol>()->declutter() = false;
	
	m_ptrLabelNode = new osgEarth::Annotation::LabelNode(mapPoint, na, m_SymStyle);
	m_ptrLabelNode->setDynamic(true);
	m_ptrLabelNode->setCullingActive(true);

	return m_ptrLabelNode;
}

bool MarathonTrakerElement::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, osg::Vec3d& world)
{
	osgUtil::LineSegmentIntersector::Intersections intersections;
	view->computeIntersections(ea.getX(), ea.getY(), intersections);

	osg::Matrix inverseMV;
	osg::ref_ptr<osg::Camera> cameraMaster = view->getCamera();
	inverseMV.invert(cameraMaster->getViewMatrix());
	osg::Vec3d eye = osg::Vec3d(0.0, 0.0, 0.0) * inverseMV;

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
			if (node->getName() == "OsgbLayer")
			{
				osg::Vec3d p = hitr->getWorldIntersectPoint();
				//hitr->nodePath();
				double dLenTmp = (eye.x() - p.x())*(eye.x() - p.x())
					+ (eye.y() - p.y())*(eye.y() - p.y())
					+ (eye.z() - p.z())*(eye.z() - p.z());

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

osg::Node* MarathonTrakerElement::createLineM(osg::Vec3d lbegin, osg::Vec3d lend, osg::Vec3d offset)
{
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
	osg::ref_ptr<osg::Geode> Line3D = new osg::Geode;

	Line3D->addDrawable(geom.get());

	geom->setUseDisplayList(false);
	geom->setUseVertexBufferObjects(true);
	geom->setCullingActive(false);

	osg::ref_ptr<osg::Vec3Array> m_ptrVerts = new osg::Vec3Array();
	m_ptrVerts->reserve(2);
	geom->setVertexArray(m_ptrVerts);

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
	//Line3D->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false));
	Line3D->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false), osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
	Line3D->getOrCreateStateSet()->setRenderBinDetails(11, "RenderBin");

	osg::ref_ptr<osg::LineWidth> ptrLineWidth = new osg::LineWidth(9.1f);
	geom->getOrCreateStateSet()->setAttributeAndModes(ptrLineWidth, osg::StateAttribute::ON);

	//Remove all children from this group
	//Line3D->removeChildren(2, Line3D->getNumChildren());

	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
	mt->setMatrix(osg::Matrix::translate(m_vecPickPoints[0]));
	mt->addChild(Line3D);

	return mt.release();
}

bool MarathonTrakerElement::isFinish()
{
	return m_bIsFinished;
}

void MarathonTrakerElement::setLinePosition(int nIndex, int nEndPoint, osg::Vec3d world)
{
	osg::MatrixTransform* ptrMat = dynamic_cast<osg::MatrixTransform*>(m_vecLines[nIndex]);

	osg::Geode* ptrGeode = dynamic_cast<osg::Geode*>(ptrMat->getChild(0));
	osg::Geometry* ptrGeo = ptrGeode->getDrawable(0)->asGeometry();
	osg::Vec3Array*  ptrVectex = dynamic_cast<osg::Vec3Array*>(ptrGeo->getVertexArray());

	(*ptrVectex)[nEndPoint].set(world.x() - m_vecOff.x(), world.y() - m_vecOff.y(), world.z() - m_vecOff.z());

	ptrVectex->dirty();

	UpateLabel(nIndex,(*ptrVectex)[0], (*ptrVectex)[1]);

	//ptrGeo->dirtyDisplayList();
	//ptrGeo->dirtyBound();
}

void MarathonTrakerElement::UpateLabel(int nIndex, osg::Vec3d start, osg::Vec3d end)
{
	double dbDistance = (start-end).length();
	std::ostringstream s1;
	s1 << setiosflags(ios::fixed) << setprecision(2) << dbDistance << "米" << "\n";
	std::string strText = CodeHelp::String_To_UTF8(s1.str());

	m_vecLabel[nIndex + 1]->setText(strText);

}

void MarathonTrakerElement::setPosition(GeoGlobe::SphereElement* ptrSphere, osg::Vec3d world)
{
	osgEarth::GeoPoint mapPoint;
	mapPoint.fromWorld(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getTerrain()->getSRS(), world);


	std::vector<GeoGlobe::SphereElement* >::iterator it = std::find(m_vecShpere.begin(), m_vecShpere.end(), ptrSphere);

	if (it == m_vecShpere.end())
		return;

	ptrSphere->setPosition(mapPoint);

	if (it != m_vecShpere.end())
	{
		int nIndex = it - m_vecShpere.begin();
		if (nIndex >= 0)
			m_vecLabel[nIndex]->setPosition(mapPoint);

		if (nIndex == 0)
		{
			setLinePosition(0, 0, world);
		}
		else if (nIndex == m_vecShpere.size()-1)
		{
			setLinePosition(nIndex - 1, 1, world);
		}
		else
		{
			setLinePosition(nIndex - 1, 1, world);
			setLinePosition(nIndex , 0, world);
		}

	}

}

bool MarathonTrakerElement::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (m_bIsFinished)
		return false;

	osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
	
	int size;
	std::ostringstream s1;
	osgEarth::GeoPoint mapPoint;

	if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		if (!pick(view, ea, m_vecWorld))
			return false;

		//线的起点
		if (m_bIsFirst)
		{	
			m_vecOff = m_vecWorld;
			m_vecPickPoints.push_back(m_vecWorld);
			size = m_vecPickPoints.size();

			RubberBand = createLineM(m_vecPickPoints[m_vecPickPoints.size() - 1], m_vecWorld, m_vecPickPoints[0]);
			displayGroup->addChild(RubberBand);
			
			//距离标牌
			std::string strText = CodeHelp::String_To_UTF8("起点");
			mapPoint.fromWorld(m_mapNode->getTerrain()->getSRS(), m_vecWorld);
			osgEarth::Annotation::LabelNode* ptrLabel = CreateLabel(m_mapNode, mapPoint, strText);
			m_vecLabel.push_back(ptrLabel);
			displayGroup->addChild(ptrLabel);

			//编辑点
			osg::ref_ptr<GeoGlobe::SphereElement> ptr = new GeoGlobe::SphereElement(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode());
			ptr->setPosition(mapPoint);
			ptr->setSize(10);
			displayGroup->addChild(ptr);
			m_vecShpere.push_back(ptr.get());

			m_bIsFirst = false;
			return  true;
		}
		else
		{
			m_vecPickPoints.push_back(m_vecWorld);
			size = m_vecPickPoints.size();
			
			//画线，使用相对位置，再偏移至绝对位置，数值小，线不抖
			osg::Node* ptrLine3D = createLineM(m_vecPickPoints[size - 2], m_vecPickPoints[size - 1], m_vecPickPoints[0]);
			m_vecLines.push_back(ptrLine3D);
			displayGroup->addChild(ptrLine3D);

			//计算三维距离
			m_Distance3D += (m_vecPickPoints[size - 2]-m_vecPickPoints[size - 1]).length();
			
			//添加标签,设置有效位		
			s1 << setiosflags(ios::fixed) << setprecision(2) << m_Distance3D << "米" << "\n";
			std::string strText = CodeHelp::String_To_UTF8(s1.str());
			mapPoint.fromWorld(m_mapNode->getTerrain()->getSRS(), m_vecWorld);
			osgEarth::Annotation::LabelNode* ptrLabel = CreateLabel(m_mapNode, mapPoint, strText);
			m_vecLabel.push_back(ptrLabel);
			displayGroup->addChild(ptrLabel);

			//编辑点
			osg::ref_ptr<GeoGlobe::SphereElement> ptr = new GeoGlobe::SphereElement(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode());
			ptr->setPosition(mapPoint);
			ptr->setSize(10);
			displayGroup->addChild(ptr);
			m_vecShpere.push_back(ptr.get());

			//删除橡皮筋对象
			displayGroup->removeChild(RubberBand);
			
		}
	}
	else if(ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
	{
		if (!m_bIsFirst)
		{
			if (!pick(view, ea, m_vecWorld))
				return false;
			
			displayGroup->removeChild(RubberBand);
			RubberBand = createLineM(m_vecPickPoints[m_vecPickPoints.size() - 1], m_vecWorld, m_vecPickPoints[0]);
			displayGroup->addChild(RubberBand);
		}
	}
	else if(ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK)
	{
		m_bIsFinished = true;
		displayGroup->removeChild(RubberBand);
	}
	return false;
}
