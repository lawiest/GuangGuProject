#ifndef OSGEARTHFEATURES_FEATURE_GEOFEATURE_XML_GEO
#define OSGEARTHFEATURES_FEATURE_GEOFEATURE_XML_GEO 1

#include <osgEarth/Map>
#include <osgEarth/ModelLayer>
#include <osgEarth/Config>
#include <osgEarth/XmlUtils>
#include <osgEarth/FileUtils>
#include <osgEarthSymbology/Style>
#include <osgEarthFeatures/FeatureDisplayLayout>

#include <osgEarthDrivers/model_feature_geom/FeatureGeomModelOptions>

#include <osgDB/ReaderWriter>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include "Tool/CodeTransformHelp.h"

#include "GEOFeatureOptions.h"
#include "Vector/GEORasterizerOptions.h"

namespace GeoGlobe
{
	namespace GeoVector
	{
		

		//确定对矢量数据渲染的方式，以栅格化的方式或者几何方式绘制
		enum GeoVectorRenderType
		{
			GEOVECTORMODEL,
			GEOVECTORIMAGE
		};
		////////////////////////吉奥矢量要素符号特征相关的结构体
		//包括颜色，点、线、面的特征geo_color，geo_pointStyle，geo_lineStyle，geo_polyStyle
		struct geo_color
		{
		    std::string value;
			int transparent;
		};

		struct geo_pointStyle
		{
			geo_color color;
			float size;
		};
		struct geo_polyStyle
		{
			geo_color FillColor;
			geo_color EdgeColor;
		    float width;
		};

		struct geo_lineStyle
		{
			geo_color color;
			float width;
		};

		struct GeoVectorXMLOptions
		{
			static double m_Xmin;
			static double m_Xmax;
			static double m_Ymin;
			static double m_Ymax;

			//2?′????è
            //该类的主要函数，根据文件路径，以及map参数，渲染的方式加载矢量数据
			//传入参数std::string xmlPath，矢量要素配置文件，描述矢量图层的数据来源及其符号特征
			//osgEarth::Map,矢量数据需要加入的map图层
			//GeoVectorRenderType 矢量数据的渲染方式，栅格或者几何绘制
			static bool create(std::string xmlPath,osgEarth::Map* map,GeoVectorRenderType type)
			{
				if (!osgDB::fileExists(xmlPath))
				{
					OE_NOTICE << "XML not exists " << std::endl;
					return false;
				}
					  
				std::ifstream in(xmlPath.c_str());
				Config doc;
				doc.fromXML(in);

				if (doc.empty()||doc.children().empty())
				{
					OE_NOTICE << "Failed to get XML " << std::endl;
					return false;
				}

				/////////读取xml配置文件中的geomap节点，获取矢量图层及对应符号特征
				Config geoMap = doc.child("geomapfile").child("geomap");

				std::map<std::string,std::string> Dataname_Stylename;
				Config geoMapLayers = geoMap.child("layers");

				for(ConfigSet::const_iterator i=geoMapLayers.children().begin();i!=geoMapLayers.children().end();++i)
				{
					Config geoFeatureLayer = *i;
					Dataname_Stylename.insert(std::map<std::string,std::string>::value_type(geoFeatureLayer.child("layersourcename").value(),geoFeatureLayer.child("layerstylename").value()));
				}

				std::string dataRootPath = osgDB::getFilePath(xmlPath);

				Config fdataSources = doc.child("geomapfile").child("geodatasources");
				Config fStyleGroups = doc.child("geomapfile").child("geostylegroups").child("geostylegroup");

				std::map<std::string,osgEarth::Symbology::Style> dataPath_osgStyle;

				for (ConfigSet::const_iterator i=fdataSources.children().begin();i!=fdataSources.children().end();++i)
				{
					const osgEarth::Config& dataSource=*i;
					std::string dname = dataSource.child("dataname").value();
					std::string dataSourcePath = dataRootPath+"/"+dname+".fcs";

					std::string name = dataSource.child("name").value();
					std::map<std::string,std::string>::iterator itr = Dataname_Stylename.find(name);

					if (itr ==Dataname_Stylename.end())
					{
						continue;
					}

					for (ConfigSet::const_iterator i= fStyleGroups.children().begin();i!=fStyleGroups.children().end();++i)
					{
						const osgEarth::Config& geoStyle = *i;
						
						if (geoStyle.child("name").value()==itr->second)
						{
							osgEarth::Symbology::Style osgStyle;

							if(geoStyle.child("type").value()=="point")
							{
								geo_pointStyle pointStyle;
								pointStyle.color.value = geoStyle.child("renditions").child("pointrendition").child("color").child("value").value();
								pointStyle.color.transparent = atoi(geoStyle.child("renditions").child("pointrendition").child("color").child("transparent").value().c_str());
								pointStyle.size = (float) atof(geoStyle.child("renditions").child("pointrendition").child("size").value().c_str());

								createStylefromGeostyle(pointStyle,osgStyle);
							}
							else if(geoStyle.child("type").value()=="polygon")
							{
								geo_polyStyle polyStyle;
								polyStyle.FillColor.value = geoStyle.child("renditions").child("polygonrendition").child("fillcolor").child("value").value();
								polyStyle.FillColor.transparent = atoi(geoStyle.child("renditions").child("polygonrendition").child("fillcolor").child("transparent").value().c_str());
								polyStyle.EdgeColor.value = geoStyle.child("renditions").child("PolygonRendition").child("EdgeColor").child("Value").value();
								polyStyle.EdgeColor.transparent = atoi(geoStyle.child("renditions").child("polygonrendition").child("edgecolor").child("transparent").value().c_str());
								polyStyle.width = (float) atof(geoStyle.child("renditions").child("polygonrendition").child("width").value().c_str());

								createStylefromGeostyle(polyStyle,osgStyle);
							}
							else if (geoStyle.child("type").value()=="line")
							{
								geo_lineStyle lineStyle;
								lineStyle.color.value = geoStyle.child("renditions").child("linerendition").child("color").child("value").value();
								lineStyle.color.transparent = atoi(geoStyle.child("renditions").child("linerendition").child("color").child("transparent").value().c_str());
								lineStyle.width = (float) atof(geoStyle.child("renditions").child("linerendition").child("width").value().c_str());

								createStylefromGeostyle(lineStyle,osgStyle);
							}

							dataPath_osgStyle.insert(std::map<std::string,osgEarth::Symbology::Style>::value_type(dataSourcePath,osgStyle));

							break;
						}
					}
				}

				for (std::map<std::string,osgEarth::Symbology::Style>::iterator itr=dataPath_osgStyle.begin();itr!=dataPath_osgStyle.end();++itr)
				{
					std::string dbpath=itr->first;
					osgEarth::Symbology::Style sty = itr->second;
					createOptionsandLayers(dbpath,sty,type,map);
				}
			}

			//′????è
			static bool create(std::string xmlPath,float height,osgEarth::Map* map,GeoVectorRenderType type)
			{
				if (!osgDB::fileExists(xmlPath))
				{
					OE_NOTICE << "XML not exists " << std::endl;
					return false;
				}

				std::ifstream in(xmlPath.c_str());
				Config doc;
				doc.fromXML(in);

				if (doc.empty()||doc.children().empty())
				{
					OE_NOTICE << "Failed to get XML " << std::endl;
					return false;
				}

				Config geoMap = doc.child("geomapfile").child("geomap");

				//设置数据集及其对应的符号特征的名称
				std::map<std::string,std::string> Dataname_Stylename;
				Config geoMapLayers = geoMap.child("layers");

				for(ConfigSet::const_iterator i=geoMapLayers.children().begin();i!=geoMapLayers.children().end();++i)
				{
					Config geoFeatureLayer = *i;
					Dataname_Stylename.insert(std::map<std::string,std::string>::value_type(geoFeatureLayer.child("layersourcename").value(),geoFeatureLayer.child("layerstylename").value()));
				}

				////////////////////////////////////////////////////根据xml文件所在的路径确定矢量要素图层数据所在的路径
				//std::string dataRootPath =  doc.child("GeoMapFile").child("GeoConnections").child("GeoConnection").child("Address").value();
				std::string dataRootPath = osgDB::getFilePath(xmlPath);

				Config fdataSources = doc.child("geomapfile").child("geodatasources");
				Config fStyleGroups = doc.child("geomapfile").child("geostylegroups").child("geostylegroup");
				Config xmlGeoConnections = doc.child("geomapfile").child("geoconnections");

				std::map<std::string,osgEarth::Symbology::Style> dataPath_osgStyle;

				for (ConfigSet::const_iterator i = fdataSources.children().begin(); i != fdataSources.children().end(); ++i)
				{
					const osgEarth::Config& dataSource=*i;
					std::string dname = dataSource.child("dataname").value();
					
					std::string dataSourcePath;
					dataSourcePath += dataRootPath;

					for (ConfigSet::const_iterator it = xmlGeoConnections.children().begin(); it != xmlGeoConnections.children().end(); ++it)
					{
						string strtemp = (*it).child("name").value();
						string strhah = dataSource.child("connection").value();
						string strFuck = (*it).child("address").value();

						if ((*it).child("name").value() == dataSource.child("connection").value())
						{
							dataSourcePath += (*it).child("address").value();
							break;
						}
					}

					dataSourcePath += "/"+ CodeHelp::UTF8_To_string(dname)+".fcs";
					dataSourcePath = CodeHelp::String_To_UTF8(dataSourcePath);

					std::string name = dataSource.child("name").value();
					std::map<std::string,std::string>::iterator itr = Dataname_Stylename.find(name);

					if (itr ==Dataname_Stylename.end())
					{
						continue;
					}
					//遍历要素符号特征数组
					for (ConfigSet::const_iterator i= fStyleGroups.children().begin();i!=fStyleGroups.children().end();++i)
					{
						const osgEarth::Config& geoStyle = *i;

						if (geoStyle.child("name").value()==itr->second)
						{
							osgEarth::Symbology::Style osgStyle;
							//将吉奥的要素特征转换为osgEarth中的要素特征
							if(geoStyle.child("type").value()=="point")//点特征
							{
								geo_pointStyle pointStyle;
								pointStyle.color.value = geoStyle.child("renditions").child("pointrendition").child("color").child("value").value();
								pointStyle.color.transparent = atoi(geoStyle.child("renditions").child("pointrendition").child("color").child("transparent").value().c_str());
								pointStyle.size = (float) atof(geoStyle.child("renditions").child("pointrendition").child("size").value().c_str());

								createStylefromGeostyle(pointStyle,osgStyle);
							}
                            //多边形特征
							else if(geoStyle.child("type").value()=="polygon")
							{
								geo_polyStyle polyStyle;
								polyStyle.FillColor.value = geoStyle.child("renditions").child("polygonrendition").child("fillcolor").child("value").value();
								polyStyle.FillColor.transparent = atoi(geoStyle.child("renditions").child("polygonrendition").child("fillcolor").child("transparent").value().c_str());
								polyStyle.EdgeColor.value = geoStyle.child("renditions").child("PolygonRendition").child("EdgeColor").child("Value").value();
								polyStyle.EdgeColor.transparent = atoi(geoStyle.child("renditions").child("polygonrendition").child("edgecolor").child("transparent").value().c_str());
								polyStyle.width = (float) atof(geoStyle.child("renditions").child("polygonrendition").child("width").value().c_str());

								createStylefromGeostyle(polyStyle,osgStyle);
							}
                            //线特征
							else if (geoStyle.child("type").value()=="line")
							{
								geo_lineStyle lineStyle;
								lineStyle.color.value = geoStyle.child("renditions").child("linerendition").child("color").child("value").value();
								lineStyle.color.transparent = atoi(geoStyle.child("renditions").child("linerendition").child("color").child("transparent").value().c_str());
								lineStyle.width = (float) atof(geoStyle.child("renditions").child("linerendition").child("width").value().c_str());

								createStylefromGeostyle(lineStyle,osgStyle);
							}

							dataPath_osgStyle.insert(std::map<std::string,osgEarth::Symbology::Style>::value_type(dataSourcePath,osgStyle));
							break;
						}
					}
				}

                //对读取到的xml文件中的每一个图层设置其相应的osgEarth中的要素图层
				for (std::map<std::string,osgEarth::Symbology::Style>::iterator itr=dataPath_osgStyle.begin();itr!=dataPath_osgStyle.end();++itr)
				{
					std::string dbpath=itr->first;
					osgEarth::Symbology::Style sty = itr->second;
                    //设置osgEarth中map中的矢量要素图层
					createOptionsandLayers(dbpath,height,sty,type,map);
				}
			}
			
             ///////////////////////////////////根据吉奥要素特征转换为osgEarth要素特征类osgEarth::Symbology::Style
			//对点特征处理
			static bool createStylefromGeostyle(geo_pointStyle gStyle,osgEarth::Symbology::Style& output)
			{
				output.getOrCreate<PointSymbol>()->size()=gStyle.size;
				osgEarth::Symbology::Color tcolor(gStyle.color.value);
				output.getOrCreate<PointSymbol>()->fill()->color()=osgEarth::Symbology::Color(tcolor.r(),tcolor.g(),tcolor.b(),(float)(gStyle.color.transparent/(float)255));

				return true;
			}
            //对多边形特征的处理
			static bool createStylefromGeostyle(geo_polyStyle gStyle,osgEarth::Symbology::Style& output,bool bline=false)
			{
				if (bline)
				{
					osgEarth::Symbology::Color tecolor(gStyle.EdgeColor.value);
					output.getOrCreateSymbol<osgEarth::Symbology::LineSymbol>()->stroke()->color()=osgEarth::Symbology::Color(tecolor.r(),tecolor.g(),tecolor.b(),(float)(gStyle.EdgeColor.transparent/(float)255));
					output.getOrCreateSymbol<osgEarth::Symbology::LineSymbol>()->stroke()->width() = gStyle.width;
					output.getOrCreateSymbol<osgEarth::Symbology::RenderSymbol>()->lighting() =false;
				}

				osgEarth::Symbology::Color tcolor(gStyle.FillColor.value);
				output.getOrCreate<PolygonSymbol>()->fill()->color()=osgEarth::Symbology::Color(tcolor.r(),tcolor.g(),tcolor.b(),(float)(gStyle.FillColor.transparent/(float)255));

				return true;
			}

             //对线特征的处理
			static bool createStylefromGeostyle(geo_lineStyle gStyle,osgEarth::Symbology::Style& output)
			{
				osgEarth::Symbology::Color tcolor(gStyle.color.value);
				output.getOrCreateSymbol<osgEarth::Symbology::LineSymbol>()->stroke()->color()=osgEarth::Symbology::Color(tcolor.r(),tcolor.g(),tcolor.b(),(float)(gStyle.color.transparent/(float)255));
				output.getOrCreateSymbol<osgEarth::Symbology::LineSymbol>()->stroke()->width() = gStyle.width;
				output.getOrCreateSymbol<osgEarth::Symbology::RenderSymbol>()->lighting() =false;

				return true;
			}

			/////////////////////////////////////create Option and Layer
			//根据获取的xml矢量要素配置文件中的属性以及渲染方式，创建osgEarth中的矢量图层参数类及其相应图层
		    static bool createOptionsandLayers(std::string dbPath,osgEarth::Symbology::Style& osgStyle,GeoVectorRenderType type,osgEarth::Map* map)
			{
				if (type == GEOVECTORMODEL)
				{
					GEOFeatureOptions geoFeaOpt;
					geoFeaOpt.url()=dbPath;

					FeatureDisplayLayout layout;
					osg::ref_ptr<StyleSheet> stys=new StyleSheet;
					//geoFeaOpt.createLayoutandStyle(layout,stys);
					//创建矢量图层
					geoFeaOpt.createLayoutandStyle(layout,osgStyle,stys);

					osgEarth::Drivers::FeatureGeomModelOptions geomOptions;
					geomOptions.featureOptions() = geoFeaOpt;
					geomOptions.layout()=layout;
					geomOptions.styles() = stys.release();
					geomOptions.depthTestEnabled()=false;
					/*geomOptions.styles() = new StyleSheet();
					Style style;
					style.setName("style");
					LineSymbol* ls = style.getOrCreateSymbol<LineSymbol>();
					ls->stroke()->color() = Color::Yellow;
					ls->stroke()->width() = 2.0f;
					style.getOrCreate<PolygonSymbol>()->fill()->color()=Color(Color::Cyan, 0.5);
					geomOptions.styles()->addStyle(style);*/
					//geomOptions.enableLighting() = false;

					osgEarth::ModelLayerOptions layerOptions( dbPath, geomOptions );
					osg::ref_ptr<osgEarth::ModelLayer> mlayer=new osgEarth::ModelLayer(layerOptions);
					//mlayer->setOpacity(0.3);
					map->addModelLayer(mlayer);
				}
                 //以矢量栅格化的方式渲染
				else if (type == GEOVECTORIMAGE)
				{
					GEOFeatureOptions geoFeaOpt;
					geoFeaOpt.url()=dbPath;
					
					GeoGlobe::GeoVector::GEORasterizerOptions rasterOptions;
					rasterOptions.featureOptions()=geoFeaOpt;
					rasterOptions.styles() = new StyleSheet();
					rasterOptions.styles()->addStyle(osgStyle);

					osgEarth::Drivers::ImageLayerOptions ilo=osgEarth::Drivers::ImageLayerOptions(dbPath,rasterOptions);
					ilo.maxLevel()=15;
					map->addImageLayer(new ImageLayer(ilo));
					//map->addImageLayer(new ImageLayer(dbPath,rasterOptions));
				}

				return false;
			}
            //根据获取的xml矢量要素配置文件中的属性以及渲染方式，创建osgEarth中的矢量图层参数类及其相应图层，并对矢量抬高一定高度
			static bool createOptionsandLayers(std::string dbPath,float height,osgEarth::Symbology::Style& osgStyle,GeoVectorRenderType type,osgEarth::Map* map)
			{
				if (type == GEOVECTORMODEL)
				{
					GEOFeatureOptions geoFeaOpt;
					geoFeaOpt.height()=height;
					geoFeaOpt.url()=dbPath;

					FeatureDisplayLayout layout;
					osg::ref_ptr<StyleSheet> stys=new StyleSheet;
					//geoFeaOpt.createLayoutandStyle(layout,stys);
					geoFeaOpt.createLayoutandStyle(layout,osgStyle,stys);

					m_Xmin = geoFeaOpt.m_Xmin; 
					m_Xmax = geoFeaOpt.m_Xmax;
					m_Ymin = geoFeaOpt.m_Ymin;
					m_Ymax = geoFeaOpt.m_Ymax;

					osgEarth::Drivers::FeatureGeomModelOptions geomOptions;
					geomOptions.featureOptions() = geoFeaOpt;
					geomOptions.layout()=layout;
					geomOptions.styles() = stys.release();
					geomOptions.depthTestEnabled()=false;
					//geomOptions.depthTestEnabled()=false();
					/*geomOptions.styles() = new StyleSheet();
					Style style;
					style.setName("style");
					LineSymbol* ls = style.getOrCreateSymbol<LineSymbol>();
					ls->stroke()->color() = Color::Yellow;
					ls->stroke()->width() = 2.0f;
					style.getOrCreate<PolygonSymbol>()->fill()->color()=Color(Color::Cyan, 0.5);
					geomOptions.styles()->addStyle(style);*/
					//geomOptions.enableLighting() = false;

					osgEarth::ModelLayerOptions layerOptions( dbPath, geomOptions );
					map->addModelLayer( new osgEarth::ModelLayer(layerOptions) );

				}
				else if (type == GEOVECTORIMAGE)
				{
					GEOFeatureOptions geoFeaOpt;
					geoFeaOpt.url()=dbPath;
					
					GeoGlobe::GeoVector::GEORasterizerOptions rasterOptions;
					rasterOptions.featureOptions()=geoFeaOpt;
					rasterOptions.styles() = new StyleSheet();
					rasterOptions.styles()->addStyle(osgStyle);

					map->addImageLayer( new ImageLayer(dbPath, rasterOptions) );
				}

				return false;
			}
		};

	}
}
#endif