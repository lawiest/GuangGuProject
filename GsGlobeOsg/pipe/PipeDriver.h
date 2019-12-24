
#ifndef GEOGLOBE_PIPE_PIPEMODELDRIVER
#define GEOGLOBE_PIPE_PIPEMODELDRIVER 

#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/Registry>
//#include <Model/Model.h>
#include <Model/ModelOptions.h>
#include <Pipe/PipeOptions.h>
#include <osg/Shape>
#include <Pipe/PipePointModel.h>
#include <Pipe/PipeLineModel.h>
//#include <Pipe/PipeLineBuilder.h>

using namespace osg;
using namespace GeoGlobe::Model;
using namespace GeoGlobe::Pipe;
namespace GeoGlobe
{
	namespace Pipe
	{
//伪插件，通过伪插件才能将管线模型以DataBasePager(线程)的方式导入
		class PipeModelDriver : public osgDB::ReaderWriter
		{
		public:
			PipeModelDriver()
			{
				supportsExtension("pipemodel","pipe defined format");
			}

			virtual const char* className() const{return "model reader";}
//管线模型的入口函数
			virtual ReadResult readNode(const std::string& uri,const osgDB::ReaderWriter::Options* options)const
			{
				std::string ext = osgDB::getFileExtension(uri);

				//判断是否调用该插件
				if ( acceptsExtension(ext) )
				{

					unsigned long long geoKey;
					int lod=-1;
					PipeModelOptions* pipeModelOptions=(PipeModelOptions*) options;
					pipeModelOptions->parsePipeModelName(uri,geoKey);

					
					osg::ref_ptr<osg::Node> node;
					if (pipeModelOptions->_pipeFeatureType==0)
					{
						//管点模型
						PipePointModelOptions* pipePointModelOptions=(PipePointModelOptions*) options;
						osg::ref_ptr<PipePointModel> pipePointModel=new PipePointModel(pipePointModelOptions);
						osg::Group* group=new osg::Group;
						stringstream ss;
						ss<<pipePointModelOptions->_featureID;
						//group->addChild(new PlaceNode(pipePointModelOptions->_mapNode,GeoPoint(pipePointModelOptions->_mapNode->getMapSRS(),pipePointModelOptions->_modelGeo.x(),pipePointModelOptions->_modelGeo.y(),pipePointModelOptions->_modelGeo.z()),ss.str()));
						//group->addChild(pipePointModel);
						node=/*group*/pipePointModel;

					}
					 else if (pipeModelOptions->_pipeFeatureType==1)
					{
						 //管段模型
						PipeLineModelOptions* pipeLineModelOptions=(PipeLineModelOptions*) options;
						osg::ref_ptr<PipeLineModel> pipeLineModel=new PipeLineModel(pipeLineModelOptions);
				        node=pipeLineModel;
					}

					if (node.valid())
					{
						return ReadResult( node.get(), ReadResult::FILE_LOADED );
					}
					else
					{
						return ReadResult::FILE_NOT_FOUND;
					}
				}
				else
				{
					return ReadResult::FILE_NOT_HANDLED;
				}
			}
		};


		REGISTER_OSGPLUGIN(pipemodel,PipeModelDriver);
	}
}

#endif
