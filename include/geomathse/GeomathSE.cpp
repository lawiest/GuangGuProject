#ifdef _WIN32
#include <atlpath.h>
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "GeomathSE.inl"
#ifdef LINK_GEOMATHSELIB
#pragma comment(lib,"gsGeomathSE.lib")
#endif
#ifndef LINK_GEOMATHSELIB
#ifdef _WIN32
#define DYNAMIC_LOAD_GEOMATHSE
#endif
#endif


#ifdef DYNAMIC_LOAD_GEOMATHSE

#ifdef __cplusplus  
extern "C"  
#endif  
void * _ReturnAddress(void);  
#pragma intrinsic(_ReturnAddress)  

static HMODULE _dll(HMODULE mode=0)
{
    static HMODULE h=0;
    if(h!=0) return h;

	HMODULE hCaller = NULL;   
	TCHAR szModuleName[MAX_PATH];   
	void *pRetAddr = _ReturnAddress();   
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS  , (TCHAR*)pRetAddr, &hCaller);    
	GetModuleFileName(hCaller, szModuleName, MAX_PATH);  
	CPath path(szModuleName);
	path.RemoveFileSpec();
	path.Append(_T("gsGeomathSE.dll"));
	//wcscpy( wcsrchr(szModuleName,'\\'),L"\\GeomathSE.dll" );
    h = LoadLibrary(path.m_strPath);

    return h;
}

geostar::geo_factory* _gf2()
{
    typedef void* (*fun_t)();
	static void* func = 0;
	if(func==0)
	{
		HMODULE h = _dll();
		func = GetProcAddress(h,"CreateGPfactory");
	}
    return (geostar::geo_factory*)((fun_t)func)();
}

geostar::geo_algorithm* _ga2()
{
    typedef void* (*fun_t)();
	static void* func = 0;
	if(func==0)
	{
		HMODULE h = _dll();
		func = GetProcAddress(h,"CreateGAlgorithm");
	}
    return (geostar::geo_algorithm*)((fun_t)func)();
}

geostar::geo_factory& _gf()
{
    typedef void* (*fun_t)();
    static geostar::geo_factory* _p = 0;
    if(_p!=0) return *_p;
    HMODULE h = _dll();
    void* func = GetProcAddress(h,"CreateGPfactory");
    _p = (geostar::geo_factory*)((fun_t)func)();
    return *_p;
}

geostar::geo_algorithm& _ga()
{
    typedef void* (*fun_t)();
    static geostar::geo_algorithm* _p = 0;
    if(_p!=0) return *_p;
    HMODULE h = _dll();
    void* func = GetProcAddress(h,"CreateGAlgorithm");
    _p = (geostar::geo_algorithm*)((fun_t)func)();
    return *_p;
}
#else

//创建GPfactory对象
extern "C" geostar::geo_factory* CreateGPfactory();
extern "C" geostar::geo_algorithm* CreateGAlgorithm();

geostar::geo_factory* _gf2()
{
     
    return (geostar::geo_factory*)(CreateGPfactory());
}

geostar::geo_algorithm* _ga2()
{
    
    return (geostar::geo_algorithm*)(CreateGAlgorithm());
}

geostar::geo_factory& _gf()
{
	static geostar::geo_factory*  gf = CreateGPfactory();
    return *gf;
}

geostar::geo_algorithm& _ga()
{
	static geostar::geo_algorithm* ga = CreateGAlgorithm();
    return *ga;
}
#endif
