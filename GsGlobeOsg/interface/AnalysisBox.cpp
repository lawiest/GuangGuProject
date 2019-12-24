#include "AnalysisBox.h"
#include "HeightAnalysis.h"
#include "AreaAnalysis.h"
#include "DistanceAnalysis.h"
#include "DistanceAnalysis3D.h"
#include "VolumeAnalysis.h"
#include "AreaAnalysis3D.h"
#include "OPInRangeAnalysis.h"
#include "LineOfSightAnalysis.h"
#include "MultipleLineOfSightAnalysis.h"
#include "TerrainProfileAnalysis.h"
#include "OsgbProfileAnalysis.h"
#include "RadarAnalysis.h"
#include "DrawElmentAnalysis.h"
#include <osg/Group>
#include <Analysis/Area3DTrakerElement.h>
#include <Analysis/Distance3DTrakerElement.h>
#include <Analysis/MarathonTrakerElement.h>
#include <Analysis/LineTraker3DElement.h>
#include <Analysis/PolygonTrakerElement3D.h>
#include <Analysis/ObserverPointsInRangeAnalysis.h>
#include <Analysis/LinearLineOfSightAnalysis.h>
#include <Analysis/MultipleLinearLineOfSightAnalysis.h>
#include <Analysis/DistanceMeasure.h>
#include <Analysis/RadarAnalysisHandler.h>
#include <Analysis/TerrainProfileGraph.h>
#include <Analysis/VolumeTrakerElement.h>
#include <Analysis/AreaTrakerElement.h>
#include <Analysis/OsgbProfileTrakerElement.h>
#include <Analysis/GeoMapTools.h>
#include "OsgCoreSingleton.h"
#include "innerimp.h"

GLOBE_NS

AnalysisBox::AnalysisBox()
{	
}

void AnalysisBox::AddAnalysis(AnalysisBase* ptrAnalysis)
{
	osg::Group* ptrRoot = OsgCoreSingleton::Instance()->Root();

	if (NULL == ptrRoot)
		return;

	switch (ptrAnalysis->Type())
	{
		case eHeighAnalysis:
		{
			HeightAnalysisPtr ptrHeightAnalysis = dynamic_cast<HeightAnalysis*>(ptrAnalysis);

			if (ptrHeightAnalysis)
			{
				osg::ref_ptr<GeoGlobe::LineTraker3DElement> ptrHeight = ptrHeightAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::LineTraker3DElement>>();
				ptrRoot->addChild(ptrHeight);

				m_vecAnalysis.push_back(ptrAnalysis);
			}
		}break;

		case eAreaAnalysis:
		{
			AreaAnalysis* ptrAreaAnalysis = dynamic_cast<AreaAnalysis*>(ptrAnalysis);
			if (ptrAreaAnalysis)
			{
				osg::ref_ptr<GeoGlobe::AreaTrakerElement>ptrAreaTraker = ptrAreaAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::AreaTrakerElement>>();
				//ptrAreaTraker->setAreaAnalysisCompletedCallback(new GeoGlobe::PolygonTrakerElement3D::AreaAnalysisCompletedCallback());
				//ptrRoot->addChild(ptrAreaTraker);
				OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(ptrAreaTraker);

				m_vecAnalysis.push_back(ptrAnalysis);
			}
		}break;

		case eDistanceAnalysis:
		{
			DistanceAnalysis* ptrDistanceAnalysis = dynamic_cast<DistanceAnalysis*>(ptrAnalysis);
			
			if (ptrDistanceAnalysis)
			{
				osg::ref_ptr<GeoGlobe::DistanceMeasureHandler> ptrMeasureTool = ptrDistanceAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::DistanceMeasureHandler>>();
				OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(ptrMeasureTool);

				m_vecAnalysis.push_back(ptrAnalysis);

			}
		}break;
		//倾斜三维面积量算
		case eAreaAnalysis3D:
		{
			//类型强转，类型不对应则为空指针
			AreaAnalysis3D* ptrAreaAnalysis3D = dynamic_cast<AreaAnalysis3D*>(ptrAnalysis);
			if (ptrAreaAnalysis3D)
			{
				osg::ref_ptr<Area3DTrakerElement> ptrAreaTraker = ptrAreaAnalysis3D->Handle()->CastTo<osg::ref_ptr<Area3DTrakerElement>>();

				OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(ptrAreaTraker);

				//ptrRoot->addChild(ptrAreaTraker->DisplayGroup());

				m_vecAnalysis.push_back(ptrAnalysis);
			}
		}break;
		//倾斜三维距离量算
		case eDistanceAnalysis3D:
		{
			//DistanceAnalysis3D* ptrDistanceAnalysis3D = dynamic_cast<DistanceAnalysis3D*>(ptrAnalysis);

			if (ptrAnalysis)
			{
				osg::ref_ptr<Distance3DTrakerElement> ptrDistance3D = ptrAnalysis->Handle()->CastTo<osg::ref_ptr<Distance3DTrakerElement> >();
				OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(ptrDistance3D);
				m_vecAnalysis.push_back(ptrAnalysis);
			}
		}break;
		case eVolumeAnalysis:
		{
			if (ptrAnalysis)
			{
				osg::ref_ptr<VolumeTrakerElement> ptrVolume = ptrAnalysis->Handle()->CastTo<osg::ref_ptr<VolumeTrakerElement> >();
				OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(ptrVolume);
				m_vecAnalysis.push_back(ptrAnalysis);
			}
		}break;
		case eOsgbProfileAnalysis:
		{
			if (ptrAnalysis)
			{
				osg::ref_ptr<OsgbProfileTrakerElement> ptrOsgbProfile = ptrAnalysis->Handle()->CastTo<osg::ref_ptr<OsgbProfileTrakerElement> >();
				OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(ptrOsgbProfile);
				m_vecAnalysis.push_back(ptrAnalysis);
			}
		}break;
		//马拉松分析
		case eMarathonAnalysis:
		{
			//DistanceAnalysis3D* ptrDistanceAnalysis3D = dynamic_cast<DistanceAnalysis3D*>(ptrAnalysis);

			if (ptrAnalysis)
			{
				osg::ref_ptr<MarathonTrakerElement> ptrMarathon = ptrAnalysis->Handle()->CastTo<osg::ref_ptr<MarathonTrakerElement>>();
				OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(ptrMarathon);
				m_vecAnalysis.push_back(ptrAnalysis);
			}
		}break;
		case eDrawElementTool:
		case eEditElmentAnalysis:
		{
			//DistanceAnalysis3D* ptrDistanceAnalysis3D = dynamic_cast<DistanceAnalysis3D*>(ptrAnalysis);
			if (ptrAnalysis)
			{
				osg::ref_ptr<GeoGlobe::GeoMapTool> ptrMapTool = ptrAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::GeoMapTool>>();
				OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(ptrMapTool);
				m_vecAnalysis.push_back(ptrAnalysis);
			}
		}break;
		case eOPInRangeAnalysis:
		{
			OPInRangeAnalysis* ptrOPInRangeAnalysis = dynamic_cast<OPInRangeAnalysis*>(ptrAnalysis);

			if (ptrOPInRangeAnalysis)
			{
				osg::ref_ptr<GeoGlobe::ObserverPointsInRangeAnalysis> ptrOPInRange = ptrOPInRangeAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::ObserverPointsInRangeAnalysis>>();
				ptrRoot->addChild(ptrOPInRange);

				m_vecAnalysis.push_back(ptrAnalysis);
			}
		}break;

		case eLineOfSightAnalysis:
		{
			LineOfSightAnalysis* ptrLineOfSightAnalysis = dynamic_cast<LineOfSightAnalysis*>(ptrAnalysis);

			if (ptrLineOfSightAnalysis)
			{
				osg::ref_ptr<GeoGlobe::LinearLineOfSightAnalysis> ptrLineOfSight = ptrLineOfSightAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::LinearLineOfSightAnalysis>>();
				ptrRoot->addChild(ptrLineOfSight);

				m_vecAnalysis.push_back(ptrAnalysis);
			}
		}break;

		case eMultipleLineOfSightAnalysis:
		{
			MultipleLineOfSightAnalysis* ptrMultipleLineOfSightAnalysis = dynamic_cast<MultipleLineOfSightAnalysis*>(ptrAnalysis);

			if (ptrMultipleLineOfSightAnalysis)
			{
				osg::ref_ptr<GeoGlobe::MultipleLinearLineOfSightAnalysis> ptrMultipleLinearOfSight = ptrMultipleLineOfSightAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::MultipleLinearLineOfSightAnalysis>>();
                ptrRoot->addChild(ptrMultipleLinearOfSight);

				m_vecAnalysis.push_back(ptrAnalysis);
			}
		}break;

		case eRadarAnalysis:
		{
			RadarAnalysis* ptrRadarAnalysis = dynamic_cast<RadarAnalysis*>(ptrAnalysis);

			if (ptrRadarAnalysis)
			{
				osg::ref_ptr<GeoGlobe::RadarAnalysisHandler> ptrRadar = ptrRadarAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::RadarAnalysisHandler>>();
				ptrRoot->addChild(ptrRadar);

				m_vecAnalysis.push_back(ptrAnalysis);
			}
		}break;

		case eTerrainProfileAnalysis:
		{
			TerrainProfileAnalysis* ptrTerrainProfileAnalysis = dynamic_cast<TerrainProfileAnalysis*>(ptrAnalysis);

			if (ptrTerrainProfileAnalysis)
			{
				osg::ref_ptr<GeoGlobe::TerrainProfileGraph> ptrTerrainProfile = ptrTerrainProfileAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::TerrainProfileGraph>>();
				OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(ptrTerrainProfile);

				m_vecAnalysis.push_back(ptrAnalysis);			
			}
		}break;

	}
}

void AnalysisBox::RemoveAnalysis(AnalysisBase* ptrAnalysis)
{
	osg::Group* ptrRoot = OsgCoreSingleton::Instance()->Root();

	if (NULL == ptrRoot)
		return;
	
	std::vector<AnalysisBasePtr>::iterator it = std::find(m_vecAnalysis.begin(), m_vecAnalysis.end(), ptrAnalysis);

	if (it != m_vecAnalysis.end())
	{
		switch (ptrAnalysis->Type())
		{
		    case eHeighAnalysis:
		    {
				HeightAnalysisPtr ptrHeightAnalysis = dynamic_cast<HeightAnalysis*>(ptrAnalysis);
				
				if (ptrHeightAnalysis)
				{
					osg::ref_ptr<GeoGlobe::LineTraker3DElement> ptrHeight = ptrHeightAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::LineTraker3DElement>>();
					ptrRoot->removeChild(ptrHeight);
					m_vecAnalysis.erase(it);
				}
			}
			break;

			case eAreaAnalysis:
			{
				AreaAnalysisPtr ptrAreaAnalysis = dynamic_cast<AreaAnalysis*>(ptrAnalysis);
				
				if (ptrAreaAnalysis)
				{
					osg::ref_ptr<GeoGlobe::AreaTrakerElement> ptrAreaTraker = ptrAreaAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::AreaTrakerElement> >();
					OsgCoreSingleton::Instance()->OsgViewer()->removeEventHandler(ptrAreaTraker);
					m_vecAnalysis.erase(it);
				}
			}
			break;

			case eMultipleLineOfSightAnalysis:
			{
				MultipleLineOfSightAnalysis* ptrMultipleLineOfSightAnalysis = dynamic_cast<MultipleLineOfSightAnalysis*>(ptrAnalysis);
				
				if (ptrMultipleLineOfSightAnalysis)
				{
					osg::ref_ptr<GeoGlobe::MultipleLinearLineOfSightAnalysis> ptrMultipleLinearOfSight = ptrMultipleLineOfSightAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::MultipleLinearLineOfSightAnalysis>>();
					ptrRoot->removeChild(ptrMultipleLinearOfSight);
					m_vecAnalysis.erase(it);
				}
			}
			break;

			case eOPInRangeAnalysis:
			{
				OPInRangeAnalysis* ptrOPInRangeAnalysis = dynamic_cast<OPInRangeAnalysis*>(ptrAnalysis);
				
				if (ptrOPInRangeAnalysis)
				{
					osg::ref_ptr<GeoGlobe::ObserverPointsInRangeAnalysis> ptrOPInRange = ptrOPInRangeAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::ObserverPointsInRangeAnalysis>>();
					ptrRoot->removeChild(ptrOPInRange);
					m_vecAnalysis.erase(it);
				}
			}
			break;

			case eLineOfSightAnalysis:
			{
				LineOfSightAnalysis* ptrLineOfSightAnalysis = dynamic_cast<LineOfSightAnalysis*>(ptrAnalysis);
				
				if (ptrLineOfSightAnalysis)
				{
					osg::ref_ptr<GeoGlobe::LinearLineOfSightAnalysis> ptrLineOfSight = ptrLineOfSightAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::LinearLineOfSightAnalysis>>();
					ptrRoot->removeChild(ptrLineOfSight);
					m_vecAnalysis.erase(it);
				}
			}
			break;

			case eRadarAnalysis:
			{
				RadarAnalysis* ptrRadarAnalysis = dynamic_cast<RadarAnalysis*>(ptrAnalysis);

				if (ptrRadarAnalysis)
				{
					osg::ref_ptr<GeoGlobe::RadarAnalysisHandler> ptrRadar = ptrRadarAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::RadarAnalysisHandler>>();
					ptrRoot->removeChild(ptrRadar);
					m_vecAnalysis.erase(it);
				}
			}
			break;

			case eDistanceAnalysis:
			{
				DistanceAnalysis* ptrDistanceAnalysis = dynamic_cast<DistanceAnalysis*>(ptrAnalysis);

				if (ptrDistanceAnalysis)
				{
					osg::ref_ptr<GeoGlobe::DistanceMeasureHandler> ptrMeasureTool = ptrDistanceAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::DistanceMeasureHandler>>();
					ptrRoot->removeChild(ptrMeasureTool->DisplayGroup());
					OsgCoreSingleton::Instance()->OsgViewer()->removeEventHandler(ptrMeasureTool);
					m_vecAnalysis.erase(it);
				}
			}
			break;
			case eVolumeAnalysis:
			{
				VolumeAnalysis* ptrVolumeAnalysis = dynamic_cast<VolumeAnalysis*>(ptrAnalysis);
				if (ptrVolumeAnalysis)
				{
					osg::ref_ptr<VolumeTrakerElement> ptrVolumeTrakerElement = ptrVolumeAnalysis->Handle()->CastTo<osg::ref_ptr<VolumeTrakerElement> >();
					OsgCoreSingleton::Instance()->OsgViewer()->removeEventHandler(ptrVolumeTrakerElement);
					osg::Node* ptrNode = ptrVolumeTrakerElement->DisplayGroup();
					ptrRoot->removeChild(ptrNode);
					m_vecAnalysis.erase(it);
				}
			}break;
			case eOsgbProfileAnalysis:
			{
				OsgbProfileAnalysis* ptrOsgbProfileAnalysis = dynamic_cast<OsgbProfileAnalysis*>(ptrAnalysis);
				if (ptrOsgbProfileAnalysis)
				{
					osg::ref_ptr<OsgbProfileTrakerElement> ptrOsgbProfileTrakerElement = ptrOsgbProfileAnalysis->Handle()->CastTo<osg::ref_ptr<OsgbProfileTrakerElement> >();
					OsgCoreSingleton::Instance()->OsgViewer()->removeEventHandler(ptrOsgbProfileTrakerElement);
					osg::Node* ptrNode = ptrOsgbProfileTrakerElement->DisplayGroup();
					ptrRoot->removeChild(ptrNode);
					m_vecAnalysis.erase(it);
				}
			}break;
			//移除三维距离量算
			case eDistanceAnalysis3D:
			{
				DistanceAnalysis3D* ptrDistanceAnalysis3D = dynamic_cast<DistanceAnalysis3D*>(ptrAnalysis);
				if (ptrDistanceAnalysis3D)
				{
					osg::ref_ptr<Distance3DTrakerElement> ptrDistance3D = ptrDistanceAnalysis3D->Handle()->CastTo<osg::ref_ptr<Distance3DTrakerElement>>();
					OsgCoreSingleton::Instance()->OsgViewer()->removeEventHandler(ptrDistance3D);
					osg::Node* ptrNode = ptrDistance3D->DisplayGroup();
					ptrRoot->removeChild(ptrNode);
					m_vecAnalysis.erase(it);
				}
			}
			break;
			case eDrawElementTool:
			case eEditElmentAnalysis:
			{
				//DrawElmentAnalysis* ptrDistanceAnalysis3D = dynamic_cast<DrawElmentAnalysis*>(ptrAnalysis);
				if (ptrAnalysis)
				{
					osg::ref_ptr<GeoGlobe::GeoMapTool> ptrDraw = ptrAnalysis->Handle()->CastTo<osg::ref_ptr<GeoGlobe::GeoMapTool> >();
					OsgCoreSingleton::Instance()->OsgViewer()->removeEventHandler(ptrDraw);
					m_vecAnalysis.erase(it);
				}
			}
			break;
			case eMarathonAnalysis:
			{
				//DistanceAnalysis3D* ptrDistanceAnalysis3D = dynamic_cast<DistanceAnalysis3D*>(ptrAnalysis);

				if (ptrAnalysis)
				{
					osg::ref_ptr<MarathonTrakerElement> ptrMarathon = ptrAnalysis->Handle()->CastTo<osg::ref_ptr<MarathonTrakerElement>>();
					OsgCoreSingleton::Instance()->OsgViewer()->removeEventHandler(ptrMarathon);
					osg::Node* ptrNode = ptrMarathon->DisplayGroup();
					GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->removeChild(ptrNode);
					m_vecAnalysis.erase(it);
				}
			}break;
			//移除三维面积量算
			case eAreaAnalysis3D:
			{
				AreaAnalysis3D* ptrAreaAnalysis3D = dynamic_cast<AreaAnalysis3D*>(ptrAnalysis);
				if (ptrAreaAnalysis3D)
				{
					osg::ref_ptr<Area3DTrakerElement> ptrArea3D = ptrAreaAnalysis3D->Handle()->CastTo<osg::ref_ptr<Area3DTrakerElement>>();
					OsgCoreSingleton::Instance()->OsgViewer() ->removeEventHandler(ptrArea3D);
					osg::Node* ptrNode = ptrArea3D->DisplayGroup();
					ptrRoot->removeChild(ptrNode);
					m_vecAnalysis.erase(it);
				}
			}
			break;

			//移除剖面分析
			case eTerrainProfileAnalysis:
			{
				TerrainProfileAnalysis* ptrTerrainProfile = dynamic_cast<TerrainProfileAnalysis*>(ptrAnalysis);
				if (ptrTerrainProfile)
				{
					osg::ref_ptr<GeoGlobe::TerrainProfileGraph> ptrTerrainProGraph = ptrTerrainProfile->Handle()->CastTo<osg::ref_ptr<GeoGlobe::TerrainProfileGraph>>();					
					OsgCoreSingleton::Instance()->OsgViewer()->removeEventHandler(ptrTerrainProGraph);
					osg::Node* ptrNode = ptrTerrainProGraph->DisplayGroup();
					OsgCoreSingleton::Instance()->MapNode()->removeChild(ptrNode);
					m_vecAnalysis.erase(it);			

				}
			}
		}
	}
}

void AnalysisBox::RemoveAllAnalysis()
{
	osg::Group* ptrRoot = OsgCoreSingleton::Instance()->Root();

	if (NULL == ptrRoot)
		return;

	for (std::vector<AnalysisBasePtr>::iterator it = m_vecAnalysis.begin(); it != m_vecAnalysis.end();)
	{			
		RemoveAnalysis(*it++);
		it = m_vecAnalysis.begin();
	}	

	m_vecAnalysis.clear();	

}

std::list<double> AnalysisBox::GetTerrainProfileElevation(AnalysisBase* ptrAnalysis)
{
	return m_ptrTerrianProfile->CastTo<osg::ref_ptr<GeoGlobe::TerrainProfileGraph>>()->ElevationVec();
}

std::list<double> AnalysisBox::GetTerrainProfileDistance(AnalysisBase* ptrAnalysis)
{
	return m_ptrTerrianProfile->CastTo<osg::ref_ptr<GeoGlobe::TerrainProfileGraph>>()->DistanceVec();
}

double AnalysisBox::GetMaxDistance(AnalysisBase* ptrAnalysis)
{
	return m_ptrTerrianProfile->CastTo<osg::ref_ptr<GeoGlobe::TerrainProfileGraph>>()->MaxDistance();
}

double AnalysisBox::GetMinElevation(AnalysisBase* ptrAnalysis)
{
	return m_ptrTerrianProfile->CastTo<osg::ref_ptr<GeoGlobe::TerrainProfileGraph>>()->MinElevation();
}

double AnalysisBox::GetMaxElevation(AnalysisBase* ptrAnalysis)
{
	return m_ptrTerrianProfile->CastTo<osg::ref_ptr<GeoGlobe::TerrainProfileGraph>>()->MaxElevation();
}

AnalysisBox::~AnalysisBox()
{

}

GLOBE_ENDNS
