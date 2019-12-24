#pragma once
#include <Layer.h>
#include <geodatabase.h> 

GLOBE_NS

class GS_API VectorLayer :public Layer
{
public:
	VectorLayer(std::string strUrl, StoreType eSerType);
	VectorLayer(KERNEL_NAME::GsTileClass* ptrTileClass);
	
	void StartPick();
	void EndPick();

	virtual ~VectorLayer();

	virtual void Visible(bool bVisible);
	virtual bool Visible() const; 
	int UID();

private:
	bool m_bIsPick;
};

GS_SMARTER_PTR(VectorLayer);

GLOBE_ENDNS


