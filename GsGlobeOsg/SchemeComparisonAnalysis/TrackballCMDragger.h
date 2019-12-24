/**
 * 在绕自身经纬平面（所有轴）滚动块进行旋转
 */

#ifndef _TRACKBALLCMDRAGGER_H
#define _TRACKBALLCMDRAGGER_H 1


//#include <osgManipulator/RotateCylinderDragger>
//#include <osgManipulator/RotateSphereDragger>
//#include <osgManipulator/AntiSquish>

#include "CMFManipulator/RotateCylinderDragger"
#include "CMFManipulator/RotateSphereDragger"
#include "CMFManipulator/AntiSquish"

#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/PolygonMode>
#include <osg/CullFace>
#include <osg/Quat>
#include <osg/AutoTransform>

using namespace CMFManipulator;


class  TrackballCMDragger : public CMFManipulator::CompositeDragger
{
    public:

		TrackballCMDragger(bool useAutoTransform,osg::ref_ptr<osg::Vec3dArray> m_modelWorldVertices);


		~TrackballCMDragger()
		{
		}


		//虚函数重定义,创建圆环
		osg::Geometry* createCircleGeometry(float radius, unsigned int numSegments);
		

		//设置该拖拽器默认形状
		void setupDefaultGeometry();


        osg::ref_ptr<RotateCylinderDragger> _xDragger;
        osg::ref_ptr<RotateCylinderDragger> _yDragger;
        osg::ref_ptr<CMFManipulator::RotateCylinderDragger> _zDragger;
        osg::ref_ptr<RotateSphereDragger>   _xyzDragger;

		osg::ref_ptr<osg::Vec3dArray> _modelWorldVertices;

		osg::Vec3d _xAxis;
		osg::Vec3d _yAxis;
		osg::Vec3d _zAxis;
};



#endif
