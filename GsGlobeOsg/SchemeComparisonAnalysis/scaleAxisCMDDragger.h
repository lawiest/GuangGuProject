/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/
//CMFManipulator - Copyright (C) 2007 Fugro-Jason B.V.

#ifndef _SCALEAXISDRAGGER_H
#define _SCALEAXISDRAGGER_H 

#include "CMFManipulator/ScaleAxisDragger"
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Quat>

using namespace CMFManipulator;

class  ScaleAxisCMDDragger : public CompositeDragger
{
public:

	ScaleAxisCMDDragger(osg::ref_ptr<osg::Vec3dArray> m_modelWorldVertices);

		//设置该拖拽器默认形状
		void setupDefaultGeometry();


    protected:


        osg::ref_ptr< CMFManipulator::Scale1DDragger >        _xDragger;
        osg::ref_ptr< CMFManipulator::Scale1DDragger >        _yDragger;
        osg::ref_ptr< CMFManipulator::Scale1DDragger >        _zDragger;

		osg::ref_ptr<osg::Vec3dArray> _modelWorldVertices;

		osg::Vec3d _xAxis;
		osg::Vec3d _yAxis;
		osg::Vec3d _zAxis;
};

#endif
