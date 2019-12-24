#pragma once
#include "geodatabase.h" 

KERNEL_NS 
/// \brief ArcGIS文件数据源类厂
class GS_API GsESRIFileGeoDatabaseFactory :public GsFileSystemGeoDatabaseFactory
{
public:
	GsESRIFileGeoDatabaseFactory();
	virtual ~GsESRIFileGeoDatabaseFactory();

	/// \brief 根据数据库连接信息打开一个空间数据库
	///\param connProperty 数据库连接信息
	///\return 返回空间数据库对象实例或者空
	virtual GsGeoDatabasePtr Open(const GsConnectProperty& connProperty);
};
/// \brief GsMemoryGeoDatabaseFactoryPtr
GS_SMARTER_PTR(GsESRIFileGeoDatabaseFactory);
DECLARE_CLASS_CREATE(GsESRIFileGeoDatabaseFactory);

KERNEL_ENDNS
