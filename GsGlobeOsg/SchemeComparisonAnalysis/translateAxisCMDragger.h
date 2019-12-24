/**
 * 在地球面上三个轴进行平移
 */

#ifndef _TRANSLATEAXISCMDRAGGER_H
#define _TRANSLATEAXISCMDRAGGER_H 



#include "CMFManipulator/Translate1DDragger"

#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Quat>

using namespace CMFManipulator;


class  TranslateAxisCMDragger : public CMFManipulator::CompositeDragger
{
    public:

        TranslateAxisCMDragger(osg::ref_ptr<osg::Vec3dArray> m_modelWorldVertices);


		//设置该拖拽器默认形状
		void setupDefaultGeometry();


        ~TranslateAxisCMDragger()
		{

		}



        osg::ref_ptr< CMFManipulator::Translate1DDragger >  _xDragger;
        osg::ref_ptr< CMFManipulator::Translate1DDragger >  _yDragger;
        osg::ref_ptr< CMFManipulator::Translate1DDragger >  _zDragger;

		osg::ref_ptr<osg::Vec3dArray> _modelWorldVertices;
	
		osg::Vec3d _cAxis;
		osg::Vec3d _xAxis;
		osg::Vec3d _yAxis;
		osg::Vec3d _zAxis;
};

#endif
