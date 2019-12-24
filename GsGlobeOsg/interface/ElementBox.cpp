#include "ElementBox.h"
#include "ModelElement.h"
#include "Element.h"
#include "OsgCoreSingleton.h"
#include <osg/Node>
#include <osg/Group>
#include <osgEarth/MapNode>
#include <OverlayElement.h>
#include <BillBoardElement.h>
#include <GroupElement.h>
#include <GeoBillBoardElement.h>

GLOBE_NS

ElementBox::ElementBox()
{

}

ElementBox::~ElementBox()
{

}

void ElementBox::AddElement(Element *ptrElement)
{
	osg::Group* ptrMapNode = OsgCoreSingleton::Instance()->MapNode();
	if (NULL == ptrElement || NULL == ptrMapNode)
		return;

	switch (ptrElement->Type())
	{
		case eModelElement:
		case eTextureMovieElement:
		case eStreamMovieElement:
		case ePointSymbolElement:
		case ePointSymbolElement3D:
		case elineSymbolElement3D:
		case eLineSymbolElement:
		case eLinePictureElement3D:
		case ePolygonSymbolElement:
		case eGroupElement:
		case eSquareElement:
		{
			osg::ref_ptr<osg::Node> ptrNode = ptrElement->Handle()->CastTo<osg::ref_ptr<osg::Node> >();

			if (NULL != ptrNode)
			{
				ptrMapNode->addChild(ptrNode);
				m_vecElement.push_back(ptrElement);
			}
			
		}break;
		case eLonAndLatElement:
		case eCompassElement:
		{
			ptrElement->Build();
			m_vecElement.push_back(ptrElement);
		}break;
		case eOverlayElement:
		{
			osg::ref_ptr<osg::Node> ptrNode = OverlayElement::m_ptrOverlayNode->CastTo<osg::ref_ptr<osg::Node> >();
			if (NULL != ptrNode)
			{
				ptrMapNode->addChild(ptrNode);
				m_vecElement.push_back(ptrElement);
			}
		}break;
		case eTextElement:
		{
			osg::ref_ptr<osg::Node> ptrNode = ptrElement->Handle()->CastTo<osg::ref_ptr<osg::Node> >();

			if (NULL != ptrNode)
			{
				OsgCoreSingleton::Instance()->RTTPickGroup()->addChild(ptrNode);
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
				OsgCoreSingleton::Instance()->RTTPickGroup()->addChild(ptrNode);
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
				OsgCoreSingleton::Instance()->RTTPickGroup()->addChild(ptrNode);
				m_vecElement.push_back(ptrElement);
			}
		}break;
		default:
		{

		}break;
	}
}

Element* ElementBox::LonAndLatElement()
{
	std::vector<ElementPtr>::iterator it = m_vecElement.begin();

	for (it; it!=m_vecElement.end(); it++)
	{
		if ((*it)->Type() == eLonAndLatElement)
			return (*it);
	}

	return NULL;
}

Element*  ElementBox::CompassElement()
{
	std::vector<ElementPtr>::iterator it = m_vecElement.begin();

	for (it; it != m_vecElement.end(); it++)
	{
		if ((*it)->Type() == eCompassElement)
			return (*it);
	}

	return NULL;
}

void ElementBox::RemoveElement(Element * ptrElement)
{
	std::vector<ElementPtr>::iterator it = std::find(m_vecElement.begin(), m_vecElement.end(), ptrElement);

	if (it == m_vecElement.end())
		return;

	switch (ptrElement->Type())
	{
		case eModelElement:
		case eTextureMovieElement:
		case eStreamMovieElement:
		case ePointSymbolElement:
		case ePointSymbolElement3D:
		case ePolygonSymbolElement:
		case elineSymbolElement3D:
		case eLinePictureElement3D:
		case eLonAndLatElement:
		case eCompassElement:
		case eTextElement:
		case eGroupElement:
		case eSquareElement:
		{
			osg::ref_ptr<osg::Node> ptrNode = ptrElement->Handle()->CastTo<osg::ref_ptr<osg::Node> >();

			if (NULL != ptrNode)
			{
				OsgCoreSingleton::Instance()->MapNode()->removeChild(ptrNode);
				m_vecElement.erase(it);
			}
		}break;
		case eOverlayElement:
		{
			osg::ref_ptr<osg::Node> ptrNode = OverlayElement::m_ptrOverlayNode->CastTo<osg::ref_ptr<osg::Node> >();
			if (NULL != ptrNode)
			{
				OsgCoreSingleton::Instance()->MapNode()->removeChild(ptrNode);
				m_vecElement.erase(it);
			}
		}break;
		case eBillboardElement:
		{
			BillBoardElementPtr ptrBill = dynamic_cast<BillBoardElement*>(ptrElement);
			osg::ref_ptr<osg::Node> ptrNode = ptrBill->LodHandle()->CastTo<osg::ref_ptr<osg::Node> >();

			if (NULL != ptrNode)
			{
				OsgCoreSingleton::Instance()->RTTPickGroup()->removeChild(ptrNode);
				m_vecElement.erase(it);
			}
		}break;
		case eGeoBillBoardElement:
		{
			GeoBillBoardElementPtr ptrBill = dynamic_cast<GeoBillBoardElement*>(ptrElement);
			osg::ref_ptr<osg::Node> ptrNode = ptrBill->LodHandle()->CastTo<osg::ref_ptr<osg::Node> >();

			if (NULL != ptrNode)
			{
				OsgCoreSingleton::Instance()->RTTPickGroup()->removeChild(ptrNode);
				m_vecElement.erase(it);
			}
		}break;
		default:
		{

		}break;
	}

}


void ElementBox::RemoveAllElement()
{
	std::vector<ElementPtr>::iterator it = m_vecElement.begin();

	for (it; it != m_vecElement.end(); it++)
	{
		RemoveElement(*it++);
		it = m_vecElement.begin();
	}
	m_vecElement.clear();
}

Element * ElementBox::findByNode(osg::Node * node)
{
	for (int i = 0, size = m_vecElement.size(); i < size; i++)
	{
		// 仅能查询
		if (!m_vecElement[i]->Handle())
			continue;
		// 若是group 则从group查询
		GroupElement* group = dynamic_cast<GroupElement*>(m_vecElement[i].p);
		if (group)
		{
			Element * findEle = group->findByNode(node);
			if (findEle)
				return findEle;
		}else
		if (m_vecElement[i]->Handle()->CastTo<osg::ref_ptr<osg::Node> >() == node)
			return m_vecElement[i];
	}
	return NULL;
}

GLOBE_ENDNS