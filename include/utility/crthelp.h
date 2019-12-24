#pragma once
#include "preconfig.h" 

UTILITY_NS 

/// \brief 封装c的运行时函数CRT 
class GS_API GsCRT
{
public:
	
	/// \brief 计算四舍五入的结果
	static double round(double r); 
 	/// \brief 按照进制转换字符串为64位整数
	/// \param str 开始指针
	/// \param endptr 结束指针
	/// \param base 进制2,4,8,10,16等
	static long long  strtoll (const char* str, char** endptr, int base);

	/// \brief 判断double是否是无效值
	///\return 返回1标示是有效的0标示无效
	static int _isnan(double x);
	/// \brief 判断double是否是无穷大、无穷小
	///\return 返回1标示是无穷大或无穷小的0标示不是
	static int _isinf(double x);

	
	/// \brief 比较两个字符串第n个字符长的大小，忽略大小写。
	///\return 返回1或0或-1
	static int strnicmp(const char *dst,const char *src,int count);

	/// \brief 比较两个字符串第n个字符长的大小
	///\return 返回1或0或-1
	static int strncmp(const char *dst,const char *src,int count);

	/// \brief 比较2个常量字符串的大小，忽略大小写
	///\return 返回1或0或-1
	static  int _stricmp(const char *string1,const char *string2);
 
	/// \brief 把常量字符串转换成long long类型
	///\return 返回long long类型整数
	static long long _atoi64(const char * _String);

	/// \brief 把常量字符串转换成double类型
	///\return 返回double类型浮点
	static double _atof(const char * _String);
	
	/// \brief 设定文件或文件夹的访问权限
	///\return 成功返回0，否则返回-1
	static int _access(const char * _Filename,int _AccessMode);
	
	/// \brief 转换浮点数据为字符串
	/// \param dblValue 要转换的浮点数
	/// \param nNumOfDigits 要使用的进制[2~16]
	/// \param pDstBuf 存储字符串的缓冲区
	///\return 返回输入缓冲区的首地址
	static const char *_gcvt(double dblValue, int nNumOfDigits, char * pDstBuf);

	/// \brief 转换整数为字符串
	/// \param nVal 要转换的整数数
	/// \param nRadix 要使用的进制[2~16]
	/// \param pDstBuf 存储字符串的缓冲区
	///\return 返回输入缓冲区的首地址
	static const char *_itoa(int nVal, char * pDstBuf, int nRadix);


};
UTILITY_ENDNS