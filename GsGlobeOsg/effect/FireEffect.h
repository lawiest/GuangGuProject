#ifndef GEOGLOBE_EFFECT_FIREEFFECT
#define GEOGLOBE_EFFECT_FIREEFFECT 1
#include <osg/LOD>
#include <osg/Group>
#include <osg/Geode>

#include <osgParticle/Particle>
#include <osgParticle/FireEffect>
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
	//实现火焰粒子特效，这边只是将osg自带的火焰特效和osgEarth定位器做了个链接，
	//具体如何实现粒子效果，请参考osg中关于粒子特效的部分
    class FireEffect:public osg::LOD
	{

	public:
		//osg自带的火焰粒子
		osg::ref_ptr<osgParticle::FireEffect> _fire;
		//定位器，是粒子系统定位到虚拟地球指定位置
		osg::ref_ptr<osgEarth::Util::ObjectLocatorNode> _locator;

		FireEffect(double lon,double lat,double altitude,osgEarth::Map* map,std::string EffectDirectory,float scale=1.2f,float intensity=5.0f)
		{

			//设置粒子出现的位置
			_locator = new osgEarth::Util::ObjectLocatorNode( map );
			_locator->getLocator()->setPosition(osg::Vec3d(lon,lat,altitude) );



			//构建osg默认的例子特效
			_fire=new osgParticle::FireEffect(osg::Vec3d(0,0,0),scale,intensity);
			//fireEffect->setWind(osg::Vec3d(0,0,10));
			//采用局部坐标系
			_fire->setUseLocalParticleSystem(false);
			//持续运行
			_fire->setEmitterDuration(FLT_MAX);
			//火焰纹利的位置
			_fire->setTextureFileName(EffectDirectory+"/smoke.rgb");
			osg::Geode *geode = new osg::Geode;   
            geode->addDrawable(_fire->getParticleSystem());

            //将特效放到定位器中
			_locator->addChild(_fire);
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
