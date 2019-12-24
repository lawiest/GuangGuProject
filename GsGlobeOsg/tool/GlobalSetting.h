#ifndef GEOGLOBE_TOOL_REGISTRY
#define GEOGLOBE_TOOL_REGISTRY 1
#include <osg/Referenced>
#include <Tool/MemCache.h>
#include <Tool/CullingUtils.h>
typedef unsigned long long ULONGLONG;
namespace GeoGlobe
{
	namespace Tool
	{

		class  GlobalSetting : public osg::Referenced
		{

		public:
			/** Access the global Registry singleton. */
			std::map<ULONGLONG,osg::Vec4d> _colorMap;
			osg::ref_ptr<GeoGlobe::Tool::MemCache> _memCache;
			osg::ref_ptr<GeoGlobe::Tool::DrawableCullCallback> _drawCulling;

			static GlobalSetting* instance(bool erase = false)
			{
				static osg::ref_ptr<GlobalSetting> s_setting = new GlobalSetting;

	/*			if (erase) 
				{   
					s_registry->destruct();
					s_registry = 0;
				}*/

				return s_setting.get(); // will return NULL on erase
			}
			GlobalSetting()
			{
				_imageLayerAlpha=1.0f;
				_glesViewHeight = 1504;
				_memCache=new GeoGlobe::Tool::MemCache;
				_drawCulling=new GeoGlobe::Tool::DrawableCullCallback;
			};

            //计算视点到焦点的距离
			inline float getDistanceToFocalPoint(const osg::Vec3d& worldPos)
			{
				return (worldPos-_target).length();
			}

			inline float getDistanceToViewPoint(const osg::Vec3d& worldPos,const osg::Vec3d& eyePos,float factor=0.5)
			{
				return (worldPos-(_target+eyePos)*factor).length();
			}

			float _imageLayerAlpha;
            //焦点位置
			osg::Vec3d           _target;
            //视点高度
			int           _glesViewHeight;
			//osg::Vec3d           _eye;
			//osg::Vec3d           _up;

		};
	
	}
}

#endif
