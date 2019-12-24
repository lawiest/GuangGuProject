#pragma once
#include <utility.h>
#include "sencegraph.h"
#include "earth.h"

#ifdef _WIN32
#include<windows.h>
#endif

namespace osgViewer
{
	class Viewer;
}

GLOBE_NS

class GS_API OsgEngine :public Gs3DEngine//, public OpenThreads::Thread
{

public:
	OsgEngine(void* handle, const Utility::GsConfig& config);
	virtual ~OsgEngine();

	osgViewer::Viewer* Viewer();
	EarthPtr GetEarth();
	void InitOSG();
	void PreFrameUpdate(void);
	void PostFrameUpdate(void);
	void Close();
	void Frame();
	virtual void run();

private:
	Utility::GsConfig m_Config;
	osgViewer::Viewer* m_Viewer;
	EarthPtr m_ptrEarth;
	bool m_Done;

#ifdef _WIN32
	HWND m_hWnd;
#endif
};

GLOBE_ENDNS