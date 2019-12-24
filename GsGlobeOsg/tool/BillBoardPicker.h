#pragma once

#include <Feature.h>
#include <osg/ref_ptr>
#include <osgEarthUtil/RTTPicker>

namespace GeoGlobe 
{
	class BillBoardPicker
	{
	public:
		BillBoardPicker();
		virtual ~BillBoardPicker();


		void StartPick();
		void EndPick();
	
		UTILITY_NAME::GsDelegate<void(GLOBE_NAME::Feature*)> OnPicked;

	protected:
		osg::ref_ptr<osgEarth::Util::RTTPicker> m_rttPicker;

	};
}
