#ifndef _IDCONVERTER_H_
#define _IDCONVERTER_H_
#include <string>

using std::string;

typedef unsigned long long GeoINT64;
typedef unsigned long long FID;
typedef unsigned long long GID;
typedef unsigned long long MatID;
typedef unsigned long long UINT64;

class  IDConverter
{
public:
	IDConverter(void);
	~IDConverter(void);

	//********************************************************************************** 
	//** 函数名称： FeatureIDToGeometryID
	//** 功能描述： 通过要素ID和细节层次计算几何ID
	//** 输    入： 要素ID，细节层次           
	//** 输    出： 无
	//** 返回值：	几何ID
	//** 作    者： 吴晨
	//** 创建日期： 2012-05-17
	//**********************************************************************************
	static GID FeatureIDToGeometryID(FID featureID, int lodLevel);

	//********************************************************************************** 
	//** 函数名称： MaterialLodIDToMatID
	//** 功能描述： 通过Surface里面包含细节层次的材质ID计算真实的材质ID
	//** 输    入： 包含细节层次的材质ID，细节层次           
	//** 输    出： 无
	//** 返回值：	材质ID
	//** 作    者： 吴晨
	//** 创建日期： 2012-05-17
	//**********************************************************************************
	static MatID MaterialLodIDToMatID(MatID matLodID, int lodLevel);

	//********************************************************************************** 
	//** 函数名称： ConvertInt64ToString
	//** 功能描述： 将64位无符整型输出为字符串，用于检查位操作是否正确
	//** 输    入： 64位无符整型           
	//** 输    出： 无
	//** 返回值：	整形的字符串形式
	//** 作    者： 吴晨
	//** 创建日期： 2012-05-17
	//**********************************************************************************
	static string ConvertInt64ToString(GeoINT64 geoID);

	//********************************************************************************** 
	//** 函数名称： GetLodLevelFromFeatureID
	//** 功能描述： 输出FeatureID中的lod层数
	//** 输    入： 64位无符整型           
	//** 输    出： 无
	//** 返回值：	lod层数
	//** 作    者： 吴晨
	//** 创建日期： 2012-06-05
	//**********************************************************************************
	static int GetLodLevelFromFeatureID(FID featureID);

	//********************************************************************************** 
	//** 函数名称： GetLodLevelFromMaterialLodID
	//** 功能描述： 输出MatID中的lod层数
	//** 输    入： 64位无符整型           
	//** 输    出： 无
	//** 返回值：	lod层数
	//** 作    者： 吴晨
	//** 创建日期： 2012-06-05
	//**********************************************************************************
	static int GetLodLevelFromMaterialLodID(MatID materialID);

	//********************************************************************************** 
	//** 函数名称： CheckSharedModel
	//** 功能描述： 判断是否为共享模型的几何或材质
	//** 输    入： 64位无符整型           
	//** 输    出： 无
	//** 返回值：	1----是共享模型，0-----不是共享模型
	//** 作    者： 吴晨
	//** 创建日期： 2012-05-17
	//**********************************************************************************
	static int CheckSharedModel(GeoINT64 geoID);

};
#endif