#pragma once
#include <paintdevice.h>
#include <utility.h>
#include <QImage>
#include "../../utility/preconfig.h"
KERNEL_NS
namespace QT{
class GS_API GsQTCanvas:public GsImageCanvas
{
public:
    virtual void FinishDraw() = 0;
    virtual void* QTImage() = 0;
};
GS_SMARTER_PTR(GsQTCanvas);

/// \brief QT画布类厂
/// \details QT画布类厂
class GS_API GsQTCanvasFactory:public GsPaintDeviceFactory
{
public:
    /// \brief 创建一个基于内存的QT画布
    static GsImageCanvasPtr CreateImageCanvas(int w,int h);
    /// \brief 根据位图创建画布
    static GsImageCanvasPtr CreateImageCanvas(QImage* img);

    /// \brief 创建一个输出SVG的Canvas
    static GsCanvasPtr CreateSVGCanvas(int w,int h,const char* strSVGFileName);


    /// \brief 创建基于QBackingStore的窗体Canvas对象
    /// \details 由QT界面程序中调用
    static GsCanvasPtr CreateBackingStoreCanvas(void* pbackingStore);

    /// \brief 从窗体或者设备句柄创建绘制设备
    virtual GsPaintDevicePtr CreatePaintDevice(void* pHandle,const char* strOption = NULL);


};
GS_SMARTER_PTR(GsQTCanvasFactory);
DECLARE_CLASS_CREATE(GsQTCanvasFactory);
}
KERNEL_ENDNS
