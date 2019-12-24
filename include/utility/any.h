#pragma once
#include "preconfig.h"  
#include "stringhelp.h"
UTILITY_NS
class GsRefObject;
/// \brief GsAny对象的数据类型
enum GsVarType
{
	/// \brief 未知数据
	eUnknownVarType =-2,
	/// \brief 空数据
	eEmpty=-1,
	/// \brief char
	eI1=0,
	/// \brief short
	eI2,
	/// \brief int
	eI4,
	/// \brief long long
	eI8,
	/// \brief unsigned char
	eUI1,
	/// \brief unsigned short
	eUI2,
	/// \brief unsigned int
	eUI4,
	/// \brief unsigned long long
	eUI8,
	/// \brief float
	eR4,
	/// \brief double
	eR8,
	/// \brief const char*
	eString,
	/// \brief bool
	eBool,
	/// \brief GsRefObject*
	eObject,
	/// \brief 二进制数据
	eBlob,
	
	/// \brief 时间日期类型
	eDateTime,
};
 

/// \brief 存储各种类型数据的对象
class  GS_API GsAny
{
public:
	/// \brief 数据类型
	GsVarType Type;
	/// \brief 数据联合体
	union
	{
		char			cVal;
		unsigned char	ucVal;
		short			sVal;
		unsigned short	usVal;
		int				iVal;
		unsigned int	uiVal;
		long long		llVal;
		unsigned long long ullVal;
		float			fltVal;
		double			dblVal;
		bool			boolVal;
		GsRefObject*		objVal;
		char*			strVal;
		unsigned char* blbVal;

		long long dateVal;
	};
	virtual ~GsAny();
	
	/// \brief 构造函数重载
	GsAny();
	/// \brief 单字符构造函数
	GsAny(char v);
	/// \brief 无符号字符构造函数
	GsAny(unsigned char v);
	/// \brief 16位的构造函数
	GsAny(short v);
	/// \brief 无符号16位的构造函数
	GsAny(unsigned short v);
	/// \brief 32位的构造函数
	GsAny(int v);
	/// \brief 无符号32位的构造函数
	GsAny(unsigned int v);
	/// \brief 64位的构造函数
	GsAny(long long v,bool bDateTime = false);
	/// \brief 无符号64位的构造函数
	GsAny(unsigned long long v);
	/// \brief 32位浮点构造函数
	GsAny(float v);
	/// \brief 64位浮点构造函数
	GsAny(double v);
	/// \brief bool类型构造函数
	GsAny(bool v);
	/// \brief GsRefObject类指针的构造函数
	GsAny(GsRefObject* v);
	/// \brief 字符常量构造函数
	GsAny(const char* v);
	/// \brief 字符常量构造函数
	GsAny(const GsString& str);

	/// \brief 无符号字符常量构造函数
	GsAny(const unsigned char* v,int nLen);
	/// \brief 类引用构造函数
	GsAny(const GsAny& rhs);

	/// \brief 符号重载，返回引用类型
	GsAny& operator =(const GsAny& rhs);
	/// \brief 符号重载，返回引用类型
	GsAny& operator =(char v);
	/// \brief 符号重载，返回引用类型
	GsAny& operator =(unsigned char v);
	/// \brief 符号重载，返回引用类型
	GsAny& operator =(short v);
	/// \brief 符号重载，返回引用类型
	GsAny& operator =(unsigned short v);
	/// \brief 符号重载，返回引用类型
	GsAny& operator =(int v);
	/// \brief 符号重载，返回引用类型
	GsAny& operator =(unsigned int v);
	/// \brief 符号重载，返回引用类型
	GsAny& operator =(long long v);
	/// \brief 符号重载，返回引用类型
	GsAny& operator =(unsigned long long v);
	/// \brief 符号重载，返回引用类型
	GsAny& operator =(float v);
	/// \brief 符号重载，返回引用类型
	GsAny& operator =(double v);
	/// \brief 符号重载，返回引用类型
	GsAny& operator =(bool v);
	/// \brief 符号重载，返回引用类型
	GsAny& operator =(const char* v);
	/// \brief 符号重载，返回引用类型
	GsAny& operator =(const GsString& str);

	/// \brief 符号重载，返回引用类型
	GsAny& operator =(GsRefObject* v);

	void Clear();
	
	/// \brief 获取字符
	char			AsChar();
	/// \brief 获取无符号字符
	unsigned char	AsUChar();
	/// \brief 获取短整型数
	short			AsShort();
	/// \brief 获取16位短整型数
	unsigned short	AsUShort();
	/// \brief 获取32位整数
	int				AsInt();
	/// \brief 获取32位无符号整数
	unsigned int	AsUInt();
	/// \brief 获取64位整数
	long long		AsLongLong();
	/// \brief 获取64位无符号整数
	unsigned long long	AsULongLong();
	/// \brief 获取32位浮点数
	float			AsFloat();
	/// \brief 获取64位浮点数
	double			AsDouble();
	/// \brief 获取bool的值
	bool			AsBool();
	/// \brief 获取GsRefObject类
	GsRefObject*	AsObj();
	/// \brief 获取字符串
	char*			AsString();
	/// \brief 获取二进制数据块
	void AsBlob(unsigned char * bBlob);

	/// \brief 类型转换
	operator char()const;
	/// \brief 类型转换
	operator unsigned char()const;
	/// \brief 类型转换
	operator short()const;
	/// \brief 类型转换
	operator unsigned short()const;
	/// \brief 类型转换
	operator int()const;
	/// \brief 类型转换
	operator unsigned int()const;
	/// \brief 类型转换
	operator long long()const;
	/// \brief 类型转换
	operator unsigned long long() const;
	/// \brief 类型转换	
	operator bool()const;
	/// \brief 类型转换
	operator float()const;
	/// \brief 类型转换
	operator double()const;
	/// \brief 类型转换
	operator const char*()const;
	/// \brief 类型转换
	operator GsRefObject*();

	
	/// \brief 设置blob数据
	void Set(const unsigned char* bBlob,int nLen);

	/// \brief 分配空的blob空间，并设置类型为blob
	unsigned char* AllocBlob(int nLen);

	/// \brief 数据的大小
	///\return 返回数据的长度
 	int ValueSize()const ;

	/// \brief 数据的头指针
	///\return 返回数据的头指针地址
 	const void* ValuePtr()const;

	
	/// \brief 计算对象的哈希值
	///\return 返回哈希值
	unsigned long long HashCode()const;

	/// \brief 数据的文本
	///\return 返回数据的文本内容
	GsString ToString();
};


/// \brief 各类数据计算为Hash值
class GS_API GsHash
{
public:
	template<class T>
	static T CombineHash(T h1, T h2)
	{
		return h1 ^ h2;
	}

	/// \brief 64位Hash计算32位Hash
	static int Hash32(unsigned long long hash64);
	/// \brief 128位hash计算64位hash
	static unsigned long long Hash64(const  GsPair<unsigned long long, unsigned long long>& hash128);

	/// \brief 任意内存块计算64位Hash
	static unsigned long long Hash64(const unsigned char* blob,int nLen);
	/// \brief 计算字符串的64位Hash
	static unsigned long long Hash64(const char* str,int nLen = -1);

	/// \brief 任意内存块计算128位Hash
	static GsPair<unsigned long long,unsigned long long> Hash128(const unsigned char* blob,int nLen);
	/// \brief 字符串计算128位Hash
	static GsPair<unsigned long long,unsigned long long> Hash128(const char* str,int nLen = -1);

	/// \brief 任意数据结构的Hash
	template<class T>
	static unsigned long long Hash64T(const T& v)
	{
		return Hash64((const unsigned char*)&v, sizeof(T));
	}
	/// \brief 任意数据结构的Hash
	template<class T>
	static GsPair<unsigned long long, unsigned long long> Hash128T(const T& v)
	{
		return Hash128((const unsigned char*)&v, sizeof(T));
	}

	/// \brief 任意内存块计算MD5
	static GsPair<unsigned long long, unsigned long long> HashMD5(const unsigned char* blob, int nLen);
	/// \brief 计算文件MD5
	static GsPair<unsigned long long, unsigned long long> HashMD5(const char* strFile);

	/// \brief 任意内存块计算MD5
	static GsString MD5(const unsigned char* blob,int nLen);

	/// \brief 计算文件MD5
	static GsString MD5(const char* strFile);
};
 
UTILITY_ENDNS
