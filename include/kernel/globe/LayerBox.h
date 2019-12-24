#pragma once
#include <utility.h>
#include <Layer.h>

GLOBE_NS

class GS_API LayerBox :public GeoStar::Utility::GsRefObject
{
public:
	LayerBox();

	//添加一个图层
	void AddLayer(Layer* layer);
	//移除一个图层
	void RemoveLayer(Layer* layer);
	//移除所有图层
	void RemoveAllLayer();
	//移除所有影像图层
	void RemoveAllImageLayer();
	//移除所有地形图层
	void RemoveAllTerrainLayer();
	//通过ID找图层
	Layer* LayerByUID(int nUID);
	//通过index找影像图层
	Layer* ImageAt(int nIndex);
	//影像图层数
	int ImageCount();
	//地形图层数
	int TerrainCount();

	void MoveTo(Layer* layer,int nIndex);

	virtual ~LayerBox();

private:
	std::vector<LayerPtr> m_vecLayer;
	//std::vector<LayerPtr> m_vecTerrainLayer;
	//std::vector<LayerPtr> m_vecPlaceNameLayer;
};

GS_SMARTER_PTR(LayerBox);

GLOBE_ENDNS
