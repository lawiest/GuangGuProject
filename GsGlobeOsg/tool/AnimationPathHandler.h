#ifndef GEOGLOBE_TOOL_ANIMATIONPATHHANDLER
#define GEOGLOBE_TOOL_ANIMATIONPATHHANDLER 1

#include <osgGA/GUIEventHandler>
#include <osgEarth/Viewpoint>


namespace GeoGlobe 
{ 
	namespace Tool
	{
		class  AnimationPathHandler : public osgGA::GUIEventHandler
		{
		public:
			struct TimeViewPoint
			{
				TimeViewPoint(float time,osgEarth::Viewpoint vp)
				{
					_time=time;
					_vp=vp;
				}
				float _time;
				osgEarth::Viewpoint _vp;
			};

		//std::vector<TimeViewPoint> _animationPath;
		bool _ifSwitchOn;
		bool _fly;
		bool _back;
		bool _onDestination;
		bool _onStartingPoint;
		double _distance;
		int _currentPoint;

        /**当前路径飞行文件*/
		std::string  _animationPathFile;

        /**存储所有记录的关键点*/
		std::vector<TimeViewPoint> _recordTimepointList;
		
		AnimationPathHandler();		

		void readAnimationPath();	

		void clearRecordPoint();

		void recordControlPoint(int ID, float time);
		
		void writeControlPoint(std::ostream& fout, TimeViewPoint &tp);		

		bool writeAnimationPath(std::string writepath);

        bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa );
		
        void play(std::string fileName);

        void stop();

		void pause();

		void restartFly();

		bool needRestartFly();

		bool backOnTrack();

		bool needBackFly();

		void backFly(bool bIsBack);

        void reset();

		void fastSpeed();

		void reduceSpeed();

        //void exit();
		
		protected:
			//GeoInterpolation _geoInterpolation;
			//void fireDistanceChanged();
			double					_fSpeed;
			double                  _time_s_set_viewpoint;
			double                  _time_s_has_fly;
			//记录暂停前的位置
			osgEarth::Viewpoint _PauseViewPoint;
			bool _bPauseReal;
			bool _bIsFlying ;
		};
	}
}
#endif
