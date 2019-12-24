//********************************************************************************** 
//** 文件名称：CT3D_AABBox.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 包围盒类的定义
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-14
//********************************************************************************** 
#include "ModelDef.h"
#include "CT3D_Vertex3d.h"
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _CT3D_AABBOX_H_
#define _CT3D_AABBOX_H_

#include <vector>
using namespace std;

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))




class /*DLL_EXPORT*/ AABBox 
{
	/** @defgroup AABBox AABBox-轴向包围盒  
    *  @{
    */

protected:

	/** @name 属性（成员变量）
	* @{
	*/
	/** 最小点 */
	Vertex3d m_VMin;

	/** 最大点 */
	Vertex3d m_VMax;

	

	/** @} */ // 属性结尾

public:

	int m_nRecPlaneIndex;

	/** @defgroup AABBox 操作（成员函数）
	* @{
	*/
	
	
	/** @name 构造与析构函数
	*  @{
	*/

	/** 构造函数1
	*/
	AABBox();
	
	
	/** 构造函数2
	*/
	AABBox(const Vertex3d& vmin,const Vertex3d& vmax);
	
	
	/** 构造函数3
	*/
	AABBox(double minx,double miny,double minz,double maxx,double maxy,double maxz);
	
	
	/** 构造函数4
	*/
	AABBox(const AABBox& box);
	
	
	/** @} */ // 构造与析构函数结尾
	
	
	
	/** @name 数据操作
	*  @{
	*/

	/** 获得AABB的最小顶点位置(X,Y,Z最小）
	* @return AABB的最小顶点位置
    */
	Vertex3d GetMin() const;
	

	/** 获得AABB的最大顶点位置(X,Y,Z最大）
	* @return AABB的最大顶点位置
    */
	Vertex3d GetMax() const;
	
	
	/** 得到AABB各顶点的坐标
	* @param [in] vCorners AABB顶点容器 
    */
	void GetVertices(std::vector<Vertex3d>& vCorners) const;
	
	/** 设置AABB的最小顶点位置(X,Y,Z最小）
	* @param [in] maxV AABB的最大顶点位置
    */
	void SetMax(const Vertex3d& maxV);
	
	/** 设置AABB的最小顶点位置(X,Y,Z最小）
	* @param [in] minV AABB的最小顶点位置
    */
	void SetMin(const Vertex3d& minV);
	
	
	/** @} */ // 数据操作组结尾
	
	
	
	/** @name 计算操作
	*  @{
	*/

	/** 计算AABB的长度
	* @return AABB的长度
    */
	double Length() const;
	
	/** 计算AABB的宽度
	* @return AABB的宽度
    */
	double Width() const;

	/** 计算AABB的高度
	* @return AABB的高度
    */
	double Height() const;

	/** 计算AABB对角线的长度
	* @return AABB对角线的长度
    */
	double Diagonal() const
	{
		return m_VMin.DVertexToVertex3d(m_VMax);
	}
	/** 计算AABB在x方向的长度
	* @return AABB在x方向的长度
    */
	double XSize() const
	{
		return m_VMax.x - m_VMin.x;
	}

	/** 计算AABB在y方向的长度
	* @return AABB在y方向的长度
    */
	double YSize() const
	{
		return m_VMax.y - m_VMin.y;
	}

	/** 计算AABB在z方向的长度
	* @return AABB在z方向的长度
    */
	double ZSize() const
	{
		return m_VMax.z - m_VMin.z;
	}

	double MaxX() const;
	double MinX() const;
	double MaxY() const;
	double MinY() const;
	double MaxZ() const;
	double MinZ() const;

	/** 计算AABB的体积
	* @return AABB的体积
    */
	double Volume() const;
	

	/** 计算AABB的中心点位置
	* @return AABB的中心点位置
    */
	Vertex3d CenterPoint() const;
	

	/** 取得AABB的八个顶点之一
	* @param [in] iIndex
	*  - 0	Xmin,Ymin,Zmin
	*  - 1	Xmax,Ymin,Zmin
    *  - 2	Xmin,Ymax,Zmin
    *  - 3  Xmax,Ymax,Zmin
	*  - 4	Xmin,Ymin,Zmax
	*  - 5	Xmax,Ymin,Zmax
    *  - 6	Xmin,Ymax,Zmax
    *  - 7  Xmax,Ymax,Zmax
	* @return AABB的八个顶点之一
    */
	Vertex3d FetchCornerPoint(int iIndex) const;

	/** 计算离传入点最近的AABB上的点
	* @param [in] vVer 传入点
	* @return 离传入点最近的AABB的点
    */
	Vertex3d GetClosestVertexToAnotherPt(Vertex3d& vVer) const;


	/** 计算传入三角形到AABB的最小距离
	* @param [in] begVer,endVer传入线段端点
	* @return 离传入点最近的AABB的点
	*/
	double GetClosestTriangleDisToAABBBox(Vertex3d& triPts1,Vertex3d& triPts2,Vertex3d& triPts3) const;

	/** 对当前AABB进行扩张确保包含传入点，若点在AABB内，则不需要扩张
	* @param [in] pnt 传入点
	* @return 扩张后的AABB
    */
	AABBox Union(const Vertex3d& pnt);
	
	/** 对当前AABB进行扩张确保包含传入点集，若点集在AABB内，则不需要扩张
	* @param [in] pnts 传入点集
	* @param [in] nPntNum 传入点的个数
	* @return 扩张后的AABB
    */
	AABBox Union(const Vertex3d* pnts, int nPntNum);
	AABBox Union(const std::vector<Vertex3d>& pnts);

	/** 对当前AABB进行扩张确保包含传入另一个AABBox，若其在AABB内，则不需要扩张
	* @param [in] otherEnvlp 传入的AABBox
	* @return 扩张后的AABB
    */
	AABBox Union(const AABBox& otherEnvlp);

	/** 保持当前AABB的中心点不变，在原有基础上，分别沿x，y，z正反轴方向扩张一个增量
	* @param [in] dx x方向增量
	* @param [in] dy y方向增量
	* @param [in] dz z方向增量
	* @return 扩张后的AABB
    */
	AABBox Inflate(double dx, double dy, double dz)
	{
		return Inflate(Vertex3d(dx,dy,dz));
	}

	/** 保持当前AABB的中心点不变，在原有基础上，分别沿x，y，z正反轴方向扩张一个增量
	* @param [in] vVec x，y，z方向增量集合（向量表示）
	* @return 扩张后的AABB
    */
	AABBox Inflate(const Vertex3d& vVec)
	{
		m_VMin -= vVec;
		m_VMax += vVec;
		Vertex3d vMin = m_VMin;
		Vertex3d vMax = m_VMax;
		m_VMin.x = min(vMin.x,vMax.x);
		m_VMin.y = min(vMin.y,vMax.y);
		m_VMin.z = min(vMin.z,vMax.z);

		m_VMax.x = max(vMin.x,vMax.x);
		m_VMax.y = max(vMin.y,vMax.y);
		m_VMax.z = max(vMin.z,vMax.z);

		return *this;
	}

	/** 保持当前AABB的中心点不变，在原有基础上，分别沿x，y，z正反轴方向减小一个增量
	* @param [in] dx x方向增量
	* @param [in] dy y方向增量
	* @param [in] dz z方向增量
	* @return 缩小后的AABB
    */
	AABBox Deflate(double dx, double dy, double dz)
	{
		return Deflate(Vertex3d(dx,dy,dz));
	}
	
	/** 保持当前AABB的中心点不变，在原有基础上，分别沿x，y，z正反轴方向减小一个增量
	* @param [in] vVec x，y，z方向增量集合（向量表示）
	* @return 缩小后的AABB
    */
	AABBox Deflate(const Vertex3d& vVec)
	{
		return Inflate(-vVec);
	}

	/** 设置AABB为空，即将最小点的位置设为正无穷大，最大点的位置设为负无穷大
    */
	inline void Empty()
	{
		const double big_number = 1e37f;
		m_VMin.x = m_VMin.y = m_VMin.z = big_number;
		m_VMax.x = m_VMax.y = m_VMax.z = -big_number;
	}


	/** 判断当前AABB是否为空
	* @return 是否为空
    */
	int IsEmpty() const
	{
	//	return ((m_VMin.x > m_VMax.x) || (m_VMin.y > m_VMax.y) || (m_VMin.z > m_VMax.z)); //xx_0128
		return ( ( ( m_VMin.x - m_VMax.x ) > MathEx::TOL ) ||
				 ( ( m_VMin.y - m_VMax.y ) > MathEx::TOL ) ||
				 ( ( m_VMin.z - m_VMax.z ) > MathEx::TOL ) );
	}

	/** 对当前AABB平移一个向量
	* @param [in] vVec 平移向量
	* @return	平移后的AABB
    */
	AABBox Offset(const Vertex3d& vVec)
	{
		m_VMax += vVec;
		m_VMin += vVec;
		return *this;
	}

	/** 将当前AABB与传入的AABB进行求交，并得到求交后的AABB
	* @param [in] otherbox 传入的AABB	
	* @param [out] retBox 求交后得到的AABB	
	* @return	1 -- 相交；0 -- 相离；2 -- 包含； 3 -- 被包含；  
    */
	int  Intersect(const AABBox& otherbox, AABBox& retBox) const;

	/** 判断一个点是否在包围盒内
	* @param [in] vPt 传入点	
	* @return	1 -- 在包围盒内；0 -- 在包围盒外；  
    */
	int IsPointInBox(const Vertex3d& vPt) const;

	/** 当前AABBBOX 与传入的AABBBOX 是否相交
	* @param [in] inBox 传的外包矩形
	* @return	1 -- 在包围盒内；0 -- 在包围盒外；  
	*/
	int IsBoxIntersectWithGivenBox(const AABBox& inBox) const;

	/** 当前AABBBOX 与传入的AABBBOX 在给定轴向的投影是否相交
	* @param [in] inBox		传入的外包矩形
	* @param [in] proAxis	传入投影轴,取值‘x’‘y’‘z’
	* @return	1 -- 投影相交（或临接，包含）；0 -- 投影不相交；  
	*/
	int IsBoxIntersectWithGivenBox2D(const AABBox& inBox, char proAxis = 'z', AABBox* intersectBox = NULL) const;

	/** 重载操作符"=",用于对当前AABB的赋值
	* @param [in] rha 传入的AABB	
	* @return 赋值后的AABB 
    */
	AABBox& operator=(const AABBox& rha)
	{
		m_VMax = rha.m_VMax;
		m_VMin = rha.m_VMin;
		return (*this);
	}

	/** 重载操作符"==",判断传入AABB是否与当前AABB相等
	* @param [in] otherbox 传入的AABB	
	* @return 1 -- 相等；0 -- 不等
    */
	int operator==(const AABBox& otherbox) const
	{
		return m_VMax == otherbox.m_VMax && m_VMin == otherbox.m_VMin;
	}
	



	/** 判断当前AABB与给定多边形在2D上的关系：包含，相交，相离
	* @param [in] ptArray 传入点数组
	* @return 1 -- 投影相交（或临接，包含）；0 -- 投影不相交； 
	*/
	int IsBoxintersectWithPolygon2D( vector<Vertex3d>& ptArray );

    static size_t sizeBuffer();

};

typedef AABBox Envelope3D;

#endif /* _INC_AABBOXENVELOPE3D_49E347290251_INCLUDED */
