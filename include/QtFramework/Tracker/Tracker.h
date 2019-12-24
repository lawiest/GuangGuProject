#pragma once
#include "QtFramework/geoqtframework_global.h"
#include <utility.h>
#include <map.h>


namespace GeoStar
{
	namespace Kernel
	{
		namespace QT
		{
			class GsGeoSpace2D;
		}
	}
}

GLOBE_NS

//平移tracker
class GEOQTFRAMEWORK_EXPORT GsZoomPanTracker :public GsSymbolTracker
{
public:
	GsZoomPanTracker(GeoStar::Kernel::QT::GsGeoSpace2D* pMap);
	virtual ~GsZoomPanTracker();
	/// \brief 接收鼠标按下消息
	virtual void OnMouseDown(GsButton eButton, int nKeyboard, const Utility::GsPT& pt)override;
	/// \brief 接收鼠标弹起消息
	virtual void OnMouseUp(GsButton eButton, int nKeyboard, const Utility::GsPT& pt)override;
	/// \brief 接收鼠标移动消息
	virtual void OnMouseMove(GsButton eButton, int nKeyboard, const Utility::GsPT& pt)override;
	//滚轮的放大和缩小的过程
	virtual void OnMouseWheel(GsButton eButton, int nDelta, int nKeyboard, const Utility::GsPT& pt)override;

protected:
	//更新渲染地图过程
	void UpdateMap();
	//获取地图组件
	GeoStar::Kernel::QT::GsGeoSpace2D* GeoSpace2D()const;
private:
	bool m_isMoved;
	GeoStar::Kernel::QT::GsGeoSpace2D * m_ptrGeoSpace2D;
};
//
class GEOQTFRAMEWORK_EXPORT GsPolylineTracker :public GsZoomPanTracker
{
public:
	GsPolylineTracker(GeoStar::Kernel::QT::GsGeoSpace2D* pMap);
	~GsPolylineTracker();
	//重置
	virtual void Reset()override;
	//绘制定点的最大值
	int MaxPointCount()const;
	//设置定点的最多值
	void MaxPointCount(int maxpoint);
	//获取需要的最少点
	int MinPointCount()const;
	//设置需要的最小点
	void MinPointCount(int minPoint);
protected:
	/// \brief 接收鼠标按下消息
	virtual void OnMouseDown(GsButton eButton, int nKeyboard, const Utility::GsPT& pt)override;
	/// \brief 接收鼠标弹起消息
	virtual void OnMouseUp(GsButton eButton, int nKeyboard, const Utility::GsPT& pt)override;
	/// \brief 接收鼠标移动消息
	virtual void OnMouseMove(GsButton eButton, int nKeyboard, const Utility::GsPT& pt)override;
	/// tracker绘制函数，子类通过覆盖此函数绘制内容。
	virtual void OnTrackerDraw(GsDisplay* pDisp)override;
	//结束Tracker的过程
	virtual void FinishTracker();
	//正在
	virtual void OnFinishTracker(const std::vector<GsRawPoint> & points);
	//鼠标左键点击
	virtual bool MouseLeftClick(GsButton eButton, int nKeyboard, const Utility::GsPT& pt);
	//右键单机过程
	virtual bool MouseRightClick(GsButton eButton, int nKeyboard, const Utility::GsPT& pt);
	//
	virtual bool OnDraw(GsDisplay* pDisp, const std::vector<GsRawPoint> & points);
private:
	std::vector<GsRawPoint> m_Points;
	int m_MaxCount;//最快
	int m_MinCount;//最小点坐标
	long m_MouseTime;
	bool m_isNeedAdd;
	bool m_IsMouseDown;
	float m_DownX;
	float m_DownY;
	bool m_isFinished;
};
//定义测量的类型
enum GEOQTFRAMEWORK_EXPORT GsMeasureType
{
	//长度测量
	Length,
	//面积测量
	Area
};

//测量Tracker编写
class GEOQTFRAMEWORK_EXPORT GsMeasureTracker :public GsPolylineTracker
{
public:
	GsMeasureTracker(GeoStar::Kernel::QT::GsGeoSpace2D* pMap);
	~GsMeasureTracker();
	//设置测量类型
	GsMeasureType MeasureType()const;
	//获取测量类型
	void MeasureType(GsMeasureType type);
protected:
	//右键单机过程
	virtual bool MouseRightClick(GsButton eButton, int nKeyboard, const Utility::GsPT& pt);
	virtual bool OnDraw(GsDisplay* pDisp, const std::vector<GsRawPoint> & points)override;
	//绘制长度测量
	void OnDrawLength(GsDisplay* pDisp, const std::vector<GsRawPoint> & points);
	//绘制面积测量
	void OnDrawArea(GsDisplay* pDisp, const std::vector<GsRawPoint> & points);
	//绘制文本的值
	void OnDrawText(GsDisplay* pDisp, const std::string& txt, const Utility::GsPTF& pt);

private:
	GsMeasureType m_MeasureType;
	GsSimpleLineSymbolPtr  m_ptrLineSymbol;
	GsSimpleFillSymbolPtr m_ptrFillSymbol;
	GsGeodesic * m_ptrGsGeoLine;
	GsGeodesicPolygon * m_ptrGsGeodesicPoly;
	GsSolidBrushPtr m_ptrTextBrush;
	GsStringFormatPtr m_ptrTextFormat;

};

//绘制的Tracker编写绘制点线面等图形的Tracker编写
class GEOQTFRAMEWORK_EXPORT GsDrawSymbolTracker :public GsPolylineTracker
{
public:
	GsDrawSymbolTracker(GeoStar::Kernel::QT::GsGeoSpace2D* pMap);
	~GsDrawSymbolTracker();
	/// \brief Geometry绘制完成的事件
	Utility::GsDelegate<void(GsGeometry*)> OnDrawSymbolFinished;
protected:
	virtual bool OnDraw(GsDisplay* pDisp, const std::vector<GsRawPoint> & points)override;
	//右键单机过程
	virtual bool MouseRightClick(GsButton eButton, int nKeyboard, const Utility::GsPT& pt) override;
	//结束的处理过程
	virtual void OnFinishTracker(const std::vector<GsRawPoint> & points)override;
	//
};


GLOBE_ENDNS
