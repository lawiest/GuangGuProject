#pragma once 
#include <geodatabase.h> 
#include "../../utility/preconfig.h"

KERNEL_NS 
	 
/// \brief OracleSptial数据库类厂
class GS_API GsOracleSpatialGeoDatabaseFactory:public GsGeoDatabaseFactory
{
private:
	GsRefObject* m_ptrInnterHandle;
	/// \brief 客户端连接的字符集名称
	Utility::GsString  m_charset;
public:
	GsOracleSpatialGeoDatabaseFactory();
	virtual ~GsOracleSpatialGeoDatabaseFactory();

	/// \brief 根据数据库连接信息打开一个空间数据库
	///\param connProperty 数据库连接信息
	///\return 返回空间数据库对象实例或者空
	virtual GsGeoDatabasePtr Open(const GsConnectProperty& connProperty);

	/// \brief 初始化GeoStar5的元表
	///\param connProperty Oracle数据库中GeoStar用户连接信息
	///\return 初始化是否成功。
	virtual bool InitializeGeoStar5Metatable(const GsConnectProperty& connProperty);

	/// \brief 判断一个空间数据库是否是GeoStar5的数据库
	///\param connProperty Oracle数据库中
	///\return 返回数据库连接的数据库是否是geostar5版本的数据库。
	virtual bool IsGeoStar5SpatialDatabase(const GsConnectProperty& connProperty);

};

/// \brief GsOracleSpatialGeoDatabaseFactoryPtr
GS_SMARTER_PTR(GsOracleSpatialGeoDatabaseFactory);
DECLARE_CLASS_CREATE(GsOracleSpatialGeoDatabaseFactory);

/// \brief MySql数据库类厂
class GS_API GsMySqlGeoDatabaseFactory:public GsGeoDatabaseFactory
{
	std::map<Utility::GsString, GsGeoDatabasePtr> m_mapDBCache;
public:
	GsMySqlGeoDatabaseFactory();
	virtual ~GsMySqlGeoDatabaseFactory();

	/// \brief 根据数据库连接信息打开一个空间数据库
	///\param connProperty 数据库连接信息
	///\return 返回空间数据库对象实例或者空
	virtual GsGeoDatabasePtr Open(const GsConnectProperty& connProperty);
};

/// \brief GsMySqlGeoDatabaseFactoryPtr
GS_SMARTER_PTR(GsMySqlGeoDatabaseFactory);
DECLARE_CLASS_CREATE(GsMySqlGeoDatabaseFactory);
 


/// \brief PostGIS数据库类厂
class GS_API GsPostGISGeoDatabaseFactory:public GsGeoDatabaseFactory
{
public:
	GsPostGISGeoDatabaseFactory();
	virtual ~GsPostGISGeoDatabaseFactory();

	/// \brief 根据数据库连接信息打开一个空间数据库
	///\param connProperty 数据库连接信息
	///\return 返回空间数据库对象实例或者空
	virtual GsGeoDatabasePtr Open(const GsConnectProperty& connProperty);
};

/// \brief GsPostGISGeoDatabaseFactoryPtr
GS_SMARTER_PTR(GsPostGISGeoDatabaseFactory);
DECLARE_CLASS_CREATE(GsPostGISGeoDatabaseFactory);
 


/// \brief 达梦数据库类厂
class GS_API GsDamengGeoDatabaseFactory:public GsGeoDatabaseFactory
{
	Utility::GsRefObject*  m_ptrInnterHandle;
public:
	GsDamengGeoDatabaseFactory();
	virtual ~GsDamengGeoDatabaseFactory();

	/// \brief 根据数据库连接信息打开一个空间数据库
	///\param connProperty 数据库连接信息
	///\return 返回空间数据库对象实例或者空
	virtual GsGeoDatabasePtr Open(const GsConnectProperty& connProperty);
};

/// \brief GsDamengGeoDatabaseFactoryPtr
GS_SMARTER_PTR(GsDamengGeoDatabaseFactory);
DECLARE_CLASS_CREATE(GsDamengGeoDatabaseFactory);
 
class GS_API GsRedisGeoDatabaseFactory :public GsGeoDatabaseFactory
{
public:
	GsRedisGeoDatabaseFactory();
	virtual ~GsRedisGeoDatabaseFactory();

	/// \brief 根据数据库连接信息打开一个空间数据库
	///\param connProperty 数据库连接信息
	///\return 返回空间数据库对象实例或者空
	virtual GsGeoDatabasePtr Open(const GsConnectProperty& connProperty);
};

/// \brief GsRedisGeoDatabaseFactoryPtr
GS_SMARTER_PTR(GsRedisGeoDatabaseFactory);
DECLARE_CLASS_CREATE(GsRedisGeoDatabaseFactory);

/// 	
/// \brief RVDB数据库类厂
/// 必须使用 GsSocketEnvironment m_Sockt;
class GS_API GsRVDBGeoDatabaseFactory :public GsGeoDatabaseFactory
{
public:
	GsRVDBGeoDatabaseFactory();
	virtual ~GsRVDBGeoDatabaseFactory();

	/// \brief 根据数据库连接信息打开一个空间数据库
	///\param connProperty 数据库连接信息
	///\return 返回空间数据库对象实例或者空
	virtual GsGeoDatabasePtr Open(const GsConnectProperty& connProperty);
};

/// \brief GsRVDBGeoDatabaseFactoryPtr
GS_SMARTER_PTR(GsRVDBGeoDatabaseFactory);
DECLARE_CLASS_CREATE(GsRVDBGeoDatabaseFactory);


/// 	
/// \brief FileGDB数据库类厂
class GS_API GsGDBGeoDatabaseFactory :public GsGeoDatabaseFactory
{
public:
	GsGDBGeoDatabaseFactory();
	virtual ~GsGDBGeoDatabaseFactory();

	/// \brief 根据数据库连接信息打开一个空间数据库
	///\param connProperty 数据库连接信息
	///\return 返回空间数据库对象实例或者空
	virtual GsGeoDatabasePtr Open(const GsConnectProperty& connProperty);
};

/// \brief GsGDBGeoDatabaseFactoryPtr
GS_SMARTER_PTR(GsGDBGeoDatabaseFactory);
DECLARE_CLASS_CREATE(GsGDBGeoDatabaseFactory);


/// \brief 桌面数据库环境初始化
class GsPCGeoDatabase
{
	static void RegisterDatabaseFactory()
	{
#define ENABLE_REDIS
#ifdef _WIN32
#if _MSC_VER <= 1600
#undef ENABLE_REDIS
#endif 
#endif
		Utility::GsString strModuleName = "gspcgeodatabaseport";
#ifdef _DEBUG
		strModuleName = "gspcgeodatabaseportd";
#endif // DEBUG

		
#ifdef ENABLE_REDIS
		REG_DELAY_CLASS_CREATE(GsRedisGeoDatabaseFactory, strModuleName.c_str());
		REG_DELAY_CLASS_CREATE_ALIAS(GsRedisGeoDatabaseFactory,"RedisGeoDatabaseFactory", strModuleName.c_str());
		
		REG_DELAY_CLASS_CREATE(GsRVDBGeoDatabaseFactory, strModuleName.c_str());
		REG_DELAY_CLASS_CREATE_ALIAS(GsRVDBGeoDatabaseFactory, "RVDBGeoDatabaseFactory", strModuleName.c_str());	
#endif

		REG_DELAY_CLASS_CREATE(GsDamengGeoDatabaseFactory, strModuleName.c_str());
		REG_DELAY_CLASS_CREATE_ALIAS(GsDamengGeoDatabaseFactory, "DamengGeoDatabaseFactory", strModuleName.c_str());
		
		REG_DELAY_CLASS_CREATE(GsPostGISGeoDatabaseFactory, strModuleName.c_str());
		REG_DELAY_CLASS_CREATE_ALIAS(GsPostGISGeoDatabaseFactory, "PostGISGeoDatabaseFactory", strModuleName.c_str());
		
		REG_DELAY_CLASS_CREATE(GsMySqlGeoDatabaseFactory, strModuleName.c_str());
		REG_DELAY_CLASS_CREATE_ALIAS(GsMySqlGeoDatabaseFactory, "MySqlGeoDatabaseFactory", strModuleName.c_str());
		
		REG_DELAY_CLASS_CREATE(GsOracleSpatialGeoDatabaseFactory, strModuleName.c_str());
		REG_DELAY_CLASS_CREATE_ALIAS(GsOracleSpatialGeoDatabaseFactory, "OracleSpatialGeoDatabaseFactory", strModuleName.c_str());

		REG_DELAY_CLASS_CREATE(GsGDBGeoDatabaseFactory, strModuleName.c_str());
		REG_DELAY_CLASS_CREATE_ALIAS(GsGDBGeoDatabaseFactory, "GsGDBGeoDatabaseFactory", strModuleName.c_str());
	} 
public:
	 

	/// \brief 初始化
	static void Initialize()
	{
		static bool bInit = false;
		static Utility::GsLock lock;
		if (bInit)
			return;
		{
			Utility::GsAutoLocker l(lock);
			RegisterDatabaseFactory();
			bInit = true;
		}
	}
};
KERNEL_ENDNS
