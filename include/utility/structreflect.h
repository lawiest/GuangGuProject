#pragma once 
#include "preconfig.h"  
#include "stringhelp.h" 
#include "stlhelp.h"
#include "any.h"
#include "locker.h"
UTILITY_NS 

/// \brief 反射对象
class GS_API GsReflectObject
{
	GsVector<GsPair<GsString, GsVarType> > m_FieldInfo;
	int m_nPack;
public:
	GsReflectObject(int nPack);
	GsReflectObject(const GsReflectObject& refObj);
	GsVector<GsPair<GsString, GsVarType> >& FieldInfo();;
	/// \brief 字节对齐
	int Pack();

};
/// \brief 反射对象池
class GS_API GsReflectPool
{
	GsLock m_Lock;
	GsStlMap<GsString, GsReflectObject> m_Map;
public:
	/// \brief 反射对象池单件
	static GsReflectPool& Singleton();
	/// \brief 获取一个反射对象信息
	GsReflectObject& Reflect(const char* name,int pack=4);
	/// \brief 获取一个反射对象信息
	GsReflectObject& Reflect(const GsString& name,int pack=4);

};
/// \brief 注册反射对象函数
#define GS_REGISTER_REFLECT(T) UTILITY_NAME::GsRegisterReflect ##T();
#define GS_DECLARE_REFLECT(T) extern "C" GS_API void GsRegisterReflect ##T();
#define GS_BEING_REFLECT_REGISTER(T,P) extern "C" GS_API void GsRegisterReflect ##T(){GsReflectObject& obj =GsReflectPool::Singleton().Reflect(#T,P);
#define GS_REFLECT_FIELD(X,Y)  obj.FieldInfo().emplace_back(X,Y);
#define GS_END_REFLECT_REGISTER }

UTILITY_ENDNS