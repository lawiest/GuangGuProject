#include <Analysis/AreaTrakerElement.h>
#include <PlaceName/GeoBillboard.h>
#include <osgEarth/GLUtils>
#include <OsgCoreSingleton.h>
#include <osgEarth/SpatialReference>
#include <tool/CodeTransformHelp.h>

using namespace osgEarth;

namespace GeoGlobe
{
	AreaTrakerElement::AreaTrakerElement()
	{
		m_bFinished = false;
		m_bIsDrag = false;

		m_ptrMapNode = GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode();
		const SpatialReference* geoSRS = m_ptrMapNode->getMapSRS()->getGeographicSRS();

		Geometry* geom = new Polygon();

		Style geomStyle;
		osgEarth::Features::Feature* feature = new osgEarth::Features::Feature(geom, geoSRS);
		feature->geoInterp() = GEOINTERP_RHUMB_LINE;

		geomStyle.getOrCreate<LineSymbol>()->stroke()->color() = Color::Lime;
		geomStyle.getOrCreate<LineSymbol>()->stroke()->width() = 3.0f;
		geomStyle.getOrCreate<LineSymbol>()->tessellationSize() = 75000;
		geomStyle.getOrCreate<LineSymbol>()->stroke()->stipplePattern() = 0x0F0F;
		geomStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
		geomStyle.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;
		//geomStyle.getOrCreate<RenderSymbol>()->depthOffset()->enabled() = true;
		//geomStyle.getOrCreate<RenderSymbol>()->depthOffset()->automatic() = true;
		geomStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::Navy, 0.5);
		m_ptrFeartureNode = new osgEarth::Annotation::FeatureNode(feature, geomStyle);

		m_ptrDisplayGroup = new osg::Group();
		m_ptrDisplayGroup->addChild(m_ptrFeartureNode);
		m_ptrMapNode->addChild(m_ptrDisplayGroup);
	}

	AreaTrakerElement::~AreaTrakerElement()
	{
		m_ptrMapNode->removeChild(m_ptrDisplayGroup);
	}

	void AreaTrakerElement::Calcuate()
	{
		double dblArea = 0.0;
		//计算面积
		//记录向量叉积之和
		double dblX = 0;
		double dblY = 0;
		double dblZ = 0;
		int nCount = m_vecWorld.size();
		osg::Vec3d ptA, ptB;
		osg::Vec3d ptGeo, vecCenter;
		for (int i = 0; i < nCount - 1; i++)
		{
			ptA = m_vecWorld.at(i);
			ptB = m_vecWorld.at(i + 1);

			dblX += ptA.y() * ptB.z() - ptA.z() * ptB.y();
			dblY += ptA.z() * ptB.x() - ptA.x() * ptB.z();
			dblZ += ptA.x() * ptB.y() - ptA.y() * ptB.x();

			ptGeo = m_vecWgs84.at(i);
			vecCenter.x() += ptGeo.x();
			vecCenter.y() += ptGeo.y();
			vecCenter.z() += ptGeo.z();
		}

		ptA = m_vecWorld.at(nCount - 1);
		ptB = m_vecWorld.at(0);

		dblX += ptA.y() * ptB.z() - ptA.z() * ptB.y();
		dblY += ptA.z() * ptB.x() - ptA.x() * ptB.z();
		dblZ += ptA.x() * ptB.y() - ptA.y() * ptB.x();

		//面积
		dblArea = 0.5 * sqrt(dblX * dblX + dblY * dblY + dblZ * dblZ);

		//中心点
		ptGeo = m_vecWgs84.at(nCount - 1);
		vecCenter.x() += ptGeo.x();
		vecCenter.y() += ptGeo.y();
		vecCenter.z() += ptGeo.z();

		vecCenter.x() = vecCenter.x() / nCount;
		vecCenter.y() = vecCenter.y() / nCount;
		vecCenter.z() = vecCenter.z() / nCount;


		osgEarth::GeoPoint geoPt = osgEarth::GeoPoint(m_ptrMapNode->getMapSRS(), vecCenter);

		osgEarth::GeoPoint geoTemp(m_ptrMapNode->getMapSRS(), vecCenter);
		osg::Vec3d tempWorld;
		geoTemp.toWorld(tempWorld);

		std::ostringstream ss;
		if (dblArea > 1e6)
		{
			double tmp = dblArea / 1e6;

			std::ostringstream oss;
			oss.precision(2);
			oss.setf(std::ios::fixed); 
			oss << tmp;

			ss << "面积：" << oss.str() << "平方千米" << "\n";
		}
		else
		{	
			double tmp = dblArea;

			std::ostringstream oss;
			oss.precision(2);
			oss.setf(std::ios::fixed);
			oss << tmp;

			ss << "面积：" << oss.str() << "平方米" << "\n";
		}

		OnFinish(dblArea);

		std::string strText = CodeHelp::String_To_UTF8(ss.str());
		osgEarth::Symbology::Style style;
		style.getOrCreate<osgEarth::Symbology::TextSymbol>()->size() = 18.0f;
		//chapin.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color()=Color::White;
		style.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()->color() = Color::White;
		style.getOrCreate<osgEarth::Symbology::TextSymbol>()->font() = "msyh.ttc";
		style.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
		osg::ref_ptr<GeoGlobe::PlaceName::GeoBillboard> ptrBillBoard = new GeoGlobe::PlaceName::GeoBillboard(geoPt, strText, style, true);
		ptrBillBoard->setCullingActive(false);

		m_ptrDisplayGroup->addChild(ptrBillBoard);
	}

	bool AreaTrakerElement::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object* object, osg::NodeVisitor* visitor)
	{
		if (m_bFinished)
			return false;

		osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());
		if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{

		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			if (m_bIsDrag)
			{
				m_bIsDrag = false;
				return false;
			}

			osg::Vec3d vecWord;
			m_ptrMapNode->getTerrain()->getWorldCoordsUnderMouse(aa.asView(), ea.getX(), ea.getY(), vecWord);
			m_vecWorld.push_back(vecWord);
			osgEarth::GeoPoint mapPoint;
			mapPoint.fromWorld(m_ptrMapNode->getMapSRS(), vecWord);
			m_vecWgs84.push_back(mapPoint.vec3d());
			if (mapPoint.x() < -180 || mapPoint.x() > 180 || mapPoint.y() < -90 || mapPoint.y() > 90 || fabs(mapPoint.z()) > 1000000)
				return false;

			osgEarth::Features::Feature* ptrFeature = m_ptrFeartureNode->getFeature();
			ptrFeature->getGeometry()->push_back(mapPoint.vec3d());
			m_ptrFeartureNode->dirty();

		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
		{
			m_bFinished = true;
			Calcuate();
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
		{
			osgEarth::Features::Feature* ptrFeature = m_ptrFeartureNode->getFeature();

			if (ptrFeature->getGeometry()->size() == 0)
				return false;

			osg::Vec3d vecWord;
			m_ptrMapNode->getTerrain()->getWorldCoordsUnderMouse(aa.asView(), ea.getX(), ea.getY(), vecWord);
			osgEarth::GeoPoint mapPoint;
			mapPoint.fromWorld(m_ptrMapNode->getMapSRS(), vecWord);

			if (mapPoint.x() < -180 || mapPoint.x() > 180 || mapPoint.y() < -90 || mapPoint.y() > 90 || fabs(mapPoint.z()) > 1000000)
				return false;

			if (ptrFeature->getGeometry()->size() == 1)
				ptrFeature->getGeometry()->push_back(mapPoint.vec3d());
			else
				ptrFeature->getGeometry()->back() = mapPoint.vec3d();

			m_ptrFeartureNode->dirty();
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::DRAG)
		{
			m_bIsDrag = true;
		}

		return false;
	}
}