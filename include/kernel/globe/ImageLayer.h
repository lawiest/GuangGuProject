#pragma once
#include <Layer.h>
#include <geodatabase.h> 

GLOBE_NS

class GS_API ImageLayer :public Layer
{
public:
	ImageLayer(std::string strUrl, StoreType eSerType);
	ImageLayer(KERNEL_NAME::GsTileClass* ptrTileClass);
	
	virtual ~ImageLayer();

	virtual void Visible(bool bVisible);
	virtual bool Visible() const; 
	int UID();

};

GS_SMARTER_PTR(ImageLayer);

GLOBE_ENDNS


