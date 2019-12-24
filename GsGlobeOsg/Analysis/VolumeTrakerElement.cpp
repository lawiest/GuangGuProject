#include "VolumeTrakerElement.h"
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

//计算土石方体积和面积
const float z_min = -9999.0;
const float z_err = -99999.0;

struct OsgbTracker
{
	double          m_xMin, m_yMin; //矩形范围
	double          m_xMax, m_yMax;

	void*           m_user;
	void(*m_fun)(void*, float* arry9);

	void OnTrack(float* arry9)
	{
		m_fun(m_user, arry9);
	}

	//判断节点是否在范围内
	bool is_out(const osg::BoundingSphere& boundSphere)
	{
		float x, y, r;
		x = boundSphere.center().x();
		y = boundSphere.center().y();
		r = boundSphere.radius();

		if (x + r < m_xMin || x - r > m_xMax || y + r < m_yMin || y - r > m_yMax)
			return true;

		return false;
	}

	//遍历paglod
	void TrackPagedLod(osg::PagedLOD& lod)
	{
		int i, k; 
		std::string strPath, strFileName;
		osg::ref_ptr<osg::Node> ptrNode;

		int nNumFiles = lod.getNumFileNames();
		int nNumChild = lod.getNumChildren();
		strPath = lod.getDatabasePath();

		//先从文件装入
		for (k = 0, i = nNumFiles; --i > 0;)
		{
			if (i < nNumChild)
			{
				ptrNode = lod.getChild(i);
			}
			else
			{
				strFileName = strPath + lod.getFileName(nNumChild);
				ptrNode = osgDB::readNodeFile(strFileName);
			}

			if (ptrNode != 0)
				++k;

			Track(ptrNode);
		}

		if (k > 0)
			return;

		ptrNode = lod.getChild(0);
		Track(ptrNode);
	}

	void Track(osg::Node* ptrNode)
	{
		if (ptrNode == NULL)
			return;

		if (is_out(ptrNode->getBound()))
			return;

		osg::Geometry* ptrGeo;
		osg::PagedLOD* ptrPagLod;
		osg::Group* ptrGroup;
		int n;

		ptrGeo = dynamic_cast<osg::Geometry*>(ptrNode);

		if (ptrGeo != NULL)
		{
			for (n = ptrGeo->getNumPrimitiveSets(); --n >= 0;)
				Track(*ptrGeo->getVertexArray(), *ptrGeo->getPrimitiveSet(n));
			return;
		}

		ptrPagLod = dynamic_cast<osg::PagedLOD*>(ptrNode);

		if (ptrPagLod != NULL)
		{
			TrackPagedLod(*ptrPagLod);
			return;
		}

		ptrGroup = dynamic_cast<osg::Group*>(ptrNode);
		if (ptrGroup != 0)
		{
			for (n = ptrGroup->getNumChildren(); --n >= 0;)
				Track(ptrGroup->getChild(n));

			return;
		}
	}

	template<class type>
	bool TrackTemplate(type* ptrArry, osg::PrimitiveSet& primitive)
	{
		if (primitive.getMode() != osg::PrimitiveSet::TRIANGLES)
			return false;

		//索引个数
		int n = primitive.getNumIndices();

		float arry[9], *ptr;
		type point;

		for (int i = 0; i < n;)
		{
			//根据索引取定点
			point = ptrArry[primitive.index(i)];
			//数组指针每次移三个单位
			ptr = arry + (i % 3) * 3;

			ptr[0] = point[0];
			ptr[1] = point[1];
			ptr[2] = point[2];

			//存满三个点就去求Z值操作
			if (((++i) % 3) == 0)
				OnTrack(arry);
		}
		return true;
	}

	bool Track(osg::Array& ptrArry, osg::PrimitiveSet& primitive)
	{
		const void* ptr = ptrArry.getDataPointer();
		switch (ptrArry.getType())
		{
		case osg::Array::Vec3ArrayType:
			return TrackTemplate((osg::Vec3*)ptr, primitive);
		case osg::Array::Vec4ArrayType:
			return TrackTemplate((osg::Vec4*)ptr, primitive);
		case osg::Array::Vec3dArrayType:
			return TrackTemplate((osg::Vec3d*)ptr, primitive);
		case osg::Array::Vec4dArrayType:
			return TrackTemplate((osg::Vec4d*)ptr, primitive);
		default:
			return false;
		}
	}

	//行列式
	static double _del(double m10, double m11, double m12, double m20, double m21, double m22)
	{
		return m11*m22 + m12*m20 + m10*m21 - m11*m20 - m10*m22 - m12*m21;
	}

	//通过平面方程一般式求落在三角形内部点的的Z值
	static double  Z(double x, double y, const float* array9)
	{
		double d, x1, y1, x2, y2, x3, y3, u, v, w;
		x1 = array9[0]; y1 = array9[1];
		x2 = array9[3]; y2 = array9[4];
		x3 = array9[6]; y3 = array9[7];

		d = _del(x1, x2, x3, y1, y2, y3);

		if (fabs(d) < FLT_EPSILON) 
			return z_err;

		u = _del(x, x2, x3, y, y2, y3);
		v = _del(x1, x, x3, y1, y, y3);
		w = _del(x1, x2, x, y1, y2, y);

		u /= d;
		v /= d;
		w /= d;

		if (u < 0 || v < 0 || w < 0)
			return z_err;

		d = 1.0 / (u + v + w);
		u *= d;
		v *= d;
		w *= d;
		return u*array9[2] + v*array9[5] + w*array9[8];
	}

	static void CalculateZ(double x, double y, const float* arry9, float* z)
	{
		double t = Z(x, y, arry9);

		if (t > *z)
			*z = (float)t;
	}
};

struct ZGrid
{
	int             m_width, m_height;   //格网的像素宽度和高度 通过构造函数

	double          m_xMin, m_yMin; 
	double          m_xMax, m_yMax;
	double          m_dx, m_dy; //格网单像素的大小
	float*          m_Z;        //Z值矩阵 规格为[m_w*m_h]

	ZGrid()
	{
		memset(this, 0, sizeof(*this));
	}

	~ZGrid()
	{
		delete m_Z;
	}

	//获得高度平均值
	double GetAva()
	{
		double sum, n;
		
		float z;
		sum = n = 0;

		for (int i = m_width*m_height; --i >= 0;)
		{
			z = m_Z[i];
			if (z <= z_min)
				continue;

			sum += z;
			++n;
		}
		return sum / n;
	}

	//获得体积,fill为填方,cut为挖方
	void GetVolume(double z, double& cut, double& fill)
	{
		double v;
		int i;
		cut = fill = 0;

		for (i = m_width*m_height; --i >= 0;)
		{
			v = m_Z[i];

			if (v <= z_min)
				continue;

			if (v > z)
				cut += v - z;

			if (v < z)
				fill += z - v;
		}

		v = m_dx*m_dy;
		cut *= v;
		fill *= v;
	}

	//通过所给的x和y的像素单位求Z
	void CalculateZ(int ix, int iy, float* arry9)
	{
		double x, y;
		float* z;

		z = m_Z + iy*m_width + ix;

		if (*z == z_err)
			return;

		y = m_yMin + iy*m_dy;
		x = m_xMin + ix*m_dx;

		//通过真实x和y求z
		OsgbTracker::CalculateZ(x, y, arry9, z);
	}

	void FindMinMax(double& min, double& max, double x)
	{
		if (x < min)
			min = x;

		if (x > max)
			max = x;
	}

	//写一个三角形内部的Z值
	void cover(float* arry9)
	{
		int i, j, xMin, yMin, xMax, yMax;
		double minMaxArry[4];

		minMaxArry[0] = minMaxArry[2] = arry9[0];
		minMaxArry[1] = minMaxArry[3] = arry9[1];

		//找到这三个点最大和最小x,y
		for (i = 3; --i > 0;)
		{
			FindMinMax(minMaxArry[0], minMaxArry[2], arry9[3 * i + 0]);
			FindMinMax(minMaxArry[1], minMaxArry[3], arry9[3 * i + 1]);
		}

		xMin = (int)((minMaxArry[0] - m_xMin) / m_dx);
		xMax = (int)((minMaxArry[2] - m_xMin) / m_dx);
		yMin = (int)((minMaxArry[1] - m_yMin) / m_dy);
		yMax = (int)((minMaxArry[3] - m_yMin) / m_dy);

		if (xMin < 0)
			xMin = 0;

		if (yMin < 0)
			yMin = 0;

		if (xMax >= m_width)
			xMax = m_width - 1;

		if (yMax >= m_height)
			yMax = m_height - 1;

		for (i = xMin; i <= xMax; ++i)
		{
			for (j = yMin; j <= yMax; ++j)
			{
				CalculateZ(i, j, arry9);
			}
		}
	}

	void Reset(osg::Geometry* ptrGeo)
	{
		osg::Vec3Array* ptrVertices = dynamic_cast<osg::Vec3Array*>(ptrGeo->getVertexArray());

		m_xMin = m_xMax = (*ptrVertices)[0].x();
		m_yMin = m_yMax = (*ptrVertices)[0].y();

		osg::Vec3Array::iterator it = ptrVertices->begin();
		for (it; it != ptrVertices->end(); it++)
		{
			FindMinMax(m_xMin, m_xMax, (*it).x());
			FindMinMax(m_yMin, m_yMax, (*it).y());
		}

		m_dx = m_xMax - m_xMin;
		m_dy = m_yMax - m_yMin;

		double d = sqrt(m_dx*m_dy)*0.001;
		m_width = (int)ceil(m_dx / d);
		m_height = (int)ceil(m_dy / d);

		m_Z = new float[m_width*m_height];
		m_dx = m_dy = d;

		//将Z填充为最小值
		std::fill_n(m_Z, m_width*m_height, z_min);

		std::vector<float> vecTriangle;
		osg::ref_ptr<osgUtil::DelaunayTriangulator> trig = new osgUtil::DelaunayTriangulator();
		trig->setInputPointArray(ptrVertices);
		trig->triangulate();
		osg::DrawElementsUInt* ptrIndex = trig->getTriangles();
		GLenum model = ptrIndex->getMode();

		if (model != osg::PrimitiveSet::TRIANGLES)
			return ;

		int nCnt = ptrIndex->getNumIndices();
		for (int k = 0; k < ptrIndex->getNumIndices(); k++)
		{
			osg::Vec3 vetex = (*ptrVertices)[ptrIndex->index(k)];
			vecTriangle.push_back(vetex.x());
			vecTriangle.push_back(vetex.y());
			vecTriangle.push_back(vetex.z());
		}

		for (int i = 0; i < vecTriangle.size(); i += 9)
		{
			cover(&vecTriangle[i]);
		}
	}

	void SetZerr()
	{
		for (int i = m_width*m_height; --i >= 0;)
		{
			if (m_Z[i] == z_min)
				m_Z[i] = z_err;
			else
				m_Z[i] = z_min;
		}
	}

};

static void GridGetZ(void* user, float* arry9)
{
	((ZGrid*)user)->cover(arry9);
}

//计算土石方体积和面积
//zz[0]返回边界面的平均高度,zz[1]返回内部区域的平均高度
void Calculate(osg::Node* ptrOsgbNode,osg::Geometry* ptrGeo, double dbElevation, double* dbResult)
{
	ZGrid grid;
	OsgbTracker tracker;

	grid.Reset(ptrGeo);
	dbResult[0] = grid.GetAva();
	grid.SetZerr();

	tracker.m_xMin = grid.m_xMin;
	tracker.m_xMax = grid.m_xMax;
	tracker.m_yMin = grid.m_yMin;
	tracker.m_yMax = grid.m_yMax;

	tracker.m_user = &grid;
	tracker.m_fun = GridGetZ;
	tracker.Track(ptrOsgbNode);
	dbResult[1] = grid.GetAva();
	grid.GetVolume(dbElevation, dbResult[2], dbResult[3]);
}


OsgbCalculateThread::OsgbCalculateThread()
{

}

void OsgbCalculateThread::run()
{
	if (m_parameter.m_ptrOsgbNode == NULL)
		return;

	double* result = new double[4];
	Calculate(m_parameter.m_ptrOsgbNode, m_parameter.m_ptrGeo, m_parameter.m_dbElevation, result);

	onDone(result[0], result[1], result[2], result[3]);

	delete []result;
}

OsgbCalculateThread::~OsgbCalculateThread()
{

}

void OsgbCalculateThread::setParameter(const OsgbCalcParameter& parameter)
{
	m_parameter = parameter;
}

void VolumeTrakerElement::OnCalcDone(double aveHeightLine, double aveHeightFace, double cut, double fill)
{
	OnCalcFinish(cut);
	std::string strCut = CodeHelp::String_To_UTF8(std::to_string(cut)+"立方米");
	m_ptrBillboard->setText(strCut);
}

VolumeTrakerElement::VolumeTrakerElement(osg::Node* ptrNode, bool bAutoCalc, double dbElevation)
{
	m_ptrCalcThread = NULL;
	m_osgbLayer = ptrNode;
	m_mapNode = GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode();
	m_bIsFirst = true;
	m_bIsFinished = false;
	m_Area3D = 0.0;
	m_ptrDisplayGroup = new osg::Group();
	m_dbElevation = dbElevation;
	m_bIsAutoCalc = bAutoCalc;
	m_isDrag = false;

	GLOBE_NAME::OsgCoreSingleton::Instance()->Root()->addChild(m_ptrDisplayGroup);
}

VolumeTrakerElement::~VolumeTrakerElement()
{
}

osg::Node* VolumeTrakerElement::DisplayGroup()
{
	return m_ptrDisplayGroup;
}

bool VolumeTrakerElement::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, osg::Vec3d& world)
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

double VolumeTrakerElement::getArea3D()
{
	return m_Area3D;
}

std::vector<osg::Vec3d> VolumeTrakerElement::getPickPoints()
{
	return m_PickPoints;
}

osg::Node* VolumeTrakerElement::createLine(osg::Vec3d lbegin, osg::Vec3d lend, osg::Vec3d offset)
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

void VolumeTrakerElement::Calculate(double dbElevation)
{
	osg::ref_ptr<osgEarth::GeoTransform> ptrLocator = dynamic_cast<osgEarth::GeoTransform*>(m_osgbLayer.get());
	osg::Matrixd outMatrix;

	osg::Geometry* linesGeom = new osg::Geometry();
	osg::Vec3Array* ptrVertices = new osg::Vec3Array(m_PickPoints.size());
	if (ptrLocator)
	{
		outMatrix = ptrLocator->getMatrix();

		osg::Matrix mt_inverse;
		mt_inverse = mt_inverse.inverse(outMatrix);

		for (int i = 0; i < m_PickPoints.size(); i++)
		{
			osg::Vec3d vec = m_PickPoints[i] * mt_inverse;
			(*ptrVertices)[i].set(vec);
		}
	}

	osg::ref_ptr<osg::DrawArrays> ptrDrawArray = new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, ptrVertices->size());
	linesGeom->setVertexArray(ptrVertices);
	linesGeom->addPrimitiveSet(ptrDrawArray);

	//double* result = new double[4];
	//Calculate(ptrLocator->getChild(0), linesGeom, 0, result);

	OsgbCalcParameter parameter;
	parameter.m_ptrGeo = linesGeom;
	parameter.m_ptrOsgbNode = ptrLocator->getChild(0);
	parameter.m_dbElevation = dbElevation;

	if (m_ptrCalcThread == NULL)
	{
		m_ptrCalcThread = new OsgbCalculateThread();
		m_ptrCalcThread->setParameter(parameter);
		m_ptrCalcThread->start();
		m_ptrCalcThread->onDone.Add(this, &VolumeTrakerElement::OnCalcDone);
	}
	else
	{
		m_ptrCalcThread->setParameter(parameter);
		m_ptrCalcThread->start();
	}
}

bool VolumeTrakerElement::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
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
			m_ptrDisplayGroup->addChild(ptrLine3D);
			
			//删除橡皮筋对象
			m_ptrDisplayGroup->removeChild(m_ptrRubberBand);
			m_ptrDisplayGroup->removeChild(m_closeRubberBand);
		}

		m_isDrag = false;
	}
	else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
	{
		if (!m_bIsFirst)
		{
			if (!pick(view, ea, world))
				return false;

			m_ptrDisplayGroup->removeChild(m_ptrRubberBand);
			m_ptrDisplayGroup->removeChild(m_closeRubberBand);

			m_ptrRubberBand = createLine(m_PickPoints[m_PickPoints.size() - 1], world, m_PickPoints[0]);
			m_closeRubberBand = createLine(m_PickPoints[0], world, m_PickPoints[0]);

			m_ptrDisplayGroup->addChild(m_ptrRubberBand);
			m_ptrDisplayGroup->addChild(m_closeRubberBand);
		}
	}
	else if (ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK)
	{
		m_isDrag = false;

		pick(view, ea, world);

		//面积闭合
		osg::Node* ptrLine3D = createLine(m_PickPoints[0], m_PickPoints[m_PickPoints.size() - 1], m_PickPoints[0]);
		m_ptrDisplayGroup->addChild(ptrLine3D);

		if (m_bIsAutoCalc)
		{
			//开始计算
			Calculate(m_dbElevation);
		}

		//添加标签,设置有效位		
		s1 << std::setiosflags(std::ios::fixed) << std::setprecision(2) << "计算中，请稍后。。。" << "\n";

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
		//chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color() = Color::White;
		style.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color() = osgEarth::Color::White;
		style.getOrCreate<osgEarth::Symbology::TextSymbol>()->font() = "msyh.ttc";
		style.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
		//style.getOrCreate<osgEarth::IconSymbol>()->declutter() = false;
		//style.getOrCreate<osgEarth::TextSymbol>()->declutter() = false;
		m_ptrBillboard = new GeoGlobe::PlaceName::GeoBillboard(osgEarth::GeoPoint(m_mapNode->getMapSRS(), mapPoint), strText, style, true);
		m_ptrBillboard->setDynamic(true);
		m_ptrBillboard->setCullingActive(false);
		m_ptrDisplayGroup->addChild(m_ptrBillboard);


		m_bIsFinished = true;
		m_ptrDisplayGroup->removeChild(m_ptrRubberBand);
		m_ptrDisplayGroup->removeChild(m_closeRubberBand);

		OnDrawFinish();
	}
	else if (ea.getEventType() == osgGA::GUIEventAdapter::DRAG)
	{
		m_isDrag = true;
	}
	return false;
}