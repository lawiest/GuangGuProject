#ifndef GEOGLOBE_AREATRAKERELEMENT3D_H
#define GEOGLOBE_AREATRAKERELEMENT3D_H 1

#include <osgEarth/GeoData>
#include <osgEarth/MapNodeObserver>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osgEarthAnnotation/FeatureNode> 
#include <event.h>
#include <osgEarth/MapNode>

namespace osgEarth 
{
	namespace Symbology
	{
		class Geometry;
	}
}

namespace GeoGlobe
{
    class AreaTrakerElement : public osgGA::GUIEventHandler
    {
    public:
		AreaTrakerElement();

        virtual ~AreaTrakerElement();

		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object* object, osg::NodeVisitor* visitor);

		void Calcuate();

		UTILITY_NAME::GsDelegate<void(double)> OnFinish;

	private:
		bool m_bFinished;
		osgEarth::Annotation::FeatureNode* m_ptrFeartureNode;
		osg::ref_ptr<osgEarth::MapNode> m_ptrMapNode;
		bool m_bIsDrag;
		std::vector<osg::Vec3d> m_vecWorld;
		std::vector<osg::Vec3d> m_vecWgs84;
		osg::ref_ptr<osg::Group> m_ptrDisplayGroup;
    };
}

#endif
