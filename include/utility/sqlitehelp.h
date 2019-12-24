#pragma once 

#include "preconfig.h" 

#include "dataio.h" 
#include "sqlite3.h" 
#include <iostream>

UTILITY_NS 

namespace Data{
	namespace Sqlite{

/// \brief sqlite数据库指针的封装。
class GS_API GsSqliteDatabase:public GsDatabase
{	
	 sqlite3 * m_pDB;
public:
	/// \brief 构造函数
	GsSqliteDatabase();
	GsSqliteDatabase(const char *filename);
	virtual ~GsSqliteDatabase();
	/// \brief 转化成数据库指针
	operator sqlite3*() const;
	/// \brief 返回指针地址
	sqlite3* DB() const;
	void Attach(sqlite3* db);	
	/// \brief 判断是否打开文件
	virtual bool IsOpen()const ;
	/// \brief 执行操作
	virtual int Execute(const char* strSQL);
	/// \brief 执行操作
	/// \param strSQL SQL语句 参见sqlite3_create_function使用方法
	/// \param strFuncName 要绑定的函数名
	/// \param SqliteCallback 绑定的函数
	/// \param pUserParam 传入的参数
	virtual int Execute(const char* strSQL, const char* strFuncName, const void* SqliteCallback, void* pUserParam);
	/// \brief 插入最后1行
	virtual long long LastInsertRowID () const;
	/// \brief 判断是否关闭
	virtual bool Close();	 
	/// \brief 最后的消息
	virtual GsString ErrorMessage();
	/// \brief 错误编号
	virtual long long ErrorCode();
	/// \brief 判断是否读取失败
	static bool Error(long long nErrorCode);
	/// \brief 打断正在运行中的数据库命令
	/// \return 不支持打断操作的能力则返回false，成功打断或者无须打断都返回true
	virtual bool Interrupt();


	/// \brief 函数模板
	template<class T>
	T Error(int nErrorCode,T bOkValue,T bFailValue)
	{
		/// \brief 如果读取成功，返回成功的消息
		if(!Error(nErrorCode))
			return bOkValue;
		/// \brief 如果读取失败，返回失败的消息
		const char* strError = sqlite3_errmsg(m_pDB);
		std::cout<<strError <<std::endl;
		return bFailValue;
	}
	/// \brief 判断表是否存在
	bool ExistTable(const char* tbName);
};


class GS_API GsSqliteTransaction:private GsNoneCopyable
{
	GsSqliteDatabase* m_pDB;
public:
	/// \brief 构造函数
	GsSqliteTransaction(GsSqliteDatabase* pDB);
	/// \brief 析构函数
	virtual ~GsSqliteTransaction(); 
	/// \brief 提交数据
	virtual void Commit ();
	/// \brief 回滚数据
	virtual void Rollback();
};

/// \brief 包装Sqlite的stmt
class GS_API GsSqliteStatement:public GsStatement
{
protected:
	GsSqliteDatabase* m_SqliteDB;
	/// \brief 创建stmt指针
	sqlite3_stmt * m_pStmt;  
	int m_nRet; 
public:
	/// \brief 缺省构造函数
	GsSqliteStatement();
	/// \brief 构造函数重载
	GsSqliteStatement(GsSqliteDatabase* db,const char* sql = NULL);
	/// \brief 准备执行
	virtual bool Prepare(GsDatabase* db,const char* sql);
	/// \brief 返回指针地址
	operator sqlite3_stmt*() {return m_pStmt;}
	/// \brief 析构函数
 	virtual ~GsSqliteStatement();
	void Close();
public: 
	/// \brief 判断指针是否为空
	virtual bool IsNull();
	/// \brief 执行读取，返回多个结果
	virtual int Execute();
	/// \brief 执行读取，返回1个结果
	virtual int ExecuteQuery();
	/// \brief 读取下一条数据
	virtual bool MoveNext();
	/// \brief 读取无符号字符数据
	virtual const unsigned char* BlobValue(int nCol);
	/// \brief 读取32位整数数据
	virtual int IntValue(int nCol);
	/// \brief 读取64位整数数据
	virtual long long Int64Value(int nCol);
	/// \brief 读取64位浮点数据
	virtual double DoubleValue(int nCol);
	/// \brief 返回当前列的值
	virtual int ValueLength(int nCol); 
	/// \brief 绑定索引
	virtual int BindValue(int nIndex,const unsigned char* pBlob ,int nLen, GsFieldType eType);
	/// \brief 释放内存
	virtual int Reset();
	/// \brief 获取查询结果的列数
	virtual int ColumnCount();
	/// \brief 获取列的类型
	virtual GsFieldType ColunmType(int n);
	/// \brief 获取列的名称
	virtual GsString ColunmName(int n); 
	/// \brief 满足要求的记录数
	virtual long long RecordCount();
	/// \brief 插入最后一行
	long long LastInsertRowID();

}; 
}
}
UTILITY_ENDNS