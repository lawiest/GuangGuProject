#include "LineShapeMath.h"


BaseLineMath::BaseLineMath()
{
}

BaseLineMath::~BaseLineMath()
{
}

int BaseLineMath::getCount()
{
	return m_Count;
}

osg::Vec3Array * BaseLineMath::VertexPoints()
{
	return m_Vertex;
}

osg::UShortArray * BaseLineMath::Indexs()
{
	return m_Index;
}

///************************************************************************************
/*-------------------------------------分割线-------------------------------------------------*/

osg::Vec3 zeorVec(0, 0, 0);

DotLine3DMath::DotLine3DMath()
{
}

DotLine3DMath::~DotLine3DMath()
{

}

void DotLine3DMath::start(int count, osg::Vec3& normal, double h, bool isClose)
{
	//计算三角切分的顶点个数
	m_IsClose = isClose;
	m_Lenght = count;//输入的点的个数
	int newCount = m_Lenght * 6 - 8;//三角切分后的节点个数
	m_Count =  ((m_Lenght - 1) * 4 - 2) * 3;// 三角形个数
	if (isClose)
	{
		newCount += 2;//若为闭合的，则节点多出2个
		m_Count += 6;// 多出两个三角形，6个节点。
	}
	m_Vertex = new osg::Vec3Array(newCount);//顶点存储
	m_CoorTextVertex = new osg::Vec2Array(newCount);//纹理坐标
	m_VertexOffset = new osg::Vec3Array(newCount);//顶点的便宜方向
	m_Index = new osg::UShortArray(m_Count);//三角形的绘制的顶点索引记录
	m_NormalVector = normal;
	m_CurPoints = 0;
	index = index2 = 0;
	mHeight = h;// 总的线的长度
	lenSum = 0;
}

void DotLine3DMath::addPoint(osg::Vec3d & point)
{
	m_CurPoints++;
	if (m_CurPoints == 1) {
		m_p1 = point;
		return;
	}
	m_p2 = point;
	// 向量相减
	m_CurAngle = (point - m_p1);
	double len= m_CurAngle.normalize();// 转换到方向的单位向量
	if (m_CurPoints == 2) {// 第一次的
		m_FirstAngle = m_CurAngle;
	}
	else {//
		dotNodePoint(false);
	}
	// 向量的叉乘
	osg::Vec3 dir = m_CurAngle^m_NormalVector;
	dir.normalize();
	//点的位置
	setPoints(index, m_p1);
	setPoints(index + 1, m_p1);
	setPoints(index + 2, m_p2);
	setPoints(index + 3, m_p2);
	//坐标的偏移量
	setVertexCoord(index, dir);
	setVertexCoord(index + 1, (zeorVec-dir));
	setVertexCoord(index + 2, dir);
	setVertexCoord(index + 3, zeorVec-dir);
	//纹理坐标设置
	setTextCoord(index, 0, (float)(lenSum / mHeight));
	setTextCoord(index + 1, 1, (float)(lenSum / mHeight));
	lenSum += len;
	setTextCoord(index + 2, 0, (float)(lenSum / mHeight));
	setTextCoord(index + 3, 1, (float)(lenSum / mHeight));
	//
	//顶点序列值,量三角形的顶点
	(*m_Index)[index2] = (unsigned short)(index);
	(*m_Index)[index2 + 1] = (GLushort)(index + 1);
	(*m_Index)[index2 + 2] = (GLushort)(index + 2);
	index2 += 3;
	//
	(*m_Index)[index2] = (GLushort)(index + 1);
	(*m_Index)[index2 + 1] = (GLushort)(index + 2);
	(*m_Index)[index2 + 2] = (GLushort)(index + 3);
	index2 += 3;
	index += 4;
	m_p1 = m_p2;
	m_preAngle = m_CurAngle;

}

void DotLine3DMath::end()
{
	if (m_IsClose) {// 若为闭合的做闭合处理
		m_preAngle = m_FirstAngle;
		dotNodePoint(true);
	}
}

osg::Vec2Array* DotLine3DMath::GetCoorTextVertex()
{
	return m_CoorTextVertex;
}

osg::Vec3Array* DotLine3DMath::GetVerTexOffset()
{
	return m_VertexOffset;
}

void DotLine3DMath::addDotPoint(osg::Vec3d & point)
{
}

void DotLine3DMath::dotNodePoint(bool isEnd)
{
	osg::Vec3 ab = m_CurAngle + m_preAngle;// 向量相加的方向
	osg::Vec3 dirab = ab^m_NormalVector;//在地平面方向的向量
	dirab.normalize();
	float axb = m_CurAngle*m_preAngle;//判断方向若==1 则同向
	int step = 2;
	if (axb < 0.999)//同向则默认
	{
		// 计算 b-a的方向
		osg::Vec3d ba = m_CurAngle - m_preAngle;
		ba.normalize();//转成单位向量
		float dirabXba = dirab*ba;
		if (dirabXba > 0) {
			step = 1;
			dirab = zeorVec - dirab;
		}//取反向方向
	}
	setPoints(index, m_p1);//points[index] = p1;
	setPoints(index + 1, m_p1);
	////文理坐标
	setTextCoord(index + 1, 0.5f, (float)(lenSum / mHeight));
	lenSum += 0.01;
	setTextCoord(index, temlemate[4 + step][0], (float)(lenSum / mHeight));// texCoord[index] = temlemate[4 + step];
	lenSum += 0.01;
	////坐标方向
	setVertexCoord(index,dirab); //coord[index] = new double[] { dx, dy };
	setVertexCoord(index + 1, 0.f,0.f);//coord[index+1] = new double[] { 0, 0 };
												  //建立三角形
												  //顶点序列值,量三角形的顶点
	(*m_Index)[index2] = (GLushort)(index - step);
	(*m_Index)[index2 + 1] = (GLushort)(index + 1);
	(*m_Index)[index2 + 2] = (GLushort)index;
	index2 += 3;
	//
	(*m_Index)[index2] = (GLushort)(index + 1);
	(*m_Index)[index2 + 1] = (GLushort)index;
	//若我i结束的处理则，标号，非0，即1
	(*m_Index)[index2 + 2] = (GLushort)(isEnd ? ((index - step + 4) % 2) : (index - step + 4));
	index2 += 3;
	index += 2;
}

void DotLine3DMath::setPoints(int index, osg::Vec3d & point)
{
	(*m_Vertex)[index] = point;
}

void DotLine3DMath::setVertexCoord(int index, float x, float y)
{
	(*m_VertexOffset)[index] = osg::Vec3(x, y, 0.0f);
}

void DotLine3DMath::setVertexCoord(int index, const osg::Vec3 & dirv)
{
	(*m_VertexOffset)[index] = dirv;
}

void DotLine3DMath::setTextCoord(int index, float x, float y)
{
	(*m_CoorTextVertex)[index] = osg::Vec2(x, y);
}