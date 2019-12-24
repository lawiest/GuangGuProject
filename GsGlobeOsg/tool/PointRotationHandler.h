#ifndef GEOGLOBE_TOOL_POINTROTATIONHANDLER
#define GEOGLOBE_TOOL_POINTROTATIONHANDLER 1

#pragma once

#include <osgGA/GUIEventHandler>

namespace GeoGlobe
{
	namespace Tool
	{
		class  PointRotationHandler : public osgGA::GUIEventHandler
		{
		public:			

			bool _fly;

			PointRotationHandler();

			void play(double flyAngle);

			void stop();

			void reduceSpeed();

			void fastSpeed();

			bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

		protected:
			double m_angle;
		};
	}
}
#endif