#include "workspaceparser.h"

#include <fstream>
#include <iostream>
using namespace osgEarth;



#pragma region WorkSpaceReader
WorkSpaceReader::WorkSpaceReader(const char *FilePath)
{
	
}
WorkSpaceReader::~WorkSpaceReader(void)
{
}
int WorkSpaceReader::ScreenNum()
{
	return m_nScreenNum;
}
void WorkSpaceReader::ReadTerrainFileList(std::list<std::string> &listFiles, int nScreenIndex)
{
	std::map<int, std::list<std::string>>::const_iterator it =  m_listTerrainFileList.find(nScreenIndex);
	if (it != m_listTerrainFileList.end())
	{
		listFiles.clear();
		listFiles.resize(it->second.size());
		std::copy(it->second.begin(), it->second.end(), listFiles.begin());
	}
}
void WorkSpaceReader::ReadImageFileList(std::list<std::string> &listFiles, int nScreenIndex)
{
	std::map<int, std::list<std::string>>::const_iterator it =  m_listImageFileList.find(nScreenIndex);
	if (it != m_listImageFileList.end())
	{
		listFiles.clear();
		listFiles.resize(it->second.size());
		std::copy(it->second.begin(), it->second.end(), listFiles.begin());
	}
}
void WorkSpaceReader::ReadModelFileList(std::list<std::string> &listFiles, int nScreenIndex)
{
	std::map<int, std::list<std::string>>::const_iterator it =  m_listModelFileList.find(nScreenIndex);
	if (it != m_listModelFileList.end())
	{
		listFiles.clear();
		listFiles.resize(it->second.size());
		std::copy(it->second.begin(), it->second.end(), listFiles.begin());
	}
}
void WorkSpaceReader::ReadVectorFileList(std::list<std::string> &listFiles, int nScreenIndex)
{
	std::map<int, std::list<std::string>>::const_iterator it =  m_listVectorFileList.find(nScreenIndex);
	if (it != m_listVectorFileList.end())
	{
		listFiles.clear();
		listFiles.resize(it->second.size());
		std::copy(it->second.begin(), it->second.end(), listFiles.begin());
	}
}
//倾斜
void WorkSpaceReader::ReadObliqueFileList(std::list<std::string> &listFiles, int nScreenIndex)
{
	std::map<int, std::list<std::string>>::const_iterator it =  m_listObliqueFileList.find(nScreenIndex);
	if (it != m_listObliqueFileList.end())
	{
		listFiles.clear();
		listFiles.resize(it->second.size());
		std::copy(it->second.begin(), it->second.end(), listFiles.begin());
	}
}
//管线
void WorkSpaceReader::ReadPipeFileList(std::list<std::string> &listFiles, int nScreenIndex)
{
	std::map<int, std::list<std::string>>::const_iterator it =  m_listPipeFileList.find(nScreenIndex);
	if (it != m_listPipeFileList.end())
	{
		listFiles.clear();
		listFiles.resize(it->second.size());
		std::copy(it->second.begin(), it->second.end(), listFiles.begin());
	}
}
//地名
void WorkSpaceReader::ReadPlaceNameFileList(std::list<std::string> &listFiles, int nScreenIndex)
{
	std::map<int, std::list<std::string>>::const_iterator it =  m_listPlaceNameFileList.find(nScreenIndex);
	if (it != m_listPlaceNameFileList.end())
	{
		listFiles.clear();
		listFiles.resize(it->second.size());
		std::copy(it->second.begin(), it->second.end(), listFiles.begin());
	}
}
std::string WorkSpaceReader::ReadValue(std::string strKey)
{
	std::map<std::string, std::string>::const_iterator it = m_makKV.find(strKey);
	if (it != m_makKV.end())
		return it->second;
	return "";
}
#pragma endregion

#pragma region WorkSpaceWritter
WorkSpaceWritter::WorkSpaceWritter()
{
	m_nScreenNum = 1;
}
WorkSpaceWritter::~WorkSpaceWritter()
{
}
void WorkSpaceWritter::ScerrnNum(int nScerrnNum)
{
	m_nScreenNum = nScerrnNum;
}
#pragma endregion

#pragma region XmlWSReader
XmlWSReader::XmlWSReader(const char *FilePath):WorkSpaceReader(FilePath)
{
	Parse(FilePath);
}
XmlWSReader::~XmlWSReader()
{
}
void XmlWSReader::Parse(const char *FilePath)
{
	const std::string sPath(FilePath);
	osgEarth::XmlDocument *pxmlDoc = osgEarth::XmlDocument::load(sPath, 0L);
	if (!pxmlDoc)
		return;

	osgEarth::XmlElement *pEleDoc = pxmlDoc->getSubElement("Document");
	osgEarth::XmlElement *pEle = pEleDoc->getSubElement("screennum");
	if (!pEle)
		return;
	
	//srceen num
	m_nScreenNum = atoi(pEle->getText().c_str());
	if (m_nScreenNum <= 0)
		return;

	//data source
	for (int i=0; i<m_nScreenNum; i++)
	{
		std::string sKey = Stringify()<<"screen_"<<i;
		XmlElement *pEleScreen = pEleDoc->getSubElement(sKey);
		if (!pEleScreen)
			continue;

		XmlElement *pEleTerrain = pEleScreen->getSubElement("Terrain");
		if (pEleTerrain)
		{
			std::stringstream ss(pEleTerrain->getText());
			std::string sTerrainFile;
			std::list<std::string> listFiles;
			while (std::getline(ss, sTerrainFile, '@'))
			{
				listFiles.push_back(sTerrainFile);
			}
			m_listTerrainFileList.insert(std::make_pair(i, listFiles));
		}

		XmlElement *pEleImage = pEleScreen->getSubElement("Image");
		if (pEleImage)
		{
			std::stringstream ss(pEleImage->getText());
			std::string sImageFile;
			std::list<std::string> listFiles;
			while (std::getline(ss, sImageFile, '@'))
			{
				listFiles.push_back(sImageFile);
			}
			m_listImageFileList.insert(std::make_pair(i, listFiles));
		}

		XmlElement *pEleModel = pEleScreen->getSubElement("Model");
		if (pEleModel)
		{
			std::stringstream ss(pEleModel->getText());
			std::string sModelFile;
			std::list<std::string> listFiles;
			while (std::getline(ss, sModelFile, '@'))
			{
				listFiles.push_back(sModelFile);
			}
			m_listModelFileList.insert(std::make_pair(i, listFiles));
		}

		XmlElement *pEleVector = pEleScreen->getSubElement("Vector");
		if (pEleVector)
		{
			std::stringstream ss(pEleVector->getText());
			std::string sVectorFile;
			std::list<std::string> listFiles;
			while (std::getline(ss, sVectorFile, '@'))
			{
				listFiles.push_back(sVectorFile);
			}
			m_listVectorFileList.insert(std::make_pair(i, listFiles));
		}

		XmlElement *pEleOblique = pEleScreen->getSubElement("Oblique");
		if (pEleOblique)
		{
			std::stringstream ss(pEleOblique->getText());
			std::string sObliqueFile;
			std::list<std::string> listFiles;
			while (std::getline(ss, sObliqueFile, '@'))
			{
				listFiles.push_back(sObliqueFile);
			}
			m_listObliqueFileList.insert(std::make_pair(i, listFiles));
		}

		XmlElement *pElepPipe = pEleScreen->getSubElement("Pipe");
		if (pElepPipe)
		{
			std::stringstream ss(pElepPipe->getText());
			std::string sPipeFile;
			std::list<std::string> listFiles;
			while (std::getline(ss, sPipeFile, '@'))
			{
				listFiles.push_back(sPipeFile);
			}
			m_listPipeFileList.insert(std::make_pair(i, listFiles));
		}

		XmlElement *pElepPlaceName = pEleScreen->getSubElement("PlaceName");
		if (pElepPlaceName)
		{
			std::stringstream ss(pElepPlaceName->getText());
			std::string sPlaceNameFile;
			std::list<std::string> listFiles;
			while (std::getline(ss, sPlaceNameFile, '@'))
			{
				listFiles.push_back(sPlaceNameFile);
			}
			m_listPlaceNameFileList.insert(std::make_pair(i, listFiles));
		}
	}

	//key value
	XmlElement *pEleValues = pEleDoc->getSubElement("keyvalue");
	if (!pEleValues)
		return;
	XmlNodeList listNodes = pEleValues->getChildren();
	XmlNodeList::const_iterator it = listNodes.begin();
	for (; it!=listNodes.end(); it++)
	{
		XmlElement *pEle = dynamic_cast<XmlElement *>((*it).get());
		if (!pEle)
			continue;
		m_makKV.insert(std::make_pair(pEle->getName(), pEle->getText()));
	}
}
#pragma endregion

#pragma region XmlWSWritter
XmlWSWritter::XmlWSWritter()
{
}
XmlWSWritter::~XmlWSWritter(void)
{
	
}
void XmlWSWritter::ScerrnNum(int nScerrnNum)
{
	WorkSpaceWritter::ScerrnNum(nScerrnNum);
}
void XmlWSWritter::Save(const char *FilePath)
{
	std::ofstream out;
	out.open(FilePath);

	m_xmlDoc.addSubElement("screennum", osgEarth::Stringify()<<m_nScreenNum);
	for (int i=0; i<m_nScreenNum; i++)
	{
		m_xmlDoc.addSubElement(osgEarth::Stringify()<<"screen_"<<i, "");
	}

	for (int i=0; i<m_nScreenNum; i++)
	{
		std::string sKey = osgEarth::Stringify()<<"screen_"<<i;
		(m_xmlDoc.getSubElement(sKey))->addSubElement("Terrain", m_listTerrainFileList[i]);
		(m_xmlDoc.getSubElement(sKey))->addSubElement("Image", m_listImageFileList[i]);
		(m_xmlDoc.getSubElement(sKey))->addSubElement("Model", m_listModelFileList[i]);
		(m_xmlDoc.getSubElement(sKey))->addSubElement("Vector", m_listVectorFileList[i]);
		(m_xmlDoc.getSubElement(sKey))->addSubElement("Oblique", m_listObliqueFileList[i]);
		(m_xmlDoc.getSubElement(sKey))->addSubElement("Pipe", m_listPipeFileList[i]);
		(m_xmlDoc.getSubElement(sKey))->addSubElement("PlaceName", m_listPlaceNameFileList[i]);
	}

	m_xmlDoc.addSubElement("keyvalue", "");
	std::map<std::string, std::string>::const_iterator it = m_makKV.begin();
	for (; it!=m_makKV.end(); it++)
	{
		(m_xmlDoc.getSubElement("keyvalue"))->addSubElement(it->first, it->second);
	}

	m_xmlDoc.store(out);
	m_strPath = FilePath;
}
void XmlWSWritter::WriteTerrainFile(std::string strFile, int nScreenIndex)
{
	std::map<int, std::string>::iterator it = m_listTerrainFileList.find(nScreenIndex);
	if (it == m_listTerrainFileList.end())
		m_listTerrainFileList.insert(std::make_pair(nScreenIndex, strFile));
	else
	{
		std::string sTest = it->second;
		it->second = sTest + "@" + strFile;
	}
}
void XmlWSWritter::WriteImageFile(std::string strFile, int nScreenIndex)
{
	std::map<int, std::string>::iterator it = m_listImageFileList.find(nScreenIndex);
	if (it == m_listImageFileList.end())
		m_listImageFileList.insert(std::make_pair(nScreenIndex, strFile));
	else
	{
		std::string sTest = it->second;
		it->second = sTest + "@" + strFile;
	}
}
void XmlWSWritter::WriteModelFile(std::string strFile, int nScreenIndex)
{
	std::map<int, std::string>::iterator it = m_listModelFileList.find(nScreenIndex);
	if (it == m_listModelFileList.end())
		m_listModelFileList.insert(std::make_pair(nScreenIndex, strFile));
	else
	{
		std::string sTest = it->second;
		it->second = sTest + "@" + strFile;
	}
}
void XmlWSWritter::WriteVectorFile(std::string strFile, int nScreenIndex)
{
	std::map<int, std::string>::iterator it = m_listVectorFileList.find(nScreenIndex);
	if (it == m_listVectorFileList.end())
		m_listVectorFileList.insert(std::make_pair(nScreenIndex, strFile));
	else
	{
		std::string sTest = it->second;
		it->second = sTest + "@" + strFile;
	}
}
void XmlWSWritter::WriteObliqueFile(std::string strFile, int nScreenIndex)
{
	std::map<int, std::string>::iterator it = m_listObliqueFileList.find(nScreenIndex);
	if (it == m_listObliqueFileList.end())
		m_listObliqueFileList.insert(std::make_pair(nScreenIndex, strFile));
	else
	{
		std::string sTest = it->second;
		it->second = sTest + "@" + strFile;
	}
}
void XmlWSWritter::WritePipeFile(std::string strFile, int nScreenIndex)
{
	std::map<int, std::string>::iterator it = m_listPipeFileList.find(nScreenIndex);
	if (it == m_listPipeFileList.end())
		m_listPipeFileList.insert(std::make_pair(nScreenIndex, strFile));
	else
	{
		std::string sTest = it->second;
		it->second = sTest + "@" + strFile;
	}
}
void XmlWSWritter::WritePlaceNameFile(std::string strFile, int nScreenIndex)
{
	std::map<int, std::string>::iterator it = m_listPlaceNameFileList.find(nScreenIndex);
	if (it == m_listPlaceNameFileList.end())
		m_listPlaceNameFileList.insert(std::make_pair(nScreenIndex, strFile));
	else
	{
		std::string sTest = it->second;
		it->second = sTest + "@" + strFile;
	}
}
void XmlWSWritter::WriteValue(std::string strKey, std::string strValue)
{
	m_makKV.insert(std::make_pair(strKey, strValue));
}
void XmlWSWritter::Clear()
{
	m_listTerrainFileList.clear();
	m_listImageFileList.clear();
	m_listModelFileList.clear();
	m_listVectorFileList.clear();
	m_listObliqueFileList.clear();
	m_listPipeFileList.clear();
	m_listPlaceNameFileList.clear();
	m_makKV.clear();
	m_xmlDoc.getChildren().clear();
}
#pragma endregion