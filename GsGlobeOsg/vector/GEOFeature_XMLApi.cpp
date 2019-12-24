
#include "Vector/GEOFeature_XMLApi.h"
#include <queue>
#include <istream>
#include <OsgCoreSingleton.h>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarth/GeoData>
#include <osgEarth/Bounds>
#include <osgEarth/XmlUtils>
#include <osg/Vec2d>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgEarthFeatures/Common>
#include <osgEarthFeatures/Feature>
#include <osgEarthSymbology/Style>
#include <osgEarth/StringUtils>

#include <osgEarth/GeoTransform>
#include <OsgCoreSingleton.h>
#include <osgUtil/Tessellator>
#include <tool/CodeTransformHelp.h>
#include <osg/Depth>
#include "RoadText.h"

using namespace osgEarth;
using namespace osgEarth::Features;
using namespace osgEarth::Symbology;

namespace GeoGlobe
{
	namespace GeoVector
	{
		GEOWFSFeatureClass::GEOWFSFeatureClass(std::string xmlPath): _wfsFeatureCollection(NULL)
		{
			m_vecOrigin = osg::Vec3d(0.0, 0.0, 0.0);
			m_dbElevation = 0.0;
			m_dbHigh = 0.0;
			m_nID = 0;
			m_OriginSRS = "";
			m_nsize = 0;

			_doc = new osgEarth::XmlDocument;
			std::ifstream fin(xmlPath.c_str(), std::ios::binary);
			_doc = osgEarth::XmlDocument::load(fin);

			_wfsFeatureCollection = static_cast<XmlElement*>(_doc->getChildren()[0].get());
		}

		GEOWFSFeatureClass::GEOWFSFeatureClass()
		{
			m_vecOrigin = osg::Vec3d(0.0, 0.0, 0.0);
			m_dbElevation = 0.0;
			m_dbHigh = 0.0;
			m_nID = 0;
			m_nsize = 0;

			m_OriginSRS = "";
		}

		GEOWFSFeatureClass::GEOWFSFeatureClass(XmlElement* root) : _wfsFeatureCollection(root)
		{
			osg::Vec3d m_vecOrigin = osg::Vec3d(0.0, 0.0, 0.0);
			m_dbElevation = 0.0;
			m_dbHigh = 0.0;
			m_nID = 0;
			m_nsize = 0;
			m_OriginSRS = "";
		}

		GEOWFSFeatureClass::~GEOWFSFeatureClass()
		{
			if (_doc)
			{
				delete _doc;
				_doc = NULL;
			}
		}

		void GEOWFSFeatureClass::setRoot(XmlElement* root)
		{
			_wfsFeatureCollection=root;
		}

		XmlElement* GEOWFSFeatureClass::getRoot()
		{
			return _wfsFeatureCollection;
		}

		//获取WFS要素图层的经纬度范围
		osgEarth::Bounds GEOWFSFeatureClass::getBounds()
		{
			if (!_wfsFeatureCollection)
			{
				return osgEarth::Bounds();
			}
				
			if (_wfsFeatureCollection->getSubElement("gml:boundedby"))
			{
				if (_wfsFeatureCollection->getSubElement("gml:boundedby")->getSubElement("gml:box"))
				{
					if (_wfsFeatureCollection->getSubElement("gml:boundedby")->getSubElement("gml:box")->getSubElement("gml:coordinates"))
					{
						std::vector<osg::Vec2d> points;
						splitCoordinates(_wfsFeatureCollection->getSubElement("gml:boundedby")->getSubElement("gml:box")->getSubElementText("gml:coordinates"),points);
						return osgEarth::Bounds(points[0].x(),points[0].y(),points[1].x(),points[1].y());
					}
				}
			}
			return osgEarth::Bounds();
		}

		//获取要素个数
		int GEOWFSFeatureClass::getFeatureCount()
		{
			if (!_wfsFeatureCollection)
			{
				return 0;
			}
			XmlNodeList featureMembers=_wfsFeatureCollection->getSubElements("gml:featuremember");
			return featureMembers.size();
		}

		//根据xml文件分离坐标，获取坐标点数组
		void GEOWFSFeatureClass::splitCoordinates( const std::string& src,std::vector<osg::Vec2d>& points)
		{
			std::string coordpair;
			osg::Vec2d point;

			int nend=0;
			int nbegin=0;
			while(nend!=-1)
			{
				nend = src.find_first_of(" ",nbegin);

				if (nend == -1)
				{
					coordpair = src.substr(nbegin,src.length()-nbegin);
				}
				else
				{
					coordpair = src.substr(nbegin,nend-nbegin);
				}

				int n = coordpair.find_first_of(",",0);
				point.x()=atof(coordpair.substr(0,n).c_str());
				point.y()=atof(coordpair.substr(n+1,coordpair.length()-n-1).c_str());
				points.push_back(point);

				nbegin = nend+1;
			}
		}

		//根据xml文件分离坐标，获取坐标点数组
		void GEOWFSFeatureClass::splitCoordinates(const std::string& src, std::vector<osg::Vec3d>& points)
		{
			std::string coordpair;
			osg::Vec3d point;

			int nend = 0;
			int nbegin = 0;
			while (nend != -1)
			{
				nend = src.find_first_of(" ", nbegin);

				if (nend == -1)
				{
					coordpair = src.substr(nbegin, src.length() - nbegin);
				}
				else
				{
					coordpair = src.substr(nbegin, nend - nbegin);
				}

				int n = coordpair.find_first_of(",", 0);
				point.x() = atof(coordpair.substr(0, n).c_str());
				point.y() = atof(coordpair.substr(n + 1, coordpair.length() - n - 1).c_str());
				int nLast = coordpair.find_last_of(",", coordpair.length());
				point.z() = atof(coordpair.substr(nLast + 1, coordpair.length()).c_str());
				points.push_back(point);

				nbegin = nend + 1;
			}
		}

		//根据ogc中的gml格式创建要素类，创建所有要素
		void GEOWFSFeatureClass::createFeaturesfromGML(const SpatialReference* srs,FeatureList& features)
		{
			if (!_wfsFeatureCollection)
			{
				return;
			}
			XmlNodeList featureMembers=_wfsFeatureCollection->getSubElements("gml:featuremember");

            //遍历要素列表
			for (XmlNodeList::const_iterator itr=featureMembers.begin();itr!=featureMembers.end();itr++)
			{
				XmlElement* e_featureMember = static_cast<XmlElement*>( itr->get() );
				XmlElement* e_feature = static_cast<XmlElement*>(e_featureMember->getChildren()[0].get());

				int fid =atoi(e_feature->getSubElementText("oid").c_str());

				XmlElement* e_geometry = e_feature->getSubElement("geometry");

				std::vector< osg::ref_ptr<osgEarth::Symbology::Geometry> > geoms;
                //根据gml创建该要素对应的几何体
				createGeometryfromGML(e_geometry,geoms);
				//根据几何体的类型设置其符号类型、颜色等
				for (int i=0;i<geoms.size();i++)
				{
					Style style;
						
					if (geoms[i]->getType()==osgEarth::Symbology::Geometry::TYPE_POINTSET)
					{
						style.getOrCreateSymbol<PointSymbol>()->size()=5.0;
						style.getOrCreateSymbol<PointSymbol>()->fill()=osgEarth::Symbology::Color::Blue;
						style.getOrCreateSymbol<RenderSymbol>()->depthTest()=false;
						style.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
						style.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
					}
					if (geoms[i]->getType()==osgEarth::Symbology::Geometry::TYPE_LINESTRING)
					{
						LineSymbol* ls = style.getOrCreateSymbol<LineSymbol>();
						ls->stroke()->color() = Color::Yellow;
						ls->stroke()->width() = 1.0f;
						style.getOrCreateSymbol<RenderSymbol>()->depthTest()=false;
						style.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
						style.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
					}
					if (geoms[i]->getType()==osgEarth::Symbology::Geometry::TYPE_POLYGON)
					{
						LineSymbol* ls = style.getOrCreateSymbol<LineSymbol>();
						ls->stroke()->color() = Color::Yellow;
						ls->stroke()->width() = 1.0f;
						style.getOrCreateSymbol<RenderSymbol>()->depthTest()=false;
						style.getOrCreateSymbol<PolygonSymbol>()->fill()->color()=osgEarth::Symbology::Color::Blue;
						style.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
						style.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
					}
					features.push_back(new Feature(geoms[i],srs,style,fid));
				}
					
			}
		}

		//根据要素的oid号创建该要素
		void GEOWFSFeatureClass::createFeaturesfromGMLbyOID(int oid,const SpatialReference* srs,FeatureList& features)
		{
			if (!_wfsFeatureCollection)
			{
				return;
			}
			XmlNodeList featureMembers=_wfsFeatureCollection->getSubElements("gml:featuremember");

			for (XmlNodeList::const_iterator itr=featureMembers.begin();itr!=featureMembers.end();itr++)
			{
				XmlElement* e_featureMember = static_cast<XmlElement*>( itr->get() );
				XmlElement* e_feature = static_cast<XmlElement*>(e_featureMember->getChildren()[0].get());

				int fid =atoi(e_feature->getSubElementText("oid").c_str());
				if (fid==oid)
				{
					XmlElement* e_geometry = e_feature->getSubElement("geometry");

					std::vector< osg::ref_ptr<osgEarth::Symbology::Geometry> > geoms;
					createGeometryfromGML(e_geometry,geoms);
					for (int i=0;i<geoms.size();i++)
					{
						Style style;

						if (geoms[i]->getType()==osgEarth::Symbology::Geometry::TYPE_POINTSET)
						{
							style.getOrCreateSymbol<PointSymbol>()->size()=5.0;
							style.getOrCreateSymbol<PointSymbol>()->fill()=osgEarth::Symbology::Color::Blue;
							style.getOrCreateSymbol<RenderSymbol>()->depthTest()=false;
						}
						if (geoms[i]->getType()==osgEarth::Symbology::Geometry::TYPE_LINESTRING)
						{
							LineSymbol* ls = style.getOrCreateSymbol<LineSymbol>();
							ls->stroke()->color() = Color::Yellow;
							ls->stroke()->width() = 1.0f;
							style.getOrCreateSymbol<RenderSymbol>()->depthTest()=false;
						}
						if (geoms[i]->getType()==osgEarth::Symbology::Geometry::TYPE_POLYGON)
						{
							LineSymbol* ls = style.getOrCreateSymbol<LineSymbol>();
							ls->stroke()->color() = Color::Yellow;
							ls->stroke()->width() = 1.0f;
							style.getOrCreateSymbol<RenderSymbol>()->depthTest()=false;
							//style.getOrCreateSymbol<PolygonSymbol>()->fill()->color()=osgEarth::Symbology::Color::Blue;
						}
						features.push_back(new Feature(geoms[i],srs,style,fid));
					}//for geom
					break;
				}//if(fid==oid)
				else
				{
					continue;
				}
			}//for featuremember
		}

		void GEOWFSFeatureClass::createFeaturesfromGML(const SpatialReference* srs,float height,FeatureList& features)
		{
			if (!_wfsFeatureCollection)
			{
				return;
			}
			XmlNodeList featureMembers=_wfsFeatureCollection->getSubElements("gml:featuremember");

			for (XmlNodeList::const_iterator itr=featureMembers.begin();itr!=featureMembers.end();itr++)
			{
				XmlElement* e_featureMember = static_cast<XmlElement*>( itr->get() );
				XmlElement* e_feature = static_cast<XmlElement*>(e_featureMember->getChildren()[0].get());

				int fid =atoi(e_feature->getSubElementText("oid").c_str());

				XmlElement* e_geometry = e_feature->getSubElement("geometry");

				std::vector< osg::ref_ptr<osgEarth::Symbology::Geometry> > geoms;
				createGeometryfromGML(e_geometry,height,geoms);
				for (int i=0;i<geoms.size();i++)
				{
					Style style;

					if (geoms[i]->getType()==osgEarth::Symbology::Geometry::TYPE_POINTSET)
					{
						style.getOrCreateSymbol<PointSymbol>()->size()=5.0;
						style.getOrCreateSymbol<PointSymbol>()->fill()=osgEarth::Symbology::Color::Blue;
						style.getOrCreateSymbol<RenderSymbol>()->depthTest()=false;
					}
					if (geoms[i]->getType()==osgEarth::Symbology::Geometry::TYPE_LINESTRING)
					{
						LineSymbol* ls = style.getOrCreateSymbol<LineSymbol>();
						ls->stroke()->color() = Color::Yellow;
						ls->stroke()->width() = 1.0f;
						style.getOrCreateSymbol<RenderSymbol>()->depthTest()=false;
					}
					if (geoms[i]->getType()==osgEarth::Symbology::Geometry::TYPE_POLYGON)
					{
						LineSymbol* ls = style.getOrCreateSymbol<LineSymbol>();
						ls->stroke()->color() = Color::Yellow;
						ls->stroke()->width() = 1.0f;
						style.getOrCreateSymbol<RenderSymbol>()->depthTest()=false;
						style.getOrCreateSymbol<PolygonSymbol>()->fill()->color()=osgEarth::Symbology::Color::Blue;
					}
					features.push_back(new Feature(geoms[i],srs,style,fid));
				}

			}
		}

		void GEOWFSFeatureClass::createFeaturesfromGML(const SpatialReference* srs,FeatureList& features,float transparent,float height)
		{
			if (!_wfsFeatureCollection)
			{
				return;
			}
			XmlNodeList featureMembers=_wfsFeatureCollection->getSubElements("gml:featuremember");

			for (XmlNodeList::const_iterator itr=featureMembers.begin();itr!=featureMembers.end();itr++)
			{
				XmlElement* e_featureMember = static_cast<XmlElement*>( itr->get() );
				XmlElement* e_feature = static_cast<XmlElement*>(e_featureMember->getChildren()[0].get());

				int fid =atoi(e_feature->getSubElementText("oid").c_str());
				m_dbElevation = atof(e_feature->getSubElementText("FDG").c_str());
				m_dbHigh = atof(e_feature->getSubElementText("LG").c_str());
				m_nID  = atoi(e_feature->getSubElementText("HBID").c_str());
				
				m_strRoadName = e_feature->getSubElementText("NAME").c_str();

				//临时保存Feature用于后续使用
				Style styleTemp;
				m_ptrFeature = new Feature(NULL, srs, styleTemp, fid);

				osgEarth::XmlNodeList children = e_feature->getChildren();

				for (int i = 0; i < children.size(); i++)
				{
					std::string strName = dynamic_cast<XmlElement*>(children.at(i).get())->getName();
					std::string strValue = dynamic_cast<XmlElement*>(children.at(i).get())->getText();
					m_ptrFeature->set(strName, strValue);
				}


				XmlElement* e_geometry = e_feature->getSubElement("geometry");

				std::vector< osg::ref_ptr<osgEarth::Symbology::Geometry> > geoms;
				createGeometryfromGML(e_geometry,height,geoms);
				for (int i = 0; i < geoms.size(); i++)
				{
					Style style;

					if (geoms[i]->getType() == osgEarth::Symbology::Geometry::TYPE_POINTSET)
					{
						style.getOrCreateSymbol<PointSymbol>()->size() = 5.0;
						style.getOrCreateSymbol<PointSymbol>()->fill() = Color(osgEarth::Symbology::Color::Blue, transparent);
						/*style.getOrCreateSymbol<RenderSymbol>()->depthTest()=false;
						style.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
						style.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;*/
					}
					if (geoms[i]->getType() == osgEarth::Symbology::Geometry::TYPE_LINESTRING)
					{
						LineSymbol* ls = style.getOrCreateSymbol<LineSymbol>();
						ls->stroke()->color() = Color(Color::Yellow, transparent);
						ls->stroke()->width() = 1.0f;
						//style.getOrCreateSymbol<RenderSymbol>()->depthTest()=false;
						//style.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
						//style.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
					}
					if (geoms[i]->getType() == osgEarth::Symbology::Geometry::TYPE_POLYGON)
					{
						LineSymbol* ls = style.getOrCreateSymbol<LineSymbol>();
						ls->stroke()->color() = Color(Color::Yellow, transparent);
						ls->stroke()->width() = 1.0f;
						style.getOrCreateSymbol<RenderSymbol>()->depthTest() = true;
						style.getOrCreateSymbol<PolygonSymbol>()->fill()->color() = Color(osgEarth::Symbology::Color::Cyan, transparent);
						//style.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
						//style.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
					}

					osg::ref_ptr<Feature> ptrFeature = new Feature(geoms[i], srs, style, fid);

					//osg::ref_ptr<osgEarth::Annotation::FeatureNode>  featureNode = new osgEarth::Annotation::FeatureNode(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode(), ptrFeature, style);
					//GLOBE_NAME::OsgCoreSingleton::Instance()->Root()->addChild(featureNode);

					if (m_OriginSRS != "EPSG:4547")
						features.push_back(ptrFeature);
				}
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////create geometry
		//创建点几何体
		Symbology::Geometry* GEOWFSFeatureClass::createPointfromGML(XmlElement* e_point)
		{
			Symbology::Geometry* output = new Symbology::PointSet(1);//only one point

			std::vector<osg::Vec2d> points;
			splitCoordinates(e_point->getSubElementText("gml:coordinates"),points);

			output->push_back(points[0].x(),points[0].y());

			return output;
		}

		//创建线几何体
		Symbology::Geometry* GEOWFSFeatureClass::createLinefromGML(XmlElement* e_line)
		{
			std::vector<osg::Vec2d> points;
			splitCoordinates(e_line->getSubElementText("gml:coordinates"),points);

			Symbology::Geometry* output =  new Symbology::LineString(points.size());

			for(int i=0;i<points.size();i++)
			{
				osg::Vec3d p(points[i],0.0);
				if ( output->size() == 0 || p != output->back() ) // remove dupes
					output->push_back( p );
			}
			return output;
		}

		//创建多边形几何体
		Symbology::Geometry* GEOWFSFeatureClass::createPolyfromGML(XmlElement* e_poly)
		{
			std::vector<osg::Vec2d> points;
			splitCoordinates(e_poly->getSubElementText("gml:coordinates"),points);

			Symbology::Geometry* output =  new Symbology::Polygon(points.size());

			for(int i=0;i<points.size();i++)
			{
				osg::Vec3d p(points[i],0.0);
				if ( output->size() == 0 || p != output->back() ) // remove dupes
					output->push_back( p );
			}
			output->rewind( Symbology::Ring::ORIENTATION_CCW );

			return output;
		}

        //创建不同类型的几何体
		void GEOWFSFeatureClass::createGeometryfromGML(XmlElement* e_geometry,std::vector< osg::ref_ptr<osgEarth::Symbology::Geometry> >& geoms)
		{
			XmlElement* e_gmlType = static_cast<XmlElement*>(e_geometry->getChildren()[0].get());
			std::string gmlType = e_gmlType->getName();
			if (gmlType == "gml:point")
			{
				geoms.push_back(createPointfromGML(e_gmlType));
			}

			if (gmlType == "gml:linestring")
			{
				geoms.push_back(createLinefromGML(e_gmlType));
			}

			if (gmlType == "gml:linearring")
			{
				geoms.push_back(createPolyfromGML(e_gmlType));
			}

			if (gmlType == "gml:multilinestring")
			{
				XmlNodeList linemembers=e_gmlType->getSubElements("gml:linestringmember");
				for (XmlNodeList::const_iterator itr=linemembers.begin();itr!=linemembers.end();itr++)
				{
					XmlElement* e_line = static_cast<XmlElement*>(itr->get())->getSubElement("gml:linestring");
					geoms.push_back(createLinefromGML(e_line));
				}
			}

			if (gmlType == "gml:polygon")
			{
				XmlElement*outer = e_gmlType->getSubElement("gml:outerboundaryis");
				if (outer)
				{
					XmlElement* e_poly = outer->getSubElement("gml:linearring");
					geoms.push_back(createPolyfromGML(e_poly));
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////height geometry
		Symbology::Geometry* GEOWFSFeatureClass::createPointfromGML(XmlElement* e_point,float height)
		{
			Symbology::Geometry* output = new Symbology::PointSet(1);//only one point
			std::vector<osg::Vec2d> points;
			splitCoordinates(e_point->getSubElementText("gml:coordinates"),points);

			output->push_back(points[0].x(),points[0].y(),height);

			return output;
		}

		osgText::Font* GEOWFSFeatureClass::getFont(std::string strFontName)
		{
			if (m_font == NULL)
				m_font = osgText::readFontFile(strFontName);

			return m_font;
		}

		osgText::Text* GEOWFSFeatureClass::createText(const osg::Vec3& pos, const std::string& content, float size, osg::Vec4 color, std::string strFont)
		{
			osgText::Text* ptrText = new osgText::Text;
			//osg::ref_ptr<osgText::Font> font = osgText::readFontFile(strFont);
			ptrText->setFont(getFont(m_strFontFile));
			ptrText->setCharacterSize(size);
			ptrText->setColor(color);
			ptrText->setAxisAlignment(osgText::TextBase::XY_PLANE);
			ptrText->setAlignment(osgText::TextBase::CENTER_CENTER);
			ptrText->setPosition(pos);
			//std::string str = CodeHelp::String_To_UTF8(content);
			ptrText->setText(content, osgText::String::ENCODING_UTF8);
			ptrText->setBackdropColor(m_shadowColor);
			ptrText->setBackdropOffset(m_dbOffset);
			ptrText->setBackdropType((osgText::Text::BackdropType)m_eShadowPos);

			ptrText->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
			ptrText->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false));
			return ptrText;
		}
		Symbology::Geometry* GEOWFSFeatureClass::createLinefromGML(XmlElement* e_line,float height)
		{
			std::vector<osg::Vec2d> points;
			splitCoordinates(e_line->getSubElementText("gml:coordinates"),points);

			if (m_nsize != 0)
			{
				//Symbology::Geometry* output = new Symbology::LineString(points.size());

				//for (int i = 0; i < points.size(); i++)
				//{
				//	osg::Vec3d p(points[i], height);
				//	if (output->size() == 0 || p != output->back()) // remove dupes
				//		output->push_back(p);
				//}
				//m_ptrFeature->setGeometry(output);
				//GLOBE_NAME::OsgCoreSingleton::Instance()->Features().push_back(m_ptrFeature);
				//叶节点
				std::vector<osg::ref_ptr<osg::Geode>> vTextGeode;
				int size = m_nsize;
				for (int i = 0; i < 5; i++)
				{
					osg::ref_ptr<osg::Geode> textGeode = new osg::Geode;
					textGeode->addDrawable(createText(osg::Vec3(0.0f, 0.0f, 0.0f), m_strRoadName, size, m_Color, m_strFontFile));
					textGeode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
					textGeode->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false));
					textGeode->getOrCreateStateSet()->setRenderBinDetails(12, "RenderBin");
					vTextGeode.push_back(textGeode);
					//改变字体大小
					size *= 1.5;
				}				

				int nIndex = 0;
				double fLineLength = 0.0f;
				for (int i = 0; i < points.size() - 1; i++)
				{
					if (m_strRoadName =="")
						continue;

					osg::Vec3d vecPoint1 = osg::Vec3d(points[i].x(), points[i].y(), 45.0);
					osg::Vec3d vecPoint2 = osg::Vec3d(points[i + 1].x(), points[i + 1].y(), 45.0);
					//if (points.size() < 3)
					//	vecPoint2 = osg::Vec3d(points[i + 1].x(), points[i + 1].y(), 30.0);
					//else
					//	vecPoint2 = osg::Vec3d(points[i + 2].x(), points[i + 2].y(), 30.0);

					osgEarth::GeoPoint geoPoint1 = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), vecPoint1);
					osgEarth::GeoPoint geoPoint2 = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), vecPoint2);

					osg::Vec3d word1;
					geoPoint1.toWorld(word1);
					osg::Vec3d word2;
					geoPoint2.toWorld(word2);


					double dbLenth = (word1 - word2).length();
					double nameLenth = m_strRoadName.size() * m_nsize;

					//if (dbLenth - nameLenth / 2 >= 0)
					{
						//createGroundTextNode(vecPoint1, vecPoint2, m_strName);
						RoadText* ptrRoad = new RoadText(fLineLength,vecPoint1, vecPoint2, vTextGeode);
						//GLOBE_NAME::OsgCoreSingleton::Instance()->LonAndLat()->Change().Add(ptrRoad, &RoadText::OnChange);
					}
				}
				return NULL;
			}
			else
			{
				Symbology::Geometry* output =  new Symbology::LineString(points.size());

				for(int i=0;i<points.size();i++)
				{
					osg::Vec3d p(points[i],height);
					if ( output->size() == 0 || p != output->back() ) // remove dupes
						output->push_back( p );
				}
				return output;
			}
		}

		void GEOWFSFeatureClass::CreatePolygon(osg::Vec3Array* ptrVertices,double x,double y,double z)
		{
			osg::ref_ptr<osg::Geode> geode = new osg::Geode();

			// create Geometry object to store all the vertices and lines primitive.
			osg::ref_ptr<osg::Geometry> polyGeom = new osg::Geometry();

			polyGeom->setVertexArray(ptrVertices);

			polyGeom->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
			polyGeom->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

			osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
			shared_colors->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
			// same trick for shared normal.
			osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
			shared_normals->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
			// use the shared color array.
			polyGeom->setColorArray(shared_colors.get(), osg::Array::BIND_OVERALL);
			// use the shared normal array.
			polyGeom->setNormalArray(shared_normals.get(), osg::Array::BIND_OVERALL);

			// This time we simply use primitive, and hardwire the number of coords to use
			// since we know up front,
			polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, ptrVertices->size()));


			osg::ref_ptr<osgUtil::Tessellator> tscx = new osgUtil::Tessellator();
			tscx->setTessellationNormal(osg::Vec3(0.0, 0.0, 1.0));
			tscx->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
			tscx->setBoundaryOnly(false);
			tscx->setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD); // the commonest tessellation is default, ODD. GE2 allows intersections of constraints to be found.
			tscx->retessellatePolygons(*(polyGeom)); // this should insert extra vertices where constraints overlap

			//printTriangles("Polygon", *polyGeom);

			// add the points geometry to the geode.
			geode->addDrawable(polyGeom);
			geode->setName("WFSVector");
			geode->setUserValue("Elevation", m_dbElevation);
			geode->setUserValue("High", m_dbHigh);
			geode->setUserValue("ID", m_nID);
			geode->setUserValue("OriginSRS", m_OriginSRS);
			geode->setUserValue("OriginPoint", m_vecOrigin);
			geode->setUserData(m_ptrFeature.release());

			osg::ref_ptr<osgEarth::GeoTransform> ptrLocator = new osgEarth::GeoTransform();
			ptrLocator->addChild(geode);
			GeoPoint point = GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(),osg::Vec3d(x, y, z));
			ptrLocator->setPosition(point);

			//osg::ref_ptr<osg::LOD> ptrLod = new osg::LOD();
			//ptrLod->addChild(ptrLocator,0,2500);

			//存起来准备以后用
			//GLOBE_NAME::OsgCoreSingleton::Instance()->Vector()->addChild(geode);
			//到主场景中渲染
			GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->addChild(ptrLocator);
		}

		Symbology::Geometry* GEOWFSFeatureClass::createPolyfromGML(XmlElement* e_poly,float height)
		{
			std::vector<osg::Vec2d> points;
			splitCoordinates(e_poly->getSubElementText("gml:coordinates"),points);
			Symbology::Geometry* output =  new Symbology::Polygon(points.size());

			osg::ref_ptr<osg::Vec3Array> ptrVertices = new osg::Vec3Array();

			for(int i=0;i<points.size();i++)
			{
				osg::Vec3d p;
				if (m_OriginSRS == "EPSG:4547")
				{
					//osg::Vec3d vecWorld;
					//osgEarth::GeoPoint geoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMap()->getSRS(), vecOut);
					//geoPoint.toWorld(vecWorld);
					osg::Vec2d zeroPos = osg::Vec2d(points[i].x() - m_vecOrigin.x(), points[i].y() - m_vecOrigin.y());
					//geoPoint.fromWorld(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMap()->getSRS(), vecWorld + zeroPos);

					ptrVertices->push_back(osg::Vec3(zeroPos,/*-500*/m_dbElevation));

					p = osg::Vec3d(zeroPos, m_dbElevation);
				}
				else
				{
					p = osg::Vec3d(points[i], /*-500*/m_dbElevation);
				}
				
				if ( output->size() == 0 || p != output->back() ) // remove dupes
					output->push_back( p );
			}

			osg::ref_ptr<osgEarth::SpatialReference> ptrSrs = osgEarth::SpatialReference::create(m_OriginSRS);
			osg::Vec3d vecOutWgs84;
			if (ptrSrs != NULL)
				ptrSrs->transform(m_vecOrigin, GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMap()->getSRS(), vecOutWgs84);

			m_ptrFeature->setGeometry(output);
			CreatePolygon(ptrVertices, vecOutWgs84.x(), vecOutWgs84.y(), vecOutWgs84.z());
		
			output->rewind( Symbology::Ring::ORIENTATION_CCW );
			return output;
		}

		void GEOWFSFeatureClass::createGeometryfromGML(XmlElement* e_geometry,float height,std::vector< osg::ref_ptr<osgEarth::Symbology::Geometry> >& geoms)
		{

			XmlElement* e_gmlType = static_cast<XmlElement*>(e_geometry->getChildren()[0].get());
			std::string gmlType = e_gmlType->getName();
			if (gmlType == "gml:point")
			{
				geoms.push_back(createPointfromGML(e_gmlType,height));
			}

			if (gmlType == "gml:linestring")
			{
				Symbology::Geometry* ptrGeo = createLinefromGML(e_gmlType, height);
				if (ptrGeo)
					geoms.push_back(ptrGeo);
			}

			if (gmlType == "gml:linearring")
			{
				geoms.push_back(createPolyfromGML(e_gmlType,height));
			}

			if (gmlType == "gml:multilinestring")
			{
				XmlNodeList linemembers=e_gmlType->getSubElements("gml:linestringmember");
				for (XmlNodeList::const_iterator itr=linemembers.begin();itr!=linemembers.end();itr++)
				{
					XmlElement* e_line = static_cast<XmlElement*>(itr->get())->getSubElement("gml:linestring");

					Symbology::Geometry* ptrGeo = createLinefromGML(e_line, height);
					if (ptrGeo)
						geoms.push_back(ptrGeo);
				}
			}

			if (gmlType == "gml:polygon")
			{
				XmlElement*outer = e_gmlType->getSubElement("gml:outerboundaryis");
				if (outer)
				{
					XmlElement* e_poly = outer->getSubElement("gml:linearring");
					geoms.push_back(createPolyfromGML(e_poly,height));
				}
			}
		}
	}
}