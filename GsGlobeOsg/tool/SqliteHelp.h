#pragma once 
#include "sqlite3.h" 
#include <iostream>

//基于SQLITE的互斥锁 
class CSqliteLock
 {
	sqlite3_mutex *m_pMutex;
 public:
	CSqliteLock();
	virtual ~CSqliteLock();
	void Lock();
	void Unlock();
 };

//锁定者。
template<typename T>
class  CLocker
{
	T* m_L;
public:
	CLocker(T* l)
	{
		m_L= NULL;
		m_L = l;
		m_L->Lock();
	}
	CLocker(T* l,bool bRead)
	{
		m_L= NULL;
		m_L = l;
		m_L->Lock(bRead);

	}
	~CLocker()
	{	
		m_L->Unlock();
		m_L= NULL;
	}

};


#ifndef LOCK_ME 
#define LOCK_ME CLocker<CSqliteLock> locker(this);
#endif

#ifndef LOCK_IT 
#define LOCK_IT(x) CLocker<CSqliteLock> locker(&x);
#endif

//sqlite数据库指针的封装。
class CSqliteDB
{
public:
	 sqlite3 * m_pDB;
	 CSqliteDB();
	 virtual ~CSqliteDB();
	 void Close();
	 sqlite3* DB();
	 void Attach(sqlite3* db);
	 //开始会话
	 bool Begin();
	 bool Commit();
	 bool Abort();
	 int Changes();
	 void ReportError();
	 //最后的消息
	 std::string ErrorMessage();
	 static bool Error(int nErrorCode);
	template<class T>
	T Error(int nErrorCode,T bOkValue,T bFailValue)
	{
		if(!Error(nErrorCode))
			return bOkValue;
		
		const char* strError = sqlite3_errmsg(m_pDB);
		std::cout<<strError <<std::endl;
		return bFailValue;
	}
	 //执行SQL语句
	 bool Excute(const char* sql);
	operator sqlite3*() 
	{
		return m_pDB;
	}
	//判断表是否存在
	bool ExistTable(const char* tbName);
	
	//启动一个SavePoint
	__int64 SavePoint();
	bool SavePoint(__int64 nSavePoint);
	bool SavePoint(const char* strSavePoint);

	//回滚到某个SavePoint
	bool Rollback(__int64 nSavePoint);
	bool Rollback(const char* strSavePoint);
	
	//释放SavePoint
	bool Release(__int64 nSavePoint);
	bool Release(const char* strSavePoint);
};
//包装Sqlite的stmt
class CStmt
{
protected:
	sqlite3_stmt * m_pStmt;
	bool m_bFreeOnEnd ;
public:
	CStmt(sqlite3* db,const char* sql,bool bFreeOnEnd = true);

	CStmt();
	CStmt(sqlite3_stmt * pStmt,bool bFreeOnEnd = true);
	//重新使用Stmt
	bool Reuse();

	bool Prepare(sqlite3* db,const char* sql,bool bFreeOnEnd = true);
	operator sqlite3_stmt*() {return m_pStmt;}
 
	~CStmt();
	void Close();
	bool IsNull();
	int RecordCount();
	int Step();
	int IntValue(int nCol);
	__int64 Int64Value(int nCol);
	const char*StringValuePtr(int nCol);
	std::string StringValue(int nCol);
	const unsigned char* BlobValue(int nCol);
	long ValueLength(int nCol);
	double DoubleValue(int nCol);
	int Reset();
	//绑定blob
	int Bind(int nIndex,const unsigned char* pBlob ,int nLen);
	//绑定integer
	int Bind(int nIndex,int nValue);
	//绑定double
	int Bind(int nIndex,double nValue);
	//绑定integer
	int Bind(int nIndex,__int64 nValue);
	int Bind(int nIndex,unsigned __int64 nValue);
	
	//绑定integer
	int Bind(int nIndex);
	//绑定integer
	int Bind(int nIndex,const char* pStr);
	int ColumnCount();

	std::string ColunmType(int n);
	std::string ColunmName(int n); 

};
//带锁的Stmt
template<class T=CStmt>
class CSafeStmt:public T,public CSqliteLock
{
};
class CSavePoint
{
	CSqliteDB* m_pDB;
	__int64  m_nPoint;
public:
	CSavePoint(CSqliteDB* pDB)
	{
		m_pDB =  pDB;
		m_nPoint = m_pDB->SavePoint();
	}
	virtual ~CSavePoint()
	{
		if(0 != m_nPoint)
			m_pDB->Release(m_nPoint);
	}
	void Rollback()
	{
		m_pDB->Rollback(m_nPoint);
		m_nPoint = 0;
	}
};
