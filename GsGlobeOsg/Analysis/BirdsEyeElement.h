#ifndef GEOGLOBE_BIRDSEYEELEMENT_H
#define GEOGLOBE_BIRDSEYEELEMENT_H 1

#include <osgEarth/GeoData>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>

#include "tool/GGEarthManipulator.h"


using namespace GeoStar::Kernel::Globe;

namespace GeoGlobe
{
    class BirdsEyeElement : public osg::Camera
    {
    public:
        BirdsEyeElement(GGEarthManipulator* earthpulator,std::string strImgPath ,double dblXmin = -180,double dblXmax = 180, double dblYmin = -90,double dblYmax = 90);
        virtual ~BirdsEyeElement();

		virtual void traverse(osg::NodeVisitor& nv); 

		void setImageResource(std::string strImgFile,double dblXmin,double dblXmax,double dblYmin,double dblYmax);

		void SetVisible(bool bVisible);

	private:
		 bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	private:
		osg::ref_ptr<GGEarthManipulator> m_earthpulator;

		osg::ref_ptr<osg::Geode> m_pGeodeCross;
		osg::ref_ptr<osg::Vec3dArray> m_pVertexCross;

		osg::ref_ptr<osg::Geode> m_pGeodeCrossQuad;
		osg::ref_ptr<osg::Vec3dArray> m_pVertexCrossQuad;

		bool m_bVisible;
    };

	class ImageNode : public osg::Camera
	{
	public:
		ImageNode(const char * strImgFile);
	};
} // namespace GeoGlobe

#endif