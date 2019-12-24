#pragma once
#include <Layer.h>
#include <geodatabase.h> 

GLOBE_NS

class GS_API PlaceNameLayer :public Layer
{
public:
	PlaceNameLayer(std::string strUrl, StoreType eSerType, bool bAutoHide = true, float minLOD = 0, float maxLOD = INT_MAX);
	PlaceNameLayer(KERNEL_NAME::GsTileClass* ptrTileClass);
	
	virtual ~PlaceNameLayer();

	virtual void Visible(bool bVisible);
	virtual bool Visible() const; 
	int UID();
};

GS_SMARTER_PTR(PlaceNameLayer);

GLOBE_ENDNS


