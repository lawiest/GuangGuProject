#include "SnowEffect.h"

using namespace GeoGlobe;
using namespace Effect;


SnowEffect::SnowEffect(osgEarth::Map* map)
	:m_map(map)
	,m_snowEffect(nullptr)
	,m_transform(nullptr)
{
	m_snowEffect =  new osgParticle::PrecipitationEffect;
	if(m_snowEffect)
	{
		m_snowEffect->snow(0.5f);
		m_snowEffect->setParticleColor(osg::Vec4(1,1,1,1));
		m_snowEffect->setUseFarLineSegments(false);
		m_snowEffect->setWind(osg::Vec3(2,2,2));
		m_snowEffect->setNodeMask(0x1);
	}

	m_transform = new osg::MatrixTransform;
	if(m_transform)
	{
		m_transform->setNodeMask(0x1);
	}
	this->setNodeMask(0x1); 
}

SnowEffect::~SnowEffect()
{
	m_snowEffect = nullptr;
	m_transform = nullptr;
}

void SnowEffect::SetIntensity(float intensity)
{
	if(m_snowEffect)
	{
		m_snowEffect->snow(intensity);
	}
}


void SnowEffect::SetColor(const osg::Vec4& color)
{
	if(m_snowEffect)
	{
		m_snowEffect->setParticleColor(color);
	}
}

void SnowEffect::SetLevel(int level)
{
	if(m_snowEffect)
	{
		m_snowEffect->setParticleSize(level/10.0f);
	}
}

void SnowEffect::SetPosition(double lon,double lat,double altitude)
{
	if(m_snowEffect&&m_map&&m_transform)
	{
		m_map->getProfile()->getSRS()->getEllipsoid()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(lat),osg::DegreesToRadians(lon),altitude,m_worldMatrix);

		m_transform->setMatrix(m_worldMatrix);

		m_transform->addChild(m_snowEffect);
	}
}

void SnowEffect::SetScale(float scale)
{
	if(m_snowEffect&&m_transform)
	{
		m_transform->setMatrix(m_worldMatrix * osg::Matrix::scale(scale,scale,scale));
	}
}

void SnowEffect::Visible(bool visible)
{
	if(m_snowEffect&&m_transform)
	{
		if(visible)
		{
			m_snowEffect->setNodeMask(0x1);
			m_transform->setNodeMask(0x1);
			this->setNodeMask(0x1); 
		}
		else
		{
			m_snowEffect->setNodeMask(0x0);
			m_transform->setNodeMask(0x0);
			this->setNodeMask(0x0); 
		}
	}
}

bool SnowEffect::Visible()
{
	return (this->getNodeMask() != 0);
}