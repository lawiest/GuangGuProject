#include "FindNodeVisitor.h"


//构造函数，初始化并设置遍历所有的子节点
FindNodeVisitor::FindNodeVisitor(const std::string &searchName) : 
osg::NodeVisitor(TRAVERSE_ALL_CHILDREN), 
	searchForName(searchName) 
{ 
	//
} 

//重载apply方法								   
void FindNodeVisitor::apply(osg::Node &searchNode) 
{ 
	//判断节点名称是否与查找的节点名称一样
	if (searchNode.getName() == searchForName)
	{
		//添加到节点系列
		foundNodeList.push_back(&searchNode);
	}

	//继续遍历
	traverse(searchNode); 
} 

//重载apply方法
void FindNodeVisitor::apply(osg::Transform &searchNode)
{
	apply ((osg::Node&)searchNode);

	traverse(searchNode);
}

//重载apply方法
void FindNodeVisitor::apply(osg::Geode &geode)
{
	apply ( (osg::Node&)geode);

	traverse((osg::Node&)geode);
}

//设置要查找的节点名称
void FindNodeVisitor::setNameToFind(const std::string &searchName) 
{ 
	searchForName = searchName; 

	foundNodeList.clear(); 
} 

//得到查找节点向量中第一个节点
osg::ref_ptr<osg::Node> FindNodeVisitor::getFirst()  //未找到节点的时候需要返回空，否则出现 vector泄露
{
	if (foundNodeList.size()>=1)
	{
		return *(foundNodeList.begin());
	} 
	else
	{

		return 0;
	}
	
}
