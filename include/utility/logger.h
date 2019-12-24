#pragma once
#include "preconfig.h" 
#include <fstream> 
#include "locker.h"
#include "stringhelp.h"
UTILITY_NS  
/// \brief 日志的级别	
enum GsLogLevel
{
  eLOGALL = -99999,
  eLOGNONE=99999,
  eLOGTRACE= -100,
  eLOGDEBUG= 0,
  eLOGINFO=100,
  eLOGWARN=200,
  eLOGERROR=300,
  eLOGFATAL=400,
};

/// \brief 自定义的输出管道
class GS_API GsCustomLogOutput
{
public:
	~GsCustomLogOutput(){};

	virtual bool OnLog(const char* log) = 0;
	virtual void OnFlush(){};
};

/// \brief 将日志输出到文件
class GS_API GsFileCustomLogOutput:public GsCustomLogOutput
{
	std::ofstream m_file;
public:
	virtual bool OnLog(const char* log);
	virtual void OnFlush();
	GsFileCustomLogOutput(const char* strFileName,bool bAppend = false);
};

/// \brief 创建日志
class GS_API GsLogger
{  
	GsSafeObject<GsString>	m_strLasterError;
	GsString				m_strName;
	GsCustomLogOutput*		m_pHook;
	volatile	bool		m_AutoFlush;

	volatile GsLogLevel m_nLevel;
	/// \brief 日志流操作
	class GS_API GsLogStream:public Utility::GsStringStream
	{
		GsLogger& m_Log;
		bool m_bEnable;
		GsLogLevel m_nLevel;
	public:
		GsLogStream(const GsLogStream &rhs);
		GsLogStream(GsLogger& log, GsLogLevel l,bool bEnable);
		virtual ~GsLogStream();
	};

	  
public:
	/// \brief 构造函数
	GsLogger();
	GsLogger(const char* strLogName);
	
	/// \brief 以数字型的唯一Key添加一个全局日志
	/// \param logger 要添加的日志指针
	/// \param nUniqueKey 日志的唯一Key，用于获取日志的标识
	/// \param bManagePointer 是否系统管理日志指针，如果为true,那么系统退出时会自动删除日志
	static void AddGlobalLogger(GsLogger* logger, long long nUniqueKey, bool bManagePointer = false);

	/// \brief 获取全局日志，如果根据key无法获取，那么返回缺省日志
	static GsLogger& GlobalLogger(long long nUniqueKey);

	/// \brief 从全局换从移除日志，返回日志的指针
	static GsLogger* RemoveGlobalLogger(long long nUniqueKey);


	/// \brief 判断当前程序是否运行在调试状态
	///\details 仅仅对于windows平台下Visual Studio中调试状态有效，其他情况下一律返回false
	static bool IsDebuging();

	/// \brief 最后报告的错误
	GsString LastError();
	/// \brief 设置日志级别
	GsLogLevel LogLevel(GsLogLevel l);
	/// \brief 获得日志级别
	GsLogLevel LogLevel();
	/// \brief 自定义输出
	GsCustomLogOutput* CustomOutput(GsCustomLogOutput* pOutput);
	GsCustomLogOutput* CustomOutput();
	/// \brief 获取日志名
	GsString Name() const;
	/// \brief 虚析构函数
	virtual ~GsLogger(); 
	/// \brief 日志操作后输出流是否刷新
	///\return 返回true表示刷新，否则，不刷新
	bool AutoFlush();
	
	/// \brief 日志操作后输出流是否刷新
	///\return 返回true表示刷新，否则，不刷新
	void AutoFlush(bool b);


	/// \brief 报告一段日志
	void Log(GsLogLevel l,const char* log);
	/// \brief 缺省的日志对象
	static GsLogger& Default();	
	/// \brief 输出日志
	GsLogStream operator << (const GsLogLevel& l) const;
};
/// \brief 全局日志对象各级别宏定义
#define GSLOG				UTILITY_NAME::GsLogger::Default()

#define GSLOG_ERROR			UTILITY_NAME::GsLogger::Default()<<UTILITY_NAME::eLOGERROR
#define GS_E				UTILITY_NAME::GsLogger::Default()<<UTILITY_NAME::eLOGERROR

#define GSLOG_TRACE			UTILITY_NAME::GsLogger::Default()<<UTILITY_NAME::eLOGTRACE
#define GS_T				UTILITY_NAME::GsLogger::Default()<<UTILITY_NAME::eLOGTRACE

#define GSLOG_DEBUG			UTILITY_NAME::GsLogger::Default()<<UTILITY_NAME::eLOGDEBUG
#define GS_D				UTILITY_NAME::GsLogger::Default()<<UTILITY_NAME::eLOGDEBUG

#define GSLOG_INFO			UTILITY_NAME::GsLogger::Default()<<UTILITY_NAME::eLOGINFO
#define GS_I				UTILITY_NAME::GsLogger::Default()<<UTILITY_NAME::eLOGINFO

#define GSLOG_WARN			UTILITY_NAME::GsLogger::Default()<<UTILITY_NAME::eLOGWARN
#define GS_W				UTILITY_NAME::GsLogger::Default()<<UTILITY_NAME::eLOGWARN

#define GSLOG_FATAL			UTILITY_NAME::GsLogger::Default()<<UTILITY_NAME::eLOGFATAL
#define GS_F				UTILITY_NAME::GsLogger::Default()<<UTILITY_NAME::eLOGFATAL

/// \brief 堆栈追踪
class GS_API GsStackTrace
{
	Utility::GsString m_File, m_Func;
	int m_Line;
public:
	GsStackTrace(const char* file, int line, const char* func);
	~GsStackTrace();
};

#define GS_ST		UTILITY_NAME::GsStackTrace __trace__(__FILE__,__LINE__,__FUNCTION__);


UTILITY_ENDNS
