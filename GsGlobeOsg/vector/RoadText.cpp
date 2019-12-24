
#include "RoadText.h"
#include <OsgCoreSingleton.h>
#include <osgUtil/Tessellator>
#include <tool/CodeTransformHelp.h>
#include <osg/Depth>
#include <osgText/Text>
#include <OsgCoreSingleton.h>
#include <osgEarth/MapNode>
#include <LonAndLatElement.h>
#include <osgViewer/Viewer>

namespace GeoGlobe 
{

	//RoadText::RoadText(osg::Vec3d p0, osg::Vec3d p1, std::string strRoad, int nIndex, double dbSize, osg::Vec4 color, std::string strFont)
	//{
	//	m_PoseState = positive;
	//	m_DistState = Zero;
	//	m_vecStart = p0;
	//	m_vecEnd = p1;

	//	//叶节点
	//	osg::ref_ptr<osg::Geode> textGeode = new osg::Geode;
	//	textGeode->addDrawable(createText(osg::Vec3(0.0f, 0.0f, 0.0f),strRoad, dbSize, color, strFont));

	//	//计算角度
	//	//对于动态控制的话，只需要根据两个点在屏幕上的相对位置（pWin.x > pWin.x或者相反，来把a取反(-a)即可）
	//	//但是这要统一管理屏幕内的Text，然后每帧都做一次计算，目前机制上不是太好做。
	//	float angle;
	//	if (p0.y() > p1.y() && p0.x() > p1.x())
	//	{
	//		angle = acos((p0.x() - p1.x()) / (p1 - p0).length());
	//	}
	//	else if (p0.y() <= p1.y() && p0.x() <= p1.x())
	//	{
	//		angle = acos((p1.x() - p0.x()) / (p1 - p0).length());
	//	}
	//	else if (p0.x() < p1.x() && p0.y() > p1.y())
	//	{
	//		angle = acos((p0.x() - p1.x()) / (p1 - p0).length()) + osg::PI;
	//	}
	//	else
	//	{
	//		angle = acos((p1.x() - p0.x()) / (p1 - p0).length()) + osg::PI;
	//	}

	//	m_Angle = angle;

	//	//旋转计算出的角度
	//	m_Trans = new osg::MatrixTransform();
	//	m_Trans->setMatrix(osg::Matrix::rotate(angle, 0, 0, 1));
	//	m_Trans->addChild(textGeode.get());

	//	textGeode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	//	textGeode->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, false));

	//	//取两个点的中心点
	//	osg::Vec3d pC = (p1 + p0) / 2;
	//	m_vecPosition = pC;
	//	//把文字放到中心点
	//	osg::ref_ptr<osgEarth::Util::ObjectLocatorNode> ptrLocator = new osgEarth::Util::ObjectLocatorNode(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMap());
	//	ptrLocator->addChild(m_Trans);
	//	ptrLocator->getLocator()->setPosition(pC);

	//	osg::ref_ptr<osg::LOD> ptrLOD = new osg::LOD();
	//	if (nIndex%3 == 0)
	//	{
	//		ptrLOD->addChild(ptrLocator, 0, 800);
	//	}
	//	else if(nIndex % 3 == 1)
	//	{
	//		ptrLOD->addChild(ptrLocator, 0, 1600);
	//	}
	//	else if(nIndex % 3 == 2)
	//	{
	//		ptrLOD->addChild(ptrLocator, 0, 2500);
	//	}

	//	GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->addChild(ptrLOD);
	//	//GLOBE_NAME::OsgCoreSingleton::Instance()->LonAndLat()->Change().Add(this, &RoadText::OnChange);
	//}
	//多大间隔打一个点
	double g_length_level_4 = 150;
	double g_length_level_3 = g_length_level_4*2;
	double g_length_level_2 = g_length_level_3*2;
	double g_length_level_1 = g_length_level_2*2;
	double g_length_level_0 = g_length_level_1*2;
	RoadText::RoadText(double& fLineLength,osg::Vec3d p0, osg::Vec3d p1, const std::vector<osg::ref_ptr<osg::Geode> > &vTextGeode)
	{		
		m_PoseState = positive;
		m_DistState = Zero;
		m_vecStart = p0;
		m_vecEnd = p1;
		m_bCheck = false;
		

		//计算角度
		//对于动态控制的话，只需要根据两个点在屏幕上的相对位置（pWin.x > pWin.x或者相反，来把a取反(-a)即可）
		//但是这要统一管理屏幕内的Text，然后每帧都做一次计算，目前机制上不是太好做。
		float angle;
		if (p0.y() > p1.y() && p0.x() > p1.x())
		{
			angle = acos((p0.x() - p1.x()) / (p1 - p0).length());
		}
		else if (p0.y() <= p1.y() && p0.x() <= p1.x())
		{
			angle = acos((p1.x() - p0.x()) / (p1 - p0).length());
		}
		else if (p0.x() < p1.x() && p0.y() > p1.y())
		{
			angle = acos((p0.x() - p1.x()) / (p1 - p0).length()) + osg::PI;
		}
		else
		{
			angle = acos((p1.x() - p0.x()) / (p1 - p0).length()) + osg::PI;
		}

		m_Angle = angle;

		

		//------------------------这地方修改下--------//
		//取两个点的中心点
		//osg::Vec3d pC = (p1 + p0) / 2;
		//m_vecPosition = pC;
		osgEarth::GeoPoint gP;
		osgEarth::GeoPoint gP0(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), p0.x(), p0.y(), p0.z());
		osgEarth::GeoPoint gP1(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), p1.x(), p1.y(), p1.z());
		osg::Vec3d wP0;
		osg::Vec3d wP1;
		//转到笛卡尔坐标以方便计算距离
		gP0.toWorld(wP0);
		gP1.toWorld(wP1);

		int countLast = fLineLength / g_length_level_4;
		double fRemainLast = fLineLength - g_length_level_4 * countLast;
		double fLengthThis = (wP1 - wP0).length();
		//如果当前段大于0
		if (fLengthThis > 0)
		{
			double fRemainThis = fRemainLast + fLengthThis;
			//如果剩余线段大于最小间隔g_length_level_4，则一直循环
			int countThis = fRemainThis / g_length_level_4;
			osg::Vec3d positionText = wP0;
			osg::Vec3d p0p1 = wP1 - wP0;
			for (int i = 0; i < countThis; i++)
			{
				double ratio = (g_length_level_4 - fRemainLast) / fLengthThis;
				//通过长度来算
				positionText.x() += ratio * p0p1.x();
				positionText.y() += ratio * p0p1.y();
				positionText.z() += ratio * p0p1.z();
				
				//转经纬度挂上去
				gP.fromWorld(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), positionText);

				countLast++;
				osg::ref_ptr<osg::LOD> ptrLOD = new osg::LOD();
				//按LOD挂接
				if (countLast % 1 == 0)
				{					
					ptrLOD->addChild(LocatorAndRotate(osg::Vec3d(gP.x(), gP.y(), gP.z()),angle,vTextGeode[0], true), 0, g_length_level_4*4);
				}
				if (countLast % 2 == 0)
				{
					ptrLOD->addChild(LocatorAndRotate(osg::Vec3d(gP.x(), gP.y(), gP.z()), angle, vTextGeode[1]), g_length_level_4 * 4, g_length_level_3 * 4);
				}
				if (countLast % 4 == 0)
				{
					ptrLOD->addChild(LocatorAndRotate(osg::Vec3d(gP.x(), gP.y(), gP.z()), angle, vTextGeode[2]), g_length_level_3 * 4, g_length_level_2 * 4);
				}
				if (countLast % 8 == 0)
				{
					ptrLOD->addChild(LocatorAndRotate(osg::Vec3d(gP.x(), gP.y(), gP.z()), angle, vTextGeode[3]), g_length_level_2 * 4, g_length_level_1 * 4);
				}
				if (countLast % 16 == 0)
				{
					ptrLOD->addChild(LocatorAndRotate(osg::Vec3d(gP.x(), gP.y(), gP.z()), angle, vTextGeode[4]), g_length_level_1 * 4, g_length_level_0 * 4);
				}
				//置0下次计算
				fRemainLast = 0;
				GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->addChild(ptrLOD);
			}
			
			GLOBE_NAME::OsgCoreSingleton::Instance()->LonAndLat()->Change().Add(this, &RoadText::OnChange);

			//累加当前段
			fLineLength += (wP1 - wP0).length();
		}
	}
	//osg::Vec3d(gP.x(), gP.y(), gP.z())
	osg::Node* RoadText::LocatorAndRotate(osg::Vec3d vPositionText,double angle, osg::Geode* textGeode, bool bCheck)
	{
		//把文字放到指定位置
		osgEarth::GeoTransform* ptrLocator = new osgEarth::GeoTransform();
		
		GLOBE_NAME::OsgCoreSingleton::Instance()->CorrectPosByOblique(vPositionText);
		if (bCheck)
		{
			m_vPtrLocators.push_back(ptrLocator);
		}

		osgEarth::GeoPoint point = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(),vPositionText);
		ptrLocator->setPosition(point);
		//旋转计算出的角度
		osg::MatrixTransform* Trans = new osg::MatrixTransform();
		Trans->setMatrix(osg::Matrix::rotate(angle, 0, 0, 1));
		Trans->addChild(textGeode);		
		ptrLocator->addChild(Trans);

		return ptrLocator;
	}
	void RoadText::OnChange()
	{
		if (m_bCheck)
		{
			return;
		}
		if (m_vPtrLocators.size() == 0)
		{
			m_bCheck = true;
			return;
		}


		//高度大于2500直接返回
		double dbDistance = m_scBox.DistanceFromCameraToGround();
		if (dbDistance > 500)
			return;

		double dbXMin, dbXMax, dbYMin, dbYMax;
		m_scBox.ViewExtent(dbXMin, dbXMax, dbYMin, dbYMax);

		osgEarth::GeoExtent extent = osgEarth::GeoExtent(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), dbXMin, dbYMin, dbXMax, dbYMax);
		
		osg::Vec3d eye, center, up;
		GLOBE_NAME::OsgCoreSingleton::Instance()->OsgViewer()->getCamera()->getViewMatrixAsLookAt(eye, center, up);

		for (int i = m_vPtrLocators.size() - 1; i >= 0; i--)
		{	
			osgEarth::GeoPoint transform = m_vPtrLocators[i]->getPosition();
			osg::Vec3d pL = osg::Vec3d(transform.x(), transform.y(), transform.z());
			osgEarth::GeoPoint pIn(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), pL.x(), pL.y(), pL.z());
			
			osg::Vec3d pInWorld;
			pIn.toWorld(pInWorld);
			
			double distance = (pInWorld - eye).length();
			double distance1 = (pInWorld - center).length();
			if (distance < 200)
			{				
				osgEarth::GeoPoint pOut;
				double dHeight = 1.0;
				Convert(pIn, pOut, dHeight);
				osg::Vec3d pO(pOut.x(), pOut.y(), pOut.z());
				osgEarth::GeoPoint point = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), pO);
				m_vPtrLocators[i]->setPosition(point);
				m_vPtrLocators.erase(m_vPtrLocators.begin() + i);
			}
			
		}
		
		/*
		double winSX, winSY, winEX, winEY;
		m_scBox.LonLat2Window(m_vecStart.x(), m_vecStart.y(), m_vecStart.z(), winSX, winSY);
		m_scBox.LonLat2Window(m_vecEnd.x(), m_vecEnd.y(), m_vecEnd.z(), winEX, winEY);
		double angle = m_Angle;

		if ((m_vecStart.x() > m_vecEnd.x() && winSX < winEX) || (m_vecStart.x() < m_vecEnd.x() && winSX > winEX))
		{
			if (m_PoseState == positive)
			{
				m_Trans->setMatrix(osg::Matrix::rotate(angle + osg::PI, 0, 0, 1));
				m_PoseState = opposite;
			}
		}
		else if (m_PoseState == opposite)
		{
			m_Trans->setMatrix(osg::Matrix::rotate(angle, 0, 0, 1));
			m_PoseState = positive;
		}

		if (dbDistance < 800)
		{
			if (m_ptrText && m_dbCurrentCharacterSize != m_dbCharacterSize)
			{
				m_ptrText->setCharacterSize(m_dbCharacterSize);
				m_dbCurrentCharacterSize = m_dbCharacterSize;
			}
		}
		else if (dbDistance >= 800 && dbDistance < 1600)
		{
			if (m_ptrText && m_dbCurrentCharacterSize != m_dbCharacterSize * 2)
			{
				m_ptrText->setCharacterSize(m_dbCharacterSize * 2);
				m_dbCurrentCharacterSize = m_dbCharacterSize * 2;
			}
		}
		else if (dbDistance >= 1600 && dbDistance < 2500)
		{
			if (m_ptrText && m_dbCurrentCharacterSize != m_dbCharacterSize * 3)
			{
				m_ptrText->setCharacterSize(m_dbCharacterSize * 3);
				m_dbCurrentCharacterSize = m_dbCharacterSize * 3;
			}
		}*/
	}

	//转换点，给一个坐标，转换到对象的表面去
	//输入为点的地理坐标pIn，想要高出表面多高dHeight
	//输出为结果地理坐标pOut
	bool RoadText::Convert(const osgEarth::GeoPoint& pIn, osgEarth::GeoPoint& pOut, double dHeight)
	{
		//地理坐标的变换，起点增加1000米高度，终点减去1000米
		//如果本身没有z值，可以设置或者认为z为0
		osgEarth::GeoPoint pStart = pIn;
		pStart.z() += 1000;
		osgEarth::GeoPoint pEnd = pIn;
		pEnd.z() -= 1000;

		//求交得到交点
		if (PickSurface(pStart, pEnd, pOut))
		{
			//抬高
			pOut.z() += dHeight;
			return true;
		}
		return false;
	}

	//输入一条线段，处理沿线情况，让输入的顶点和线都不要钻地
	//输入为点的地理坐标pStart，pEnd,想要高出表面多高dHeight
	//输出为结果集合，其中起点在第一个，终点在最后一个，中间是可能需要插入的点
	bool RoadText::Convert(const osgEarth::GeoPoint& pStart, const osgEarth::GeoPoint& pEnd, std::vector<osgEarth::GeoPoint> &vOut, double dHeight)
	{
		//先把顶点拔高
		osgEarth::GeoPoint outStart, outEnd;
		Convert(pStart, outStart, dHeight);
		Convert(pEnd, outEnd, dHeight);

		//保存起点
		vOut.push_back(outStart);
		//判断线和场景交点
		osgEarth::GeoPoint p0 = outStart;
		osgEarth::GeoPoint p1 = outEnd;
		osgEarth::GeoPoint pResult;
		//做循环调用，算法是从起点往终点逼近
		//每次重设p0值，作为新的起点，一直到没有交点为止
		while (PickSurface(p0, p1, pResult))
		{
			if (p0 == pResult || p1 == pResult)
			{
				break;
			}
			//抬高、保存
			pResult.z() += dHeight;
			vOut.push_back(pResult);

			//重设新的起点
			p0 = pResult;
		}
		//保存结尾
		vOut.push_back(outEnd);
		return true;
	}
	//求交
	bool RoadText::PickSurface(osg::Vec3d start, osg::Vec3d end, osg::Vec3d& result)
	{
		//构造求交器
		osg::ref_ptr<osgUtil::IntersectionVisitor> iv = new osgUtil::IntersectionVisitor;
		osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(start, end);

		//结果集
		osgUtil::LineSegmentIntersector::Intersections intersections;

		//指定对象的求交
		iv->setIntersector(ls.get());
		GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->accept(*(iv.get()));

		//拿到第一个交点，也就是表面
		if (ls->containsIntersections())
		{
			result = end;
			intersections = ls->getIntersections();
			osgUtil::LineSegmentIntersector::Intersections::iterator iter = intersections.begin();
			for (; iter != intersections.end(); iter++)
			{
				osg::Vec3 point = iter->getWorldIntersectPoint();
				//发现离起点更近的点，则更新到result
				if ((start - point).length() < (start - result).length())
				{
					result = point;
				}
			}
			return true;
		}
		return false;
	}

	bool RoadText::PickSurface(const osgEarth::GeoPoint& pStart, const osgEarth::GeoPoint& pEnd, osgEarth::GeoPoint& pResult)
	{
		//转换为世界坐标
		osg::Vec3d start, end, result;
		pStart.toWorld(start);
		pEnd.toWorld(end);
		//求交得到交点
		if (PickSurface(start, end, result))
		{
			pResult.fromWorld(pStart.getSRS(), result);
			return true;
		}
		return false;
	}

}
