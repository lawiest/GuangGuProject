#ifndef OSGEARTH_GEOVECTORQUERYBYATTRIBUTE_QUERY_TOOL_H
#define OSGEARTH_GEOVECTORQUERYBYATTRIBUTE_QUERY_TOOL_H 1

#include <osgDB/ConvertUTF>
#include <osgGA/GUIEventHandler>
#include <osg/View>
#include <osgViewer/Viewer>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Vec3d>
#include <osg/Camera>
#include <osg/Matrix>
#include <osg/Drawable>
#include <osg/Depth>
#include <osg/LineWidth>
#include <osg/Point>
#include <osgUtil/LineSegmentIntersector>

#include <osgEarth/MapNode>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthUtil/Controls>
#include <osgEarthFeatures/Common>
#include <osgEarthFeatures/Feature>
#include <osgEarthSymbology/Geometry>
#include <osgEarthSymbology/Style>
#include <osgEarth/StringUtils>

#include <vector>

#include "GEOFeatureUtils.h"
#include "GeoFeature_SqliteApi.h"
#include "GEOFeature_XMLApi.h"
#include "GEOWFSFeatureOptions.h"
#include "Tool/GGEarthManipulator.h"
#include <osgDB/FileUtils>

#define AHLGeode "highlightGeode";
namespace GeoGlobe {
	namespace GeoVector
	{
		using namespace osgEarth;

		//吉奥要素高亮结构体
		struct GEOVectorHighlightCommom
		{
			//创建高亮要素的符号特征
			static Style buildStyle( const osg::Vec4 &color, float width )
			{
				// Define a style for the feature data. Since we are going to render the
				// vectors as lines, configure the line symbolizer:
				Style style;

				PointSymbol* ps = style.getOrCreateSymbol<PointSymbol>();
				ps->fill()->color()=color;
				ps->size()=width;
				

				LineSymbol* ls = style.getOrCreateSymbol<LineSymbol>();
				ls->stroke()->color() = color;
				ls->stroke()->width() = width;    
				ls->tessellation() = 10;

				/*AltitudeSymbol* as = style.getOrCreate<AltitudeSymbol>();
				as->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
				as->technique() = AltitudeSymbol::TECHNIQUE_SCENE;*/

                RenderSymbol* rs = style.getOrCreateSymbol<RenderSymbol>();
				rs->depthTest()=false;
				rs->lighting()=false;
				//rs->depthOffset()->enabled() = true;
				//rs->depthOffset()->minBias() = 1000;

				return style;    
			}
            //移除高亮要素
			static void removeHighlightGeode(osg::Group* root,std::string geodename)
			{
				for (int i=0;i<root->getNumChildren();)
				{
					if (root->getChild(i)->getName()==geodename)
					{
						root->removeChild(i);
					}
					else
						i++;
				}
			}
		};
		//virtual class
		//吉奥本地数据属性查询
		struct GEOVectorFCSAttributeQuery
		{

            //根据要素的oid号来查询并高亮显示该要素,并定位到该要素
			static void execute(int oid,std::string path,
				osgViewer::Viewer* viewer,osg::Group* root,MapNode* mapNode)
			{
				GEOVectorHighlightCommom::removeHighlightGeode(root,"highlightGeode");

				GEOFeatureClass* geoFeaClass = new GEOFeatureClass;
				geoFeaClass->init(path);
				std::vector<int> oids;
				oids.push_back(oid);
				highlightLocalVector(oids,geoFeaClass,root,mapNode);
				osgEarth::Bounds bds;
				if (geoFeaClass->getOidBoundBox(oid,bds))
				{
					GGEarthManipulator* manip=dynamic_cast<GGEarthManipulator*>(viewer->getCameraManipulator());
					manip->setViewpoint(osgEarth::Viewpoint("",bds.center().x(),bds.center().y(),10000,0,-89,2),5);
				}
			}

			//local,one oid one feature
			//static void highlightLocalVector(std::vector<int>& oids,GEOFeatureClass* geoFeaClass,osg::Group* root,MapNode* mapNode)
			//{
			//	for (int i=0;i<oids.size();i++)
			//	{

			//		int oid = oids[i];
			//		GEOFeatureH gfeature;
			//		geoFeaClass->createGEOFeatureH(oid,&gfeature);
			//	 

			//		osg::ref_ptr<osgEarth::Annotation::FeatureNode> fnode=new osgEarth::Annotation::FeatureNode(mapNode,GEOFeatureUtils::createFeature(&gfeature,mapNode->getMapSRS()));
			//		fnode->setName("highlightGeode");
			//		fnode->setStyle(GEOVectorHighlightCommom::buildStyle(osgEarth::Symbology::Color::Red,5.0));
			//		root->addChild(fnode);
			//		
			//	}//for oid,every picked vector
			//}
			//
			static void highlightLocalVector(std::vector<int>& oids,GEOFeatureClass* geoFeaClass,osg::Group* root,MapNode* mapNode)
			{//one feature more oid
				if (oids.size()<1)
				{
					return;
				}
				if (oids.size()==1)//ÅÐ¶ÏÒ»¸öµãµÄÇé¿ö
				{
					int oid = oids[0];
					GEOFeatureH gfeature;
					geoFeaClass->createGEOFeatureH(oid,&gfeature);
					GEOGeometryH* geomHandle = gfeature.getGeomH();
					if (gfeature.valid()&&geomHandle)
					{
						if ((geoGEOMETRYTYPE)geomHandle->getGeomType()==GEO_GEOMETRY_POINT)
						{
							osg::ref_ptr<osg::Geode> osggeode = new osg::Geode;
							osggeode->setName("highlightGeode");
							root->addChild(osggeode);

							osggeode->setCullingActive(false);
							osggeode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
							osggeode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
							osggeode->getOrCreateStateSet()->setRenderBinDetails(1,"DepthSortedBin");

							osg::ref_ptr<osg::Point> psize = new osg::Point;
							psize->setSize(5.0);
							osggeode->getOrCreateStateSet()->setAttributeAndModes(psize.get(),osg::StateAttribute::ON);

							osg::ref_ptr<osg::Geometry> osggeom = new osg::Geometry;
							osggeode->addDrawable(osggeom);

							osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
							colors->push_back(osg::Vec4(1.0,0.0,0.0,1.0));
							osggeom->setColorArray(colors,osg::Array::BIND_OVERALL);

							//get the vertexarray,convert the coordinates
							osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray;
							osggeom->setVertexArray(vertices);

							int ptNum = geomHandle->getNumPoints();
							for (int i=0;i<ptNum;i++)
							{
								GeoPoint geopt(mapNode->getMapSRS(),osg::Vec3d(geomHandle->getGeoPoint(i),0.0));
								osg::Vec3d worldPoints;
								geopt.toWorld(worldPoints);
								vertices->push_back(worldPoints);
							}

							int numParts = geomHandle->getNumInfo();
							for (int i=0;i<numParts;i++)
							{
								osggeom->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::POINTS,geomHandle->getPartOffset(i),geomHandle->getPartPoints(i)));
							}
							return;
						}//if point

					}//if (gfeature.valid())
				}//if oid =1
				osg::ref_ptr<osgEarth::Symbology::MultiGeometry> multi = new Symbology::MultiGeometry();
				for (int i=0;i<oids.size();i++)
				{
					int oid = oids[i];
					GEOFeatureH gfeature;
					geoFeaClass->createGEOFeatureH(oid,&gfeature);

					multi->add(GEOFeatureUtils::createGeometry(gfeature.getGeomH()));
				}//for oid,every picked vector
				osg::ref_ptr<osgEarth::Features::Feature> oefea = new osgEarth::Features::Feature(multi,mapNode->getMapSRS());
				oefea->style()=GEOVectorHighlightCommom::buildStyle(osgEarth::Symbology::Color::Red,5.0);
				osg::ref_ptr<osgEarth::Annotation::FeatureNode> fnode=new osgEarth::Annotation::FeatureNode(oefea);
				fnode->setName("highlightGeode");
				
				//fnode->setStyle(GEOVectorHighlightCommom::buildStyle(osgEarth::Symbology::Color::Red,5.0));
				root->addChild(fnode);
			}

		};//class virtual handler
		//吉奥wfs服务矢量数据属性查询
		struct GEOVectorWFSAttributeQuery
		{
            //根据要素的oid号来查询并高亮显示该要素
			static void execute(int oid,const GeoGlobe::GeoVector::GEOWFSFeatureOptions& options,
				osgViewer::Viewer* viewer,osg::Group* root,MapNode* mapNode)
			{
				GEOVectorHighlightCommom::removeHighlightGeode(root,"highlightGeode");
				std::string wfsUrl = options.url().get().full();
				std::string typeName = options.typeName().get();
				std::string cachePath = options.cachePath().get();
				execute(oid,wfsUrl,typeName,cachePath,viewer,root,mapNode);
			}
            //根据要素的oid号来查询并高亮显示该要素，并对下载的文件缓存
			static void execute(int oid,std::string& wfsUrl,std::string& typeName,std::string& cachePath,
				osgViewer::Viewer* viewer,osg::Group* root,MapNode* mapNode)
			{
				std::stringstream buf;
				buf<<cachePath<<"/"<<typeName<<oid<<"querycache.xml";
				std::string xmlqcPath =  buf.str();

				
				if (getAttrQueryResponseXML(oid,wfsUrl,typeName,xmlqcPath))
				{
					if (!osgDB::fileExists(xmlqcPath))
					{
						return;
					}
					
					std::ifstream fin(xmlqcPath.c_str());
					//std::ifstream fin(xmlqcPath.c_str(),ios::binary);
					osg::ref_ptr<XmlDocument> doc = XmlDocument::load(fin);
					if (!doc)
					{
						return;
					}
					osg::ref_ptr<XmlElement> e_root = static_cast<XmlElement*>(doc->getChildren()[0].get());
					if (!e_root)
					{
						return;
					}
					GEOWFSFeatureClass* gmlFeaClass = new GEOWFSFeatureClass(e_root);
					highlightWFSVector(gmlFeaClass,root,mapNode);
					osgEarth::Bounds bds=gmlFeaClass->getBounds();
					GGEarthManipulator* manip=dynamic_cast<GGEarthManipulator*>(viewer->getCameraManipulator());
					manip->setViewpoint(osgEarth::Viewpoint("",bds.center().x(),bds.center().y(),10000,0,-89,2),5);
				}

			}

			//编写oid要素查询的post请求
			static bool getAttrQueryResponseXML(int oid,std::string& wfsUrl,std::string& typeName,std::string& xmlqcPath)
			{
				//xmlqcPath="E:/OSG/data/cache/vector/wfsaq.xml";
				if (!osgDB::fileExists(xmlqcPath))
				{
					//ReadResult rr = URI(createWFSAttrQueryURL(oid,wfsUrl,typeName)).readString();
					std::string utf8url = osgDB::convertStringFromCurrentCodePageToUTF8(createWFSAttrQueryURL(oid,wfsUrl,typeName));
					ReadResult rr = URI(utf8url).readString();
					//ReadResult rr = URI("http://192.168.42.55:9010/wfs_dl/wfs?VERSION=1.0.0&REQUEST=GetFeature&FeatureID=%E4%BA%94%E4%B8%87%E8%A1%8C%E6%94%BF%E6%9D%91%E7%82%B9.1").readString();
					if (rr.succeeded())
					{
						saveResponse(rr.getString(),xmlqcPath);
						return true;
					}
						
					else
						return false;
				}
				else
					return true;
			}
			//获取根据oid号矢量要素查询的url
			static std::string createWFSAttrQueryURL(int oid,std::string& wfsUrl,std::string& typeName)
			{
				std::string url;
				std::ostringstream buf;
				buf<<wfsUrl
					<<"?"
					<<"VERSION=1.0.0"
					<<"&REQUEST=GetFeature"
					<<"&FeatureID="
					<<typeName
					<<"."
					<<oid;
				url=buf.str();
				return url;
			}
			//编写ogc规范的矢量要素的post请求
			static std::string createWFSAttrQueryFilter(int oid)
			{
				std::stringstream buf;
				//<ogc:Filter><ogc:FeatureId fid="5"/></ogc:Filter>
				buf<<"<ogc:Filter xmlns:ogc="
					<<"\""
					<<"http://www.opengis.net/ogc"
					<<"\""
					<<"><ogc:FeatureId fid="
					<<"\""
					<<oid
					<<"\""
					<<"/></ogc::Filter>";
					return buf.str();
			}
			//保存该.xml文件
			static void saveResponse(const std::string& buffer, const std::string& filename)
			{
				if (!osgDB::fileExists(osgDB::getFilePath(filename)) )
					osgDB::makeDirectoryForFile(filename);
				std::ofstream fout;
				fout.open(filename.c_str(), std::ios::out | std::ios::binary);        
				fout.write(buffer.c_str(), buffer.size());        
				fout.close();
			}
            //高亮矢量要素
			static void highlightWFSVector(GEOWFSFeatureClass* gmlFeaClass,osg::Group* root,MapNode* mapNode)
			{
				if (!gmlFeaClass)
				{
					return;
				}
				//»ñµÃFeatureList
				FeatureList features;
				gmlFeaClass->createFeaturesfromGML(mapNode->getMapSRS(),features);
				
				//features to one feature node
				osg::ref_ptr<osgEarth::Symbology::MultiGeometry> multi = new Symbology::MultiGeometry();
				for (FeatureList::iterator itr = features.begin();itr!=features.end();itr++)
				{
					Feature* feature = itr->get();
					//multi->add(feature->getGeometry());

					/*Symbology::Geometry* output = new Symbology::PointSet(1);
					osg::Vec3d fpt=feature->getGeometry()->asVector().back();
					output->push_back(fpt);
					multi->add(output);
					break;*/
					std::vector<osg::Vec3d> ps=feature->getGeometry()->asVector();
					Symbology::Geometry* output = new Symbology::LineString(ps.size());
					for (int i=0;i<ps.size();i++)
					{
						output->push_back(ps[i]);
					}
					multi->add(output);
					break;
				}
				osg::ref_ptr<osgEarth::Features::Feature> oefea = new osgEarth::Features::Feature(multi,mapNode->getMapSRS());
				oefea->style()=GEOVectorHighlightCommom::buildStyle(osgEarth::Symbology::Color::Red,5.0);
				osg::ref_ptr<osgEarth::Annotation::FeatureNode> fnode=new osgEarth::Annotation::FeatureNode(oefea);
				fnode->setName("highlightGeode");
				//fnode->setStyle(GEOVectorHighlightCommom::buildStyle(osgEarth::Symbology::Color::Red,5.0));
				root->addChild(fnode);
				//for (FeatureList::iterator itr = features.begin();itr!=features.end();itr++)
				//{
				//	Feature* feature = itr->get();
				//	osg::ref_ptr<osgEarth::Annotation::FeatureNode> fnode=new osgEarth::Annotation::FeatureNode(mapNode,feature);
				//	fnode->setName("highlightGeode");
				//	fnode->setStyle(GEOVectorHighlightCommom::buildStyle(osgEarth::Symbology::Color::Red,5.0));
				//	root->addChild(fnode);
				//}//for every feature

			}
            //高亮查询到的矢量要素，并定位到该要素
			static void highlightWFSVectorAndSetViewPoint(GEOWFSFeatureClass* gmlFeaClass,osgViewer::Viewer* viewer,osg::Group* root,MapNode* mapNode)
			{
				if (!gmlFeaClass)
				{
					return;
				}
				//»ñµÃFeatureList
				FeatureList features;
				gmlFeaClass->createFeaturesfromGML(mapNode->getMapSRS(),features);
				
				//features to one feature node
				osg::ref_ptr<osgEarth::Symbology::MultiGeometry> multi = new Symbology::MultiGeometry();
				for (FeatureList::iterator itr = features.begin();itr!=features.end();itr++)
				{
					Feature* feature = itr->get();
					//multi->add(feature->getGeometry());

					/*Symbology::Geometry* output = new Symbology::PointSet(1);
					osg::Vec3d fpt=feature->getGeometry()->asVector().back();
					output->push_back(fpt);
					multi->add(output);
					break;*/
					std::vector<osg::Vec3d> ps=feature->getGeometry()->asVector();
					Symbology::Geometry* output = new Symbology::LineString(ps.size());
					for (int i=0;i<ps.size();i++)
					{
						output->push_back(ps[i]);
					}
					multi->add(output);
					break;
				}
				osg::ref_ptr<osgEarth::Features::Feature> oefea = new osgEarth::Features::Feature(multi,mapNode->getMapSRS());
				oefea->style()=GEOVectorHighlightCommom::buildStyle(osgEarth::Symbology::Color::Red,5.0);
				osg::ref_ptr<osgEarth::Annotation::FeatureNode> fnode=new osgEarth::Annotation::FeatureNode(oefea);
				fnode->setName("highlightGeode");
				//fnode->setStyle(GEOVectorHighlightCommom::buildStyle(osgEarth::Symbology::Color::Red,5.0));
				root->addChild(fnode);
				
				osg::BoundingSphered bs;
				oefea->getWorldBound(mapNode->getMapSRS(),bs);
				osgEarth::Bounds bds=oefea->getGeometry()->getBounds();
				GGEarthManipulator* manip=dynamic_cast<GGEarthManipulator*>(viewer->getCameraManipulator());
				manip->setViewpoint(osgEarth::Viewpoint("",bds.center().x(),bds.center().y(),bs.radius()*2.0,0,-89,2),5);
			}
            //高亮查询到的矢量要素
			static void highlightWFSVectorbyOID(int oid,GEOWFSFeatureClass* gmlFeaClass,osg::Group* root,MapNode* mapNode)
			{
				if (!gmlFeaClass)
				{
					return;
				}
				//»ñµÃFeatureList
				FeatureList features;
				gmlFeaClass->createFeaturesfromGMLbyOID(oid,mapNode->getMapSRS(),features);
				
				//features to one feature node
				osg::ref_ptr<osgEarth::Symbology::MultiGeometry> multi = new Symbology::MultiGeometry();
				for (FeatureList::iterator itr = features.begin();itr!=features.end();itr++)
				{
					Feature* feature = itr->get();
					//multi->add(feature->getGeometry());

					/*Symbology::Geometry* output = new Symbology::PointSet(1);
					osg::Vec3d fpt=feature->getGeometry()->asVector().back();
					output->push_back(fpt);
					multi->add(output);
					break;*/
					std::vector<osg::Vec3d> ps=feature->getGeometry()->asVector();
					Symbology::Geometry* output = new Symbology::LineString(ps.size());
					for (int i=0;i<ps.size();i++)
					{
						output->push_back(ps[i]);
					}
					multi->add(output);
					break;
				}
				osg::ref_ptr<osgEarth::Features::Feature> oefea = new osgEarth::Features::Feature(multi,mapNode->getMapSRS());
				oefea->style()=GEOVectorHighlightCommom::buildStyle(osgEarth::Symbology::Color::Red,5.0);
				osg::ref_ptr<osgEarth::Annotation::FeatureNode> fnode=new osgEarth::Annotation::FeatureNode(oefea);
				fnode->setName("highlightGeode");
				//fnode->setStyle(GEOVectorHighlightCommom::buildStyle(osgEarth::Symbology::Color::Red,5.0));
				root->addChild(fnode);
				//for (FeatureList::iterator itr = features.begin();itr!=features.end();itr++)
				//{
				//	Feature* feature = itr->get();
				//	osg::ref_ptr<osgEarth::Annotation::FeatureNode> fnode=new osgEarth::Annotation::FeatureNode(mapNode,feature);
				//	fnode->setName("highlightGeode");
				//	fnode->setStyle(GEOVectorHighlightCommom::buildStyle(osgEarth::Symbology::Color::Red,5.0));
				//	root->addChild(fnode);
				//}//for every feature

			}
		};
	}//namespace geovector
}



#endif// OSGEARTH_GEOVECTORQUERYBYATTRIBUTE_QUERY_TOOL_H