#pragma once

#include <Layer.h>
#include <geodatabase.h> 


GLOBE_NS

class GS_API WFSPicker :public Layer
{
public:
	WFSLayer(std::string strUrl, std::string strTypeName, double dbheight = 0.0);
	WFSLayer(KERNEL_NAME::GsTileClass* ptrTileClass);
	
	void ReadWFSMetaDataFromServer(std::string strUrl, std::string strTypeName);
	std::string ReadFeatureType(std::string strUrl, std::string strTypeName);

	void StartPick();
	void EndPick();

	static std::vector<std::string> getTypeNameList(std::string strUrl);

	virtual ~WFSLayer();

	virtual void Visible(bool bVisible);
	virtual bool Visible() const; 
	int UID();

protected:
	//std::vector<std::string> m_vecTypeList;
	bool m_bIsPick;

};

GS_SMARTER_PTR(WFSLayer);

GLOBE_ENDNS


