#ifndef GEOGLOBE_SPHEREELEMENT_H
#define GEOGLOBE_SPHEREELEMENT_H 1

#include <osgEarth/Common>
#include <osgEarth/GeoData>
#include <osgEarth/MapNodeObserver>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osgEarth/Terrain>
#include <osgViewer/Viewer>
#include <osg/AutoTransform>



namespace GeoGlobe
{
    class SphereElement : public osg::MatrixTransform, public osgEarth::MapNodeObserver
    {
    public:
        SphereElement(osgEarth::MapNode* mapNode);
        virtual ~SphereElement();

        const osgEarth::GeoPoint& getPosition() const;
        void setPosition(const osgEarth::GeoPoint& position);

		const osg::Vec4f& getColor() const;
		void setColor(const osg::Vec4f& color);

		float getSize() const;
		void setSize(float size);  

		void updateTransform();

    public: // MapNodeObserver
        virtual void setMapNode(osgEarth::MapNode* mapNode );
        virtual osgEarth::MapNode* getMapNode() { return _mapNode.get(); }

	private:
        osg::observer_ptr< osgEarth::MapNode > _mapNode;
        osgEarth::GeoPoint _position;
		osg::MatrixTransform* _scaler;
		osg::ShapeDrawable* _shapeDrawable;
		osg::MatrixTransform* _mtNode;

		osg::Vec4f _color;
		float _size;
    };
} // namespace GeoGlobe

#endif