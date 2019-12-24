#pragma once
#include "../utility/preconfig.h"
#include <object.h>  
#include <vectorhelp.h>
#include <geometry.h>
KERNEL_NS 

	
/// \brief 粗查空间索引对象抽象基类
class GS_API GsSpatialIndexQuery:public Utility::GsRefObject
{
public:
	virtual ~GsSpatialIndexQuery();
	/// \brief 根据矩形范围查询相交对象id
	/// \param box 要检索的矩形范围
	/// \param vecResult 返回和检索矩形相交的对象id
	/// \return 返回是否检索到对象
	virtual bool Query(const GsBox& box, Utility::GsVector<long long> &vecResult) = 0;
};
/// \brief GsSpatialIndexQueryPtr
GS_SMARTER_PTR(GsSpatialIndexQuery);

/// \brief 支持修改的空间索引抽象基类
class GS_API GsSpatialIndexEdit:public GsSpatialIndexQuery
{
public:
	virtual ~GsSpatialIndexEdit();
	/// \brief 增加一个对象到空间索引
	/// \param box 要增加对象的box
	/// \param nID 要增加对象的id
	/// \return 增加是否成功
	virtual bool Add(const GsBox& box,int nID) = 0;
	/// \brief 从空间索引移除一个对象
	/// \param nID 要移除对象的id
	/// \return 移除是否成功
	virtual bool Remove(int nID) = 0;
};
/// \brief GsSpatialIndexEditPtr
GS_SMARTER_PTR(GsSpatialIndexEdit);

 
/// \brief 基于本地QIX文件的只读R树空间索引
class GS_API GsQIXFileSpatialIndex:public GsSpatialIndexQuery
{
	void* m_pQIXHandle;
public:
	/// \brief 从QIX文件名构造
	GsQIXFileSpatialIndex(const char* strQIXFile);
	~GsQIXFileSpatialIndex();

	/// \brief 根据矩形范围查询相交对象id
	/// \param box 要检索的矩形范围
	/// \param vecResult 返回和检索矩形相交的对象id
	/// \return 返回是否检索到对象
	virtual bool Query(const GsBox& box, UTILITY_NAME::GsVector<long long> &vecResult);
	 
	/// \brief 查询多个维度
	/// \param minBound 维度最小的值
	/// \param maxBound 维度最大的值
	/// \param nDim 要检索的维度
	/// \param vecResult 查询结果
	/// \return 返回是否检索到对象
	virtual bool QueryMultiDimension(const double* minBound,const double* maxBound,int nDim, UTILITY_NAME::GsVector<long long> &vecResult);
	
};
/// \brief GsQIXFileSpatialIndexPtr
GS_SMARTER_PTR(GsQIXFileSpatialIndex);


/// \brief Geohash计算
class GS_API GsGeohash
{
	double m_xmin, m_ymin, m_xmax, m_ymax;
	static const int m_maxlen = 18;
	static const unsigned long long m_mask = 1ULL << 45;
	static const Utility::GsString m_lcdigits_;
	static const Utility::GsString m_ucdigits_;
	
	double m_shift; 
	double m_dx;
	double m_dy;
	double m_loneps;
	double m_lateps;

public:
	GsGeohash(double xmin =  -180.0, double ymin= -90.0, double xmax=180.0, double ymax = 90.0);

	/// \brief Geohash正算
	Utility::GsString Forward(double x, double y, int len);
	
	/// \brief Geohash反算。从字符串计算位置
	///	\param[in] geohash the geohash.
	///	\param[out] x 点的x坐标
	///	\param[out] y 点的y坐标
	///	\param[out] len geohash的长度.
	///	\param[in] centerp 如果真（缺省）则返回geohash位置的中心否则返回西南角坐标
	bool Reverse(const char* geohash, double&x, double &y, int& len, bool centerp = true);
};


KERNEL_ENDNS
