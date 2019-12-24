//********************************************************************************** 
//** 文件名称：Feature.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 要素类定义
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-7
//********************************************************************************** 
#ifndef _FEATURE_H_
#define _FEATURE_H_

#include <osg/Referenced>
#include <string>
#include <vector>
#include <fstream>
#include "Buffer.h"
#include <osg/Vec3d>
#include "CT3D_Vertex3d.h"


//typedef struct tagLocation
//{
//	union
//	{  
//		struct
//		{
//			double dLongitude;//经度
//			double dLatitude;//纬度
//			double dElevation;//高程
//		};
//
//		struct
//		{
//			double X,Y,Z;
//		};
//		struct
//		{
//			double dblX,dblY,dblZ;
//		};
//		struct
//		{
//			double x,y,z;
//		};
//	}; 
//
//}Location;//定位
//
//typedef struct tagRotation
//{
//	union
//	{  
//		struct
//		{
//			double dLongitudeDegree;//绕经线方向
//			double dLatitudeDegree;//绕纬线方向
//			double dNormalDegree;//绕法线方向
//		};
//
//		struct
//		{
//			double X,Y,Z;
//		};
//		struct
//		{
//			double dblX,dblY,dblZ;
//		};
//		struct
//		{
//			double x,y,z;
//		};
//	}; 
//
//}Rotation;//旋转
//
//typedef struct tagZoom
//{
//	union
//	{  
//		struct
//		{
//			double dLongitudeOrientation;//经线方向
//			double dLatitudeOrientation;//纬线方向
//			double dNormalOrientation;//法线方向
//		};
//
//		struct
//		{
//			double X,Y,Z;
//		};
//		struct
//		{
//			double dblX,dblY,dblZ;
//		};
//		struct
//		{
//			double x,y,z;
//		};
//	}; 
//
//}Zoom;//缩放





using namespace std;

/**LOD结构体*/
class LOD_INFO_BASE{
public:
	LOD_INFO_BASE();
	LOD_INFO_BASE(const LOD_INFO_BASE& lodInfo);

	virtual bool readBuffer(Buffer &buffer);
	virtual void writeBuffer(Buffer &buffer);
	virtual size_t sizeBuffer();

	
	void copy(const LOD_INFO_BASE& lodInfo);
public:
    byte	    byRelation;		//与上一级别LOD的关系
    double		dSwitchIn;		//该LOD层次的最近可视距离
    double		dSwitchOut;		//该LOD层次的最远可视距离
    UINT		unDataSize;		//该LOD层次的几何数据数据量
    UINT	    unTriangleNum;	//该LOD层次的几何数据三角面片个数
    UINT	    unVertexNum;	//该LOD层次的几何数据顶点数
	//std::string dateName;
};

//扩展Lod信息
class  LOD_INFO : public LOD_INFO_BASE
{
public:
	virtual bool readBuffer(Buffer &buffer);
	virtual void writeBuffer(Buffer &buffer);
	virtual size_t sizeBuffer();

	void copy(const LOD_INFO& lodInfo);
public:
	UINT64      unGeometryID;   //该LOD层次的几何数据的ID
	std::string dateName;
	//std::vector<MTData> m_vecMTData; //该LOD层次的模型所对应的材质数据

	void clear();
};

typedef vector<LOD_INFO> LOD_INFOList;//要素带的LOD信息列表


//5.0.2
class  CFeature:public osg::Referenced
{
public:
//********************************************************************************** 
//** 函数名称： CFeature
//** 功能描述： 构造函数
//** 输    入： 无          
//** 输    出： 无
//** 返回值：无
//** 作    者： 杨灵
//** 创建日期： 2012-04-07
//**********************************************************************************
    CFeature(void);

//********************************************************************************** 
//** 函数名称： CFeature
//** 功能描述： 拷贝构造函数
//** 输    入： feature          
//** 输    出： 无
//** 返回值：无
//** 作    者： 杨灵
//** 创建日期： 2012-04-07
//**********************************************************************************
    CFeature( CFeature& feature );

//********************************************************************************** 
//** 函数名称： ~CFeature
//** 功能描述： 析构函数
//** 输    入： 无          
//** 输    出： 无
//** 返回值：无
//** 作    者： 杨灵
//** 创建日期： 2012-04-07
//**********************************************************************************

    ~CFeature(void);
    

public:
	
	std::string _dbName;

    /** 要素ID*/
    UINT64   m_nFID;
    
    /** 要素名称长度*/
    int m_strNameLen;
   
    /** 要素名称*/
    std::string  m_strName;

    /** 要素对象对应的要素类id*/
    UINT64 m_nFCLID;
    
    /** 要素类型编码长度*/
    int m_nCodeLen;
    /** 要素类型编码*/	
    string		 m_strCode; 

    /** 状态*/
    int  m_nState;
    /** 定位*/
    Location m_location;
    
    /** 旋转*/
    Rotation m_rotation;
    
    /** 缩放*/
    Zoom m_zoom;
 
    UINT64 m_uFeatureCreateTime;//要素创建时间
    UINT64 m_uFeatureUpdateTime;//要素更新时间

    /** 经纬度AABB的最小顶点位置(X,Y,Z最小）[对象绝对坐标]*/
   // Vertex3d m_MinVertex;
	osg::Vec3d m_MinVertex;
   
    /** 经纬度AABB的最大顶点位置(X,Y,Z最小）[对象绝对坐标]*/
  //  Vertex3d m_MaxVertex;
	osg::Vec3d m_MaxVertex;

    /** 要素LOD的总级数*/
     byte m_byLodLeverNum;
    /** LOD信息*/
    LOD_INFOList m_LOD_Info_List;
public:
	int  _curLOD;
	inline void computeCurLOD(double distance)
	{
		for (int i=0;i<m_LOD_Info_List.size();i++)
		{
			double min=m_LOD_Info_List[i].dSwitchIn;
			double max=m_LOD_Info_List[i].dSwitchOut;
			if (distance<max&&distance>=min)
			{
				_curLOD=i;
				return;
			}
		}
		_curLOD=-1;
	}
	//********************************************************************************** 
	//** 函数名称： readBuffer
	//** 功能描述： 从缓冲区中读数据
	//** 输    入： buffer:Buffer对象的引用           
	//** 输    出： 无
	//** 返回值：成功为true，失败为false
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	virtual bool readBuffer(Buffer &buffer);

	//********************************************************************************** 
	//** 函数名称： writeBuffer
	//** 功能描述： 将数据写到缓冲区
	//** 输    入： buffer:Buffer对象的引用          
	//** 输    出： 无
	//** 返回值：无
	//** 作    者： 杨灵
	//** 创建日期： 2012-04-11
	//**********************************************************************************
	virtual void writeBuffer(Buffer &buffer);
    
    //********************************************************************************** 
    //** 函数名称： sizeBuffer
    //** 功能描述： 得到要素大小
    //** 输    入： 无          
    //** 输    出： 无
    //** 返回值：要素大小
    //** 作    者： 杨灵
    //** 创建日期： 2012-04-11
    //**********************************************************************************
    size_t sizeBuffer();

    //********************************************************************************** 
    //** 函数名称： readBufferForService
    //** 功能描述： 从缓冲区中读数据,此处不读取要素定义中的FeatureClassID
    //** 输    入： buffer:Buffer对象的引用           
    //** 输    出： 无
    //** 返回值：成功为true，失败为false
    //** 作    者： 杨灵
    //** 创建日期： 2012-04-11
    //**********************************************************************************
    virtual bool readBufferForService(Buffer &buffer);

    //********************************************************************************** 
    //** 函数名称： writeBufferForService
    //** 功能描述： 将数据写到缓冲区，此处不写入要素定义中的FeatureClassID
    //** 输    入： buffer:Buffer对象的引用          
    //** 输    出： 无
    //** 返回值：无
    //** 作    者： 杨灵
    //** 创建日期： 2012-04-11
    //**********************************************************************************
    virtual void writeBufferForService(Buffer &buffer);

    //********************************************************************************** 
    //** 函数名称： sizeBuffer
    //** 功能描述： 得到要素大小,此处不计算要素定义中的FeatureClassID
    //** 输    入： 无          
    //** 输    出： 无
    //** 返回值：要素大小
    //** 作    者： 杨灵
    //** 创建日期： 2012-06-08
    //**********************************************************************************
    size_t sizeBufferForService();
    
private:

//********************************************************************************** 
//** 函数名称： copy
//** 功能描述： 拷贝传入的要素对象的各项属性
//** 输    入： feature:要素对象的引用         
//** 输    出： 无
//** 返回值：无
//** 作    者： 杨灵
//** 创建日期： 2012-04-07
//**********************************************************************************
    void copy(const CFeature& feature);
public:
	void setFeatureID(UINT64 nfid);
	UINT64 getFeatureID();

	void setFeatureName(string strFeatureName);
	string getFeatureName();
	int getFeatureNameLen();

	void setFCLID(UINT64 nFCLID);
	UINT64 getFCLID();

	void setCode(string strCode);
	string getCode();
	int getCodeLen();

	void setStatus(int nState);
	int getStatus();

	void setLocation(Location v);
	Location getLocation();

	void setRotation(Rotation v);
	Rotation getRotation();

	void setZoom(Zoom v);
	Zoom getZoom();

	void setLodLeverNum(byte bLodLeverNum);
	byte getLodLeverNum();

	void setLOD_INFOList(LOD_INFOList& v);
	LOD_INFOList& getLOD_INFOList();

    //void getMinVertex(Vertex3d& minVertex);
    //void getMaxVertex(Vertex3d& maxVertex);

protected:

	//拷贝基础信息
 	virtual void copyBaseInfo(const CFeature& feature);

	//读写基础信息
	//bForService 是否为客户端数据
	virtual bool readBaseInfo(Buffer& buffer, bool bForService);
	virtual void writeBaseInfo(Buffer& buffer, bool bForService);

	//读写数据
	//bForService 是否为客户端数据
	virtual bool readBuffer(Buffer& buffer, bool bForService);
	virtual void writeBuffer(Buffer& buffer, bool bForService);

};

typedef vector<LOD_INFO_BASE> LodInofBaseList;//要素带的LOD信息列表

//5.0 feature结构
class  CFeature50 : public CFeature
{
	LodInofBaseList m_vecLodInfo;
public:
	LodInofBaseList& getLOD_INFOList()
	{
		return m_vecLodInfo;
	}

protected:
	//读写数据
	//bForService 是否为客户端数据
	virtual bool readBuffer(Buffer& buffer, bool bForService);
	virtual void writeBuffer(Buffer& buffer, bool bForService);
};

#endif