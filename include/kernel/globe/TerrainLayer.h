#pragma once
#include <Layer.h>
#include <geodatabase.h> 

GLOBE_NS

class GS_API TerrainLayer :public Layer
{
public:
	TerrainLayer(std::string strUrl, StoreType eSerType);
	TerrainLayer(KERNEL_NAME::GsTileClass* ptrTileClass);
	
	virtual ~TerrainLayer();

	virtual void Visible(bool bVisible);
	virtual bool Visible() const; 
	int UID();

};

GS_SMARTER_PTR(TerrainLayer);

GLOBE_ENDNS


