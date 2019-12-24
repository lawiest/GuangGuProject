
//#include <Tool/SQLiteOP.h>
//#include <Model/ModelDef.h>
#include "SQLiteOP.h"
#include <zlib.h>

//#include "CharSet.h"



CSQLiteOP::CSQLiteOP(void)
{
	m_pSqliteDB = 0;
	m_bFirstInsert = true;
	m_pInsertStmt = 0;
	m_bIsOpen = false;
	//setThreadSafeReferenceCounting(true);
//	m_growbuff.allocbuff(1*1024*1024);
	
}

CSQLiteOP::~CSQLiteOP(void)
{ 
//	m_growbuff.clear();
	if (m_pSqliteDB)
		CloseDataBase();
}

void CSQLiteOP::DataBaseName(const std::string& strDB)
{
	m_strDataBaseName = strDB;
}
const std::string& CSQLiteOP::DataBaseName()
{
	return m_strDataBaseName;
}

bool CSQLiteOP::OpenDataBase(const char *DBPath)
{
	if (! DBPath) return false;
	//m_strMetaFile = CCharSet::Unicode_2_UTF8(_bstr_t(db),db.Length());
//	CComBSTR tempbstr = DBPath;
//	string strDB = CCharSet::Unicode_2_UTF8(tempbstr);
	std::string strDB=DBPath;
	if (SQLITE_OK == sqlite3_open(strDB.c_str(), &m_pSqliteDB))
	{
		m_bIsOpen = true;
		return true;
	}
	else
	{
		return false;
	}
}

bool CSQLiteOP::CreateKeyValueTable(const char *TableName)
{
	if (! TableName) return false;
	if (! m_pSqliteDB) return false;
	
	std::string strName(TableName);
		std::string strSQLHead = "create table ";
		std::string strSQLTail = " (Key INTEGER(64) PRIMARY KEY, BinValue BLOB, DataSize INTEGER(64))";
		std::string strSQL = strSQLHead + strName + strSQLTail;
		if (SQLITE_OK == sqlite3_exec(m_pSqliteDB, strSQL.c_str(), 0, 0, 0))
	{
		strSQLHead = "create index keyindex on ";
		strSQLTail = " (Key)";
		strSQL = strSQLHead + strName + strSQLTail;
		return (SQLITE_OK == sqlite3_exec(m_pSqliteDB, strSQL.c_str(), 0, 0, 0));
	}
		
	else
		return false;
}

bool CSQLiteOP::CreateKeyLodTable(const char *TableName)
{
	if (! TableName) return false;
	if (! m_pSqliteDB) return false;

	std::string strName(TableName);
	std::string strSQLHead = "create table ";
	std::string strSQLTail = " (Key INTEGER(64), Lod integer, PRIMARY KEY(Key,Lod))";
	std::string strSQL = strSQLHead + strName + strSQLTail;
	if (SQLITE_OK == sqlite3_exec(m_pSqliteDB, strSQL.c_str(), 0, 0, 0))
	{
	/*	strSQLHead = "create index keyindex on ";
		strSQLTail = " (Key)";
		strSQL = strSQLHead + strName + strSQLTail;*/
		return true/*(SQLITE_OK == sqlite3_exec(m_pSqliteDB, strSQL.c_str(), 0, 0, 0))*/;
	}

	else
		return false;
}
//////////////////////////image and terrain
bool CSQLiteOP::CreateTileMetaTable(const char* TableName)
{
	if (! TableName) return false;
	if (! m_pSqliteDB) return false;

	std::string strName(TableName);
	std::string strSQLHead = "create table ";
	std::string strSQLTail = "(itemname text,itemdata blob,itemtype integer,itemid integer,primary key(itemname))";
	std::string strSQL = strSQLHead + strName + strSQLTail;
	if (SQLITE_OK == sqlite3_exec(m_pSqliteDB, strSQL.c_str(), 0, 0, 0))
	{
		return true;
	}
	else
		return false;

}
///////////////////////////////
bool CSQLiteOP::CreateTileValueTable(const char *TableName)
{
	if (! TableName) return false;
	if (! m_pSqliteDB) return false;

	std::string strName(TableName);
	std::string strSQLHead = "create table ";
	std::string strSQLTail = " (Level integer,Row integer,Col integer,tiletype integer,tiledata blob,tilesize integer,primary key(Level,Row,Col))";
	std::string strSQL = strSQLHead + strName + strSQLTail;
	if (SQLITE_OK == sqlite3_exec(m_pSqliteDB, strSQL.c_str(), 0, 0, 0))
	{
		return true;
		/*strSQLHead = "create index keyindex on ";
		strSQLTail = " (Key)";
		strSQL = strSQLHead + strName + strSQLTail;
		return (SQLITE_OK == sqlite3_exec(m_pSqliteDB, strSQL.c_str(), 0, 0, 0));*/
	}

	else
		return false;
}

bool CSQLiteOP::CreateFeatureRelationTable()
{
	if (! m_pSqliteDB) return false;
	
	std::string strName(RELATION_TABLE_NAME);
	std::string strSQLHead = "create table ";
	std::string strField1 = FEATURE_KEY_NAME;
	std::string strField2 = FEATURECLASS_KEY_NAME;
	std::string strField3 = FEATURECLASS_ATTRIBUTE_TABLE_NAME;
	/*std::string strSQLTail = " (" + strField1 + " INTEGER(64), " + strField2 + " INTEGER(64), " + strField3 + " TEXT)";*/
	std::string strSQLTail = " (" + strField2 + " INTEGER(64)PRIMARY KEY, " + strField3 + " TEXT)";
	std::string strSQL = strSQLHead + strName + strSQLTail;
	if (SQLITE_OK == sqlite3_exec(m_pSqliteDB, strSQL.c_str(), 0, 0, 0))
		return true;
	else
		return false;
}

bool CSQLiteOP::DropTable(const char *TableName)
{
	if (! TableName) return false;
	if (! m_pSqliteDB) return false;

	std::string strName(TableName);
	std::string strSQL = "drop table ";
	strSQL = strSQL + strName;
	if (SQLITE_OK == sqlite3_exec(m_pSqliteDB, strSQL.c_str(), 0, 0, 0))
		return true;
	else
		return false;
}


bool CSQLiteOP::InsertKeyValueData(const char *TableName, ULONGLONG llKey, const char *pcData, int nDataLen)
{
	if ((! pcData) || nDataLen <= 0 || ! TableName || ! m_pSqliteDB)
		return false;
	
	std::string strName(TableName);
	std::string strSQL = "insert into " + strName + " (Key, BinValue, DataSize) values (?, ?, ?)";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;
	
	sqlite3_bind_int64(pStmt, 1, (LONGLONG)llKey);
	sqlite3_bind_blob(pStmt, 2, pcData, nDataLen, 0);
	sqlite3_bind_int64(pStmt, 3, (LONGLONG)nDataLen);
	if (SQLITE_DONE != sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	sqlite3_finalize(pStmt);
	return true;
}

bool CSQLiteOP::InsertKeyLodData(const char *TableName, ULONGLONG llKey, int lod)
{


	std::string strName(TableName);
	std::string strSQL = "insert into " + strName + " (Key, Lod) values (?, ?)";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;

	sqlite3_bind_int64(pStmt, 1, (LONGLONG)llKey);
	sqlite3_bind_int(pStmt, 2, lod);
	if (SQLITE_DONE != sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	sqlite3_finalize(pStmt);
	return true;
}
bool CSQLiteOP::InsertTileValueData(const char *TableName,int lev,int c,int r, const char *pcData, int nDataLen)
{
	if ((! pcData) || nDataLen <= 0 || ! TableName || ! m_pSqliteDB)
		return false;

	std::string strName(TableName);
	std::string strSQL = "insert into " + strName + " values(?,?,?,?,?,?)";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;

	sqlite3_bind_int(pStmt,1,lev);
	sqlite3_bind_int(pStmt,2,r);
	sqlite3_bind_int(pStmt,3,c);
	sqlite3_bind_int(pStmt,4,101);
	sqlite3_bind_int(pStmt,6,nDataLen);
	sqlite3_bind_blob(pStmt,5,pcData,nDataLen,NULL);
	//sqlite3_step(stmt);
	if (SQLITE_DONE != sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	sqlite3_finalize(pStmt);
	return true;
}

bool CSQLiteOP::InsertFeatureRelationTable(ULONGLONG uFeatureKey, ULONGLONG uFeatureClassKey, const char *FeatureClassAttName, int nNameLen)
{
	if (! FeatureClassAttName) return false;

	std::string strName(RELATION_TABLE_NAME);
	/*std::string strSQL = "insert into " + strName + " values (?, ?, ?)";*/
	std::string strSQL = "insert into " + strName + " values (?, ?)";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;
	
	/*sqlite3_bind_int64(pStmt, 1, (LONGLONG)uFeatureKey);
	sqlite3_bind_int64(pStmt, 2, (LONGLONG)uFeatureClassKey);
	sqlite3_bind_text(pStmt, 3, FeatureClassAttName, nNameLen, 0);*/
	sqlite3_bind_int64(pStmt, 1, (LONGLONG)uFeatureClassKey);
	sqlite3_bind_text(pStmt, 2, FeatureClassAttName, nNameLen, 0);

	if (SQLITE_OK != sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	sqlite3_finalize(pStmt);
	return true;
}

//�����������ݣ��������Ƿ��ύ
bool CSQLiteOP::InsertKeyValueDataBatch(const char *TableName, ULONGLONG llKey, const char *pcData, int nDataLen)
{
	
	if ((! pcData) || nDataLen <= 0 || ! TableName || ! m_pSqliteDB)
		return false;

	//��һ��ʱ��ʼһ������
	if(m_bFirstInsert)
	{
		sqlite3_stmt *pStmtBegin = 0;
		std::string BeginSQL = "BEGIN TRANSACTION";
		if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, BeginSQL.c_str(), BeginSQL.size(), &pStmtBegin, 0))
		{
			return false;
		}
		if (sqlite3_step(pStmtBegin) != SQLITE_DONE)
		{
			sqlite3_finalize(pStmtBegin);
			return false;
		}
		sqlite3_finalize(pStmtBegin);

		//��һ��ʱ��ʼ�������ַ���
		m_pInsertStmt = 0;
		m_bFirstInsert = false;
		std::string strName(TableName);
		std::string strSQL = "insert into " + strName + " (Key, BinValue, DataSize) values (?, ?, ?)";
		if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &m_pInsertStmt, 0))
		{
			if(m_pInsertStmt)
			{
				sqlite3_finalize(m_pInsertStmt);
			}
			return false;
		}
	}

	//��������
	ULONGLONG tempKey = llKey;
	sqlite3_bind_int64(m_pInsertStmt, 1, tempKey);
	sqlite3_bind_blob(m_pInsertStmt, 2, pcData, nDataLen, 0);
	sqlite3_bind_int64(m_pInsertStmt, 3, (ULONGLONG)nDataLen);
	if (SQLITE_DONE != sqlite3_step(m_pInsertStmt))
	{
		//sqlite3_finalize(m_pInsertStmt);
		sqlite3_reset(m_pInsertStmt);
		return false;
	}
	sqlite3_reset(m_pInsertStmt);

	return true;
}

bool CSQLiteOP::InsertTileValueDataBatch(const char *TableName, int lev,int c,int r, const char *pcData, int nDataLen)
{
	
	if ((! pcData) || nDataLen <= 0 || ! TableName || ! m_pSqliteDB)
		return false;

	//��һ��ʱ��ʼһ������
	if(m_bFirstInsert)
	{
		sqlite3_stmt *pStmtBegin = 0;
		std::string BeginSQL = "BEGIN TRANSACTION";
		if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, BeginSQL.c_str(), BeginSQL.size(), &pStmtBegin, 0))
		{
			return false;
		}
		if (sqlite3_step(pStmtBegin) != SQLITE_DONE)
		{
			sqlite3_finalize(pStmtBegin);
			return false;
		}
		sqlite3_finalize(pStmtBegin);

		//��һ��ʱ��ʼ�������ַ���
		m_pInsertStmt = 0;
		m_bFirstInsert = false;
		std::string strName(TableName);
		//std::string strSQL = "insert into " + strName + " (Key, BinValue, DataSize) values (?, ?, ?)";
		std::string strSQL = "insert into " + strName + " values(?,?,?,?,?,?)";
		if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &m_pInsertStmt, 0))
		{
			if(m_pInsertStmt)
			{
				sqlite3_finalize(m_pInsertStmt);
			}
			return false;
		}
	}

	//��������
	//ULONGLONG tempKey = llKey;
	//sqlite3_bind_int64(m_pInsertStmt, 1, tempKey);
	//sqlite3_bind_blob(m_pInsertStmt, 2, pcData, nDataLen, 0);
	//sqlite3_bind_int64(m_pInsertStmt, 3, (ULONGLONG)nDataLen);
	//if(m_pInsertStmt==NULL)
	//	return

	sqlite3_bind_int(m_pInsertStmt,1,lev);
	sqlite3_bind_int(m_pInsertStmt,2,r);
	sqlite3_bind_int(m_pInsertStmt,3,c);
	sqlite3_bind_int(m_pInsertStmt,4,101);
	sqlite3_bind_int(m_pInsertStmt,6,nDataLen);
	sqlite3_bind_blob(m_pInsertStmt,5,pcData,nDataLen,NULL);


	if (SQLITE_DONE != sqlite3_step(m_pInsertStmt))
	{
		//sqlite3_finalize(m_pInsertStmt);
		sqlite3_reset(m_pInsertStmt);
		return false;
	}
	sqlite3_reset(m_pInsertStmt);

	return true;
}

////////////////////////////////////////////image and terrain,dataType

bool CSQLiteOP::InsertTileValueDataBatch(const char *TableName, int lev,int c,int r, const char *pcData, int nDataLen,int dataType)
{

	if ((! pcData) || nDataLen <= 0 || ! TableName || ! m_pSqliteDB)
		return false;

	//��һ��ʱ��ʼһ������
	if(m_bFirstInsert)
	{
		sqlite3_stmt *pStmtBegin = 0;
		std::string BeginSQL = "BEGIN TRANSACTION";
		if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, BeginSQL.c_str(), BeginSQL.size(), &pStmtBegin, 0))
		{
			return false;
		}
		if (sqlite3_step(pStmtBegin) != SQLITE_DONE)
		{
			sqlite3_finalize(pStmtBegin);
			return false;
		}
		sqlite3_finalize(pStmtBegin);

		//��һ��ʱ��ʼ�������ַ���
		m_pInsertStmt = 0;
		m_bFirstInsert = false;
		std::string strName(TableName);
		//std::string strSQL = "insert into " + strName + " (Key, BinValue, DataSize) values (?, ?, ?)";
		std::string strSQL = "insert into " + strName + " values(?,?,?,?,?,?)";
		if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &m_pInsertStmt, 0))
		{
			if(m_pInsertStmt)
			{
				sqlite3_finalize(m_pInsertStmt);
			}
			return false;
		}
	}

	//��������
	//ULONGLONG tempKey = llKey;
	//sqlite3_bind_int64(m_pInsertStmt, 1, tempKey);
	//sqlite3_bind_blob(m_pInsertStmt, 2, pcData, nDataLen, 0);
	//sqlite3_bind_int64(m_pInsertStmt, 3, (ULONGLONG)nDataLen);
	//if(m_pInsertStmt==NULL)
	//	return

	sqlite3_bind_int(m_pInsertStmt,1,lev);
	sqlite3_bind_int(m_pInsertStmt,2,r);
	sqlite3_bind_int(m_pInsertStmt,3,c);
	sqlite3_bind_int(m_pInsertStmt,4,dataType);
	sqlite3_bind_int(m_pInsertStmt,6,nDataLen);
	sqlite3_bind_blob(m_pInsertStmt,5,pcData,nDataLen,NULL);


	if (SQLITE_DONE != sqlite3_step(m_pInsertStmt))
	{
		//sqlite3_finalize(m_pInsertStmt);
		sqlite3_reset(m_pInsertStmt);
		return false;
	}
	sqlite3_reset(m_pInsertStmt);

	return true;
}
///////////////////////////////////////////////////////////////

bool CSQLiteOP::CommitCache()
{
	
	if (m_bFirstInsert || !m_pInsertStmt)
		return false;

	m_bFirstInsert = true;
	sqlite3_finalize(m_pInsertStmt);
	m_pInsertStmt = 0;
	sqlite3_stmt *pStmtCommit = 0;
	std::string commitSQL = "COMMIT";
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, commitSQL.c_str(), commitSQL.size(), &pStmtCommit, 0))
	{
		return false;
	}
	if (sqlite3_step(pStmtCommit) != SQLITE_DONE)
	{
		sqlite3_finalize(pStmtCommit);
		return false;
	}
	sqlite3_finalize(pStmtCommit);

	return true;
}

void CSQLiteOP::CloseDataBase()
{
	sqlite3_close(m_pSqliteDB);
	m_pSqliteDB = 0;
	m_bIsOpen = false;
}

bool CSQLiteOP::IsTableExist(const char *TableName)
{
	if (! TableName) return false;
	if (! m_pSqliteDB) return false;

	std::string strName(TableName);
	std::string strSQL = "SELECT COUNT(*)  as CNT FROM sqlite_master where type='table' and name='" + strName + "'";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	if (SQLITE_ROW != sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	int nRs = (sqlite3_column_int(pStmt, 0) > 0);
	sqlite3_finalize(pStmt);
	return nRs;
}

void CSQLiteOP::QueryKeyValueTable(const char *TableName, CSQLiteRecordSet **ppRecordSet)
{
	if (! TableName) return;

	std::string strName(TableName);
	std::string strSQL = "select * from " +  strName;

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
	{
		sqlite3_finalize(pStmt);
		return;
	}

	*ppRecordSet = new CSQLiteRecordSet;

	while (SQLITE_DONE != sqlite3_step(pStmt))
	{
		ULONGLONG llKey = (ULONGLONG)sqlite3_column_int64(pStmt, 0);
		int nDataLen = sqlite3_column_bytes(pStmt, 1);

		(*ppRecordSet)->InsertKeyValue(llKey, (char *)sqlite3_column_blob(pStmt, 1), nDataLen);
	}

	sqlite3_finalize(pStmt);
}

//��ѯ�µĹ�ϵ��
std::string CSQLiteOP::GetFeatureClassNameByKey(const char *TableName,ULONGLONG llKey)
{
	if (! TableName) return "";

	std::string strName(TableName);
	std::string strSQL = "SELECT * FROM " +  strName + " where FeatureClassKey = ?";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return "";

	sqlite3_bind_int64(pStmt, 1, (LONGLONG)llKey);
	if (SQLITE_ROW != sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return "";
	}
	int nNameLen = sqlite3_column_bytes(pStmt, 1);
	char strTemp[128] = {};
	memcpy(strTemp,sqlite3_column_blob(pStmt,1),nNameLen);
	std::string strResult = strTemp;
	sqlite3_finalize(pStmt);
	return strResult;
}


//��ѯ�ϵĹ�ϵ��
std::string  CSQLiteOP::GetFeatureClassNameByKeyOld(const char *TableName,ULONGLONG llKey)
{
	if (! TableName) return "";

	std::string strName(TableName);
	std::string strSQL = "SELECT * FROM " +  strName + " where FeatureClassKey = ?";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return "";

	sqlite3_bind_int64(pStmt, 1, (LONGLONG)llKey);
	if (SQLITE_ROW != sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return "";
	}
	int nNameLen = sqlite3_column_bytes(pStmt, 2);
	char strTemp[128] = {};
	memcpy(strTemp,sqlite3_column_blob(pStmt,2),nNameLen);
	std::string strResult = strTemp;
	sqlite3_finalize(pStmt);
	return strResult;

}
bool CSQLiteOP::QuerySingleKeyValueTable(const char *TableName, ULONGLONG llKey, int &nDataLen, char **ppcData)
{
	if (! TableName) return false;
	
	*ppcData = 0;

	std::string strName(TableName);
	std::string strSQL = "SELECT * FROM " +  strName + " where Key = ?";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;
	
	sqlite3_bind_int64(pStmt, 1, (LONGLONG)llKey);

	if (SQLITE_DONE == sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}
	
	llKey = (ULONGLONG)sqlite3_column_int64(pStmt, 0);
	nDataLen = sqlite3_column_bytes(pStmt, 1);

	*ppcData = new char[nDataLen];
	memcpy(*ppcData, sqlite3_column_blob(pStmt, 1), nDataLen);

	sqlite3_finalize(pStmt);
	return true;

	/*return (char *)sqlite3_column_blob(pStmt, 1);*/
}



bool CSQLiteOP::QuerySingleKeyValueTable(const char *TableName, ULONGLONG llKey, const char* strVersion, int nVersionSize, char* varData)
{
	if (! TableName) 
		return false;

	long nDataLen = 0;

	std::string strName(TableName);
	std::string strSQL = "SELECT * FROM " +  strName + " where Key = ?";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;

	sqlite3_bind_int64(pStmt, 1, (LONGLONG)llKey);

	if (SQLITE_DONE == sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	llKey = (ULONGLONG)sqlite3_column_int64(pStmt, 0);
	int nRealDataSize = sqlite3_column_bytes(pStmt, 1);

	//У���빲6+9�ֽ�
	nDataLen = nRealDataSize + nVersionSize + 9;
	//CGrowBuffer	  m_growbuff;
	//if (m_growbuff.allocbuff(nDataLen) == NULL)
	//{
	//	sqlite3_finalize(pStmt);
	//	return false;
	//}
	
	int nPos = 0;
	//�汾��
//	memcpy(m_growbuff.buffhead(), strVersion, nVersionSize);
	nPos += nVersionSize;

	//��������
//	memcpy(m_growbuff.buffhead()+nPos, sqlite3_column_blob(pStmt, 1), nRealDataSize);
	nPos += nRealDataSize;

	//������
	unsigned char cEnd = '0';
//	memcpy(m_growbuff.buffhead()+nPos, &cEnd, 1);
	nPos += 1;

	//���ݰ����ܳ���
//	memcpy(m_growbuff.buffhead()+nPos, &nDataLen, 4);
	nPos += 4;

	//�������
	int nCount = 1;
//	memcpy(m_growbuff.buffhead()+nPos, &nCount, 4);

	sqlite3_finalize(pStmt);
	//CSafeArrayVariantPtr varSrcData(m_growbuff.buffhead(), m_growbuff.buffsize());
	//::VariantCopy(varData, &varSrcData);

	return true;
}

bool CSQLiteOP::UpdateSingleKeyValueTable(const char *TableName, ULONGLONG llKey, const char *pNewData, int nDataLen)
{
	if (! TableName || ! pNewData) return false;

	std::string strName(TableName);
	std::string strSQL = "update " + strName + " set BinValue = ? where Key = ?";

	sqlite3_stmt *pStmt = 0;
//	strSQL = CCharSet::MBCSToUTF8(strSQL);
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;
	
	sqlite3_bind_int64(pStmt, 2, (LONGLONG)llKey);
	sqlite3_bind_blob(pStmt, 1, pNewData, nDataLen, 0);
	
	int ret = sqlite3_step(pStmt); 
	if (ret != SQLITE_DONE)
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	sqlite3_finalize(pStmt);
	return true;
}

bool CSQLiteOP::DeleteSingleKeyValueTable(const char *TableName, ULONGLONG llKey)
{
	if (! TableName) return false;

	std::string strName(TableName);
	std::string strSQL = "delete from " + strName + " where Key = ?";

	sqlite3_stmt *pStmt = 0;
	//strSQL = CCharSet::MBCSToUTF8(strSQL);
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;

	sqlite3_bind_int64(pStmt, 1, (LONGLONG)llKey);
	int ret = sqlite3_step(pStmt); 
	if (ret != SQLITE_DONE)
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	sqlite3_finalize(pStmt);
	return true;
}

bool CSQLiteOP::QueryDataFromRELTable(const char *TableName,std::vector<ST_RELMeat> &vecRELMeta)
{
	if (! TableName) return false;

	std::string strName(TableName);
	std::string strSQL = "SELECT * FROM " +  strName;

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;
	while(SQLITE_ROW == sqlite3_step(pStmt))
	{
		ULONGLONG llKey = (ULONGLONG)sqlite3_column_int64(pStmt, 0);
		int nNameLen = sqlite3_column_bytes(pStmt, 1);
		char cTemp[128] = {};
		memcpy(cTemp,sqlite3_column_blob(pStmt,1),nNameLen);
		string strTemp = cTemp;
		ST_RELMeat stRelMeta;
		stRelMeta.nFCKey = llKey;
		stRelMeta.strFCName = strTemp;
		vecRELMeta.push_back(stRelMeta);
	}
	sqlite3_finalize(pStmt);
	return true;
}

bool CSQLiteOP::GetOIDsFromFC(const char *TableName,std::vector<int> &vecOIDs)
{
	if (! TableName) return false;

	std::string strName(TableName);
	std::string strSQL = "SELECT OID FROM " +  strName;

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;
	while(SQLITE_ROW == sqlite3_step(pStmt))
	{
		int nOID = sqlite3_column_int(pStmt,0);
		vecOIDs.push_back(nOID);
	}
	sqlite3_finalize(pStmt);
	return true;

}



bool CSQLiteOP::GetFeatureKeysFromFC(const char *TableName,std::vector<ULONGLONG> &vecFKeys)
{
	if (! TableName) return false;

	std::string strName(TableName);
	std::string strSQL = "SELECT FEATUREKEY FROM " +  strName;

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;
	while(SQLITE_ROW == sqlite3_step(pStmt))
	{
		ULONGLONG llFKey = sqlite3_column_int64(pStmt,0);
		vecFKeys.push_back(llFKey);
	}
	sqlite3_finalize(pStmt);
	return true;

}


bool CSQLiteOP::GetKeysFromKeyValueTable(const char *TableName,std::vector<ULONGLONG> &vecKeys)
{
	if (! TableName) return false;

	std::string strName(TableName);
	std::string strSQL = "SELECT key FROM " +  strName;

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;
	while(SQLITE_ROW == sqlite3_step(pStmt))
	{
		ULONGLONG llKey = (ULONGLONG)sqlite3_column_int64(pStmt, 0);
		vecKeys.push_back(llKey);
	}
	sqlite3_finalize(pStmt);
	return true;
}

bool CSQLiteOP::GetLodByFeatureKey(ULONGLONG FeatureKey,Buffer& buffer)
{

	//if (! TableName) 
	//	return false;

	long nDataLen = 0;

	std::string strName(FEATURECLASS_TABLE_NAME);
	std::string strSQL = "SELECT LOD FROM " + strName  + " where FEATUREKEY = ?";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;

	sqlite3_bind_int64(pStmt, 1, (LONGLONG)FeatureKey);

	if (SQLITE_DONE == sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	unsigned char* varData=(unsigned char*)sqlite3_column_blob(pStmt, 0);
	int length=sqlite3_column_bytes(pStmt, 0);
	//buffer=Buffer(varData,length,true);
	buffer.set(varData+5,length-5,true);
	//FeatureKey = (ULONGLONG)sqlite3_column_int64(pStmt, 0);
	//int nRealDataSize = sqlite3_column_bytes(pStmt, 1);

	//У���빲6+9�ֽ�
	//nDataLen = nRealDataSize + nVersionSize + 9;
	//CGrowBuffer	  m_growbuff;
	//if (m_growbuff.allocbuff(nDataLen) == NULL)
	//{
	//	sqlite3_finalize(pStmt);
	//	return false;
	//}

	//int nPos = 0;
	//�汾��
	//	memcpy(m_growbuff.buffhead(), strVersion, nVersionSize);
	//nPos += nVersionSize;

	//��������
	//	memcpy(m_growbuff.buffhead()+nPos, sqlite3_column_blob(pStmt, 1), nRealDataSize);
	//nPos += nRealDataSize;

	////������
	//unsigned char cEnd = '0';
	////	memcpy(m_growbuff.buffhead()+nPos, &cEnd, 1);
	//nPos += 1;

	////���ݰ����ܳ���
	////	memcpy(m_growbuff.buffhead()+nPos, &nDataLen, 4);
	//nPos += 4;

	////�������
	//int nCount = 1;
	//	memcpy(m_growbuff.buffhead()+nPos, &nCount, 4);

	sqlite3_finalize(pStmt);
	//CSafeArrayVariantPtr varSrcData(m_growbuff.buffhead(), m_growbuff.buffsize());
	//::VariantCopy(varData, &varSrcData);

	return true;

}

bool CSQLiteOP::GetGeometryByGeoKey(ULONGLONG llKey, Buffer& geoBuffer)
{


	long nDataLen = 0;

	std::string strName(GEOMETRY_TABLE_NAME);
	std::string strSQL = "SELECT * FROM " +  strName + " where Key = ?";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;

	sqlite3_bind_int64(pStmt, 1, (LONGLONG)llKey);

	if (SQLITE_DONE == sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	llKey = (ULONGLONG)sqlite3_column_int64(pStmt, 0);
	int nRealDataSize = sqlite3_column_bytes(pStmt, 1);

	unsigned char* compressData=(unsigned char*)sqlite3_column_blob(pStmt, 1);
	geoBuffer.set(compressData,nRealDataSize,true);
	int tempbufLen;
	geoBuffer.read((char*)&tempbufLen,sizeof(int));
	unsigned long packbuffersize =tempbufLen ;
	unsigned char* uncompressData=new unsigned char[packbuffersize];
	//unsigned long packbuffersize2;
	uncompress(uncompressData,&packbuffersize,compressData+4,nRealDataSize);
	geoBuffer.clear();

	//unsigned long nSize = *((int*)compressData);
	//unsigned char* pBuffer = new unsigned char[nSize];
	//uncompress((unsigned char*)pBuffer, &nSize, compressData+4, nRealDataSize-4);
	//unsigned char szVersionSize = pBuffer[0];
	//char szVersion[8] = {0};
	//char* pHeadBuffer = (char*)pBuffer;
	//memcpy(szVersion, pHeadBuffer+1, /*szVersionSize*/5);

	geoBuffer.set(uncompressData+5,packbuffersize-5,true);
	delete uncompressData;
	//int length=sqlite3_column_bytes(pStmt, 0);
	//buffer=Buffer(varData,length,true);
	//geoBuffer.set(varData,nRealDataSize,true);

	//for (int i=0;i<100;i++)
	//{
	//	Buffer buf;
	//	buf.set(varData+i,nRealDataSize-i,true);
	//	UID64 m_uFID;
	//	byte m_byTypeID;
	//	int m_nObjNameLen;
	//	buf.read((char*)&m_uFID,sizeof(UID64));
	//	buf.read((char*)&m_byTypeID,sizeof(byte));
	//	buf.read((char*)&m_nObjNameLen,sizeof(int));

	//	std::cout<<"FID: "<<m_uFID<<"  TypeID:  "<<m_byTypeID<<"   bjNameLen: "<<m_nObjNameLen<<std::endl;
	//}

	//sqlite3_column_blob(pStmt, 1);
	//У���빲6+9�ֽ�
	//nDataLen = nRealDataSize + nVersionSize + 9;
	//CGrowBuffer	  m_growbuff;
	//if (m_growbuff.allocbuff(nDataLen) == NULL)
	//{
	//	sqlite3_finalize(pStmt);
	//	return false;
	//}

	//int nPos = 0;
	////�汾��
	////	memcpy(m_growbuff.buffhead(), strVersion, nVersionSize);
	//nPos += nVersionSize;

	////��������
	////	memcpy(m_growbuff.buffhead()+nPos, sqlite3_column_blob(pStmt, 1), nRealDataSize);
	//nPos += nRealDataSize;

	////������
	//unsigned char cEnd = '0';
	////	memcpy(m_growbuff.buffhead()+nPos, &cEnd, 1);
	//nPos += 1;

	////���ݰ����ܳ���
	////	memcpy(m_growbuff.buffhead()+nPos, &nDataLen, 4);
	//nPos += 4;

	////�������
	//int nCount = 1;
	//	memcpy(m_growbuff.buffhead()+nPos, &nCount, 4);

	sqlite3_finalize(pStmt);
	//CSafeArrayVariantPtr varSrcData(m_growbuff.buffhead(), m_growbuff.buffsize());
	//::VariantCopy(varData, &varSrcData);

	return true;
}


bool CSQLiteOP::GetTextureByTexKey(ULONGLONG llKey, Buffer& texBuffer)
{


	long nDataLen = 0;

	std::string strName(TEX_TABLE_NAME);
	std::string strSQL = "SELECT * FROM " +  strName + " where Key = ?";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;

	sqlite3_bind_int64(pStmt, 1, (LONGLONG)llKey);

	if (SQLITE_DONE == sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	llKey = (ULONGLONG)sqlite3_column_int64(pStmt, 0);
	int nRealDataSize = sqlite3_column_bytes(pStmt, 1);
	//if (nRealDataSize < 5)
	//	return false;

	unsigned char* texData=(unsigned char*)sqlite3_column_blob(pStmt, 1);

	texBuffer.set(texData+5,nRealDataSize-5,true);
	//texBuffer = Buffer(texData+5 /*+ 7 + 4*/, nRealDataSize-5/* - 11 - 9*/);
//	delete texData;
	sqlite3_finalize(pStmt);
	return true;
	
	//unsigned char* compressData=(unsigned char*)sqlite3_column_blob(pStmt, 1);
	//texBuffer.set(compressData,nRealDataSize,true);
	//int tempbufLen;
	//texBuffer.read((char*)&tempbufLen,sizeof(int));
	//unsigned long packbuffersize =tempbufLen ;
	//unsigned char* uncompressData=new unsigned char[packbuffersize];
	////unsigned long packbuffersize2;
	//uncompress(uncompressData,&packbuffersize,compressData+4,nRealDataSize);
	//texBuffer.clear();
	//texBuffer.set(uncompressData+5,packbuffersize-5,true);
	//int length=sqlite3_column_bytes(pStmt, 0);
	//buffer=Buffer(varData,length,true);
	//geoBuffer.set(varData,nRealDataSize,true);

	//for (int i=0;i<100;i++)
	//{
	//	Buffer buf;
	//	buf.set(varData+i,nRealDataSize-i,true);
	//	UID64 m_uFID;
	//	byte m_byTypeID;
	//	int m_nObjNameLen;
	//	buf.read((char*)&m_uFID,sizeof(UID64));
	//	buf.read((char*)&m_byTypeID,sizeof(byte));
	//	buf.read((char*)&m_nObjNameLen,sizeof(int));

	//	std::cout<<"FID: "<<m_uFID<<"  TypeID:  "<<m_byTypeID<<"   bjNameLen: "<<m_nObjNameLen<<std::endl;
	//}

	//sqlite3_column_blob(pStmt, 1);
	//У���빲6+9�ֽ�
	//nDataLen = nRealDataSize + nVersionSize + 9;
	//CGrowBuffer	  m_growbuff;
	//if (m_growbuff.allocbuff(nDataLen) == NULL)
	//{
	//	sqlite3_finalize(pStmt);
	//	return false;
	//}

	//int nPos = 0;
	////�汾��
	////	memcpy(m_growbuff.buffhead(), strVersion, nVersionSize);
	//nPos += nVersionSize;

	////��������
	////	memcpy(m_growbuff.buffhead()+nPos, sqlite3_column_blob(pStmt, 1), nRealDataSize);
	//nPos += nRealDataSize;

	////������
	//unsigned char cEnd = '0';
	////	memcpy(m_growbuff.buffhead()+nPos, &cEnd, 1);
	//nPos += 1;

	////���ݰ����ܳ���
	////	memcpy(m_growbuff.buffhead()+nPos, &nDataLen, 4);
	//nPos += 4;

	////�������
	//int nCount = 1;
	//	memcpy(m_growbuff.buffhead()+nPos, &nCount, 4);

	sqlite3_finalize(pStmt);
	//CSafeArrayVariantPtr varSrcData(m_growbuff.buffhead(), m_growbuff.buffsize());
	//::VariantCopy(varData, &varSrcData);

	return true;
}


//void CSQLiteOP::Buffer2Features(FEATURE_MAP &vtFeatures, char *pData, int nSize) const
//{
//
//	if (! pData || nSize <= 0) return;
//
//	int pos = 0;
//	pos += 6;
//
//
//
//	char szTag[5] = {0};
//	memcpy(szTag, pData, 4);
//	char szVersion[3] = {0};
//	memcpy(szVersion, pData+4, 2);
//	if ( stricmp(szTag, "GMDL") == 0 )
//	{
//		if (stricmp(szVersion, "05") == 0)
//			_options->_modelVersion=eModelVersionGMDL500;
//
//		//����Ϊ��5.1
//		if (stricmp(szVersion, "51") == 0)
//			_options->_modelVersion=eModelVersionGMDL501;
//
//		//����Ϊ��5.2
//		if (stricmp(szVersion, "52") == 0)
//			_options->_modelVersion=eModelVersionGMDL502;
//
//		//����Ϊ��5.3
//		if (stricmp(szVersion, "53") == 0)
//			_options->_modelVersion=eModelVersionGMDL503;
//
//		//����Ϊ��5.4
//		if (stricmp(szVersion, "54") == 0)
//			_options->_modelVersion=eModelVersionGMDL504;
//
//
//	}
//
//
//	while(pos != nSize-9)
//	{
//		//����
//		byte curMetaType = 0;
//		memcpy((char *)&curMetaType, pData + pos, sizeof(byte));
//		pos += 1;
//		//����
//		int nMetaDataLen = 0;
//		memcpy((char *)&nMetaDataLen,pData + pos,sizeof(int));
//		pos += 4;
//		if (pos + nMetaDataLen > nSize)
//		{
//			break;
//		}
//
//	
//
//
//
//
//		//������תΪFeature
//		Buffer buff(pData + pos,nMetaDataLen);
//		CFeature *tempFeature = new CFeature;
//		tempFeature->readBufferForService(buff);
//		pos += nMetaDataLen;
//		//�����ɵ�Feature�ŵ�������
//
//		/*	Location location=tempFeature->getLocation();
//		double centerLon=location.dLongitude;
//		double centerLat=location.dLatitude;
//		double centerElevation=location.dElevation;
//		osg::Vec3d position;
//		_srs->transformToWorld(osg::Vec3d(centerLon,centerLat,centerElevation),position);
//		location.worldPosition=position;
//		tempFeature->setLocation(location);
//		tempFeature->_dbName=dbName;
//		LOD_INFOList lodInfoList=tempFeature->getLOD_INFOList();
//		for (int i=0;i<lodInfoList.size();i++)
//		{
//		char strLon[100],strLat[100],strElevation[100];
//		sprintf(strLon,"%.14lf",centerLon);
//		sprintf(strLat,"%.14lf",centerLat);
//		sprintf(strElevation,"%.14lf",centerElevation);
//		ULONGLONG geoKey=lodInfoList[i].unGeometryID;
//		std::stringstream ct3dGeoName;
//		ct3dGeoName<<dbName<<"."<<geoKey<<"."<<strLon<<"."<<strLat<<"."<<strElevation<<".ct3d";
//		lodInfoList[i].dateName=ct3dGeoName.str();
//		}
//		tempFeature->setLOD_INFOList(lodInfoList);*/
//		vtFeatures.insert(make_pair(tempFeature->getFeatureID(),tempFeature));
//		//delete
//		//vtFeatures.push_back(tempFeature);
//	}
//}


//bool CSQLiteOP::GetTileByTileKey(int level,int row,int col,Buffer& tileBuffer)
//{
//
//	std::string strName(TILE_TABLE_NAME);
//	std::string strSQL = "SELECT tiledata,tilesize FROM " +  strName + " where level = ? and row=? and col=?";
//
//	sqlite3_stmt *pStmt = 0;
//	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
//		return false;
//
//	sqlite3_bind_int(pStmt, 1, level);
//	sqlite3_bind_int(pStmt, 2, row);
//	sqlite3_bind_int(pStmt, 3, col);
//
//	if (SQLITE_DONE == sqlite3_step(pStmt))
//	{
//		sqlite3_finalize(pStmt);
//		return false;
//	}
//
//	int nSize = sqlite3_column_int(pStmt, 1);
//	unsigned char* tileData=(unsigned char*)sqlite3_column_blob(pStmt, 0);
//	tileBuffer.set(texData+5,nRealDataSize-5,true);
//
//
//
//	sqlite3_finalize(pStmt);
//
//
//
//	return true;
//}

bool CSQLiteOP::GetTileDataByTileKey(int level,int col,int row, char **ppTileBuff,int& nSize)
{

	std::string strName(TILE_TABLE_NAME);
	std::string strSQL = "SELECT tiledata,tilesize FROM " +  strName + " where level = ? and row=? and col=?";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;

	sqlite3_bind_int(pStmt, 1, level);
	sqlite3_bind_int(pStmt, 2, row);
	sqlite3_bind_int(pStmt, 3, col);

	if (SQLITE_DONE == sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	nSize = sqlite3_column_int(pStmt, 1);
	char* data=(char*)sqlite3_column_blob(pStmt, 0);
	*ppTileBuff = new char[nSize];
	memcpy(*ppTileBuff,data,nSize);



	sqlite3_finalize(pStmt);



	return true;
}

/////////////////////////////////////////////////////////////////////////////image and terrain

bool  CSQLiteOP::ReadGeometryTypeFromFCS(int& nGeometry)
{
	if (! m_pSqliteDB) return false;

	std::string sql;
	sql="select value from metatable where name='GeometryType'";
	sqlite3_stmt* stmt;

	if (SQLITE_OK == sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			nGeometry=sqlite3_column_int(stmt,0);
		}
	}

	sqlite3_finalize(stmt);
	return true;
}

bool CSQLiteOP::ReadTileRangeFromFCS(TileMetaData& metadata)
{
	if (! m_pSqliteDB) return false;

	sqlite3_stmt* stmt;

	std::string sql;

	sql="select value from metatable where name='XMin'";
	if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			metadata.bbox.west=sqlite3_column_double(stmt,0);
		}
	}

	sql="select value from metatable where name='XMax'";
	if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			metadata.bbox.east=sqlite3_column_double(stmt,0);
		}
	}

	sql="select value from metatable where name='YMin'";
	if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			metadata.bbox.north=sqlite3_column_double(stmt,0);
		}
	}

	sql="select value from metatable where name='YMax'";
	if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			metadata.bbox.south=sqlite3_column_double(stmt,0);
		}
	}

	sql="select value from metatable where name='Name'";
	if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			metadata.strDataSetName=(char*)sqlite3_column_text(stmt,0);
		}
	}

	sqlite3_finalize(stmt);
	return true;
}

bool CSQLiteOP::ReadTileName(string& strTileName)
{
	if (! m_pSqliteDB) return false;

	std::string sql;
	sql="select value from metatable where name='Name'";
	sqlite3_stmt* stmt;


	if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			strTileName = string((const char*)sqlite3_column_text(stmt,0));
		}
	}

	sqlite3_finalize(stmt);
	return true;
}


bool CSQLiteOP::ReadTileMeta(TileMetaData& metadata)
{
	if (! m_pSqliteDB) return false;

	std::string sql;
	sql="select itemdata from TileMeta where itemname='ValidTopLevel'";
	sqlite3_stmt* stmt;

	if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
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
	if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
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
	if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			metadata.bbox.west=sqlite3_column_double(stmt,0);
		}
	}


	sql="select itemdata from TileMeta where itemname='East'";
	if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			metadata.bbox.east=sqlite3_column_double(stmt,0);
		}
	}
	

	sql="select itemdata from TileMeta where itemname='South'";
	if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			metadata.bbox.south=sqlite3_column_double(stmt,0);
		}
	}

	sql="select itemdata from TileMeta where itemname='North'";
	if (SQLITE_OK==sqlite3_prepare(m_pSqliteDB,sql.c_str(),-1,&stmt,0))
	{
		while(sqlite3_step(stmt)==SQLITE_ROW)
		{
			metadata.bbox.north=sqlite3_column_double(stmt,0);
		}
	}
	sqlite3_finalize(stmt);
	return true;
}


bool CSQLiteOP::GetTileDataByTileKey(int level,int col,int row, char **ppTileBuff,int& nSize,int& dataType)
{

	std::string strName(TILE_TABLE_NAME);
	std::string strSQL = "SELECT tiledata,tilesize,tiletype FROM " +  strName + " where level = ? and row=? and col=?";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;

	sqlite3_bind_int(pStmt, 1, level);
	sqlite3_bind_int(pStmt, 2, row);
	sqlite3_bind_int(pStmt, 3, col);

	if (SQLITE_DONE == sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}
	
	
		dataType=sqlite3_column_int(pStmt, 2);
		nSize = sqlite3_column_int(pStmt, 1);
		char* data=(char*)sqlite3_column_blob(pStmt, 0);
		*ppTileBuff = new char[nSize];
		memcpy(*ppTileBuff,data,nSize);
		sqlite3_finalize(pStmt);
		return true;
	
}
bool CSQLiteOP::GetTileDataByTileKey(std::string tablename,int level,int col,int row, char **ppTileBuff,int& nSize,int& dataType)
{

	std::string strName=tablename;
	std::string strSQL = "SELECT tiledata,tilesize,tiletype FROM " +  strName + " where level = ? and row=? and col=?";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;

	sqlite3_bind_int(pStmt, 1, level);
	sqlite3_bind_int(pStmt, 2, row);
	sqlite3_bind_int(pStmt, 3, col);

	if (SQLITE_DONE == sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}


	dataType=sqlite3_column_int(pStmt, 2);
	nSize = sqlite3_column_int(pStmt, 1);
	char* data=(char*)sqlite3_column_blob(pStmt, 0);
	*ppTileBuff = new char[nSize];
	memcpy(*ppTileBuff,data,nSize);
	sqlite3_finalize(pStmt);
	return true;

}

/////////////////////////////////////////////////
bool CSQLiteOP::IsTileDataExist(int level,int col,int row)
{
	if (! m_pSqliteDB) return false;

	std::string strName(TILE_TABLE_NAME);
	std::string strSQL = "SELECT COUNT(*) FROM " +  strName + " where level = ? and row=? and col=?";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;

	sqlite3_bind_int(pStmt, 1, level);
	sqlite3_bind_int(pStmt, 2, row);
	sqlite3_bind_int(pStmt, 3, col);

	if (SQLITE_DONE == sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	//nSize = sqlite3_column_int(pStmt, 1);
	//char* data=(char*)sqlite3_column_blob(pStmt, 0);
	//*ppTileBuff = new char[nSize];
	//memcpy(*ppTileBuff,data,nSize);
	int count=sqlite3_column_int(pStmt,0);


	sqlite3_finalize(pStmt);

	if(count>0)
		return true;

	return false;
}

bool CSQLiteOP::IsTileDataExist(const char *TableName,int level,int col,int row)
{
	if (! m_pSqliteDB) return false;

	std::string strName(TableName);
	std::string strSQL = "SELECT COUNT(*) FROM " +  strName + " where level = ? and row=? and col=?";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;

	sqlite3_bind_int(pStmt, 1, level);
	sqlite3_bind_int(pStmt, 2, row);
	sqlite3_bind_int(pStmt, 3, col);

	if (SQLITE_DONE == sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	//nSize = sqlite3_column_int(pStmt, 1);
	//char* data=(char*)sqlite3_column_blob(pStmt, 0);
	//*ppTileBuff = new char[nSize];
	//memcpy(*ppTileBuff,data,nSize);
	int count=sqlite3_column_int(pStmt,0);


	sqlite3_finalize(pStmt);

	if(count>0)
		return true;

	return false;
}


bool CSQLiteOP::IsModelExist(ULONGLONG& fcKey,int& lod)
{
	if (! m_pSqliteDB) return false;

	std::string strName("Model");
	std::string strSQL = "SELECT COUNT(*) FROM " +  strName + " where key = ? and lod=? ";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;

	sqlite3_bind_int64(pStmt, 1, (LONGLONG)fcKey);
	sqlite3_bind_int(pStmt, 2, lod);
	//sqlite3_bind_int(pStmt, 3, col);

	if (SQLITE_DONE == sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	//nSize = sqlite3_column_int(pStmt, 1);
	//char* data=(char*)sqlite3_column_blob(pStmt, 0);
	//*ppTileBuff = new char[nSize];
	//memcpy(*ppTileBuff,data,nSize);
	int count=sqlite3_column_int(pStmt,0);


	sqlite3_finalize(pStmt);

	if(count>0)
		return true;

	return false;
}

bool CSQLiteOP::IsKeyValueExist(const char *TableName,ULONGLONG& key)
{
	if (! m_pSqliteDB) return false;

	std::string strName(TableName);
	std::string strSQL = "SELECT COUNT(*) FROM " +  strName + " where key = ? ";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;

	sqlite3_bind_int64(pStmt, 1, (LONGLONG)key);
	//sqlite3_bind_int(pStmt, 2, lod);
	//sqlite3_bind_int(pStmt, 3, col);

	if (SQLITE_DONE == sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	//nSize = sqlite3_column_int(pStmt, 1);
	//char* data=(char*)sqlite3_column_blob(pStmt, 0);
	//*ppTileBuff = new char[nSize];
	//memcpy(*ppTileBuff,data,nSize);
	int count=sqlite3_column_int(pStmt,0);


	sqlite3_finalize(pStmt);

	if(count>0)
		return true;

	return false;
}


bool  CSQLiteOP::GetPlaceNameIconDataByID(int dataID,char** ppTileBuff,int& dataType,int& dataSize)
{
	std::string strSQL = "SELECT PIC1,TYPE FROM WTFSICONLIB where ID = ?";

	sqlite3_stmt *pStmt = 0;
	if (SQLITE_OK != sqlite3_prepare(m_pSqliteDB, strSQL.c_str(), strSQL.size(), &pStmt, 0))
		return false;

	sqlite3_bind_int(pStmt, 1,dataID);
	if (SQLITE_DONE == sqlite3_step(pStmt))
	{
		sqlite3_finalize(pStmt);
		return false;
	}

	dataType=sqlite3_column_int(pStmt, 1);
	dataSize=sqlite3_column_bytes(pStmt,0);
	char* data=(char*)sqlite3_column_blob(pStmt, 0);
	*ppTileBuff = new char[dataSize];
	memcpy(*ppTileBuff,data,dataSize);
	sqlite3_finalize(pStmt);
	return true;

}
