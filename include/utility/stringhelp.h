#pragma once
#include "preconfig.h"  
#include "vectorhelp.h" 
#include "stlhelp.h"

UTILITY_NS 


/// \brief 单字节字符串,兼容std::string方法
class GS_API GsString :public GsStlString
{
public:
	/// \brief 缺省构造
	GsString();

	/// \brief 拷贝构造
	GsString(const GsString& str);

	/// \brief stl字符串拷贝构造
	GsString(const GsStlString& str);
	 

	/// \brief 标准std::string构造
	template<class STD>
	GsString(const STD& str)
	{
		operator=(str.c_str());
	} 
	/// \brief 单字节字符串构造
	GsString(const char* str); 
	/// \brief 单字节字符串构造
	GsString(char* str);

	/// \brief 单字节字符串构造，偏移和长度
	GsString(const char* str, int off, int count);
	/// \brief 单字节字符串构造，指定长度
	GsString(const char* str, int count);
	/// \brief 根据字符串头尾指针构造
	GsString(const char* start, const char* end);
	/// \brief 根据任意内存构造
	GsString(const unsigned char* start, const unsigned char* end);
	
	/// \brief 重复字节
	GsString(int count,char e);
	
	/// \brief 根据迭代器构造
	GsString(GsString::const_iterator first, GsString::const_iterator last);
	/// \brief 根据迭代器构造
	GsString(GsString::iterator first, GsString::iterator last);
	/// \brief 根据迭代器构造
	GsString(GsString::reverse_iterator first, GsString::reverse_iterator last);
	/// \brief 根据迭代器构造
	GsString(GsString::const_reverse_iterator first, GsString::const_reverse_iterator last);


	~GsString();

	/// \brief 判断字符串是否为空
	static bool IsNullOrEmpty(const char* str);

	/// \brief 赋值操作符
	GsString& operator = (const char* str);
	/// \brief 赋值操作符
	GsString& operator = (const GsStlString& str);
	/// \brief 赋值操作符
	GsString& operator = (const GsString& str); 
	
	///// \brief std::string重载操作符
	//template<class STD>
	//operator STD() const
	//{
	//	return STD(c_str());
	//} 
	/// \brief bool重载操作符，判断字符串是否为空
	operator bool() const;

	/// \brief cosnt char* 重载操作符
	operator const char*() const;



	using GsStlString::operator+=;
	/// \brief +=操作符
	template<class STD>
	GsString& operator += (const STD& str)
	{
		GsStlString::operator+=(str.c_str());
		return *this;
	}
	/// \brief +=操作符
	GsString& operator += (const GsString& str);
	 
	/// \brief +操作符
	GsString  operator + (const char* str)const ;
	/// \brief +操作符
	GsString operator + (const GsString& str)const ;
	/// \brief +操作符
	GsString operator + (const GsStlString& str) const;
};
/// \brief +操作符
static GsString operator +(const GsString& a, const std::string& b)
{
	return a.operator+(b.c_str());
}

/// \brief 宽字节字符串
class GS_API GsWString :public GsStlWString
{
public:
	/// \brief 缺省构造
	GsWString();
	/// \brief 拷贝构造
	GsWString(const GsWString& str);
	/// \brief stl字符串构造
	GsWString(const GsStlWString& str);
	 
	/// \brief std::wstring构造字符串构造
	template<class STD>
	GsWString(const STD& str)
	{
		operator =(str.c_str());
	}
	/// \brief 宽字节构造
	GsWString(const wchar_t* str);
	/// \brief 宽字节构造
	GsWString(wchar_t* str);

	/// \brief 宽字节构造，偏移和长度
	GsWString(const wchar_t* str, int off, int count);
	/// \brief 宽字节构造，指定长度
	GsWString(const wchar_t* str, int count);
	/// \brief 宽字节构造，字符串头尾指针
	GsWString(const wchar_t* start, const wchar_t* end);
	/// \brief 重复构造
	GsWString(int count, wchar_t e);
	/// \brief 迭代器构造
	GsWString(GsWString::iterator first, GsWString::iterator last);
	/// \brief 迭代器构造
	GsWString(GsWString::const_iterator first, GsWString::const_iterator last);
	/// \brief 迭代器构造
	GsWString(GsWString::reverse_iterator first, GsWString::reverse_iterator last);
	/// \brief 迭代器构造
	GsWString(GsWString::const_reverse_iterator first, GsWString::const_reverse_iterator last);

	~GsWString();

	/// \brief 判断字符串是否为空
	static bool IsNullOrEmpty(const wchar_t* str); 
	
	/// \brief 等于赋值操作符
	GsWString& operator = (const wchar_t* str);
	/// \brief 等于赋值操作符
	GsWString& operator = (const GsStlWString& str);
	/// \brief 等于赋值操作符
	GsWString& operator = (const GsWString& str); 

	/// \brief std::wstring操作符重载
	template<class STD>
	operator STD() const
	{
		return STD(c_str());
	}

	/// \brief bool操作符重载，标识字符串是否为空
	operator bool() const;

	using GsStlWString::operator+=;

	/// \brief +=操作符
	template<class STD>
	GsWString& operator += (const STD& str)
	{
		GsStlWString::operator+=(str.c_str());
		return *this;
	}
	/// \brief +=操作符
	GsWString& operator += (const GsWString& str);
	/// \brief +=操作符
	GsWString& operator += (const wchar_t* str);



	 

	/// \brief +操作符
	GsWString operator + (const wchar_t* str) const;
	/// \brief +操作符
	GsWString operator + (const GsStlWString& str)const;
	/// \brief +操作符
	GsWString operator + (const GsWString& str)const;
};

/// \brief +操作符
static GsWString operator +(const GsWString& a, const std::wstring& b) 
{
	return a.operator+(b.c_str());
}



/// \brief 参数结构类型（key-value结构）
struct GsKeyValuePair
{
	GsString Key;
	GsString Value;
	GsKeyValuePair()
	{

	}
	GsKeyValuePair(const char* k, const char* v):Key(k),Value(v)
	{

	}
	GsKeyValuePair(const GsString& k, const GsString& v) :Key(k), Value(v)
	{

	}

};

/// \brief 增加字符串KeyValue对
class GS_API GsStringMap
{
	void* m_Map;
	bool m_bIgnorecase;
public:
	/// \brief 构造，根据Key是否区分大小写
	GsStringMap(bool bIgnorecase = false);
	/// \brief 拷贝构造
	GsStringMap(const GsStringMap& rhs);
	~GsStringMap();

	/// \brief Key是否忽略大小写
	bool IgnoreCase()const; 

	/// \brief 增加KeyValue对
	void AddPair(const GsKeyValuePair& pari);
	/// \brief 增加KeyValue对
	void AddPair(const char* k,const char* v);
	/// \brief 增加KeyValue对
	void AddPair(const GsString& k, const GsString& v);

	/// \brief 数量
	int Count()const ;
	/// \brief 移除
	void Remove(const char* k);
	/// \brief 移除
	void Remove(const GsString& k);
	/// \brief 移除所有
	void Clear();
	/// \brief []操作符
	GsString& operator [](const char* key);
	/// \brief []操作符
	GsString& operator [](const GsString& key); 
	/// \brief []操作符
	const GsString& operator [](const char* key)const;
	/// \brief []操作符
	const GsString& operator [](const GsString& key)const;

	/// \brief 赋值操作符
	GsStringMap& operator =(const GsStringMap& rhs);

	/// \brief 判断Key是否存在
	bool Exists(const char* k)const;
	/// \brief 判断Key是否存在
	bool Exists(const GsString& k)const;

	/// \brief 获取Key的值
	GsString& Value(const char* key);
	/// \brief 获取Key的值
	GsString& Value(const GsString& key);
	/// \brief 获取Key的值
	const GsString& Value(const char* key)const;
	/// \brief 获取Key的值
	const GsString& Value(const GsString& key)const;
};

class GsByteBuffer;

/// \brief 常用的正则表达式
enum GsWellknownRegex
{
	/// \brief 匹配整数
	eMatchInt,
	/// \brief 匹配浮点数
	eMatchFloat,

};
/// \brief 正则表达式标识
enum GsRegexFlags
{
	/// \brief 无标识
	eNoFlag=0,
	/// \brief 单行
	eSingleline = 0x01,
	/// \brief 多行
	eMultiline = 0x02,
	/// \brief 全局
	eGlobal = 0x04,
	/// \brief 忽略大小写
	eIgnorecase = 0x08,
	/// \brief 从右到做匹配
	eRightToLeft = 0x10,
	/// \brief 扩展
	eExtended = 0x20
};
/// \brief 正则表达上下文
class GS_API GsRegexpContext
{
protected:
	GsRegexpContext();
public:
	virtual ~GsRegexpContext();
	/// \brief 释放上下文对象
	static void ReleaseContext(GsRegexpContext * pContext);
};

/// \brief 正则表达式匹配结果
class GS_API  GsMatchResult:public GsVector<int>
{
	int At(int nIndex, int def)const;
public:
	/// \brief 是否成功匹配
	bool IsMatched() const;
	/// \brief 开始的位置
	int Start() const;
	/// \brief 结束的位置
	int End  () const;

public:
	/// \brief 最大组数量
	int MaxGroupNumber() const;
	/// \brief 第N个组的开始位置
	int GroupStart(int nGroupNumber) const;
	/// \brief 第N个组的结束位置
	int GroupEnd  (int nGroupNumber) const;

public: 
	/// \brief 拷贝构造
	GsMatchResult(const GsMatchResult & from);
	/// \brief 从匹配结果数组构造
	GsMatchResult(const int* val,int nlen);
	/// \brief 从上下文构造
	GsMatchResult(GsRegexpContext * pContext = 0, int nMaxNumber = -1);
	/// \brief 赋值操作
	GsMatchResult & operator = (const GsMatchResult& rhs);

	/// \brief 是否匹配
	operator bool() const;
};


/// \brief 正则表达式
class GS_API GsRegexp
{
	void* m_Handle;
	GsString m_Description;
	GsString m_Pattern;
public:
	/// \brief 缺省构造
	GsRegexp();
	/// \brief 从正则表达式和标识构造
	GsRegexp(const char* pattern, GsRegexFlags flag = eNoFlag);
	~GsRegexp();
	
	/// \brief 获取常用的正则表达式匹配串
	static GsString WellknownRegex(GsWellknownRegex eRegex);

	/// \brief 判断字符串是否符合正则表达式的模式
	static bool IsMatch(const char* str, const char* strPattern);


	/// \brief 正则表达式的描述
	GsString Description();
	/// \brief 正则表达式的描述
	void Description(const char* desc);
	/// \brief 模式定义字符串
	GsString Pattern();

	/// \brief 重新设置正则表达式
	bool Compile(const char * pattern, GsRegexFlags flag = eNoFlag);
	
	/// \brief 精确匹配
	GsMatchResult MatchExact(const char * tstring) const;
	/// \brief 匹配
	GsMatchResult Match(const char * tstring) const;
	/// \brief 从指定开始位置查找
	GsMatchResult Match(const char * tstring, int start) const;

	/// \brief 根据上下文匹配
	GsMatchResult Match(GsRegexpContext * pContext) const;
	
	/// \brief 根据分组的名称获得分组的索引
	int NamedGroupNumber(const char * group_name) const;
	
	/// \brief 准备匹配生成上下文
	GsRegexpContext * PrepareMatch(const char * tstring) const;
	/// \brief 准备匹配生成上下文
	GsRegexpContext * PrepareMatch(const char * tstring, int start) const;
	/// \brief 替换
	GsString Replace(const char * tstring, const char* replaceto, int start = -1, int ntimes = -1, GsMatchResult * result = 0, GsRegexpContext * pContext = 0) const;
	/// \brief 替换
	GsString Replace(const char* tstring, int string_length, const char* replaceto, int to_length, int start = -1, int ntimes = -1, GsMatchResult * result = 0, GsRegexpContext * pContext = 0) const;
	
};


/// \brief 编码id
enum GsCodePageID
{
	eCP_ACP = 0,
	eCP_GBK=936,
	eCP_UTF16 = 1200,
	eCP_UTF7 = 65000,
	eCP_UTF8 = 65001,
	eCP_GB2312=20936,
	eCP_GB18030=54936,
	
};
/// \brief 宽字符串转窄字符
class GS_API GsCW2A
{ 
	
public:
	GsString m_Str;
	GsCW2A(const wchar_t* str);
	GsCW2A(const wchar_t* str,GsCodePageID eCodePage);
	GsCW2A(const wchar_t* str,const char* codepage);
	GsCW2A(const GsWString& str);
	GsCW2A(const GsWString& str,GsCodePageID eCodePage);
	GsCW2A(const GsWString& str,const char* codepage);

	~GsCW2A() throw();
	operator const char*() const;
	operator GsString() const;
private:
	bool Init(const wchar_t* str,const char* codepage);
	bool Init(const wchar_t* str, GsCodePageID eCodePage);

private:
	GsCW2A(const GsCW2A&);
	GsCW2A& operator=(const GsCW2A&);
};

/// \brief 窄字符串转宽字符
class GS_API GsCA2W
{ 
public:
	GsWString m_WStr;
	GsCA2W(const char* str);
	GsCA2W(const char* str,GsCodePageID eCodePage);
	GsCA2W(const char* str,const char* codepage);
	GsCA2W(const GsString& str);
	GsCA2W(const GsString& str,GsCodePageID eCodePage);
	GsCA2W(const GsString& str,const char* codepage);

	~GsCA2W() throw();
	operator const wchar_t*() const;
	operator GsWString() const;
private:
	bool Init(const char* str,const char* codepage);
	bool Init(const char* str,GsCodePageID eCodePage);

private:
	GsCA2W(const GsCA2W&);
	GsCA2W& operator=(const GsCA2W&);
};


/// \brief 实现字符串编码相关的功能
class GS_API GsEncoding
{
public:
	GsEncoding();

	/// \brief 将本地字符串转换为utf8字符串
	///\return 返回转换为utf8编码的字符串
	static GsString ToUtf8(const char* str);

	/// \brief 将本地Unicode字符串转换为utf8字符串
	///\return 返回转换为utf8编码的字符串
	static GsString ToUtf8(const wchar_t* str);

	/// \brief 将本地Unicode字符串转换为MBCS的本地窄字符串
	///\return 返回转换为utf8编码的字符串
	static GsString ToLocal(const wchar_t* str);

	/// \brief 将utf8的窄字符串转换Unicode宽字符串
	///\return 返回Unicode编码的字符串
	static GsWString Utf8ToUnicode(const char* str);
	
	/// \brief 将MBCS本地的窄字符串转换Unicode宽字符串
	///\return 返回Unicode编码的字符串
	static GsWString LocalToUnicode(const char* str);

	/// \brief 将Utf8的字符串转换为本地字符串
	///\return 返回转换为本地编码的字符串。
	static GsString ToLocal(const char* str);

};

/// \brief 将字符串转换为utf8字符串
class GS_API GsUtf8
{
	const char* m_Ori;
	GsString m_Utf;
public:
	 GsUtf8(const char* ori);
	explicit GsUtf8(const GsString& ori);
	operator const char*();
	operator GsString();
	GsString Str();
};

/// \brief 将字符串转换为本地字符串
class GS_API GsLocal
{
	const char* m_Ori;
	GsString m_Utf;
public:
	GsLocal(const char* ori);
	GsLocal(const GsString& ori);
	operator const char*();
	operator GsString();
	GsString Str();
};



/// \brief 将任意数据转化为字符串
/// \details 
/// GsString str =  GsStringStream() <<"Number_"<<i;
class GS_API GsStringStream :public GsStlSStream
{
	GsString m_str;
public:
	GsStringStream();
	virtual ~GsStringStream();
	GsString Str();
	const char* CStr();
	operator GsString ();
	operator const char* ();
	template<class STD>
	operator STD()
	{
		return STD(CStr());
	}
}; 

/// \brief 提供字符串相关的一些功能函数。
class GS_API GsStringHelp
{
public:
	/// \brief 空字符串
	static GsString EmptyString();

	/// \brief 比较两个字符串
	static int Compare(const char* strA, const char* strB, bool bIgnoreCase = true);
	/// \brief 比较两个字符串，忽略大小写
	static int CompareNoCase(const char* strA, const char* strB);
	/// \brief 判断两个字符串是否相等。
	static bool IsEqual(const char* strA, const char* strB, bool bIgnoreCase = true);
	
	/// \brief 转换为字符串对象
	static GsString ToString(const char* str);
	/// \brief 转换为字符串对象
	static GsString ToString(char v);
	/// \brief 转换为字符串对象
	static GsString ToString(unsigned char v);
	/// \brief 转换为字符串对象
	static GsString ToString(short v);
	/// \brief 转换为字符串对象
	static GsString ToString(unsigned short v);
	/// \brief 转换为字符串对象
	static GsString ToString(int v);
	/// \brief 转换为字符串对象
	static GsString ToString(unsigned int v);
	/// \brief 转换为字符串对象
	static GsString ToString(long long v);
	/// \brief 转换为字符串对象
	static GsString ToString(unsigned long long v);
	/// \brief 转换为字符串对象
	static GsString ToString(float v, int _DigitCount = 8);
	/// \brief 转换为字符串对象
	static GsString ToString(double v, int _DigitCount = 18);

	/// \brief 转换为字符串对象
	static GsString ToString(bool v);
	/// \brief 转换为字符串对象
	static GsString ToString(const unsigned char* blob,int nLen);

	/// \brief 格式化字符串
	static GsString Format(int nMaxLen,const char* format,...);


	/// \brief 字符串是否为空或者无效字符串
	static bool IsNullOrEmpty(const char* str);

	/// \brief 字符串是否为空或者无效字符串
	static bool IsNullOrEmpty(const wchar_t* str);

		/// \brief 将字符串分割为若干子串
	static GsVector<GsString> Split(const char* str,const char* strSep);

	/// \brief 将字符串分割为若干子串
	static GsVector<GsString> Split(const char* str,char sp);

	
	/// \brief 将字符串中的制定字符串替换为新的部分
	static GsString Replace(const char* str,const char* src,const char* dst);



	/// \brief 剔除字符串前面的空白字符
	static GsString Trim(const char* str);

	/// \brief 剔除字符串前面空白字符
	static GsString TrimLeft(const char* str);

	/// \brief 剔除字符串尾部的空白字符
	static GsString TrimRight(const char* str);

	/// \brief 字符串是否以某个之串开头。
	static bool StartWith(const char* str,const char* strHead,bool bIgnoringCase = true);

	/// \brief 字符串是否以某个之结束。
	static bool EndWith(const char* str,const char* strTail,bool bIgnoringCase = true);


	
	/// \brief 通配符匹配字符串
	/// \details 通过 *a  来匹配   aaaa或者bbba或者ccca等，*代表1到N个字符，？代表1个字符
	///\param  pattern 通配符匹配模式如（*?abc)
	///\param str 要匹配的字符串
	static bool IsMatch(const char* pattern,const char* str);

	/// \brief 将字符串转化为大写
	static GsString ToUpper (const char* str);

	/// \brief 将字符串转化为小写
	static GsString ToLower(const char* str);

	/// \brief 字符串是否全部由整数的字符构成（包含负号）
	static bool IsIntString(const char* str);

	/// \brief 字符串是否全部由数字的字符构成（包含负号）
	static bool IsNumberString(const char* str);
	
	/// \brief 字符串是否全部由浮点数的字符构成（包含负号）
	static bool IsFloatString(const char* str);
	/// \brief 字符串是否是布尔类型的数据
	static bool IsBoolString(const char* str);


	/// \brief 将AABB00模式的16进制字符串转换为任意字符串
	static GsString FromHexString(const char* str);
	/// \brief 将任意字符串转换成AABB00模式的16进制字符串
	static GsString ToHexString(const char *str);

	/// \brief 将AABB00模式的16进制字符串转换为二进制块
	static bool FromHexString(const char* str,GsByteBuffer* pOut);
	/// \brief 将任意二进制块转换成AABB00模式的16进制字符串
	static GsString ToHexString(const unsigned char *blob,int nLen);

	/// \brief 从字符串解析布尔数据
	static bool ParseBool(const char* strValue, bool bDefault = false);

	/// \brief 尝试解析布尔数据
	static bool TryParse(const char* strValue, bool& val);

	/// \brief 从字符串解析整数
	static long long ParseInteger(const char* strValue, long long nDefault = 0);

	/// \brief 尝试解析整数
	static bool TryParse(const char* strValue, long long& val);
	/// \brief 尝试解析整数
	static bool TryParse(const char* strValue, int& val);


	/// \brief 从字符串解析浮点数
	static double ParseFloat(const char* strValue, double fDefault = 0);

	/// \brief 尝试解析浮点数
	static bool TryParse(const char* strValue, double& val);


};
UTILITY_ENDNS