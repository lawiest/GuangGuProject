#pragma once 
#include "SqliteHelp.h"

#include <string>
#include <vector>
#include "Tool/StringTable.h"
#include <Base/Service.h>
#include <Model/Buffer.h>
#include <map>
#include <osg/Referenced>
#define TILE_TABLE_NAME "TileTable"
using namespace std;
typedef unsigned long long ULONGLONG;
typedef long long LONGLONG;

class CCacheSqlite:public CSqliteDB,public CSqliteLock
{
	long m_CommitCount;
	long m_nMaxCommit;
public:
	CCacheSqlite();
	~CCacheSqlite();
	void FlushCommentOne();
	void StartCommitOne();
	void EndCommitOne();
};
//使用sqlite实现的TileCache
class CSqliteTileCache
{ 
	CCacheSqlite m_pDB;
	CSafeStmt<>  m_stmtWrite;
	CSafeStmt<>  m_stmtRead;
	CSafeStmt<>  m_stmtAddTick;
	CSafeStmt<>  m_stmtIsDataExist;
	std::string m_strPath;
	__int64    m_nMaxPage;

	std::string	  m_strDataBaseName;
	bool m_bIsOpen;
public:
	CSqliteTileCache(void);
	virtual ~CSqliteTileCache(void);

	//__int64 PageSize();
	//bool Open(const char* pPath);
	//void Close();
	//__int64 MaxCapability();
	//void MaxCapability(__int64  nValue);
	//bool StatState(__int64& nTotal,__int64 &nCount);
	//bool DeleteUntil(__int64 nMaxBytes,__int64 nTotalBytes,__int64 nCountRs);
	//bool DeleteUntil();
	//bool DeleteInvalidData();
	//bool Assigin(CMemIndex* pOut);
	//__int64 UsedPages();
	////增加一个数据的tick
	//bool AddTick(TileDataKey key,time_t time);
	////提交
	//void Flush();

	////写入一个缓存。
	//bool WriteCache(TileDataKey key,time_t starttime,time_t endtime,long nLifecycle, const unsigned char* buff,long nLen);
	////读取一个Cache
	//bool ReadCache(TileDataKey key,time_t time,time_t &starttime,time_t& endtime,CGrowBuffer& buff);

	////清空缓存
	//bool DeleteCache();

	////删除整个数据集
	//bool DeleteCache(unsigned __int64 nDSKey);
	//
	////删除单个
	//bool DeleteCache(TileDataKey key);
	//
	////删除数据集中某个级别范围的
	//bool DeleteCache(unsigned  __int64 nDSKey,long nStartLevel,long nEndLevel);
	//
	////删除数据集中某个级别范围的
	//bool DeleteCache(unsigned __int64 nDSKey,long nLevel,long nMinRow,long nMinCol,long nMaxRow,long nMaxCol);

	////写入元数据
	//bool WriteMeta(unsigned __int64 nDSKey,const char* strKey,const unsigned char* pValue,long nLen,long nType);
	////读取元数据
	//bool ReadMeta(unsigned __int64 nDSKey,const char* strKey,CGrowBuffer& buff,long& nType);

	void Close();

	void DataBaseName(const std::string& strDB);
	const std::string& DataBaseName();

	bool OpenDataBase(const char *DBPath);
	bool CreateTileValueTable(const char *TableName);

	bool ReadTileMeta(TileMetaData& metadata);
	bool CreateTileMetaTable(const char* TableName);
	bool InsertTileValueDataBatch(const char *TableName, int lev,int c,int r, const unsigned char *pcData, int nDataLen,int dataType);
	bool InsertTileValueDataBatch(const char *TableName, int lev,int c,int r, const unsigned char *pcData, int nDataLen);
	bool GetTileDataByTileKey(int level,int col,int row, char **ppTileBuff,int& nSize,int& dataType);
	bool GetTileDataByTileKey(std::string tablename,int level,int col,int row, char **ppTileBuff,int& nSize,int& dataType);

	bool IsTileDataExist(int level,int col,int row);
	bool IsTileDataExist(const char *TableName,int level,int col,int row);
};
