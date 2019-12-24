#pragma once
#include <utility.h>

#include "SceneBox.h"
#include "LayerBox.h"
#include "SymbolBox.h"
#include "AnalysisBox.h"

namespace osg
{
	class Geometry;
	class Node;
}

namespace osgViewer
{
	class Viewer;
}

GLOBE_NS

class EarthManipulator;

//·â×°µØÇò
class GS_API Earth:public GeoStar::Utility::GsRefObject
{

public:
	Earth(const GeoStar::Utility::GsConfig& Config);
	~Earth();
	osg::Node* RootNodePtr();
	osg::Node* MapNodePtr();
	void Init(osgViewer::Viewer* pViewer);
	void PreFrameUpdate(void);
	void PostFrameUpdate(void);
	void Test(std::string strUrl);

	LayerBox* LayerBoxP();
	SceneBox* SceneBoxP();
	SymbolBox* SymbolBoxP();

private:
	osg::Node*			m_Root;
	osg::Node*			m_pMapNode;
	osgViewer::Viewer*	m_pViewer;
	EarthManipulator*	m_Manip;

	UTILITY_NAME::GsConfig	m_Config;

	SceneBoxPtr  m_ptrSceneBox;
	LayerBoxPtr  m_ptrLayerBox;
	SymbolBoxPtr m_ptrSymbolBox;
	AnalysisBoxPtr m_ptrAnalysisBox;
};

#define MAP_NODE(node) osg::ref_ptr<osgEarth::MapNode> mapNode = dynamic_cast<osgEarth::MapNode*>(node)

GS_SMARTER_PTR(Earth);

GLOBE_ENDNS