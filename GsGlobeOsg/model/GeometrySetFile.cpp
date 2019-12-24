#include "GeometrySetFile.h"
#include <sys/stat.h>
CGeometrySetFile::CGeometrySetFile(void)
:m_ReadBuf(),
m_WriteBuf()
{
	//增加初始化
	m_nMemGeometryMax = 0;
}

CGeometrySetFile::~CGeometrySetFile(void)
{
    for(int i = 0; i < m_GeometrySetFileBody.m_GeometryList.size();i++)
    {
		SafeDelete(m_GeometrySetFileBody.m_GeometryList[i].pGeometry);
    }
	for(int i = 0; i < m_newGeometrySetFileBody.m_NewGeometryList.size();i++)
	{
		if(m_newGeometrySetFileBody.m_NewGeometryList[i].pGeometry != NULL)
		{
			delete m_newGeometrySetFileBody.m_NewGeometryList[i].pGeometry;
			m_newGeometrySetFileBody.m_NewGeometryList[i].pGeometry = NULL;
		}
	}
}

//********************************************************************************** 
//** 函数名称： read
//** 功能描述： 从文件中读数据到缓冲区中
//** 输    入： f:文件输入流对象的引用           
//** 输    出： 无
//** 返回值：无
//** 作    者： 杨灵
//** 创建日期： 2012-04-16
//**********************************************************************************
 GeometryList& CGeometrySetFile:: read(ifstream& f)
 {
	 setMemGeometryClear();
     //得到系统数据版本号
     f.read((char*)&m_GeometrySetFileHeader.uDataVersion,sizeof(UINT));

     //得到校验位
     f.read((char*)&m_GeometrySetFileHeader.uCheckBit,sizeof(UINT64));

     //得到对象个数
     f.read((char*)&m_GeometrySetFileHeader.unObjNum,sizeof(UINT));
    
     UINT64 unGSID=0;
     UINT unOffset=0;
     typedef pair <UINT64, UINT> Int_Pair;
     for(UINT i=0;i<m_GeometrySetFileHeader.unObjNum;i++)
     {
         f.read((char*)&unGSID,sizeof(UINT64));
         f.read((char*)&unOffset,sizeof(UINT));
         m_GeometrySetFileHeader.IdAndOffsetList.insert(Int_Pair(unGSID,unOffset));
     }
     
     //跳过文件头
     f.seekg(2*sizeof(UINT) + sizeof(UINT64) + m_GeometrySetFileHeader.unObjNum*(sizeof(UINT64)+sizeof(UINT)),ios::beg);
    
     //设置几何体对象的个数
     //m_GeometrySetFileBody.GMSet.setGeometryNum(m_GeometrySetFileHeader.unObjNum);

     //得到文件头大小
     size_t sFileHeadBuff=sizeof(UINT)+sizeof(UINT64)+sizeof(UINT)
         +m_GeometrySetFileHeader.unObjNum*(sizeof(UINT64)+sizeof(UINT));
      
     //得到文件体大小
     size_t sFileBodyBuff=getFileSize()-sFileHeadBuff;

     //char* p=new char[sFileBodyBuff];
     
     size_t readByte=0;
     if(m_pGeometry!=NULL)
     {
         while(!f.eof()&&readByte<getFileSize())
         {
             f.read(m_pGeometry+readByte,getFileSize()-readByte);
             readByte+=f.gcount();
         }

     }

     m_ReadBuf.set(m_pGeometry,sFileBodyBuff);
     //p=NULL;
     GeometryItem item;

     for(UINT i=0;i< m_GeometrySetFileHeader.unObjNum;i++)
     {
         //读取几何ID
         m_ReadBuf.read((char*)&item.uID,sizeof(UINT64));

         //读取几何类型
         m_ReadBuf.read((char*)&item.byType,sizeof(byte));

		 if(255!=item.byType)
		 {    //读取几何数据长度
			 m_ReadBuf.read((char*)&item.uDataLen,sizeof(UINT));

			 //根据几何类型创建相应的几何类
			 item.pGeometry = CGeometry::CreateGeometry(item.byType);

			 //从相应的缓冲区读取数据
			 if( item.pGeometry!=NULL)
			 {
				 item.pGeometry->readBuffer(m_ReadBuf);
			 }
		 }
		 else
		 {
			item.uDataLen=0;
			item.pGeometry = NULL;	
		 }
         //读取创建时间
         m_ReadBuf.read((char*)&item.uFeatureCreateTime,sizeof(UINT64));
         //读取更新时间
         m_ReadBuf.read((char*)&item.uFeatureUpdateTime,sizeof(UINT64));
         m_GeometrySetFileBody.m_GeometryList.push_back(item);
     }
 
     return m_GeometrySetFileBody.m_GeometryList;
    
 }


 NewGeometryList& CGeometrySetFile:: readnew(ifstream& f)
 {
	 setMemGeometryClear();
	 //得到系统数据版本号
	 f.read((char*)&m_GeometrySetFileHeader.uDataVersion,sizeof(UINT));

	 //得到校验位
	 f.read((char*)&m_GeometrySetFileHeader.uCheckBit,sizeof(UINT64));

	 //得到对象个数
	 f.read((char*)&m_GeometrySetFileHeader.unObjNum,sizeof(UINT));

	 UINT64 unGSID=0;
	 UINT unOffset=0;
	 typedef pair <UINT64, UINT> Int_Pair;
	 for(UINT i=0;i<m_GeometrySetFileHeader.unObjNum;i++)
	 {
		 f.read((char*)&unGSID,sizeof(UINT64));
		 f.read((char*)&unOffset,sizeof(UINT));
		 m_GeometrySetFileHeader.IdAndOffsetList.insert(Int_Pair(unGSID,unOffset));
	 }

	 //跳过文件头
	 f.seekg(2*sizeof(UINT) + sizeof(UINT64) + m_GeometrySetFileHeader.unObjNum*(sizeof(UINT64)+sizeof(UINT)),ios::beg);

	 //设置几何体对象的个数
	 //m_GeometrySetFileBody.GMSet.setGeometryNum(m_GeometrySetFileHeader.unObjNum);

	 //得到文件头大小
	 size_t sFileHeadBuff=sizeof(UINT)+sizeof(UINT64)+sizeof(UINT)
		 +m_GeometrySetFileHeader.unObjNum*(sizeof(UINT64)+sizeof(UINT));

	 //得到文件体大小
	 size_t sFileBodyBuff=getFileSize()-sFileHeadBuff;

	 //char* p=new char[sFileBodyBuff];

	 size_t readByte=0;
	 if(m_pGeometry!=NULL)
	 {
		 while(!f.eof()&&readByte<getFileSize())
		 {
			 f.read(m_pGeometry+readByte,getFileSize()-readByte);
			 readByte+=f.gcount();
		 }

	 }

	 m_ReadBuf.set(m_pGeometry,sFileBodyBuff);
	 //p=NULL;
	 //NewGeometryItem item;

	 for(UINT i=0;i< m_GeometrySetFileHeader.unObjNum;i++)
	 {
		 NewGeometryItem item;

		 //读取几何ID
		 m_ReadBuf.read((char*)&item.uID,sizeof(UINT64));

		 //读取几何类型
		 m_ReadBuf.read((char*)&item.byType,sizeof(byte));

		 if(255!=item.byType)
		 {    //读取几何数据长度
			 m_ReadBuf.read((char*)&item.uDataLen,sizeof(UINT));

			 //根据几何类型创建相应的几何类
			 item.pGeometry = CGeometry::CreateGeometry(item.byType);

			 //从相应的缓冲区读取数据
			 if( item.pGeometry!=NULL)
			 {
				 item.pGeometry->readBuffer(m_ReadBuf);
			 }
		 }
		 else
		 {
			 item.uDataLen=0;
			 item.pGeometry = NULL;	
		 }
		 //读取创建时间
		 m_ReadBuf.read((char*)&item.uFeatureCreateTime,sizeof(UINT64));
		 //读取更新时间
		 m_ReadBuf.read((char*)&item.uFeatureUpdateTime,sizeof(UINT64));

		 //读取lodInfo的个数
		 byte nLodCount = 0;
		 m_ReadBuf.read((char *)&nLodCount,sizeof(byte));
		 for(byte nLodIndex = 0;nLodIndex<nLodCount;nLodIndex++)
		 {
			 LodInfo lodData;
			 lodData.readBuffer(m_ReadBuf);
			 item.vecLodInfo.push_back(lodData);
		 }
		 m_newGeometrySetFileBody.m_NewGeometryList.push_back(item);
		 //m_GeometrySetFileBody.m_GeometryList.push_back(item);
		 
	 }

	 return  m_newGeometrySetFileBody.m_NewGeometryList;

 }

 //********************************************************************************** 
 //** 函数名称： writeObject
 //** 功能描述： 将数据库中导出的数据写入文件中
 //** 输    入： f:文件输出流对象的引用
 //**            buf:要写入的对象内存块
 //** 输    出： 无
 //** 返回值：无
 //** 作    者： 杨灵
 //** 创建日期： 2012-07-27
 //**********************************************************************************

 void CGeometrySetFile::writeObject(ofstream & f,Buffer& buf)
 {
     UINT64 nGeoID = 0;
     byte nGeoType = 0;
     UINT nDataLen = buf.size()-5 + sizeof(nGeoID)+sizeof(nGeoType)+sizeof(nDataLen)+2*sizeof(UINT64);
     UINT64 startTime =0;
    UINT64 endtime =0;
     //跳过buf的头5个字节          
       buf.setPos(5); 
       buf.read((void*)&nGeoID,sizeof(nGeoID));
       buf.read((void*)&nGeoType,sizeof(nGeoType));
       buf.setPos(5);

    //写入几何数据
        f.write((char*)&nGeoID,sizeof(nGeoID));
        f.write((char*)&nGeoType,sizeof(nGeoType));
        f.write((char*)&nDataLen,sizeof(nDataLen));
        f.write((char*)buf.getCurPtr(),buf.size()-5);

        f.write((char*)&startTime,sizeof(startTime));
        f.write((char*)&endtime,sizeof(endtime));
        m_objSizeList.push_back(make_pair(nGeoID,nDataLen));

             
 }

 //********************************************************************************** 
 //** 函数名称： writeFileHeader
 //** 功能描述： 为导出的数据文件添加文件头
 //** 输    入： f:文件输出流对象的引用
 //**              glist:几何列表
 //** 输    出： 无
 //** 返回值：无
 //** 作    者： 杨灵
 //** 创建日期： 2012-07-27
 //**********************************************************************************
  void CGeometrySetFile::writeFileHeader(ofstream & f)
 {
     //几何类型
     byte byType = 255;
    
     //缓存偏移量
     int nOffset = 0;
     //几何数据量
     int nGeoSize = 0;
     UINT64 uGeoID = 0;
     char *p = NULL;
     //写入文件头部分

     //写入系统数据版本号
     int nVersion =1;
     f.write((char*)&nVersion,sizeof(UINT));
     //写入校验位
     char cCheckbits[2];
     cCheckbits[0]='g';
     cCheckbits[1]='s';
     f.write(cCheckbits,sizeof(UINT64));

     //写入几何个数
     UINT nObjNum =m_objSizeList.size();
     f.write((char*)&nObjNum, sizeof(UINT));

     ObjectSizeList::iterator iter =m_objSizeList.begin();
     for(iter;iter != m_objSizeList.end();iter++)
     {
         //写入几何ID
         uGeoID = iter->first;
         f.write((char*)&uGeoID,sizeof(UID64));
         //写入几何偏移量
         nOffset = getObjOffset(uGeoID);
         f.write((char*)&nOffset,sizeof(UINT));

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
size_t CGeometrySetFile::getFileSize()
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
void CGeometrySetFile::setFileName(string filename)
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
string CGeometrySetFile::getFileName()
{
    return m_strFileName;
}
GeometryList& CGeometrySetFile::getGeometryList()
{
	return m_GeometrySetFileBody.m_GeometryList;
}
NewGeometryList& CGeometrySetFile::getNewGeometryList()
{
	return m_newGeometrySetFileBody.m_NewGeometryList;
}
void CGeometrySetFile::Flush()
{
    if (m_GeometrySetFileHeader.IdAndOffsetList.size() > 0)
    {
        m_GeometrySetFileHeader.IdAndOffsetList.clear();
    }
    if(m_GeometrySetFileBody.m_GeometryList.size() > 0)
    {
        for(int i = 0; i < m_GeometrySetFileBody.m_GeometryList.size();i++)
        {
            delete m_GeometrySetFileBody.m_GeometryList[i].pGeometry;
            m_GeometrySetFileBody.m_GeometryList[i].pGeometry = NULL;
        }
        m_GeometrySetFileBody.m_GeometryList.clear();
        m_strFileName.clear();
        m_ReadBuf.clear();
        m_WriteBuf.clear();    
    }

	if(m_newGeometrySetFileBody.m_NewGeometryList.size() > 0)
	{
		for(int i = 0; i < m_newGeometrySetFileBody.m_NewGeometryList.size();i++)
		{
			delete m_newGeometrySetFileBody.m_NewGeometryList[i].pGeometry;
			m_newGeometrySetFileBody.m_NewGeometryList[i].pGeometry = NULL;
		}
		m_newGeometrySetFileBody.m_NewGeometryList.clear();
		m_strFileName.clear();
		m_ReadBuf.clear();
		m_WriteBuf.clear();    
	}
    m_objSizeList.clear();
}
//********************************************************************************** 
    //** 函数名称： getGeometryNum
    //** 功能描述： 得到几何体的个数
    //** 输    入： 无          
    //** 输    出： 无
    //** 返回值：要素的个数
    //** 作    者： 杨灵
    //** 创建日期： 2012-04-16
    //**********************************************************************************
    UINT CGeometrySetFile::getGeometryNum() const
	{
		return m_GeometrySetFileBody.m_GeometryList.size();
	}
    //********************************************************************************** 
    //** 函数名称： getObjOffset
    //** 功能描述： 得到几何偏移量
    //** 输    入： GeoID:几何ID号
    //** 输    出： 无
    //** 返回值：指定几何的偏移量
    //** 作    者： 杨灵
    //** 创建日期： 2012-07-04
    //**********************************************************************************
    int CGeometrySetFile::getObjOffset(UINT64 nGeoID)
    {
        int nsize1 = 0;
        int nsize2 = 0;
        int nsize = -1;
        int index = 0;
        for(int i = 0; i < m_objSizeList.size();i++)
        {
            if(m_objSizeList[i].first == nGeoID)
            {
                index = i;
                break;
            }
        }

        //计算文件头偏移量
        //  nsize1 =(m_objSizeList.size() -index-1)*(sizeof(UINT64)+sizeof(UINT));
        UINT nVersion = 0;//版本号
        UINT64 nCheckBit =0;//校验位
        UINT nObjCount =0;//对象个数


        nsize1 = sizeof(nVersion) + sizeof(nCheckBit) + sizeof(nObjCount) +  m_objSizeList.size()*( sizeof(UINT64)+sizeof(UINT) );

        //计算文件体偏移量
        for(int i = 0; i < index;i++)
        {
            nsize2 += m_objSizeList[i].second;
        }
        nsize =nsize1 + nsize2; 


        return nsize;
    }
	void CGeometrySetFile::setMemGeometryMax(int MemMax)
	{
		m_nMemGeometryMax=MemMax;
	}
// 	void  CGeometrySetFile::setMemGeometry()
// 	{
// 		m_pGeometry=new char[m_nMemGeometryMax];
// 	}
	bool  CGeometrySetFile::setMemGeometry()
	{
		try
		{
			m_pGeometry=new char[m_nMemGeometryMax];
		}
		catch(...)
		{
			return false;
		}

		return true;
	}
	void  CGeometrySetFile::setMemGeometryClear()
	{
		memset(m_pGeometry,0,m_nMemGeometryMax);
	}
	void  CGeometrySetFile::deleteMemGeometry()
	{
		SafeDeleteArray(m_pGeometry);
	}