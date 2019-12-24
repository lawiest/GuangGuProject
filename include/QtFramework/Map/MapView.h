
#pragma once
#include <utility.h>
#include <geometry.h>
#include "QtFramework\geoqtframework_global.h"
#include <QWidget>
#include "QPointer"

class QHBoxLayout;
class QSplitter;

namespace GeoStar 
{
	namespace Kernel
	{
		class GsFeatureLayer;
		class GsTileLayer;
		namespace QT
		{
			class GsGeoSpace2D;
		}
	}
}

GLOBE_NS

class GsGeospace3DQt;

enum GEOQTFRAMEWORK_EXPORT GsViewMode
{
	View2D = 0,
	View3D,
	Vew2DAnd3D
};

class GEOQTFRAMEWORK_EXPORT GsMapView:public QWidget,public GeoStar::Utility::GsRefObject
{
	QOBJECT_H
public:
	GsMapView(QWidget* parent = 0);
	~GsMapView();

	GeoStar::Kernel::QT::GsGeoSpace2D* GeoSpce2D()const;
	GeoStar::Kernel::Globe::GsGeospace3DQt *GeoSpace3D() const;
	//当前显示模式
	GsViewMode ViewMode()const;
	//设置显示模式
	void SetViewMode(GsViewMode viewMode);

private:
	//初始化widget过程
	void InitialWidget();
private:
	GeoStar::Kernel::Globe::GsGeospace3DQt *m_PtrGeoSpace3D;
	GeoStar::Kernel::QT::GsGeoSpace2D* m_PtrGeoSpace2D;
	GsViewMode m_ViewMode;
	//
	QPointer<QHBoxLayout> horizontalLayout;
	QPointer<QSplitter> splitter;
};


class GEOQTFRAMEWORK_EXPORT GsMapUtils
{
public:
	 GsMapUtils();
	~ GsMapUtils();
	//打开矢量图层
	static GsFeatureLayer* OpenFeatureLayer(const char* path, const char* vecName);
	//打开矢量图层文件
	static GsFeatureLayer* OpenFeatureLayer(const char* filePath);
	//创建新的数据源
	static GsFeatureLayer* NewFeatureLayer(const char* file, GsGeometryType geometryType);
	static GsFeatureLayer* NewFeatureLayer(const char* file, GsGeometryType geometryType, GsSpatialReference *spatial);
	static GsFeatureLayer* NewFeatureLayer(const char* file, GsGeometryType geometryType, GsSpatialReference *spatial, std::vector<std::string>& fields);
	static GsFeatureLayer* NewFeatureLayer(const char* file, GsGeometryType geometryType, std::vector<std::string>& fields);
	//打开瓦片图层
	static GsTileLayer *OpenTileLayer(const char* file);
	static GsTileLayer *OpenTileLayer(const char *path, const char * vecName);
private:

};



GLOBE_ENDNS
