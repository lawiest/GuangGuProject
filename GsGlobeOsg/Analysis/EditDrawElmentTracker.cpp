#include "EditDrawElmentTracker.h"
#include "globe/Element.h"
#include "GeoMapTools.h"
#include "globe/BillBoardElement.h"
#include "globe/GeoBillBoardElement.h"
#include "globe/LinePictureElement.h"
#include "globe/PolygonSymbolElement.h"
#include <osg/Depth>
#include "OsgCoreSingleton.h"
#include <osg/LineWidth>

using namespace GeoStar::Kernel::Globe;
namespace GeoGlobe
{

	osg::Vec4f ToColor(osg::Vec4i& color)
	{
		return osg::Vec4f(color.x() / 255.f, color.y() / 255.f, color.z() / 255.f, color.w() / 255.f);
	}


	EditDrawElmentTracker::EditDrawElmentTracker(GeoMapTool* mapNode)
	{
		m_ptrMapTool = mapNode;
		m_ptrMapTool->SetMonitorMouseMove(true);
		 m_CurElement=NULL;//当前编辑的element
		 m_CurrentNodeIndex=-1;//当前选中的节点
		 m_DrawMode=0;//编辑模式或绘制模式
		 m_GeomType=1;// 1 是点，2 是线 ，3 是面
		 m_LineColor = osg::Vec4i(255, 255, 0, 255);// 线的颜色
		 m_PointColor = osg::Vec4i(0, 0, 255, 255);//点的颜色
		 m_CurrPointColor = osg::Vec4i(0, 255, 40, 255);// 点高亮颜色
		 m_pointSize = 8;// 点的大小
		 m_CurrPointSize = 12;//高亮点的大小
	}
	
	EditDrawElmentTracker::~EditDrawElmentTracker()
	{

	}

	bool EditDrawElmentTracker::SetEditElement(GLOBE_NAME::Element * edit)
	{
		if (!edit)
			return false;
		if ((edit->Type()&ElementGeoType::Point) == ElementGeoType::Point)
		{
			double x, y, z;
			edit->getPosition(x, y, z);
			m_point.push_back(KERNEL_NAME::GsRawPoint3D(x, y, z));
			m_GeomType = 1;
		}
		else if (edit->Type()==ElementType::eLinePictureElement3D)
		{
			LinePictureElement3D* line = dynamic_cast<LinePictureElement3D*>(edit);
			m_point = line->GetPoints();
			m_GeomType = 2;
		}
		else if (edit->Type() == ElementType::ePolygonSymbolElement)
		{
			PolygonSymbolElement* polygon = dynamic_cast<PolygonSymbolElement*>(edit);
			m_point = polygon->getPoints();
			m_GeomType = 2;
		}
		else {
			return false;
		}
		m_CurElement = edit;
		// 绘制编辑的线
		if(m_GeomType==2)
		DrawLineFeature();
		// 添加绘制的节点
		DrawPointNode();
		
		return true;
	}
	void EditDrawElmentTracker::CancelEdit()
	{
		if (m_CurElement == NULL)
			return;
		m_CurElement = NULL;//当前编辑的element
		m_CurrentNodeIndex = -1;//当前选中的节点
		m_point.clear();// 节点的清楚
		// 节点清除
		for (int i = 0, size = m_NodePoints.size(); i < size; i++)
		{
			removeChild(m_NodePoints[i]);
		}
		m_NodePoints.clear();
		//线的清除
		if (m_GeomType == 2)
		{
			m_feature->getGeometry()->clear();
			m_featureNode->init();
		}
		// 面的清楚
	}
	int EditDrawElmentTracker::GetNodeSize()
	{
		return m_point.size();
	}
	int EditDrawElmentTracker::GetDrawMode()
	{
		return 0;
	}
	// 判断是否点中一个节点，并处理节点高亮显示
	int EditDrawElmentTracker::HitNode(osg::Node * node)
	{
		for (int i = 0, size = m_NodePoints.size(); i < size; i++)
		{
			if (m_NodePoints[i] == node)//
			{
				SetCurrNodeIndex(i);
				return i;
			}
		}
		return -1;
	}
	int EditDrawElmentTracker::GetCurrentIndex()
	{
		return m_CurrentNodeIndex;
	}
	bool EditDrawElmentTracker::AddPoint(const KERNEL_NAME::GsRawPoint3D & p)
	{
		// 点类型不能插入点
		if (m_GeomType == 1)
		{
			return false;
		}
		if (m_CurrentNodeIndex > -1 )
		{
			bool isInter= InsertPoint(p, m_CurrentNodeIndex + 1);
			SetCurrNodeIndex(m_CurrentNodeIndex + 1);
			return isInter;
		}
		//默认添加的结尾
		m_point.push_back(p);
		// 顶点的创建
		osg::ref_ptr<SphereElement> pNode = CreatePointNode(p);
		m_NodePoints.push_back(pNode);
		addChild(pNode);
		//线的位置更新；
		if (m_GeomType == 2)
		{
			(m_feature->getGeometry())->push_back(osg::Vec3d(p.X, p.Y, p.Z));
			m_featureNode->init();
		}
		// 面的节点更新的过程

		return true;
	}
	bool EditDrawElmentTracker::InsertPoint(const KERNEL_NAME::GsRawPoint3D & p, int index)
	{
		m_point.insert(m_point.begin() + index, p);
		osg::ref_ptr<SphereElement> pNode = CreatePointNode(p);
		m_NodePoints.insert(m_NodePoints.begin() + index, pNode);
		addChild(pNode);
		//线的节点添加
		if (m_GeomType == 2)
		{
			osgEarth::Symbology::Geometry *geom = (m_feature->getGeometry());
			geom->insert(geom->begin() + index, osg::Vec3d(p.X, p.Y, p.Z));
			m_featureNode->init();
		}
		return true;
	}
	void EditDrawElmentTracker::MovePoint(const KERNEL_NAME::GsRawPoint3D & p, int index)
	{
		if (m_CurrentNodeIndex < 0)
			return;
		int i = m_CurrentNodeIndex;
		m_point[i] += p;
		m_NodePoints[i]->setPosition(osgEarth::GeoPoint(getMapNode()->getMapSRS(), m_point[i].X, m_point[i].Y, m_point[i].Z));
		// 线的坐标更新
		if (m_GeomType == 2)
		{
			osgEarth::Symbology::Geometry *geom = (m_feature->getGeometry());
			(*geom)[i]= osg::Vec3d(m_point[i].X, m_point[i].Y, m_point[i].Z);
			m_featureNode->init();
		}

	}
	bool EditDrawElmentTracker::DeleCurNode()
	{
		if (m_CurrentNodeIndex < 0 || m_point.size() == 1)
			return false;

		int deleIndex = m_CurrentNodeIndex;
		SetCurrNodeIndex(-1);
		// 删除节点
		m_point.erase(m_point.begin() + deleIndex);
		// 顶点的处理过程
		removeChild(m_NodePoints[deleIndex]);
		m_NodePoints.erase(m_NodePoints.begin() + deleIndex);
		// 线的处理过程
		if (m_GeomType == 2)
		{
			osgEarth::Symbology::Geometry *geom = (m_feature->getGeometry());
			geom->erase(geom->begin() + deleIndex);
			m_featureNode->init();
		}
		return true;
	}
	void EditDrawElmentTracker::UpdateElement()
	{
		if (!m_CurElement)
			return;
		GLOBE_NAME::Element* edit = m_CurElement;
		if ((edit->Type()&ElementGeoType::Point) == ElementGeoType::Point)
		{
			edit->setPosition(m_point[0].X, m_point[0].Y, m_point[0].Z);
		}
		else if (edit->Type() == ElementType::eLinePictureElement3D)
		{
			LinePictureElement3D* line = dynamic_cast<LinePictureElement3D*>(edit);
			line->setPoints(m_point);
		}
		else if (edit->Type() == ElementType::ePolygonSymbolElement)
		{
			PolygonSymbolElement* polygon = dynamic_cast<PolygonSymbolElement*>(edit);
			polygon->setPoints(m_point);
		}
	}

	void EditDrawElmentTracker::SetCurrNodeIndex(int index)
	{
		if (m_CurrentNodeIndex == index)
			return;
		if (m_CurrentNodeIndex > -1)
		{
			m_NodePoints[m_CurrentNodeIndex]->setColor(ToColor(m_PointColor));
			m_NodePoints[m_CurrentNodeIndex]->setSize(m_pointSize);
		}
		m_CurrentNodeIndex = index;
		if (index > -1)
		{
			m_NodePoints[index]->setColor(ToColor(m_CurrPointColor));
			m_NodePoints[index]->setSize(m_CurrPointSize);
		}
			
	}


	osgEarth::MapNode * EditDrawElmentTracker::getMapNode()
	{
		return m_ptrMapTool->getMapNode();
	}

	void EditDrawElmentTracker::CreateLineFeature()
	{
		m_feature = new  osgEarth::Features::Feature(new osgEarth::Symbology::LineString(), getMapNode()->getMapSRS());
		m_feature->geoInterp() = osgEarth::GeoInterpolation::GEOINTERP_GREAT_CIRCLE;

		// clamp to the terrain skin as it pages in
		osgEarth::Symbology::AltitudeSymbol* alt = m_feature->style()->getOrCreate<osgEarth::Symbology::AltitudeSymbol>();

		// offset to mitigate Z fighting
		osgEarth::Symbology::RenderSymbol* render = m_feature->style()->getOrCreate<osgEarth::Symbology::RenderSymbol>();
		render->depthOffset()->enabled() = true;
		render->depthOffset()->automatic() = true;

		// define a style for the line
		osgEarth::Symbology::LineSymbol* ls = m_feature->style()->getOrCreate<osgEarth::Symbology::LineSymbol>();
		ls->stroke()->color() = osgEarth::Symbology::Color::Yellow;
		ls->stroke()->width() = 5.0f;
		ls->stroke()->widthUnits() = osgEarth::Symbology::Units::PIXELS;
		ls->tessellation() = 150;

		m_featureNode = new osgEarth::Annotation::FeatureNode(m_feature.get());
		m_featureNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

		addChild(m_featureNode.get());
	}
	SphereElement * EditDrawElmentTracker::CreatePointNode(const KERNEL_NAME::GsRawPoint3D & p)
	{
		osg::ref_ptr<SphereElement> pNode = new SphereElement(getMapNode());
		pNode->setColor(ToColor(m_PointColor));
		pNode->setSize(m_pointSize);
		pNode->setPosition(osgEarth::GeoPoint(getMapNode()->getMapSRS(), p.X, p.Y, p.Z));
		return pNode.release();
	}
	void EditDrawElmentTracker::CreatePolygonFeature()
	{

	}
	void EditDrawElmentTracker::DrawPointNode()
	{
		for (int i = 0, size = m_point.size(); i < size; i++)
		{
			osg::ref_ptr<SphereElement> pNode = CreatePointNode(m_point[i]);
			addChild(pNode);
			m_NodePoints.push_back(pNode);
		}
	}
	void EditDrawElmentTracker::DrawLineFeature()
	{
		if (!m_feature)
			CreateLineFeature();
		m_feature->getGeometry()->clear();
		for (int i = 0; i < m_point.size(); i++)
		{
			m_feature->getGeometry()->push_back(osg::Vec3d(m_point[i].X, m_point[i].Y, m_point[i].Z));
		}
		m_featureNode->init();
	}
	void EditDrawElmentTracker::DrawPolygonFeature()
	{

	}

	/*****************************************************************/

	osg::Geometry* CreateLineGeometry()
	{
		osg::ref_ptr<osg::Geometry> ptrPolygon = new osg::Geometry();

		osg::ref_ptr<osg::Vec3Array> ptrVertices = new osg::Vec3Array();
		osg::ref_ptr<osg::StateSet> stateset = ptrPolygon->getOrCreateStateSet();
		stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
		//设置法线跟着缩放
		stateset->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
		stateset->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false), osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
		//stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
		stateset->setRenderBinDetails(11, "RenderBin");
		//开启透明
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		ptrPolygon->setVertexArray(ptrVertices);
		osg::ref_ptr<osg::Vec4Array> ptrColors = new osg::Vec4Array;
		ptrColors->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
		ptrPolygon->setColorArray(ptrColors, osg::Array::BIND_OVERALL);
		ptrPolygon->setCullingActive(false);
		osg::ref_ptr<osg::DrawArrays> ptrDrawArray = new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, ptrVertices->size());
		ptrPolygon->addPrimitiveSet(ptrDrawArray);
		return ptrPolygon.release();
	}
	SimpleLineElement::SimpleLineElement()
	{
		m_Geom = CreateLineGeometry();
		osg::ref_ptr<osg::Geode> ptrGeode = new osg::Geode();
		m_Points = new osg::Vec3dArray();
		ptrGeode->setCullingActive(false);
		ptrGeode->addDrawable(m_Geom);
		ptrGeode->setDataVariance(osg::Object::DYNAMIC);

		ptrGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
		ptrGeode->getOrCreateStateSet()->setAttributeAndModes(new osg::Program, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

		addChild(ptrGeode);
	}
	SimpleLineElement::~SimpleLineElement()
	{
	}
	void SimpleLineElement::SetLineWidth(float w)
	{
		osg::ref_ptr<osg::LineWidth> ptrLineWidth = new osg::LineWidth(w);
		m_Geom->getOrCreateStateSet()->setAttributeAndModes(ptrLineWidth, osg::StateAttribute::ON);
	}
	float SimpleLineElement::GetLineWidth()
	{
		return 0.0f;
	}
	void SimpleLineElement::SetLineColor(const osg::Vec4 & linecolor)
	{
		osg::Vec4Array* color = dynamic_cast<osg::Vec4Array*>(m_Geom->getColorArray());
		if (!color)
			return;
		(*color)[0] = linecolor;
		color->dirty();
	}
	const osg::Vec4 & SimpleLineElement::GetLineColor() const
	{
		// TODO: 在此处插入 return 语句
		osg::Vec4Array* color = dynamic_cast<osg::Vec4Array*>(m_Geom->getColorArray());
		return color->at(0);
	}
	void SimpleLineElement::AddPoint(const osg::Vec3d & point)
	{
		m_Points->push_back(point);
	}
	void SimpleLineElement::SetPoints(osg::Vec3dArray& points)
	{
	//	m_Points = points;
	}
	int SimpleLineElement::GetPointCount()
	{
		return m_Points->size();
	}
	void SimpleLineElement::Clear()
	{
		m_Points->clear();
	}
	void SimpleLineElement::Init()
	{
		osg::ref_ptr<osgEarth::MapNode> ptrMapNode = GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode();
		osg::Vec3d vecPos;
		osg::Vec3d firstPos;
		int size = m_Points->size();
		osg::Vec3Array* linepoints = dynamic_cast<osg::Vec3Array*>(m_Geom->getVertexArray());
		linepoints->clear();
		for (int i = 0; i < size; i++)
		{
			osgEarth::GeoPoint mapPoint(ptrMapNode->getMapSRS(), (*m_Points)[i]);
			mapPoint.toWorld(vecPos);
			if (i == 0)
				firstPos = vecPos;
			linepoints->push_back(vecPos - firstPos);
		}
		//
		m_Geom->setPrimitiveSet(0, new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, size));
		setMatrix(osg::Matrix::translate(firstPos));
	}
}