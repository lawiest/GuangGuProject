#pragma once
#include <utility.h>
#include <geometry.h>

namespace osg
{
	class Node;
}

namespace osgEarth
{
	class MapNode;
}

GLOBE_NS

class GS_API Symbol :public GeoStar::Utility::GsRefObject
{
public:
	Symbol();
	virtual ~Symbol();

	//设置是否可见
	virtual void Visible(bool bVisible) = 0;
	//获取是否可见
	virtual bool Visible() const = 0;

	virtual void Build(osgEarth::MapNode* ptrMapNode) {};
	virtual void Position(KERNEL_NAME::GsRawPoint3D gsPosition) {};
	virtual KERNEL_NAME::GsRawPoint3D Position() { return KERNEL_NAME::GsRawPoint3D(0.0, 0.0, 0.0); };

	virtual osg::Node *SymbolNode() { return NULL; };

	virtual int UID() = 0;

protected:
	double m_dbLongitude;
	double m_dbLatitude;
	double m_dbHeight;
	
};

GS_SMARTER_PTR(Symbol);

GLOBE_ENDNS

