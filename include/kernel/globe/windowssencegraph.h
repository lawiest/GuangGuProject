#pragma once
#include "sencegraph.h"   

#ifdef _WIN32

GLOBE_NS
template<class T>
class GsWindowsSenceGraph :public T
{
	HWND m_hwnd;
public:
	GsWindowsSenceGraph(HWND hwnd)
	{
		m_hwnd = hwnd;
	}
	virtual void* WindowHandle()
	{
		return (void*)m_hwnd;
	}

};
GLOBE_ENDNS

#endif
