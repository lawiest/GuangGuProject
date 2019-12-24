#ifndef GEOGLOBE_TOOL_CULLINGUTILS
#define GEOGLOBE_TOOL_CULLINGUTILS 1

#include <osg/NodeCallback>
#include <osgEarth/Common>
#include <osgEarth/optional>
#include <osgEarth/SpatialReference>
#include <osg/NodeCallback>
#include <osg/ClusterCullingCallback>
#include <osg/CoordinateSystemNode>
#include <osg/MatrixTransform>
#include <osg/Vec3d>
#include <osg/Vec3>
#include <osg/ClipPlane>
#include <osgUtil/CullVisitor>
#include <osgEarth/CullingUtils>
#include <osgEarth/GeoData>

namespace GeoGlobe
{
	namespace Tool
	{
		class  DistanceCullingCallback : public osg::NodeCallback {
		public:
			DistanceCullingCallback( const osgEarth::SpatialReference* srs, const osg::Vec3d& world,float minRange,float maxRange,float maxAltitude=FLT_MAX)
			{
				_srs=srs;
				_world=world;
				_minRange=minRange;
				_maxRange=maxRange;
				_maxAltitude=maxAltitude;

			}


			void operator()(osg::Node* node, osg::NodeVisitor* nv)
			{
				if (nv->getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
				{        
					osgUtil::CullVisitor* cv = osgEarth::Culling::asCullVisitor(nv);


					osg::Vec3d eye = cv->getViewPoint();

					double alt = 0.0;

					if ( _srs && !_srs->isProjected() )
					{
						osgEarth::GeoPoint mapPoint;
						mapPoint.fromWorld( _srs.get(), eye );
						alt = mapPoint.z();
					}
					else
					{
						alt = eye.z();
					}

					float required_range = cv->getDistanceToViewPoint(_world/*getCenter()*/,false);


					if(alt < _maxAltitude&&(required_range<_minRange||required_range>_maxRange))
						return;

					traverse(node, nv );


				}


			}


		private:


			osg::ref_ptr< const osgEarth::SpatialReference > _srs;
			osg::Vec3d _world;
			float _minRange;
			float _maxRange;
			float _maxAltitude;


		};


		struct DrawableCullCallback : public osg::Drawable::CullCallback
				{
		            float  _pixelSize;
					/** do customized cull code.*/
		            DrawableCullCallback()
		            {
		            	_pixelSize=50;
		            }
					inline bool cull(osg::NodeVisitor* nv, osg::Drawable* drawable, osg::State* /*state*/) const
					{
						//std::cout<<"Drawable cull callback "<<drawable<<std::endl;

						if(_pixelSize==0)
							return false;
						osgUtil::CullVisitor* cv = osgEarth::Culling::asCullVisitor(nv);
						float pixel=(drawable->getBound().center()*cv->getCurrentCullingSet().getPixelSizeVector())*_pixelSize;
						if (pixel>drawable->getBound().radius())
						{
							return true;
						}

						return false;
					}
				};
	}


}



#endif
