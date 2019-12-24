#ifndef OSGEARTHFEATURES_FEATURE_FEATUREQUREY
#define OSGEARTHFEATURES_FEATURE_FEATUREQUREY 1

#include <osgEarthFeatures/Feature>
#include <osgEarthFeatures/FeatureSource>
#include <osgEarthUtil/FeatureQueryTool>
#include <osgEarthUtil/Controls>
#include <tool/CodeTransformHelp.h>
#include <osgEarth/Registry> 

namespace GeoGlobe
{
	namespace GeoVector
	{
		
		using namespace osgEarth::Features;
		using namespace osgEarth::Util;

		class FeatureInfoReadoutCallback : public FeatureQueryTool::Callback
		{
		public:
			FeatureInfoReadoutCallback(osgEarth::MapNode* mapnode)
			{
				m_MapNode = mapnode;
				m_vec = NULL;
			}
			virtual ~FeatureInfoReadoutCallback() {}
        
		protected:			
			struct Selection {
				osg::observer_ptr<FeatureSourceIndexNode> _index;
				osg::observer_ptr<osg::Group>             _group;
				FeatureID                                 _fid;
				bool operator < ( const Selection& rhs ) const { return _fid < rhs._fid; }
			};
			typedef std::set<Selection> SelectionSet;
			SelectionSet _selections;
			VectorInfoWindow *m_vec;

			osg::ref_ptr<osgEarth::MapNode> m_MapNode;
			std::vector<osg::ref_ptr<osgEarth::Annotation::FeatureNode>> m_vecPolyNode;

		public:
			virtual void onHit( FeatureSourceIndexNode* index, FeatureID fid, const EventArgs& args )
			{
				clear();			
				//获取属性
				const Feature* f = 0L;
				if ( index && index->getFeature(fid, f) )
				{	
					if (NULL != m_vec)
						m_vec->close();

					const AttributeTable& attrs = f->getAttrs();
					string strLayerAtrr = (*attrs.find("f_vc_layer")).second.getString();
					double dArea = atof((*attrs.find("shape_area")).second.getString().c_str());
					double dLeng = atof((*attrs.find("shape_leng")).second.getString().c_str());
					
					m_vec = new VectorInfoWindow(fid,strLayerAtrr,dArea,dLeng);
					m_vec->show();


					//高亮
					Feature* HightLightFeature = new Feature(*f,osg::CopyOp::DEEP_COPY_ALL);

					HightLightFeature->style()->getOrCreate<LineSymbol>()->stroke()->color() = Color(Color::White, 1.0);
					HightLightFeature->style()->getOrCreate<LineSymbol>()->stroke()->width() = 2.0f;
					HightLightFeature->style()->getOrCreate<LineSymbol>()->stroke()->stipple() = 0x0F0F;
					HightLightFeature->style()->getOrCreate<LineSymbol>()->tessellation() = 20;

					HightLightFeature->style()->getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::Yellow, 1.0);
					HightLightFeature->style()->getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN /*AltitudeSymbol::CLAMP_ABSOLUTE*/;
					HightLightFeature->style()->getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_DRAPE;

					osg::ref_ptr<osgEarth::Annotation::FeatureNode> polyNode = new osgEarth::Annotation::FeatureNode(m_MapNode.get(), HightLightFeature, true);
					m_MapNode->addChild(polyNode);
					m_vecPolyNode.push_back(polyNode);
				}
				args._aa->requestRedraw();
			}

			virtual void onMiss( const EventArgs& args )
			{
				clear();
				args._aa->requestRedraw();
			}
			void clear()
			{
				for( SelectionSet::const_iterator i = _selections.begin(); i != _selections.end(); ++i )
				{
					const Selection& selection = *i;
					osg::ref_ptr<osg::Group> safeGroup = selection._group.get();
					if ( safeGroup.valid() && safeGroup->getNumParents() > 0 )
					{
						osg::Group* parent = safeGroup->getParent(0);
						if ( parent ) 
							parent->removeChild( safeGroup.get() );
					}
				}
				_selections.clear();

				std::vector<osg::ref_ptr<osgEarth::Annotation::FeatureNode>>::iterator it = m_vecPolyNode.begin();
				for(;it != m_vecPolyNode.end(); it++)
				{
					m_MapNode->removeChild(*it);
				}
				m_vecPolyNode.clear();
			}
		};
	}//geovector
}//geoglobe
#endif//OSGEARTHFEATURES_FEATURE_FEATUREQUREY