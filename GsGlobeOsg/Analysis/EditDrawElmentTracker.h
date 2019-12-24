#pragma once
#include <osg/Group>
#include <geometry.h>
#include <osgViewer/View>
#include <osgEarth/MapNode>
#include <osg/Geometry>
#include <osgEarthAnnotation/FeatureNode>
#include <GsReference.h>
#include "SphereElement.h"
GLOBE_NS
class Element;
GLOBE_ENDNS
namespace GeoGlobe
{
	class GeoMapTool;
	/******封装编辑，绘制的tracker的过程*****/
	class EditDrawElmentTracker :public osg::Group
	{
	public:
		EditDrawElmentTracker(GeoMapTool* mapNode);
		~EditDrawElmentTracker();
		// 设置编辑对象
		bool SetEditElement(GLOBE_NAME::Element* edit);
		// 取消编辑对象
		void CancelEdit();

		int GetNodeSize();
		int GetDrawMode();
		// 判断是否 点中的节点的过程 -1 没有任何节点绘制
		int HitNode(osg::Node* node);
		// 获取当前点中的节点过程
		int GetCurrentIndex();
		bool AddPoint(const KERNEL_NAME::GsRawPoint3D& p);
		bool InsertPoint(const KERNEL_NAME::GsRawPoint3D& p, int index);
		void MovePoint(const KERNEL_NAME::GsRawPoint3D& p, int index);
		//删除当前节点
		bool DeleCurNode();
		// 更新elment 的位置信息
		void UpdateElement();

	protected:
		//
		void SetCurrNodeIndex(int index);

		osgEarth::MapNode* getMapNode();
		void CreateLineFeature();// 创建线对象
		SphereElement* CreatePointNode(const KERNEL_NAME::GsRawPoint3D& p);
		void CreatePolygonFeature();// 创建面feature
		void DrawPointNode();// 绘制点对象
		void DrawLineFeature();// 绘制线的过程
		void DrawPolygonFeature();// 绘制面类型

	protected:
		std::vector<KERNEL_NAME::GsRawPoint3D> m_point; //坐标位置
		std::vector <osg::ref_ptr<SphereElement>> m_NodePoints;
		osg::ref_ptr< osgEarth::Annotation::FeatureNode> m_featureNode;//绘制线的样式过程
		osg::ref_ptr< osgEarth::Features::Feature>  m_feature;
		osg::Vec4i m_PointColor;
		osg::Vec4i m_LineColor;
		osg::Vec4i m_CurrPointColor;//当前节点颜色对象
		float m_pointSize;// 节点大小
		float m_CurrPointSize;//当前节点大小		
		GLOBE_NAME::Element * m_CurElement;//当前编辑的element
		int m_CurrentNodeIndex;//当前选中的节点
		int m_DrawMode;//编辑模式或绘制模式
		int m_GeomType;// 1 是点，2 是线 ，3 是面
		//
		GeoMapTool* m_ptrMapTool;// 绘制tracker 对应的操作工具

	};
	// 绘制简单先的封装
	class SimpleLineElement:public osg::MatrixTransform
	{
	public:
		SimpleLineElement();
		~SimpleLineElement();

		void SetLineWidth(float w);
		float GetLineWidth();

		void SetLineColor(const osg::Vec4& color);
		const osg::Vec4 &GetLineColor()const;

		void AddPoint(const osg::Vec3d& point);
		void SetPoints(osg::Vec3dArray & points);
		int GetPointCount();
		void Clear();
		void Init();

	private:
		osg::ref_ptr<osg::Geometry> m_Geom;
		osg::ref_ptr<osg::Vec3dArray> m_Points;
	};
}
