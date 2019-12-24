#ifndef GEOGLOBE_EFFECT_PRECIPITATIONEFFECT
#define GEOGLOBE_EFFECT_PRECIPITATIONEFFECT 1
#include <osg/Group>
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
#include <osg/ImageUtils>
namespace GeoGlobe
{
	namespace Effect
	{
	//实现降雨粒子特效
	//具体如何实现粒子效果，请参考osg中关于粒子特效的部分

		class PrecipitationEffect:public osg::Group
		{
		public:
			//std::string _textureFileName;
			double _range;
			osg::ref_ptr<osgEarth::Util::ObjectLocatorNode> _locator;
			PrecipitationEffect(double lon,double lat,double altitude,osgEarth::Map* map,double range=300,int type=0)
			{
				//_textureFileName=textureFileName;
				_range=range;


				//设置降雨发生的位置
				_locator = new osgEarth::Util::ObjectLocatorNode( map );
				_locator->getLocator()->setPosition(osg::Vec3d(lon,lat,altitude) );

				//构建例子系统
				osgParticle::ParticleSystemUpdater *psu = new osgParticle::ParticleSystemUpdater;
				if (type==0)
				{
					//降雨
					psu->addParticleSystem(createRainParticleSystem());
				}
				else
				{
					//降雪
					psu->addParticleSystem(createSnowParticleSystem());
				}
				
				_locator->addChild(psu);
				this->addChild(_locator);

			}


			osgParticle::ParticleSystem *createRainParticleSystem()
			{
				//设置例子属性，具体参照osg粒子系统部分内容
				osgParticle::Particle ptemplate;

				ptemplate.setLifeTime(3);        // 3 seconds of life

				float radius = 0.0025f;
				float density = 0.5f; // 0.5kg/m^3
				// the following ranges set the envelope of the respective 
				// graphical properties in time.
				ptemplate.setSizeRange(osgParticle::rangef(2.0f, 3.0f));
				ptemplate.setAlphaRange(osgParticle::rangef(0.3f, 0.5f));
				ptemplate.setColorRange(osgParticle::rangev4(
					osg::Vec4(0.6f,0.6f, 0.6f, 0.5f),
					osg::Vec4(0.6f, 0.6f, 0.6f, 0.5f)));

				// these are physical properties of the particle
				ptemplate.setRadius(radius);    // 5 cm wide particles
				ptemplate.setMass(density*radius*radius*radius*osg::PI*4.0f/3.0f);    // 50 g heavy
				ptemplate.setShape(osgParticle::Particle::LINE);
			//	ptemplate.set




				osgParticle::ParticleSystem *ps = new osgParticle::ParticleSystem;
				//ps->setDefaultAttributes(_textureFileName,false,false);
				ps->setDefaultParticleTemplate(ptemplate);


				setParticleStateSet(ps->getOrCreateStateSet());


				//设置粒子发射属性
				osgParticle::ModularEmitter *emitter = new osgParticle::ModularEmitter;
				emitter->setParticleSystem(ps);
				//emitter = new osgParticle::ModularEmitter;
				emitter->setNumParticlesToCreateMovementCompensationRatio(1.5f);
				emitter->setCounter(new osgParticle::RandomRateCounter);
				emitter->setPlacer(new osgParticle::SectorPlacer);
				emitter->setShooter(new osgParticle::RadialShooter);
				osgParticle::RandomRateCounter* counter = dynamic_cast<osgParticle::RandomRateCounter*>(emitter->getCounter());
				if (counter)
				{
					counter->setRateRange(_range*2,_range*2);
				}

				osgParticle::SectorPlacer* placer = dynamic_cast<osgParticle::SectorPlacer*>(emitter->getPlacer());
				if (placer)
				{
					placer->setCenter(osg::Vec3d(0,0,0));
					placer->setRadiusRange(0.0f,_range);
				}

				osgParticle::RadialShooter* shooter = dynamic_cast<osgParticle::RadialShooter*>(emitter->getShooter());
				if (shooter)
				{
					shooter->setThetaRange(0.0f, osg::PI_4);
					shooter->setInitialSpeedRange(0.0f,0.0f);
				}




				_locator->addChild(emitter);

				osgParticle::ModularProgram *program = new osgParticle::ModularProgram;
				program->setParticleSystem(ps);


				//设置粒子的运动轨迹
				program->addOperator(new PrecipitationOperator);
				_locator->addChild(program);
				osg::Geode *geode = new osg::Geode;    
				geode->addDrawable(ps);

				// add the geode to the scene graph
				_locator->addChild(geode);


				return ps;

			}

			osgParticle::ParticleSystem *createSnowParticleSystem()
			{

				//功能与降雨一样，只是修改了例子的形状
				osgParticle::Particle ptemplate;

				ptemplate.setLifeTime(3);        // 3 seconds of life

				float radius = 0.0025f;
				float density = 0.5f; // 0.5kg/m^3
				// the following ranges set the envelope of the respective 
				// graphical properties in time.
				ptemplate.setSizeRange(osgParticle::rangef(0.0f, 1.0f));
				ptemplate.setAlphaRange(osgParticle::rangef(0.3f, 0.5f));
				ptemplate.setColorRange(osgParticle::rangev4(
					osg::Vec4(0.85f,0.85f, 0.85f, 0.3f),
					osg::Vec4(0.85f, 0.85f, 0.85f, 0.5f)));

				// these are physical properties of the particle
				ptemplate.setRadius(radius);    // 5 cm wide particles
				ptemplate.setMass(density*radius*radius*radius*osg::PI*4.0f/3.0f);    // 50 g heavy

				osgParticle::ParticleSystem *ps = new osgParticle::ParticleSystem;
				//ps->setDefaultAttributes(_textureFileName,false,false);
				ps->setDefaultParticleTemplate(ptemplate);


	     		setParticleStateSet(ps->getOrCreateStateSet());

				osgParticle::ModularEmitter *emitter = new osgParticle::ModularEmitter;
				emitter->setParticleSystem(ps);
				//emitter = new osgParticle::ModularEmitter;
				emitter->setNumParticlesToCreateMovementCompensationRatio(1.5f);
				emitter->setCounter(new osgParticle::RandomRateCounter);
				emitter->setPlacer(new osgParticle::SectorPlacer);
				emitter->setShooter(new osgParticle::RadialShooter);
				osgParticle::RandomRateCounter* counter = dynamic_cast<osgParticle::RandomRateCounter*>(emitter->getCounter());
				if (counter)
				{
					counter->setRateRange(_range*2,_range*2);
				}

				osgParticle::SectorPlacer* placer = dynamic_cast<osgParticle::SectorPlacer*>(emitter->getPlacer());
				if (placer)
				{
					placer->setCenter(osg::Vec3d(0,0,0));
					placer->setRadiusRange(0.0f,_range);
				}

				osgParticle::RadialShooter* shooter = dynamic_cast<osgParticle::RadialShooter*>(emitter->getShooter());
				if (shooter)
				{
					shooter->setThetaRange(0.0f, osg::PI_4);
					shooter->setInitialSpeedRange(0.0f,0.0f);
				}

				_locator->addChild(emitter);

				osgParticle::ModularProgram *program = new osgParticle::ModularProgram;
				program->setParticleSystem(ps);
				program->addOperator(new PrecipitationOperator);
				_locator->addChild(program);
				osg::Geode *geode = new osg::Geode;    
				geode->addDrawable(ps);

				// add the geode to the scene graph
				_locator->addChild(geode);


				return ps;

			}


			//设置粒子的绘制状态:纹理，融合方式等
			void setParticleStateSet(osg::StateSet *stateset)
			{
				stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
				stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				stateset->setAttributeAndModes( new osg::Depth(osg::Depth::LEQUAL, 0.0, 1.0, false) );


				osg::Texture2D *texture = new osg::Texture2D;

				texture->setImage(createSpotLightImage(osg::Vec4(1.0f,1.0f,1.0f,1.0f),osg::Vec4(1.0f,1.0f,1.0f,0.0f),32,1.0));
				texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
				texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
				texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::MIRROR);
				texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::MIRROR);        
				stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

				osg::TexEnv *texenv = new osg::TexEnv;
				texenv->setMode(osg::TexEnv::MODULATE);
				//stateset->setTextureAttribute(0, texenv);

				osg::BlendFunc *blend = new osg::BlendFunc;


			}

			//设置粒子的运动轨迹
			class PrecipitationOperator: public osgParticle::Operator {
			public:
				PrecipitationOperator() 
					: osgParticle::Operator(), center_(0, 0, 0), axis_(0, 0, 1), intensity_(0.1f) {}

				PrecipitationOperator(const PrecipitationOperator &copy, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY)
					: osgParticle::Operator(copy, copyop), center_(copy.center_), axis_(copy.axis_), intensity_(copy.intensity_) {}

				META_Object(osgParticle, PrecipitationOperator);

				void setCenter(const osg::Vec3 &c)
				{
					center_ = c;
				}

				void setAxis(const osg::Vec3 &a)
				{
					axis_ = a / a.length();
				}

				// this method is called by ModularProgram before applying
				// operators on the particle set via the operate() method.    
				void beginOperate(osgParticle::Program *prg)
				{

				}

				// apply a vortex-like acceleration. This code is not optimized,
				// it's here only for demonstration purposes.
				void operate(osgParticle::Particle *P, double dt)
				{
					//跟随重力运动
						P->addVelocity(osg::Vec3f(10*dt,10*dt,-30.8f*dt));
				}

			protected:
				virtual ~PrecipitationOperator() {}

			private:
				osg::Vec3 center_;
				osg::Vec3 xf_center_;
				osg::Vec3 axis_;
				osg::Vec3 xf_axis_;
				osg::Vec3f x_axis_;
				osg::Vec3f y_axis_;
				osg::Vec3f z_axis_;
				float intensity_;
			};

		};


	}
}

#endif
