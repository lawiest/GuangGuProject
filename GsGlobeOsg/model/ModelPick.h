#pragma once

#include <osgViewer/Viewer>
#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIActionAdapter>

#include <model/LodModel.h>
#include "Analysis/FormHook.h"
using namespace osg;
namespace GeoGlobe
{
	namespace Model
	{
		class PickHandler : public osgGA::GUIEventHandler {
		public:

			PickHandler(osg::Group* root);

			~PickHandler();

			bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

			virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);
			void virtualpick(GeoGlobe::Model::LodModel *pModelNode);
			//清除已有模型状态
			void clearModelState();
			void clearModelState(unsigned long long nID);
			void setMainWindow(GeoGlobe::IMainWindow * ptrMainWindow);
			void setConWnd(GeoGlobe::IConsultationWindow * ptrConWnd);
			void setFinished(bool bFinish);

			void Transparent(float fValue);
		protected:
			std::map<ULONGLONG,osg::ref_ptr<GeoGlobe::Model::LodModel>> _mapModel;
			std::map<ULONGLONG,osg::ref_ptr<osg::StateSet>> _MapState;
			osg::Group* m_root;

			GeoGlobe::IMainWindow * m_ptrMainWindow;
			GeoGlobe::IConsultationWindow * m_ptrConWnd;
			bool         m_bTransparent;
			bool         m_bFinished;
			float        m_fTransValue;
		};

	}
}