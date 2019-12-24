#ifndef GEOGLOBE_TOOL_FREEFLYHANDLER
#define GEOGLOBE_TOOL_FREEFLYHANDLER 1

#pragma once

#include <osgGA/GUIEventHandler>
#include <osgEarthAnnotation/GeoPositionNode>


namespace GeoGlobe
{
	namespace Tool
	{
		class  FreeFlyHandler : public osgGA::GUIEventHandler
		{
		public:	

			FreeFlyHandler();

			void play();

			void stop();

			void fastSpeed();
			void reduceSpeed();

			bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);			
			
		protected:		

			bool _fly;	
			osg::Node*  _model;
			double lat0, lon0, lat1, lon1, lat, lon, height;

			osg::Vec3d vec;    //方向向量
			osg::ref_ptr<osgEarth::Annotation::GeoPositionNode>      _geo;
			osgEarth::GeoPoint _focalPoint;
			double _time;
			double t;
		};
	}
}
#endif