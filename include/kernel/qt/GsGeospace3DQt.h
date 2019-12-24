#pragma once
#include <utility.h>
#include <GsReference.h>
#include <geometry.h>
#include <QWidget>
#include <QTimer>

namespace osgQt
{
	class GraphicsWindowQt;
}

namespace osgEarth
{
	class MapNode;
}

namespace osgViewer
{
	class Viewer;
}

GLOBE_NS

class LayerBox;
class SceneBox;
class AnalysisBox;

class ElementBox;
class PlaceBox;
class SymbolBox;

class FlyBox;

class EarthManipulator;
class AnimationPathHandler;
class PointRotationHandler;

class GS_API GsGeospace3DQt : public QWidget, public GeoStar::Utility::GsRefObject
{
public:
	GsGeospace3DQt(QWidget* parent = 0, Qt::WindowFlags f = 0);

	QWidget* addViewWidget(osgQt::GraphicsWindowQt* gw);

	osgQt::GraphicsWindowQt* createGraphicsWindow(int x, int y, int w, int h, const std::string& name = "", bool windowDecoration = false);

	LayerBox* LayerBox();
	SceneBox* SceneBox();
	AnalysisBox* AnalysisBox();

	ElementBox* ElementBox();
	SymbolBox* SymbolBox();
	PlaceBox* PlaceBox();
	FlyBox* FlyBox();

	//设置倾斜全局参数，用来点位纠偏
	void setSrsInfo(const std::string& strSrs,const KERNEL_NAME::GsRawPoint3D& originPoint);

	void CaptureScreen(std::string strPicture);

	void Test(std::string strUrl);

	virtual void paintEvent(QPaintEvent* event);

protected:

	QTimer _timer;
	osgEarth::MapNode* m_ptrMapNode;
	EarthManipulator* m_ptrManip;
	osgViewer::Viewer* m_ptrViewer;

	GLOBE_NAME::LayerBox* m_ptrLayerBox;
	GLOBE_NAME::SceneBox* m_ptrScenceBox;
	GLOBE_NAME::AnalysisBox * m_ptrAnalysisBox;
	GLOBE_NAME::ElementBox * m_ptrElementBox;
	GLOBE_NAME::SymbolBox * m_ptrSymbolBox;
	GLOBE_NAME::FlyBox* m_ptrFlyBox;

};

GS_SMARTER_PTR(GsGeospace3DQt);

GLOBE_ENDNS

