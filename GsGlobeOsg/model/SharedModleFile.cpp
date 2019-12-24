//********************************************************************************** 
//** 文件名称：SharedModelFile.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 共享模型数据文件实现
//** 创 建 人： 李静
//** 创建日期： 2012-04-11
//********************************************************************************** 

#include "SharedModleFile.h"
#include <sys/stat.h>
//********************************************************************************** 
//** 函数名称： CSharedModelFile
//** 功能描述： 构造函数
//** 输    入： 无          
//** 输    出： 无
//** 返回值：无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
CSharedModleFile::CSharedModleFile(void)
{
}

//********************************************************************************** 
//** 函数名称： ~CSharedModelFile
//** 功能描述： 析构函数
//** 输    入： 无          
//** 输    出： 无
//** 返回值：无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
CSharedModleFile::~CSharedModleFile(void)
{
}

//********************************************************************************** 
//** 函数名称： read
//** 功能描述： 读取文件数据
//** 输    入： 流对象f的引用           
//** 输    出： 无
//** 返回值：	共享模型对象列表
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
SharedModelList CSharedModleFile::read(std::ifstream & f)
{
	//得到系统数据版本号
	f.read((char*)&m_SDMFileHeder.unDataVersion,sizeof(UINT));
	//得到校验位
	f.read((char*)&m_SDMFileHeder.uCheckBit,sizeof(UINT64));
	//得到对象个数
	f.read((char*)&m_SDMFileHeder.unObjectNum,sizeof(UINT));
	//得到文件头大小
	size_t sFileHeadBuff=sizeof(UINT)+sizeof(UINT64)+sizeof(UINT)
		+m_SDMFileHeder.unObjectNum*(sizeof(UINT)+sizeof(UID64));

    UINT64 unSDMID=0;
    UINT unOffset=0;
    typedef pair <UINT64, UINT> Int_Pair;
    for(UINT i=0;i<m_SDMFileHeder.unObjectNum;i++)
    {
        f.read((char*)&unSDMID,sizeof(UINT64));
        f.read((char*)&unOffset,sizeof(UINT));
        m_SDMFileHeder.sdmidAndOffsetList.insert(Int_Pair(unSDMID,unOffset));
    }

	//跳过文件头
	f.seekg(sFileHeadBuff,ios::beg);
	size_t sFileBodyBuff=getFileSize()-sFileHeadBuff;
	char* p=new char[sFileBodyBuff];
	size_t readByte=0;
	if(p!=NULL)
	{
		while(!f.eof()&&readByte<getFileSize())
		{
			f.read(p+readByte,getFileSize()-readByte);
			readByte+=f.gcount();
		}

	}

    m_ReadBuf.attach((void*)p,sFileBodyBuff);
	p=NULL;
	for(UINT i=1;i<=m_SDMFileHeder.unObjectNum;i++)
	{
		//定义模型对象指针
		SharedModelSharedPtr sharedmodel(new CSharedModel());
		//读取对象对象
		sharedmodel->readBuffer(m_ReadBuf);

		//读取lod信息（根据新结构增加）
		sharedmodel->readlodBuffer(m_ReadBuf);

		m_SDMFileBody.m_ShareModelList.push_back(sharedmodel);
	}
	return m_SDMFileBody.m_ShareModelList;
}

//********************************************************************************** 
//** 函数名称： write
//** 功能描述： 读取文件数据
//** 输    入： 流对象f的引用           
//** 输    出： 无
//** 返回值：	无
//** 作    者： 李静
//** 创建日期： 2012-04-11
//**********************************************************************************
void CSharedModleFile::write(std::ofstream & f)
{
	if(m_FileSize != 0)
	{
		char* p = new char[m_FileSize];
		if(p != NULL)
		{
            m_WriteBuf.attach((void*)p,m_FileSize);
		}
		else
		{
			return;
		}
	}


	//写入文件头部分
	m_WriteBuf.write((char*)&(m_SDMFileHeder.unDataVersion),sizeof(UINT));
	m_WriteBuf.write((char*)&(m_SDMFileHeder.uCheckBit),sizeof(UINT64));
	m_WriteBuf.write((char*)&(m_SDMFileHeder.unObjectNum),sizeof(UINT));

    ObjectIDAndOffsetList::iterator iter;
    iter = m_SDMFileHeder.sdmidAndOffsetList.begin();

	for(UINT i=0;i<m_SDMFileHeder.sdmidAndOffsetList.size();i++)
	{
		m_WriteBuf.write((char*)&iter->first,sizeof(UID64));
		m_WriteBuf.write((char*)&iter->second,sizeof(UINT));
		iter++;
	}
    int nSize = 0;
	//写入文件体部分到缓冲区
	for(UINT i=0;i<m_SDMFileHeder.unObjectNum;i++)
	{
		m_SDMFileBody.m_ShareModelList[i]->writeBuffer(m_WriteBuf);
        nSize += m_SDMFileBody.m_ShareModelList[i]->sizeBuffer();
	}
    size_t writeByte=1024;
    size_t curWriteByte=0;
    size_t sFileSize = m_FileSize;
    m_WriteBuf.setPos(0);
    if(sFileSize % writeByte > 0)
    {
        for(int i = 0; i < sFileSize / writeByte; i++)
        {

            f.write((const char*)m_WriteBuf.getCurPtr(),writeByte);
            curWriteByte += writeByte;
            m_WriteBuf.setPos(curWriteByte);
        }
        f.write((const char*)m_WriteBuf.getCurPtr(),sFileSize % writeByte);
        curWriteByte += sFileSize % writeByte;
        m_WriteBuf.setPos(curWriteByte-1);//已到文件末尾
    }
    else
    {
        if(sFileSize / writeByte > 1)
        {
            for(int j = 0; j < sFileSize / writeByte; j++)
            {
                f.write((const char*)m_WriteBuf.getCurPtr(),writeByte);
                curWriteByte += writeByte;
                if(curWriteByte < sFileSize)
                {
                    m_WriteBuf.setPos(curWriteByte);
                }
                else
                {
                    m_WriteBuf.setPos(curWriteByte-1);
                }
            }
        }
        else
        {
            f.write((const char*)m_WriteBuf.getCurPtr(),writeByte);
        }

    }
}

//********************************************************************************** 
//** 函数名称： getFileSize
//** 功能描述： 得到文件大小
//** 输    入： 无         
//** 输    出： 无
//** 返回值：文件大小
//** 作    者： 李静
//** 创建日期： 2012-04-14
//**********************************************************************************
size_t CSharedModleFile::getFileSize()
{
	if(!m_strFileName.c_str())
	{
		return -1;
	}
	struct stat f_stat;
	if(stat(m_strFileName.c_str(),&f_stat)==-1)
	{
		return -1;
	}
	m_FileSize = f_stat.st_size;
	return f_stat.st_size;
}
//********************************************************************************** 
//** 函数名称： setFileName
//** 功能描述： 设置文件名称
//** 输    入： 文件名称         
//** 输    出： 无
//** 返回值：	无
//** 作    者： 李静
//** 创建日期： 2012-04-14
//**********************************************************************************
void CSharedModleFile::setFileName(string filename)
{
	m_strFileName=filename;
}
//********************************************************************************** 
//** 函数名称： getFileName
//** 功能描述： 得到文件名称
//** 输    入： 无         
//** 输    出： 无
//** 返回值：	文件名称
//** 作    者： 李静
//** 创建日期： 2012-04-14
//**********************************************************************************
string CSharedModleFile::getFileName()
{
	return m_strFileName;
}
SharedModelList& CSharedModleFile::getSharedModelList()
{
	return m_SDMFileBody.m_ShareModelList;
}
void CSharedModleFile::Flush()
{
    if(m_SDMFileBody.m_ShareModelList.size()>0)
    {
                  
        m_SDMFileBody.m_ShareModelList.clear();
        
    }
}