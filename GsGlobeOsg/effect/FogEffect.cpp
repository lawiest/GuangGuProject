#include "FogEffect.h"
#include <osgEarthUtil/Fog>

using namespace GeoGlobe;
using namespace Effect;

FogEffect::FogEffect()
	:m_fog(nullptr)
	,m_fogEffect(nullptr)
	,m_node(0)
	,m_visible(true)
{
	m_fog =  new osg::Fog;
	m_fogEffect = new  osgEarth::Util::FogEffect();

	if(m_fog)
	{
		m_fog->setFogCoordinateSource(osg::Fog::FRAGMENT_DEPTH); // it effects the 
	}
}

FogEffect::~FogEffect()
{
	m_fogEffect =nullptr;
	m_fog = nullptr;
	m_node = 0;
}

void FogEffect::SetColor(const osg::Vec4& color)
{
	if(m_fog)
	{
		m_fog->setColor(color);
	}
}

void FogEffect::SetMode(osg::Fog::Mode mode)
{
	if(m_fog)
	{
		m_fog->setMode(mode);
	}
}

void FogEffect::SetDensity(double density)
{
	if(m_fog)
	{
		m_fog->setDensity(density);
	}
}

void FogEffect::BindTo(osg::Node* node)
{
	if(!node || !m_fog || !m_fogEffect)
	{
		return;
	}
	m_node = node;
	osg::StateSet *state= node->getOrCreateStateSet(); // you should use  a stateset which node you want to effect with fog.
	m_fogEffect->attach(state);
	state->setAttributeAndModes(m_fog.get(),osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
}

void FogEffect::SetStartAndEnd(float start, float end)
{
	if(m_fog)
	{
		m_fog->setStart(start);
		m_fog->setEnd(end);
	}
}

void FogEffect::Visible(bool visible)
{
	if(!m_node)
	{
		return;
	}

	if(visible != m_visible)
	{
		m_visible = visible;
		if(visible==false)
		{			
			//设置为false后状态还原。zhangli yuzhou
			osg::StateSet *state= m_node->getOrCreateStateSet(); // you should use  a stateset which node you want to effect with fog.			
			m_fogEffect->detach();
			state->setAttributeAndModes(m_fog.get(),osg::StateAttribute::OFF);
		}
		else
		{
			osg::StateSet *state= m_node->getOrCreateStateSet(); // you should use  a stateset which node you want to effect with fog.
			m_fogEffect->attach(state);
			state->setAttributeAndModes(m_fog.get(),osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		}
	}
}

bool FogEffect::Visible()
{
	return m_visible;
}
