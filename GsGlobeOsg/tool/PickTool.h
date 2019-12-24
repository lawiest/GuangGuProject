#ifndef GEOGLOBE_TOOL_PICKTOOL
#define GEOGLOBE_TOOL_PICKTOOL 1

#include <osgGA/GUIEventHandler>
#include <Model/LodModel.h>
#include <osgViewer/Viewer>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/IntersectionVisitor>
using namespace std;
namespace GeoGlobe
{
	namespace Tool
	{
		class PickTool:public osgGA::GUIEventHandler
		{
		public:

			bool _mouseDown ;
			float _mouseDownX ;
			float _mouseDownY ;
			int j;
			osg::Node::NodeMask _intersectionMask;
			bool _insert;

			//std::vector<osg::ref_ptr<GeoGlobe::Model::LodModel>> _selected;
			osg::ref_ptr<GeoGlobe::Model::LodModel> _selectedModel;

			PickTool()
			{

				_intersectionMask=0x1;
				_insert=false;
				j=1;
			}

			bool pick(osgViewer::View* view, double x, double y)
			{

				//osgUtil::LineSegmentIntersector::Intersections results;  
				//view->computeIntersections( x, y, results, _intersectionMask );
				osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW,x,y);
				lsi->setIntersectionLimit(lsi->LIMIT_NEAREST);
				osgUtil::IntersectionVisitor iv(lsi.get());
				iv.setTraversalMask(_intersectionMask);
				view->getCamera()->accept(iv);
				//_node->accept(iv);

				//ModelPicker picker(lsi.get());
				//picker.setTraversalMask(_intersectionMask);
				//view->getCamera()->accept(picker);

				if (_selectedModel)
				{
					_selectedModel->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
				}

				if (lsi->containsIntersections())
				{
					GeoGlobe::Model::LodModel* lodModel=dynamic_cast<GeoGlobe::Model::LodModel*>(lsi->getFirstIntersection().drawable->getParent(0)->getParent(0)->getParent(0));
					if (lodModel)
					{
						_selectedModel=lodModel;
						_selectedModel->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::ON|osg::StateAttribute::PROTECTED);
						cout<<_selectedModel->_options->_feature->getFeatureID()<<endl;
						cout<<_selectedModel->_options->_feature->getFeatureName()<<endl;
						cout<<_selectedModel->_options->_feature->getLocation().dLatitude<<endl;
						cout<<_selectedModel->_options->_feature->getLocation().dLongitude<<endl;
						cout<<_selectedModel->_options->_feature->getLocation().dElevation<<endl;

					}
					//	_selected=()lsi->getFirstIntersection().drawable->getParent(0)->getParent(0)->getParent(0);


				}


				return false;
			}

			virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
			{

				osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView()); 

				if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN && ea.getKey() == osgGA::GUIEventAdapter::KEY_Control_L)
				{  
					_insert=true;
				}
				else if (ea.getEventType() == osgGA::GUIEventAdapter::KEYUP && ea.getKey() == osgGA::GUIEventAdapter::KEY_Control_L)
				{
					_insert=false;
				}


				if (_insert&&ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
				{        
					_mouseDown = true;
					_mouseDownX = ea.getX();
					_mouseDownY = ea.getY();
				}
				else if (_insert&&ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
				{ 
					if (osg::equivalent(ea.getX(), _mouseDownX) && osg::equivalent(ea.getY(), _mouseDownY))
					{

						pick(view, ea.getX(), ea.getY());
						
					}
				}


				return false;
			}

		};
	}
}

#endif