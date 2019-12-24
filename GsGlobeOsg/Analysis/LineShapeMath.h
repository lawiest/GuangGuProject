#pragma once
#include <osg\Array>
/// \brief  基本线的计算过程
class BaseLineMath
{
public:
	const float temlemate[7][2] = { { 0, 0.26f },{ 1, 0.26f },{ 0, 0.74f },{ 1, 0.74f },{ 0.5f, 0.5f },{ 1.f, 0.5f },{ 0.f, 0.5f } };

	const float template2[4][2] = { { 0, 0 },{ 1, 0 },{ 0, 0.25f },{ 1, 0.25f } };
	const float template3[4][2] = { { 0, 0.75f },{ 1, 0.75f },{ 0, 1 },{ 1, 1 } };

	BaseLineMath();
	~BaseLineMath();
	/**
	* 切分成的所有三角形的个数
	*
	* @return
	*/
	int getCount();

	virtual void start(int count, float w, double h, bool isClose) {};

	virtual void addPoint(osg::Vec3d& point) {};

	virtual void end() = 0;
	// 获取三维点的过程
	virtual osg::Vec3Array* VertexPoints();
	// 获取点序列的过程
	virtual osg::UShortArray* Indexs();

protected:
	osg::Vec3Array* m_Vertex;
	osg::UShortArray* m_Index;
	int m_Count;
};


/*****************
* 可以根据法线向量偏移的过程
***/
class DotLine3DMath :public BaseLineMath
{
public:
	DotLine3DMath();
	~DotLine3DMath();

	virtual void start(int count, osg::Vec3& normal, double h, bool isClose) ;
	virtual void addPoint(osg::Vec3d& point) ;
	virtual void end()override;
	// 点的偏移量的过程
	osg::Vec2Array* GetCoorTextVertex();
	// 面的偏移量的过程
	osg::Vec3Array* GetVerTexOffset();
protected:
	/// \brief 添加点的过程
	void addDotPoint(osg::Vec3d& point);
	/// \brief 添加结束的处理
	void dotNodePoint(bool isEnd);
	//
	//设置节点坐标
	void setPoints(int index, osg::Vec3d & point);
	// 节点偏移量
	void setVertexCoord(int index, float x, float y);

	void setVertexCoord(int index,const osg::Vec3& dirv);
	//设置纹理坐标
	void setTextCoord(int index, float x, float y);
protected:
	osg::Vec2Array* m_CoorTextVertex;//纹理坐标贴图坐标
	osg::Vec3Array* m_VertexOffset;//偏移量向量
	int m_Lenght;
	double  mHeight;
	//
	double  lenSum;
	int index = 0, index2 = 0;
	int m_CurPoints;
	bool m_IsClose;//是否为闭合的线
	osg::Vec3d m_p1, m_p2;
	osg::Vec3 m_NormalVector;//法线向量
	osg::Vec3d m_preAngle, m_CurAngle, m_FirstAngle;//上一个向量，当前向量，第一个向量的值。
};
