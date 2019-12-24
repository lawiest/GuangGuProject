#include <QtFramework/Map/MapView.h>
#include <kernel.h>
#include <qt\geospace2d.h>
#include <qt\GsGeospace3DQt.h>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QSplitter>

GLOBE_NS

GsMapView::GsMapView(QWidget * parent):QWidget(parent),m_ViewMode(GsViewMode::Vew2DAnd3D)
{
	GeoStar::Kernel::GsKernel::Initialize();
	InitialWidget();
	SetViewMode(GsViewMode::Vew2DAnd3D);
}

GsMapView::~GsMapView()
{
}

GeoStar::Kernel::QT::GsGeoSpace2D * GsMapView::GeoSpce2D() const
{
	return m_PtrGeoSpace2D;
}

GeoStar::Kernel::Globe::GsGeospace3DQt * GsMapView::GeoSpace3D() const
{
	return m_PtrGeoSpace3D;
}

GsViewMode GsMapView::ViewMode() const
{
	return m_ViewMode;
}

void GsMapView::SetViewMode(GsViewMode  viewMode)
{
	m_ViewMode = viewMode;
	m_PtrGeoSpace2D->Widget()->setVisible(viewMode!=GsViewMode::View3D);
	m_PtrGeoSpace3D->setVisible(viewMode != GsViewMode::View2D);
}

void GsMapView::InitialWidget()
{

	horizontalLayout = new QHBoxLayout(this);
	horizontalLayout->setSpacing(2);
	horizontalLayout->setContentsMargins(11, 11, 11, 11);
	horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
	horizontalLayout->setContentsMargins(0, 0, 0, 0);
	//3d组件
	m_PtrGeoSpace3D = new GsGeospace3DQt(this);
	m_PtrGeoSpace3D->setObjectName(QStringLiteral("QtSpace3D"));
	//
	horizontalLayout->addWidget(m_PtrGeoSpace3D);
	//2d组件
	m_PtrGeoSpace2D = new GeoStar::Kernel::QT::GsGeoSpace2D(this->windowHandle());
	QWidget * widget = QWidget::createWindowContainer(m_PtrGeoSpace2D, this);
	m_PtrGeoSpace2D->Widget(widget);
	horizontalLayout->addWidget(widget);
}


GsMapUtils::GsMapUtils()
{
}

GsMapUtils::~GsMapUtils()
{
}


//打开矢量图层
GsFeatureLayer* GsMapUtils::OpenFeatureLayer(const char* path, const char* vecName)
{
	GeoStar::Kernel::GsConnectProperty conn;
	conn.Server = path;
	GsSqliteGeoDatabaseFactory fac;
	GsGeoDatabasePtr ptrGDB = fac.Open(conn);
	if (!ptrGDB)
		return NULL;
	GsFeatureClassPtr ptrFeatureClass = ptrGDB->OpenFeatureClass(vecName);
	if (!ptrFeatureClass)
		return NULL;
	return new GsFeatureLayer(ptrFeatureClass);

}
//打开矢量图层文件
GsFeatureLayer* GsMapUtils::OpenFeatureLayer(const char* filePath)
{
	std::string str(filePath);
	int index = str.find_last_of('\\');
	if (index < 0) {
		index = str.find_last_of('/');
	}
	if (index < 0)
		return NULL;
	index += 1;
	std::string fileDir = str.substr(0, index);
	std::string fileName = str.substr(index, str.length() - index);
	return OpenFeatureLayer(fileDir.c_str(), fileName.c_str());
}

GsFeatureLayer* GsMapUtils::NewFeatureLayer(const char* file, GsGeometryType featureType)
{
	return NewFeatureLayer(file, featureType, new GsSpatialReference());
}
GsFeatureLayer* GsMapUtils::NewFeatureLayer(const char* file, GsGeometryType featureType, GsSpatialReference *spatial)
{
	std::vector<std::string> fields;
	fields.push_back("Name");
	return NewFeatureLayer(file, featureType, spatial, fields);
}
GsFeatureLayer*  GsMapUtils::NewFeatureLayer(const char* file, GsGeometryType geometryType, std::vector<std::string>& fields)
{
	return NewFeatureLayer(file, geometryType, new GsSpatialReference(), fields);
}
//创建FeatureLayer
GsFeatureLayer* GsMapUtils::NewFeatureLayer(const char* file, GsGeometryType featureType, GsSpatialReference *spatial, std::vector<std::string>& fields)
{
	std::string str(file);
	int index = str.find_last_of('\\');
	if (index < 0) {
		index = str.find_last_of('/');
	}
	if (index < 0)
		return NULL;
	index += 1;
	std::string fileDir = str.substr(0, index);
	std::string fileName = str.substr(index, str.length() - index);
	//创建新的数据源
	GeoStar::Kernel::GsConnectProperty conn;
	conn.Server = fileDir;
	GsSqliteGeoDatabaseFactory fac;
	GsGeoDatabasePtr ptrGDB = fac.Open(conn);
	if (!ptrGDB)
		return NULL;
	GsFields gsFields;
	for (int i = 0, size = fields.size(); i < size; i++)
	{
		GsField gsFld(fields[i].c_str(), GeoStar::Utility::Data::GsFieldType::eStringType);
		gsFields.Fields.push_back(gsFld);
	}
	GsGeometryColumnInfo colummInfo;
	colummInfo.GeometryType = featureType;
	GsFeatureClassPtr ptrFeatureClass = ptrGDB->CreateFeatureClass(fileName.c_str(), gsFields, colummInfo, spatial);
	if (!ptrFeatureClass)
		return NULL;
	return new GsFeatureLayer(ptrFeatureClass);
}

//打开瓦片图层
GsTileLayer *GsMapUtils::OpenTileLayer(const char* file)
{
	std::string str(file);
	int index = str.find_last_of('\\');
	if (index < 0) {
		index = str.find_last_of('/');
	}
	if (index < 0)
		return NULL;
	index += 1;
	std::string fileDir = str.substr(0, index);
	std::string fileName = str.substr(index, str.length() - index);
	return OpenTileLayer(fileDir.c_str(), fileName.c_str());
}
//打开瓦片图层
GsTileLayer *GsMapUtils::OpenTileLayer(const char *path, const char * vecName)
{
	GeoStar::Kernel::GsConnectProperty conn;
	conn.Server = path;
	GsSqliteGeoDatabaseFactory fac;
	GsGeoDatabasePtr ptrGDB = fac.Open(conn);
	if (!ptrGDB)
		return NULL;
	GsTileClassPtr ptrTileClass = ptrGDB->OpenTileClass(vecName);
	if (!ptrTileClass)
		return NULL;
	return new GsTileLayer(ptrTileClass);
}


GLOBE_ENDNS