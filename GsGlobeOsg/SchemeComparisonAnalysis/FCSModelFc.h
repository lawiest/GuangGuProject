#ifndef _FCSMODELFC_H_    //如果没有定义这个宏
#define _FCSMODELFC_H_     //定义这个宏

#include "SchemeComparisonAnalysis/CoreOSG.h"

#include "SchemeComparisonAnalysis/CoEarth.h"

#include "SchemeComparisonAnalysis/translateAxisCMDragger.h"

#include "SchemeComparisonAnalysis/TrackballCMDragger.h"

#include "SchemeComparisonAnalysis/ExtScaleAxisCMDragger.h"

#include "SchemeComparisonAnalysis/FindVertexVisitor.h"
//#include "SchemeComparisonAnalysis/FindVertexVisitor.h"

using namespace osgEarth;
using namespace CMFManipulator;

//FCS房屋类
class CModelFCS :public osg::MatrixTransform
{
public:
	CModelFCS(ULONGLONG *m_ukey,osg::ref_ptr<osg::Vec3dArray> m_modelLLVertices,osgEarth::MapNode* m_MapNode,osg::ref_ptr<osg::Group> m_root,std::vector<int> m_getTexNums,std::vector<osg::ref_ptr<osg::Image>> m_GeoImage,float *m_Hlength=new float,osg::Vec3d m_direction=osg::Vec3d(0.0f, 0.0f, 1.0f),osg::ref_ptr<osg::Vec3dArray> m_newmodelLLVertices=new osg::Vec3dArray,vector<osg::Matrix>* m_dragCMVectMatrix=new vector<osg::Matrix>,int * m_dragCMTimes=new int)
	{
		_mukey=m_ukey;
		_geokey=1;//墙面从1开始，屋顶为0

		 _Xmax=0;
		 _Xmin=0;
		 _Ymax=0;
		 _Ymin=0;

		 _modelLLVertices=m_modelLLVertices;
		 _premodelLLVertices=new osg::Vec3dArray;
		 _newLlVertices=m_newmodelLLVertices;
		 _prenewLlVertices=new osg::Vec3dArray;
		 
		 _dragCMVectMatrix=m_dragCMVectMatrix;
		 _dragCMTimes=m_dragCMTimes;

		 _predragCMVectMatrix=new vector<osg::Matrix>;
		 _predragCMVectMatrix->push_back(_predragTmpMatrix);
		 _predragCMVectMatrix->push_back(_predragTmpMatrix);
		 _predragCMVectMatrix->push_back(_predragTmpMatrix);
		 _predragCMTimes=new int;
		 *_predragCMTimes=0;

		 _direction=m_direction;
		 _mHlength=m_Hlength;

		 _mMapNode=m_MapNode;
		 _mMapSRS=m_MapNode->getMapSRS();
		 _mGeoSRS=m_MapNode->getMapSRS()->getGeographicSRS();

		 _Dragger=new CMFManipulator::Translate1DDragger;

		 _root=m_root;

		 _GeoImage=m_GeoImage;

		 _getTexNums=m_getTexNums;


		 Createextrusion();
	}

~CModelFCS()
	{

	}

//几何面的类
class CModelFGeometry:public osg::Geometry
{
public:
	CModelFGeometry(ULONGLONG m_mukey)
	{
		_mukeyg=m_mukey;
		_faceTexNum=0;
	}
	~CModelFGeometry()
	{

	}


public:
	//标识符
	ULONGLONG _mukeyg;
	//纹理贴图
	int _faceTexNum;//面纹理号
};

//创建二维纹理状态对象
osg::ref_ptr<osg::StateSet> createTexture2DState(osg::ref_ptr<osg::Image> image)
{
	//创建状态集对象
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	//创建二维纹理对象
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
	texture->setDataVariance(osg::Object::DYNAMIC);
	//设置边界处理
	texture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::REPEAT);
	texture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::REPEAT);
	texture->setWrap(osg::Texture2D::WRAP_R,osg::Texture2D::REPEAT);
	//设置滤波
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

	//设置贴图
	texture->setImage(image.get());
	texture->setUnRefImageDataAfterApply(true);//使用图像后，删除OPENGL的缓存。
//非自动纹理	//纹理到一个纹理坐标必须在0-1上
	stateset->setTextureAttributeAndModes(0,texture.get(),osg::StateAttribute::ON);

//自动纹理坐标
#if 0
	osg::ref_ptr<osg::TexGen> texgen=new osg::TexGen;
	texgen->setMode(osg::TexGen::OBJECT_LINEAR);

	//点经纬度坐标转换为世界坐标
		osg::Vec3dArray::iterator iter=_modelLLVertices->begin();
		//位置
		osgEarth::GeoPoint mapPos=osgEarth::GeoPoint(_mGeoSRS,(*iter).x(),(*iter).y(),(*iter).z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_mMapSRS);
		osg::Vec3d worldMapPosition;
		mapPos.toWorld(worldMapPosition);//经纬度转换为世界坐标
		worldMapPosition.normalize();

//	texgen->setPlane(osg::TexGen::T,osg::Plane(worldMapPosition,0.0f));//设置生成坐标的参考轴，涉及纹理的方向

	texgen->setPlane(osg::TexGen::T,osg::Plane(0.0f,0.0f,1.0f,0.0f));//设置生成坐标的参考轴，涉及纹理的方向

	stateset->setTextureAttribute(0,texture.get(),osg::StateAttribute::OVERRIDE);
	stateset->setTextureMode(0,GL_TEXTURE_2D,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);

	stateset->setTextureAttribute(0,texgen.get(),osg::StateAttribute::OVERRIDE);//自动纹理绑定(生成)
	//设置各个方向轴是否采用自动纹理坐标
	stateset->setTextureMode(0,GL_TEXTURE_GEN_S,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	stateset->setTextureMode(0,GL_TEXTURE_GEN_T,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	stateset->setTextureMode(0,GL_TEXTURE_GEN_R,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);


#endif

	return stateset.get() ;
}

osg::ref_ptr<osg::Geometry> CreateSideQuad(osg::ref_ptr<osg::Vec3Array> m_Vertices,std::vector<osg::ref_ptr<osg::Image>> m_GeoImage)
{
	ULONGLONG geomNum=*_mukey*1000+_geokey;
	osg::ref_ptr<CModelFGeometry> ceVer = new CModelFGeometry(geomNum);
	
	ceVer->setVertexArray( m_Vertices.get() );

	ceVer->addPrimitiveSet( new osg::DrawArrays(GL_QUADS,0, 4) );//原点面

//纹理处理
#if 1
	//纹理所占的世界场长度
	float d=0.0f;
	//高度
	double height = (m_Vertices->at(1)-m_Vertices->at(0)).length();
	//纹理所占的世界场长度
	if (height<1000)
	{
		d=15.0f;
	} 
	else
	{
		d=15000.0f;
	}

	int Ytex=height/d;
	//宽度：
	double length = (m_Vertices->at(2)-m_Vertices->at(1)).length();
	int Xtex=length/d;
//	ceVer->addPrimitiveSet( m_indicesVertices.get() );

/*****
//非自动纹理
///显示纹理坐标可以根据面的大小宽度来直接设定纹理坐标
*****/
	osg::ref_ptr<osg::Vec2Array> vt2=new osg::Vec2Array;
	//判断是否有过窄的区域
	if (Xtex==0&&Ytex!=0)
	{	
		vt2->push_back(osg::Vec2(1.0f,0.0f));
		vt2->push_back(osg::Vec2(1.0f,Ytex));
		vt2->push_back(osg::Vec2(0.0f,Ytex));
		vt2->push_back(osg::Vec2(0.0f,0.0f));
	} 
	else if (Ytex==0&&Xtex!=0)
	{
		vt2->push_back(osg::Vec2(Xtex,0.0f));
		vt2->push_back(osg::Vec2(Xtex,1.0f));
		vt2->push_back(osg::Vec2(0.0f,1.0f));
		vt2->push_back(osg::Vec2(0.0f,0.0f));
	} 
	else if(Xtex==0&&Ytex==0)
	{
		vt2->push_back(osg::Vec2(1.0f,0.0f));
		vt2->push_back(osg::Vec2(1.0f,1.0f));
		vt2->push_back(osg::Vec2(0.0f,1.0f));
		vt2->push_back(osg::Vec2(0.0f,0.0f));
	}
	else
	{
		vt2->push_back(osg::Vec2(Xtex,0.0f));
		vt2->push_back(osg::Vec2(Xtex,Ytex));
		vt2->push_back(osg::Vec2(0.0f,Ytex));
		vt2->push_back(osg::Vec2(0.0f,0.0f));
	}

	ceVer->setTexCoordArray(0,vt2);
	
//每个面贴纹理

/*
	if (_geokey<m_GeoImage.size())
	{*/
		ceVer->_faceTexNum=_geokey;
		ceVer->setStateSet(createTexture2DState(m_GeoImage.at(_getTexNums.at(_geokey++))));//注意最后一个_geokey要自加一,,
		
/*
	}
	else
	{
		ceVer->_faceTexNum=m_GeoImage.size()-1;
		ceVer->setStateSet(createTexture2DState(m_GeoImage.at(m_GeoImage.size()-1),0));
		_geokey++;
	}*/
#endif

	osgUtil::SmoothingVisitor::smooth( *ceVer );
	
	cout<<geomNum<<endl;

	return ceVer.get();
}

//新经纬度坐标
void saveNewLlVertices(float length,osg::Vec3d direction=osg::Vec3d(0.0f, 0.0f, 1.0f))
{

	osg::Vec3d offset = direction * length;//拉伸变量

	_newLlVertices->clear();

	//将原经纬度顶点保存
	_newLlVertices->insert( _newLlVertices->begin(), _modelLLVertices->begin(),_modelLLVertices->end() );

	//将新生成经纬度顶点保存
	for (osg::Vec3dArray::iterator iter=_modelLLVertices->begin();iter!=_modelLLVertices->end();iter++)
	{		
		_newLlVertices->push_back( (*iter) + offset );
	}
}

//创建模型
void Createextrusion()
{
//	osg::ref_ptr<osg::Group> agroup=new osg::Group;//房屋类型构建为选中做铺垫
	if (!_modelLLVertices)
	{
		cout<<"CModel坐标没有!"<<endl;
		system("pause");
	}
 
//	osg::ref_ptr<CModelF> agroup=new CModelF(_mukey,_modelLLVertices,_mMapNode,_root,_getTexNums,_mHlength,_newLlVertices);
	
	//原点数
	unsigned int _verticesllNum = _modelLLVertices->size();

/*
	osg::Vec3d offset = direction * length;//拉伸变量

	_newLlVertices->clear();

	//将原经纬度顶点保存
	_newLlVertices->insert( _newLlVertices->begin(), _modelLLVertices->begin(),_modelLLVertices->end() );
	
	//将新生成经纬度顶点保存
	for (osg::Vec3dArray::iterator iter=_modelLLVertices->begin();iter!=_modelLLVertices->end();iter++)
	{		
			_newLlVertices->push_back( (*iter) + offset );
	}
*/

	saveNewLlVertices(*_mHlength,_direction);

	//点经纬度坐标转换为世界坐标
	osg::ref_ptr<osg::Vec3Array> modelWorldVertices=new osg::Vec3Array;	
		//屋顶顶点
		osg::ref_ptr<osg::Vec3Array> WDmodelWorldVertices=new osg::Vec3Array;
		osg::ref_ptr<osg::Vec3Array> WDmodelllVertices=new osg::Vec3Array;
		int i=0;
	for (osg::Vec3dArray::iterator iter=_newLlVertices->begin();iter!=_newLlVertices->end();iter++)
	{

		//位置
		osgEarth::GeoPoint mapPos=osgEarth::GeoPoint(_mGeoSRS,(*iter).x(),(*iter).y(),(*iter).z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_mMapSRS);

		osg::Vec3d worldMapPosition;
		mapPos.toWorld(worldMapPosition);//经纬度转换为世界坐标

		modelWorldVertices->push_back(worldMapPosition);
		
		i++;
		if (i>_verticesllNum)
		{
			WDmodelWorldVertices->push_back(worldMapPosition);
			WDmodelllVertices->push_back(*iter);
		}
	}

//顶面
#if 1
	osg::ref_ptr<CModelFGeometry> extrusionVer = new CModelFGeometry(*_mukey*1000);
	//定义颜色数组 
/*
	osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array;//四个坐标点类型指针
//	c->push_back( osg::Vec4( 1.f, 0.f, 1.f, 1.f ) ); 
//	c->push_back( osg::Vec4( 0.f, 1.f, 0.f, 1.f ) ); 
//	c->push_back( osg::Vec4( 0.f, 0.f, 1.f, 1.f ) ); 
	c->push_back( osg::Vec4( 1.f, 1.f, 0.f, 1.f ) ); 
	extrusionVer->setColorArray( c.get() ); 
	extrusionVer->setColorBinding( osg::Geometry::BIND_OFF );//设置颜色绑定方式为逐点绑定 BIND_PER_VERTEX //BIND_OVERALL*/

/*
	//底、顶面
	extrusionVer->setVertexArray( modelWorldVertices.get() );//索引需要有用
	//	extrusionVer->addPrimitiveSet( new osg::DrawArrays(GL_POLYGON,0, _verticesllNum) );//原点面
	extrusionVer->addPrimitiveSet( new osg::DrawArrays(GL_POLYGON,_verticesllNum, _verticesllNum) );//复制点面*/

	//顶面
	extrusionVer->setVertexArray( WDmodelWorldVertices.get() );//索引需要有用
	extrusionVer->addPrimitiveSet( new osg::DrawArrays(GL_POLYGON,0, _verticesllNum) );//复制点面

	//纹理坐标
	osg::ref_ptr<osg::Vec2Array> vt2=new osg::Vec2Array;
	int wdscale=5000;//屋顶经纬度坐标放缩比例
	osg::Vec2 temp0(WDmodelllVertices->at(0).x()*wdscale,WDmodelllVertices->at(0).y()*wdscale);

	for (int i=0;i<_verticesllNum;i++)
	{
		osg::Vec2 tempw(WDmodelllVertices->at(i).x()*wdscale-temp0.x(),WDmodelllVertices->at(i).y()*wdscale-temp0.y());
		vt2->push_back(tempw);
	} 
	extrusionVer->setTexCoordArray(0,vt2);
	extrusionVer->setStateSet(createTexture2DState(_GeoImage.at(_getTexNums.at(0))));
	extrusionVer->_faceTexNum=_getTexNums.at(0);


	osgUtil::Tessellator tessellator;
	tessellator.setTessellationType(osgUtil::Tessellator::TESS_TYPE_POLYGONS );
	tessellator.setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD );
	tessellator.retessellatePolygons( *extrusionVer );

	osgUtil::SmoothingVisitor::smooth( *extrusionVer );//自动生成法线

//	extrusionVer->setStateSet(createTexture2DState(m_GeoImage.at(0),0));
	osg::ref_ptr<osg::Geode> WD=new osg::Geode;
	

	WD->addDrawable(extrusionVer);

	this->addChild(WD);//屋顶要在墙前

#endif

//侧面
	//侧面一起绘制
#if 0
	osg::ref_ptr<osg::Geometry> ceVer = new osg::Geometry;
	ceVer->setVertexArray( modelWorldVertices.get() );

	//共用边线
#if 1
	osg::ref_ptr<osg::DrawElementsUInt> indicesVertices =new osg::DrawElementsUInt( GL_QUAD_STRIP );//创建索引数组，利用索引绘制顶点数组，注意顺序
	for ( unsigned int i=0; i<_verticesllNum; ++i )
	{
		indicesVertices->push_back( i );
		indicesVertices->push_back(i+_verticesllNum);
	}

	indicesVertices->push_back( 0 );
	indicesVertices->push_back(_verticesllNum);///*逆向使用*/(_verticesllNum-1-i) + _verticesllNum );* /
#endif

	ceVer->addPrimitiveSet( indicesVertices.get() );

	ceVer->setStateSet(createTexture2DState(image.get(),0));

	osgUtil::SmoothingVisitor::smooth( *ceVer );
	osg::ref_ptr<osg::Geode> WALL=new osg::Geode;
	WALL->addDrawable(ceVer);
	agroup->addChild(WALL);
#endif

	//侧面单独绘制
#if 1

	//顶点数据处理
#if 1
	osg::ref_ptr<osg::Geode> WALL=new osg::Geode;


	for ( unsigned int i=0; i<_verticesllNum-1; i++ )//侧面为矩形
	{
		osg::ref_ptr<osg::Vec3Array> GeoVertices = new osg::Vec3Array;
		GeoVertices->push_back(modelWorldVertices->at(i));
		GeoVertices->push_back(modelWorldVertices->at(i+_verticesllNum));
		GeoVertices->push_back(modelWorldVertices->at(i+1+_verticesllNum));
		GeoVertices->push_back(modelWorldVertices->at(i+1));

		//每个面循环完就绘制
		WALL->addDrawable(CreateSideQuad(GeoVertices,_GeoImage));

	}
	osg::ref_ptr<osg::Vec3Array> GeoVerticeslast = new osg::Vec3Array;
	GeoVerticeslast->push_back(modelWorldVertices->at(_verticesllNum-1));
	GeoVerticeslast->push_back(modelWorldVertices->at(_verticesllNum-1+_verticesllNum));
	GeoVerticeslast->push_back(modelWorldVertices->at(0+_verticesllNum));
	GeoVerticeslast->push_back(modelWorldVertices->at(0));

	//最后一个面绘制
	WALL->addDrawable(CreateSideQuad(GeoVerticeslast,_GeoImage));
	

#endif

	//agroup->addChild(WALL);

#endif

	this->addChild(WALL);

}

//创建预显示模型
osg::ref_ptr<osg::Geode> CreateTransparentModel(osg::ref_ptr<osg::Vec3dArray> newllVertices)
{
	if (!newllVertices)
	{
		cout<<"预显示坐标没有!"<<endl;
		system("pause");
	}

	unsigned int _verticesllNum=newllVertices->size()/2;

	//点经纬度坐标转换为世界坐标
	osg::ref_ptr<osg::Vec3Array> modelWorldVertices=new osg::Vec3Array;	

	for (osg::Vec3dArray::iterator iter=newllVertices->begin();iter!=newllVertices->end();iter++)
	{

		//位置
		osgEarth::GeoPoint mapPos=osgEarth::GeoPoint(_mGeoSRS,(*iter).x(),(*iter).y(),(*iter).z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_mMapSRS);

		osg::Vec3d worldMapPosition;
		mapPos.toWorld(worldMapPosition);//经纬度转换为世界坐标

		modelWorldVertices->push_back(worldMapPosition);
	}

	osg::ref_ptr<osg::Geode> transparentModel=new osg::Geode;

	//顶面
#if 1
	osg::ref_ptr<osg::Geometry> extrusionVer = new osg::Geometry;
	//底、顶面
	extrusionVer->setVertexArray( modelWorldVertices.get() );//索引需要有用
	extrusionVer->addPrimitiveSet( new osg::DrawArrays(GL_POLYGON,_verticesllNum, _verticesllNum) );//复制点面

	osgUtil::Tessellator tessellator;
	tessellator.setTessellationType(osgUtil::Tessellator::TESS_TYPE_POLYGONS );
	tessellator.setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD );
	tessellator.retessellatePolygons( *extrusionVer );

	osgUtil::SmoothingVisitor::smooth( *extrusionVer );//自动生成法线

	transparentModel->addDrawable(extrusionVer);

#endif

	//侧面一起绘制
#if 1
	osg::ref_ptr<osg::Geometry> ceVer = new osg::Geometry;
	ceVer->setVertexArray( modelWorldVertices.get() );

//共用边线
	osg::ref_ptr<osg::DrawElementsUInt> indicesVertices =new osg::DrawElementsUInt( GL_QUAD_STRIP );//创建索引数组，利用索引绘制顶点数组，注意顺序
	for ( unsigned int i=0; i<_verticesllNum; ++i )
	{
		indicesVertices->push_back( i );
		indicesVertices->push_back(i+_verticesllNum);
	}

	indicesVertices->push_back( 0 );
	indicesVertices->push_back(_verticesllNum);/// *逆向使用* /(_verticesllNum-1-i) + _verticesllNum );* /


	ceVer->addPrimitiveSet( indicesVertices.get() );

	osgUtil::SmoothingVisitor::smooth( *ceVer );
	transparentModel->addDrawable(ceVer);

#endif
		//设置透明+颜色或材质
#if 1

	//方法1
	osg::StateSet* state = transparentModel->getOrCreateStateSet();
	state->setMode(GL_BLEND,osg::StateAttribute::ON);
	osg::ref_ptr<osg::Material> mat = new osg::Material;
	//漫发射光
	osg::Vec4 destDiffuse;
	destDiffuse.set(1.0,1.0,0.0,0.5);
	mat->setDiffuse(osg::Material::FRONT_AND_BACK,destDiffuse);
	//环境光
	mat->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(1.0,1.0,0.0,0.5));
	//设置材质的混合光颜色
	mat->setTransparency(osg::Material::FRONT_AND_BACK,0.5);
	state->setAttributeAndModes(mat,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
	state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);


	//方法2
/*	osg::StateSet* state = root->getOrCreateStateSet();
	//关闭灯光

	state->setMode(GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
	//打开混合融合模式
	state->setMode(GL_BLEND,osg::StateAttribute::ON);
	state->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
	state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	//使用BlendFunc实现透明效果
	osg::BlendColor* bc =new osg::BlendColor(osg::Vec4(1.0,1.0,1.0,0.0));
	osg::BlendFunc*bf = new osg::BlendFunc();
	state->setAttributeAndModes(bf,osg::StateAttribute::ON);
	state->setAttributeAndModes(bc,osg::StateAttribute::ON);
	bf->setSource(osg::BlendFunc::CONSTANT_ALPHA);
	bf->setDestination(osg::BlendFunc::ONE_MINUS_CONSTANT_ALPHA);
	bc->setConstantColor(osg::Vec4(1,1,1,0.5));*/

#endif

	return transparentModel.get();

}

//拖拽器
void EnableDragger(DragType chooseDragType) 
{ 
		//计算拖拽器方向
		osg::ref_ptr<osg::Vec3dArray> xyzcModelLlVertices=new osg::Vec3dArray;

		osg::ref_ptr<osg::Vec3dArray> xyzcModelWorldVertices=new osg::Vec3dArray;

		{
			//1.中心点世界转经纬
			GeoPoint convertPoint;

			convertPoint.fromWorld(_mGeoSRS,this->getBound().center());

			xyzcModelLlVertices->push_back(osg::Vec3d(convertPoint.x(),convertPoint.y(),convertPoint.z()));
			//2.获得X,Y,Z方向三个点
			osg::Vec3d xCenterM(convertPoint.x()-0.001f,convertPoint.y(),convertPoint.z());
			osg::Vec3d yCenterM(convertPoint.x(),convertPoint.y()-0.001f,convertPoint.z());
			osg::Vec3d zCenterM(convertPoint.x(),convertPoint.y(),convertPoint.z()-10.001f);

			xyzcModelLlVertices->push_back(xCenterM);
			xyzcModelLlVertices->push_back(yCenterM);
			xyzcModelLlVertices->push_back(zCenterM);
			//3.X、Y、Z、C全部转为世界坐标

			for(osg::Vec3dArray::iterator iter=xyzcModelLlVertices->begin();iter!=xyzcModelLlVertices->end();iter++)
			{
				osgEarth::GeoPoint mapPos=osgEarth::GeoPoint(_mGeoSRS,(*iter).x(),(*iter).y(),(*iter).z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_mMapSRS);

				osg::Vec3d tempWVarr;
				mapPos.toWorld(tempWVarr);//经纬度转换为世界坐标

				xyzcModelWorldVertices->push_back(tempWVarr);//新坐标矩阵计算
			}
		}

		if (chooseDragType==TranslateAxis)
		{

			//将上次拖拽器变换最后一次放到新拖拽器变换第一次中
			this->_predragTempTimes=*this->_predragCMTimes;
			if (this->_predragTempTimes>0)
			{
				this->_predragTmpMatrix=this->_predragCMVectMatrix->at((this->_predragTempTimes-1)%3);
			}
			else if (this->_predragTempTimes==0)
			{
				this->_predragTmpMatrix=this->_predragCMVectMatrix->at(0);
			}

			vector<osg::Matrix>::iterator iterm=_Dragger->_DragVectMatrix->begin();
			for (int i=0;i<3&&iterm!=_Dragger->_DragVectMatrix->end();i++)
			{
				this->_predragCMVectMatrix->at(i)=*iterm;

				iterm++;
			}

			this->_predragCMVectMatrix->at(0)=this->_predragTmpMatrix;

			*this->_predragCMTimes=*_Dragger->_DragTimes;

			_Dragger=new TranslateAxisCMDragger(xyzcModelWorldVertices);

			//主要目的是保证地址不丢失
			this->_dragCMTimes=_Dragger->_DragTimes;
			this->_dragCMVectMatrix=_Dragger->_DragVectMatrix;

			_Dragger->addTransformUpdating(this); //参数为 matrixtransform类型

			float scale = this->getBound().radius();//大小
			_Dragger->setMatrix(osg::Matrix::scale(scale, scale, scale) * osg::Matrix::translate(this->getBound().center()));

		} 
		else if(chooseDragType==Rotateball)
		{
			//将上次拖拽器变换最后一次放到新拖拽器变换第一次中
			this->_predragTempTimes=*this->_predragCMTimes;
			if (this->_predragTempTimes>0)
			{
				this->_predragTmpMatrix=this->_predragCMVectMatrix->at((this->_predragTempTimes-1)%3);
			}
			else if (this->_predragTempTimes==0)
			{
				this->_predragTmpMatrix=this->_predragCMVectMatrix->at(0);
			}

			vector<osg::Matrix>::iterator iterm=_Dragger->_DragVectMatrix->begin();
			for (int i=0;i<3&&iterm!=_Dragger->_DragVectMatrix->end();i++)
			{
				this->_predragCMVectMatrix->at(i)=*iterm;

				iterm++;
			}

			this->_predragCMVectMatrix->at(0)=this->_predragTmpMatrix;

			*this->_predragCMTimes=*_Dragger->_DragTimes;

			_Dragger=new TrackballCMDragger(false,xyzcModelWorldVertices);

			//主要目的是保证地址不丢失
			this->_dragCMTimes=_Dragger->_DragTimes;
			this->_dragCMVectMatrix=_Dragger->_DragVectMatrix;

			_Dragger->addTransformUpdating(this); //参数为 matrixtransform类型

			float scale = this->getBound().radius();//大小
			_Dragger->setMatrix(osg::Matrix::scale(scale, scale, scale) * osg::Matrix::translate(this->getBound().center()));
		}
		else if (chooseDragType==ExtScaleAxis)
		{

			//在模型拉伸前计算现在的顶点
			{
				osg::Matrix newModelMatrix=this->getMatrix();
				//此处是否可以更快？
				osg::ref_ptr<osg::Vec3dArray> newmodelWorldVertices=new osg::Vec3dArray;

				//原模型顶点更新

				//模型原点保存
				this->_premodelLLVertices->clear();
				this->_premodelLLVertices->insert( this->_premodelLLVertices->begin(), this->_modelLLVertices->begin(),this->_modelLLVertices->end() );//保存旧点

				for(osg::Vec3dArray::iterator iter=this->_modelLLVertices->begin();iter!=this->_modelLLVertices->end();iter++)
				{
					osgEarth::GeoPoint mapPos=osgEarth::GeoPoint(_mGeoSRS,(*iter).x(),(*iter).y(),(*iter).z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_mMapSRS);

					osg::Vec3d tempWVarr;
					mapPos.toWorld(tempWVarr);//经纬度转换为世界坐标

					newmodelWorldVertices->push_back(tempWVarr*newModelMatrix );//新坐标矩阵计算
				}
				this->_modelLLVertices->clear();

				for (osg::Vec3dArray::iterator iter=newmodelWorldVertices->begin();iter!=newmodelWorldVertices->end();iter++)
				{
					osg::Vec3d tempLVarr;

					GeoPoint convertPoint;

					convertPoint.fromWorld(_mGeoSRS,*iter);
					this->_modelLLVertices->push_back(osg::Vec3d(convertPoint.x(),convertPoint.y(),convertPoint.z()));
				}
				newmodelWorldVertices->clear();

				//预显示模型顶点更新	
				this->_prenewLlVertices->clear();
				this->_prenewLlVertices->insert( this->_prenewLlVertices->begin(), this->_newLlVertices->begin(),this->_newLlVertices->end() );//保存旧点

				for(osg::Vec3dArray::iterator iter=this->_newLlVertices->begin();iter!=this->_newLlVertices->end();iter++)
				{
					osgEarth::GeoPoint mapPos=osgEarth::GeoPoint(_mGeoSRS,(*iter).x(),(*iter).y(),(*iter).z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_mMapSRS);

					osg::Vec3d tempWVarr;
					mapPos.toWorld(tempWVarr);//经纬度转换为世界坐标

					newmodelWorldVertices->push_back(tempWVarr*newModelMatrix );//新坐标矩阵计算
				}
				this->_newLlVertices->clear();

				for (osg::Vec3dArray::iterator iter=newmodelWorldVertices->begin();iter!=newmodelWorldVertices->end();iter++)
				{
					osg::Vec3d tempLVarr;

					GeoPoint convertPoint;

					convertPoint.fromWorld(_mGeoSRS,*iter);
					this->_newLlVertices->push_back(osg::Vec3d(convertPoint.x(),convertPoint.y(),convertPoint.z()));
				}
				newmodelWorldVertices->clear();
			}

			//拉伸拖拽器位置
			osg::Vec3d dragLlPosition(xyzcModelLlVertices->at(0).x(),xyzcModelLlVertices->at(0).y(),this->_newLlVertices->at(0).z());//中心点经纬度坐标

			GeoPoint mapPos=osgEarth::GeoPoint(_mGeoSRS,dragLlPosition.x(),dragLlPosition.y(),dragLlPosition.z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_mMapSRS);
			osg::Vec3d dragWorldPosition;
			mapPos.toWorld(dragWorldPosition);//经纬度转换为世界坐标,//中心点世界坐标

			osg::ref_ptr<osg::Geode> temptGeode=CreateTransparentModel(this->_newLlVertices);
			osg::ref_ptr<osg::MatrixTransform> temptMT=new osg::MatrixTransform;
			temptMT->setName("temptMT");
			temptMT->addChild(temptGeode);
			osg::Vec3 m_modelposition=temptGeode->getBound().center();
			temptMT->postMult(osg::Matrix::translate(-m_modelposition) * osg::Matrix::scale(osg::Vec3d(0.95,0.95,0.95)) * osg::Matrix::translate(m_modelposition));	//放缩原理跟旋转类似


			this->setNodeMask(0);//隐藏原模型

			_root->addChild(temptMT);

			_Dragger=new  ExtScaleAxisCMDragger(xyzcModelWorldVertices);

			_Dragger->addTransformUpdating(temptMT); //参数为 matrixtransform类型

			//拖拽器大小 位置
			float scale = this->getBound().radius();
			//拖拽器Z轴高度
			float dragZHight=this->_newLlVertices->at(this->_newLlVertices->size()/2).z()-this->_newLlVertices->at(0).z()+5;

			_Dragger->setMatrix(osg::Matrix::scale(dragZHight,dragZHight, dragZHight) * osg::Matrix::translate(dragWorldPosition));
		}



		_Dragger->setupDefaultGeometry();
		_root->addChild(_Dragger); //直接将拖拽器添加在场景
		//	_Dragger->addTransformUpdating(this); //参数为 matrixtransform类型
		_Dragger->setHandleEvents(true); 
		_Dragger->setActivationModKeyMask(osgGA::GUIEventAdapter::MODKEY_CTRL);

}

void DisableDragger() 
{ 
		_root->removeChild(_Dragger);
		_Dragger->removeTransformUpdating(this); 
		_Dragger->setHandleEvents(false); 	
}


void getGeoPoint(int v,double& x,double& y,double& z)//根据索引值获取点的坐标
{
	if (_BlockHead.dim==2)
	{
		z=0.0;
		x=_coordData[2*v];
		y=_coordData[2*v+1];
	}
	if (_BlockHead.dim==3)
	{
		z=_coordData[3*v+2];
		x=_coordData[3*v];
		y=_coordData[3*v+1];
	}
}

void setGeomType(geoGEOMETRYTYPE1 gtype)//设置几何体对象的类型
{
	_BlockHead.type=gtype;
}
int getGeomType()//获取几何体对象的类型
{
	return _BlockHead.type;
}

int getNumPoints()//获取点的个数
{
	return _BlockHead.coordnum/_BlockHead.dim;//点的个数不是坐标个数
}
int getPartPoints(int p)//从0开始计算part，三元组的个数,每个部分点个数，不是坐标个数
{

	//获取每一部分的点的个数
	if (p==getNumInfo()-1)//最后一个三元组
	{
		return (_BlockHead.coordnum-_infoData[3*p]+1)/_BlockHead.dim;
	}
	else
	{
		return (_infoData[3*p+3]-_infoData[3*p])/_BlockHead.dim;
	}

}
int getPartType(int p)//获取几何对象某个部分的类型
{
	return _infoData[3*p+1];
}
int getPartOffset(int p)//获取几何对象某个部分的坐标偏移值
{
	return (_infoData[p*3]-1)/_BlockHead.dim;
}
int getNumInfo()//获得三元组的个数，即几何对象的组成部分个数
{
	return _BlockHead.infonum/3;//infodata为一个三元组，三元组的个数
}

public:
	//坐标
	float _Xmax;
	float _Xmin;
	float _Ymax;
	float _Ymin;

	osg::ref_ptr<osg::Vec3dArray> _modelLLVertices;
	osg::ref_ptr<osg::Vec3dArray> _premodelLLVertices;//模型原点保存
	//房屋标志符
	ULONGLONG *_mukey;
	//每个房屋的每个面标志符
	ULONGLONG _geokey;

	//经纬度转世界坐标
	osgEarth::MapNode* _mMapNode;
	const osgEarth::SpatialReference* _mMapSRS;
	const osgEarth::SpatialReference* _mGeoSRS;

	//拖拽器
	osg::ref_ptr<CMFManipulator::Dragger> _Dragger;
	osg::ref_ptr<osg::Group> _root;

	//拉伸
	osg::Vec3d _direction;
	float *_mHlength;//高度
	osg::ref_ptr<osg::Vec3dArray> _newLlVertices;//包含底面和顶面的点，点数是原来顶点的两倍
	osg::ref_ptr<osg::Vec3dArray> _prenewLlVertices;
	osg::Matrix _predragTmpMatrix;
	int _predragTempTimes;
	vector<osg::Matrix>* _dragCMVectMatrix;
	int * _dragCMTimes;
	vector<osg::Matrix>* _predragCMVectMatrix;
	int * _predragCMTimes;

	//纹理贴图
	//纹理包
	std::vector<osg::ref_ptr<osg::Image>> _GeoImage;
	//接收纹理编号
	std::vector<int> _getTexNums;


//FCS

	fcscmf_blob_head _BlockHead;
	long* _infoData;
	double* _coordData;
	bool _bValid;

};






#endif



