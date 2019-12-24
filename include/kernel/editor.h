#pragma once
#include "../utility/preconfig.h"
#include <vectorhelp.h>
#include "map.h"

KERNEL_NS 

class GS_API GsFeatureIDCacheSegment :public Utility::GsRefObject
{
	unsigned int m_Start, m_End;
	Utility::GsVector<long long> m_Cache;
	GsFeatureClassPtr m_ptrFeaClass;
public:
	GsFeatureIDCacheSegment(GsFeatureClass* pFeaClass,int nStart);
	virtual ~GsFeatureIDCacheSegment();

	/// \brief 顺序ID开始的值
	unsigned int Start();

	/// \brief 最后的顺序ID
	unsigned int End();

	/// \brief 判断一个顺序ID是否属于这个段
	bool IsIn(unsigned int n);
	/// \brief 此段所关联的地物类
	GsFeatureClass* FeatureClass();

	/// \brief 获取顺序ID说关联的地物OID
	long long OID(unsigned int n);

	/// \brief 存储一个OID获取对应的顺序ID
	unsigned int Add(long long oid);

};
/// \brief GsFeatureIDCacheSegmentPtr
GS_SMARTER_PTR(GsFeatureIDCacheSegment);

/// \brief 实现地物ID的缓存
/// \details 任意地物缓存可以获取一个32位的顺序ID，通过顺序ID可以获取地物的OID和所属地物类
class GS_API GsFeatureIDCache :public Utility::GsRefObject
{
	Utility::GsList<GsFeatureIDCacheSegmentPtr> m_CacheSegment;
public:
	GsFeatureIDCache();
	~GsFeatureIDCache();
	/// \brief 缓存地物的ID获取顺序ID
	unsigned int CacheFeatureID(GsFeature* pFea);
	
	/// \brief 根据顺序ID获取地物OID和所属地物类
	GsFeatureClass* FeatureID(unsigned int orderID, long long &oid);
};
/// \brief GsFeatureIDCachePtr
GS_SMARTER_PTR(GsFeatureIDCache);

 
/// \brief 捕捉到的地物，地物以及地物所属的地物类。
class GS_API GsSnapFeature :public Utility::GsRefObject
{
	GsFeatureClassPtr m_FeatureClass;
	GsFeaturePtr    m_Feature; 
	GsGeometryPtr	m_ptrGeo;
	
public:
	
	GsSnapFeature(GsFeatureClass* pFeaClass, long long oid);
	GsSnapFeature(GsFeatureClass* pFeaClass, GsFeature* fea);
	GsSnapFeature(GsGeometry* geo);
	
	GsGeometryBlob* GeometryBlob();
	GsGeometryPtr Geometry();
};
/// \brief GsSnapFeaturePtr
GS_SMARTER_PTR(GsSnapFeature);


/// \brief 几何捕捉算法的名称
enum GsSnaperType
{
	/// \brief 节点
	eSnapNode,
	/// \brief 最近点
	eSnapNearest,
	/// \brief 中点
	eSnapMiddle,
	/// \brief 1/3点
	eSnapOneThird,
	/// \brief 1/4点
	eSnapOneFour,

	/// \brief 圆心
	eSnapCenter,

	/// \brief 垂足
	eSnapPer,

	/// \brief 水平
	eSnapHorizontal,

	/// \brief 垂直
	eSnapVertical,


	/// \brief 用户自定义捕捉
	eUserSnapType = 1000,

};


/// \brief 捕捉计算的上下文参数
class GS_API GsSnapContext
{
protected:
	double m_Tolerance;
	GsRawPoint* m_RefPoint;
	GsRawPoint m_SnapPoint;
	GsSnapFeaturePtr m_ptrSnapFeature;

	Utility::GsVector<GsSymbolPtr> m_SnapSymbols;
	
	GsSnapContext();
public:
	virtual ~GsSnapContext();
	/// \brief 捕捉计算的地物
	virtual  GsSnapFeature* SnapFeature();
	/// \brief 捕捉输入点，捕捉成功则修改此点的值
	virtual  GsRawPoint& Point();
	
	/// \brief 添加一个用于绘制捕捉结果的符号
	virtual void AddSnapSymbol(GsSymbol* sym);

	/// \brief 捕捉计算时的容差
	virtual double Tolerance();

	/// \brief 参考点，上一次编辑输入的点。
	virtual GsRawPoint* ReferencePoint();
};
/// \brief 捕捉算法
class GS_API GsSnaper:public Utility::GsRefObject
{
protected:
	bool m_Enable;
	GsSnaperType m_Type;
	Utility::GsString m_Name;
	GsSnaper();
	/// \brief 捕捉计算
	/// \param pt 输入要捕捉的坐标点，捕捉成功的话则修改其坐标值
	/// \param tol 捕捉的全局容差 
	virtual bool OnSnap(GsSnapContext* pContext)const = 0;
public:
	virtual ~GsSnaper();
	
	/// \brief 是否允许该算子
	bool &Enable();
	
	/// \brief 捕捉算法类型用于识别不同的捕捉算法
	GsSnaperType Type();

	/// \brief 捕捉算法的名称
	Utility::GsString Name();
	/// \brief 捕捉算法的名称
	void Name(const char* name);


	/// \brief 捕捉计算
	/// \param pt 输入要捕捉的坐标点，捕捉成功的话则修改其坐标值
	/// \param tol 捕捉的全局容差 
	virtual bool Snap(GsSnapContext* pContext)const;

};
/// \brief GsSnaperPtr
GS_SMARTER_PTR(GsSnaper);

/// \brief 捕捉Geometry的节点
class GS_API GsNodeSnaper:public GsSnaper
{
	GsSymbolPtr m_ptrSymbol;
public:
	GsNodeSnaper();
	~GsNodeSnaper();
protected:
	/// \brief 捕捉计算
	/// \param pt 输入要捕捉的坐标点，捕捉成功的话则修改其坐标值
	/// \param tol 捕捉的全局容差 
	virtual bool OnSnap(GsSnapContext* pContext)const;
};

/// \brief 捕捉Geometry的最近点
class GS_API GsNearestSnaper:public GsSnaper
{
	GsSymbolPtr m_ptrSymbol;
public:
	GsNearestSnaper();
	~GsNearestSnaper();
protected:
	/// \brief 捕捉计算
	/// \param pt 输入要捕捉的坐标点，捕捉成功的话则修改其坐标值
	/// \param tol 捕捉的全局容差 
	virtual bool OnSnap(GsSnapContext* pContext)const;
};

/// \brief 捕捉Geometry的垂足
class GS_API GsPerpendicularSnaper :public GsSnaper
{
	GsSymbolPtr m_ptrSymbol;
public:
	GsPerpendicularSnaper();
	~GsPerpendicularSnaper();
protected:
	/// \brief 捕捉计算
	/// \param pt 输入要捕捉的坐标点，捕捉成功的话则修改其坐标值
	/// \param tol 捕捉的全局容差 
	virtual bool OnSnap(GsSnapContext* pContext)const;
};

/// \brief 捕捉Geometry的特殊点(中点，三分之一点，四分之一点)
class GS_API GsPercentSnaper :public GsSnaper
{
	GsSymbolPtr m_ptrSymbol;
	int m_Division;
public:
	GsPercentSnaper(GsSnaperType pType);
	~GsPercentSnaper();
protected:
	/// \brief 捕捉计算
	/// \param pt 输入要捕捉的坐标点，捕捉成功的话则修改其坐标值
	/// \param tol 捕捉的全局容差 
	virtual bool OnSnap(GsSnapContext* pContext)const;
};
/// \brief 捕捉特定方向角的点
class GS_API GsAngleSnaper :public GsSnaper
{
	double m_dblAngle;
public:
	GsAngleSnaper(double dblAngle);
	~GsAngleSnaper();
protected:
	/// \brief 捕捉计算
	/// \param pt 输入要捕捉的坐标点，捕捉成功的话则修改其坐标值
	/// \param tol 捕捉的全局容差 
	virtual bool OnSnap(GsSnapContext* pContext)const;
};
/// \brief GsSnapResultPtr
GS_SMARTER_PTR(GsAngleSnaper);

/// \brief 捕捉结果
class GS_API GsSnapResult:public Utility::GsRefObject
{
protected: 
	GsRawPoint  m_OriPoint,m_SnapPoint; 
	GsSnaperPtr m_ptrSnaper;
	GsRawPoint* m_RefPoint;
	GsSnapResult();
public:
	virtual ~GsSnapResult();
	
	/// \brief 原始点
	const GsRawPoint& OriginalPoint() const;
	/// \brief 捕捉到的坐标
	const GsRawPoint& SnapPoint() const;

	/// \brief 参考点
	const GsRawPoint* ReferencePoint() const;

	/// \brief 捕捉算法
	const GsSnaper* Snaper() const;

	/// \brief 距离
	double Distance() const;

	/// \brief 绘制捕捉结果
	virtual void Draw(GsDisplay* pDisp)const = 0;
};

/// \brief GsSnapResultPtr
GS_SMARTER_PTR(GsSnapResult);

/// \brief 捕捉容器
/// \details 和地图绑定实现数据捕捉
class GS_API GsSnapContainer:public Utility::GsRefObject
{
	GsSnapResultPtr				m_ptrSnapResult;
	Utility::GsList<GsSnaperPtr> m_ListSnaper;
	Utility::GsGrowByteBuffer m_CacheScreen;
	Utility::GsRect m_ScreenExtent;
	GsDisplayTransformationPtr m_ptrDT;
	GsMapPtr m_ptrMap;
	Utility::GsGrowByteBuffer m_SnapCache;
	GsFeatureIDCache			m_SnapIDCache;
	void*						m_FakeCanvas;
	int							m_InterTolPixel;
	double						m_InterTol;
	bool						m_EnableSnap;
	/// \brief 用于捕捉自身
	GsGeometryPtr				m_ptrGeo;
	/// \brief 缓存最近捕捉到的地物。
	Utility::GsSimpleLRUCache<unsigned int, GsSnapFeaturePtr> m_RecentSnapFeature;
	
	GsSnapFeaturePtr FindFeature(unsigned int orderID);
	void CachePoint(GsGeometryBlob* pBlob, unsigned int order);
	void CachePoint(double* pCoord,int nDim,int nCoordLen, unsigned int order);
	void CacheLine(GsGeometryBlob* pBlob, unsigned int order);
	void CacheLine(double* pCoord, int nDim, int nCoordLen, unsigned int order);
	void CacheSurface(GsGeometryBlob* pBlob, unsigned int order);
private:
	void OnAfterStartDrawing(GsDisplay* disp);
public:
	GsSnapContainer(GsMap* pMap);
	virtual ~GsSnapContainer();
	
	/// \brief 捕捉算法对象
	GsSnaper* Snaper(GsSnaperType eType);
	/// \brief 添加一个自定义或者系统捕捉算法
	void AddSnaper(GsSnaper* snaper);
	
	/// \brief 移除一个捕捉算法
	GsSnaperPtr RemoveSnaper(GsSnaper* snaper);


	/// \brief 是否启动捕捉
	bool&   EnableSnap();

	/// \brief 缓存地物，将地物信息缓存到捕捉容器中
	/// \details 地物绘制过程中所有需要捕捉的地物缓存到此容器中以实现对容器的捕捉。
	virtual void CacheFeature(GsFeature* pFea);

	/// \brief 捕捉容器中的地物
	/// \param screenPoint 鼠标像素坐标
	/// \param nPixelTol 容差，以鼠标坐标为中心上下左右延伸多少像素范围进行检索
	/// \details 粗略捕捉地物对象，根据屏幕鼠标所在的像素点所在的容差区域检索。
	virtual UTILITY_NAME::GsVector<GsSnapFeaturePtr> SnapFeature(const Utility::GsPT& screenPoint, int nPixelTol);
	/// \brief 捕捉容器中的地物
	/// \param screenPoint 鼠标像素坐标
	/// \param nPixelTol 容差，以鼠标坐标为中心上下左右延伸多少像素范围进行检索
	/// \details 粗略捕捉地物对象，根据屏幕鼠标所在的像素点所在的容差区域检索。
	virtual void SnapFeature(const Utility::GsPT& screenPoint, int nPixelTol, UTILITY_NAME::GsVector<GsSnapFeaturePtr>& vec);

	/// \brief 捕捉节点
	/// \param screenPoint 鼠标的屏幕坐标点 
	/// \param nPixelTol 屏幕捕捉容差
	/// \param pRefPoint 参考点,上一次编辑添加的点，参考点用于计算辅助线捕捉，比如水平线，垂直线，延长线，垂线等等
	/// \return 返回捕捉结果，如果没有成功捕捉则返回NULL
	virtual GsSnapResultPtr SnapPoint(const Utility::GsPT& screenPoint, int nPixelTol, GsRawPoint* pRefPoint = NULL);

	/// \brief 捕捉外部传入的几何对象，用于捕捉自身
	/// \param geo要捕捉的几何对象
	virtual void Geometry(GsGeometry* geo);
	/// \brief 获取被捕捉的几何对象
	/// \return 返回被捕捉的几何对象
	virtual GsGeometry* Geometry() const;

	Utility::GsDelegate<GsFeaturePtr(const GsFeatureClass* pFcs,const long long oid)> OnFindFeature;
};
/// \brief GsSnapContainerPtr
GS_SMARTER_PTR(GsSnapContainer);


/// \brief 捕捉数据渲染器
/// \details 将捕捉渲染器加入到FeatureRenderer中以实现将渲染地物加入到捕捉容器中
class GS_API GsSnapRendition :public GsFeatureRendition
{
	GsSnapContainerPtr m_ptrContainer;
public:
	GsSnapRendition(GsSnapContainer* container);
	~GsSnapRendition();

	/// \brief 渲染一个地物
	virtual bool Render(GsFeature* pFea, GsSymbol* pSym);
};
/// \brief GsSnapRenditionPtr
GS_SMARTER_PTR(GsSnapRendition);


KERNEL_ENDNS