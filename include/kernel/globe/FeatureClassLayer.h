#pragma once
#include <Layer.h>
#include <geodatabase.h> 

#include <GeoBillBoardElement.h>

GLOBE_NS

class GS_API QueryElevationCallback :public GeoStar::Utility::GsRefObject
{
public:
	virtual double Query(double lon, double lat) = 0;
};
GS_SMARTER_PTR(QueryElevationCallback);

class GS_API FeatureClassLayer :public Layer
{
public:
	/*
	ptrQueryElv：查高程回调类
	nMinLod：整个图层所有元素最小可见距离（物体离相机）
	nMaxLod：整个图层所有元素最大可见距离（物体离相机）
	*/

	FeatureClassLayer(std::string strUrl, StoreType eSerType, std::string strFont = "msyh.ttc" ,bool bCoverAutoHide = true, QueryElevationCallback* ptrQueryEle = NULL,int nMinLod = 0, int nMaxLod = INT_MAX);
	FeatureClassLayer(KERNEL_NAME::GsFeatureClass* ptrFeatureClass,
					std::string layerName,
					std::string labeField, 
					std::string strIcon, 
					std::string strFont = "msyh.ttc" ,
					bool bCoverAutoHide = true, 
					QueryElevationCallback* ptrQueryElv = NULL, 
					int nMinLod = 0, 
					int nMaxLod = INT_MAX);
	
	virtual ~FeatureClassLayer();

	virtual void Visible(bool bVisible);
	virtual bool Visible() const; 

	/*过滤，含有该子字符串的元素会显示,其余的会被隐藏*/
	void Filter(std::string strFilter);

	std::vector<GeoBillBoardElementPtr>& getAllElement();

	int UID();

protected:
	std::vector<GeoBillBoardElementPtr> m_vecBillBord;
	QueryElevationCallbackPtr m_QueryElv;
};

GS_SMARTER_PTR(FeatureClassLayer);

GLOBE_ENDNS


