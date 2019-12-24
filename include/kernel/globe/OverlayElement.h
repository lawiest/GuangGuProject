
#pragma once
#include "Element.h"
#include <GsReference.h>
#include <Layer.h>

namespace osg
{
	class Node;
}

GLOBE_NS

/*
可以将绘制元素渲染到纹理并贴到所给图层数据上，例如将视频纹理元素贴到倾斜数据上
*/
class GS_API OverlayElement :public Element
{
public:

	/*
	注意，被投影图层ptrBaseLayer会当做OverlayElement的子节点渲染！
	所以不需要加到LayerBox里，否侧ptrBaseLayer会被渲染两次并且会覆盖投影效果
	*/
	OverlayElement(Element* ptrElement, Layer* ptrBaseLayer);

	/*
	设置投影方向，例如简单的投向Y轴方向(0.0,1.0,0.0)
	如果你明确知道要投的方向可设置此方法
	*/
	void setProjectDirection(double x, double y, double z);
	/*
	添加子节点即待融合节点,例如倾斜数据
	注意：外部程序不要调用它目前只供内部调用以后可能会更改！
	*/
	static void addChild(osg::Node* ptrNode);
	
	//添加待投影子元素，例如一个视频
	void addOverlaySubgraph(Element* ptrElement);

	//移除以投影子元素
	void removeOverlaySubgraph(Element* ptrElement);
	//移除所有子节点，例如已添加的倾斜数据
	void removeChildren();

	virtual~OverlayElement();

	static GsReferencePtr m_ptrOverlayNode;

};

GS_SMARTER_PTR(OverlayElement);

GLOBE_ENDNS