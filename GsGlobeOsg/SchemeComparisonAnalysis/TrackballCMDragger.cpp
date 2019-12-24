#include "SchemeComparisonAnalysis/TrackballCMDragger.h"



TrackballCMDragger::TrackballCMDragger(bool useAutoTransform,osg::ref_ptr<osg::Vec3dArray> m_modelWorldVertices):_modelWorldVertices(m_modelWorldVertices)
{
	_xAxis=osg::Vec3d(_modelWorldVertices->at(1).x()-_modelWorldVertices->at(0).x(),_modelWorldVertices->at(1).y()-_modelWorldVertices->at(0).y(),_modelWorldVertices->at(1).z()-_modelWorldVertices->at(0).z());
	_yAxis=osg::Vec3d(_modelWorldVertices->at(2).x()-_modelWorldVertices->at(0).x(),_modelWorldVertices->at(2).y()-_modelWorldVertices->at(0).y(),_modelWorldVertices->at(2).z()-_modelWorldVertices->at(0).z());
	_zAxis=osg::Vec3d(_modelWorldVertices->at(3).x(),_modelWorldVertices->at(3).y(),_modelWorldVertices->at(3).z()/*-_modelWorldVertices->at(0).z()*/);

	_xAxis.normalize();
	_yAxis.normalize();
	_zAxis.normalize();

	if (useAutoTransform)
	{
		float pixelSize = 50.0f;
		osg::MatrixTransform* scaler = new osg::MatrixTransform;
		scaler->setMatrix(osg::Matrix::scale(pixelSize, pixelSize, pixelSize));

		osg::AutoTransform *at = new osg::AutoTransform;
		at->setAutoScaleToScreen(true);
		at->addChild(scaler);


		AntiSquish* as = new AntiSquish;
		as->addChild(at);
		addChild(as);

//		_xDragger = new RotateCylinderDragger();
//		scaler->addChild(_xDragger.get());
//		addDragger(_xDragger.get());

// 		_yDragger = new RotateCylinderDragger();
// 		scaler->addChild(_yDragger.get());
// 		addDragger(_yDragger.get());

		_zDragger = new CMFManipulator::RotateCylinderDragger();
		scaler->addChild(_zDragger.get());
		addDragger(_zDragger.get());


// 		_xyzDragger = new RotateSphereDragger();
// 		scaler->addChild(_xyzDragger.get());
// 		addDragger(_xyzDragger.get());
	}
	else
	{

// 		_xDragger = new RotateCylinderDragger();
// 		addChild(_xDragger.get());
// 		addDragger(_xDragger.get());

// 		_yDragger = new RotateCylinderDragger();
// 		addChild(_yDragger.get());
// 		addDragger(_yDragger.get());

		_zDragger = new CMFManipulator::RotateCylinderDragger();
		addChild(_zDragger.get());
		addDragger(_zDragger.get());


// 		_xyzDragger = new RotateSphereDragger();
// 		addChild(_xyzDragger.get());
// 		addDragger(_xyzDragger.get());
	}

	setParentDragger(getParentDragger());
}


//虚函数重定义,创建圆环
osg::Geometry* TrackballCMDragger::createCircleGeometry(float radius, unsigned int numSegments)
{
	const float angleDelta = 2.0f*osg::PI/(float)numSegments;
	const float r = radius;
	float angle = 0.0f;
	osg::Vec3Array* vertexArray = new osg::Vec3Array(numSegments);
	osg::Vec3Array* normalArray = new osg::Vec3Array(numSegments);
	for(unsigned int i = 0; i < numSegments; ++i,angle+=angleDelta)
	{
		float c = cosf(angle);
		float s = sinf(angle);
		(*vertexArray)[i].set(c*r,s*r,0.0f);
		(*normalArray)[i].set(c,s,0.0f);
	}
	osg::Geometry* geometry = new osg::Geometry();
	geometry->setVertexArray(vertexArray);
	geometry->setNormalArray(normalArray, osg::Array::BIND_PER_VERTEX);
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,vertexArray->size()));

	geometry->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);//关闭灯光
	return geometry;
}


//设置该拖拽器默认形状
void TrackballCMDragger::setupDefaultGeometry()
{
	osg::Geode* geode = new osg::Geode;
	{
		osg::TessellationHints* hints = new osg::TessellationHints;
		hints->setCreateTop(false);
		hints->setCreateBottom(false);
		hints->setCreateBackFace(false);

		osg::Cylinder* cylinder = new osg::Cylinder;
		cylinder->setHeight(0.15f);
		osg::ShapeDrawable* cylinderDrawable = new osg::ShapeDrawable(cylinder,hints);
		geode->addDrawable(cylinderDrawable);
		setDrawableToAlwaysCull(*cylinderDrawable);
		geode->addDrawable(createCircleGeometry(1.0f, 100));
	}

	// 设置圆柱绘制模式
	{
		osg::PolygonMode* polymode = new osg::PolygonMode;
		polymode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
		geode->getOrCreateStateSet()->setAttributeAndModes(polymode,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
		geode->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(2.0f),osg::StateAttribute::ON);

#if !defined(OSG_GLES2_AVAILABLE)
		geode->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
#endif

	}

	// 每个方向添加
//	_xDragger->addChild(geode);
//	_yDragger->addChild(geode);
	_zDragger->addChild(geode);


	//将环旋转到自己定义的Z轴
	osg::Quat zrotation; zrotation.makeRotate(osg::Vec3d(0.0f, 0.0f, 1.0f), _zAxis);
	_zDragger->setMatrix(osg::Matrix(zrotation));

	_zDragger->setColor(osg::Vec4(0.0f,0.0f,1.0f,1.0f));


// 
// 	//旋转到X,Y相应位置,要在Z旋转的基础上，在旋转
// 	{
// 
// 		osg::Quat rotation; rotation.makeRotate(_zAxis, _xAxis);
// 		_xDragger->setMatrix(osg::Matrix(zrotation*rotation));//OSG旋转是从左到右顺序
// 	}
// 
// 	{
// 		osg::Quat rotation; rotation.makeRotate(_zAxis, _yAxis);
// 		_yDragger->setMatrix(osg::Matrix(zrotation*rotation));
// 	}
// 
// 
// 	_xDragger->setColor(osg::Vec4(1.0f,0.0f,0.0f,1.0f));
// 	_yDragger->setColor(osg::Vec4(0.0f,1.0f,0.0f,1.0f));

	// 添加不可见的球拖拽器.

// 	{
// 		osg::Drawable* sphereDrawable = new osg::ShapeDrawable(new osg::Sphere());
// 		setDrawableToAlwaysCull(*sphereDrawable);
// 		osg::Geode* sphereGeode = new osg::Geode;
// 		sphereGeode->addDrawable(sphereDrawable);
// 		_xyzDragger->addChild(sphereGeode);
// 	}
}




