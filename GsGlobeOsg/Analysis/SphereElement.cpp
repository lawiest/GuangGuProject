
#include <Analysis/SphereElement.h>

using namespace osgEarth;


GeoGlobe::SphereElement::SphereElement( osgEarth::MapNode* mapNode):
_color(0.0f, 0.0f, 1.0f, 1.0f),
_size( 3.0f )
{
	setCullingActive( false );

	//Build the handle
	osg::Sphere* shape = new osg::Sphere(osg::Vec3(0,0,0), 1.0f);   
	osg::Geode* geode = new osg::Geode();
	_shapeDrawable = new osg::ShapeDrawable( shape );    
	_shapeDrawable->setDataVariance( osg::Object::DYNAMIC );
	_shapeDrawable->setColor( _color );
	geode->addDrawable( _shapeDrawable );          

	geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	geode->getOrCreateStateSet()->setRenderBinDetails(12, "RenderBin");

	_scaler = new osg::MatrixTransform;
	_scaler->setMatrix( osg::Matrixd::scale( _size, _size, _size ));
	_scaler->addChild( geode );

	osg::AutoTransform* at = new osg::AutoTransform;
	at->setAutoScaleToScreen( true );
	at->addChild( _scaler );
	addChild( at );

	setMapNode( mapNode );
}

GeoGlobe::SphereElement::~SphereElement()
{

}

void GeoGlobe::SphereElement::setMapNode( MapNode* mapNode )
{
	MapNode* oldMapNode = getMapNode();

	if ( oldMapNode != mapNode )
	{
		_mapNode = mapNode;
	}
}

const osgEarth::GeoPoint& GeoGlobe::SphereElement::getPosition() const
{
	return _position;
}
void GeoGlobe::SphereElement::setPosition(const osgEarth::GeoPoint& position)
{
	if (_position != position)
	{
		_position = position;
		updateTransform();
	}
}

const osg::Vec4f& GeoGlobe::SphereElement::getColor() const
{
	 return _color;
}
void GeoGlobe::SphereElement::setColor(const osg::Vec4f& color)
{
	if (_color != color)
	{
		_color = color;
		_shapeDrawable->setColor( _color );
	}
}

float GeoGlobe::SphereElement::getSize() const
{
	return _size;
}

void GeoGlobe::SphereElement::setSize(float size)
{
	if (_size != size)
	{
		_size = size;
		_scaler->setMatrix( osg::Matrixd::scale( _size, _size, _size ));
	}
}

void GeoGlobe::SphereElement::updateTransform()
{
	if ( getMapNode() )
	{
		osg::Matrixd matrix;

		GeoPoint mapPoint( _position );
		mapPoint = mapPoint.transform( _mapNode->getMapSRS() );
		if (!mapPoint.makeAbsolute( getMapNode()->getTerrain() ))
		{
			OE_WARN << "Failed to clamp dragger" << std::endl;
			return;            
		}

		mapPoint.createLocalToWorld( matrix );
		setMatrix( matrix );
	}
}