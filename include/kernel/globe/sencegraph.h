#pragma once  
#include "utility.h"
#include <kernel.h>

GLOBE_NS 

class GS_API Gs3DEngine :public Utility::GsRefObject
{
public:
	virtual ~Gs3DEngine(); 
};
/// \brief Gs3DEnginePtr
GS_SMARTER_PTR(Gs3DEngine);

class GS_API GsSenceGraph:public Utility::GsRefObject 
{
protected:
	Gs3DEnginePtr		m_ptrEngine;
	Utility::GsConfig	m_Config;
private:
	void UpdateEngine();
public:
	GsSenceGraph();
	virtual ~GsSenceGraph();
	virtual Utility::GsConfig Config();
	void Config(const Utility::GsConfig& config);

	virtual void Initialize() = 0;
	virtual void Close();

	virtual void* WindowHandle() = 0;
}; 

/// \brief GsSenceGraphPtr
GS_SMARTER_PTR(GsSenceGraph);

class GS_API GsGLSenceGraph:public GsSenceGraph
{  
public:
	GsGLSenceGraph();
	virtual ~GsGLSenceGraph();
	virtual void Initialize();

public:
	void Test(std::string strUrl);
	void Frame();
	Gs3DEngine* GetOSGEngine();
};
/// \brief GsGLSenceGraphPtr
GS_SMARTER_PTR(GsGLSenceGraph);

GLOBE_ENDNS