#include "ObliqueImageLayer.h"
#include "innerimp.h"
#include "tool/ObliqueImage.h"
#include <osgEarth/GeoTransform>
#include "OsgCoreSingleton.h"
#include <osgEarth/MapNode>

#include <osg/TextureRectangle>
#include <osg/ImageStream>
#include <tool/OverlayNode.h>
#include <osg/Math>
#include <osg/LineWidth>
#include <OsgCoreSingleton.h>
#include <osgearth/MapNode>
#include <tool/WFSFeaturePicker.h>

class ReadOSGBFileCallback : public osgDB::Registry::ReadFileCallback
{
public:

	virtual osgDB::ReaderWriter::ReadResult readNode(const std::string& fileName, const osgDB::ReaderWriter::Options* options)
	{
		//判断是否调用该插件
		if (osgDB::getFileExtension(fileName) == "osgb")
		{
			osgDB::ReaderWriter::ReadResult result = osgDB::Registry::instance()->readNodeImplementation(fileName, options);
			osg::ref_ptr<osg::Node> node = result.getNode();

			osg::ref_ptr<osg::PagedLOD> lod = dynamic_cast<osg::PagedLOD*>(node.get());
			if (lod != NULL)
			{
				osg::LOD::RangeList rangeList = lod->getRangeList();

				for (int i = 0; i < rangeList.size(); i++)
				{
					rangeList[i].first = rangeList[i].first * 2;
					rangeList[i].second = rangeList[i].second * 2;

					//if (i == 0)
					//	lod->setRange(i, dbLod, dbLod + rangeList[i].second/* * dbLod*/);
					//else
					//	lod->setRange(i, rangeList[i - 1].second, rangeList[i].first + rangeList[i].second/* * dbLod*/);
				}

				lod->setRangeList(rangeList);
			}

			if (node)
			{

				if (node.valid())
				{
					return osgDB::ReaderWriter::ReadResult(node.get(), osgDB::ReaderWriter::ReadResult::FILE_LOADED);
				}
				else
				{
					return osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND;
				}
			}
		}
		return osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND;
	}
};


GLOBE_NS

class VisibleNodeVisitor :public osg::NodeVisitor
{
public:

	VisibleNodeVisitor():osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
	{
		setNodeMaskOverride(1);
		m_bVisible = false;
	}

	virtual ~VisibleNodeVisitor(){}

	void setVisible(bool bVisible)
	{
		m_bVisible = bVisible;
	}

	virtual void apply(osg::PagedLOD& plod)
	{
		setVisible(plod);
		traverse(plod);
	}

	virtual void apply(osg::LOD& lod)
	{
		setVisible(lod);
		traverse(lod);
	}

	virtual void apply(osg::Group& group)
	{
		setVisible(group);
		traverse(group);
	}

	virtual void apply(osg::Node& node)
	{
		setVisible(node);
		traverse(node);
	}

	virtual void apply(osg::Geode& geode)
	{
		setVisible(geode);
		traverse(geode);
	}

	template<class T>
	void setVisible(T& node)
	{
		if (m_bVisible)
			node.setNodeMask(1);
		else
			node.setNodeMask(0);
	}

private:
	bool m_bVisible;
};

GeoGlobe::PickerParamter g_pickParamter;

void ObliqueImageLayer::StartChangeRangList()
{
	osgDB::Registry::instance()->setReadFileCallback(new ReadOSGBFileCallback());
}

void ObliqueImageLayer::setLod(double dbLod)
{
	GeoGlobe::CObliqueImageLoader::SetLod(dbLod,0);
}

void ObliqueImageLayer::Pick(const GsRawPoint3D& pos)
{
	if (m_bIsPick && m_ptrPickHandle != NULL)
	{
		osg::ref_ptr<GeoGlobe::WFSFeaturePicker> ptrWFSFeaturePicker = m_ptrPickHandle->CastTo<osg::ref_ptr<GeoGlobe::WFSFeaturePicker> >();
		if (ptrWFSFeaturePicker)
		{
			osg::Vec3d vecPos = osg::Vec3d(pos.X, pos.Y, pos.Z);
			GLOBE_NAME::OsgCoreSingleton::Instance()->CorrectPosByOblique(vecPos);

			osgEarth::GeoPoint geoPos = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), vecPos);
			geoPos.toWorld(vecPos);
			std::string data = ptrWFSFeaturePicker->Request(vecPos);
			ptrWFSFeaturePicker->ParseXML(data);
		}
	}
}

void ObliqueImageLayer::StartPick()
{
	if (!m_bIsPick)
	{
		if (m_ptrPickHandle == NULL)
		{
			osg::ref_ptr<GeoGlobe::WFSFeaturePicker> ptrWFSFeaturePicker = new GeoGlobe::WFSFeaturePicker(g_pickParamter);
			m_ptrPickHandle = new ReferenceImp<GeoGlobe::WFSFeaturePicker>(ptrWFSFeaturePicker);
			OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(ptrWFSFeaturePicker);
		}
		else
		{
			osg::ref_ptr<GeoGlobe::WFSFeaturePicker> ptrWFSFeaturePicker = m_ptrPickHandle->CastTo<osg::ref_ptr<GeoGlobe::WFSFeaturePicker> >();
			OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(ptrWFSFeaturePicker);
		}

		m_bIsPick = true;
	}
}

void ObliqueImageLayer::EndPick()
{
	if (m_bIsPick)
	{
		osg::ref_ptr<GeoGlobe::WFSFeaturePicker> ptrWFSFeaturePicker = m_ptrPickHandle->CastTo<osg::ref_ptr<GeoGlobe::WFSFeaturePicker> >();
		ptrWFSFeaturePicker->ClearPreHighLight();
		OsgCoreSingleton::Instance()->OsgViewer()->removeEventHandler(ptrWFSFeaturePicker);
		m_bIsPick = false;
	}
}

UTILITY_NAME::GsDelegate<void(Feature&)>& ObliqueImageLayer::OnPicked()
{
	osg::ref_ptr<GeoGlobe::WFSFeaturePicker> ptrWFSFeaturePicker = m_ptrPickHandle->CastTo<osg::ref_ptr<GeoGlobe::WFSFeaturePicker> >();
	return ptrWFSFeaturePicker->OnPicked;
}

ObliqueImageLayer::ObliqueImageLayer(std::string strUrl, StoreType eSerType, double dbMinLod , double dbMaxLod)
{
	osg::ref_ptr<osg::Group> ptrOsgGroup = NULL;
	m_bIsPick = false;
	m_ptrPickHandle = NULL;

	if (eSerType == eLocal)
		ptrOsgGroup = GeoGlobe::CObliqueImageLoader::LoadFromFile(strUrl, dbMinLod, dbMaxLod);
	else
		ptrOsgGroup = GeoGlobe::CObliqueImageLoader::LoadFromServer(strUrl, dbMinLod, dbMaxLod);

	if (NULL != ptrOsgGroup)
	{
		ptrOsgGroup->getOrCreateStateSet()->setAttributeAndModes(new osg::Program, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
		ptrOsgGroup->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

		ptrOsgGroup->setName("OsgbLayer");
	}
	osg::ref_ptr<osgEarth::GeoTransform> ptrLocator = new osgEarth::GeoTransform();
	ptrLocator->addChild(ptrOsgGroup);

	m_ptrLayerHandle = new ReferenceImp<osgEarth::GeoTransform>(ptrLocator);

	osg::Vec3d vecPos = GeoGlobe::CObliqueImageLoader::Position();

	std::string strProj = GeoGlobe::CObliqueImageLoader::ProjString();
	osg::ref_ptr<osgEarth::SpatialReference> ptrSrs = osgEarth::SpatialReference::create(strProj);

	osg::Vec3d vecOut;
	if (ptrSrs != NULL)
		ptrSrs->transform(vecPos, OsgCoreSingleton::Instance()->MapNode()->getMap()->getSRS(), vecOut);

	osgEarth::GeoPoint point = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), vecOut);
	ptrLocator->setPosition(point);

	osg::Matrixd outMatrix;
	outMatrix = ptrLocator->getMatrix();

	g_pickParamter.transMatrix = outMatrix;
	g_pickParamter.vecOrigin = vecPos;
	g_pickParamter.strOriginSrs = strProj;

	setCenter(vecOut.x(), vecOut.y(), vecOut.z());
	//AddVectorModel(vecOut.x(), vecOut.y(), vecOut.z());

	m_LayerType = eOPModel;
	m_strLayerUrl = strUrl;
}

void* ObliqueImageLayer::getPickParamter()
{
	return &g_pickParamter;
}

void ObliqueImageLayer::setPickParamter(const std::string& strUrl, const std::string& strTypeName)
{
	g_pickParamter.strUrl = strUrl;
	g_pickParamter.strTypeName = strTypeName;
}

ObliqueImageLayer::~ObliqueImageLayer()
{

}

void ObliqueImageLayer::Visible(bool bVisible)
{
	m_bVisible = bVisible;

	VisibleNodeVisitor visible;
	visible.setVisible(bVisible);

	m_ptrLayerHandle->CastTo<osg::ref_ptr<osgEarth::GeoTransform> >()->accept(visible);
}

bool ObliqueImageLayer::Visible() const
{
	return m_bVisible;
}

int ObliqueImageLayer::UID()
{
	return -1;
}

GLOBE_ENDNS