#include "SchemeComparisonAnalysis/ExtScaleAxisCMDragger.h"




ExtScaleAxisCMDragger::ExtScaleAxisCMDragger(osg::ref_ptr<osg::Vec3dArray> m_modelWorldVertices):_modelWorldVertices(m_modelWorldVertices)
{
	_xAxis=osg::Vec3d(_modelWorldVertices->at(1).x()-_modelWorldVertices->at(0).x(),_modelWorldVertices->at(1).y()-_modelWorldVertices->at(0).y(),_modelWorldVertices->at(1).z()-_modelWorldVertices->at(0).z());
	//			_yAxis=osg::Vec3d(_modelWorldVertices->at(2).x()-_modelWorldVertices->at(0).x(),_modelWorldVertices->at(2).y()-_modelWorldVertices->at(0).y(),_modelWorldVertices->at(2).z()-_modelWorldVertices->at(0).z());
	_zAxis=osg::Vec3d(_modelWorldVertices->at(3).x(),_modelWorldVertices->at(3).y(),_modelWorldVertices->at(3).z());

	_xAxis.normalize();
	//_yAxis.normalize();
	_zAxis.normalize();

	/*_xDragger = new CMFManipulator::Scale1DDragger();
	addChild(_xDragger.get());
	addDragger(_xDragger.get());*/

	/*	_yDragger = new CMFManipulator::Scale1DDragger();
	addChild(_yDragger.get());
	addDragger(_yDragger.get());*/

	_zDragger = new CMFManipulator::Scale1DDragger();
	addChild(_zDragger.get());
	addDragger(_zDragger.get());

	setParentDragger(getParentDragger());
}

//设置该拖拽器默认形状
void ExtScaleAxisCMDragger::setupDefaultGeometry()
{
			osg::Geode* lineGeode = new osg::Geode;
			{
				osg::Geometry* geometry = new osg::Geometry();

				osg::Vec3Array* vertices = new osg::Vec3Array(2);
				(*vertices)[0] = osg::Vec3(0.0f,0.0f,0.0f);
				(*vertices)[1] = osg::Vec3(1.0f,0.0f,0.0f);

				geometry->setVertexArray(vertices);
				geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));

				lineGeode->addDrawable(geometry);
			}

			//// 线模式
			{
				osg::LineWidth* linewidth = new osg::LineWidth();
				linewidth->setWidth(2.0f);
				lineGeode->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
				lineGeode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
			}


		//	_xDragger->addChild(lineGeode);
		//	_yDragger->addChild(lineGeode);
			_zDragger->addChild(lineGeode);

			osg::Geode* geode = new osg::Geode;

			// 方向头盒子.
		//	geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(1.0f,0.0f,0.0f), 0.25)));

			// 方向头.
			osg::Sphere* sphere = new osg::Sphere(osg::Vec3(1.0f,0.0f,0.0f), 0.05);
			osg::ref_ptr<osg::ShapeDrawable> shapsphere=new osg::ShapeDrawable(sphere);
			shapsphere->setColor(osg::Vec4(0.0f,0.0f,1.0f,1.0f));

			geode->addDrawable(shapsphere);



		//	_xDragger->addChild(geode);
		//	_yDragger->addChild(geode);
			_zDragger->addChild(geode);


		/*	// 将Z，Y轴旋转到相应的位置
			{
				osg::Quat rotation; rotation.makeRotate(osg::Vec3(1.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 1.0f));
				_zDragger->setMatrix(osg::Matrix(rotation));
			}

			{
				osg::Quat rotation; rotation.makeRotate(osg::Vec3(1.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 1.0f, 0.0f));
				_yDragger->setMatrix(osg::Matrix(rotation));
			}*/


			//先将环旋转到自己定义的X轴,基准为X轴
			osg::Quat xrotation; xrotation.makeRotate(osg::Vec3d(1.0f, 0.0f, 0.0f), _xAxis);
			/*_xDragger->setMatrix(osg::Matrix(xrotation));

			_xDragger->setColor(osg::Vec4(1.0f,0.0f,0.0f,1.0f));*/
				


			//旋转到Z,Y相应位置,要在X旋转的基础上，在旋转
			{
				
				osg::Quat rotation; rotation.makeRotate(_xAxis, _zAxis);
				_zDragger->setMatrix(osg::Matrix(xrotation*rotation));//OSG旋转是从左到右顺序
			}

/*
			{
				osg::Quat rotation; rotation.makeRotate(_xAxis, _yAxis);
				_yDragger->setMatrix(osg::Matrix(xrotation*rotation));
			}*/

			
			_zDragger->setColor(osg::Vec4(0.0f,0.0f,1.0f,1.0f));
	//		_yDragger->setColor(osg::Vec4(0.0f,1.0f,0.0f,1.0f));


}





