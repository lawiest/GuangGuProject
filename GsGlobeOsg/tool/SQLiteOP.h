#pragma once


#include <string>
#include <vector>
#include <sqlite3.h>
//#include "sqlite3.h"
#include "Tool/StringTable.h"
#include <Base/SERVICE.h>
#include <Model/Buffer.h>
#include <map>
//#include <Model/Feature.h>
#include <osg/Referenced>
#define TILE_TABLE_NAME "TileTable"

#ifndef _TOOL_CSQLiteOP_H_
#define _TOOL_CSQLiteOP_H_

//#include "SafeArrayVariant.h" 
using namespace std;

typedef unsigned long long ULONGLONG;
typedef long long LONGLONG;

//typedef std::map<ULONGLONG,osg::ref_ptr<CFeature>> FEATURE_MAP;
//typedef char VARIANT;
class CSQLiteOP;

struct ST_RELMeat
{
	ST_RELMeat()
	{
		strFCName = "";
		nFCKey = 0;
	}
	string strFCName;
	ULONGLONG nFCKey;
};

class CSQLiteRecordSet
{
public:
	CSQLiteRecordSet()
	{
		m_nIndex = 0;
	}

	~CSQLiteRecordSet()
	{
		for (unsigned int i=0; i<m_vtRecord.size(); i++)
		{
			m_vtRecord[i].free();
		}
		m_vtRecord.clear();
	}

	friend class CSQLiteOP;

public:
	char *MoveNext(ULONGLONG &llKey,int &nDataLen)
	{
		if (m_nIndex < m_vtRecord.size() -1)
		{
			llKey = m_vtRecord[m_nIndex].llKey;
			nDataLen = m_vtRecord[m_nIndex].nDataLen;
			m_nIndex++;
			return m_vtRecord[m_nIndex].pDBData;
		}

		return 0;
	}

	int GetRecordCount()
	{
		return m_vtRecord.size();
	}

protected:
	struct ST_DBData
	{
		ULONGLONG llKey;
		char *    pDBData;
		int       nDataLen;

		ST_DBData(){llKey=0; pDBData=0; nDataLen=0;}
		void free(){if(pDBData){delete []pDBData; pDBData=0;}}
	};

protected:
	std::vector<ST_DBData> m_vtRecord;
	unsigned int m_nIndex;

	void InsertKeyValue(ULONGLONG llKey, char *pData, int nLen)
	{
		//if (! pData || nLen <= 0) return;

		char *pselfData = new char[nLen];
		memcpy(pselfData, pData, nLen);
		ST_DBData oData;
		oData.llKey = llKey;
		oData.pDBData = pselfData;
		oData.nDataLen = nLen;
		m_vtRecord.push_back(oData);
	}
};

class CSQLiteOP:public osg::Referenced
{
public:
	CSQLiteOP(void);
	~CSQLiteOP(void);

public:
	void DataBaseName(const std::string& strDB);
	const std::string& DataBaseName();

	bool OpenDataBase(const char *DBPath);
	bool ReadGeometryTypeFromFCS(int& nGeometry);
	bool CreateKeyValueTable(const char *TableName);
	bool CreateTileValueTable(const char *TableName);
	bool CreateKeyLodTable(const char *TableName);
	/////////////////////////////image and terrain
	bool ReadTileMeta(TileMetaData& metadata);
	bool ReadTileName(string& strTileName);
	bool ReadTileRangeFromFCS(TileMetaData& metadata);

	bool CreateTileMetaTable(const char* TableName);
	bool InsertTileValueDataBatch(const char *TableName, int lev,int c,int r, const char *pcData, int nDataLen,int dataType);
	bool GetTileDataByTileKey(int level,int col,int row, char **ppTileBuff,int& nSize,int& dataType);
	bool GetTileDataByTileKey(std::string tablename,int level,int col,int row, char **ppTileBuff,int& nSize,int& dataType);
	bool GetPlaceNameIconDataByID(int dataID,char** data,int& dataType,int& dataSize);
	///////////////////////////////


	bool InsertKeyValueData(const char *TableName, ULONGLONG llKey, const char *pcData, int nDataLen);
	bool InsertTileValueData(const char *TableName,int lev,int c,int r, const char *pcData, int nDataLen);
	bool InsertKeyLodData(const char *TableName, ULONGLONG llKey, int lod);
	void QueryKeyValueTable(const char *TableName, CSQLiteRecordSet **ppRecordSet);
	bool QuerySingleKeyValueTable(const char *TableName, ULONGLONG llKey, int &nDataLen, char **ppcData);
	bool UpdateSingleKeyValueTable(const char *TableName, ULONGLONG llKey, const char *pNewData, int nDataLen);
	bool DeleteSingleKeyValueTable(const char *TableName, ULONGLONG llKey);
	bool DropTable(const char *TableName);
	void CloseDataBase();
	bool IsTableExist(const char *TableName);
	bool IsTileDataExist(int level,int col,int row);
	bool IsTileDataExist(const char *TableName,int level,int col,int row);
	bool IsModelExist(ULONGLONG& fcKey,int& lod);
	bool IsKeyValueExist(const char *TableName,ULONGLONG& key);
	
	bool CreateFeatureRelationTable();
	bool InsertFeatureRelationTable(ULONGLONG uFeatureKey, ULONGLONG uFeatureClassKey, const char *FeatureClassAttName, int nNameLen);
	bool InsertKeyValueDataBatch(const char *TableName, ULONGLONG llKey, const char *pcData, int nDataLen);
	bool InsertTileValueDataBatch(const char *TableName, int lev,int c,int r, const char *pcData, int nDataLen);

	bool CommitCache();
	std::string GetFeatureClassNameByKey(const char *TableName,ULONGLONG llKey);
	std::string GetFeatureClassNameByKeyOld(const char *TableName,ULONGLONG llKey);
	//��ȡ���ݣ�ͬʱ����У������Ϣ��ǰ6�ֽڰ汾�ţ���9�ֽڽ����������ݴ�С
	bool QuerySingleKeyValueTable(const char *TableName, ULONGLONG llKey, const char* strVersion, int nVersionSize, char* varData);

	bool IsOpenState()
	{
 		return m_bIsOpen;
	}
	bool QueryDataFromRELTable(const char *TableName,std::vector<ST_RELMeat> &vecRELMeta);
	bool GetOIDsFromFC(const char *TableName,std::vector<int> &vecOIDs);
	bool GetFeatureKeysFromFC(const char *TableName,std::vector<ULONGLONG> &vecFKeys);
	bool GetKeysFromKeyValueTable(const char *TableName,std::vector<ULONGLONG> &vecKeys);
	bool GetLodByFeatureKey(ULONGLONG FeatureKey,Buffer& buffer);
	bool GetGeometryByGeoKey(ULONGLONG GeoKey, Buffer& buffer);
	bool GetTextureByTexKey(ULONGLONG TexKepy, Buffer& buffer);
	bool GetTileDataByTileKey(int level,int col,int row, char **ppTileBuff,int& nSize);
	//bool GetTileByTileKey(int level,int row,int col,Buffer& texBuffer);
	//void Buffer2Features(FEATURE_MAP &vtFeatures, char *pData, int nSize);

	sqlite3 *m_pSqliteDB;
private:
	sqlite3_stmt *m_pInsertStmt;
	bool          m_bFirstInsert;
	
	//CGrowBuffer	  m_growbuff;
	std::string	  m_strDataBaseName;
	bool m_bIsOpen;

	OpenThreads::Mutex   _commitMutex;
//	CGrowBuffer	  m_growbuff;
};
#endif
