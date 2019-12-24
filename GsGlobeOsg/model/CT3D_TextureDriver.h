#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/Registry>
#include <Tool/SQLiteOP.h>
#include <Model/Buffer.h>
#include <Model/Material.h>

#ifndef CT3D_TEXTUREDRIVER
#define CT3D_TEXTUREDRIVER 1
namespace Model{
class CT3D_TextureDriver : public osgDB::ReaderWriter
{
public:
	CT3D_TextureDriver() { supportsExtension("ct3d_tex","user defined format");}

	virtual const char* className()
	{
		return "User Defined Texture Engine";
	}


	void parseFileName(std::string uri,unsigned long long& texKey) const
	{
		std::vector<std::string> vecStr;

		std::string lessExtension;
		std::string ext=osgDB::getFileExtension(uri);
		//vecStr->push_back(ext);
		lessExtension=osgDB::getNameLessExtension(uri);

		std::string temp=osgDB::getFileExtension(lessExtension);
		std::stringstream tempss;
		tempss<<temp;
		tempss>>texKey;
		//for (int i=0;i<7;i++)
		//{
		//	ext=osgDB::getFileExtension(lessExtension);
		//	vecStr.push_back(ext);
		//	lessExtension=osgDB::getNameLessExtension(lessExtension);
		//}


		//elevation= atof(std::string(vecStr[1]+"."+vecStr[0]).c_str());
		//lat= atof(std::string(vecStr[3]+"."+vecStr[2]).c_str());
		//lon= atof(std::string(vecStr[5]+"."+vecStr[4]).c_str());

		//std::stringstream tempss;
		//tempss<<vecStr[6].c_str();
		//;
		//tempss>>geoKey;



	}


	virtual ReadResult readImage(const std::string& uri, const Options* options) const
	{
		std::string ext = osgDB::getFileExtension(uri);
		


		if ( acceptsExtension(ext) )
		{
			unsigned long long texKey;
			parseFileName(uri,texKey);
			std::string DBPath=osgDB::getNameLessAllExtensions(uri);
			std::string texDBPath=DBPath+".tex";
			CSQLiteOP SQLiteTexOP;
			SQLiteTexOP.OpenDataBase(texDBPath.c_str());
			
			Buffer texBuffer;
			if(!SQLiteTexOP.GetTextureByTexKey(texKey,texBuffer))
				return NULL;

			CMaterial pMaterial;

			if (!pMaterial.readBuffer(texBuffer)||0!= pMaterial.getMaterialType())
				return NULL;

			std::string string;
			string.assign((char*)texBuffer.getCurPtr(),texBuffer.size() - texBuffer.getPos() + 1);
			std::stringstream stream;
			stream<<string;

			std::string ext=osgDB::getFileExtension(pMaterial.getMaterialName());
			osg::ref_ptr<osgDB::ReaderWriter> rw = osgDB::Registry::instance()->getReaderWriterForExtension( ext );
			osg::ref_ptr<osg::Image> image = rw->readImage(stream).getImage();
			//osg::ref_ptr<osg::Image> image=osgDB::readImageFile("C:/Users/Administrator/Desktop/test.jpg");
			//SQLiteTexOP.CloseDataBase();
			if (image.valid())
			{
				return ReadResult( image.get(), ReadResult::FILE_LOADED );
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

REGISTER_OSGPLUGIN(ct3d_tex, CT3D_TextureDriver);
}
#endif // OSGEARTH_ENGINE_MP_OPTIONS