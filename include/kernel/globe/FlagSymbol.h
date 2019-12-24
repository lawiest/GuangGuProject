#pragma once

#include "Symbol.h"
#include <GsReference.h>

namespace osg
{
	class Node;
	class DrawElementsUShort;
}

namespace osgEarth
{
	class MapNode;
}

GLOBE_NS

class GS_API FlagSymbol : public Symbol
{
public:
	FlagSymbol(double dLongitude, double dLatitude, double dHeight);
	virtual ~FlagSymbol();

	//设置是否可见
	void Visible(bool bVisible);
	//获取是否可见
	bool Visible() const;

	void Position(KERNEL_NAME::GsRawPoint3D gsPosition);
	KERNEL_NAME::GsRawPoint3D Position();

	void Scale(double dbScale);
	void RotateToScreen(bool bRotate);
	void Text(std::string strText);

	void Build(osgEarth::MapNode* ptrMapNode);

	osg::Node *SymbolNode();
	
	int UID() ;

	void CreateFlag(double x, double y, double z);
	void CreateRectangle(double x, double y, double z, double dLength, double dWidth, double dHeight,osg::DrawElementsUShort* ptrIndex,bool bIsFlagRoot = false);

private:
	GsReferencePtr m_ptrFlagGeometry;
	GsReferencePtr m_ptrLocatorNode;
	GsReferencePtr m_ptrVectices;
	GsReferencePtr m_ptrTrans;
	GsReferencePtr m_ptrText;

	osgEarth::MapNode* m_pMapNode;

	int m_nIndex;
};

GS_SMARTER_PTR(FlagSymbol);

GLOBE_ENDNS

