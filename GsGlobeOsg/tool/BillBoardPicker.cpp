#include "OsgCoreSingleton.h"
#include "BillBoardPicker.h"
#include <osgViewer/Viewer>
#include <PlaceName/PlaceNode.h>
#include <PlaceName/GeoBillboard.h>
#include <osgEarth/Registry>
#include <osgEarth/VirtualProgram>
#include <Feature.h>

namespace GeoGlobe 
{
	static osg::Uniform*     s_highlightUniform;

	/**
	* Callback that you install on the RTTPicker.
	*/
	struct MyPickCallback : public osgEarth::Util::RTTPicker::Callback
	{
		BillBoardPicker* m_ptrBillBoardPicker;

		MyPickCallback(BillBoardPicker* ptrBillBoardPicker)
		{
			m_ptrBillBoardPicker = ptrBillBoardPicker;
		}

		void DealWith(osgEarth::ObjectID id, osg::Node* ptrNode)
		{
			s_highlightUniform->set(id);

			std::string strLayerName = "";
			std::string strOID = "";
			std::string x = "";
			std::string y = "";
			std::string z = "";
			ptrNode->getUserValue("LayerName", strLayerName);
			ptrNode->getUserValue("OID", strOID);
			ptrNode->getUserValue("x", x);
			ptrNode->getUserValue("y", y);
			ptrNode->getUserValue("z", z);

			GLOBE_NAME::FeaturePtr ptrGeoFeature = new GLOBE_NAME::Feature();
			ptrGeoFeature->m_attr["LayerName"] = strLayerName;
			ptrGeoFeature->m_attr["OID"] = strOID;
			ptrGeoFeature->m_attr["x"] = x;
			ptrGeoFeature->m_attr["y"] = y;
			ptrGeoFeature->m_attr["z"] = z;

			if (m_ptrBillBoardPicker)
				m_ptrBillBoardPicker->OnPicked(ptrGeoFeature);
		}

		virtual void onHit(osgEarth::ObjectID id)
		{
			// Check whether it's an annotation:
			GeoGlobe::Annotation::PlaceNode* ptrPlaceNode = osgEarth::Registry::objectIndex()->get<GeoGlobe::Annotation::PlaceNode>(id);
			GeoGlobe::PlaceName::GeoBillboard * ptrGeoPlaceNode = osgEarth::Registry::objectIndex()->get<GeoGlobe::PlaceName::GeoBillboard>(id);
			if (ptrPlaceNode)
			{	
				DealWith(id, ptrPlaceNode);
			}
			else if (ptrGeoPlaceNode)
			{
				DealWith(id, ptrGeoPlaceNode);
			} 
			else
			{
				s_highlightUniform->set(0u);
			}
		}

		virtual void onMiss()
		{
			s_highlightUniform->set(0u);
		}

		// pick whenever the mouse moves.
		virtual bool accept(const osgGA::GUIEventAdapter& ea, const osgGA::GUIActionAdapter& aa)
		{
			return ea.getButton() == ea.LEFT_MOUSE_BUTTON && ea.getEventType() == ea.DOUBLECLICK;
		}
	};

	// Shaders that will highlight the currently "picked" feature.

	const char* highlightVert =
		"#version " GLSL_VERSION_STR "\n"
		"uniform uint objectid_to_highlight; \n"
		"uint oe_index_objectid;      // Stage global containing object id \n"
		"flat out int selected; \n"
		"void checkForHighlight(inout vec4 vertex) \n"
		"{ \n"
		"    selected = (objectid_to_highlight > 1u && objectid_to_highlight == oe_index_objectid) ? 1 : 0; \n"
		"} \n";
// billboard 关闭显示效果
	const char* highlightFrag =
		"#version " GLSL_VERSION_STR "\n"
		"flat in int selected; \n"
		"void highlightFragment(inout vec4 color) \n"
		"{ \n"
		"    if ( selected == 1 ) \n"
		"        color.rgb = mix(color.rgb, clamp(vec3(0.5,2.0,2.0)*(1.0-color.rgb), 0.0, 1.0), 0.0); \n"
		"} \n";

	void installHighlighter(osg::StateSet* stateSet, int attrLocation)
	{
		// This shader program will highlight the selected object.
		osgEarth::VirtualProgram* vp = osgEarth::VirtualProgram::getOrCreate(stateSet);
		vp->setFunction("checkForHighlight", highlightVert, osgEarth::ShaderComp::LOCATION_VERTEX_CLIP);
		vp->setFunction("highlightFragment", highlightFrag, osgEarth::ShaderComp::LOCATION_FRAGMENT_COLORING);

		// Since we're accessing object IDs, we need to load the indexing shader as well:
		osgEarth::Registry::objectIndex()->loadShaders(vp);

		// A uniform that will tell the shader which object to highlight:
		s_highlightUniform = new osg::Uniform("objectid_to_highlight", 0u);
		stateSet->addUniform(s_highlightUniform);
	}

	BillBoardPicker::BillBoardPicker()
	{
		// create a picker of the specified size.
		m_rttPicker = new osgEarth::Util::RTTPicker();
		GLOBE_NAME::OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(m_rttPicker);

		// add the graph that will be picked.
		m_rttPicker->addChild(GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode());

		osg::ref_ptr<MyPickCallback> ptrPickCallBack = new MyPickCallback(this);
		// install a callback that controls the picker and listens for hits.
		m_rttPicker->setDefaultCallback(ptrPickCallBack);

		installHighlighter(
			GLOBE_NAME::OsgCoreSingleton::Instance()->MapNode()->getOrCreateStateSet(),
			osgEarth::Registry::objectIndex()->getObjectIDAttribLocation());
	}


	void BillBoardPicker::StartPick()
	{
		GLOBE_NAME::OsgCoreSingleton::Instance()->OsgViewer()->addEventHandler(m_rttPicker);
	}

	void BillBoardPicker::EndPick()
	{
		GLOBE_NAME::OsgCoreSingleton::Instance()->OsgViewer()->removeEventHandler(m_rttPicker);
	}

	BillBoardPicker::~BillBoardPicker()
	{

	}
}