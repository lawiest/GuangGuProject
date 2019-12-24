#include "SqliteTileCache.h"
#include "time.h"
#include  <io.h>

CCacheSqlite::CCacheSqlite():m_CommitCount(0),m_nMaxCommit(50)
{

}
CCacheSqlite::~CCacheSqlite()
{
	if(m_CommitCount != 0)
		Commit();
}
void CCacheSqlite::FlushCommentOne()
{
	if(m_CommitCount ==0)
		return;
	Commit();
	m_CommitCount = 0;
} 
void CCacheSqlite::EndCommitOne()
{
	if(m_CommitCount >m_nMaxCommit)
	{
		Commit();
		m_CommitCount = 0;
	}
}
void CCacheSqlite::StartCommitOne()
{
	LOCK_ME;
	if(m_CommitCount ==0)
		Begin();
	m_CommitCount++;
}

CSqliteTileCache::CSqliteTileCache(void) 
{
	m_nMaxPage  =-1;
	m_bIsOpen = false;
}

CSqliteTileCache::~CSqliteTileCache(void)
{
	Close();
}
void CSqliteTileCache::Close()
{
	m_stmtWrite.Close();
	m_stmtRead.Close();
	m_stmtAddTick.Close();
	m_pDB.Close();
}

void CSqliteTileCache::DataBaseName(const std::string& strDB)
{
	m_strDataBaseName = strDB;
}
const std::string& CSqliteTileCache::DataBaseName()
{
	return m_strDataBaseName;
}

bool CSqliteTileCache::OpenDataBase(const char *DBPath)
{
	if (! DBPath) 
		return false;

	m_strPath = DBPath;
	sqlite3* db = NULL;
	sqlite3_open(m_strPath.c_str(),&db);
	if(NULL == db)
		return false;

	m_pDB.Attach(db);
	m_bIsOpen = true;

	return true;
}

bool CSqliteTileCache::CreateTileValueTable(const char *TableName)
{
	if (! TableName) 
		return false;

	if (! m_pDB.DB()) 
		return false;

	std::string strName(TableName);
	std::string strSQLHead = "create table ";
	std::string strSQLTail = " (Level integer,Row integer,Col integer,tiletype integer,tiledata blob,tilesize integer,primary key(Level,Row,Col))";
	std::string strSQL = strSQLHead + strName + strSQLTail;

	if (m_pDB.Excute(strSQL.c_str()))
		return true;

	return false;
}

bool CSqliteTileCache::ReadTileMeta(TileMetaData& metadata)
{
	if (! m_pDB.DB()) 
		return false;

	std::string sql;
	sql="select itemdata from TileMeta where itemname='ValidTopLevel'";
	sqlite3_stmt* stmt;

	if (SQLITE_OK==sqlite3_prepare(m_pDB.DB(),sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			metadata.toplevel=sqlite3_column_int(stmt,0)-1;
		}
		if(metadata.toplevel<0)
		{
			metadata.toplevel=0;
		}
	}

	sql="select itemdata from TileMeta where itemname='ValidBottomLevel'";
	if (SQLITE_OK==sqlite3_prepare(m_pDB.DB(),sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			metadata.bottomlevel=sqlite3_column_int(stmt,0)-1;
		}
		if(metadata.bottomlevel<0)
		{
			metadata.bottomlevel=0;
		}
	}

	sql="select itemdata from TileMeta where itemname='West'";
	if (SQLITE_OK==sqlite3_prepare(m_pDB.DB(),sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			metadata.bbox.west=sqlite3_column_double(stmt,0);
		}
	}

	sql="select itemdata from TileMeta where itemname='East'";
	if (SQLITE_OK==sqlite3_prepare(m_pDB.DB(),sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			metadata.bbox.east=sqlite3_column_double(stmt,0);
		}
	}


	sql="select itemdata from TileMeta where itemname='South'";
	if (SQLITE_OK==sqlite3_prepare(m_pDB.DB(),sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			metadata.bbox.south=sqlite3_column_double(stmt,0);
		}
	}

	sql="select itemdata from TileMeta where itemname='North'";
	if (SQLITE_OK==sqlite3_prepare(m_pDB.DB(),sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			metadata.bbox.north=sqlite3_column_double(stmt,0);
		}
	}
	sqlite3_finalize(stmt);
	return true;
}

bool CSqliteTileCache::CreateTileMetaTable(const char* TableName)
{
	if (! TableName) 
		return false;
	if (! m_pDB.DB()) 
		return false;

	std::string strName(TableName);
	std::string strSQLHead = "create table ";
	std::string strSQLTail = "(itemname text,itemdata blob,itemtype integer,itemid integer,primary key(itemname))";
	std::string strSQL = strSQLHead + strName + strSQLTail;
	if (m_pDB.Excute(strSQL.c_str()))
		return true;

	return false;
}

bool CSqliteTileCache::InsertTileValueDataBatch(const char *TableName, int lev,int c,int r, const unsigned char *pcData, int nDataLen,int dataType)
{
	if ((! pcData) || nDataLen <= 0 || ! TableName || ! m_pDB.DB())
		return false;

	LOCK_IT(m_stmtWrite);
	m_pDB.StartCommitOne();
	if(m_stmtWrite.IsNull())
	{
		//sqlite3_stmt *pStmtBegin = 0;
		//std::string BeginSQL = "BEGIN TRANSACTION";
		//if (SQLITE_OK != sqlite3_prepare(m_pDB.DB(), BeginSQL.c_str(), BeginSQL.size(), &pStmtBegin, 0))
		//{
		//	return false;
		//}
		//if (sqlite3_step(pStmtBegin) != SQLITE_DONE)
		//{
		//	sqlite3_finalize(pStmtBegin);
		//	return false;
		//}
		//sqlite3_finalize(pStmtBegin);

		std::string strName(TableName);
		std::string strSQL = "insert into " + strName + " values(?,?,?,?,?,?)";
		m_stmtWrite.Prepare(m_pDB.DB(),strSQL.c_str());
	}
	m_stmtWrite.Reset();
	m_stmtWrite.Bind(1,lev);
	m_stmtWrite.Bind(2,r);
	m_stmtWrite.Bind(3,c);
	m_stmtWrite.Bind(4,dataType);
	m_stmtWrite.Bind(6,nDataLen);
	m_stmtWrite.Bind(5,pcData,nDataLen);

	//如果不失败则成功
	bool bOK = !m_pDB.Error(m_stmtWrite.Step()); 
	m_pDB.EndCommitOne();

	return bOK;
}

bool CSqliteTileCache::InsertTileValueDataBatch(const char *TableName, int lev,int c,int r, const unsigned char *pcData, int nDataLen)
{
	if ((! pcData) || nDataLen <= 0 || ! TableName || ! m_pDB.DB())
		return false;

	LOCK_IT(m_stmtWrite);
	m_pDB.StartCommitOne();
	if(m_stmtWrite.IsNull())
	{
		/*sqlite3_stmt *pStmtBegin = 0;
		std::string BeginSQL = "BEGIN TRANSACTION";
		if (SQLITE_OK != sqlite3_prepare(m_pDB.DB(), BeginSQL.c_str(), BeginSQL.size(), &pStmtBegin, 0))
		{
		return false;
		}
		if (sqlite3_step(pStmtBegin) != SQLITE_DONE)
		{
		sqlite3_finalize(pStmtBegin);
		return false;
		}
		sqlite3_finalize(pStmtBegin);*/

		std::string strName(TableName);
		std::string strSQL = "insert into " + strName + " values(?,?,?,?,?,?)";
		m_stmtWrite.Prepare(m_pDB.DB(),strSQL.c_str());
	}
	m_stmtWrite.Reset();

	m_stmtWrite.Bind(1,lev);
	m_stmtWrite.Bind(2,r);
	m_stmtWrite.Bind(3,c);
	m_stmtWrite.Bind(4,101);
	m_stmtWrite.Bind(6,nDataLen);
	m_stmtWrite.Bind(5,pcData,nDataLen);

	//如果不失败则成功
	bool bOK = !m_pDB.Error(m_stmtWrite.Step()); 
	m_pDB.EndCommitOne();

	return bOK;
}

bool CSqliteTileCache::GetTileDataByTileKey(int level,int col,int row, char **ppTileBuff,int& nSize,int& dataType)
{
	LOCK_IT(m_stmtRead);
	if(m_stmtRead.IsNull())
	{
		std::string strName(TILE_TABLE_NAME);
		std::string strSQL = "SELECT tiledata,tilesize,tiletype FROM " +  strName + " where level = ? and row=? and col=?";
		m_stmtRead.Prepare(m_pDB.DB(),strSQL.c_str());
	}
	m_stmtRead.Reset();
	//primary key
	m_stmtRead.Bind(1, level);
	m_stmtRead.Bind(2, row);
	m_stmtRead.Bind(3, col);

	//如果没有记录的话则返回false。
	if(SQLITE_DONE == m_stmtRead.Step())
		return false;

	//分配空间获得数据
	dataType = m_stmtRead.IntValue(2);
	nSize = m_stmtRead.IntValue(1);
	char* data=(char*)m_stmtRead.BlobValue(0);
	*ppTileBuff = new char[nSize];
	memcpy(*ppTileBuff,data,nSize);
	return true;
}

bool CSqliteTileCache::GetTileDataByTileKey(std::string tablename,int level,int col,int row, char **ppTileBuff,int& nSize,int& dataType)
{
	LOCK_IT(m_stmtRead);
	if(m_stmtRead.IsNull())
	{
		std::string strName=tablename;
		std::string strSQL = "SELECT tiledata,tilesize,tiletype FROM " +  strName + " where level = ? and row=? and col=?";
		m_stmtRead.Prepare(m_pDB.DB(),strSQL.c_str());
	}
	m_stmtRead.Reset();
	//primary key
	m_stmtRead.Bind(1, level);
	m_stmtRead.Bind(2, row);
	m_stmtRead.Bind(3, col);
	//如果没有记录的话则返回false。
	if(SQLITE_DONE == m_stmtRead.Step())
		return false;

	//分配空间获得数据
	dataType = m_stmtRead.IntValue(2);
	nSize = m_stmtRead.IntValue(1);
	char* data=(char*)m_stmtRead.BlobValue(0);
	*ppTileBuff = new char[nSize];
	memcpy(*ppTileBuff,data,nSize);
	return true;
}

bool CSqliteTileCache::IsTileDataExist(int level,int col,int row)
{
	if (! m_pDB.DB()) 
		return false;

	LOCK_IT(m_stmtIsDataExist);
	if(m_stmtIsDataExist.IsNull())
	{
		std::string strName(TILE_TABLE_NAME);
		std::string strSQL = "SELECT tiledata,tilesize,tiletype FROM " +  strName + " where level = ? and row=? and col=?";
		m_stmtIsDataExist.Prepare(m_pDB.DB(),strSQL.c_str());
	}
	m_stmtIsDataExist.Reset();
	//primary key
	m_stmtIsDataExist.Bind(1, level);
	m_stmtIsDataExist.Bind(2, row);
	m_stmtIsDataExist.Bind(3, col);

	//如果没有记录的话则返回false。
	if(SQLITE_DONE == m_stmtIsDataExist.Step())
		return false;

	int count = m_stmtIsDataExist.IntValue(0);

	if(count>0)
		return true;

	return false;
}
bool CSqliteTileCache::IsTileDataExist(const char *TableName,int level,int col,int row)
{
	if (! m_pDB.DB()) 
		return false;

	LOCK_IT(m_stmtIsDataExist);
	if(m_stmtIsDataExist.IsNull())
	{
		std::string strName(TableName);
		std::string strSQL = "SELECT COUNT(*) FROM " +  strName + " where level = ? and row=? and col=?";
		m_stmtIsDataExist.Prepare(m_pDB.DB(),strSQL.c_str());
	}
	m_stmtIsDataExist.Reset();
	//primary key
	m_stmtIsDataExist.Bind(1, level);
	m_stmtIsDataExist.Bind(2, row);
	m_stmtIsDataExist.Bind(3, col);

	//如果没有记录的话则返回false。
	if(SQLITE_DONE != m_stmtIsDataExist.Step())
		return false;

	int count = m_stmtIsDataExist.IntValue(0);

	if(count>0)
		return true;

	return false;
}