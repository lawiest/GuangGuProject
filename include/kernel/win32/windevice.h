#pragma once
#include "paintdevice.h"
#include "windows.h"
#include "../../utility/preconfig.h" 

KERNEL_NS
namespace Win32{
class GS_API GsWin32Canvas:public GsImageCanvas
{
public:
    virtual void FinishDraw() = 0;
    virtual void* Win32Image() = 0;
};

GS_SMARTER_PTR(GsWin32Canvas);

/// \brief Win32画布类厂
/// \details Win32画布类厂
class GS_API GsWin32CanvasFactory:public GsPaintDeviceFactory
{
public:
    /// \brief 创建一个基于内存的D2D画布
    static GsImageCanvasPtr CreateImageCanvas(int w,int h);
	
	/// \brief 从DC创建一个画布
	static GsCanvasPtr CreateCanvasFromHDC(void* handle);

	/// \brief 从GDI+句柄创建创建一个画布
	static GsCanvasPtr CreateCanvasFromGDIPlus(void* handle);

	/// \brief 从窗体或者设备句柄创建绘制设备
	virtual GsPaintDevicePtr CreatePaintDevice(void* pHandle,const char* strOption = NULL);

    /// \brief 创建基于Windows窗台的Canvas
    /// \details 由QT界面程序中调用
    static GsPaintDevicePtr CreateHwndDevice(HWND hWnd);

};
GS_SMARTER_PTR(GsWin32CanvasFactory);
DECLARE_CLASS_CREATE(GsWin32CanvasFactory);



}
KERNEL_ENDNS 
