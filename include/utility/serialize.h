#pragma once
#include "preconfig.h" 
#include "object.h"
#include "any.h"
#include "configuration.h"
#include "timer.h"
UTILITY_NS


/// \brief 可序列化对象类厂
class GS_API GsSerializebleClassFactory :public GsRefObject
{
protected:
	GsSerializebleClassFactory();
public:
	virtual ~GsSerializebleClassFactory();

	/// \brief 创建类的实例
	/// \details  返回的对象的引用计数为1
	virtual GsRefObject* CreateInstance(const char* className) = 0;
};
/// \brief GsSerializebleClassFactoryPtr
GS_SMARTER_PTR(GsSerializebleClassFactory);


/// \brief 将多个可序列化对象类厂复合成一个对象
class GS_API GsMultiSerializebleClassFactory :public GsSerializebleClassFactory
{ 
	GsVector<GsSerializebleClassFactoryPtr> m_AllFactory;
public:
	GsMultiSerializebleClassFactory();
	virtual ~GsMultiSerializebleClassFactory();
	
	/// \brief 增加一个类厂
	void Add(GsSerializebleClassFactory* fac);
	/// \brief 清除所有类厂
	void Clear();
	/// \brief 数量
	int Count();
	/// \brief 移除一个
	GsSerializebleClassFactoryPtr Remove(int i);

	/// \brief 创建类的实例
	/// \details  返回的对象的引用计数为1
	virtual GsRefObject* CreateInstance(const char* className);

};
/// \brief GsMultiSerializebleClassFactoryPtr
GS_SMARTER_PTR(GsMultiSerializebleClassFactory);

/// \brief 序列化流基类接口
class GS_API GsSerializeStream :public GsRefObject
{
protected:
	GsSerializebleClassFactoryPtr m_ptrClassFactory;
	GsRefObject* CreateInstance(const char* className);

	GsSerializeStream(); 
public:
	/// \brief 用户实现自定义对象的类厂
	GsSerializebleClassFactory* ClassFactory();
	void ClassFactory(GsSerializebleClassFactory* fac);

	/// \brief 开始一个对象存储，需要调用EndObject封闭
	virtual bool BeginObject(const char* name, const char* type) = 0;

	/// \brief 结束一个对象存储
	virtual bool EndObject( ) = 0;


	/// \brief 根据属性名称获取一个属性的类型
	virtual GsVarType PropertyType(const char* name) = 0;
	/// \brief 判断一个属性是否存在
	virtual bool Exists(const char* name) = 0;

	/// \brief 保存任意属性
	/// \param const GsString& Name 属性名称
	/// \param GsAny * Value 属性值
	///\return 返回是否成功
	virtual bool SaveProperty(const char * strName,const GsAny & Value);
	
	/// \brief 读取任意属性
	/// \param const GsString& Name 属性名称
	/// \param GsAny * DefaultValue 默认值
	///\return 返回值
	virtual GsAny LoadProperty(const char *  strName, const GsAny& DefaultValue);

	/// \brief 设置指定key的指定字符串值
	/// \param const char* key  属性名称
	/// \param const char* strValue 属性值
	virtual void Save(const char* key,const char* strValue) = 0;


	/// \brief 设置指定key的指定字符串值
	/// \param const char* key  属性名称
	/// \param const char* strValue 属性值
	virtual void Save(const char* key, const GsString & strValue) = 0;

	/// \brief 设置指定key的值32位整形值
	/// \param const char* key  属性名称
	/// \param  int value 属性值
	virtual void Save(const char* key,int value) = 0;


	/// \brief 设置指定key的无符号32位整形值
	/// \param const char* key  属性名称
	/// \param  unsigned int value属性值
	virtual void Save(const char* key,unsigned int value) = 0;

	/// \brief 设置指定key的64位整形值
	/// \param const char* key  属性名称
	/// \param  long long value 属性值
	virtual void Save(const char* key,long long value)= 0;

	/// \brief 设置指定key的无符号64位整形值
	/// \param const char* key  属性名称
	/// \param  unsigned long long 属性值
	virtual void Save(const char* key,unsigned long long value) = 0;
	/// \brief 设置指定key的bool值
	/// \param const char* key  属性名称
	/// \param  bool value 值
	virtual void Save(const char* key,bool value) = 0;
	/// \brief 设置指定key的float值
	/// \param const char* key  属性名称
	/// \param  float value 值
	virtual void Save(const char* key,float value) = 0;

	/// \brief 设置指定key的double值
	/// \param const char* key  属性名称
	/// \param  double value 值
	virtual void Save(const char* key,double value) = 0;

	/// \brief 设置指定key的GsRefObject值
	/// \param const char* key  属性名称
	/// \param  GsRefObject* value 值
	virtual bool Save(const char* key, GsRefObject* value) = 0;

	/// \brief 存储时间
	/// \param const char* key  属性名称
	/// \param  GsDateTime 时间
	virtual void Save(const char* key, const GsDateTime& time) = 0;


	/// \brief 设置指定key的二进制值,二进制根据不同类型的流,可能实现不同的表达方式,
	/// \brief 常见的将二进制转为 base64编码存储打xml,json, 或者如geometry 可以转为geojson或者gml,wkt等
	/// \param const char* key  属性名称
	/// \param  GsAny * value 值
	virtual void Save(const char* key, const unsigned char* blob, int nlen) = 0;
	
	/// \brief 存储自定义类型
	/// \param key  属性名称
	/// \param type  属性类型描述
	/// \param value 属性值
	virtual void Save(const char* key, const char* type, const char* value) = 0;


	/// \brief 根据key读取字符串
	/// \param const char* key 键值
	/// \param const char* strDefaultValue 默认值
	///\return GsString 返回字符串
	virtual GsString LoadStringValue(const char* key, const char* strDefaultValue) = 0;
	/// \brief 根据key读取字符串
	/// \param const char* key 键值
	/// \param const char* strDefaultValue 默认值
	///\return GsString 返回字符串
	virtual GsString LoadStringValue(const char* key, const GsString& strDefaultValue);

	/// \brief 根据key读取int值
	/// \param const char* key 键值
	/// \param int intDefaultvalue 默认值
	///\return int 返回int
	virtual int LoadIntValue(const char* key, int intDefaultvalue) = 0;

	/// \brief 根据key读取unsigned int值
	/// \param const char* key 键值
	/// \param unsigned int uintDefaultvalue 默认值
	///\return int 返回unsigned int
	virtual unsigned int  LoadUIntValue(const char* key, unsigned int uintDefaultvalue) = 0;


	/// \brief 根据key读取long long值
	/// \param const char* key 键值
	/// \param long long llDefaultvalue 默认值
	///\return long long 返回long long
	virtual long long LoadInt64Value(const char* key, long long llDefaultvalue) = 0;

	/// \brief 根据key读取unsigned long long值
	/// \param const char* key 键值
	/// \param unsigned long long llDefaultvalue 默认值
	///\return unsigned long long 返回unsigned long long
	virtual unsigned long long LoadUInt64Value(const char* key, unsigned long long ullDefaultvalue) = 0;

	/// \brief 根据key读取bool值
	/// \param const char* key 键值
	/// \param bool bDefaultValue 默认值
	///\return bool 返回bool
	virtual bool LoadBoolValue(const char* key, bool bDefaultValue) = 0;

	/// \brief 根据key读取float 值
	/// \param const char* key 键值
	/// \param float fDefaultValue 默认值
	///\return float 返回float
	virtual	float LoadFloatValue(const char* key, float fDefaultValue) = 0;

	/// \brief 根据key读取 double  值
	/// \param const char* key 键值
	/// \param  double dblDefaultValue默认值
	///\return double 返回 double 
	virtual double LoadDoubleValue(const char* key, double dblDefaultValue) = 0;
	
	/// \brief 根据key读取 Time 值
	/// \param const char* key 键值
	/// \param  const GsDateTime& defaultValuee默认值
	///\return GsDateTime 返回 时间
	virtual GsDateTime LoadDateTimeValue(const char* key, const GsDateTime& defaultValue) = 0;

	/// \brief 根据key读取 double  值
	/// \param const char* key 键值
	/// \param  GsRefObject* bDefaultValue默认值
	///\return GsRefObject* 返回 GsRefObject* 
	virtual GsRefObject* LoadObjectValue(const char* key) = 0;

	/// \brief 根据key读取 double  值
	/// \param const char* key 键值
	/// \param  GsRefObject* bDefaultValue默认值
	///\return GsRefObject* 返回 GsRefObject* 
	template<class T>
	GsSmarterPtr<T>  LoadObjectValueT(const char* key)
	{
		GsRefObject* ref = LoadObjectValue(key);
		if (!ref) return 0;
		T* o = dynamic_cast<T*>(ref);
		if (!o)
		{
			ref->Release();
			return 0;
		}
		return GsSmarterPtr<T>(o, false);
	}

	/// \brief 根据key读取 GsAny  值
	/// \param const char* key 键值
	/// \param  GsAny* anyDefaultBlob默认值
	///\return GsAny 返回 GsAny 
	virtual GsAny LoadBlobValue(const char* key, const GsAny& anyDefaultBlob) = 0;

	/// \brief 载入用户自定义类型的数据
	/// \param  key 键值
	/// \param type 返回数据类型
	/// \return 返回数据的值
	virtual GsString LoadCustomValue(const char* key, GsString& type) = 0;

};
GS_SMARTER_PTR(GsSerializeStream);
/// \brief XML序列化流类
class GS_API GsXMLSerializeStream :public GsSerializeStream
{
	Utility::GsStack<GsConfig> m_StackConfig;
public:
	using GsSerializeStream::LoadStringValue;

	/// \brief xml序列化构造函数
	/// \param const char* strXML xml字符串
	GsXMLSerializeStream(const char* strXML=NULL);
	/// \brief 获取序列化的XML
	///\return GsString 字符串
	virtual GsString XML();
	/// \brief 设置序列化字符串
	/// \param const GsString& strXml 字符串
	///\return void
	virtual void XML(const char * strXml);

	/// \brief 开始一个对象存储，需要调用EndObject封闭
	virtual bool BeginObject(const char* name, const char* type) ;

	/// \brief 结束一个对象存储
	virtual bool EndObject( );


	/// \brief 根据属性名称获取一个属性的类型
	virtual GsVarType PropertyType(const char* name);
	/// \brief 判断一个属性是否存在
	virtual bool Exists(const char* name);
	/// \brief 存储自定义类型
	/// \param key  属性名称
	/// \param type  属性类型描述
	/// \param value 属性值
	virtual void Save(const char* key, const char* type, const char* value);
	/// \brief 载入用户自定义类型的数据
	/// \param  key 键值
	/// \param type 返回数据类型
	/// \return 返回数据的值
	virtual GsString LoadCustomValue(const char* key, GsString& type);



	/// \brief 设置指定key的指定字符串值
	/// \param const char* key  属性名称
	/// \param const char* strValue 属性值
	virtual void Save(const char* key, const char* strValue) ;


	/// \brief 设置指定key的指定字符串值
	/// \param const char* key  属性名称
	/// \param const char* strValue 属性值
	virtual void Save(const char* key, const GsString & strValue) ;

	/// \brief 设置指定key的值32位整形值
	/// \param const char* key  属性名称
	/// \param  int value 属性值
	virtual void Save(const char* key, int value) ;


	/// \brief 设置指定key的无符号32位整形值
	/// \param const char* key  属性名称
	/// \param  unsigned int value属性值
	virtual void Save(const char* key, unsigned int value) ;

	/// \brief 设置指定key的64位整形值
	/// \param const char* key  属性名称
	/// \param  long long value 属性值
	virtual void Save(const char* key, long long value) ;

	/// \brief 设置指定key的无符号64位整形值
	/// \param const char* key  属性名称
	/// \param  unsigned long long 属性值
	virtual void Save(const char* key, unsigned long long value);
	/// \brief 设置指定key的bool值
	/// \param const char* key  属性名称
	/// \param  bool value 值
	virtual void Save(const char* key, bool value) ;
	/// \brief 设置指定key的float值
	/// \param const char* key  属性名称
	/// \param  float value 值
	virtual void Save(const char* key, float value);

	/// \brief 设置指定key的double值
	/// \param const char* key  属性名称
	/// \param  double value 值
	virtual void Save(const char* key, double value);

	/// \brief 设置指定key的GsRefObject值
	/// \param const char* key  属性名称
	/// \param  GsRefObject* value 值
	virtual bool Save(const char* key, GsRefObject* value);

	/// \brief 存储时间
	/// \param const char* key  属性名称
	/// \param  GsDateTime 时间
	virtual void Save(const char* key, const GsDateTime& time);


	/// \brief 设置指定key的二进制值,二进制根据不同类型的流,可能实现不同的表达方式,
	/// \brief 常见的将二进制转为 base64编码存储打xml,json, 或者如geometry 可以转为geojson或者gml,wkt等
	/// \param const char* key  属性名称
	/// \param  GsAny * value 值
	virtual void Save(const char* key, const unsigned char* blob, int nlen);

	/// \brief 根据key读取字符串
	/// \param const char* key 键值
	/// \param const char* strDefaultValue 默认值
	///\return GsString 返回字符串
	virtual GsString LoadStringValue(const char* key, const char* strDefaultValue);

	/// \brief 根据key读取int值
	/// \param const char* key 键值
	/// \param int intDefaultvalue 默认值
	///\return int 返回int
	virtual int LoadIntValue(const char* key, int intDefaultvalue);

	/// \brief 根据key读取unsigned int值
	/// \param const char* key 键值
	/// \param unsigned int uintDefaultvalue 默认值
	///\return int 返回unsigned int
	virtual unsigned int  LoadUIntValue(const char* key, unsigned int uintDefaultvalue);


	/// \brief 根据key读取long long值
	/// \param const char* key 键值
	/// \param long long llDefaultvalue 默认值
	///\return long long 返回long long
	virtual long long LoadInt64Value(const char* key, long long llDefaultvalue);

	/// \brief 根据key读取unsigned long long值
	/// \param const char* key 键值
	/// \param unsigned long long llDefaultvalue 默认值
	///\return unsigned long long 返回unsigned long long
	virtual unsigned long long LoadUInt64Value(const char* key, unsigned long long ullDefaultvalue);

	/// \brief 根据key读取bool值
	/// \param const char* key 键值
	/// \param bool bDefaultValue 默认值
	///\return bool 返回bool
	virtual bool LoadBoolValue(const char* key, bool bDefaultValue);

	/// \brief 根据key读取float 值
	/// \param const char* key 键值
	/// \param float fDefaultValue 默认值
	///\return float 返回float
	virtual	float LoadFloatValue(const char* key, float fDefaultValue);

	/// \brief 根据key读取 double  值
	/// \param const char* key 键值
	/// \param  double dblDefaultValue默认值
	///\return double 返回 double 
	virtual double LoadDoubleValue(const char* key, double dblDefaultValue);

	/// \brief 根据key读取 Time 值
	/// \param const char* key 键值
	/// \param  const GsDateTime& defaultValuee默认值
	///\return GsDateTime 返回 时间
	virtual GsDateTime LoadDateTimeValue(const char* key, const GsDateTime& defaultValue);

	/// \brief 根据key读取 double  值
	/// \param const char* key 键值
	/// \param  GsRefObject* bDefaultValue默认值
	///\return GsRefObject* 返回 GsRefObject* 
	virtual GsRefObject* LoadObjectValue(const char* key);

	/// \brief 根据key读取 GsAny  值
	/// \param const char* key 键值
	/// \param  GsAny* anyDefaultBlob默认值
	///\return GsAny 返回 GsAny 
	virtual GsAny LoadBlobValue(const char* key, const GsAny& anyDefaultBlob);

};
GS_SMARTER_PTR(GsXMLSerializeStream);

/// \brief 序列化接口类
class GS_API GsSerialize
{
public:
	~GsSerialize() {}
	/// \brief 对象序列化为流
	/// \param GsSerializeStream * pSerStream
	///\return 返回是否成功
	virtual bool Serialize(GsSerializeStream * pSerStream) = 0;
	/// \brief 流序列化为对象
	/// \param GsSerializeStream * pSerStream
	///\return 返回是否成功
	virtual bool DeSerialize(GsSerializeStream * pSerStream) = 0;

	/// \brief 对象名称
	virtual GsString ClassName() = 0;

};

UTILITY_ENDNS