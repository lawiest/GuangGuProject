#include "SqliteHelp.h"
#include "time.h" 
//基于SQLITE的互斥锁 
 
CSqliteLock::CSqliteLock()
{
	m_pMutex =  sqlite3_mutex_alloc(SQLITE_MUTEX_FAST);
}
CSqliteLock::~CSqliteLock()
{
	sqlite3_mutex_free(m_pMutex);
}
void CSqliteLock::Lock()
{
	sqlite3_mutex_enter(m_pMutex);
}
void CSqliteLock::Unlock()
{
	sqlite3_mutex_leave(m_pMutex);
}

 
 

//sqlite数据库指针的封装。 
CSqliteDB::CSqliteDB():m_pDB(NULL)
 {

 }
 CSqliteDB::~CSqliteDB()
 {
	Close();
 }
 
 void CSqliteDB::Close()
 {
	if(NULL != m_pDB)
		sqlite3_close(m_pDB);
	
	m_pDB = NULL;
 }
 sqlite3* CSqliteDB::DB()
 {
	 return m_pDB;
 }
 void CSqliteDB::Attach(sqlite3* db)
 {	
	 //Close();
	 m_pDB = db;
 }
 //开始会话
 bool CSqliteDB::Begin()
 { 
	return Excute("begin");
 }
 bool CSqliteDB::Commit()
 { 
	 return Excute("commit");
 }
 bool CSqliteDB::Abort()
 {
	 return Excute("rollback");
 }

 int CSqliteDB::Changes()
 {
	 return sqlite3_changes(m_pDB);
 }


 void CSqliteDB::ReportError()
 {
	std::string str = ErrorMessage();
	if(str.empty() )
		return ;

	std::cout<<str.c_str()<<std::endl;
 }
 //最后的消息
 std::string CSqliteDB::ErrorMessage()
 {
	const char* strError = sqlite3_errmsg(m_pDB);
	if(NULL == strError)
		return std::string();
	
	return std::string(strError);
 }
 bool CSqliteDB::Error(int nErrorCode)
 {
	return !(SQLITE_OK == nErrorCode ||
		SQLITE_DONE == nErrorCode);
	
 }
 
 //执行SQL语句
 bool CSqliteDB::Excute(const char* sql)
 {
	 int nRet = sqlite3_exec(m_pDB,sql,NULL,NULL,NULL);
	 if(nRet == SQLITE_OK )
		 return true;
	 return Error(nRet,true,false);
 } 
 //启动一个SavePoint
__int64 CSqliteDB::SavePoint()
{
	time_t t;
	t = time(&t);
	SavePoint(t);
	return t;
}
bool CSqliteDB::SavePoint(__int64 nSavePoint)
{
	char buff[128];
	sprintf(buff,"SPT%X",nSavePoint);
	return SavePoint(buff);
}
bool CSqliteDB::SavePoint(const char* strSavePoint)
{
	std::string str = "SavePoint ";
	str+=strSavePoint;
	return Excute(str.c_str());
}

//回滚到某个SavePoint
bool CSqliteDB::Rollback(__int64 nSavePoint)
{
	char buff[128];
	sprintf(buff,"SPT%X",nSavePoint);
	return Rollback(buff);
}
bool CSqliteDB::Rollback(const char* strSavePoint)
{
	std::string sql = "rollback savepoint to ";
	sql+=strSavePoint;
	return Excute(sql.c_str());
}

//释放SavePoint
bool CSqliteDB::Release(__int64 nSavePoint)
{
	char buff[128];
	sprintf(buff,"SPT%X",nSavePoint);
	return Release(buff);
}
bool CSqliteDB::Release(const char* strSavePoint)
{
	std::string sql = "release savepoint ";
	sql+=strSavePoint;
	return Excute(sql.c_str());
}
//判断表是否存在
bool CSqliteDB::ExistTable(const char* tbName)
{
	CStmt stmt(m_pDB,"select count(*) from sqlite_master where type ='table' and name=?");
	stmt.Bind(1,tbName);
	if(SQLITE_ROW != stmt.Step())
		return false;

	//如果返回记录个数大于0则表示表存在
	return stmt.IntValue(0)>0;
}

//包装Sqlite的stmt 
CStmt::CStmt(sqlite3* db,const char* sql,bool bFreeOnEnd ):m_pStmt(NULL)
{
	Prepare(db,sql,bFreeOnEnd);;
}

CStmt::CStmt()
{
	m_bFreeOnEnd  = true;
	m_pStmt = NULL;
}
CStmt::CStmt(sqlite3_stmt * pStmt,bool bFreeOnEnd)
{
	m_pStmt = pStmt;
	m_bFreeOnEnd  = bFreeOnEnd ;
}
//重新使用Stmt
bool CStmt::Reuse()
{
	if(NULL == m_pStmt)
		return false;
	return Reset() == SQLITE_OK;
}
bool CStmt::Prepare(sqlite3* db,const char* sql,bool bFreeOnEnd)
{
	//Close();
	if(SQLITE_OK != sqlite3_prepare(db,sql,strlen(sql),&m_pStmt,NULL))
		m_pStmt = NULL;

	m_bFreeOnEnd  = bFreeOnEnd ;
	return NULL != m_pStmt;
} 

CStmt::~CStmt()
{
	if(!m_bFreeOnEnd)
		return;
	
	Close();
}
void CStmt::Close()
{
	if(m_pStmt == NULL)
		return;

	sqlite3_finalize(m_pStmt);
	m_pStmt  = NULL;
}
bool CStmt::IsNull()
{
	return m_pStmt == NULL;
}
int CStmt::RecordCount()
{
	return sqlite3_data_count(m_pStmt);
}
int CStmt::Step()
{
	if(NULL == m_pStmt)
		return -1;
	int nRet = sqlite3_step(m_pStmt); 
	return nRet;
}
int CStmt::IntValue(int nCol)
{
	return sqlite3_column_int(m_pStmt,nCol);
}
__int64 CStmt::Int64Value(int nCol)
{
	return sqlite3_column_int64(m_pStmt,nCol);
}
const char* CStmt::StringValuePtr(int nCol)
{
	return (const char*)sqlite3_column_text(m_pStmt,nCol);
}
std::string CStmt::StringValue(int nCol)
{
	const char* str = (const char*)sqlite3_column_text(m_pStmt,nCol);
	if(str == NULL)
		return std::string();
	return std::string(str);
}
const unsigned char* CStmt::BlobValue(int nCol)
{
	return (unsigned char*)sqlite3_column_blob(m_pStmt,nCol);
}
long CStmt::ValueLength(int nCol)
{
	return sqlite3_column_bytes(m_pStmt,nCol);
}
double CStmt::DoubleValue(int nCol)
{
	return sqlite3_column_double(m_pStmt,nCol);
}
int CStmt::Reset()
{
	return sqlite3_reset(m_pStmt);
}
//绑定blob
int CStmt::Bind(int nIndex,const unsigned char* pBlob ,int nLen)
{
	return sqlite3_bind_blob(m_pStmt,nIndex,pBlob,nLen,NULL);
}

//绑定integer
int CStmt::Bind(int nIndex,int nValue)
{
	return sqlite3_bind_int(m_pStmt,nIndex,nValue);
}

//绑定double
int CStmt::Bind(int nIndex,double nValue)
{
	return sqlite3_bind_double(m_pStmt,nIndex,nValue);
}
int  CStmt::Bind(int nIndex,unsigned __int64 nValue)
{
	return sqlite3_bind_int64(m_pStmt,nIndex,(__int64)nValue);
}
//绑定integer
int CStmt::Bind(int nIndex,__int64 nValue)
{
	return sqlite3_bind_int64(m_pStmt,nIndex,nValue);
}

//绑定integer
int CStmt::Bind(int nIndex)
{
	return sqlite3_bind_null(m_pStmt,nIndex);
}

//绑定integer
int CStmt::Bind(int nIndex,const char* pStr)
{
	if(NULL == pStr)
		return Bind(nIndex);
	return sqlite3_bind_text(m_pStmt,nIndex,pStr,strlen(pStr),NULL);
}
 
  
int CStmt::ColumnCount()
{
	return sqlite3_column_count(m_pStmt);
}


 
std::string CStmt::ColunmType(int n)
{
	const char* str = sqlite3_column_decltype(m_pStmt,n);
	if(NULL == str)
		return std::string();
	return std::string(str);
}
std::string CStmt::ColunmName(int n)
{
	const char* str = sqlite3_column_name(m_pStmt,n);
	if(NULL == str)
		return std::string();
	return std::string(str); 
}
 