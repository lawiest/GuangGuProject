#pragma once
#include "preconfig.h"  
#include "stringhelp.h"  
#include "memorymanager.h"
#include "vectorhelp.h"

UTILITY_NS
/// \brief http状态编码
enum GsHttpStatus{
	/// \brief 1xx 信息
	eCONTINUE = 100,
	eSWITCHING_PROTOCOLS = 101,
	ePROCESSING = 102,

	/// \brief 2xx 成功
	eOK = 200,
	eCREATED = 201,
	eACCEPTED = 202,
	eNON_AUTHORITATIVE_INFORMATION = 203,
	eNO_CONTENT = 204,
	eRESET_CONTENT = 205,
	ePARTIAL_CONTENT = 206,
	eMULTI_STATUS = 207,
	eIM_USED = 226,

	/// \brief 3xx 重定向
	eMULTIPLE_CHOICES = 300,
	eMOVED_PERMANENTLY = 301,
	eFOUND = 302,
	eSEE_OTHER = 303,
	eNOT_MODIFIED = 304,
	eUSE_PROXY = 305,
	eTEMPORARY_REDIRECT = 307,
	
	/// \brief 4xx 客户端错误
	eBAD_REQUEST = 400,
	eUNAUTHORIZED = 401,
	ePAYMENT_REQUIRED = 402,
	eFORBIDDEN = 403,
	eNOT_FOUND = 404,
	eMETHOD_NOT_ALLOWED = 405,
	eNOT_ACCEPTABLE = 406,
	ePROXY_AUTHENTICATION_REQUIRED = 407,
	eREQUEST_TIMEOUT = 408,
	eCONFLICT = 409,
	eGONE = 410,
	eLENGTH_REQUIRED = 411,
	ePRECONDITION_FAILED = 412,
	eREQUEST_ENTITY_TOO_LARGE = 413,
	eREQUEST_URI_TOO_LONG = 414,
	eUNSUPPORTED_MEDIA_TYPE = 415,
	eREQUESTED_RANGE_NOT_SATISFIABLE = 416,
	eEXPECTATION_FAILED = 417,
	eUNPROCESSABLE_ENTITY = 422,
	eLOCKED = 423,
	eFAILED_DEPENDENCY = 424,
	eUPGRADE_REQUIRED = 426,

	/// \brief 5xx 服务器错误
	eINTERNAL_SERVER_ERROR = 500,
	eNOT_IMPLEMENTED = 501,
	eBAD_GATEWAY = 502,
	eSERVICE_UNAVAILABLE = 503,
	eGATEWAY_TIMEOUT = 504,
	eHTTP_VERSION_NOT_SUPPORTED = 505,
	eINSUFFICIENT_STORAGE = 507,
	eNOT_EXTENDED = 510,
};
class GsHttpCallback
{
public:
	virtual ~GsHttpCallback(){}
	/// \brief 覆盖此方法以获取从服务下载的数据
	virtual bool OnData(const unsigned char* pData,int nLen) = 0;
	
	/// \brief 返回开始时发生
	virtual bool OnResponse(const GsStringMap& header,GsHttpStatus status) = 0;

	/// \brief 返回开始时发生
	virtual bool OnResponseComplete() = 0;
};
/// \brief http客户端
class GS_API GsHttpClient
{
	void* m_Handle;
protected:
	GsHttpStatus m_Status;
	GsString	 m_Method;
	/// \brief http请求头。
	GsStringMap m_Headers, m_ResponseHeaders;
public:
	/// \brief 构造
	GsHttpClient();
	virtual ~GsHttpClient();

	/// \brief 获取请求的方法POST/GET
	GsString Method();

	/// \brief 设置请求的方法POST/GET
	void Method(const char* method);

	/// \brief 请求头信息
	GsStringMap& RequestHeaders();

	/// \brief 最近一次状态值
	GsHttpStatus LastStatus();

	/// \brief 下载url地址返回内容为字符串数据
	/// \param url   要下载的url地址
	Utility::GsString Download(const char* url);

	/// \brief 下载url地址返回内容为二进制数据
	/// \param url   要下载的url地址
	/// \param pOutputBuffer  输出数据的buffer
	GsHttpStatus Download(const char* url,GsByteBuffer* pOutputBuffer);

	/// \brief 下载url地址返回内容为文件
	/// \param url   要下载的url地址
	/// \param file 输出数据的文件
	GsHttpStatus Download(const char* url,const char* file);

	 
	/// \brief 启动回调下载，通过Callback回调
	GsHttpStatus Download(const char* url,GsHttpCallback* pCallBack);
};


UTILITY_ENDNS
