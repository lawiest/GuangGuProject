#pragma once 
#include "preconfig.h" 
#include "stlhelp.h"
#include "vectorhelp.h"
#include "stringhelp.h"
#include "crthelp.h"
UTILITY_NS 

/// \brief 枚举类型
template<typename ENUM>
class  GsEnumType
{
	GsVector<GsPair<GsString,ENUM>>  m_vecPair;
	void Push(const GsString& str ,ENUM value)
	{
		m_vecPair.emplace_back(str,value);
	}
	static GsEnumType<ENUM> m_Instance;
public:
	GsEnumType()
	{
	}
	/// \brief 获取某特定类型枚举实例
	static GsEnumType<ENUM>* Instance()
	{
		return &m_Instance;
	}
	/// \brief 从字符串获取枚举值
	ENUM FromString(const char* strValue,ENUM eDefault)
	{
		if(m_vecPair.empty())
			return eDefault;
		typename GsVector<GsPair<GsString,ENUM> >::iterator it = m_vecPair.begin();
		for(;it != m_vecPair.end();it++)
		{
			if(GsCRT::_stricmp(it->first.c_str(),strValue)==0)
				return it->second;
		}
		//无法匹配到字符串则将字符串当做数值来处理
		int nNumValue;
		//如果此时值不等于LONG_MIN则认为字符串成功解析成一个数字。
		if(GsStringHelp::TryParse(strValue, nNumValue))
		{
			it = m_vecPair.begin();
			for(;it != m_vecPair.end();it++)
			{
				if(nNumValue == it->second)
					return it->second;
			}
		}
		//此处在将来还可以加入考虑16进制的情况。

		return eDefault;
	}
	/// \brief 将枚举值转换成字符串
	GsVector<GsString> ToStringArray(ENUM eValue,const char* strDefault)
	{
		GsVector<GsString> vec;
		GsString strRet = (NULL == strDefault)?"":strDefault;
		typename GsVector<GsPair<GsString,ENUM> >::iterator it = m_vecPair.begin();
		for(;it != m_vecPair.end();it++)
		{
			if(it->second == eValue)
				vec.push_back(it->first);
			
		}
		if(vec.empty())
			vec.push_back(strRet);

		return vec;
	}
	/// \brief 将枚举值转换成字符串
	GsString ToString(ENUM eValue,const char* strDefault)
	{
		GsString strRet = (NULL == strDefault)?"":strDefault;
		typename GsVector<GsPair<GsString,ENUM> >::iterator it = m_vecPair.begin();
		for(;it != m_vecPair.end();it++)
		{
			if(it->second == eValue)
				return it->first;
		}

		return strRet;
	}

};
/// \brief 枚举解析器是将C++的枚举从字符串解析成枚举值，或者将枚举值解析为字符串。
class GS_API GsEnumParser
{
public: 
	/// \brief 从字符串解析枚举值
	template<typename ENUM>	
	static ENUM ParserFromString(const GsString& strValue,ENUM eDefault)
	{
		return ParserFromString(strValue.c_str(),eDefault);
	};
	/// \brief 从字符串解析枚举值
	template<typename ENUM>
	static ENUM ParserFromString(const char* strValue,ENUM eDefault)
	{
		return GsEnumType<ENUM>::Instance()->FromString(strValue,eDefault);
	}	
	/// \brief 枚举值转换字符串
	template<typename ENUM>
	static GsString ToString(ENUM eValue,const char* strDefault)
	{
		return GsEnumType<ENUM>::Instance()->ToString(eValue,strDefault);
	}
	/// \brief 枚举值转换字符串
	template<typename ENUM>
	static GsString ToString(ENUM eValue,const GsString& strDefault)
	{
		return GsEnumType<ENUM>::Instance()->ToString(eValue,strDefault.c_str());
	}
	/// \brief 枚举值转换字符串的多个结果
	template<typename ENUM>
	static GsVector<GsString> ToStringArray(ENUM eValue,const GsString& strDefault)
	{
		return GsEnumType<ENUM>::Instance()->ToStringArray(eValue,strDefault.c_str());
	}
};


/// \brief 枚举值直接当做字符串
#define ENUM_PUSH(X) Push(#X,X);
/// \brief 其他字符串对应的枚举值。
#define ENUM_PUSH2(K,V) Push(#K,V);

/// \brief 单件声明
#define ENUM_SINGLETONG(T) template<> GsEnumType<T> GsEnumType<T>::m_Instance=GsEnumType<T>();
/// \brief 特例化开始
#define ENUM_BEGIN_SPECIAL_CASE(T) template<> GsEnumType<T>::GsEnumType(){

/// \brief 特例化结束
#define ENUM_END_SPECIAL_CASE(T) };ENUM_SINGLETONG(T);

UTILITY_ENDNS