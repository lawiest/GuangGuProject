#pragma once
#include "Element.h"

#include <GsReference.h>

GLOBE_NS

class GS_API PointSymbolElement : public Element
{
public:

	PointSymbolElement();
	virtual~PointSymbolElement();
	void AddPoint(double x, double y, double z);
	//µãÑÕÉ«£¬r[0-255],g[0-255],b[0-255],a[0-1]
	void setColor(int r, int g, int b, float a = 1.0);
	void setSize(int nSize);
};

GS_SMARTER_PTR(PointSymbolElement);

GLOBE_ENDNS