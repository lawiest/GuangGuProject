#ifndef GEOGLOBE_MODEL_MODELLAYER
#define GEOGLOBE_MODEL_MODELLAYER 1
#include <osg/PagedLOD>
#include <Model/ModelOptions.h>
#include <osg/LOD>
#include <Model/ModelDriver.h>
#include <Model/QuadModelTileDriver.h>
#include<osg/Vec3d>
//#include <android/log.h>
using namespace osg;

GG_MODEL_NS

/*一个ModelLayer对应一个模型数据集*/
class ModelLayer: public osg::LOD
{
public:
	/*构造参数，包含了数据集名称，纹理几何数据库等信息*/
	osg::ref_ptr<ModelLayerOptions> _options;
	/*设置数据集是否可见*/
	bool                            _visble;
	/*设置是否重置数据集的数据*/
	bool                            _reset;
	/*数据集的中心点经纬度*/
	osg::Vec3d                      _centerGeo;
	
	/*构造函数，实现数据集的顶层瓦片的生成*/
	ModelLayer(ModelLayerOptions *modelLayerOptions)
	{
		_visble=true;
		_reset = false;
		_options=modelLayerOptions;

		//根据数据集范围确定顶层瓦片
		std::vector< TileKey > keys;
	 osgEarth::GeoExtent ext=GeoExtent(_options->_mapNode->getMapSRS(),_options->_west,_options->_south,_options->_east,_options->_north);
		int firstLevel=_options->_minLevel-3;
		firstLevel=firstLevel>0?firstLevel:0;
		_options->_mapNode->getMap()->getProfile()->getIntersectingTiles(ext,firstLevel,keys);

		//生成顶层瓦片
		for( unsigned i=0; i<keys.size(); ++i )
		{
			std::string quadModelTileName=Stringify() <<"Model"<<"."<< keys[i].getLevelOfDetail() << "." << keys[i].getTileX() <<"." <<keys[i].getTileY()<<".quadmodeltile";
			osg::ref_ptr<osg::Node> modelLayer = osgDB::readNodeFile(quadModelTileName,modelLayerOptions);
			//this->setFileName(i, quadModelTileName );
			this->addChild(modelLayer,0,FLT_MAX);
		}

		osg::Vec3d center;
		osgEarth::GeoPoint gp=ext.getBoundingGeoCircle().getCenter();
		_centerGeo=osg::Vec3d(gp.x(),gp.y(),gp.z());
		gp.toWorld(center);

		//ext.getBoundingGeoCircle()->getCenter()
		//设置数据集的中心
		this->setCenter(center);
		//设置数据集的包围圆半径
		this->setRadius(ext.getBoundingGeoCircle().getRadius());


		//设置模型选取时的高亮颜色，这里默认为红色
		this->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
		osg::Material* m = new osg::Material();
		m->setAmbient(m->FRONT_AND_BACK, osg::Vec4(1,0,0,1));
		m->setDiffuse(m->FRONT_AND_BACK, osg::Vec4(1,0,0,1));
		m->setSpecular(m->FRONT_AND_BACK, osg::Vec4(1,0,0,1));
		//	m->setEmission(m->FRONT_AND_BACK, osg::Vec4(1,0,0,1));
		m->setShininess(m->FRONT_AND_BACK, 32.0);
		this->getOrCreateStateSet()->setAttributeAndModes(m, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
		this->getOrCreateStateSet()->setAttributeAndModes(new osg::Program,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
	}
	
	/*继承自osg::Node,每次场景遍历时都会运行该函数，在里面可以实现可视性检测等功能*/
	virtual void traverse(NodeVisitor& nv)
	{
		//是否显示
		if (!_visble)
			return;

		//重置数据集
		if (_reset)
		{
			if(_children.size() == 0)
				return;
		
			removeChildren(0, _children.size());
			std::vector< TileKey > keys;
		 osgEarth::GeoExtent ext=GeoExtent(_options->_mapNode->getMapSRS(),_options->_west,_options->_south,_options->_east,_options->_north);
			int firstLevel=_options->_minLevel-3;
			firstLevel=firstLevel>0?firstLevel:0;
			_options->_mapNode->getMap()->getProfile()->getIntersectingTiles(ext,firstLevel,keys);

			for( unsigned i=0; i<keys.size(); ++i )
			{
				std::string quadModelTileName=Stringify() <<"Model"<<"."<< keys[i].getLevelOfDetail() << "." << keys[i].getTileX() <<"." <<keys[i].getTileY()<<".quadmodeltile";
				osg::ref_ptr<osg::Node> modelLayer = osgDB::readNodeFile(quadModelTileName,_options);
				this->addChild(modelLayer,0,FLT_MAX);
			}

			_reset = false;
		
			return;
		}
	
		osg::LOD::traverse(nv);
		////根据距离判断数据集是否可见，这部分代码模仿osg::LOD::traverse写的
		//switch(nv.getTraversalMode())
		//{
		//case(NodeVisitor::TRAVERSE_ALL_CHILDREN):
		//	std::for_each(_children.begin(),_children.end(),NodeAcceptOp(nv));
		//	break;
		//case(NodeVisitor::TRAVERSE_ACTIVE_CHILDREN):
		//	{
		//		float required_range = 0;
		//		//判断根据距离还是像素大小确定模型是否可视，默认是根据距离
		//		if (_rangeMode==DISTANCE_FROM_EYE_POINT)
		//			//确定视点范围
		//			required_range = nv.getDistanceToViewPoint(getCenter(),true);
		//		else
		//		{
		//			osg::CullStack* cullStack = dynamic_cast<osg::CullStack*>(&nv);
		//			if (cullStack && cullStack->getLODScale())
		//			{
		//				required_range = cullStack->clampedPixelSize(getBound()) / cullStack->getLODScale();
		//			}
		//			else
		//			{
		//				// fallback to selecting the highest res tile by
		//				// finding out the max range
		//				for(unsigned int i=0;i<_rangeList.size();++i)
		//				{
		//					required_range = osg::maximum(required_range,_rangeList[i].first);
		//				}
		//			}
		//		}

		//		unsigned int numChildren = _children.size();
		//		if (_rangeList.size()<numChildren) numChildren=_rangeList.size();

		//		for(unsigned int i=0;i<numChildren;++i)
		//		{
		//			if (_rangeList[i].first<=required_range && required_range<_rangeList[i].second)
		//			{
		//				//满足视点范围则继续遍历
		//				_children[i]->accept(nv);
		//			}
		//		}
		//		break;
		//	}
		//default:
		//	break;
		//}
	}
	
};

GG_MODEL_ENDNS

#endif //GEOGLOBE_MODEL_MODELLAYER
