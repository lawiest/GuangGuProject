#pragma once
#include "ModelDef.h"
#include <vector>
#include <utility>
#include<fstream>
#include <map>
#include <utility>
#include "ModelDef.h"
#include "CT3D_Geometry.h"
using namespace std;
typedef struct _tagGeometryItem
{
    UINT64 uID;//GeometryID
    byte byType;//几何数据类型
    UINT uDataLen;//几何数据长度
    CGeometry* pGeometry;//几何数据体
    UINT64 uFeatureCreateTime;//要素创建时间
    UINT64 uFeatureUpdateTime;//要素更新时间
}GeometryItem;


typedef struct /*DLL_EXPORT*/ tagMTData
{
public:
	void readBuffer(Buffer& buffer)
	{
		buffer.read(&nDataLen, sizeof(nDataLen));
		pMTData = new char[nDataLen];
		buffer.read(pMTData, nDataLen);
	}

	void writeBuffer(Buffer& buffer)
	{
		buffer.write(&nDataLen, sizeof(nDataLen));
		buffer.write(pMTData, nDataLen);
	}

	void copy(const tagMTData& data)
	{
		nDataLen = data.nDataLen;
		if (nDataLen<=0)
		{
			pMTData = NULL;
			return;
		}

		pMTData = new char[nDataLen];
		memcpy(pMTData, data.pMTData, nDataLen);
	}
	int sizeBuffer()
	{
		int nSize = 0;
		nSize += sizeof(int) + nDataLen;
		return nSize;
	}
public:
	int nDataLen;
	char* pMTData;
}MTData;

typedef struct /*DLL_EXPORT*/ tagLOD
{
public:
	void readBuffer(Buffer& buffer)
	{
		buffer.read(&nIndex, sizeof(byte));
		//材质数据个数
		int nSize = 0;
		buffer.read(&nSize,sizeof(int));
		for (int i = 0;i<nSize;i++)
		{
			MTData mtdata;
			mtdata.readBuffer(buffer);
			vecMTData.push_back(mtdata);
		}
	}

	void writeBuffer(Buffer& buffer)
	{
		buffer.write(&nIndex, sizeof(byte));
		//材质数据个数
		int nSize = vecMTData.size();
		buffer.write(&nSize,sizeof(int));
		vector<MTData>::iterator it = vecMTData.begin();
		for (;it!=vecMTData.end();it++)
		{
			it->writeBuffer(buffer);
		}
	}

	void copy(const tagLOD& data)
	{
		nIndex = data.nIndex;
		for (int i = 0;i<data.vecMTData.size();i++)
		{
			MTData newData;
			newData.copy(data.vecMTData[i]);
			vecMTData.push_back(newData);
		}		
	}
	int sizeBuffer()
	{
		int nSize = 0;
		nSize += sizeof(nIndex) + sizeof(int);
		for (int i = 0;i<vecMTData.size();i++)
		{
			nSize += vecMTData[i].sizeBuffer();
		}
		return nSize;
	}
public:
	byte nIndex;
	vector<MTData> vecMTData;
}LodInfo; 

typedef struct _tagNewGeometryItem:public GeometryItem
{
	vector<LodInfo> vecLodInfo;
}NewGeometryItem;



typedef vector<GeometryItem>GeometryList;//几何列表
typedef vector<NewGeometryItem >NewGeometryList;
typedef map<UINT64,UINT> GeometryIDAndOffsetList;//存储几何对象的ID和Offset列表
typedef struct _tagGeometryFileHeader
{
    UINT uDataVersion;//数据版本号
    UINT64 uCheckBit;//校验位
    UINT unObjNum;//对象个数
    GeometryIDAndOffsetList IdAndOffsetList;//几何体的ID和Offset列表

}GeometrySetFileHeader;//文件头

typedef struct _tagGeometrySetFileBody
{
    GeometryList m_GeometryList;//几何列表

}GeometrySetFileBody;//文件体

typedef struct _tagNewGeometrySetFileBody
{
	NewGeometryList m_NewGeometryList;//几何列表

}NewGeometrySetFileBody;//文件体

typedef vector<pair<UINT64,int>> ObjectSizeList;//每个对象的大小，在数据从数据库中导出到文件时使用

class /*DLL_EXPORT*/ CGeometrySetFile
{
public:
    CGeometrySetFile(void);
    virtual ~CGeometrySetFile(void);

    //********************************************************************************** 
    //** 函数名称： read
    //** 功能描述： 从文件中读数据到缓冲区中
    //** 输    入： f:文件输入流对象的引用           
    //** 输    出： 无
    //** 返回值：无
    //** 作    者： 杨灵
    //** 创建日期： 2012-04-16
    //**********************************************************************************
    virtual GeometryList& read(ifstream& f);

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
    virtual void writeObject(ofstream & f,Buffer& buf);


    //********************************************************************************** 
    //** 函数名称： writeFileHeader
    //** 功能描述： 为导出的数据文件添加文件头
    //** 输    入： f:文件输出流对象的引用
    //** 输    出： 无
    //** 返回值：无
    //** 作    者： 杨灵
    //** 创建日期： 2012-07-27
    //**********************************************************************************
    virtual void writeFileHeader(ofstream & f);

    //********************************************************************************** 
    //** 函数名称： setGeometryNum
    //** 功能描述： 设置几何体的个数
    //** 输    入： 文件中读取要素的个数          
    //** 输    出： 无
    //** 返回值：无
    //** 作    者： 杨灵
    //** 创建日期： 2012-04-16
    //**********************************************************************************
    void  setGeometryNum(UINT unMaterialNum);


    //********************************************************************************** 
    //** 函数名称： getGeometryNum
    //** 功能描述： 得到几何体的个数
    //** 输    入： 无          
    //** 输    出： 无
    //** 返回值：要素的个数
    //** 作    者： 杨灵
    //** 创建日期： 2012-04-16
    //**********************************************************************************
    UINT getGeometryNum() const;
    void setFileName(string filename);
    string getFileName();
    void Flush();
    
private:
 
    GeometrySetFileHeader m_GeometrySetFileHeader;//文件头
    GeometrySetFileBody  m_GeometrySetFileBody;//文件体
	NewGeometrySetFileBody m_newGeometrySetFileBody;
    UINT m_unGeometryNum;//几何体个数
    string m_strFileName;//文件名称
    size_t m_FileSize;//文件大小
    Buffer m_ReadBuf;//读文件缓冲区大小
    Buffer m_WriteBuf;//写文件缓冲区大小
    ObjectSizeList m_objSizeList;//对象的size列表
	char* m_pGeometry;
	int m_nMemGeometryMax;
public:
    void setMemGeometryMax(int MemMax);

	//void  setMemGeometry();
	bool  setMemGeometry();

    void  deleteMemGeometry();

	void  setMemGeometryClear();

	GeometryList& getGeometryList();
	//********************************************************************************** 
    //** 函数名称： getFileSize
    //** 功能描述： 得到文件大小
    //** 输    入： 无         
    //** 输    出： 无
    //** 返回值：文件大小
    //** 作    者： 李静
    //** 创建日期： 2012-04-14
    //**********************************************************************************
	size_t getFileSize();

    //********************************************************************************** 
    //** 函数名称： getObjOffset
    //** 功能描述： 得到几何偏移量
    //** 输    入： GeoID:几何ID号
    //** 输    出： 无
    //** 返回值：指定几何的偏移量
    //** 作    者： 杨灵
    //** 创建日期： 2012-07-04
    //**********************************************************************************
   
    int getObjOffset(UINT64 nGeoID);
	NewGeometryList& CGeometrySetFile:: readnew(ifstream& f);
	NewGeometryList& getNewGeometryList();
};
