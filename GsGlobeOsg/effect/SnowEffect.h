#ifndef __SNOW_EFFECT__H__
#define __SNOW_EFFECT__H__

#include <osg/LOD>
#include <osg/Group>
#include <osg/Geode>

#include <osgParticle/Particle>
#include <osgParticle/PrecipitationEffect>
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
#include <osg/MatrixTransform>
#include <osg/Matrix>
#include <osgEarth/Map>

namespace GeoGlobe
{
	namespace Effect
	{
		class SnowEffect  : public osg::Node
		{
		public:
			explicit SnowEffect(osgEarth::Map* map);
			~SnowEffect();

		public:
			void SetPosition(double lon,double lat,double altitude);
			void SetScale(float scale);
			void SetColor(const osg::Vec4& color);
			void SetLevel(int level);
			void SetIntensity(float intensity);
			void Visible(bool visible);
			bool Visible();
			osg::MatrixTransform* GetMatrixTransform(){return m_transform.get();}

		protected:
			osgEarth::Map* m_map;
			osg::ref_ptr<osgParticle::PrecipitationEffect> m_snowEffect;
			osg::Matrix m_worldMatrix;
			osg::ref_ptr<osg::MatrixTransform> m_transform;
		};
	}
}

#endif