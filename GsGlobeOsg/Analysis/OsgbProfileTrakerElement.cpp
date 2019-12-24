#include "OsgbProfileTrakerElement.h"
#include "osgViewer/Viewer"
#include "osgEarth/GeoData"
#include "tool/GGEarthManipulator.h"
#include "OsgCoreSingleton.h"
#include <tool/CodeTransformHelp.h>
#include <osg/LineWidth>
#include <osg/Depth>
#include <osgEarth/GeoTransform>
#include <osgUtil/Tessellator>
#include <osgUtil/DelaunayTriangulator>

OsgbProfileTrakerElement::OsgbProfileTrakerElement(osg::Node* ptrNode, bool bAutoCalc, double dbStep )
{
	m_osgbLayer = ptrNode;
	m_mapNode = GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode();
	m_bIsFirst = true;
	m_bIsFinished = false;
	m_ptrDisplayGroup = new osg::Group();
	m_bIsAutoCalc = bAutoCalc;
	m_isDrag = false;
	m_dbStep = dbStep;

	GLOBE_NAME::OsgCoreSingleton::Instance()->Root()->addChild(m_ptrDisplayGroup);
}

OsgbProfileTrakerElement::~OsgbProfileTrakerElement()
{

}

osg::Node* OsgbProfileTrakerElement::DisplayGroup()
{
	return m_ptrDisplayGroup;
}

bool OsgbProfileTrakerElement::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, osg::Vec3d& world)
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
			if (node->getName() == "OsgbLayer")
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

std::vector<osg::Vec3d> OsgbProfileTrakerElement::getPickPoints()
{
	return m_PickPoints;
}

osg::Node* OsgbProfileTrakerElement::createLine(osg::Vec3d lbegin, osg::Vec3d lend, osg::Vec3d offset)
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
	geom->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false), osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
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

	osg::ref_ptr<osg::MatrixTransform> ptrMatrix = new osg::MatrixTransform();
	ptrMatrix->setMatrix(osg::Matrix::translate(m_PickPoints[0]));
	ptrMatrix->addChild(Line3D);

	return ptrMatrix.release();
}

osg::Vec3d OsgbProfileTrakerElement::PickOsgb(osg::Vec3d point)
{
	osgViewer::Viewer* ptrView = GLOBE_NAME::OsgCoreSingleton::Instance()->OsgViewer();

	osg::Matrix VPW = ptrView->getCamera()->getViewMatrix() *
		ptrView->getCamera()->getProjectionMatrix() *
		ptrView->getCamera()->getViewport()->computeWindowMatrix();

	osg::ref_ptr<osgEarth::GeoTransform> ptrLocator = dynamic_cast<osgEarth::GeoTransform*>(m_osgbLayer.get());
	osg::Matrixd outMatrix;
	outMatrix = ptrLocator->getMatrix();

	osg::Vec3d worldpick = osg::Vec3d(0.0,0.0,0.0);

	osg::Vec3d world = point * outMatrix;
	osg::Vec3d window = world * VPW;
	osg::Vec3d up = world;

	up.normalize();

	double segOffset = 50000;
	osg::Vec3d start = world + (up * segOffset);
	osg::Vec3d end = world - (up * segOffset);

	float local_x, local_y;
	const osg::Camera* camera = ptrView->getCamera();//GLOBE_NAME::OsgCoreSingleton::Instance()->OsgViewer()->getCameraContainingPosition(window.x(), window.y(), local_x, local_y);

	osg::ref_ptr< osgUtil::LineSegmentIntersector > picker = new osgUtil::LineSegmentIntersector(start, end);
	osgUtil::IntersectionVisitor iv(picker.get());
	iv.setTraversalMask(0xffffffff);

	const_cast<osg::Camera*>(camera)->accept(iv);

	double dLen2Longest = 0.0;
	bool bFind = false;
	osgUtil::LineSegmentIntersector::Intersections intersections;
	if (picker->containsIntersections())
	{
		intersections = picker->getIntersections();
		osgUtil::LineSegmentIntersector::Intersections::iterator hitr, hitrShortest;
		for (hitr = intersections.begin(); hitr != intersections.end(); hitr++)
		{
			osg::NodePath nodePath = hitr->nodePath;
			for (int i = 0; i < nodePath.size(); i++)
			{
				osg::ref_ptr<osg::Node> node = nodePath[i];
				if (node == NULL)
					continue;

				if (node->getName() == "OsgbLayer")
				{
					osg::Vec3d p = hitr->getWorldIntersectPoint();

					if (dLen2Longest < p.z())
					{
						dLen2Longest = p.z();
						hitrShortest = hitr;
						bFind = true;
					}
				}
			}
		}

		if (bFind == true)
		{
			worldpick = hitrShortest->getWorldIntersectPoint();
			//m_ptrRootGroup->addChild(CreateNode(world));
		}
	}

	return worldpick;
}

void OsgbProfileTrakerElement::Calculate(double dbStep)
{
	osg::ref_ptr<osgEarth::GeoTransform> ptrLocator = dynamic_cast<osgEarth::GeoTransform*>(m_osgbLayer.get());
	osg::Matrixd outMatrix;

	//首先将点还原到倾斜局部坐标
	std::vector<osg::Vec3d> vecPoints;
	if (ptrLocator)
	{
		outMatrix = ptrLocator->getMatrix();
		osg::Matrix mt_inverse;
		mt_inverse = mt_inverse.inverse(outMatrix);

		for (int i = 0; i < m_PickPoints.size(); i++)
		{
			osg::Vec3d vec = m_PickPoints[i] * mt_inverse;
			vecPoints.push_back(vec);
		}
	}

	//确定最小点和方向
	osg::Vec3d dir;
	osg::Vec3d min;
	if (vecPoints[0].x() >= vecPoints[1].x())
	{
		dir = vecPoints[0] - vecPoints[1];
		min = vecPoints[1];
	}
	else
	{
		dir = vecPoints[1] - vecPoints[0];
		min = vecPoints[0];
	}
	dir.normalize();

	double len = (vecPoints[1] - vecPoints[0]).length();
	double part = len / dbStep;

	//每个点和倾斜求交
	std::vector<osg::Vec3d> vecPicked;
	for (int i = 0; i <= dbStep; i++)
	{
		osg::Vec3d vecP = PickOsgb(min + dir*part*i);
		vecPicked.push_back(vecP);
	}

	//移除之前的线
	m_ptrDisplayGroup->removeChildren(0, m_ptrDisplayGroup->getNumChildren());

	//判断偏移点
	osg::Vec3d offset;
	if (min == vecPoints[0])
		offset = vecPicked[0];
	else
		offset = vecPicked[vecPicked.size() - 1];

	double dbSum = 0.0;
	osgEarth::GeoPoint geoPoint;
	//第一个距离为0;
	m_vecDistance.push_back(0.0);
	
	for (int j = 0; j < vecPicked.size() - 1; j++)
	{		
		//计算每段距离累加和
		dbSum += (vecPicked[j] - vecPicked[j + 1]).length();
		m_vecDistance.push_back(dbSum);

		//求得海拔高度
		geoPoint.fromWorld(m_mapNode->getMapSRS(),vecPicked[j]);
		m_vecElevation.push_back(geoPoint.z());

		//将交到的点连成线画出来
		osg::Node* ptrLine3D = createLine(vecPicked[j], vecPicked[j+1], offset);
		m_ptrDisplayGroup->addChild(ptrLine3D);
	}
	//最后一个点的海拔
	geoPoint.fromWorld(m_mapNode->getMapSRS(), vecPicked[vecPicked.size() - 1]);
	m_vecElevation.push_back(geoPoint.z());

	OnCalcFinish(m_vecDistance, m_vecElevation);
}

bool OsgbProfileTrakerElement::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	if (m_bIsFinished)
		return false;

	osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
	osg::Group* root = static_cast<osg::Group*>(view->getSceneData());
	int size;
	std::ostringstream s1;
	osgEarth::GeoPoint mapPoint;

	if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		if (m_isDrag)
		{
			m_isDrag = false;
			return true;
		}

		//线的起点
		if (m_bIsFirst)
		{
			if (!pick(view, ea, m_world))
				return false;

			m_PickPoints.push_back(m_world);
			size = m_PickPoints.size();

			m_bIsFirst = false;

			return false;
		}
		else
		{
			m_PickPoints.push_back(m_world);
			size = m_PickPoints.size();
			//画线，使用相对位置，再偏移至绝对位置，数值小，线不抖
			osg::Node* ptrLine3D = createLine(m_PickPoints[size - 2], m_PickPoints[size - 1], m_PickPoints[0]);
			m_ptrDisplayGroup->addChild(ptrLine3D);
			
			//删除橡皮筋对象
			m_ptrDisplayGroup->removeChild(m_ptrRubberBand);
			m_ptrDisplayGroup->removeChild(m_closeRubberBand);

			if (m_PickPoints.size() >= 2)
			{
				if (m_bIsAutoCalc)
				{	
					Calculate(m_dbStep);
				}
			
				m_bIsFinished = true;
				OnDrawFinish();
			}
		}
	}
	else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
	{
		if (!m_bIsFirst)
		{
			if (!pick(view, ea, m_world))
				return false;

			m_ptrDisplayGroup->removeChild(m_ptrRubberBand);
			m_ptrDisplayGroup->removeChild(m_closeRubberBand);

			m_ptrRubberBand = createLine(m_PickPoints[m_PickPoints.size() - 1], m_world, m_PickPoints[0]);
			m_closeRubberBand = createLine(m_PickPoints[0], m_world, m_PickPoints[0]);

			m_ptrDisplayGroup->addChild(m_ptrRubberBand);
			m_ptrDisplayGroup->addChild(m_closeRubberBand);
		}
	}
	else if (ea.getEventType() == osgGA::GUIEventAdapter::DRAG)
	{
		m_isDrag = true;
	}
	return false;
}