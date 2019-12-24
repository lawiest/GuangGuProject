#include "PlaceNameLayerPrivate.h"
#include <osgEarth/MapNode>
//#include <PlaceName/QuadPlaceNameTileDriver.h>

GLOBE_NS

PlaceNameLayerPrivate::PlaceNameLayerPrivate(PlaceNameLayerOptions *pnLayerOptions)
{
	m_ptrOptions =pnLayerOptions;
	m_bVisible=true;

	std::vector<osgEarth::TileKey> keys;
	osgEarth::GeoExtent ext= osgEarth::GeoExtent(m_ptrOptions->_mapNode->getMapSRS(), m_ptrOptions->_west, m_ptrOptions->_south, m_ptrOptions->_east, m_ptrOptions->_north);

	int firstLevel = m_ptrOptions->_minLevel - 3;
	firstLevel = firstLevel > 0 ? firstLevel : 0;
	m_ptrOptions->_mapNode->getMap()->getProfile()->getIntersectingTiles(ext, firstLevel, keys);
	
	for( unsigned i=0; i<keys.size(); ++i )
	{
		std::string quadPNTileName = osgEarth::Stringify() << "PlaceName" << "." << keys[i].getLevelOfDetail() << "." << keys[i].getTileX() << "." << keys[i].getTileY() << ".quadplacenametile";
		osg::ref_ptr<osg::Node> pnTile = osgDB::readNodeFile(quadPNTileName, pnLayerOptions);
		//this->setFileName(i, quadModelTileName );
		this->addChild(pnTile, 0, FLT_MAX);
	}

	osg::Vec3d center;
	osgEarth::GeoPoint gp = ext.computeBoundingGeoCircle().getCenter();
	gp.toWorld(center);
	
	//ext.getBoundingGeoCircle()->getCenter()
	this->setCenter(center);
	this->setRadius(ext.computeBoundingGeoCircle().getRadius());
	this->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
}

void PlaceNameLayerPrivate::Visible(bool bVisible)
{
	m_bVisible = bVisible;
}

bool PlaceNameLayerPrivate::Visible() const
{
	return m_bVisible;
}

void PlaceNameLayerPrivate::traverse(osg::NodeVisitor& nv)
{
	if(!m_bVisible)
		return;

	osg::LOD::traverse(nv);
}

GLOBE_ENDNS
