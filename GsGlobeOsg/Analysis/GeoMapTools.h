#pragma once
#include <osgGA/GUIEventHandler>
#include <geometry.h>
#include <osgViewer/View>
#include <osgEarth/MapNode>
#include <osgEarthAnnotation\FeatureNode>
#include "EditDrawElmentTracker.h"
#include <GsReference.h>

GLOBE_NS
class ElementBox;
GLOBE_ENDNS

namespace GeoGlobe
{
	/****
	* 基本的地图工具类型，封装了点击，右键和
	*/
	class GeoMapTool :public osgGA::GUIEventHandler
	{
	protected:
		std::string m_zToolName;

	public:
		GeoMapTool(osgEarth::MapNode* mapNode);
		virtual~GeoMapTool();

		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;
		// 鼠标按下的事件处理
		virtual bool onMousePress(const osgGA::GUIEventAdapter& ea,const KERNEL_NAME::GsRawPoint3D& point);
		// 鼠标移动的事件处理
		virtual bool onMouseMove(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		// 鼠标抬起的事件处理
		virtual bool onMouseUp(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		// 拖拽的相应事件
		virtual bool onMouseDrag(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point) { return false; };
		// 键盘的按下事件
		virtual bool onKeyDown(const osgGA::GUIEventAdapter& ea) { return false; }
		// 键盘的弹起事件
		virtual bool onKeyUp(const osgGA::GUIEventAdapter& ea) { return false; }
		// 是否激活使用设置
		virtual void SetEnable(bool enable);
		virtual void SetMonitorMouseMove(bool enable);
		virtual bool Enable();

		virtual void setMapNode( osgEarth::MapNode* mapNode);
		virtual osgEarth::MapNode* getMapNode();

		// 是否点下键盘事件
		bool isPressKey(int key);

	protected:
		// 封装鼠标点击的
		virtual bool LeftMouseClick(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		// 鼠标右键
		virtual bool RightMouseClick(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		// 鼠标 双击
		virtual bool MouseDoubleClick(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		// 鼠标位置转换到经纬度的方法
		bool getLocationAt(osgViewer::View* view, double x, double y, KERNEL_NAME::GsRawPoint3D& point);
		// 地理转换到 世界坐标
		bool PointToWorld(KERNEL_NAME::GsRawPoint3D&point, osg::Vec3d & outWorld);
		// 世界坐标转换到经纬度
		bool WorldToGeoPoint(osg::Vec3d & world, KERNEL_NAME::GsRawPoint3D& point);
		GLOBE_NAME::ElementBox* GetElementBox();
		//
		bool m_isEnable;
		bool m_isOpenMove;
		osg::ref_ptr<osgEarth::MapNode> m_mapNode;
		std::vector<int> m_CurPressKey;
		osgViewer::View* m_ptrCurView;// 内部使用当前view
	};

	/****************************************/
	//*--           NewPolygonLineTool        */
	/****************************************/
	class NewPolygonLineTool :public GeoMapTool
	{
	public:
		NewPolygonLineTool(osgEarth::MapNode* mapNode);
		virtual ~NewPolygonLineTool();
		// 鼠标按下的事件处理
		virtual bool onMousePress(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		// 鼠标移动的事件处理
		virtual bool onMouseMove(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		// 鼠标抬起的事件处理
		virtual bool onMouseUp(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		// 是否激活使用设置
		virtual void SetEnable(bool enable);

		/// \brief 调用次函数会结束工具过程
		  virtual void FinishTracker();
		    /// \brief 正在结束过程调用的方法
		  virtual void OnFinishTracker(const std::vector<KERNEL_NAME::GsRawPoint3D> & points);
		    /// \brief 更新数据需要
		  virtual void Update();
		    /// \brief 重置组件的过程
		  virtual void Reset();
		    /// \brief 获取绘制多段线需要的最小点的个数
		    /// \return 返回绘制多段线需要的最小点个数
		    int MinPointCount()const;

		    /// \brief 设置绘制多段线需要最小点个数
		    /// \param min 输入的值
		    void MinPointCount(int min);
		    /// \brief 获取需要的最大点个数
		    ///
		    int MaxPointCount()const;
		    /// \brief 设置绘制需要的最大值
		    /// \param max 输入的值
		    void MaxPointCount(int max);
	protected:
		virtual bool LeftMouseClick(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		virtual bool RightMouseClick(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);

	protected:
		    std::vector<KERNEL_NAME::GsRawPoint3D> m_Points;//存储的点数据
		    int m_MaxCount;//最快
		    int m_MinCount;//最小点坐标
		    double m_MouseTime;
		    bool m_isNeedAdd;
		    bool m_IsMouseDown;
		    float m_DownX;
		    float m_DownY;
		    bool m_isFinished;
	};

	/***********************
	*简单绘制工具的过程
	/*********************************/
	class DrawElementTool : public NewPolygonLineTool
	{
	public:
		DrawElementTool(osgEarth::MapNode* mapNode);
		virtual~DrawElementTool();
		/// \brief 正在结束过程调用的方法
		virtual void OnFinishTracker(const std::vector<KERNEL_NAME::GsRawPoint3D> & points);
		/// \brief 更新数据需要
		virtual void Update();

		osg::Node* DisplayGroup();
	//	virtual bool MouseDoubleClick(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		// 完成的委托过程
		UTILITY_NAME::GsDelegate<void(const KERNEL_NAME::GsRawPoint3D&)> OnDoubleClickEvent;
		UTILITY_NAME::GsDelegate<void(const std::vector<KERNEL_NAME::GsRawPoint3D> &)> OnDrawFinishEvent;
	protected:
		virtual bool RightMouseClick(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		virtual bool MouseDoubleClick(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point) override;
		void CreateFeatrue();
		void Clear();
	protected:
		//
		//osgEarth::GeoInterpolation _geoInterpolation;
		//osg::ref_ptr<osg::Node> RubberBand;
		//osg::ref_ptr< osgEarth::Annotation::FeatureNode > _featureNode;
		//osg::ref_ptr< osgEarth::Features::Feature >  _feature;
		osg::ref_ptr<SimpleLineElement> m_SimpleLine;
		////距离的Label
		//osgEarth::Symbology::Style m_SymStyle;
		//osg::ref_ptr<osg::Geometry> m_drawLine;
		//旋转矩阵
		osg::ref_ptr<osg::MatrixTransform> mt;
		osg::ref_ptr<osg::Group> displayGroup;
	};
	/********************************************************
	* 矩形拉框绘制的工具的封装
	**********************************************************/
	class DrawRectElementTool :public DrawElementTool
	{
	public:
		DrawRectElementTool(osgEarth::MapNode* mapNode);
		virtual~DrawRectElementTool();

		
		virtual void OnFinishTracker(const std::vector<KERNEL_NAME::GsRawPoint3D> & points);
		/// \brief 更新数据需要
		virtual void Update();
		///  \brief 获取当前绘制model
		int GetDrawModel();
		/// \brief 设置当前model 的过程
		/// \brief param model 1 是 点、2、是线、3是面、4是矩形
		void SetDrawModel(int model);
	protected:
		int m_DrawModel;// 1 是 点、2、是线、3是面、4是矩形
	};
}