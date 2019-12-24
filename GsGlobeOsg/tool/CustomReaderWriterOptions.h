
#ifndef CUSTOM_READER_WRITER_OPTIONS
#define CUSTOM_READER_WRITER_OPTIONS 1
#include <osgEarth/MapNode>

class CustomReaderWriterOptions : public osgDB::ReaderWriter::Options
{
public:
	CustomReaderWriterOptions()
	{
	}
	~CustomReaderWriterOptions()
	{
	//	sqlite3_close(_db);
	}
	void setSqlitePath(std::string dbpath)
	{
	//	sqlite3_open(dbpath.c_str(),&_db);
	}
	//sqlite3* getSqlite()
	//{
	//	return _db;
	//}
	void setMapNode(osgEarth::MapNode* mapNode)
	{
		_mapNode=mapNode;
	}

	//void setDBName(std::string dbName)
	//{
	//	_dbName=dbName;
	//}

	//std::string getDBName()
	//{
	//	return _dbName;
	//}

	void setDBName(std::string dataSetFileName,std::string modelGeometrysFiles)
	{
		 _dataSetFileName=dataSetFileName;
		 _modelGeometrysFiles=modelGeometrysFiles;
	}	

	void getDBName(std::string& dataSetFileName,std::string& modelGeometrysFiles)
	{
		dataSetFileName=_dataSetFileName;
		modelGeometrysFiles=_modelGeometrysFiles;
	}


	virtual osg::Object* clone(const osg::CopyOp& copyop) const 
	{
		return (osg::Object*)(this);
	}
	osgEarth::MapNode* getMapNode()
	{
		return _mapNode;
	}

	osg::ref_ptr<osgEarth::MapNode>  _mapNode;
	std::string                      _dataSetFileName;
	std::string                      _modelGeometrysFiles;
//	sqlite3* _db;
};
#endif