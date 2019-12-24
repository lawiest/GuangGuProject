#include "SchemeComparisonAnalysis/translateAxisCMDragger.h"


TranslateAxisCMDragger::TranslateAxisCMDragger(osg::ref_ptr<osg::Vec3dArray> m_modelWorldVertices):_modelWorldVertices(m_modelWorldVertices)
{
	_xAxis=osg::Vec3d(_modelWorldVertices->at(1).x()-_modelWorldVertices->at(0).x(),_modelWorldVertices->at(1).y()-_modelWorldVertices->at(0).y(),_modelWorldVertices->at(1).z()-_modelWorldVertices->at(0).z());
	_yAxis=osg::Vec3d(_modelWorldVertices->at(2).x()-_modelWorldVertices->at(0).x(),_modelWorldVertices->at(2).y()-_modelWorldVertices->at(0).y(),_modelWorldVertices->at(2).z()-_modelWorldVertices->at(0).z());
	_zAxis=osg::Vec3d(_modelWorldVertices->at(3).x(),_modelWorldVertices->at(3).y(),_modelWorldVertices->at(3).z()/*-_modelWorldVertices->at(0).z()*/);

	_xAxis.normalize();
	_yAxis.normalize();
	_zAxis.normalize();

	//三个轴的移动轴
	_xDragger = new CMFManipulator::Translate1DDragger(osg::Vec3(0.0,0.0,0.0), _zAxis);
	addChild(_xDragger.get());
	addDragger(_xDragger.get());

	_yDragger = new CMFManipulator::Translate1DDragger(osg::Vec3(0.0,0.0,0.0), _zAxis);
	addChild(_yDragger.get());
	addDragger(_yDragger.get());

	_zDragger = new CMFManipulator::Translate1DDragger(osg::Vec3(0.0,0.0,0.0), _zAxis);
	addChild(_zDragger.get());
	addDragger(_zDragger.get());

	setParentDragger(getParentDragger());
}


//设置该拖拽器默认形状
void TranslateAxisCMDragger::setupDefaultGeometry()
{
	//方向线
	osg::Geode* lineGeode = new osg::Geode;
	{
		osg::Geometry* geometry = new osg::Geometry();

		osg::Vec3Array* vertices = new osg::Vec3Array(2);
		(*vertices)[0] = osg::Vec3(0.0f,0.0f,0.0f);   //起点的位置，是addChild进去的
		(*vertices)[1] = _zAxis;

		geometry->setVertexArray(vertices);
		geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));

		lineGeode->addDrawable(geometry);
	}

	{
		osg::LineWidth* linewidth = new osg::LineWidth();
		linewidth->setWidth(5.0f);
		lineGeode->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
		lineGeode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	}

	// 将方向线添加到每个1D拖拽器中.
	_xDragger->addChild(lineGeode);
	_yDragger->addChild(lineGeode);
	_zDragger->addChild(lineGeode);



	// 锥+不可见圆柱来通过点选来进行拉动操作 handle响应
	osg::Quat rotation1; rotation1.makeRotate(osg::Vec3d(0.0f, 0.0f, 1.0f), _zAxis);//几何体旋转量

	//X
	osg::ref_ptr<osg::Geode> xgeode=new osg::Geode;
	xgeode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	{
		osg::Cone* cone = new osg::Cone (osg::Vec3(0.0f, 0.0f, 1.0f), 0.025f, 0.10f);
		osg::ref_ptr<osg::ShapeDrawable> shapcone=new osg::ShapeDrawable(cone);
		shapcone->setColor(osg::Vec4(1.0f,0.0f,0.0f,1.0f));

		osg::Cylinder* cylinder = new osg::Cylinder (osg::Vec3(0.0f, 0.0f, 0.5f), 0.015f, 1.0f);
		osg::ref_ptr<osg::ShapeDrawable> shapcyl=new osg::ShapeDrawable(cylinder);
		shapcyl->setColor(osg::Vec4(1.0f,0.0f,0.0f,1.0f));

		osg::ref_ptr<osg::Drawable> geometry = shapcyl;

		setDrawableToAlwaysCull(*geometry);//隐藏，因为线可以选中后变色


		xgeode->addDrawable(shapcone);
		xgeode->addDrawable(shapcyl);
	}
	osg::ref_ptr<osg::MatrixTransform> xtrans=new osg::MatrixTransform();
	xtrans->setMatrix(osg::Matrix(rotation1));
	xtrans->addChild(xgeode);

	//Y
	osg::ref_ptr<osg::Geode> ygeode=new osg::Geode;
	ygeode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	{
		osg::Cone* cone = new osg::Cone (osg::Vec3(0.0f, 0.0f, 1.0f), 0.025f, 0.10f);
		osg::ref_ptr<osg::ShapeDrawable> shapcone=new osg::ShapeDrawable(cone);
		shapcone->setColor(osg::Vec4(0.0f,1.0f,0.0f,1.0f));

		osg::Cylinder* cylinder = new osg::Cylinder (osg::Vec3(0.0f, 0.0f, 0.5f), 0.015f, 1.0f);
		osg::ref_ptr<osg::ShapeDrawable> shapcyl=new osg::ShapeDrawable(cylinder);
		shapcyl->setColor(osg::Vec4(0.0f,1.0f,0.0f,1.0f));

		osg::ref_ptr<osg::Drawable> geometry = shapcyl;

		setDrawableToAlwaysCull(*geometry);//隐藏，因为线可以选中后变色

		ygeode->addDrawable(shapcone);
		ygeode->addDrawable(shapcyl);
	}
	osg::ref_ptr<osg::MatrixTransform> ytrans=new osg::MatrixTransform();
	ytrans->setMatrix(osg::Matrix(rotation1));
	ytrans->addChild(ygeode);

	//Z
	osg::ref_ptr<osg::Geode> zgeode=new osg::Geode;
	zgeode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	{

		osg::Cone* cone = new osg::Cone (osg::Vec3(0.0f, 0.0f, 1.0f), 0.025f, 0.10f);
		osg::ref_ptr<osg::ShapeDrawable> shapcone=new osg::ShapeDrawable(cone);
		shapcone->setColor(osg::Vec4(0.0f,0.0f,1.0f,1.0f));

		osg::Cylinder* cylinder = new osg::Cylinder (osg::Vec3(0.0f, 0.0f, 0.5f), 0.015f, 1.0f);
		osg::ref_ptr<osg::ShapeDrawable> shapcyl=new osg::ShapeDrawable(cylinder);
		shapcyl->setColor(osg::Vec4(0.0f,0.0f,1.0f,1.0f));

		osg::ref_ptr<osg::Drawable> geometry = shapcyl;

		setDrawableToAlwaysCull(*geometry);//隐藏，因为线可以选中后变色

		zgeode->addDrawable(shapcone);
		zgeode->addDrawable(shapcyl);
	}
	osg::ref_ptr<osg::MatrixTransform> ztrans=new osg::MatrixTransform();
	ztrans->setMatrix(osg::Matrix(rotation1));
	ztrans->addChild(zgeode);

	_xDragger->addChild(xtrans);
	_yDragger->addChild(ytrans);
	_zDragger->addChild(ztrans);

	// 将X轴旋转到相应的位置
	{
		osg::Quat rotation; rotation.makeRotate(_zAxis,_xAxis);
		_xDragger->setMatrix(osg::Matrix(rotation));
	}

	// 将Y轴旋转到相应的位置
	{
		osg::Quat rotation; rotation.makeRotate(_zAxis,_yAxis);
		_yDragger->setMatrix(osg::Matrix(rotation));
	}

	// 每个方向的操作器颜色不一样
	_xDragger->setColor(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
	_yDragger->setColor(osg::Vec4(0.0f,1.0f,0.0f,1.0f));
	_zDragger->setColor(osg::Vec4(0.0f,0.0f,1.0f,1.0f));
}









