#include "SymbolBox.h"
#include <osg/Group>
#include "OsgCoreSingleton.h"

GLOBE_NS

SymbolBox::SymbolBox()
{
}

void SymbolBox::AddSymbol(Symbol* ptrSymbol)
{
	if (NULL == ptrSymbol)
		return;

	ptrSymbol->Build(OsgCoreSingleton::Instance()->MapNode());
	OsgCoreSingleton::Instance()->Root()->addChild(ptrSymbol->SymbolNode());

	m_vecSymbol.push_back(ptrSymbol);
}

void SymbolBox::RemoveSymbol(Symbol* ptrSymbol)
{
	if (NULL == ptrSymbol)
		return;

	OsgCoreSingleton::Instance()->Root()->removeChild(ptrSymbol->SymbolNode());

	std::vector<SymbolPtr>::iterator it = std::find(m_vecSymbol.begin(), m_vecSymbol.end(), ptrSymbol);

	if (it!= m_vecSymbol.end())
		m_vecSymbol.erase(it);
}

SymbolBox::~SymbolBox()
{

}

GLOBE_ENDNS