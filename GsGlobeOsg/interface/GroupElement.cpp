#include "globe/GroupElement.h"
#include "ModelElement.h"
#include "Element.h"
#include "OsgCoreSingleton.h"
#include <osg/Node>
#include <osg/Group>
#include <osgEarth/MapNode>
#include <OverlayElement.h>
#include <BillBoardElement.h>
#include <GeoBillBoardElement.h>
#include "innerimp.h"
#include "Analysis/DistanceLoD.h"

GLOBE_NS

GroupElement::GroupElement(int lodmode, int minLod, int maxLod)
{
	m_minLod = minLod;
	m_maxLod = maxLod;
	osg::ref_ptr<osg::DistanceLOD> ptrLOD = new osg::DistanceLOD((osg::DistanceLODMODE)(lodmode));
	m_ptrElementHandle = new ReferenceImp<osg::DistanceLOD>(ptrLOD);
	m_ElementType = eGroupElement;
}

GroupElement::~GroupElement()
{
}

void GroupElement::AddElement(Element * ptrElement)
{

	osg::ref_ptr<osg::LOD> ptrMapNode = m_ptrElementHandle->CastTo<osg::ref_ptr<osg::LOD>>();// OsgCoreSingleton::Instance()->MapNode();
	if (NULL == ptrElement || NULL == ptrMapNode)
		return;
	switch (ptrElement->Type())
	{
		case eModelElement:
		case eTextureMovieElement:
		case eStreamMovieElement:
		case elineSymbolElement3D:
		case eLinePictureElement3D:
		case ePolygonSymbolElement:
		case eTextElement:
		case eSquareElement:
		{
			osg::ref_ptr<osg::Node> ptrNode = ptrElement->Handle()->CastTo<osg::ref_ptr<osg::Node> >();

			if (NULL != ptrNode)
			{
				ptrMapNode->addChild(ptrNode,m_minLod,m_maxLod);
				m_vecElement.push_back(ptrElement);
			}

		}break;
		case eBillboardElement:
		{
			BillBoardElementPtr ptrBill = dynamic_cast<BillBoardElement*>(ptrElement); 
			//osg::ref_ptr<osg::Node> ptrNode = ptrElement->Handle()->CastTo<osg::ref_ptr<osg::Node> >();
			osg::ref_ptr<osg::Node> ptrNode = ptrBill->LodHandle()->CastTo<osg::ref_ptr<osg::Node> >();

			/*拾取BillBoard等屏幕元素需要使用osgearth自带的RTT技术拾取必须把BillBoard加到MapNode子群组中，
			注意是子群组，不能直接作为孩子加到MapNode下面，目前原因不明*/
			if (NULL != ptrNode)
			{
				ptrMapNode->addChild(ptrNode, m_minLod, m_maxLod);
				//OsgCoreSingleton::Instance()->RTTPickGroup()->addChild(ptrNode);
				m_vecElement.push_back(ptrElement);
			}
		}break;
		case eGeoBillBoardElement:
		{
			GeoBillBoardElementPtr ptrBill = dynamic_cast<GeoBillBoardElement*>(ptrElement);
			//osg::ref_ptr<osg::Node> ptrNode = ptrElement->Handle()->CastTo<osg::ref_ptr<osg::Node> >();
			osg::ref_ptr<osg::Node> ptrNode = ptrBill->LodHandle()->CastTo<osg::ref_ptr<osg::Node> >();

			/*拾取BillBoard等屏幕元素需要使用osgearth自带的RTT技术拾取必须把BillBoard加到MapNode子群组中，
			注意是子群组，不能直接作为孩子加到MapNode下面，目前原因不明*/
			if (NULL != ptrNode)
			{
				ptrMapNode->addChild(ptrNode, m_minLod, m_maxLod);
				//OsgCoreSingleton::Instance()->RTTPickGroup()->addChild(ptrNode);
				m_vecElement.push_back(ptrElement);
			}
		}break;
		default:
		{

		}break;
	}
}

void GroupElement::RemoveElement(Element * ptrElement)
{
	std::vector<ElementPtr>::iterator it = std::find(m_vecElement.begin(), m_vecElement.end(), ptrElement);
	osg::ref_ptr<osg::LOD> ptrMapNode = m_ptrElementHandle->CastTo<osg::ref_ptr<osg::LOD>>();
	switch (ptrElement->Type())
	{
		case eModelElement:
		case eTextureMovieElement:
		case eStreamMovieElement:
		case ePolygonSymbolElement:
		case elineSymbolElement3D:
		case eLinePictureElement3D:
		case eTextElement:
		case eSquareElement:
		{
			osg::ref_ptr<osg::Node> ptrNode = ptrElement->Handle()->CastTo<osg::ref_ptr<osg::Node> >();

			if (NULL != ptrNode)
			{
				ptrMapNode->removeChild(ptrNode);
				m_vecElement.erase(it);
			}
		}break;
		case eBillboardElement:
		{
			BillBoardElementPtr ptrBill = dynamic_cast<BillBoardElement*>(ptrElement);
			osg::ref_ptr<osg::Node> ptrNode = ptrBill->LodHandle()->CastTo<osg::ref_ptr<osg::Node> >();

			if (NULL != ptrNode)
			{
				ptrMapNode->removeChild(ptrNode);
				m_vecElement.erase(it);
			}
		}break;
		case eGeoBillBoardElement:
		{
			GeoBillBoardElementPtr ptrBill = dynamic_cast<GeoBillBoardElement*>(ptrElement);
			osg::ref_ptr<osg::Node> ptrNode = ptrBill->LodHandle()->CastTo<osg::ref_ptr<osg::Node> >();

			if (NULL != ptrNode)
			{
				ptrMapNode->removeChild(ptrNode);
				m_vecElement.erase(it);
			}
		}break;
		default:
		{

		}break;
	}
}

void GroupElement::RemoveAllElement()
{
	std::vector<ElementPtr>::iterator it = m_vecElement.begin();

	for (it; it != m_vecElement.end(); it++)
	{
		RemoveElement(*it++);
		it = m_vecElement.begin();
	}
	m_vecElement.clear();
}

void GroupElement::SetLod(int minLod, int maxLod) 
{
	m_minLod = minLod;
	m_maxLod = maxLod;
	osg::ref_ptr<osg::LOD> ptrMapNode = m_ptrElementHandle->CastTo<osg::ref_ptr<osg::LOD>>();
	for (int i = 0, size = m_vecElement.size(); i < size; i++)
	{
		ptrMapNode->setRange(i, m_minLod, m_maxLod);
	}
}

int GroupElement::GetMinLod()
{
	return m_minLod;
}

int GroupElement::GetMaxLod()
{
	return m_maxLod;
}

int GroupElement::ElementsSize()
{
	return m_vecElement.size();
}

Element * GroupElement::GetElement(int index)
{
	if (index<0 || index >= m_vecElement.size())
		return NULL;
	return m_vecElement[index];
}

bool GroupElement::Visible()
{
	return m_ptrElementHandle->CastTo<osg::ref_ptr<osg::DistanceLOD>>()->Visible();
}

void GroupElement::Visible(bool bVisible)
{
	m_ptrElementHandle->CastTo<osg::ref_ptr<osg::DistanceLOD>>()->Visible(bVisible);
	for (int i = 0, size = m_vecElement.size(); i < size; i++)
	{
		m_vecElement[i]->Visible(bVisible);
	}
}

Element * GroupElement::findByNode(osg::Node * node)
{
	for (int i = 0, size = m_vecElement.size(); i < size; i++)
	{
		// 仅能查询
		if (!m_vecElement[i]->Handle())
			continue;
		if (m_vecElement[i]->Handle()->CastTo<osg::ref_ptr<osg::Node>>() == node)
			return m_vecElement[i];
	}
	return NULL;
}

GLOBE_ENDNS