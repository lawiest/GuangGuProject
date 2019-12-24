#pragma once
#include "GeoMapTools.h"
#include "EditDrawElmentTracker.h"
#include <osgEarthUtil/RTTPicker>
GLOBE_NS
class Element;
GLOBE_ENDNS

namespace GeoGlobe
{
	//****** elment 选择的事件过程 记录当前选择的状态的
	class ElementSelectTool :public GeoMapTool
	{
	public:
		ElementSelectTool(osgEarth::MapNode* mapNode);
		virtual ~ElementSelectTool();
		virtual bool onMousePress(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		// 鼠标抬起的事件处理
		virtual bool onMouseUp(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		// 工具的激活状态
		virtual void SetEnable(bool enable);
		void setEnableSelect(bool enable);
		bool isEnableSelect();
		// 设置当前选中的element
		void SetSelected(GLOBE_NAME::Element* seleElement);
		// 选择时触发的事件消息
		UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Element*, GLOBE_NAME::Element*)> OnSelectEvent;
	protected:
		// RTTPicker 回调的结构体
		struct RTTPickerCallBack:public osgEarth::Util::RTTPicker::Callback
		{
			ElementSelectTool * selectTool;
			RTTPickerCallBack(ElementSelectTool * tool);
			virtual void onHit(osgEarth::ObjectID id)override;
			virtual void onMiss()override;
		};
	protected:
		
		virtual bool LeftMouseClick(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		
		// 返回选择的对象的过程
		GLOBE_NAME::Element* OnPickerElement(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

		// 对当前选中的对象是否会进行选择并触发事件的标记
		bool OnSelectElements(GLOBE_NAME::Element* seletEle,bool isSendEvent=true);
		// 内部可以调用的选择
		virtual void InterOnSelectElement(GLOBE_NAME::Element* oldElem, GLOBE_NAME::Element* newElem) {};
		// 取消选择的过程
		void CancelSelect();
	protected:
		double m_MouseTime;
		bool m_isNeedAdd;
		bool m_IsMouseDown;
		float m_DownX;
		float m_DownY;
		bool m_EnableSelect;
		// 当前选中的element
		GLOBE_NAME::Element * m_CurrSelectElement;
		GLOBE_NAME::Element * m_TemSeletElement;//缓冲选择的过程
		osg::ref_ptr< osgEarth::Util::RTTPicker> m_rttPicker;// rttp 选择过程
		osg::ref_ptr<osgEarth::Util::RTTPicker::Callback> m_RttPickerCallback;
	};

	/**********************
	* 编辑组件的过程
	****/
	class EditElementTool :public ElementSelectTool
	{
	public:
		enum EditMode
		{
			/// 旋转和缩放编辑
			RotaAndScale = 1,
			/// 节点信息的编辑，控制点和定位点
			NodeEdit
		};
	public:
		EditElementTool(osgEarth::MapNode* mapNode);
		virtual ~EditElementTool();

		// 鼠标按下的事件处理
		virtual bool onMousePress(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		// 鼠标移动的事件处理
		virtual bool onMouseMove(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		// 鼠标拖动事件处理
		virtual bool onMouseDrag(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		// 鼠标抬起的事件处理
		virtual bool onMouseUp(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		// 接收键盘事件
		virtual bool onKeyUp(const osgGA::GUIEventAdapter& ea);
		// 开始编辑的过程
		void setEditElement(GLOBE_NAME::Element* editElement);
		// 开始编辑的事件
		UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Element*)> OnEditElementEvent;
		// 结束编辑的事件
		UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Element*)> OnEndEditElementEvent;
		// 删除element 触发的事件过程
		UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Element*)> OnDeleteElementEvent;
		//
	protected:
		// 封装鼠标点击的
		virtual bool LeftMouseClick(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);
		// 鼠标右键
		virtual bool RightMouseClick(const osgGA::GUIEventAdapter& ea, const KERNEL_NAME::GsRawPoint3D& point);

		virtual void InterOnSelectElement(GLOBE_NAME::Element* oldElem, GLOBE_NAME::Element* newElem)override;
		// 判断是否点击到一个节点
		int HitNode(const osgGA::GUIEventAdapter& ea);

		// 编辑更新的过程
		void OnUpdateEdit(int type);
		void CancelEdit();

	protected:// 变量定义的部分
		 // 编辑状态类型，0 ：点选，1：选中进行编辑状态，2：编辑节点 ，3: 移动整个图形
		 int m_EditStateType = 0;
		 GLOBE_NAME::Element * m_CurEditElement;
		 KERNEL_NAME::GsRawPoint3D m_DownPoint, m_MovePoint;
		 EditMode m_EditMode;
		 osg::ref_ptr<EditDrawElmentTracker> m_EditDrawTracker;
	};
}
