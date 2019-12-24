#include "SchemeComparisonAnalysis/ModelFc.h"
using namespace osgEarth;


CModelF::CModelF(ULONGLONG *m_ukey,osg::ref_ptr<osg::Vec3dArray> m_modelLLVertices,osgEarth::MapNode* m_MapNode,osg::ref_ptr<osg::Group> m_allCMFroot,std::vector<int> m_getTexNums,std::vector<osg::ref_ptr<osg::Image>> m_GeoImage,float *m_floorHeight,int *m_floorNums,osg::Vec3d m_direction,osg::ref_ptr<osg::Vec3dArray> m_newmodelLLVertices)
{
	_mukey=m_ukey;

	_geokeym=1;//墙面从1开始，屋顶为0

	_Xmax=new double;
	_Xmin=new double;
	_Ymax=new double;
	_Ymin=new double;

	_modelLLVertices=m_modelLLVertices;
	_premodelLLVertices=new osg::Vec3dArray;
	_newLlVertices=m_newmodelLLVertices;
	_prenewLlVertices=new osg::Vec3dArray;
	_InsetLineLlVertices=new osg::Vec3dArray;
	_preInsetLineLlVertices=new osg::Vec3dArray;

	_LineATextroot=new osg::Group;
	this->addChild(_LineATextroot);

	_direction=m_direction;


	_mMapNode=m_MapNode;
	_mMapSRS=m_MapNode->getMapSRS();
	_mGeoSRS=m_MapNode->getMapSRS()->getGeographicSRS();

	_Dragger=new CMFManipulator::Translate1DDragger;
	_mAllCMfroot=m_allCMFroot;
	

	_GeoImage=m_GeoImage;

	_getTexNums=m_getTexNums;

	_mfloorHeight=*m_floorHeight;//每层高度
	_mfloorNums=*m_floorNums;//楼层数
	_InsertValue=0.0001;

	CreateFloorextrusion();
}

CModelF::CModelF(ULONGLONG *m_ukey,osgEarth::MapNode* m_MapNode)
{
	_mukey=m_ukey;
	_mMapNode=m_MapNode;
	_mMapSRS=m_MapNode->getMapSRS();
	_mGeoSRS=m_MapNode->getMapSRS()->getGeographicSRS();
}

CModelF::CModelFFloorGeometry::CModelFFloorGeometry(ULONGLONG m_mukeyFloorGeom,ULONGLONG m_mukeyWallGeom,ULONGLONG m_mukeyCMFGeom,int m_faceTexNumGeom)
{
	_mukeyFloorGeom=m_mukeyFloorGeom;
	_mukeyWallGeom=m_mukeyWallGeom;
	_mukeyCMFGeom=m_mukeyCMFGeom;

	_faceTexNumGeom=m_faceTexNumGeom;
}

CModelF::CModelFWallGeode::CModelFWallGeode(ULONGLONG m_mukeyWallGeode,ULONGLONG m_mukeyCMFGeom,int m_faceTexNumGeode) 
{
	_mukeyWallGeode=m_mukeyWallGeode;
	_mukeyCMFGeode=m_mukeyCMFGeom;

	_faceTexNumGeode=m_faceTexNumGeode;
}

//创建二维纹理状态对象
osg::ref_ptr<osg::StateSet> CModelF::createTexture2DState(osg::ref_ptr<osg::Image> image)
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

	texture->setResizeNonPowerOfTwoHint(false);//设置/获取是否自动转换纹理图片尺寸到2的幂次方;


//非自动纹理	//纹理到一个纹理坐标必须在0-1上
	stateset->setTextureAttributeAndModes(0,texture.get(),osg::StateAttribute::ON||osg::StateAttribute::PROTECTED);
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	return stateset.get() ;
}

void CModelF::x_min_max(osg::ref_ptr<osg::Vec3dArray> m_modelLLVertices,double* x_min,double* x_max)
{
	osg::Vec3dArray::iterator iter1=m_modelLLVertices->begin();

	double xmax=(*iter1).x();
	double xmin=(*iter1).x();
	for (osg::Vec3dArray::iterator iter=m_modelLLVertices->begin();iter!=m_modelLLVertices->end();iter++)
	{			

		if(xmax < (*iter).x()) xmax = (*iter).x();

		if(xmin > (*iter).x()) xmin = (*iter).x();

	}

	*x_max=xmax;
	*x_min=xmin;
}

void CModelF::y_min_max(osg::ref_ptr<osg::Vec3dArray> m_modelLLVertices,double* y_min,double* y_max)
{

	osg::Vec3dArray::iterator iter1=m_modelLLVertices->begin();

	double ymax=(*iter1).y();
	double ymin=(*iter1).y();
	for (osg::Vec3dArray::iterator iter=m_modelLLVertices->begin();iter!=m_modelLLVertices->end();iter++)
	{			
		if(ymax < (*iter).y()) ymax = (*iter).y();

		if(ymin > (*iter).y()) ymin = (*iter).y();

	}

	*y_max=ymax;
	*y_min=ymin;
}
/****修改纹理、颜色****/
#if 1 
//纹理
void CModelF::setCModelWallTexture(osg::ref_ptr<osg::Texture2D> mtexture)
{
	mtexture->setResizeNonPowerOfTwoHint(false);//设置/获取是否自动转换纹理图片尺寸到2的幂次方;

	for (int j=1/*0不要屋顶*/;j<this->getNumChildren();j++)
	{

		osg::ref_ptr<CModelF::CModelFWallGeode> geodenode=dynamic_cast<CModelF::CModelFWallGeode*>(this->getChild(j));
		if (geodenode==NULL) return;

		for(int i=0;i<geodenode->getNumDrawables();i++)
		{
			osg::ref_ptr<CModelF::CModelFFloorGeometry> geometryGeodenode=dynamic_cast<CModelF::CModelFFloorGeometry*>(geodenode->getDrawable(i)->asGeometry());

			geometryGeodenode->_faceTexNumGeom=1;
			geometryGeodenode->getOrCreateStateSet()->setTextureAttributeAndModes(0,mtexture,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);//替换面 几何体纹理
		}
	}
}
void CModelF::CModelFWallGeode::setCModelFaceTexture(osg::ref_ptr<osg::Texture2D> mtexture)
{
	mtexture->setResizeNonPowerOfTwoHint(false);//设置/获取是否自动转换纹理图片尺寸到2的幂次方;

	for(int i=0;i<this->getNumDrawables();i++)
	{
		osg::ref_ptr<CModelF::CModelFFloorGeometry> geometryGeodenode=dynamic_cast<CModelF::CModelFFloorGeometry*>(this->getDrawable(i)->asGeometry());

		geometryGeodenode->_faceTexNumGeom=1;
		geometryGeodenode->getOrCreateStateSet()->setTextureAttributeAndModes(0,mtexture,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);//替换面 几何体纹理
	}
}
void CModelF::CModelFFloorGeometry::setCModelFloorTexture(osg::ref_ptr<osg::Texture2D> mtexture)
{
//	mtexture->setResizeNonPowerOfTwoHint(false);//设置/获取是否自动转换纹理图片尺寸到2的幂次方;

	this->getOrCreateStateSet()->setTextureAttributeAndModes(0,mtexture,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);//替换面 几何体纹理
}
//颜色
void CModelF::setCModelWallColor(osg::ref_ptr<osg::Vec4dArray> mcolor)
{
	for (int j=0;j<this->getNumChildren();j++)
	{

		osg::ref_ptr<CModelF::CModelFWallGeode> geodenode=dynamic_cast<CModelF::CModelFWallGeode*>(this->getChild(j));
		if (geodenode==NULL) return;

		for(int i=0;i<geodenode->getNumDrawables();i++)
		{
			osg::ref_ptr<CModelF::CModelFFloorGeometry> geometryGeodenode=dynamic_cast<CModelF::CModelFFloorGeometry*>(geodenode->getDrawable(i)->asGeometry());

			geometryGeodenode->_faceTexNumGeom=1;
			geometryGeodenode->setCModelFloorColor(mcolor);//绑定颜色
		}
	}
}
void CModelF::CModelFWallGeode::setCModelFaceColor(osg::ref_ptr<osg::Vec4dArray> mcolor)
{
	for(int i=0;i<this->getNumDrawables();i++)
	{
		osg::ref_ptr<CModelF::CModelFFloorGeometry> geometryGeodenode=dynamic_cast<CModelF::CModelFFloorGeometry*>(this->getDrawable(i)->asGeometry());

		geometryGeodenode->_faceTexNumGeom=1;
		geometryGeodenode->setCModelFloorColor(mcolor);//绑定颜色
	}
}
void CModelF::CModelFFloorGeometry::setCModelFloorColor(osg::ref_ptr<osg::Vec4dArray> mcolor)
{
	this->setColorArray( mcolor.get() ,osg::Array::BIND_OVERALL); 
	this->setColorBinding( osg::Geometry::BIND_OVERALL );//设置颜色绑定方式为逐
	this->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::PROTECTED||osg::StateAttribute::OFF);
}

#endif
//****楼层****//
#if 1

//绘制一层
osg::ref_ptr<osg::Geometry> CModelF::CreateFloorQuad(osg::ref_ptr<osg::Vec3Array> m_Vertices,ULONGLONG m_geodeNum,int m_TexNum)
{
ULONGLONG geomNum=m_geodeNum+_geokeyf;
	osg::ref_ptr<CModelFFloorGeometry> ceVer = new CModelFFloorGeometry(geomNum,m_geodeNum,*_mukey,m_TexNum);

	//定义颜色数组 
	osg::ref_ptr<osg::Vec4dArray> colorva=new osg::Vec4dArray;
	colorva->push_back( osg::Vec4( 1.f, 1.0f, 1.f, 1.f ) ); 
	ceVer->setColorArray( colorva.get() ,osg::Array::BIND_PER_PRIMITIVE_SET); 
	ceVer->setColorBinding( osg::Geometry::BIND_PER_PRIMITIVE_SET );//设置颜色绑定方式为逐点绑定 BIND_PER_VERTEX
	ceVer->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::PROTECTED||osg::StateAttribute::OFF);

	ceVer->setVertexArray( m_Vertices.get() );
	ceVer->addPrimitiveSet( new osg::DrawArrays(GL_QUADS,0, 4) );//原点面
	//纹理处理
#if 1
	//纹理所占的世界场长度
	float d=0.0f;
	//高度
	double height = (m_Vertices->at(2)-m_Vertices->at(1)).length();
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
	double length = (m_Vertices->at(1)-m_Vertices->at(0)).length();
	int Xtex=length/d;

	/*****
	//非自动纹理
	///显示纹理坐标可以根据面的大小宽度来直接设定纹理坐标
	*****/
	osg::ref_ptr<osg::Vec2Array> vt2=new osg::Vec2Array;
	//判断是否有过窄的区域
	if (Xtex==0&&Ytex!=0)
	{	
		vt2->push_back(osg::Vec2(0.0f,0.0f));
		vt2->push_back(osg::Vec2(1.0f,0.0f));
		vt2->push_back(osg::Vec2(1.0f,Ytex));
		vt2->push_back(osg::Vec2(0.0f,Ytex));	
	} 
	else if (Ytex==0&&Xtex!=0)
	{
		vt2->push_back(osg::Vec2(0.0f,0.0f));
		vt2->push_back(osg::Vec2(Xtex,0.0f));
		vt2->push_back(osg::Vec2(Xtex,1.0f));
		vt2->push_back(osg::Vec2(0.0f,1.0f));
	} 
	else if(Xtex==0&&Ytex==0)
	{
		vt2->push_back(osg::Vec2(0.0f,0.0f));
		vt2->push_back(osg::Vec2(1.0f,0.0f));
		vt2->push_back(osg::Vec2(1.0f,1.0f));
		vt2->push_back(osg::Vec2(0.0f,1.0f));
	}
	else
	{
		vt2->push_back(osg::Vec2(0.0f,0.0f));
		vt2->push_back(osg::Vec2(Xtex,0.0f));
		vt2->push_back(osg::Vec2(Xtex,Ytex));
		vt2->push_back(osg::Vec2(0.0f,Ytex));	
	}

	ceVer->setTexCoordArray(0,vt2);

	//每个面贴纹理

	if (_getTexNums.size()!=0)
	{
		ceVer->_faceTexNumGeom=m_TexNum;
		ceVer->setStateSet(createTexture2DState(_GeoImage.at(_getTexNums.at(/*m_TexNum*/0))));//注意最后一个_geokey要自加一,,
		_geokeyf++;
	} 
	else
	{
	//	ceVer->_faceTexNum=m_TexNum;
		ceVer->setStateSet(createTexture2DState(_GeoImage.at(0)));//注意最后一个_geokey要自加一,,
		_geokeyf++;
	}

#endif

	osgUtil::SmoothingVisitor::smooth( *ceVer );

	cout<<"楼层号："<<geomNum<<endl;

	return ceVer.get();
}

//绘制一个面
osg::ref_ptr<osg::Geode> CModelF::CreateFloorSideQuad(osg::ref_ptr<osg::Vec3Array> m_Vertices)//平行输入的点
{
	ULONGLONG geodeNum=*_mukey*100000000+_geokeym*10000;
	osg::ref_ptr<CModelFWallGeode> WALLceVer;

	if (_getTexNums.size()!=0)
	{
		WALLceVer = new CModelFWallGeode(geodeNum,*_mukey,_getTexNums.at(_geokeym));
		WALLceVer->_faceTexNumGeode=_getTexNums.at(_geokeym);
	}
	else
	{
		WALLceVer = new CModelFWallGeode(geodeNum,*_mukey);
		WALLceVer->_faceTexNumGeode=0;
	}
	
	cout<<"墙 面 号："<<geodeNum<<endl;

	_geokeyf=1;//楼层从1开始，墙面号为0

	int sss=0;
	for (int i=0;i<_mfloorNums;i++)//修改为顺时针
	{
		osg::ref_ptr<osg::Vec3Array> GeoVertices = new osg::Vec3Array;

		GeoVertices->push_back(m_Vertices->at(sss));
		GeoVertices->push_back(m_Vertices->at(sss+1));
		GeoVertices->push_back(m_Vertices->at(sss+3));
		GeoVertices->push_back(m_Vertices->at(sss+2));
		 
		WALLceVer->addDrawable(CreateFloorQuad(GeoVertices,geodeNum,WALLceVer->_faceTexNumGeode));
		
		sss+=2;

	}

	_geokeym++;

	WALLceVer->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::PROTECTED||osg::StateAttribute::OFF);//禁止混合以及防止其他节点对其影响

	return WALLceVer.get();
}

//新经纬度坐标，每层楼的顶点
void CModelF::saveFloorNewLlVertices(osg::Vec3d direction)
{
	_newLlVertices->clear();

	//将原经纬度顶点保存
	_newLlVertices->insert( _newLlVertices->begin(), _modelLLVertices->begin(),_modelLLVertices->end() );

	for (int i=0;i<_mfloorNums;i++)
	{
		osg::Vec3d offset = direction * _mfloorHeight*(i+1);//第i+1层拉伸变量

		//将新生成经纬度顶点保存
		for (osg::Vec3dArray::iterator iter=_modelLLVertices->begin();iter!=_modelLLVertices->end();iter++)
		{		
			_newLlVertices->push_back( (*iter) + offset );
		}
	}
}

//边界内插点
void CModelF::InsertLinePoint(osg::Vec3d direction)
{
	osg::ref_ptr<osg::Vec3dArray> m_tInsetLineLlVertices=new osg::Vec3dArray;

	//将原经纬度顶点保存
	osg::ref_ptr<osg::Vec3dArray> m_tempLlVertices=new osg::Vec3dArray;
	m_tempLlVertices->insert( m_tempLlVertices->begin(), _modelLLVertices->begin(),_modelLLVertices->end() );
	int m_VetNums=m_tempLlVertices->size();
	_InsetLineTime=new int[m_VetNums];
	_InsetLineDirect=new osg::Vec3dArray;
		for (int i=0;i<m_VetNums-1;i++)
		{
			_InsetLineTime[i]=	get2PointInserttime(m_tempLlVertices->at(i+1),(m_tempLlVertices->at(i)));
			osg::Vec3d tempvec3d=(m_tempLlVertices->at(i+1).operator-((m_tempLlVertices->at(i)))/_InsetLineTime[i]);
			_InsetLineDirect->push_back(tempvec3d);
			for(int j=0;j<_InsetLineTime[i];j++)
			{
				osg::Vec3d tempvec3dd(tempvec3d.x()*j,tempvec3d.y()*j,tempvec3d.z()*j);
				osg::Vec3d tempvec3ddd=m_tempLlVertices->at(i).operator+(tempvec3dd);
				m_tInsetLineLlVertices->push_back(tempvec3ddd);
			}
		}

		_InsetLineTime[m_VetNums-1]=get2PointInserttime(m_tempLlVertices->at(0),(m_tempLlVertices->at(m_VetNums-1)));
		osg::Vec3d m_tempvec3daa=(m_tempLlVertices->at(0).operator-((m_tempLlVertices->at(m_VetNums-1)))/_InsetLineTime[m_VetNums-1]);
		_InsetLineDirect->push_back(m_tempvec3daa);
		for(int i=0;i<_InsetLineTime[m_VetNums-1];i++)
		{
			osg::Vec3d tempvec3da(m_tempvec3daa.x()*i,m_tempvec3daa.y()*i,m_tempvec3daa.z()*i);
			osg::Vec3d tempvec3daa=m_tempLlVertices->at(m_VetNums-1).operator+(tempvec3da);
			m_tInsetLineLlVertices->push_back(tempvec3daa);
		}
		saveInsertLlVertices(m_tInsetLineLlVertices);
}

//内插值确定
void CModelF::saveInsertLlVertices(osg::ref_ptr<osg::Vec3dArray> m_tInsetLineLlVertices,osg::Vec3d direction)
{
	_InsetLineLlVertices->clear();

	//将原内插经纬度顶点保存
	_InsetLineLlVertices->insert( _InsetLineLlVertices->begin(), m_tInsetLineLlVertices->begin(),m_tInsetLineLlVertices->end() );


	osg::Vec3d offset = direction * _mfloorHeight*_mfloorNums;
	//将新生成经纬度顶点保存
	for (osg::Vec3dArray::iterator iter=m_tInsetLineLlVertices->begin();iter!=m_tInsetLineLlVertices->end();iter++)
	{		
		_InsetLineLlVertices->push_back( (*iter) + offset );
	}
}

int CModelF::get2PointInserttime(osg::Vec3d p1,osg::Vec3d p2)
{
	int m_inserttime;
	m_inserttime=sqrt((p1.x()-p2.x())*(p1.x()-p2.x())+(p1.y()-p2.y())*(p1.y()-p2.y())+(p1.z()-p2.z())*(p1.z()-p2.z()))/_InsertValue;
	if (m_inserttime==0)
	{
		m_inserttime=1;
	}
	return m_inserttime;
}

void CModelF::DrawCMFLines()
{
	//横线
	int modelVN=_modelLLVertices->size();
	for(int i=0;i<_mfloorNums+1;i++)
	{
		for (int j=0;j<modelVN;j++)
		{
			if (j!=modelVN-1)
			{
				_CMFLines=new CDrawLine(_newLlVertices->at(i*modelVN+j),_newLlVertices->at(i*modelVN+j+1),_mMapNode);
				_CMFLines->drawLine();
			} 
			else
			{
				_CMFLines=new CDrawLine(_newLlVertices->at(i*modelVN+j),_newLlVertices->at(i*modelVN+0),_mMapNode);
				_CMFLines->drawLine();
			}
			_LineATextroot->addChild(_CMFLines);
		}
	}
	//竖线
	int modelInsertNums=_InsetLineLlVertices->size()/2;
	for(int i=0;i<modelInsertNums;i++)
	{
		_CMFLines=new CDrawLine(_InsetLineLlVertices->at(i),_InsetLineLlVertices->at(i+modelInsertNums),_mMapNode);
		_CMFLines->drawLine();

		_LineATextroot->addChild(_CMFLines);
	}

}

void CModelF::DrawCMFText(const char* mstring)
{
	//横线
	int modelVN=_modelLLVertices->size();

	for(int k=0;k<modelVN;k++)
	{
		for(int i=0;i<_mfloorNums;i++)
		{
			for (int j=0;j<_InsetLineTime[k];j++)
			{
				osg::Vec3d tempPosition=_newLlVertices->at(i*modelVN+k).operator+(osg::Vec3d(_InsetLineDirect->at(k).x()*(j*2+1)/2,_InsetLineDirect->at(k).y()*(j*2+1)/2,_InsetLineDirect->at(k).z()*(j*2+1)/2).operator+(osg::Vec3d(0,0,_mfloorHeight/2)));
				_CMFText=new CCMFText(mstring,tempPosition,_mMapNode);
				_CMFText->DrawText();

				_LineATextroot->addChild(_CMFText);
			}
		}
	}
}

void CModelF::RemoveLinesAText()
{
	this->removeChild(_LineATextroot);
	_LineATextroot=new osg::Group;
	this->addChild(_LineATextroot);
}

//创建模型,绘制一个房屋
void CModelF::CreateFloorextrusion()
{
	if (!_modelLLVertices)
	{
		cout<<"CModel坐标没有!"<<endl;
		system("pause");
	}

	//原点数,几个点就是几边形
	unsigned int _verticesllNum = _modelLLVertices->size();

	x_min_max(_modelLLVertices,_Xmin,_Xmax);
	y_min_max(_modelLLVertices,_Ymin,_Ymax);

	_mHlength=_mfloorHeight*_mfloorNums;

	saveFloorNewLlVertices(_direction);

	InsertLinePoint(_direction);
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
		if (i>(_mfloorNums*_verticesllNum))
		{
			WDmodelWorldVertices->push_back(worldMapPosition);
			WDmodelllVertices->push_back(*iter);
		}
	}


//顶面
#if 1
	osg::ref_ptr<CModelFFloorGeometry> extrusionVer = new CModelFFloorGeometry(*_mukey*100000000,*_mukey*100000000,*_mukey);
	cout<<"屋顶："<<*_mukey*100000000<<endl;

	//定义颜色数组 
	osg::ref_ptr<osg::Vec4dArray> colorva=new osg::Vec4dArray;
	colorva->push_back( osg::Vec4( 1.f, 1.0f, 1.f, 1.f ) ); 
	extrusionVer->setColorArray( colorva.get(),osg::Array::BIND_OVERALL); 

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
	if (_getTexNums.size()!=0)
	{
		extrusionVer->setStateSet(createTexture2DState(_GeoImage.at(0/*_getTexNums.at(0)*/)));
		extrusionVer->_faceTexNumGeom=1;//_getTexNums.at(0);
	} 
	else
	{
		extrusionVer->setStateSet(createTexture2DState(_GeoImage.at(0)));
		extrusionVer->_faceTexNumGeom=0;
	}


	osgUtil::Tessellator tessellator;
	tessellator.setTessellationType(osgUtil::Tessellator::TESS_TYPE_POLYGONS);
	tessellator.setBoundaryOnly(false);
	tessellator.setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD );
	tessellator.retessellatePolygons( *extrusionVer );

	//自动生成法线,正确生成的前提是坐标是逆时针的。
	osgUtil::SmoothingVisitor::smooth( *extrusionVer );

	osg::ref_ptr<CModelFWallGeode> WD=new CModelFWallGeode(*_mukey*100000000,*_mukey,0);

	WD->addDrawable(extrusionVer);

	this->addChild(WD);//屋顶要在墙前

#endif

//侧面
	//侧面单独绘制
#if 1

	//顶点数据处理
	for ( unsigned int i=0; i<_verticesllNum-1; i++ )//各n-1个面,将某个面的顶点坐标传入
	{
		osg::ref_ptr<osg::Vec3Array> GeoVertices = new osg::Vec3Array;//某面的全部顶点

		for ( unsigned int j=0; j<=_mfloorNums; j++ )//侧面为矩形
		{
			GeoVertices->push_back(modelWorldVertices->at(i+j*_verticesllNum));
			GeoVertices->push_back(modelWorldVertices->at(i+1+j*_verticesllNum));
		}

		this->addChild(CreateFloorSideQuad(GeoVertices));
	}

	//最后一个面
	osg::ref_ptr<osg::Vec3Array> GeoVerticeslast = new osg::Vec3Array;
	for ( unsigned int j=0; j<=_mfloorNums; j++ )//侧面为矩形
	{
		GeoVerticeslast->push_back(modelWorldVertices->at(_verticesllNum-1+j*_verticesllNum));
		GeoVerticeslast->push_back(modelWorldVertices->at(0+j*_verticesllNum));
	}

	this->addChild(CreateFloorSideQuad(GeoVerticeslast));

#endif



}

#endif
//****！****//

//创建预显示模型
osg::ref_ptr<osg::Geode> CModelF::CreateTransparentModel(osg::ref_ptr<osg::Vec3dArray> newllVertices)
{
	if (!newllVertices)
	{
		cout<<"预显示坐标没有!"<<endl;
		system("pause");
	}
	  
	//几边形
	unsigned int _verticesllNum=newllVertices->size()/(this->_mfloorNums+1);


	//点经纬度坐标转换为世界坐标
	osg::ref_ptr<osg::Vec3Array> modelWorldVertices=new osg::Vec3Array;	
	  //底面点
	osg::Vec3dArray::iterator iter=newllVertices->begin();
	for (int i=0;i<_verticesllNum;i++)
	{
		//位置
		osgEarth::GeoPoint mapPos=osgEarth::GeoPoint(_mGeoSRS,(*iter).x(),(*iter).y(),(*iter).z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_mMapSRS);

		osg::Vec3d worldMapPosition;
		mapPos.toWorld(worldMapPosition);//经纬度转换为世界坐标

		modelWorldVertices->push_back(worldMapPosition);

		iter++;
	}
	  //顶点
	osg::Vec3dArray::iterator iter1=newllVertices->end();
	iter1-=_verticesllNum;
	for (int i=0;i<_verticesllNum;i++)
	{
		//位置
		osgEarth::GeoPoint mapPos=osgEarth::GeoPoint(_mGeoSRS,(*iter1).x(),(*iter1).y(),(*iter1).z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_mMapSRS);

		osg::Vec3d worldMapPosition;
		mapPos.toWorld(worldMapPosition);//经纬度转换为世界坐标

		modelWorldVertices->push_back(worldMapPosition);
		iter1++;
	}

	/*unsigned int _verticesllNum=newllVertices->size()/2;

	//点经纬度坐标转换为世界坐标
	osg::ref_ptr<osg::Vec3Array> modelWorldVertices=new osg::Vec3Array;	

	for (osg::Vec3dArray::iterator iter=newllVertices->begin();iter!=newllVertices->end();iter++)
	{

		//位置
		osgEarth::GeoPoint mapPos=osgEarth::GeoPoint(_mGeoSRS,(*iter).x(),(*iter).y(),(*iter).z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_mMapSRS);

		osg::Vec3d worldMapPosition;
		mapPos.toWorld(worldMapPosition);//经纬度转换为世界坐标

		modelWorldVertices->push_back(worldMapPosition);
	}*/

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
void CModelF::addExtrusionDragger(DragType chooseDragType) 
{ 
	//计算拖拽器方向
	osg::ref_ptr<osg::Vec3dArray> xyzcModelLlVertices=new osg::Vec3dArray;
	osg::ref_ptr<osg::Vec3dArray> xyzcModelWorldVertices=new osg::Vec3dArray;
	computeDragdir(this,xyzcModelWorldVertices,xyzcModelLlVertices);

	if (chooseDragType==ExtScaleAxis)
	{
		//拉伸拖拽器位置
		osg::Vec3d dragLlPosition(xyzcModelLlVertices->at(0).x(),xyzcModelLlVertices->at(0).y(),this->_newLlVertices->at(0).z());//中心点经纬度坐标

		GeoPoint mapPos=osgEarth::GeoPoint(_mGeoSRS,dragLlPosition.x(),dragLlPosition.y(),dragLlPosition.z(),osgEarth::AltitudeMode::ALTMODE_ABSOLUTE).transform(_mMapSRS);
		osg::Vec3d dragWorldPosition;
		mapPos.toWorld(dragWorldPosition);//经纬度转换为世界坐标,//中心点世界坐标

		_Dragger=new  ExtScaleAxisCMDragger(xyzcModelWorldVertices);

		_Dragger->addTransformUpdating(this/*temptMT*/); //参数为 matrixtransform类型
		
		_mHlength=_mfloorHeight*_mfloorNums;
		_Dragger->setMatrix(osg::Matrix::scale(_mHlength,_mHlength, _mHlength) * osg::Matrix::translate(dragWorldPosition));
	}



	_Dragger->setupDefaultGeometry();
	_mAllCMfroot->addChild(_Dragger); //直接将拖拽器添加在场景
	_Dragger->setHandleEvents(true); 
	_Dragger->setActivationModKeyMask(osgGA::GUIEventAdapter::MODKEY_CTRL);

}

void CModelF::DisableDragger() 
{ 

	if (_Dragger!=NULL)
	{
		_mAllCMfroot->removeChild(_Dragger);
		_Dragger->removeTransformUpdating(this); 
		_Dragger->setHandleEvents(false); 	
	}

}

void CModelF::computeDragdir(osg::ref_ptr<osg::MatrixTransform> m_sceneMatTransf,osg::ref_ptr<osg::Vec3dArray> xyzcModelWorldVertices,osg::ref_ptr<osg::Vec3dArray> xyzcModelLlVertices)
{
	//1.中心点世界转经纬
	GeoPoint convertPoint;
	convertPoint.fromWorld(_mGeoSRS,m_sceneMatTransf->getBound().center());

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

void CModelF::setFloorHeight()
{

}
float CModelF::getFloorHeight()
{
	return _mfloorHeight;
}
void CModelF::setFloorNums()
{

}
int CModelF::getFloorNums()
{
	return _mfloorNums;
}



