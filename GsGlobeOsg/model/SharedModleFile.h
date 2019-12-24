//********************************************************************************** 
//** 文件名称：SharedModelFile.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 共享模型数据文件定义
//** 创 建 人： 李静
//** 创建日期： 2012-04-11
//********************************************************************************** 
#pragma once
#include "SharedModel.h"
#include <vector>
#include <map>
#include <memory>
using namespace std;

typedef std::shared_ptr<CSharedModel> SharedModelSharedPtr;//共享模型智能指针
typedef vector<SharedModelSharedPtr> SharedModelList;
typedef map<UID64,UINT> ObjectIDAndOffsetList;//共享模型的ID和偏移量

typedef struct _tagSDMFileHeder
{
	UINT unDataVersion;//数据版本号
	UINT64 uCheckBit;//校验位
	UINT unObjectNum;//对象个数
	ObjectIDAndOffsetList sdmidAndOffsetList;//对象的偏移量列表
	
}SDMFileHeder;//文件头

typedef struct _tagSDMFileBody
{
	SharedModelList m_ShareModelList;//共享模型文件列表

}SDMFileBody;//文件体

class /*DLL_EXPORT*/ CSharedModleFile
{
public:
	//********************************************************************************** 
	//** 函数名称： CSharedModelFile
	//** 功能描述： 构造函数
	//** 输    入： 无          
	//** 输    出： 无
	//** 返回值：无
	//** 作    者： 李静
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	CSharedModleFile(void);

	//********************************************************************************** 
	//** 函数名称： ~CSharedModelFile
	//** 功能描述： 析构函数
	//** 输    入： 无          
	//** 输    出： 无
	//** 返回值：无
	//** 作    者： 李静
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	~CSharedModleFile(void);

	//********************************************************************************** 
	//** 函数名称： read
	//** 功能描述： 读取文件数据
	//** 输    入： 流对象f的引用           
	//** 输    出： 无
	//** 返回值：	共享模型对象列表
	//** 作    者： 李静
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	virtual SharedModelList read(std::ifstream & f);

	//********************************************************************************** 
	//** 函数名称： write
	//** 功能描述： 读取文件数据
	//** 输    入： 流对象f的引用           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 李静
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	virtual void write(std::ofstream & f);
private:
	SDMFileHeder m_SDMFileHeder;
	SDMFileBody m_SDMFileBody;
	string m_strFileName;//文件名称
	size_t m_FileSize;//文件大小
    Buffer m_ReadBuf;//读文件缓冲区大小
    Buffer m_WriteBuf;//写文件缓冲区大小

public:
	void setFileName(string filename);
	string getFileName();
	//********************************************************************************** 
	//** 函数名称： getFileSize
	//** 功能描述： 得到文件大小
	//** 输    入： strFileName文件路径         
	//** 输    出： 无
	//** 返回值：文件大小
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	size_t getFileSize();
	SharedModelList& getSharedModelList();
    void Flush();
};
