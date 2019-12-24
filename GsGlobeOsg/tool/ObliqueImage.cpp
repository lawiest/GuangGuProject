#pragma once

#include "ObliqueImage.h"
#include <osgDB/ReadFile>
#include <io.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "curl/curl.h"
#include <stack>
#include <osg/MatrixTransform>
#include <osgEarth/XmlUtils>

#include <tool/GeoGlobleEvent.h>
#include <osgEarth/GeoTransform>
#include <osgearth/MapNode>
#include "Analysis/DistanceLoD.h"

using namespace std;

#include <utility.h>

class OsgbWorkThread : public OpenThreads::Thread
{
public:

	OsgbWorkThread()
	{
		dbLod = 0.0;
	}

	virtual void run()
	{
		for (int i = 0; i < vecFile.size(); i++)
		{
			osg::ref_ptr<osg::Node> node = NULL;
			
			if (vecPathFiles.size() == 0)
			{
				node = osgDB::readNodeFile(vecFile[i]);
			}
			else
			{
				std::string httpPath = vecPathFiles[i] + vecFile[i].substr(0, vecFile[i].length() - 1) + ".osgb";
				node = osgDB::readNodeFile(httpPath);
			}

			if (node == NULL)
				continue;

			//setLod(node);

			node->setName("OsgbLayer");
			node->getOrCreateStateSet()->setAttributeAndModes(new osg::Program, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
			node->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
			osg::ref_ptr<osgEarth::GeoTransform> ptrLocator = new osgEarth::GeoTransform();
			ptrLocator->addChild(node);

			osg::Vec3d vecPos = GeoGlobe::CObliqueImageLoader::Position();

			std::string strProj = GeoGlobe::CObliqueImageLoader::ProjString();
			osg::ref_ptr<osgEarth::SpatialReference> ptrSrs = osgEarth::SpatialReference::create(strProj);

			osg::Vec3d vecOut;
			if (ptrSrs != NULL)
				ptrSrs->transform(vecPos, GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMap()->getSRS(), vecOut);

			osgEarth::GeoPoint point = osgEarth::GeoPoint(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getMapSRS(), vecOut);
			ptrLocator->setPosition(point);

			osg::ref_ptr<GLOBE_NAME::GeoGlobleEventAdapter> ptrUserEvenAdapter = new GLOBE_NAME::GeoGlobleEventAdapter();
			ptrUserEvenAdapter->setEventType(GLOBE_NAME::ADD_OVERLAYNODE);
			ptrUserEvenAdapter->setData(ptrLocator);
			GLOBE_NAME::OsgCoreSingleton::Instance()->OsgViewer()->getEventQueue()->userEvent(ptrUserEvenAdapter);
		}
	}

	void setLod(osg::Node* node)
	{
		if (dbLod == 0.0)
			return;

		osg::ref_ptr<osg::PagedLOD> lod = dynamic_cast<osg::PagedLOD*>(node);
		if (lod != NULL)
		{
			osg::LOD::RangeList rangeList = lod->getRangeList();

			for (int i = 0; i < rangeList.size(); i++)
			{
				//rangeList[i].first = rangeList[i].first * 5;
				//rangeList[i].second = rangeList[i].second * 5;

				if (i == 0)
					lod->setRange(i, dbLod, dbLod + rangeList[i].second/* * dbLod*/);
				else
					lod->setRange(i, rangeList[i - 1].second, rangeList[i].first + rangeList[i].second/* * dbLod*/);
			}

			//lod->setRangeList(rangeList);
		}
	}

	void setPathFile(std::vector<std::string> path)
	{
		vecPathFiles = path;
	}

	void setFile(std::vector<std::string> file)
	{
		vecFile = file;
	}

	void setLod(double lod)
	{
		dbLod = lod;
	}

private:
	std::vector<std::string> vecPathFiles;
	std::vector<std::string> vecFile;
	double dbLod;
};

namespace GeoGlobe
{
	double CObliqueImageLoader::dbHeight = 0.0;
	osg::Group* CObliqueImageLoader::m_ptrGroup = NULL;
	double CObliqueImageLoader::m_dbStepX = 1.3;
	double CObliqueImageLoader::m_dbStepY = 1.4;
	osg::Vec3d CObliqueImageLoader::m_vecPos = osg::Vec3d(110.0, 30.0, 6371393 * 1.7);
	bool CObliqueImageLoader::m_isRotate =false;
	std::string CObliqueImageLoader::m_strSrs = "wgs84";

	CObliqueImageLoader::CObliqueImageLoader() 
	{
		m_ptrGroup = new osg::Group();
	}

	CObliqueImageLoader::~CObliqueImageLoader() 
	{

	}

	void CObliqueImageLoader::getFolders(std::string path, std::vector<std::string>& files)
	{
		//文件句柄  
		long hFile = 0;  
		//文件信息  
		struct _finddata_t fileinfo;  
		std::string p;  
		if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)  
		{ 
			do  
			{
				//如果是目录,迭代之  
				//如果不是,加入列表  
				if((fileinfo.attrib &  _A_SUBDIR))  
				{  
					files.push_back(p.assign(path).append("\\").append(fileinfo.name).append("\\").append(fileinfo.name).append(".osgb") );  
				}  
			}while(_findnext(hFile, &fileinfo)  == 0);  
			_findclose(hFile);  
		}  
	}

	void getFoldersEx(std::string path, std::vector<std::string>& files)
	{
		//文件句柄  
		intptr_t hFile = 0;
		//文件信息  
		struct _finddata_t fileinfo;
		std::string p;
		if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
		{
			do
			{
				//如果是目录,迭代之  
				//如果不是,加入列表  
				if ((fileinfo.attrib /*&  _A_SUBDIR*/))
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name))/*.append("\\").append(fileinfo.name).append(".osgb"))*/;
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}


	void CObliqueImageLoader::getFiles( std::string path, std::vector<std::string>& files )  
	{  
		//文件句柄  
		long   hFile   =   0;  
		//文件信息  
		struct _finddata_t fileinfo;  
		std::string p;  
		if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)  
		{  
			do  
			{  
				//如果是目录,迭代之  
				//如果不是,加入列表  
				if((fileinfo.attrib &  _A_SUBDIR))  
				{  
					if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)  
						getFiles( p.assign(path).append("\\").append(fileinfo.name), files );  
				}  
				else  
				{  
					files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
				}  
			}while(_findnext(hFile, &fileinfo)  == 0);  
			_findclose(hFile);  
		}  
	} 

	void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
	{
		std::string::size_type pos1, pos2;
		pos2 = s.find(c);
		pos1 = 0;
		while (std::string::npos != pos2)
		{
			v.push_back(s.substr(pos1, pos2 - pos1));

			pos1 = pos2 + c.size();
			pos2 = s.find(c, pos1);
		}
		if (pos1 != s.length())
			v.push_back(s.substr(pos1));
	}

	void str2Vec3d(std::vector<std::string> v, osg::Vec3d &p)
	{
		int pSize = v.size();
		for (int i = 0; i < pSize; i++)
		{
			p[i] = (atof(v[i].c_str()));
		}
		return;
	}

	void ReadMetaInfoFromLocal(std::string strPath, osg::Vec3d& vecPos, std::string& strSrs)
	{
		size_t index2 = strPath.rfind("\\");
		std::string strMetaInfoPath = strPath.substr(0, index2 ==std::string::npos ? strPath.rfind("/") : index2);
		strMetaInfoPath += index2==std::string::npos? "/metadata.xml":"\\metadata.xml";
		osg::ref_ptr<osgEarth::XmlDocument> doc = osgEarth::XmlDocument::load(strMetaInfoPath);

		if (!doc)
			return;

		string strSRSPos = doc->findElement("SRSOrigin")->getText();

		strSrs = doc->findElement("SRS")->getText();

		std::vector<std::string> vSrs;
		SplitString(strSRSPos, vSrs, ",");
		str2Vec3d(vSrs, vecPos);
	}

	osg::Group* CObliqueImageLoader::LoadFromFile(std::string path, double dbMinLod , double dbMaxLod )
	{
		ReadMetaInfoFromLocal(path,m_vecPos,m_strSrs);

		osg::Group* pOImageGroup = new osg::Group();
		std::vector<std::string> files;

		long hFile = 0;
		//文件信息  
		struct _finddata_t fileinfo;
		hFile = _findfirst(path.c_str(), &fileinfo);

		if ((fileinfo.attrib &  _A_NORMAL))
			files.push_back(path);

		//获取该路径下的所有文件  
		getFoldersEx(path, files);
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(files[2]);
		if (node)
			pOImageGroup->addChild(node);

// 		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("E:\\OSGB1016\\DSM\\DSM_L0_R0_C0.osgb");
// 		pOImageGroup->addChild(node);
// 		OsgbWorkThread * workThread = new OsgbWorkThread();
// 		//workThread->setPathFile(pathFiles);
// 		workThread->setFile(files);
// 		//workThread->setLod(dbLod);
// 		workThread->start();
// 
// 		GLOBE_NAME::OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(new GLOBE_NAME::GeoGlobleEventHandler());
		//int size = files.size();
		//for (int i = 0; i < size; i++)
		//{
		//	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(files[i]);

		//	//osg::ref_ptr<osg::PagedLOD> lod= dynamic_cast<osg::PagedLOD*>( node.get());
		//	//if (lod != NULL)
		//	//{		
		//	//	osg::LOD::RangeList rangeList = lod->getRangeList();
		//	//	
		//	//	for (int i = 0; i<rangeList.size();i++)
		//	//	{
		//	//		rangeList[i].first = rangeList[i].first*2;
		//	//		rangeList[i].second = rangeList[i].second*2;
		//	//	}

		//	//	lod->setRangeList(rangeList);
		//	//}

		//	if (node != NULL)
		//	{
		//		pOImageGroup->addChild(node.get());
		//	}
		//	//解析osgb文件
		//	//AnalysisOsgbNode(node.get());	 
		//}

		return pOImageGroup;
	}

	//***************************************
	size_t writeToString(void *ptr, size_t size, size_t count, void *stream)
	{
		((std::string*)stream)->append((char*)ptr, 0, size* count);
		return size* count;
	}

	//get the web's html source code using curllib
	void http_get(std::string httpurl,std::string &result)
	{
		result = "";
		CURL *curl;
		CURLcode res;
		const char *url = httpurl.c_str();
		curl = curl_easy_init();
		if(curl)
		{
			curl_easy_setopt(curl,CURLOPT_URL,url);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToString);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
		}
		return ;
	}


	//extract the osgbFileName from every <a XXX>XXX</a> html code
	void lineparser(std::string line, std::vector<std::string>& files)
	{
		std::string head = "href=\"";
		if (line.find(head) != -1)
		{
			line = line.substr(line.find(head), line.find("</a>") - line.find(head));
			std::string osgbFileName = line.substr(line.find(">") + 1, line.length() - line.find(">"));
			files.push_back(osgbFileName);
		}
		return;
	}

	//use apache server
	void apacheParser(std::string content, std::vector<std::string>& files)
	{
		std::string temp = content;
		std::string tab_top = "<tr";
		std::string tab_end = "</tr>";

		size_t topPos = 0;
		size_t endPos = 0;
		//find pairs of <a and </a>,analysis the words between them which contains one osgbFileName
		for (; temp.find(tab_top) != -1;)
		{
			topPos = content.find(tab_top, topPos + 1);
			endPos = topPos;
			topPos = content.find(tab_end, endPos + 1);
			std::string line = content.substr(endPos, topPos - endPos + tab_end.size());
			if (line.find("[DIR]") == -1 && line.find("[TXT]") == -1)
			{
				temp = content.substr(topPos + 1, content.length() - topPos);
				continue;
			}
			lineparser(line, files);
			temp = content.substr(topPos + 1, content.length() - topPos);
		}

		vector<string>::iterator iter = files.begin();

		if (iter != files.end())
			files.erase(iter);

		return;
	}

	//use nginx server
	void nginxParser(std::string content, std::vector<std::string>& files)
	{
		std::string temp = content;
		std::string tab_top = "<a";
		std::string tab_end = "</a>";

		size_t topPos = 0;
		size_t endPos = 0;
		//find pairs of <a and </a>,analysis the words between them which contains one osgbFileName
		for (; temp.find(tab_top) != -1;)
		{
			topPos = content.find(tab_top, topPos + 1);
			endPos = topPos;
			topPos = content.find(tab_end, endPos + 1);
			std::string line = content.substr(endPos, topPos - endPos + tab_end.size());
			lineparser(line, files);
			temp = content.substr(topPos + 1, content.length() - topPos);
		}

		//delete the forume which is father dir and have no data;
		//delete the parent dir
		vector<string>::iterator iter = files.begin();

		if (iter!= files.end())
			files.erase(iter);

		return;
	}

	//split html code to lines which contains osgbFileName  ,if the html use <tr></tr>
	void contentparser(std::string content, std::vector<std::string>& files)
	{
		if (content.find("</tr>") != -1)
		{
			apacheParser(content, files);
			return;
		}
		nginxParser(content, files);
	}

	void getRootName(std::string path, std::vector<std::string>& files)
	{
		std::string httpWebCode;
		http_get(path,httpWebCode);
		contentparser(httpWebCode, files);
	}

	void getPathFiles(std::string path, std::vector<std::string>& pathFiles, std::vector<std::string>& name)
	{	
		getRootName(path,pathFiles);
		name = pathFiles;
		for(int i=0;i<pathFiles.size();i++)
		{
			pathFiles[i] = path+pathFiles[i];
		}
	}

	//***************************************


	bool GetXY(std::string filename, int &x, int &y)
	{
		if (filename.size() < 4)
		{
			return false;
		}
		int index0 = filename.rfind("_");
		std::string y_str = filename.substr(index0 + 1);
		std::string y_remain = filename.substr(0, index0);
		y = atoi(y_str.c_str());

		int index1 = y_remain.rfind("_");
		std::string x_str = y_remain.substr(index1 + 1);
		x = atoi(x_str.c_str());
		return true;
	}

	void getHttpFiles(std::string path, std::vector<std::string>& files)
	{
		std::string httpWebCode;
		http_get(path, httpWebCode);
		contentparser(httpWebCode, files);
	}

	osg::Group* CObliqueImageLoader::LoadFromServerNormal(std::string strUrl)
	{
		m_ptrGroup = new osg::Group();
		//osg::Group* pOImageGroup = new osg::Group();
		std::vector<std::string> files;

		strUrl = "http://192.168.51.77/data/jj3dmodeldata/data/";
		getHttpFiles(strUrl, files);

		int size = files.size();
		for (int i = 0; i < size; i++)
		{
			std::string httpPath = strUrl + files[i] + files[i].substr(0, files[i].length() - 1) + ".osgb";
			osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(httpPath);

			if (node && node->getBound().center().z() > dbHeight)
			{
				dbHeight = node->getBound().center().z();
			}

			if (node != NULL)
			{
				m_ptrGroup->addChild(node.get());
			}
		}

		return m_ptrGroup;
	}

	void CObliqueImageLoader::SetLod(double dbMinRange, double dbLod)
	{
		if (m_ptrGroup == NULL)
			return;

		cout << "数据外接圆半径：" << m_ptrGroup->getBound().radius()<<endl;

	 	for (int i = 0; i< m_ptrGroup->getNumChildren(); i++)
	 	{
			osg::Node* node = m_ptrGroup->getChild(i);

			osg::ref_ptr<osg::PagedLOD> lod = dynamic_cast<osg::PagedLOD*>(node);
			if (lod != NULL)
			{
				osg::LOD::RangeList rangeList = lod->getRangeList();

				for (int i = 0; i < rangeList.size(); i++)
				{
					//rangeList[i].first = rangeList[i].first * 5;
					//rangeList[i].second = rangeList[i].second * 5;

					if (i == 0)
						lod->setRange(i, dbMinRange, dbMinRange + rangeList[i].second/* * dbLod*/);
					else
						lod->setRange(i, rangeList[i - 1].second, rangeList[i].first + rangeList[i].second/* * dbLod*/);
				}

				//lod->setRangeList(rangeList);
			}
	 	}
	}


	void CObliqueImageLoader::SetStep(double dbStep)
	{
		m_dbStepY = dbStep;
	}

	osg::Group* CObliqueImageLoader::LoadFromServerEx(std::string strUrl)
	{
		osg::Group* pOImageGroup = new osg::Group();

		//***********************
		//path = "http://192.168.51.77/data/jj3dmodeldata/data/";
		//strUrl = "http://192.168.51.104/data/Data_Copy_Test/";


		//得到每个大文件夹和同级的metadata.xml的路径
		std::vector<std::string> pathFiles;
		std::vector<std::string> name;
		getPathFiles(strUrl, pathFiles, name);

		if (pathFiles.size() <= 0)
			return NULL;

		std::vector<std::string> files;
		//getRootName(pathFiles[0], files);
		files = name;

		for (int i = 0; i<pathFiles.size() - 1; i++)
		{
			//获取大文件夹下的包含的所有根文件名称
			std::vector<std::string> files;
			getRootName(pathFiles[0], files);
			

			m_ptrGroup = new osg::Group();

			for (int j = 0; j <files.size(); j++)
			{
				std::string httpPath = pathFiles[0] + files[j] + files[j].substr(0, files[j].length() - 1) + ".osgb";
				osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(httpPath);

				if (node && node->getBound().center().z() > dbHeight)
				{
					dbHeight = node->getBound().center().z();
				}

				m_ptrGroup->addChild(node);
			}


			for (int i = 0; i<name.size() - 2; i++)
			{
				int x = 0;
				int y = 0;
				if (GetXY(name[i], x, y))
				{
					if (pOImageGroup != NULL)
					{
						int w = m_dbStepX * m_ptrGroup->getBound().radius();
						int h = m_dbStepY * m_ptrGroup->getBound().radius();

						osg::Vec3d v3_i_j;
						v3_i_j.x() = x*w;
						v3_i_j.y() = y*h;
						v3_i_j.z() = 0;

						osg::MatrixTransform *m_i_j = new osg::MatrixTransform();
						if (m_isRotate)
						{
							m_i_j->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(-5.0), osg::Vec3d(0.0, 1.0, 0.0))*osg::Matrix::rotate(osg::DegreesToRadians(53.0), osg::Vec3d(0.0, 0.0, 1.0))*osg::Matrix::rotate(osg::DegreesToRadians(15.0), osg::Vec3d(1.0, 0.0, 0.0))* osg::Matrix::translate(v3_i_j));
						}
						else
						{
							m_i_j->setMatrix(osg::Matrix::translate(v3_i_j));
						}
						
						m_i_j->addChild(m_ptrGroup);
						pOImageGroup->addChild(m_i_j);
					}
				}
			}

		}

		return pOImageGroup;
	}

	//get the metadataXML infomation
	void getMetadataInfo(std::string dataUrl, osg::Vec3d& srs ,std::string& strSrs)
	{
		dataUrl = dataUrl.substr(0, dataUrl.size() - 1);
		dataUrl = dataUrl.substr(0, dataUrl.find_last_of("/") + 1);
		std::vector<std::string> pathFiles;
		std::vector<std::string> name;
		getPathFiles(dataUrl, pathFiles, name);
		int filesCount = pathFiles.size();
		for (std::vector<std::string>::iterator it = pathFiles.begin(); it != pathFiles.end(); it++)
		{
			if ((*it).find("metadata.xml") != -1)
			{
				std::string xmlfile;
				http_get((*it), xmlfile);

				istringstream stream(xmlfile);
				osg::ref_ptr<osgEarth::XmlDocument> doc = osgEarth::XmlDocument::load(stream);

				string strSRS = doc->findElement("SRSOrigin")->getText();

				strSrs = doc->findElement("SRS")->getText();

				std::vector<std::string> vSrs;
				SplitString(strSRS, vSrs, ",");
				str2Vec3d(vSrs, srs);
			}
		}
		return;
	}

	osg::Vec3d CObliqueImageLoader::Position()
	{
		return m_vecPos;
	}

	std::string CObliqueImageLoader::ProjString()
	{
		return m_strSrs;
	}

	osg::Group* CObliqueImageLoader::LoadFromServer(std::string strUrl, double dbMinLod, double dbMaxLod)
	{
		if (m_ptrGroup == NULL)
			m_ptrGroup = new osg::Group();

		osg::LOD* pOImageGroup = new osg::DistanceLOD();

		osg::Vec3d vecPos;
		getMetadataInfo(strUrl, vecPos, m_strSrs);

		m_vecPos = vecPos;

		//得到每个大文件夹和同级的metadata.xml的路径
		std::vector<std::string> pathFiles; 
		std::vector<std::string> name;
		getPathFiles(strUrl,pathFiles,name);

		if (pathFiles.size() <= 0)
			return NULL;

		std::vector<std::string> files;
		files = name;

		////测试代码
		//OsgbWorkThread * workThread = new OsgbWorkThread();
		//workThread->setPathFile(pathFiles);
		//workThread->setFile(files);
		//workThread->setLod(dbLod);
		//workThread->start();

		//GLOBE_NAME::OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(new GLOBE_NAME::GeoGlobleEventHandler());
		////测试代码

		//for(int i=0;i<pathFiles.size()-1;i++)
		//{
		//	std::string httpPath = pathFiles[i] + files[i].substr(0, files[i].length() - 1) + ".osgb";
		//	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(httpPath);
		//	if (node != NULL)
		//		pOImageGroup->addChild(node);
		//}
		
		osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(pathFiles[0]);

		if (node != NULL)
			pOImageGroup->addChild(node, dbMinLod,dbMaxLod);

		return pOImageGroup;
	}
}
