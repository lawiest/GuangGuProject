
#include <osgEarth/Horizon>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgEarth/SpatialReference>
#include <osgEarth/MapNode>
#include <osgEarth/GeoTransform>
#include <osgUtil/SmoothingVisitor>
#include <osgUtil/Tessellator>
#include "OsgCoreSingleton.h"
#include "WFSFeaturePicker.h"
#include <osgEarthFeatures/Feature>
#include <Feature.h>
#include <geometry.h>
#include <OsgCoreSingleton.h>
#include <osgEarth/XmlUtils>
#include <Analysis/SphereElement.h>

namespace GeoGlobe 
{

	WFSFeaturePicker::WFSFeaturePicker(const PickerParamter& pickParamter)
	{
		m_pickParamter = pickParamter;
		ptrBuilding = new osg::Group();
		m_dbFDG = 0.0;
		m_dbLG = 0.0;
		m_dbPickHeight = 0.0;
		m_isDrag = false;
	}

	WFSFeaturePicker::~WFSFeaturePicker()
	{

	}

	std::string WFSFeaturePicker::Request(osg::Vec3 point)
	{
		osg::Vec3d vecOut;
		osg::Matrix mt_inverse;
		mt_inverse = mt_inverse.inverse(m_pickParamter.transMatrix);
		vecOut = point * mt_inverse;
		vecOut += m_pickParamter.vecOrigin;

		m_dbPickHeight = vecOut.z();
		std::string strUrl = m_pickParamter.strUrl;
		std::string strTypeName = m_pickParamter.strTypeName;
		std::string strCoord;

		std::ostringstream stream;
		stream.precision(18);
		stream << vecOut.x();
		strCoord += stream.str();
		strCoord += ",";
		stream.str("");
		stream << vecOut.y();
		strCoord += stream.str();

		std::string strRequesUrl = strUrl + "?VERSION=1.0.0&REQUEST=GetFeature&SERVICE=WFS&" +
			std::string("TYPENAME=") + strTypeName + std::string("&FILTER=") +
			std::string("<ogc:Filter%20xmlns:ogc=\"http://www.opengis.net/ogc\">") +
			std::string("<ogc:Intersects>") +
			std::string("<gml:Point%20xmlns:gml=\"http://www.opengis.net/gml\">") +
			std::string("<gml:coordinates>") + strCoord + std::string("</gml:coordinates>") +
			std::string("</gml:Point>") +
			std::string("</ogc:Intersects>") +
			std::string("</ogc:Filter>");

		osgEarth::HTTPResponse ptrResponse = osgEarth::HTTPClient::get(strRequesUrl);
		std::string data = ptrResponse.getPartAsString(0);

		return data;
	}

	bool WFSFeaturePicker::getBestFeature(osgEarth::XmlNodeList featureList, osgEarth::XmlElement& featue)
	{
		if (featureList.size() == 1)
		{
			osgEarth::XmlElement* featureRoot = static_cast<osgEarth::XmlElement*>(featureList[0].get());
			osgEarth::XmlElement* feature = static_cast<osgEarth::XmlElement*>(featureRoot->getChildren()[0].get());
			featue = *feature;
			return true;
		}

		for (osgEarth::XmlNodeList::const_iterator itr = featureList.begin(); itr != featureList.end(); itr++)
		{
			osgEarth::XmlElement* featureRoot = static_cast<osgEarth::XmlElement*>(itr->get());
			osgEarth::XmlElement* feature = static_cast<osgEarth::XmlElement*>(featureRoot->getChildren()[0].get());
			double dbFDG = atof(feature->getSubElementText("FDG").c_str());

			//加个5米的阙值
			if (dbFDG+5 >= m_dbPickHeight)
			{
				featue = *feature;
				return true;
			}
		}

		return false;
	}

	void WFSFeaturePicker::DealWithRelative(int nId)
	{
		std::string strUrl = m_pickParamter.strUrl;
		std::string strTypeName = m_pickParamter.strTypeName;

		std::string strRequesUrl = strUrl + "?VERSION=1.0.0&REQUEST=GetFeature&SERVICE=WFS&" +
			std::string("TYPENAME=") + strTypeName + std::string("&FILTER=") +
			std::string("<ogc:Filter%20xmlns:ogc=\"http://www.opengis.net/ogc\">") +
			std::string("<ogc:PropertyIsEqualTo>") +
			std::string("<ogc:PropertyName>HBID</ogc:PropertyName>") +
			std::string("<ogc:Literal>")+
			std::to_string(nId)+
			std::string("</ogc:Literal>") +
			std::string("</ogc:PropertyIsEqualTo>") +
			std::string("</ogc:Filter>");

		osgEarth::HTTPResponse ptrResponse = osgEarth::HTTPClient::get(strRequesUrl);
		std::string data = ptrResponse.getPartAsString(0);


		std::istringstream istream(data);
		osg::ref_ptr<osgEarth::XmlDocument> ptrDocument = osgEarth::XmlDocument::load(istream);
		if (NULL == ptrDocument)
			return;

		osg::ref_ptr<osgEarth::XmlElement> ptrRoot = dynamic_cast<osgEarth::XmlElement*>(ptrDocument->getChildren()[0].get());
		if (NULL == ptrRoot)
			return;

		osgEarth::XmlNodeList featureList = ptrRoot->getSubElements("gml:featuremember");
		for (int i = 0; i < featureList.size(); i++)
		{
			osgEarth::XmlElement* featureRoot = static_cast<osgEarth::XmlElement*>(featureList[i].get());
			osgEarth::XmlElement* feature = static_cast<osgEarth::XmlElement*>(featureRoot->getChildren()[0].get());
			BuildFeature(feature);
		}
	}

	void WFSFeaturePicker::ParseXML(std::string strData)
	{
		std::istringstream istream(strData);
		osg::ref_ptr<osgEarth::XmlDocument> ptrDocument = osgEarth::XmlDocument::load(istream);
		if (NULL == ptrDocument)
			return ;

		osg::ref_ptr<osgEarth::XmlElement> ptrRoot = dynamic_cast<osgEarth::XmlElement*>(ptrDocument->getChildren()[0].get());
		if (NULL == ptrRoot)
			return ;

		osgEarth::XmlNodeList featureList = ptrRoot->getSubElements("gml:featuremember");
		if (featureList.size() <= 0)
			return ;

		osg::ref_ptr<osgEarth::XmlElement> feature = new osgEarth::XmlElement("");
		bool bIsOk = getBestFeature(featureList, *feature);

		if (bIsOk == false)
			return ;

		int fid = atoi(feature->getSubElementText("oid").c_str());
		m_dbFDG = atof(feature->getSubElementText("FDG").c_str());
		m_dbLG = atof(feature->getSubElementText("LG").c_str());
		int nHBID = atoi(feature->getSubElementText("HBID").c_str());

		if (nHBID != 0)
		{
			DealWithRelative(nHBID);
		}

		BuildFeature(feature);
	}

	void WFSFeaturePicker::BuildFeature(osgEarth::XmlElement * ptrFeature)
	{
		osgEarth::XmlNodeList children = ptrFeature->getChildren();

		GLOBE_NAME::Feature geoFeature;

		for (int i = 0; i < children.size(); i++)
		{
			std::string strName = dynamic_cast<osgEarth::XmlElement*>(children.at(i).get())->getName();
			std::string strValue = dynamic_cast<osgEarth::XmlElement*>(children.at(i).get())->getText();
			geoFeature.m_attr[strName] = strValue;
		}

		osgEarth::XmlElement* ptrXMLGeometry = ptrFeature->getSubElement("geometry");

		osg::ref_ptr<osgEarth::XmlElement> ptrGMLType = static_cast<osgEarth::XmlElement*>(ptrXMLGeometry->getChildren()[0].get());
		std::string gmlType = ptrGMLType->getName();
		if (gmlType == "gml:point") {}
		else if (gmlType == "gml:linestring") {}
		else if (gmlType == "gml:linearring") {}
		else if (gmlType == "gml:multilinestring") {}
		else if (gmlType == "gml:polygon")
		{
			osg::ref_ptr<osgEarth::XmlElement> ptrOuter = ptrGMLType->getSubElement("gml:outerboundaryis");
			if (ptrOuter)
			{
				osg::ref_ptr<osgEarth::XmlElement> ptrPolygon = ptrOuter->getSubElement("gml:linearring");

				osg::ref_ptr<osg::Vec3Array> ptrVertex = new osg::Vec3Array();
				splitCoordinates(ptrPolygon->getSubElementText("gml:coordinates"), ptrVertex, geoFeature);
				OnPicked(geoFeature);
				Build(ptrVertex);
			}
		}
	}

	bool WFSFeaturePicker::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			if (m_isDrag)
			{
				m_isDrag = false;
				return false;
			}

			osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
			osgUtil::LineSegmentIntersector::Intersections intersections;
			view->computeIntersections(ea.getX(), ea.getY(), intersections);

			ClearPreHighLight();

			osgUtil::LineSegmentIntersector::Intersections::iterator hitr, hitrShortest;
			for (hitr = intersections.begin(); hitr != intersections.end(); hitr++)
			{
				osg::NodePath nodePath = hitr->nodePath;
				for (int i = 0; i < nodePath.size(); i++)
				{
					osg::ref_ptr<osg::Node> node = nodePath[i];
					if (node == NULL)
						continue;

					if (node->getName() == "OsgbLayer")
					{
						osg::Vec3d world = hitr->getWorldIntersectPoint();
						
						std::string data = Request(world);
						ParseXML(data);

						return false;
					}
				}
			}

			m_isDrag = false;
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::DRAG)
		{
			m_isDrag = true;
		}
		return false;
	}

	void WFSFeaturePicker::splitCoordinates(const std::string& src, osg::Vec3Array* ptrVertex, GLOBE_NAME::Feature& geoFeature)
	{
		std::string coordpair;
		osg::Vec3 point;

		osg::ref_ptr<osgEarth::SpatialReference> ptrSrs = osgEarth::SpatialReference::create(m_pickParamter.strOriginSrs);

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

			point.x() = atof(coordpair.substr(0, n).c_str()) - m_pickParamter.vecOrigin.x();
			point.y() = atof(coordpair.substr(n + 1, coordpair.length() - n - 1).c_str()) - m_pickParamter.vecOrigin.y();
			point.z() = m_dbFDG; 
			
			if (ptrSrs != NULL)
			{
				osg::Vec3d vecOutWgs84;
				osg::Vec3 vec = osg::Vec3(point.x() + m_pickParamter.vecOrigin.x(), point.y() + m_pickParamter.vecOrigin.y(), 0);
				ptrSrs->transform(m_pickParamter.vecOrigin, GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMap()->getSRS(), vecOutWgs84);
				geoFeature.m_geo.push_back(KERNEL_NAME::GsRawPoint3D(vecOutWgs84.x(), vecOutWgs84.y(), point.z()));
			}

			ptrVertex->push_back(point);

			nbegin = nend + 1;
		}
	}

	void WFSFeaturePicker::Build(osg::Vec3Array* ptrVertex)
	{
		//先处理拾取到的矢量
		if (ptrVertex)
			BuildHouse(ptrVertex);

		//如果id不为零处理有关联的节点
	}

	void WFSFeaturePicker::ClearPreHighLight()
	{
		for (int i = 0; i < ptrBuilding->getNumChildren(); i++)
		{
			osg::Node* ptrNode = ptrBuilding->getChild(i);
			GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->removeChild(ptrNode);
		}
	}

	void WFSFeaturePicker::BuildHouse(osg::Vec3Array* ptrVertex)
	{
		if (NULL == ptrVertex)
			return;

		osg::ref_ptr<osg::Vec3Array> ptrVec = ptrVertex;

		osg::ref_ptr<osgEarth::SpatialReference> ptrSrs = osgEarth::SpatialReference::create(m_pickParamter.strOriginSrs);
		osg::Vec3d vecOutWgs84;
		if (ptrSrs != NULL)
			ptrSrs->transform(m_pickParamter.vecOrigin, GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMap()->getSRS(), vecOutWgs84);

		//ptrVec = EnlargeBase(ptrGeo, vecOutWgs84);
		//底顶点
		osg::ref_ptr<osg::Vec3Array> ptrVecBottom = new osg::Vec3Array();
		//顶顶点
		osg::ref_ptr<osg::Vec3Array> ptrVecRoof = new osg::Vec3Array();
		for (int i = 0; i < ptrVec->size(); i++)
		{
			ptrVecBottom->push_back(osg::Vec3((*ptrVec)[i].x(), (*ptrVec)[i].y(), (*ptrVec)[i].z() - m_dbLG));
			ptrVecRoof->push_back(osg::Vec3((*ptrVec)[i].x(), (*ptrVec)[i].y(), (*ptrVec)[i].z() + 5));
		}

		//封顶
		CreatePolygon(ptrVecRoof, vecOutWgs84);
		//封底
		CreatePolygon(ptrVecBottom, vecOutWgs84);
		//墙面
		CreateWall(ptrVecRoof, ptrVecBottom, vecOutWgs84);
	}

	void WFSFeaturePicker::CreateWall(osg::Vec3Array* ptrVec, osg::Vec3Array* ptrVecH, osg::Vec3d vecOrigin)
	{
		osg::ref_ptr<osg::Geode> geode = new osg::Geode();
		for (int i = 0; i < ptrVec->size() - 1; i++)
		{
			osg::ref_ptr<osg::Geometry> ptrGeo = new osg::Geometry();
			osg::ref_ptr<osg::Vec3Array> ptrWall = new osg::Vec3Array();
			ptrWall->push_back(osg::Vec3((*ptrVec)[i].x(), (*ptrVec)[i].y(), (*ptrVec)[i].z()));
			ptrWall->push_back(osg::Vec3((*ptrVec)[i + 1].x(), (*ptrVec)[i + 1].y(), (*ptrVec)[i + 1].z()));
			ptrWall->push_back(osg::Vec3((*ptrVecH)[i + 1].x(), (*ptrVecH)[i + 1].y(), (*ptrVecH)[i + 1].z()));
			ptrWall->push_back(osg::Vec3((*ptrVecH)[i].x(), (*ptrVecH)[i].y(), (*ptrVecH)[i].z()));
			ptrWall->push_back(osg::Vec3((*ptrVec)[i].x(), (*ptrVec)[i].y(), (*ptrVec)[i].z()));

			ptrGeo->setVertexArray(ptrWall);
			ptrGeo->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
			ptrGeo->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
			colors->push_back(osg::Vec4(0.0f, 1.0f, 1.0f, 0.5f));
			//osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
			//normals->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
			ptrGeo->setColorArray(colors.get(), osg::Array::BIND_OVERALL);
			//ptrGeo->setNormalArray(normals.get(), osg::Array::BIND_OVERALL);
			ptrGeo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, ptrWall->size()));
			osgUtil::SmoothingVisitor::smooth(*ptrGeo, osg::DegreesToRadians(30.0));
			/*
			osg::ref_ptr<osgUtil::Tessellator> tscx = new osgUtil::Tessellator();
			tscx->setTessellationNormal(osg::Vec3(0.0, 0.0, 1.0));
			tscx->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
			tscx->setBoundaryOnly(false);
			tscx->setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD);
			tscx->retessellatePolygons(*(ptrGeo));
			*/

			geode->addDrawable(ptrGeo);
			geode->setName("Building Wall");
		}

		osg::ref_ptr<osgEarth::GeoTransform> ptrLocator = new osgEarth::GeoTransform();
		ptrLocator->addChild(geode);
		osgEarth::GeoPoint point = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), osg::Vec3d(vecOrigin.x(), vecOrigin.y(), vecOrigin.z()));
		ptrLocator->setPosition(point);

		ptrBuilding->addChild(ptrLocator);
		GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->addChild(ptrLocator);

	}

	void WFSFeaturePicker::CreatePolygon(osg::Vec3Array* ptrVertices, osg::Vec3d vecOrigin)
	{
		osg::Geode* geode = new osg::Geode();

		// create Geometry object to store all the vertices and lines primitive.
		osg::Geometry* polyGeom = new osg::Geometry();
		// pass the created vertex array to the points geometry object.
		polyGeom->setVertexArray(ptrVertices);

		polyGeom->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		polyGeom->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(0.0f, 1.0f, 1.0f, 0.5f));
		// same trick for shared normal.
		osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
		normals->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
		// use the shared color array.
		polyGeom->setColorArray(colors.get(), osg::Array::BIND_OVERALL);
		// use the shared normal array.
		polyGeom->setNormalArray(normals.get(), osg::Array::BIND_OVERALL);

		// This time we simply use primitive, and hardwire the number of coords to use
		// since we know up front,
		polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, ptrVertices->size()));

		osg::ref_ptr<osgUtil::Tessellator> tscx = new osgUtil::Tessellator();
		tscx->setTessellationNormal(osg::Vec3(0.0, 0.0, 1.0));
		tscx->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
		tscx->setBoundaryOnly(false);
		tscx->setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD);
		tscx->retessellatePolygons(*(polyGeom));

		osgUtil::SmoothingVisitor::smooth(*polyGeom, osg::DegreesToRadians(30.0));
		geode->addDrawable(polyGeom);
		geode->setName("roof");
		osg::ref_ptr<osgEarth::GeoTransform> ptrLocator = new osgEarth::GeoTransform();
		ptrLocator->addChild(geode);
		osgEarth::GeoPoint point = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), osg::Vec3d(vecOrigin.x(), vecOrigin.y(), vecOrigin.z()));
		ptrLocator->setPosition(point);

		ptrBuilding->addChild(ptrLocator);

		GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->addChild(ptrLocator);
	}

}