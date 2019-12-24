/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
* Copyright 2008-2013 Pelican Mapping
* http://osgearth.org
*
* osgEarth is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "Tool/AnimationPathHandler.h"
#include "tool/GGEarthManipulator.h"
#include "OsgCoreSingleton.h"
#include <osgViewer/Viewer>
#include <fstream>
#include <exception>
#include <osgearth/Units>

#define LC "[PathFly] "

using namespace GeoGlobe::Tool;


AnimationPathHandler::AnimationPathHandler()
{
	_fly=false;
	GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->setSmooth(false);
	_currentPoint=0;
	_ifSwitchOn=false;
	_back = false;
	_time_s_set_viewpoint=0;
	_time_s_has_fly=0;
	_onDestination = false;
	_onStartingPoint = false;
	_bPauseReal = false;

}

bool CheckChange(osgEarth::Viewpoint viewpoint,osgEarth::Viewpoint curviewpoint)
{
	if (fabs(curviewpoint.focalPoint()->x() - viewpoint.focalPoint()->x()) > 0.00001 ||
		fabs(curviewpoint.focalPoint()->y() - viewpoint.focalPoint()->y()) > 0.00001 ||
		fabs(curviewpoint.focalPoint()->z() - viewpoint.focalPoint()->z()) > 10 ||
		fabs(curviewpoint.heading()->as(osgEarth::Units::DEGREES) - viewpoint.heading()->as(osgEarth::Units::DEGREES)) > 0.1 ||
		fabs(curviewpoint.pitch()->as(osgEarth::Units::DEGREES) - viewpoint.pitch()->as(osgEarth::Units::DEGREES)) > 0.1 ||
		fabs(curviewpoint.range()->as(osgEarth::Units::METERS) - viewpoint.range()->as(osgEarth::Units::METERS)) > 10.0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AnimationPathHandler::play(std::string fileName)
{
	_currentPoint = 0;
	_onStartingPoint = false;
	_onDestination = false;
	_back = false;

	if (_recordTimepointList.size() > 0)
	{
		_time_s_set_viewpoint = osg::Timer::instance()->time_s();
		osgEarth::Viewpoint viewPoint = GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint();

		if (CheckChange(viewPoint, _recordTimepointList[_currentPoint]._vp))
			GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->setViewpoint(_recordTimepointList[_currentPoint]._vp, _recordTimepointList[_currentPoint]._time - _time_s_has_fly);

		_currentPoint++;
	}	

	_fly = true;
}

void AnimationPathHandler::fastSpeed()
{
	if (_recordTimepointList.size() > 0 && _recordTimepointList[_currentPoint]._time >= 0)
	{
		if (_recordTimepointList[_currentPoint]._time <= 0)
			_recordTimepointList[_currentPoint]._time = 0;
		for (int i = 0; i < _recordTimepointList.size(); i++)
		{
			//提升速度，不去减时间，每次提升约20%
			_recordTimepointList[i]._time = _recordTimepointList[i]._time * 0.8;			
		}
		_fSpeed /= 0.8;

		if (_currentPoint - 1 > 0 && _currentPoint - 1 < _recordTimepointList.size())
		{
			_fly = false;
			//当前位置
			osgEarth::Viewpoint now = GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint();
			osgEarth::GeoPoint nowPoint = now.focalPoint().get();
			osgEarth::GeoPoint nextPoint = _recordTimepointList[_currentPoint - 1]._vp.focalPoint().get();
			//剩余路程除以速度就是时间
			float time = nowPoint.distanceTo(nextPoint) / _fSpeed;

			GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->clearViewpoint();
			GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getSettings()->setArcViewpointTransitions(false);
			GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getSettings()->setAutoViewpointDurationEnabled(false);
			GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->setViewpoint(
				_recordTimepointList[_currentPoint - 1]._vp, time);

			_fly = true;
		}		
	}
}

void AnimationPathHandler::reduceSpeed()
{
	if (_recordTimepointList.size() > 0)
	{
		for (int i = 0; i < _recordTimepointList.size(); i++)
		{
			//减低速度，不去减时间，每次减低约20%
			_recordTimepointList[i]._time = _recordTimepointList[i]._time / 0.8;			
		}
		_fSpeed *= 0.8;

		if (_currentPoint - 1 > 0 && _currentPoint - 1 < _recordTimepointList.size())
		{
			_fly = false;
			//当前位置
			osgEarth::Viewpoint now = GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint();
			osgEarth::GeoPoint nowPoint = now.focalPoint().get();
			osgEarth::GeoPoint nextPoint = _recordTimepointList[_currentPoint - 1]._vp.focalPoint().get();
			//剩余路程除以速度就是时间
			float time = nowPoint.distanceTo(nextPoint) / _fSpeed;

			GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->clearViewpoint();
			GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getSettings()->setArcViewpointTransitions(false);
			GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getSettings()->setAutoViewpointDurationEnabled(false);
			GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->setViewpoint(
				_recordTimepointList[_currentPoint - 1]._vp, time);

			_fly = true;
		}
	}	
}

void AnimationPathHandler::readAnimationPath()
{	
	_recordTimepointList.clear();
	std::ifstream in(_animationPathFile.c_str(), std::ios::in | std::ios::binary);
	while (!in.eof())
	{
		double time;
		osg::Vec3d position;
		double heading;
		double pitch;
		double range;
		in >> std::setiosflags(std::ios::fixed) >> std::setprecision(6) >> time >> position.y() >> position.x() >> position.z() >> heading >> pitch >> range;
		
		if (!in.eof())
		{
			TimeViewPoint tp(time, osgEarth::Viewpoint("", position.y(), position.x(), position.z(), heading, pitch, range));
			if (_recordTimepointList.size() > 0)
			{
				//从第二个开始，以速度插值
				osgEarth::GeoPoint dD = tp._vp.focalPoint().get();
				osgEarth::GeoPoint dS = _recordTimepointList[_recordTimepointList.size() - 1]._vp.focalPoint().get();
				double d = dD.distanceTo(dS);

				//以第二个和第一个为基准，他们之间的时间是5秒，计算出之后所有段的速度
				if (_recordTimepointList.size() == 1)
				{
					//以第一段飞5秒这样的配速为基准
					_fSpeed = d / 10;
					tp._time = 10;
				}
				else
				{
					//按照第一段的速度飞行
					tp._time = d / _fSpeed;
				}
			}
			_recordTimepointList.push_back(tp);
		}
			
		//_ViewpointList.push_back(osgEarth::Viewpoint("",position,heading,pitch,range));
		//insert(time,osg::AnimationPath::ControlPoint(position,rotation));
	}		
	
}

void AnimationPathHandler::recordControlPoint(int ID, float time)
{
	if (ID == 0)
	{
		_recordTimepointList.clear();
	}
	TimeViewPoint tp(time, GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint());

	if (_recordTimepointList.size() > 0)
	{
		//从第二个开始，以速度插值
		osgEarth::GeoPoint dD = tp._vp.focalPoint().get();
		osgEarth::GeoPoint dS = _recordTimepointList[_recordTimepointList.size() - 1]._vp.focalPoint().get();
		double d = dD.distanceTo(dS);

		//以第二个和第一个为基准，他们之间的时间是5秒，计算出之后所有段的速度
		if (_recordTimepointList.size() == 1)
		{
			//以第一段飞5秒这样的配速为基准
			_fSpeed = d / 5;
			tp._time = 5;
		}
		else
		{
			//按照第一段的速度飞行
			tp._time = d / _fSpeed;
		}
	}

	//if (_recordTimepointList.size() == 1)
	//	tp._time += 8;
	 
	_recordTimepointList.push_back(tp);
}

 void AnimationPathHandler::clearRecordPoint()
 {
 	if (GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator() == NULL)
 		return;
 
 	_recordTimepointList.clear();
 }

bool AnimationPathHandler::writeAnimationPath(std::string writepath)
{
	try {
		std::ofstream fout(writepath.c_str());
		fout.precision(6);
		for (int i = 0; i < _recordTimepointList.size(); i++)
		{
			writeControlPoint(fout, _recordTimepointList[i]);
		}
		fout.close();
	}
	catch (std::exception e)
	{
		return false;
	}
	return true;
}

void AnimationPathHandler::writeControlPoint(std::ostream& fout, TimeViewPoint &tp)
{
	fout << std::setiosflags(std::ios::fixed) << std::setprecision(6) << tp._time << " " << tp._vp.focalPoint()->x() << " " << tp._vp.focalPoint()->y() << " " << tp._vp.focalPoint()->z() << " " << tp._vp.getHeading() << " " << tp._vp.getPitch() << " " << tp._vp.getRange() << std::endl;
}

void AnimationPathHandler::reset()
{
	if (GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator() == NULL)
		return;
	
	 GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->cancelViewpointTransition();
	 _currentPoint = 0;
	 _fly=true;
	 _recordTimepointList.clear();
	 _animationPathFile = "";		
}

// void AnimationPathHandler::exit()
// {
// 	if (GLOBE_NAME::OsgCoreSingleton::GGEarthManipulator() == NULL)
// 		return;
// 
//     /**退出飞行*/
// 	_fly  = false;
// 	GLOBE_NAME::OsgCoreSingleton::GGEarthManipulator()->getSettings()->setArcViewpointTransitions(true);
// 	GLOBE_NAME::OsgCoreSingleton::GGEarthManipulator()->getSettings()->setAutoViewpointDurationEnabled(true);
// }

void AnimationPathHandler::stop()
{
	GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->cancelViewpointTransition();
	_fly = false;
}

void AnimationPathHandler::pause()
{
	GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->cancelViewpointTransition();
	_fly = false;

	_bPauseReal = true;
	_PauseViewPoint = GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint();
}

bool AnimationPathHandler::needBackFly()
{
	//本身就在反向飞行
	if (_fly && _back)
	{
		return false;
	}
	//在起点
	if (_onStartingPoint)
	{
		return false;
	}
	return true;
}
void AnimationPathHandler::backFly(bool bIsBack)
{
	//GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->clearViewpoint();
	if (needBackFly())
	{
		//先回到轨道上
		bool bDo = backOnTrack();

		//如果正在向前，则让往前走
		if (!_back)
		{		
			if (!bDo)
			{
				GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->cancelViewpointTransition();
			}			
			_currentPoint-=2;
		}
		//如果是暂停，则继续走
		else
		{
			if (!(_onStartingPoint || _onDestination))
			{
				_currentPoint++;
			}			
		}
		_back = true;
		_fly = true;
	}
	_bPauseReal = false;
}

//什么情况下还需要继续正向飞行
bool AnimationPathHandler::needRestartFly()
{
	//如果正在飞行，且正向，则不需处理
	if (_fly && !_back)
		return false;

	//如果已经到终点了，也不需要处理
	if (_onDestination)
		return false;

	return true;
}
void AnimationPathHandler::restartFly()
{
	//看是不是需要处理
	if (needRestartFly())
	{
		//先回到轨道上
		bool bDo = backOnTrack();

		//如果正在后退，则让往前走
		if (_back)
		{		
			if (!bDo)
			{
				GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->cancelViewpointTransition();
			}
			_currentPoint+=2;
		}
		//如果是暂停，则继续走
		else
		{
			if (!(_onStartingPoint || _onDestination))
			{
				_currentPoint--;
			}
			else
			{
				_currentPoint++;
			}
			
		}
		_back = false;
		_fly = true;
	}
	_bPauseReal = false;
}

bool AnimationPathHandler::backOnTrack()
{
	if (_onDestination)
	{
		//检查视口是在终点
		if (CheckChange(_recordTimepointList[_recordTimepointList.size() - 1]._vp, GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint()))
		{			
			GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->setViewpoint(_recordTimepointList[_recordTimepointList.size() - 1]._vp, _recordTimepointList[_currentPoint - 1]._time);
			return true;
		}
		else
		{
			return false;
		}
	}

	if(_onStartingPoint)
	{
		//检查视口是在起点
		if (CheckChange(_recordTimepointList[0]._vp, GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint()))
		{			
			GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->setViewpoint(_recordTimepointList[0]._vp, _recordTimepointList[0]._time);
			return true;
		}
		else
		{
			return false;
		}
	}
	//暂停情况判断
	if (!_bPauseReal)
	{
		return false;
	}
	if (CheckChange(_PauseViewPoint, GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->getViewpoint()))
	{
		if (_currentPoint > 0 && _currentPoint <= _recordTimepointList.size())
		{
			GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->setViewpoint(_PauseViewPoint, _recordTimepointList[_currentPoint - 1]._time - _time_s_has_fly);
			return true;
		}
		else
		{
			GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator()->setViewpoint(_PauseViewPoint, _recordTimepointList[0]._time - _time_s_has_fly);
			return true;
		}
	}
	return false;
}

bool AnimationPathHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{    
	if (!_ifSwitchOn)
	{
		return false;
	}

	osg::ref_ptr<GGEarthManipulator> ptrManp = GLOBE_NAME::OsgCoreSingleton::Instance()->GGEarthManipulator();

	osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());  
	
	if (!ptrManp.valid())
		ptrManp =dynamic_cast<GGEarthManipulator*>(view->getCameraManipulator());
	
	if (!ptrManp.valid())
		return false;

	if (ea.getEventType() == osgGA::GUIEventAdapter::FRAME&&_fly)
	{
		if (!ptrManp->isSettingViewpoint())
		{
			if (_currentPoint >= 0 && (_currentPoint < _recordTimepointList.size()))
			{
				ptrManp->getSettings()->setArcViewpointTransitions(false);
				ptrManp->getSettings()->setAutoViewpointDurationEnabled(false);

				_time_s_set_viewpoint = osg::Timer::instance()->time_s();
				if (_recordTimepointList.size()!= 0)
					ptrManp->setViewpoint(_recordTimepointList[_currentPoint]._vp, _recordTimepointList[_currentPoint]._time);

				_onStartingPoint = false;
				_onDestination = false;
			}
			else
			{
				ptrManp->getSettings()->setArcViewpointTransitions(true);
				ptrManp->cancelViewpointTransition();
				_fly = false;
				if (_back)
				{
					_onStartingPoint = true;
					_onDestination = false;
					_currentPoint = 0;
					_back = false;
				}
				else
				{
					_onDestination = true;
					_onStartingPoint = false;
					_currentPoint = _recordTimepointList.size();
				}
				
				
			}

			if (_fly)
			{
				if (_back)
				{
					_currentPoint--;
				}
				else
				{
					_currentPoint++;
				}
			}
				
		}
		return false;
	}

	return false;
}

