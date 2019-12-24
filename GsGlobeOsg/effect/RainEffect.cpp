#include "RainEffect.h"

using namespace GeoGlobe;
using namespace Effect;


RainEffect::RainEffect(osgEarth::Map* map)
	:m_map(map)
	,m_rainEffect(nullptr)
	,m_transform(nullptr)
{
	m_rainEffect =  new osgParticle::PrecipitationEffect;
	if(m_rainEffect)
	{
		m_rainEffect->rain(1.0f);
		m_rainEffect->setUseFarLineSegments(true);
		m_rainEffect->setNodeMask(0x1);
	}

	m_transform = new osg::MatrixTransform;
	if(m_transform)
	{
		m_transform->setNodeMask(0x1);
	}
	this->setNodeMask(0x1); 
}

RainEffect::~RainEffect()
{
	m_rainEffect = nullptr;
	m_transform = nullptr;
}

void RainEffect::SetIntensity(float intensity)
{
	if(m_rainEffect)
	{
		m_rainEffect->rain(intensity);
	}
}


void RainEffect::SetColor(const osg::Vec4& color)
{
	if(m_rainEffect)
	{
		m_rainEffect->setParticleColor(color);
	}
}

void RainEffect::SetLevel(int level)
{
	if(m_rainEffect)
	{
		m_rainEffect->setParticleSize(level/10.0f);
	}
}

void RainEffect::SetPosition(double lon,double lat,double altitude)
{
	if(m_rainEffect&&m_map&&m_transform)
	{
		m_map->getProfile()->getSRS()->getEllipsoid()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(lat),osg::DegreesToRadians(lon),altitude,m_worldMatrix);

		m_transform->setMatrix(m_worldMatrix);

		m_transform->addChild(m_rainEffect);
	}
}

void RainEffect::SetScale(float scale)
{
	if(m_rainEffect&&m_transform)
	{
		m_transform->setMatrix(m_worldMatrix * osg::Matrix::scale(scale,scale,scale));
	}
}

void RainEffect::Visible(bool visible)
{
	if(m_rainEffect&&m_transform)
	{
		if(visible)
		{
			m_rainEffect->setNodeMask(0x1);
			m_transform->setNodeMask(0x1);
			this->setNodeMask(0x1); 
		}
		else
		{
			m_rainEffect->setNodeMask(0x0);
			m_transform->setNodeMask(0x0);
			this->setNodeMask(0x0); 
		}
	}
}

bool RainEffect::Visible()
{
	return (this->getNodeMask() != 0);
}