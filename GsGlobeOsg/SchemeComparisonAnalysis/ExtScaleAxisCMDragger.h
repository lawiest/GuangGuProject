/**
 * 放缩拉伸拖拽器
 */

#ifndef _EXTSCALEAXISCMDRAGGER_H_
#define _EXTSCALEAXISCMDRAGGER_H_ 1

//#include <osgManipulator/Scale1DDragger>
#include "CMFManipulator/Scale1DDragger"
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Quat>

using namespace CMFManipulator;

class  ExtScaleAxisCMDragger : public CMFManipulator::CompositeDragger
{
    public:

		ExtScaleAxisCMDragger(osg::ref_ptr<osg::Vec3dArray> m_modelWorldVertices);

		~ExtScaleAxisCMDragger()
		{
		}

		//设置该拖拽器默认形状
		void setupDefaultGeometry();

    protected:


        osg::ref_ptr< Scale1DDragger >        _xDragger;
        osg::ref_ptr< Scale1DDragger >        _yDragger;
        osg::ref_ptr< Scale1DDragger >        _zDragger;

		osg::ref_ptr<osg::Vec3dArray> _modelWorldVertices;

		osg::Vec3d _xAxis;
		osg::Vec3d _yAxis;
		osg::Vec3d _zAxis;

};



#endif
