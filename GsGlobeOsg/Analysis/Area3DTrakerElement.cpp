#include "Area3DTrakerElement.h"


#include "osgViewer/Viewer"
#include "osgEarth/Geodata"
#include "tool/CodeTransformHelp.h"
#include "osgEarthSymbology/Style"
#include "osgEarthSymbology/Color" 
#include "tool/GGEarthManipulator.h"
#include "OsgCoreSingleton.h"
#include "placename/GeoBillboard.h"

#define maxsize 255

Area3DTrakerElement::Area3DTrakerElement(osgEarth::MapNode* mapNode)// , GeoGlobe::IConsultationWindow* consultationWnd)
{
	//m_consultationWnd = consultationWnd;
	m_mapNode = mapNode;
	m_bIsFirst = true;
	m_PlaceNode = NULL;
	m_bIsFinished = false;
	m_isDrag = false;

	mt = new osg::MatrixTransform();
	displayGroup = new osg::Group();

	GLOBE_NAME::OsgCoreSingleton::Instance()->Root()->addChild(displayGroup);
}

Area3DTrakerElement::~Area3DTrakerElement()
{
}

osg::Node* Area3DTrakerElement::DisplayGroup()
{
	return displayGroup;
}

//叉乘算三角形面积(有正负)
void Area3DTrakerElement::AreaMeasurement(osg::Vec3d pickP1, osg::Vec3d pickP2, osg::Vec3d pickP3, double& area)
{
	osg::Vec3d P12 = pickP2 - pickP1;
	osg::Vec3d P13 = pickP3 - pickP1;
	double x1 = P12.x();
	double y1 = P12.y();
	double z1 = P12.z();
	double x2 = P13.x();
	double y2 = P13.y();
	double z2 = P13.z();

	//向量叉乘
	area = (y1*z1 - y2*z1) + (z1*x2 - z2*x1) + (x1*y2 - x2*y1);
	area = 0.5*area;

	return;
}

//所有点连成的多边形面积
bool Area3DTrakerElement::AreaMeasurement(std::vector<osg::Vec3d> pickPoints, double& area)
{
	int count = pickPoints.size();
	if (count < 3)
		return false;

	double acc;
	area = 0;
	std::vector<double> m_area;
	for (int i = 1; i < count - 1; i++)
	{
		acc = 0;
		AreaMeasurement(pickPoints[0], pickPoints[i], pickPoints[i + 1], acc);
		m_area.push_back(acc);
	}
	int num = m_area.size();
	for (int i = 0; i < num; i ++)
	{
		area = area + m_area[i];
	}
	area = abs(area);
	return true;;
}

osgEarth::Annotation::LabelNode* Area3DTrakerElement::CreateLabel(osgEarth::MapNode* m_mapNode, osgEarth::GeoPoint mapPoint, std::string na)
{
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->size() = 18.0f;
	//m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color() = Color::White;
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color() = Color::White;
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->font() = "msyh.ttc";
	m_SymStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	//m_SymStyle.getOrCreate<osgEarth::IconSymbol>()->declutter() = false;
	//m_SymStyle.getOrCreate<osgEarth::TextSymbol>()->declutter() = false;

	m_ptrLabelNode = new osgEarth::Annotation::LabelNode(mapPoint, na, m_SymStyle);
	m_ptrLabelNode->setDynamic(true);
	m_ptrLabelNode->setCullingActive(true);

	return m_ptrLabelNode;
}

bool Area3DTrakerElement::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, osg::Vec3d& world)
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

double Area3DTrakerElement::getArea3D()
{
	return m_Area3D;
}

vector<osg::Vec3d> Area3DTrakerElement::getPickPoints()
{
	return m_PickPoints;
}

osg::Node* Area3DTrakerElement::createLine(osg::Vec3d lbegin, osg::Vec3d lend, osg::Vec3d offset)
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
	Vec3d dv0 = lbegin - offset;
	Vec3d dv1 = lend - offset;

	m_ptrVerts->push_back(dv0);
	m_ptrVerts->push_back(dv1);

	geom->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, m_ptrVerts->size()));
	Line3D->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	//Remove all children from this group
	//Line3D->removeChildren(2, Line3D->getNumChildren());

	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
	mt->setMatrix(Matrix::translate(m_PickPoints[0]));
	mt->addChild(Line3D);

	return mt.release();
}



//三维面积量算
//测试算法
//叉乘算三角形面积(有正负)
void AreaMeasurement(osg::Vec2d pickP1, osg::Vec2d pickP2, osg::Vec2d pickP3, double& area)
{
	osg::Vec2d P12 = pickP2 - pickP1;
	osg::Vec2d P13 = pickP3 - pickP1;
	double x1 = P12.x();
	double y1 = P12.y();
	double x2 = P13.x();
	double y2 = P13.y();

	//向量叉乘
	area = (x1*y2 - x2*y1);
	area = 0.5*area;

	return;
}

//所有点连成的多边形面积(2维)
bool AreaMeasurement(std::vector<osg::Vec2d> pickPoints, double& area)
{
	int count = pickPoints.size();
	if (count < 3)
		return false;

	double acc;
	area = 0;
	std::vector<double> m_area;
	for (int i = 1; i < count - 1; i++)
	{
		acc = 0;
		AreaMeasurement(pickPoints[0], pickPoints[i], pickPoints[i + 1], acc);
		m_area.push_back(acc);
	}
	int num = m_area.size();
	for (int i = 0; i < num; i++)
	{
		area = area + m_area[i];
	}
	area = abs(area);
	return true;
}


//以拟合平面方法求三维面积
int det(int x1, int y1, int x2, int y2)
{
	return x1*y2 - x2*y1;
}

//int cross(point A, point B, point C, point D) 
int cross(osg::Vec2d A, osg::Vec2d B, osg::Vec2d C, osg::Vec2d D)
{
	return det(B.x() - A.x(), B.y() - A.y(), D.x() - C.x(), D.y() - C.y());
}

//冒泡排序,从小到大排序
vector<osg::Vec3d> BubbleSort(vector<osg::Vec3d> ary)
{
	int length = ary.size();
	int i, j;
	osg::Vec3d tmp;
	for (i = 0; i < length - 1; i++)
	{
		tmp = ary[i];
		for (j = length - 1; j > i; j--)
		{
			//找到数组中最小的数，并交换
			if (tmp.z() > ary[j].z())
			{
				ary[i] = ary[j];
				ary[j] = tmp;
				tmp = ary[i];
			}
		}
	}
	return ary;
}

//计算凸包点
void cullConvexHullPoints(vector<osg::Vec2d> pointsInOrder, vector<osg::Vec2d> &result)
{
	pointsInOrder.push_back(pointsInOrder[0]);
	pointsInOrder.push_back(pointsInOrder[1]);

	int size = pointsInOrder.size();

	result.push_back(pointsInOrder[0]);
	for (int i = 0; i < size - 3; i++)
	{
		result.push_back(pointsInOrder[i + 1]);
		int tSize = result.size();
		int m = cross(result[tSize - 2], result[tSize - 1], result[tSize - 1], pointsInOrder[i + 2]);
		if (m < 0 && pointsInOrder[i + 1].x() != 0 && pointsInOrder[i + 1].y() != 0)
			result.pop_back();
	}
	return;
}

//根据tan值给点排序
void PointsInAngleAscendOrder(vector<osg::Vec3d> positiveAnglePoints, vector<osg::Vec3d> negativeAnglePoints, vector<osg::Vec2d> &pointsInOrder)
{
	vector<osg::Vec3d> pointsAngleCheck;
	int positiveSize = positiveAnglePoints.size();
	int negativeSize = negativeAnglePoints.size();
	for (int i = 0; i < positiveSize; i++)
	{
		osg::Vec2d temp;
		temp.x() = positiveAnglePoints[i].x();
		temp.y() = positiveAnglePoints[i].y();
		pointsInOrder.push_back(temp);
		pointsAngleCheck.push_back(positiveAnglePoints[i]);
	}
	for (int i = 0; i < negativeSize; i++)
	{
		osg::Vec2d temp;
		temp.x() = negativeAnglePoints[i].x();
		temp.y() = negativeAnglePoints[i].y();
		pointsInOrder.push_back(temp);
		pointsAngleCheck.push_back(negativeAnglePoints[i]);
	}
	return;
}

//无序点生成凸包多边形并计算面积（Graham's Scan）
void GrahamConvexHull(const vector<osg::Vec2d> m_Points, vector<osg::Vec2d> &result)
{
	if (m_Points.size() < 3)
		return;

	//将所有点按逆时针顺序排好
	//存储每个点距中心点的角度
	//vector<double> tanAngle;
	int m_size = m_Points.size();

	//设置中心点
	//将第二个轴数值最小的点设置为中心点
	osg::Vec2d centerPoint = m_Points[0];
	for (int i = 0; i < m_size; i++)
	{
		if (m_Points[i].y() < centerPoint.y())
			centerPoint = m_Points[i];
	}

	vector<osg::Vec3d>  positiveAnglePoints;
	vector<osg::Vec3d>  negativeAnglePoints;
	//计算各点与中心点夹角tan值
	for (int i = 0; i < m_size; i++)
	{
		osg::Vec3d temp;
		temp.x() = m_Points[i].x();
		temp.y() = m_Points[i].y();
		if (m_Points[i].x() == centerPoint.x() && m_Points[i].y() == centerPoint.y())
			temp.z() = 0;
		else
		{
			double tan_angle = (m_Points[i].y() - centerPoint.y()) / (m_Points[i].x() - centerPoint.x());
			temp.z() = tan_angle;
		}
		if (temp.z() >= 0)
			positiveAnglePoints.push_back(temp);
		else  negativeAnglePoints.push_back(temp);
	}

	//按夹角tan值大小对点进行排序（注意tan正负值与角度大小的关系，正负先分开排序）
	positiveAnglePoints = BubbleSort(positiveAnglePoints);
	negativeAnglePoints = BubbleSort(negativeAnglePoints);

	//将所有点按距中心点的角度从小到大排序
	vector<osg::Vec2d> pointsInOrder;
	PointsInAngleAscendOrder(positiveAnglePoints, negativeAnglePoints, pointsInOrder);

	//排序好的点计算凸包
	cullConvexHullPoints(pointsInOrder, result);

	return;
}

void calArea3D(vector<osg::Vec3d> m_PickPoints,double &area)
{
	int size = m_PickPoints.size();
	vector<osg::Vec2d> pointsXY;
	vector<osg::Vec2d> pointsXZ;
	vector<osg::Vec2d> pointsYZ;
	for (int i = 0; i < size; i++)
	{
		osg::Vec2d temp;
		temp.x() = m_PickPoints[i].x() - m_PickPoints[0].x();
		temp.y() = m_PickPoints[i].y() - m_PickPoints[0].y();
		pointsXY.push_back(temp);
	}
	for (int i = 0; i < size; i++)
	{
		osg::Vec2d temp;
		temp.x() = m_PickPoints[i].x() - m_PickPoints[0].x();
		temp.y() = m_PickPoints[i].z() - m_PickPoints[0].z();
		pointsXZ.push_back(temp);
	}
	for (int i = 0; i < size; i++)
	{
		osg::Vec2d temp;
		temp.x() = m_PickPoints[i].y() - m_PickPoints[0].y();
		temp.y() = m_PickPoints[i].z() - m_PickPoints[0].z();
		pointsYZ.push_back(temp);
	}

	vector<osg::Vec2d> resultXY;
	double areaXY;
	GrahamConvexHull(pointsXY, resultXY);
	AreaMeasurement(resultXY, areaXY);

	vector<osg::Vec2d> resultXZ;
	double areaXZ;
	GrahamConvexHull(pointsXZ, resultXZ);
	AreaMeasurement(resultXZ, areaXZ);

	vector<osg::Vec2d> resultYZ;
	double areaYZ;
	GrahamConvexHull(pointsYZ, resultYZ);
	AreaMeasurement(resultYZ, areaYZ);

	area = sqrt((areaXY * areaXY + areaXZ * areaXZ + areaYZ * areaYZ) / 3 );

	return;
}


//测试算法
//三维面积量算

bool Area3DTrakerElement::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
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
			if (!pick(view, ea, world))
				return false;

			m_PickPoints.push_back(world);
			size = m_PickPoints.size();

			/*	
			std::string strText = CodeHelp::String_To_UTF8("起点");
			mapPoint.fromWorld(m_mapNode->getTerrain()->getSRS(), world);
			displayGroup->addChild(CreateLabel(m_mapNode, mapPoint, strText));		
			*/

			m_bIsFirst = false;
			m_isDrag = false;
			return false;
		}
		else
		{
			m_PickPoints.push_back(world);
			size = m_PickPoints.size();
			//画线，使用相对位置，再偏移至绝对位置，数值小，线不抖
			osg::Node* ptrLine3D = createLine(m_PickPoints[size - 2], m_PickPoints[size - 1], m_PickPoints[0]);
			displayGroup->addChild(ptrLine3D);
			
			//删除橡皮筋对象
			displayGroup->removeChild(RubberBand);
		    displayGroup->removeChild(m_closeRubberBand);
		}
		m_isDrag = false;
	}
	else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
	{
		if (!m_bIsFirst)
		{
			if (!pick(view, ea, world))
				return false;

			displayGroup->removeChild(RubberBand);
			displayGroup->removeChild(m_closeRubberBand);
			RubberBand = createLine(m_PickPoints[m_PickPoints.size() - 1], world, m_PickPoints[0]);

			m_closeRubberBand = createLine(m_PickPoints[0], world, m_PickPoints[0]);
			displayGroup->addChild(RubberBand);
			displayGroup->addChild(m_closeRubberBand);
		}
	}
	else if (ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK)
	{
		//面积闭合
		osg::Node* ptrLine3D = createLine(m_PickPoints[0], m_PickPoints[m_PickPoints.size() - 1], m_PickPoints[0]);
		displayGroup->addChild(ptrLine3D);

		m_isDrag = false;

		//计算三维距离
		//1 三角形剖分
		if (!AreaMeasurement(m_PickPoints, m_Area3D))
			return false;
		////2 平面拟合
		//double Area;
		//calArea3D(m_PickPoints,Area);

		//3 直接去掉Z值
		std::vector<osg::Vec2d> Points;
		double area;
		//AreaMeasurement(Points, area);(未写成成员函数)

		OnFinish(m_Area3D);

		//添加标签,设置有效位
		if (m_Area3D > 100000)
		{
			double showValue = m_Area3D / 1000000;
			s1 << setiosflags(ios::fixed) << setprecision(2) << showValue << "平方公里" << "\n";
		}
		else
			s1 << setiosflags(ios::fixed) << setprecision(2) << m_Area3D << "平方米" << "\n";

		osg::Vec3d labelPos;
		for (int i = 0;i<m_PickPoints.size();i++)
		{
			labelPos += m_PickPoints[i];
		}
		labelPos = labelPos / m_PickPoints.size();
		mapPoint.fromWorld(m_mapNode->getTerrain()->getSRS(),labelPos);

		std::string strText = CodeHelp::String_To_UTF8(s1.str());
		osgEarth::Symbology::Style style;
		style.getOrCreate<osgEarth::Symbology::TextSymbol>()->size() = 18.0f;
		//style.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color() = Color::Red;
		style.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color() = Color::White;
		style.getOrCreate<osgEarth::Symbology::TextSymbol>()->font() = "msyh.ttc";
		style.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
		osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> billboard = new GeoGlobe::PlaceName::GeoBillboard(GeoPoint(m_mapNode->getMapSRS(), mapPoint), strText, style, true);
		billboard->setCullingActive(false);


		displayGroup->addChild(/*CreateLabel(m_mapNode, mapPoint, strText)*/billboard);


		////对比，用2公式计算的
		//std::ostringstream s2;
		//s2<< setiosflags(ios::fixed) << setprecision(2) << Area << "平方米====" << "\n";
		//std::string strText2 = CodeHelp::String_To_UTF8(s2.str());
		//osg::Vec3d labelPos2;
		//labelPos2.x() = labelPos.x() + 20;
		//labelPos2.y() = labelPos.y() + 20;
		//labelPos2.z() = labelPos.z() ;
		//osgEarth::GeoPoint mapPoint2;
		//mapPoint2.fromWorld(m_mapNode->getTerrain()->getSRS(), labelPos2);
		//displayGroup->addChild(CreateLabel(m_mapNode, mapPoint2, strText2));


		m_bIsFinished = true;
		displayGroup->removeChild(RubberBand);
		displayGroup->removeChild(m_closeRubberBand);
	}
	else if (ea.getEventType() == osgGA::GUIEventAdapter::DRAG)
	{
		m_isDrag = true;
	}

	return false;
}