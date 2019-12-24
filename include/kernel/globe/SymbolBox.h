#pragma once
#include <utility.h>
#include <Symbol.h>

GLOBE_NS

class GS_API SymbolBox :public GeoStar::Utility::GsRefObject
{
public:
	SymbolBox();
	virtual ~SymbolBox();

	void AddSymbol(Symbol* ptrSymbol);
	void RemoveSymbol(Symbol* ptrSymbol);

private:
	std::vector<SymbolPtr> m_vecSymbol;
};

GS_SMARTER_PTR(SymbolBox);

GLOBE_ENDNS
