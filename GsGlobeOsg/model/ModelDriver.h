#ifndef GEOGLOBE_MODEL_MODELDRIVER
#define GEOGLOBE_MODEL_MODELDRIVER 1

#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <Model/Model.h>
#include <Model/ModelOptions.h>
#include <Model/LodModel.h>
using namespace osg;
namespace GeoGlobe
{
	namespace Model
	{

		//伪插件，通过伪插件才能将模型以DataBasePager(线程)的方式导入
		class ModelDriver : public osgDB::ReaderWriter
		{
		public:
			ModelDriver()
			{
				supportsExtension("model","model defined format");
			}

			virtual const char* className() const{return "model reader";}

			//核心代码，构建model节点
			virtual ReadResult readNode(const std::string& uri,const osgDB::ReaderWriter::Options* options)const
			{
				std::string ext = osgDB::getFileExtension(uri);

				//判断是否调用该插件
				if (acceptsExtension(ext) )
				{
                    //根据geoKey构建model
					unsigned long long geoKey;
					int lod=-1;
					//parseFileName(uri,geoKey,lod);
					LodModelOptions* lodModelOptions=(LodModelOptions*) options;
					lodModelOptions->parseModelName(uri,geoKey,lod);
					osg::ref_ptr<ModelOptions> modelOptions=new ModelOptions(lodModelOptions,geoKey,lod);
					osg::ref_ptr<Model> node=new Model(modelOptions);

					if (node.valid())
					{
						//return ReadResult( node.get(), ReadResult::FILE_LOADED );
						return ReadResult( node.release(), ReadResult::FILE_LOADED );
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
		
		REGISTER_OSGPLUGIN(model,ModelDriver);

		class LodModelDriver : public osgDB::ReaderWriter
		{
		public:
			LodModelDriver()
			{
				supportsExtension("lodmodel","lodmodel defined format");
			}

			virtual const char* className() const{return "model reader";}

			void parseFileName(std::string uri,unsigned long long& featureKey) const
			{
				std::vector<std::string> vecStr;

				std::string lessExtension;
				//std::string temKey;
				std::string lod;
				//std::string ext=osgDB::getFileExtension(uri);
				//vecStr->push_back(ext);
				lessExtension=osgDB::getNameLessExtension(uri);
				//ext=osgDB::getFileExtension(lessExtension);

				/*for (int i=0;i<2;i++)
				{*/

				std::string ext=osgDB::getFileExtension(lessExtension);
				/*vecStr.push_back(ext);
					lessExtension=osgDB::getNameLessExtension(lessExtension);
				}*/

				//lod= atoi(vecStr[0].c_str());

				std::stringstream tempss;
				tempss<<ext.c_str();
				tempss>>featureKey;
			}

			virtual ReadResult readNode(const std::string& uri,const osgDB::ReaderWriter::Options* options)const
			{
				std::string ext = osgDB::getFileExtension(uri);

				if ( acceptsExtension(ext) )
				{
					//__android_log_print(ANDROID_LOG_INFO, "modelDriver::readNode", "fly=%d", 0);
					//unsigned long long featureKey;
					////int lod=-1;
					//parseFileName(uri,featureKey);
					//LodModelOptions* lodModelOptions=
					LodModelOptions* lodModelOptions=(LodModelOptions*)options;
						osg::ref_ptr<LodModel> node=new LodModel(lodModelOptions);
					if (node.valid())
					{
						//return ReadResult( node.get(), ReadResult::FILE_LOADED );
						return ReadResult( node.release(), ReadResult::FILE_LOADED );
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

		REGISTER_OSGPLUGIN(lodmodel,LodModelDriver);

	}
}


#endif
