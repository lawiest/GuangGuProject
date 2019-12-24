#include "sencegraph.h"
#include "earth.h"
#include "OsgEngine.h"

#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib,"gsutilityd.lib")
#pragma comment(lib,"gsgeodatabased.lib")
#pragma comment(lib,"gssymbold.lib")
#pragma comment(lib,"gsgeometryd.lib")
#pragma comment(lib,"gsmapd.lib") 
#pragma comment(lib,"gsutilityd.lib") 
#pragma comment(lib,"gsspatialreferenced.lib")   
#pragma comment(lib,"gswin32portd.lib") 

#else
#pragma comment(lib,"gsutility.lib")

#pragma comment(lib,"gsgeodatabase.lib")
#pragma comment(lib,"gssymbol.lib")
#pragma comment(lib,"gsgeometry.lib")
#pragma comment(lib,"gsutility.lib") 
#pragma comment(lib,"gsspatialreference.lib") 
#pragma comment(lib,"gsmap.lib")  
#pragma comment(lib,"gswin32port.lib") 
#endif
#pragma comment(lib,"user32.lib") 
#endif
 
GLOBE_NS

GsSenceGraph::GsSenceGraph()
{

}
GsSenceGraph::~GsSenceGraph()
{ 
	Close();
}
Utility::GsConfig GsSenceGraph::Config()
{
	return m_Config;
}
void GsSenceGraph::Config(const Utility::GsConfig& config)
{
	m_Config = config;
}
void GsSenceGraph::Close()
{
	if (!m_ptrEngine)
		return;

	OsgEngine* osg = dynamic_cast<OsgEngine*>(m_ptrEngine.p);
	if(osg)
		osg->Close();	

	m_ptrEngine.Release();
}

Gs3DEngine* GsGLSenceGraph::GetOSGEngine()
{
	OsgEngine* osgEngine = dynamic_cast<OsgEngine*>(m_ptrEngine.p);
	return osgEngine;
}

GsGLSenceGraph::GsGLSenceGraph()
{
}

GsGLSenceGraph::~GsGLSenceGraph()
{
}
void GsGLSenceGraph::Frame()
{
	OsgEngine* osgEngine = dynamic_cast<OsgEngine*>(m_ptrEngine.p);
	osgEngine->Frame();
}

void GsGLSenceGraph::Test(std::string strUrl)
{
	OsgEngine* osgEngine = dynamic_cast<OsgEngine*>(m_ptrEngine.p);
	osgEngine->GetEarth()->Test(strUrl);
}

void GsGLSenceGraph::Initialize()
{
	void* win = WindowHandle();
	OsgEngine* pEngine = new OsgEngine(win,m_Config);
	m_ptrEngine = pEngine;
	//Æô¶¯äÖÈ¾Ïß³Ì
	//pEngine->start();
}

GLOBE_ENDNS