#ifndef __FOG_EFFECT__H__
#define __FOG_EFFECT__H__

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
#include <osg/Fog>
#include <osg/MatrixTransform>
#include <osg/Matrix>
#include <osgEarth/Map>

namespace osgEarth
{

	namespace Util
	{
		class FogEffect;
	}
}

namespace GeoGlobe
{
	namespace Effect
	{
		class FogEffect : public osg::StateSet
		{
		public:
			FogEffect();
			~FogEffect();

		public:
			void SetStartAndEnd(float start, float end);
			void SetMode(osg::Fog::Mode mode);
			void SetColor(const osg::Vec4& color);
			void SetDensity(double density);
			void BindTo(osg::Node* node);
			void Visible(bool visible);
			bool Visible();

		protected:
			osg::ref_ptr<osg::Fog> m_fog; 
			osg::ref_ptr<osgEarth::Util::FogEffect> m_fogEffect;
			osg::Node* m_node;
			bool m_visible;
		};
	}
}

#endif