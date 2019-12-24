//********************************************************************************** 
//** 文件名称：CT3D_3DFeatureInfoGroup.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维要素组合类定义
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-11
//********************************************************************************** 
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#pragma warning(disable:4251)
#endif
#ifndef _CT3D_3DFEATUREINFOGROUP_H_
#define _CT3D_3DFEATUREINFOGROUP_H_

#include "ModelDef.h"
#include "CT3D_Geometry.h"
#include "CT3D_AABBox.h"
#include "CT3D_3DGeometry.h"



typedef struct tagFeature_Info
{
	UID64 fid;//对象ID
	Envelope3D box3d;//包围盒
}FEATURE_INFO;

class /*DLL_EXPORT*/ C3DFeatureInfoGroup :
	public C3DGeometry
{
	/** @defgroup C3DGroup C3DGroup-几何组合体  
	*  @{
	*/

protected:
	/** @name 属性（成员变量）
	* @{
	*/
	/** 几何对象个数 */
	int m_nFeatureinfoNum;

	/** 几何对象集合，以vector为容器，存取几何对象*/
	std::vector<FEATURE_INFO> m_vFeatureInfos;

	/** @} */ // 属性结尾

public:
	/** @defgroup C3DGroup 操作（成员函数）
	* @{
	*/


	/** @name 构造与析构函数
	*  @{
	*/

	//********************************************************************************** 
	//** 函数名称： C3DFeatureInfoGroup
	//** 功能描述： 构造函数1
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	C3DFeatureInfoGroup(void)
	{
		m_byTypeID = GEOTYPE_3D_FEATUREINFOGROUP;
	}

	//********************************************************************************** 
	//** 函数名称： C3DFeatureInfoGroup
	//** 功能描述： 拷贝构造函数
	//** 输    入： 几何对象集合featureInfos的引用          
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	C3DFeatureInfoGroup(const vector<FEATURE_INFO>& featureInfos)
	{
		m_byTypeID = GEOTYPE_3D_FEATUREINFOGROUP;
		m_vFeatureInfos = featureInfos;
	}

	//********************************************************************************** 
	//** 函数名称： C3DFeatureInfoGroup
	//** 功能描述： 构造函数2
	//** 输    入： FEATURE_INFO结构体指针，结构体对象个数          
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	C3DFeatureInfoGroup( FEATURE_INFO* const featureInfos, int nNum)
	{
		m_byTypeID = GEOTYPE_3D_FEATUREINFOGROUP;
		m_vFeatureInfos.reserve(nNum);
		for(int i=0;i<nNum;i++)
			m_vFeatureInfos.push_back(featureInfos[i]);
	}

	//********************************************************************************** 
	//** 函数名称： ~C3DFeatureInfoGroup
	//** 功能描述： 析构函数
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	virtual ~C3DFeatureInfoGroup(void){}

	/** @} */ // 构造与析构函数结尾


	/** @name 数据操作
	*  @{
	*/

	
	//********************************************************************************** 
	//** 函数名称： SetCollection
	//** 功能描述： 设置几何组合体
	//** 输    入： 几何组合体geoCollection            
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	void SetCollection(const vector<FEATURE_INFO>& geoCollection)
	{
		m_vFeatureInfos = geoCollection;
	}

	//********************************************************************************** 
	//** 函数名称： SetCollection
	//** 功能描述： 向存储几何对象指针的容器中添加数据
	//** 输    入： 指向一串几何对象的指针geoCollection ；待添加的几何对象个数nNum           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	void SetCollection( FEATURE_INFO* const geoCollection, int nNum)
	{
		m_vFeatureInfos.clear();
		m_vFeatureInfos.reserve(nNum);
		for(int i=0;i<nNum;i++)
			m_vFeatureInfos.push_back(geoCollection[i]);
	}

	//********************************************************************************** 
	//** 函数名称： GetCollection
	//** 功能描述： 获得存储几何对象指针容器的引用
	//** 输    入： 待赋值的存储几何对象指针容器的引用geoCollection            
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	void GetCollection(vector<FEATURE_INFO>& geoCollection) const
	{
		geoCollection = m_vFeatureInfos;
	}


	/** @} */ // 数据操作组结尾

	//********************************************************************************** 
	//** 函数名称： CreateObject
	//** 功能描述： 开辟一个当前类对象所需的内存空间
	//** 输    入： 无            
	//** 输    出： 无
	//** 返回值：	开辟的当前类对象内存空间的首地址
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	CGeometry* CreateObject();

	//********************************************************************************** 
	//** 函数名称： readBuffer
	//** 功能描述： 从缓冲区中解析当前对象，并给成员变量赋值
	//** 输    入： buf	缓冲区Buffer对象的引用            
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	bool readBuffer( Buffer & buf);

	//********************************************************************************** 
	//** 函数名称： writeBuffer
	//** 功能描述： 把当前对象的成员变量数据，打包成一个缓冲区
	//** 输    入： buf	缓冲区Buffer对象的引用            
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	void writeBuffer(Buffer& buf);
public:
	void setFeatureinfoNum(int nFeatureinfoNum);
	int getFeatureinfoNum();
	virtual size_t sizeBuffer();


};

#endif