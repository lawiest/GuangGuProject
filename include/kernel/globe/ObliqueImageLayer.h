#pragma once
#include <Layer.h>
#include <Feature.h>

namespace osg
{
	class Node;
}

GLOBE_NS

class GS_API ObliqueImageLayer :public Layer
{
public:
	ObliqueImageLayer(std::string strUrl, StoreType eSerType, double dbMinLod = 0.0, double dbMaxLod = 300.0);
	static void StartChangeRangList();
	void setLod(double dbLod);

	//通过一个经纬度点实现倾斜单体化
	void Pick(const GsRawPoint3D& pos);

	void setPickParamter(const std::string& strUrl,const std::string& strTypeName);
	void* getPickParamter();

	virtual ~ObliqueImageLayer();

	virtual void Visible(bool bVisible);
	virtual bool Visible() const;

	void StartPick();
	void EndPick();
	UTILITY_NAME::GsDelegate<void(Feature&)>& OnPicked();

	virtual int UID();

protected:

	double m_dbLod;
	bool m_bIsPick;

	GsReferencePtr m_ptrPickHandle;
};

GS_SMARTER_PTR(ObliqueImageLayer);

GLOBE_ENDNS


