#pragma once
#include "../utility/preconfig.h"
#include <object.h>  
#include <symbol.h>  
  
KERNEL_NS

/// \brief 绘制设备类型
enum GsPaintDeviceType
{
	/// \brief 未知设备
	eUnknownDevice = -1,

	/// \brief 基于内存位图的设备
	eMemoryDevice,
	/// \brief 基于本机文件的设备
	eFileDevice,

	/// \brief Windows平台上使用GDI在HWND窗体上绘制的设备
	eWin32HwndDeviceDC,
	/// \brief Windows平台上使用Direct2D硬件加速画布在HWND窗体上绘制的设备
	// \details 仅在windows7及以上版本支持
	eWin32HwndDeviceD2D,

	/// \brief 基于QT的BackingStore创建的绘制设备
	eQTBackingStore,
	
	/// \brief 基于QT的QImage的设备
	eQTImageDevice,

	/// \brief OpenGL硬件加速绘制的设备
	eOpenGLDevice,
	/// \brief OpenGLES硬件加速绘制的设备
	eOpenGLESDevice,
};
/// \brief 绘制设备能力
/// \details 用于GsPaintDevice::TestCapbility方法测试设备能力
enum GsPaintDeviceCapbility
{
	/// \brief 绘制设备是否支持将窗体会绘制的内容复制为图片
	/// \details 0:表示不支持，1：表示支持
	eCanGrabWindowImage,
};
/// \brief 绘制设备抽象基类
/// \details 不同操作系统上的绘制设备接口不一，通过绘制设备接口实现所有操作系统上绘制的设备的同意操作。
class GS_API GsPaintDevice:public Utility::GsRefObject
{
protected:
	GsPaintDeviceType m_eType;
	GsPaintDevice();
public:
	virtual ~GsPaintDevice();
	
	/// \brief 设备的类型
	virtual GsPaintDeviceType DeviceType();


	/// \brief 重新绑定设备的内部句柄，用于设备状态发生变化后重新绑定
	virtual bool Rebind(void* pHandle, const char* strOption = NULL);

	/// \brief 测试绘制设备上是否支持某项能力
	/// \param eCapbility 绘制设备的能力
	/// \return 返回测试结果，通常情况下0为false，1位为ture
	virtual int TestCapbility(GsPaintDeviceCapbility eCapbility) = 0;

	/// \brief 以为设备为基础创建一个指定大小的位图画布
	/// \details 创建一个可以在设备绘制的位图，大小通常和设备的显示大小相当。
	/// \details 当某种显示设备不支持eCanGrabWindowImage能力时需要由Display通过此方法创建临时位图以实现双缓冲绘图。
	/// \param extent 要创建的位图大小
	/// \param pOld 上一次使用此方法创建的位图
	/// \return 创建的位图画布
	virtual GsImageCanvasPtr CreateImageCanvas(const Utility::GsRect& extent,GsImageCanvas* pOld = 0) = 0;

	/// \brief 开始设备上的绘制
	/// \param extent 绘制的范围
	/// \return 返回可以在此设备上绘制的画布
	virtual GsCanvasPtr StartDrawing(const Utility::GsRect& extent) = 0;

	/// \brief 结束设备上的绘制
	virtual bool EndDrawing() = 0;
	 	
	/// \brief 通知设备外部接收到绘制设备改变消息
	virtual void OnSizeChanged()= 0;


	/// \brief 根据设备类型创建系统内置的可绘制设备
	/// \param eType 可绘制设备内省
	/// \param pHandle 绘制设备在该平台上的句柄或者相关数据，如windows平台上位HWND句柄
	/// \param  strOption 更多的参数
	static Utility::GsSmarterPtr<GsPaintDevice> CreatePaintDevice(GsPaintDeviceType eType,void* pHandle,const char* strOption = NULL);
};
/// \brief GsPaintDevicePtr
GS_SMARTER_PTR(GsPaintDevice);


/// \brief 绘制设备抽象工厂
/// \details 用于创建不同操作系统的上的绘制设备
class GS_API GsPaintDeviceFactory:public Utility::GsRefObject
{
public:
	virtual ~GsPaintDeviceFactory(){}

	/// \brief 从窗体或者设备句柄创建绘制设备
	virtual GsPaintDevicePtr CreatePaintDevice(void* pHandle,const char* strOption = NULL) = 0;
};
/// \brief GsPaintDeviceFactoryPtr
GS_SMARTER_PTR(GsPaintDeviceFactory);

KERNEL_ENDNS