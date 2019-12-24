#ifndef GEOGLOBE_EFFECT_SMOKEEFFECT
#define GEOGLOBE_EFFECT_SMOKEEFFECT 1
#include <osg/LOD>
#include <osg/Geode>

#include <osgParticle/Particle>
#include <osgParticle/SmokeEffect>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/AccelOperator>
#include <osgParticle/FluidFrictionOperator>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osgEarth/Map>
#include <osgEarthUtil/ObjectLocator>
namespace GeoGlobe
{
	namespace Effect
	{
	//实现烟雾粒子特效，这边只是将osg自带的烟雾特效和osgEarth定位器做了个链接，
	//具体如何实现粒子效果，请参考osg中关于粒子特效的部分
		class SmokeEffect:public osg::LOD
		{
		
		public:
			//osg自带的烟雾粒子
			osg::ref_ptr<osgParticle::SmokeEffect> _smoke;
			//定位器，是粒子系统定位到虚拟地球指定位置
			osg::ref_ptr<osgEarth::Util::ObjectLocatorNode> _locator;


			SmokeEffect(double lon,double lat,double altitude,osgEarth::Map* map,std::string EffectDirectory,float scale=1.0f,float intensity=1.0f)
			{
				//设置粒子出现的位置
				_locator = new osgEarth::Util::ObjectLocatorNode( map );
				_locator->getLocator()->setPosition(osg::Vec3d(lon,lat,altitude) );

				//构建osg默认的例子特效
				_smoke=new osgParticle::SmokeEffect(osg::Vec3d(0,0,0),scale,intensity);
				//fireEffect->setWind(osg::Vec3d(0,0,10));
				//采用局部坐标系
				_smoke->setUseLocalParticleSystem(false);
				//持续运行
				_smoke->setEmitterDuration(FLT_MAX);
				//烟雾纹理的位置
				_smoke->setTextureFileName(EffectDirectory+"/smoke.rgb");
				osg::Geode *geode = new osg::Geode;   
				geode->addDrawable(_smoke->getParticleSystem());


				//将特效放到定位器中
				_locator->addChild(_smoke);
				_locator->addChild(geode);


				//当离特效位置远的时候不更新绘制粒子
				osg::Vec3d center;
				map->getProfile()->getSRS()->transformToWorld(osg::Vec3d(lon,lat,altitude),center);
				this->setCenter(center);
				this->setRange(0,0,1000);
				this->addChild(_locator);

			}


		};


	}
}


#endif
