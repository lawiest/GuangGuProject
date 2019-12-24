#include "VectorLayer.h"
#include "innerimp.h"
#include <OsgCoreSingleton.h>
#include <osgearth/MapNode>
#include <osgEarthSymbology/Style>
#include <osgearth/ModelLayer>
#include <osgEarthSymbology/AltitudeSymbol>
#include <osgEarthDrivers/feature_ogr/OGRFeatureOptions>
#include <osgEarthDrivers/model_feature_geom/FeatureGeomModelOptions>
//#include <osgEarthUtil/FeatureQueryTool>
#include <osgEarthFeatures/FeatureIndex>
#include <osgEarth/Registry>
#include <osgEarthAnnotation/FeatureNode>
#include <OsgCoreSingleton.h>
#include <osgViewer/Viewer>

#include <osgEarth/IntersectionPicker>

#include <osgEarthFeatures/FeatureModelGraph>
#include <osgDB/FileUtils>

using namespace osgEarth::Symbology;
using namespace osgEarth;
//using namespace osgEarth::Util;
using namespace osgEarth::Features;
using namespace osgEarth::Symbology;

GLOBE_NS


class FeaturePicker : public osgGA::GUIEventHandler
{
public:
	FeaturePicker()
	{

	}
	virtual ~FeaturePicker()
	{

	}

protected:

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		//if (ea.getEventType() == ea.RELEASE)
		//{
		//	IntersectionPicker picker(dynamic_cast<osgViewer::View*>(aa.asView()));
		//	IntersectionPicker::Hits hits;
		//	if (picker.pick(ea.getX(), ea.getY(), hits)) 
		//	{
		//		std::set<ObjectID> oids;
		//		if (picker.getObjectIDs(hits, oids)) 
		//		{
		//			ObjectIndex* index = Registry::objectIndex();
		//			ObjectID oid = *oids.begin();
		//			osg::ref_ptr<FeatureIndex> fi = index->get<FeatureIndex>(oid);
		//			if (fi.valid()) 
		//			{
		//				OE_NOTICE << "IsectPicker: found OID " << oid << "\n";
		//				Feature* f = fi->getFeature(oid);
		//				if (f) 
		//				{
		//					OE_NOTICE << "...feature ID = " << f->getFID() << "\n";
		//				}
		//			}
		//			osg::ref_ptr<Feature> f = index->get<Feature>(oid);
		//			if (f.valid()) 
		//			{
		//				OE_NOTICE << "IsectPicker: found OID " << oid << "\n";
		//				OE_NOTICE << "...feature ID = " << f->getFID() << "\n";
		//			}
		//			//osg::ref_ptr<AnnotationNode> a = index->get<AnnotationNode>(oid);
		//			//if (a) 
		//			//{
		//			//	OE_NOTICE << "IsectPicker: found annotation " << a->getName() << "\n";
		//			//}
		//		}
		//		else 
		//		{
		//			OE_NOTICE << "IsectPicker: picked, but no OIDs\n";
		//		}
		//	}
		//	else 
		//	{
		//		OE_NOTICE << "IsectPicker: no intersect\n";
		//	}
		//}

		if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
			osgUtil::LineSegmentIntersector::Intersections intersections;
			view->computeIntersections(ea.getX(), ea.getY(), intersections);

			osg::Matrix _inverseMV;
			osg::ref_ptr<osg::Camera> cameraMaster = view->getCamera();
			_inverseMV.invert(cameraMaster->getViewMatrix());
			osg::Vec3 ptEye = osg::Vec3(0, 0, 0) * _inverseMV;
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

					{
						osg::Vec3d p = hitr->getWorldIntersectPoint();

						osgEarth::GeoPoint mapPoint;
						mapPoint.fromWorld(OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), p);

					}

					osgEarth::Features::FeatureModelGraph* ptrFeatureIndexNode = dynamic_cast<osgEarth::Features::FeatureModelGraph*>(node.get());
					if (NULL != ptrFeatureIndexNode)
					{
							IntersectionPicker picker(dynamic_cast<osgViewer::View*>(aa.asView()), ptrFeatureIndexNode);
							IntersectionPicker::Hits hits;
							if (picker.pick(ea.getX(), ea.getY(), hits)) 
							{
								std::set<ObjectID> oids;
								if (picker.getObjectIDs(hits, oids)) 
								{
									ObjectIndex* index = Registry::objectIndex();
									ObjectID oid = *oids.begin();
									osg::ref_ptr<FeatureIndex> fi = index->get<FeatureIndex>(oid);
									if (fi.valid()) 
									{
										OE_NOTICE << "IsectPicker: found OID " << oid << "\n";
										Feature* f = fi->getFeature(oid);
										if (f) 
										{
											OE_NOTICE << "...feature ID = " << f->getFID() << "\n";
										}
									}
									osg::ref_ptr<Feature> f = index->get<Feature>(oid);
									if (f.valid()) 
									{
										OE_NOTICE << "IsectPicker: found OID " << oid << "\n";
										OE_NOTICE << "...feature ID = " << f->getFID() << "\n";
									}
									//osg::ref_ptr<AnnotationNode> a = index->get<AnnotationNode>(oid);
									//if (a) 
									//{
									//	OE_NOTICE << "IsectPicker: found annotation " << a->getName() << "\n";
									//}
								}
								else 
								{
									OE_NOTICE << "IsectPicker: picked, but no OIDs\n";
								}
							}
							else 
							{
								OE_NOTICE << "IsectPicker: no intersect\n";
							}
					}

					//osgEarth::Features::FeatureSourceIndexNode* ptrFeatureIndexNode = dynamic_cast<osgEarth::Features::FeatureSourceIndexNode*>(node.get());
					//if (NULL != ptrFeatureIndexNode)
					//{
					//	int i = 0;
					//	i++;
					//}

				}
			}
		}

		return false;
	}
};



////osg::ref_ptr<osgEarth::Util::FeatureQueryTool> g_ptrTool;
//
//class ReadoutCallback : public FeatureQueryTool::Callback
//{
//public:
//	ReadoutCallback() : _lastFID(~0)
//	{
//	}
//
//	void setElevation(Feature* ptrBuildingFeature,double dbElevation)
//	{
//		for (int i = 0; i<ptrBuildingFeature->getGeometry()->size(); i++)
//		{
//			ptrBuildingFeature->getGeometry()->at(i).z() = dbElevation;
//		}
//	}
//
//	void onHit(ObjectID id)
//	{
//		FeatureIndex* index = Registry::objectIndex()->get<FeatureIndex>(id);
//		Feature* feature = index ? index->getFeature(id) : 0L;
//
//		if (feature == NULL)
//			return;
//
//		double dbElevation = 95.0;
//		double dbHeight = 32.0;
//
//		if (feature && feature->getFID() != _lastFID)
//		{
//			//const AttributeTable& attrs = feature->getAttrs();
//			//AttributeTable::const_iterator it = attrs.find("elevation");
//			//if (it!= attrs.end())
//			//	dbElevation = it->second.getDouble();
//
//			//it = attrs.find("high");
//			//if (it != attrs.end())
//			//	dbHeight = it->second.getDouble();
//
//			//设置离地面高度
//			setElevation(feature, dbHeight);
//
//			//高亮 
//			Feature* ptrBuildingFeature = new Feature(*feature, osg::CopyOp::DEEP_COPY_ALL);
//
//			LineSymbol* line = ptrBuildingFeature->style()->getOrCreate<LineSymbol>();
//			line->stroke()->color() = Color(Color::Yellow, 0.5);
//			line->stroke()->width() = 1.0f;
//			line->stroke()->widthUnits() = Units::PIXELS;
//			ptrBuildingFeature->style()->getOrCreateSymbol<ExtrusionSymbol>()->height() = dbElevation;
//			//ptrBuildingFeature->style()->getOrCreate<osgEarth::RenderSymbol>()->depthOffset()->enabled() = true;
//			//ptrBuildingFeature->style()->getOrCreate<osgEarth::RenderSymbol>()->depthOffset()->automatic() = true;
//			ptrBuildingFeature->style()->getOrCreate<osgEarth::RenderSymbol>()->depthTest() = true;
//			ptrBuildingFeature->style()->getOrCreate<PolygonSymbol>()->fill()->color() = Color(osgEarth::Symbology::Color::Cyan, 0.5);
//			//ptrBuildingFeature->style()->getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN /*AltitudeSymbol::CLAMP_ABSOLUTE*/;
//			//ptrBuildingFeature->style()->getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
//
//			osg::ref_ptr<osgEarth::Annotation::FeatureNode> polyNode = new osgEarth::Annotation::FeatureNode(OsgCoreSingleton::Instance()->MapNode(), ptrBuildingFeature);
//			OsgCoreSingleton::Instance()->MapNode()->addChild(polyNode);
//
//			_lastFID = feature->getFID();
//		}
//	}
//
//	void onMiss()
//	{
//		//_grid->setVisible(false);
//		_lastFID = 0u;
//	}
//
//	bool accept(const osgGA::GUIEventAdapter& ea, const osgGA::GUIActionAdapter& aa)
//	{
//		return ea.getEventType() == ea.RELEASE; // click
//	}
//
//	//Grid*     _grid;
//	FeatureID _lastFID;
//};

VectorLayer::VectorLayer(std::string strUrl, StoreType eSerType)
{
	if (!osgDB::fileExists(strUrl))
		return;

	m_bIsPick = false;

	int i;
	int FileCode;
	int Unused;
	int FileLength;
	int Version;
	int ShapeType;
	double Xmin;
	double Ymin;
	double Xmax;
	double Ymax;

	// 打开坐标文件
	FILE * ptrFileShpFile;
	if ((ptrFileShpFile = fopen(strUrl.c_str(), "rb")) != NULL)
	{
		fread(&FileCode, sizeof(int), 1, ptrFileShpFile);//从m_ShpFile_fp里面的值读到Filecode里面去，每次读一个int型字节的长度，读取一次  

		for (i = 0; i < 5; i++)
		{
			fread(&Unused, sizeof(int), 1, ptrFileShpFile);
		}

		fread(&FileLength, sizeof(int), 1, ptrFileShpFile);//读取FileLength  
		fread(&Version, sizeof(int), 1, ptrFileShpFile);//读取Version的值  
		fread(&ShapeType, sizeof(int), 1, ptrFileShpFile);//读取ShapeType的值  

		fread(&Xmin, sizeof(double), 1, ptrFileShpFile);
		fread(&Ymin, sizeof(double), 1, ptrFileShpFile);
		fread(&Xmax, sizeof(double), 1, ptrFileShpFile);
		fread(&Ymax, sizeof(double), 1, ptrFileShpFile);
	}

	Style style;

	//可以抬高矢量
	//std::string strverticalOffset("120");
	//style.getOrCreate<AltitudeSymbol>()->verticalOffset() = NumericExpression(strverticalOffset);

	//可以设置矢量前端显示
	//style.getOrCreate<RenderSymbol>()->depthTest() = false;

	switch (ShapeType)
	{
		case 1:
		{
			style.getOrCreate<PointSymbol>()->fill()->color() = osgEarth::Symbology::Color(1, 0, 0, 0.8);
			style.getOrCreate<PointSymbol>()->size() = 4;
		}
		break;
		case 3:
		{
			LineSymbol* line = style.getOrCreate<LineSymbol>();
			line->stroke()->color() = Color::Yellow;
			line->stroke()->width() = 1.0f;
			line->stroke()->widthUnits() = Units::PIXELS;
			//style.getOrCreate<osgEarth::AltitudeSymbol>()->verticalOffset() = 0;
		}
		break;
		case 25:
		case 5:
		{
			//style.getOrCreateSymbol<PolygonSymbol>()->fill()->color() = osgEarth::Symbology::Color(1.0, 1.0, 0.0, 0.6);
			LineSymbol* line = style.getOrCreate<LineSymbol>();
			line->stroke()->color() = Color::Yellow;
			line->stroke()->width() = 1.0f;
			line->stroke()->widthUnits() = Units::PIXELS;
			style.getOrCreate<osgEarth::AltitudeSymbol>()->verticalOffset() = 0;
			style.getOrCreateSymbol<PolygonSymbol>()->fill()->color() = Color(osgEarth::Symbology::Color::Cyan, 1.0);
			//style.getOrCreateSymbol<ExtrusionSymbol>()->height() = 100;
			//style.getOrCreate<osgEarth::AltitudeSymbol>()->clamping() = osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
			//style.getOrCreate<osgEarth::AltitudeSymbol>()->technique() = osgEarth::AltitudeSymbol::TECHNIQUE_DRAPE;
			//解决Z fighting
			//style.getOrCreate<osgEarth::RenderSymbol>()->depthOffset()->enabled() = true;
			//style.getOrCreate<osgEarth::RenderSymbol>()->depthOffset()->automatic() = true;
		}
		break;
		default:
		{
			//style.getOrCreateSymbol<PolygonSymbol>()->fill()->color() = osgEarth::Symbology::Color(1.0, 1.0, 0.0, 0.6);
			LineSymbol* line = style.getOrCreate<LineSymbol>();
			line->stroke()->color() = Color::Yellow;
			line->stroke()->width() = 1.0f;
			line->stroke()->widthUnits() = Units::PIXELS;
			style.getOrCreate<osgEarth::AltitudeSymbol>()->verticalOffset() = 0;
			style.getOrCreateSymbol<PolygonSymbol>()->fill()->color() = Color(osgEarth::Symbology::Color::Cyan, 1.0);

			//LineSymbol* line = style.getOrCreate<LineSymbol>();
			//line->stroke()->color() = Color::Yellow;
			//line->stroke()->width() = 1.0f;
			//line->stroke()->widthUnits() = Units::PIXELS;
			//style.getOrCreate<osgEarth::AltitudeSymbol>()->verticalOffset() = 0;
		}
		break;
	}

	osgEarth::Drivers::OGRFeatureOptions featureOptions;
	featureOptions.url() = strUrl;

	osgEarth::Drivers::FeatureGeomModelOptions geomOptions;
	geomOptions.featureOptions() = featureOptions;
	geomOptions.styles() = new StyleSheet();
	geomOptions.styles()->addStyle(style);
	geomOptions.enableLighting() = false;

// 	osgEarth::Features::FeatureSourceIndexOptions psindexOption;
// 	psindexOption.embedFeatures() = true;
// 	geomOptions.featureIndexing() = psindexOption;

	osgEarth::ModelLayerOptions layerOptions("pickVector", geomOptions);
	osg::ref_ptr<osgEarth::ModelLayer> ptrVecLayer = new osgEarth::ModelLayer(layerOptions);


	m_ptrLayerHandle = new ReferenceImp <osgEarth::ModelLayer>(ptrVecLayer);
	OsgCoreSingleton::Instance()->MapNode()->getMap()->addLayer(ptrVecLayer);

	Range(Xmin, Xmax, Ymin, Ymax);

	m_LayerType = eVector;
	m_StoreType = eSerType;

	m_strLayerUrl = strUrl;
}

void VectorLayer::StartPick()
{
	if (!m_bIsPick)
	{
		//g_ptrTool = new osgEarth::Util::FeatureQueryTool();
		//g_ptrTool->addChild(OsgCoreSingleton::Instance()->MapNode());

		//g_ptrTool->setDefaultCallback(new ReadoutCallback());
		//OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(g_ptrTool);
		OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(new FeaturePicker());
		m_bIsPick = true;
	}
}

void VectorLayer::EndPick()
{
	if (m_bIsPick)
	{
		//OsgCoreSingleton::Instance()->OsgViewer()->removeEventHandler(g_ptrTool);
		//g_ptrTool = NULL;
		m_bIsPick = false;
	}
}

VectorLayer::VectorLayer(KERNEL_NAME::GsTileClass* ptrTileClass)
{

}

int VectorLayer::UID()
{
	return m_ptrLayerHandle->CastTo<osg::ref_ptr<osgEarth::ModelLayer>>()->getUID();
}
 

VectorLayer::~VectorLayer()
{

}

void VectorLayer::Visible(bool bVisible)
{
	m_ptrLayerHandle->CastTo<osg::ref_ptr<osgEarth::ModelLayer>>()->setVisible(bVisible);
}

bool VectorLayer::Visible() const
{
	return m_ptrLayerHandle->CastTo<osg::ref_ptr<osgEarth::ModelLayer>>()->getVisible();
}

GLOBE_ENDNS