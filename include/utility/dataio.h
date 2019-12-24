#pragma once
#include "preconfig.h" 
#include "object.h"
#include "stringhelp.h"
#include "timer.h"
UTILITY_NS

/// \brief 创建Data命名空间
namespace Data
{
	/// \brief 封装数据库接口
	class GS_API GsDatabase:private GsNoneCopyable
	{ 
    public:
		virtual ~GsDatabase();
		/// \brief 判断是否打开文件
        virtual bool IsOpen()const = 0;
		/// \brief 执行操作
		virtual int Execute(const char* strSQL) = 0;
		/// \brief 插入最后1行
		virtual long long LastInsertRowID () const = 0;
		/// \brief 判断是否关闭
		virtual bool Close()=0;
		/// \brief 最后的消息
		GsString ErrorMessage();
		/// \brief 错误编号
		virtual long long ErrorCode();
		
		/// \brief 打断正在运行中的数据库命令
		/// \return 不支持打断操作的能力则返回false，成功打断或者无须打断都返回true
		virtual bool Interrupt();

    };
	/// \brief 事务对象
	class GS_API GsTransaction :public Utility::GsRefObject
	{
	protected:
		GsTransaction() {}
	public:
		virtual ~GsTransaction() {}

		/// \brief 是否处于事务中
		///\return 返回是否处于事务中
		virtual bool IsTransaction() = 0;

		/// \brief 启动事务
		///\return 返回是否成功启动
		virtual bool StartTransaction() = 0;

		/// \brief 提交事务
		///\return 返回是否提交成功
		virtual bool CommitTransaction() = 0;

		/// \brief 取消事务
		///\return 返回是否取消成功
		virtual bool RollbackTransaction() = 0;
	};
	/// \brief GsTransactionPtr
	GS_SMARTER_PTR(GsTransaction);
	
	enum GsFieldType
	{
		eErrorType,
		/// \brief BOOL类型
		eBoolType,
		/// \brief 32位的整型
		eIntType,
		/// \brief 32位的无符号整型
		eUIntType,
		/// \brief 64位的整型
		eInt64Type,
		/// \brief 64位的无符号整型
		eUInt64Type,
		/// \brief 字符串类型
		eStringType,
		/// \brief 二进制类型
		eBlobType,
		/// \brief 浮点型
		eFloatType,
		/// \brief 双精度浮点型
		eDoubleType,
		/// \brief 几何类型
		eGeometryType,
		
		/// \brief 日期类型
		eDateType
	};

	/// \brief 封装数据库的字符串操作
	class GS_API GsStatement:private GsNoneCopyable
	{
	protected:
		GsDatabase*  m_pDB;
		GsString m_strSQL;

	public:
		/// \brief 缺省构造函数
		GsStatement();
		/// \brief 重载构造函数，创建数据库指针
		GsStatement(GsDatabase* db,const char* strSQL);
		virtual ~GsStatement();
		virtual bool Prepare(GsDatabase* db,const char* sql) = 0;
		/// \brief 将本地字符串转化为数据库字符串
		virtual GsString SQLString();
		/// \brief 返回-1
		virtual long long RecordCount();
		/// \brief 读取32位
		virtual int IntValue(int nCol);
		/// \brief 读取64位
		virtual long long Int64Value(int nCol);
		/// \brief 读取字符串
		virtual GsString StringValue(int nCol);
		/// \brief 读取日期
		virtual GsDateTime DataTimeValue(int nCol);
		/// \brief 读取无符号字符常量
		virtual const char* StringValuePtr(int nCol);
		/// \brief 读取double类型数据
		virtual double DoubleValue(int nCol);
		/// \brief 绑定无符号字符
		virtual int Bind(int nIndex,const unsigned char* pBlob ,int nLen);
		/// \brief 绑定32位整数
		virtual int Bind(int nIndex,int nValue);
		/// \brief 绑定64位浮点数
		virtual int Bind(int nIndex,double nValue);
		/// \brief 绑定64位无符号整数
		virtual int Bind(int nIndex,unsigned long long nValue);
		/// \brief 绑定函数重载
		virtual int Bind(int nIndex);
		/// \brief 绑定字符常量
		virtual int Bind(int nIndex,const char* pStr);
		/// \brief 绑定64位整数或者日期
		virtual int Bind(int nIndex,long long nValue,bool bIsDateTime=false);

	public:
		/// \brief 执行读取，返回1个结果
		virtual int ExecuteQuery();
		/// \brief 执行读取，返回多个结果
		virtual int Execute();
		/// \brief 返回下1条数据
		virtual bool MoveNext() = 0;
		/// \brief 读取无符号字符数据
		virtual const unsigned char* BlobValue(int nCol) = 0;
		/// \brief 返回当前列的值
		virtual int ValueLength(int nCol)= 0;
		/// \brief 绑定索引
		virtual int BindValue(int nIndex,const unsigned char* pBlob ,int nLen, GsFieldType eType) = 0;
		/// \brief 释放内存
		virtual int Reset() = 0;
		/// \brief 获取查询结果的列数
		virtual int ColumnCount() = 0;
		/// \brief 获取列的类型
		virtual GsFieldType ColunmType(int n) = 0;
		/// \brief 获取列的名称
		virtual GsString ColunmName(int n) = 0; 
		 
	};
}
 
UTILITY_ENDNS
