//********************************************************************************** 
//** 文件名称：CT3D_AABBox.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 包围盒类的实现
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-14
//********************************************************************************** 



#include "CT3D_AABBox.h"
#include "assert.h"

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

AABBox::AABBox()
{
    const double big_number = 1e37f;
    m_VMin.x = m_VMin.y = m_VMin.z = big_number;
    m_VMax.x = m_VMax.y = m_VMax.z = -big_number;
    m_nRecPlaneIndex = -1;
}

/** 构造函数2
*/
AABBox::AABBox(const Vertex3d& vmin,const Vertex3d& vmax)
{
    m_VMin = vmin;
    m_VMax = vmax;
    m_nRecPlaneIndex = -1;
}

/** 构造函数3
*/
AABBox::AABBox(double minx,double miny,double minz,double maxx,double maxy,double maxz)
{
    m_VMin.x = minx;m_VMin.y = miny;m_VMin.z = minz;
    m_VMax.x = maxx;m_VMax.y = maxy;m_VMax.z = maxz;
    m_nRecPlaneIndex = -1;
}

/** 构造函数4
*/
AABBox::AABBox(const AABBox& box)
{
    m_VMax = box.m_VMax;
    m_VMin = box.m_VMin;
    m_nRecPlaneIndex = box.m_nRecPlaneIndex;
}

/** @} */ // 构造与析构函数结尾



/** @name 数据操作
*  @{
*/

/** 获得AABB的最小顶点位置(X,Y,Z最小）
* @return AABB的最小顶点位置
*/
Vertex3d AABBox::GetMin() const
{
    return m_VMin;
}

/** 获得AABB的最大顶点位置(X,Y,Z最大）
* @return AABB的最大顶点位置
*/
Vertex3d AABBox::GetMax() const
{
    return m_VMax;
}

/** 得到AABB各顶点的坐标
* @param [in] vCorners AABB顶点容器 
*/
void AABBox::GetVertices(std::vector<Vertex3d>& vCorners) const
{
    vCorners.reserve(8);
    for(int i=0;i<8;i++)
    {
        Vertex3d pt = FetchCornerPoint(i);
        vCorners.push_back(pt);
    }
}
/** 设置AABB的最小顶点位置(X,Y,Z最小）
* @param [in] maxV AABB的最大顶点位置
*/
void AABBox::SetMax(const Vertex3d& maxV)
{
    m_VMax = maxV;
}

/** 设置AABB的最小顶点位置(X,Y,Z最小）
* @param [in] minV AABB的最小顶点位置
*/
void AABBox::SetMin(const Vertex3d& minV)
{
    m_VMin = minV;
}

/** @} */ // 数据操作组结尾

double AABBox::MaxX() const
{
    return m_VMax.x;
}
double AABBox::MinX() const
{
    return m_VMin.x;
}
double AABBox::MaxY() const
{
    return m_VMax.y;
}
double AABBox::MinY() const
{
    return m_VMin.y;
}
double AABBox::MaxZ() const
{
    return m_VMax.z;
}
double AABBox::MinZ() const
{
    return m_VMin.z;
}

/** 计算AABB的体积
* @return AABB的体积
*/
double AABBox::Volume() const
{
    return XSize() * YSize() * ZSize();
}

/** 计算AABB的中心点位置
* @return AABB的中心点位置
*/
Vertex3d AABBox::CenterPoint() const
{
    return (m_VMin + m_VMax) / 2;
}



Vertex3d AABBox::FetchCornerPoint(int iIndex) const
{
    assert(iIndex >= 0 && iIndex <= 7);
    return Vertex3d((iIndex & 1) ? m_VMax.x : m_VMin.x,
        (iIndex & 2) ? m_VMax.y : m_VMin.y,
        (iIndex & 4) ? m_VMax.z : m_VMin.z);
}

Vertex3d AABBox::GetClosestVertexToAnotherPt(Vertex3d& vVer) const
{
    Vertex3d vRes;
    //	if(vVer.x < m_VMin.x)  //xx_0128
    if(( vVer.x - m_VMin.x ) < -MathEx::TOL)
        vRes.x = m_VMin.x;
    //	else if(vVer.x > m_VMax.x) //xx_0128
    else if(( vVer.x - m_VMax.x ) > MathEx::TOL )
        vRes.x = m_VMax.x;
    else
        vRes.x = vVer.x;

    //	if(vVer.y < m_VMin.y)
    if(( vVer.y - m_VMin.y ) < -MathEx::TOL )
        vRes.y = m_VMin.y;
    //	else if(vVer.y > m_VMax.y) //xx_0128
    else if(( vVer.y - m_VMax.y ) > MathEx::TOL )
        vRes.y = m_VMax.y;
    else
        vRes.y = vVer.y;

    //	if(vVer.z < m_VMin.z)
    if(( vVer.z - m_VMin.z ) < -MathEx::TOL )
        vRes.z = m_VMin.z;
    //	else if(vVer.z > m_VMax.z) //xx_0128
    else if(( vVer.z - m_VMax.z ) > MathEx::TOL )
        vRes.z = m_VMax.z;
    else
        vRes.z = vVer.z;

    return vRes;
}





AABBox AABBox::Union(const Vertex3d& pnt)
{
    //xx_0128
    //	if(pnt.x < m_VMin.x) m_VMin.x = pnt.x;
    if(( pnt.x - m_VMin.x ) < -MathEx::TOL ) m_VMin.x = pnt.x;
    //	if(pnt.x > m_VMax.x) m_VMax.x = pnt.x;
    if(( pnt.x - m_VMax.x ) > MathEx::TOL ) m_VMax.x = pnt.x;
    //	if(pnt.y < m_VMin.y) m_VMin.y = pnt.y;
    if(( pnt.y - m_VMin.y ) < -MathEx::TOL ) m_VMin.y = pnt.y;
    //	if(pnt.y > m_VMax.y) m_VMax.y = pnt.y;
    if(( pnt.y - m_VMax.y ) > MathEx::TOL ) m_VMax.y = pnt.y;
    //	if(pnt.z < m_VMin.z) m_VMin.z = pnt.z;
    if(( pnt.z - m_VMin.z ) < -MathEx::TOL ) m_VMin.z = pnt.z;
    //	if(pnt.z > m_VMax.z) m_VMax.z = pnt.z;
    if(( pnt.z - m_VMax.z ) > MathEx::TOL ) m_VMax.z = pnt.z;
    return *this;
}

AABBox AABBox::Union(const Vertex3d* pnts, int nPntNum)
{
    for(int i=0; i<nPntNum; i++)
        Union(pnts[i]);
    return *this;
}

AABBox AABBox::Union(const std::vector<Vertex3d>& pnts)
{
    int nCount = pnts.size();
    for(int i=0; i<nCount; i++)
        Union(pnts[i]);
    return *this;
}

AABBox AABBox::Union(const AABBox& otherEnvlp)
{

    if(( m_VMax.x - otherEnvlp.m_VMax.x ) < -MathEx::TOL )	m_VMax.x = otherEnvlp.m_VMax.x;
    if(( m_VMax.y - otherEnvlp.m_VMax.y ) < -MathEx::TOL )	m_VMax.y = otherEnvlp.m_VMax.y;
    if(( m_VMax.z - otherEnvlp.m_VMax.z ) < -MathEx::TOL )	m_VMax.z = otherEnvlp.m_VMax.z;


    if(( m_VMin.x - otherEnvlp.m_VMin.x ) > MathEx::TOL )	m_VMin.x = otherEnvlp.m_VMin.x;
    if(( m_VMin.y - otherEnvlp.m_VMin.y ) > MathEx::TOL )	m_VMin.y = otherEnvlp.m_VMin.y;	
    if(( m_VMin.z - otherEnvlp.m_VMin.z ) > MathEx::TOL )	m_VMin.z = otherEnvlp.m_VMin.z;
    return *this;
}

int  AABBox::Intersect(const AABBox& otherbox, AABBox& retBox) const
{
    //	if(	m_VMax.x < otherbox.m_VMin.x ||  //xx_0128
    if(	( m_VMax.x - otherbox.m_VMin.x ) < -MathEx::TOL  ||
        //		m_VMin.x > otherbox.m_VMax.x || 
        ( m_VMin.x - otherbox.m_VMax.x ) > MathEx::TOL || 
        //		m_VMax.y < otherbox.m_VMin.y || 
        ( m_VMax.y - otherbox.m_VMin.y ) < -MathEx::TOL  || 
        //		m_VMin.y > otherbox.m_VMax.y ||
        ( m_VMin.y - otherbox.m_VMax.y ) > MathEx::TOL ||
        //		m_VMax.z < otherbox.m_VMin.z || 
        ( m_VMax.z - otherbox.m_VMin.z ) < -MathEx::TOL  || 
        //		m_VMin.z > otherbox.m_VMax.z
        ( m_VMin.z - otherbox.m_VMax.z ) > MathEx::TOL
        )//不相交
        return 0;

    //	if(m_VMax.x > otherbox.m_VMax.x && m_VMax.y > otherbox.m_VMax.y && m_VMax.z > otherbox.m_VMax.z  //xx_0128
    if(( ( m_VMax.x - otherbox.m_VMax.x ) > MathEx::TOL ) && ( ( m_VMax.y - otherbox.m_VMax.y ) > MathEx::TOL  ) && ( ( m_VMax.z - otherbox.m_VMax.z ) > MathEx::TOL  )
        //&& m_VMin.x < otherbox.m_VMin.x && m_VMin.y < otherbox.m_VMin.y && m_VMin.z < otherbox.m_VMin.z)//包含
        && ( m_VMin.x - otherbox.m_VMin.x ) < -MathEx::TOL  && ( m_VMin.y - otherbox.m_VMin.y ) < -MathEx::TOL  && ( m_VMin.z - otherbox.m_VMin.z ) < -MathEx::TOL )//包含
        return 2;

    if(m_VMax.x < otherbox.m_VMax.x && m_VMax.y < otherbox.m_VMax.y && m_VMax.z < otherbox.m_VMax.z  
        //	&& m_VMin.x > otherbox.m_VMin.x && m_VMin.y > otherbox.m_VMin.y && m_VMin.z > otherbox.m_VMin.z)//被包含
        &&( ( m_VMin.x - otherbox.m_VMin.x ) > MathEx::TOL ) &&( ( m_VMin.y - otherbox.m_VMin.y ) > MathEx::TOL ) &&( ( m_VMin.z - otherbox.m_VMin.z ) > MathEx::TOL ) )//被包含
        return 3;

    retBox.m_VMax.x = min(m_VMax.x, otherbox.m_VMax.x);
    retBox.m_VMax.y = min(m_VMax.y, otherbox.m_VMax.y);
    retBox.m_VMax.z = min(m_VMax.z, otherbox.m_VMax.z);
    retBox.m_VMin.x = max(m_VMin.x, otherbox.m_VMin.x);
    retBox.m_VMin.y = max(m_VMin.y, otherbox.m_VMin.y);
    retBox.m_VMin.z = max(m_VMin.z, otherbox.m_VMin.z);

    return 1;
}

int AABBox::IsPointInBox(const Vertex3d& vPt) const
{
    Vertex3d vmin = m_VMin;
    /*	int bRes = (vPt.x >= m_VMin.x) && (vPt.x <= m_VMax.x) &&
    (vPt.y >= m_VMin.y) && (vPt.y <= m_VMax.y) &&
    (vPt.z >= m_VMin.z) && (vPt.z <= m_VMax.z);*/

    int bRes = ( vPt.x - m_VMin.x > -MathEx::TOL )&& 
        ( vPt.x - m_VMax.x <  MathEx::TOL ) &&
        ( vPt.y - m_VMin.y > -MathEx::TOL ) && 
        ( vPt.y  - m_VMax.y < MathEx::TOL ) &&
        ( vPt.z - m_VMin.z > -MathEx::TOL ) && 
        ( vPt.z - m_VMax.z < MathEx::TOL  );

    return bRes;
}

int AABBox::IsBoxIntersectWithGivenBox(const Envelope3D& inBox) const // xx_modify_0310
{
    Vertex3d Vmin = inBox.GetMin();
    Vertex3d Vmax = inBox.GetMax();

    // 分别判断两外包盒在X\Y\Z方向的投影
    if( ( Vmin.x - m_VMax.x ) > MathEx::TOL || ( m_VMin.x - Vmax.x ) > MathEx::TOL ) return 0;
    if( ( Vmin.y - m_VMax.y ) > MathEx::TOL || ( m_VMin.y - Vmax.y ) > MathEx::TOL ) return 0;
    if( ( Vmin.z - m_VMax.z ) > MathEx::TOL || ( m_VMin.z - Vmax.z ) > MathEx::TOL ) return 0;
    return 1;
}


int AABBox::IsBoxIntersectWithGivenBox2D(const AABBox& inBox, char proAxis, AABBox* intersectBox) const // xx_add_0617
{
    Vertex3d Vmin = inBox.GetMin();
    Vertex3d Vmax = inBox.GetMax();

    if( proAxis == 'z' )
    {
        if( ( Vmin.x - m_VMax.x ) > MathEx::TOL || ( m_VMin.x - Vmax.x ) > MathEx::TOL ) return 0;
        if( ( Vmin.y - m_VMax.y ) > MathEx::TOL || ( m_VMin.y - Vmax.y ) > MathEx::TOL ) return 0;

        if( intersectBox )  
        {
            Vertex3d nMin,nMax;
            nMin.x = max(Vmin.x, m_VMin.x);
            nMin.y = max(Vmin.y, m_VMin.y);

            nMax.x = min(Vmax.x, m_VMax.x);
            nMax.y = min(Vmax.y, m_VMax.y);
        }
        return 1;
    }
    else if( proAxis == 'y' )
    {
        if( ( Vmin.x - m_VMax.x ) > MathEx::TOL || ( m_VMin.x - Vmax.x ) > MathEx::TOL ) return 0;
        if( ( Vmin.z - m_VMax.z ) > MathEx::TOL || ( m_VMin.z - Vmax.z ) > MathEx::TOL ) return 0;

        if( intersectBox )  
        {
            Vertex3d nMin,nMax;
            nMin.x = max(Vmin.x, m_VMin.x);
            nMin.z = max(Vmin.z, m_VMin.z);

            nMax.x = min(Vmax.x, m_VMax.x);
            nMax.z = min(Vmax.z, m_VMax.z);
        }
        return 1;
    }
    else if( proAxis == 'x' )
    {
        if( ( Vmin.y - m_VMax.y ) > MathEx::TOL || ( m_VMin.y - Vmax.y ) > MathEx::TOL ) return 0;
        if( ( Vmin.z - m_VMax.z ) > MathEx::TOL || ( m_VMin.z - Vmax.z ) > MathEx::TOL ) return 0;

        if( intersectBox )  
        {
            Vertex3d nMin,nMax;
            nMin.y = max(Vmin.y, m_VMin.y);
            nMin.z = max(Vmin.z, m_VMin.z);

            nMax.y = min(Vmax.y, m_VMax.y);
            nMax.z = min(Vmax.z, m_VMax.z);
        }
        return 1;
    }
    else
        return 0;

}


double AABBox::Height() const
{
    return fabs(m_VMax.z - m_VMin.z);
}
size_t AABBox::sizeBuffer()
{
    return 2*Vertex3d::sizeBuffer();
}