//********************************************************************************** 
//** 文件名称：CT3D_3DGroup.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 几何组合体类的定义
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-14
//********************************************************************************** 


#ifndef _INC_C3DGROUP_49E3472302EE_INCLUDED
#define _INC_C3DGROUP_49E3472302EE_INCLUDED

#include "ModelDef.h"
#include "CT3D_3DGeometry.h"

#include <utility>
#include <vector>
using namespace std;
typedef pair<int ,C3DGeometry*>_GeometryPair; //几何对象的大小和指针值对
typedef vector<_GeometryPair> _GeometryList;
class /*DLL_EXPORT*/ C3DGroup 
: public C3DGeometry
{
	/** @defgroup C3DGroup C3DGroup-几何组合体  
    *  @{
    */

protected:
	
    //几何对象数
     int m_nGeometryNum;

	/** 几何对象集合，以vector为容器，存取几何对象的指针 */
	_GeometryList m_vGeoCollection;
	
	/** @} */ // 属性结尾

public:

	
	/** @defgroup C3DGroup 操作（成员函数）
	* @{
	*/
	

	/** @name 构造与析构函数
	*  @{
	*/

	//********************************************************************************** 
	//** 函数名称： C3DGroup
	//** 功能描述： 构造函数1
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	C3DGroup()
	{
		m_byTypeID = GEOTYPE_3D_GROUP;
		
	}

	//********************************************************************************** 
	//** 函数名称： C3DGroup
	//** 功能描述： 构造函数2
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	C3DGroup(const _GeometryList& geoCollection)
	{
		m_byTypeID = GEOTYPE_3D_GROUP;
        int nNum = geoCollection.size();
        m_vGeoCollection.reserve(nNum);
        for(int i=0;i<nNum;i++)
        {
            m_vGeoCollection.push_back(geoCollection[i]);
        }
	}

	//********************************************************************************** 
	//** 函数名称： ~C3DGroup
	//** 功能描述： 构造函数3
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	virtual ~C3DGroup()
	{
		Clear();
	}
	
	/** @} */ // 构造与析构函数结尾
	
	
	
	/** @name 数据操作
	*  @{
	*/

	//********************************************************************************** 
	//** 函数名称： SetCollection
	//** 功能描述： 设置几何组合体
	//** 输    入： 已知几何组合体geoCollection          
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	void SetCollection(const _GeometryList& geoCollection)
	{
		for(size_t i = 0; i < geoCollection.size();i++)
        {
            _GeometryPair geopair(geoCollection[i].first,geoCollection[i].second);
            m_vGeoCollection.push_back(geopair);
        }

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
	void GetCollection(_GeometryList& geoCollection) const
	{
        for(size_t i = 0; i < m_vGeoCollection.size();i++)
        {
            _GeometryPair geopair(m_vGeoCollection[i].first,m_vGeoCollection[i].second);
            geoCollection.push_back(geopair);
        }
	}
	
	//********************************************************************************** 
	//** 函数名称： RemoveAll
	//** 功能描述： 清空存储几何对象指针的容器，释放容器中元素指针管理的内存(暂时不释放内存)
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	void RemoveAll()
	{
		m_vGeoCollection.clear();
	}
	
	//********************************************************************************** 
	//** 函数名称： Clear
	//** 功能描述： 清空存储几何对象指针的容器，释放容器中元素指针管理的内存(暂时不释放内存)
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	void Clear()
	{
		int nCount = m_vGeoCollection.size();
		for(int i=nCount-1; i>-1; i--)
		{
			if(m_vGeoCollection[i].second != NULL)
			{
				delete m_vGeoCollection[i].second; 
                m_vGeoCollection[i].second = NULL;
			}
		}
		RemoveAll();
	}
		
	//********************************************************************************** 
	//** 函数名称： CreateObject
	//** 功能描述： 开辟一个当前类对象所需的内存空间
	//** 输    入： 无           
	//** 输    出： 无
	//** 返回值：	无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-16
	//**********************************************************************************
	virtual CGeometry* CreateObject();
	
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

	void setGeometryNum(int nGeometryNum)
	{
		m_nGeometryNum=nGeometryNum;
	}

	int getGeometryNum()
	{
		return m_nGeometryNum;
	}

	virtual size_t sizeBuffer();

};
#endif /* _INC_C3DGROUP_49E3472302EE_INCLUDED */
