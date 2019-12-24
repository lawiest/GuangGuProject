/*
WorkSpaceWritter的子类中定义各种数据结构，这些结构对应的是文件组织方式。由于不同格式文件有自己的组织方式，所以不同的子类定义各自不同的数据结构。
WorkSpaceReader作为抽象基类，存放通用的数据结构。这些结构对应的是内存中数据的组织方式。不同格式的文件内容将会解析为同样的数据格式，所以这些数据结构定义在抽象基类中。
*/

#pragma once

#include <list>
#include <vector>
#include <map>
#include <string>

#include "osgEarth\XmlUtils"
#include "osgEarth\config"

/*
工作空间文件读取
*/
class WorkSpaceReader
{
public:
	WorkSpaceReader(const char *FilePath);
	virtual ~WorkSpaceReader(void);

	void ReadTerrainFileList(std::list<std::string> &listFiles, int nScreenIndex);
	void ReadImageFileList(std::list<std::string> &listFiles, int nScreenIndex);
	void ReadModelFileList(std::list<std::string> &listFiles, int nScreenIndex);
	void ReadVectorFileList(std::list<std::string> &listFiles, int nScreenIndex);
	//倾斜
	void ReadObliqueFileList(std::list<std::string> &listFiles, int nScreenIndex);
	//管线
	void ReadPipeFileList(std::list<std::string> &listFiles, int nScreenIndex);
	//地名
	void ReadPlaceNameFileList(std::list<std::string> &listFiles, int nScreenIndex);

	int ScreenNum();

	std::string ReadValue(std::string strKey);

protected:

	std::map<std::string, std::string> m_mapKeyValue;
	std::map<int, std::list<std::string>> m_listTerrainFileList;
	std::map<int, std::list<std::string>> m_listImageFileList;
	std::map<int, std::list<std::string>> m_listModelFileList;
	std::map<int, std::list<std::string>> m_listVectorFileList;
	std::map<int, std::list<std::string>> m_listObliqueFileList;
	std::map<int, std::list<std::string>> m_listPipeFileList;
	std::map<int, std::list<std::string>> m_listPlaceNameFileList;
	std::map<std::string, std::string>    m_makKV;
	int m_nScreenNum;
};

/*
xml格式工作空间文件读取
*/
class XmlWSReader : public WorkSpaceReader
{
public:
	XmlWSReader(const char *FilePath);
	virtual ~XmlWSReader(void);
protected:
	virtual void Parse(const char *FilePath);
};


/*
工作空间文件写入
*/
class WorkSpaceWritter
{
public:
	WorkSpaceWritter();
	virtual ~WorkSpaceWritter(void);

	virtual void WriteTerrainFile(std::string strFile, int nScreenIndex) = 0;
	virtual void WriteImageFile(std::string strFile, int nScreenIndex) = 0;
	virtual void WriteModelFile(std::string strFile, int nScreenIndex) = 0;
	virtual void WriteVectorFile(std::string strFile, int nScreenIndex) = 0;
	virtual void WriteObliqueFile(std::string strFile, int nScreenIndex) = 0;
	virtual void WritePipeFile(std::string strFile, int nScreenIndex) = 0;
	virtual void WritePlaceNameFile(std::string strFile, int nScreenIndex) = 0;

	virtual void WriteValue(std::string strKey, std::string strValue) = 0;
	virtual void Save(const char *FilePath) = 0;
	virtual void Clear() = 0;

	virtual void ScerrnNum(int nScerrnNum);
	std::string FilePath(){return m_strPath;}

protected:
	int m_nScreenNum;
	std::string m_strPath;
};

/*
xml格式工作空间文件写入
*/
class XmlWSWritter : public WorkSpaceWritter
{
public:
	XmlWSWritter();
	virtual ~XmlWSWritter(void);

	virtual void WriteTerrainFile(std::string strFile, int nScreenIndex);
	virtual void WriteImageFile(std::string strFile, int nScreenIndex);
	virtual void WriteModelFile(std::string strFile, int nScreenIndex);
	virtual void WriteVectorFile(std::string strFile, int nScreenIndex);
	virtual void WriteObliqueFile(std::string strFile, int nScreenIndex);
	virtual void WritePipeFile(std::string strFile, int nScreenIndex);
	virtual void WritePlaceNameFile(std::string strFile, int nScreenIndex);

	virtual void WriteValue(std::string strKey, std::string strValue);
	virtual void ScerrnNum(int nScerrnNum);
	virtual void Save(const char *FilePath);
	virtual void Clear();
protected:
	osgEarth::XmlDocument m_xmlDoc;
	std::map<int, std::string> m_listTerrainFileList;
	std::map<int, std::string> m_listImageFileList;
	std::map<int, std::string> m_listModelFileList;
	std::map<int, std::string> m_listVectorFileList;
	std::map<int, std::string> m_listObliqueFileList;
	std::map<int, std::string> m_listPipeFileList;
	std::map<int, std::string> m_listPlaceNameFileList;

	std::map<std::string, std::string> m_makKV;
};