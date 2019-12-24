#ifndef GEOGLOBE_PIPE_PIPELAYER
#define GEOGLOBE_PIPE_PIPELAYER 1
#include <osg/PagedLOD>
#include <Pipe/PipeOptions.h>
#include <osg/LOD>
#include <Pipe/PipeDriver.h>
#include <Pipe/QuadPipeTileDriver.h>

using namespace osg;
namespace GeoGlobe
{
	namespace Pipe
	{
	//三维管线最顶层的节点负责管理单个瓦片数据集
		class PipeLayer: public osg::LOD
		{
		public:
			osg::ref_ptr<PipeLayerOptions> _options;

			//是否可见
			bool                            _visble;
			//数据集的中心位置
			osg::Vec3d                      _centerGeo;

			                         
			PipeLayer(PipeLayerOptions *pipeLayerOptions)
			{
				_visble=true;
				_options=pipeLayerOptions;


				std::vector< TileKey > keys;
			 osgEarth::GeoExtent ext=GeoExtent(_options->_mapNode->getMapSRS(),_options->_west,_options->_south,_options->_east,_options->_north);
				//__android_log_print(ANDROID_LOG_ERROR, "PIPELAYER", "%f,%f,%f,%f",_options->_west,_options->_south,_options->_east,_options->_north);
				int firstLevel=_options->_minLevel-3;
				firstLevel=firstLevel>0?firstLevel:0;
				_options->_mapNode->getMap()->getProfile()->getIntersectingTiles(ext,firstLevel,keys);

				//根据数据集范围构建顶层瓦片
				for( unsigned i=0; i<keys.size(); ++i )
				{
					std::string quadModelTileName=Stringify() <<"Pipe"<<"."<< keys[i].getLevelOfDetail() << "." << keys[i].getTileX() <<"." <<keys[i].getTileY()<<".quadpipetile";
					osg::ref_ptr<osg::Node> modelLayer = osgDB::readNodeFile(quadModelTileName,pipeLayerOptions);
					//this->setFileName(i, quadModelTileName );
					this->addChild(modelLayer,0,FLT_MAX);


				}

				osg::Vec3d centerCart;
				osgEarth::GeoPoint gp=ext.getBoundingGeoCircle().getCenter();
				gp.toWorld(centerCart);
				_centerGeo=gp.vec3d();
				//ext.getBoundingGeoCircle()->getCenter()
				this->setCenter(centerCart);
				this->setRadius(ext.getBoundingGeoCircle().getRadius());
				this->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
				this->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
				this->getOrCreateStateSet()->setAttributeAndModes(new osg::Program,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
			}

			//每次场景遍历时都会走这部分
			void traverse(NodeVisitor& nv)
			{
				_options->_memCache->pruneIfNecessary();
				if (!_visble)
				return;
				
				//只有当视点离数据集中心较近时才会开始运作
				switch(nv.getTraversalMode())
				{
				case(NodeVisitor::TRAVERSE_ALL_CHILDREN):
					std::for_each(_children.begin(),_children.end(),NodeAcceptOp(nv));
					break;
				case(NodeVisitor::TRAVERSE_ACTIVE_CHILDREN):
					{
						float required_range = 0;
						if (_rangeMode==DISTANCE_FROM_EYE_POINT)
						{
							required_range = nv.getDistanceToViewPoint(getCenter(),true);
						}
						else
						{
							osg::CullStack* cullStack = dynamic_cast<osg::CullStack*>(&nv);
							if (cullStack && cullStack->getLODScale())
							{
								required_range = cullStack->clampedPixelSize(getBound()) / cullStack->getLODScale();
							}
							else
							{
								// fallback to selecting the highest res tile by
								// finding out the max range
								for(unsigned int i=0;i<_rangeList.size();++i)
								{
									required_range = osg::maximum(required_range,_rangeList[i].first);
								}
							}
						}

						unsigned int numChildren = _children.size();
						if (_rangeList.size()<numChildren) numChildren=_rangeList.size();

						for(unsigned int i=0;i<numChildren;++i)
						{
							if (_rangeList[i].first<=required_range && required_range<_rangeList[i].second)
							{
								_children[i]->accept(nv);
							}
						}
						break;
					}
				default:
					break;
				}
			}
		};

	}
}

#endif
