#pragma once
#include "preconfig.h" 
#include "object.h"
#include "memorymanager.h" 
#include "vectorhelp.h"
#include <fstream>
#include "structreflect.h"
UTILITY_NS 

/// \brief 创建一个int类型的点结构体
struct GS_API GsPT
{
	int X,Y;
	GsPT();
	GsPT(const GsPT& rhs);
	GsPT(int x,int y);
	
	/// \brief 点Offset点
	GsPT operator +(const GsPT & pt) const;
	
	/// \brief offset自身
	GsPT& operator +=(const GsPT & pt);

	/// \brief 点Offset点
	GsPT operator -(const GsPT & pt) const;
	
	/// \brief offset自身
	GsPT& operator -=(const GsPT & pt); 
};
GS_DECLARE_REFLECT(GsPT);


/// \brief 创建一个float类型的点结构体
struct GS_API GsPTF
{
	float X,Y;

	/// \brief 缺省构造函数
	GsPTF();
	/// \brief 重载构造函数
	GsPTF(const GsPT& rhs);
	/// \brief 重载构造函数
	GsPTF(const GsPTF& rhs);
	/// \brief 重载构造函数
	GsPTF(int x,int y);
	/// \brief 重载构造函数
	GsPTF(float x,float y);

	/// \brief 点Offset点
	GsPTF operator +(const GsPTF & pt) const;
	
	/// \brief offset自身
	GsPTF& operator +=(const GsPTF & pt);
	
	/// \brief 点Offset点
	GsPTF operator -(const GsPTF & pt) const;
	
	/// \brief offset自身
	GsPTF& operator -=(const GsPTF & pt);
};

/// \brief 设置int型宽度和高度
struct GS_API GsSize
{
	int Width,Height;
	/// \brief 缺省构造函数
	GsSize();
	/// \brief 重载构造函数
	GsSize(const GsSize& s);
	/// \brief 从点构造
	GsSize(const GsPT& p);
	/// \brief 重载构造函数
	GsSize(int w,int h);
	
	/// \brief 是否相等
	bool operator == (const GsSize& s)const;
	/// \brief 赋值
	GsSize &operator = (const GsSize& s);
	/// \brief 是否不等
	bool operator != (const GsSize& s)const;

};
 
/// \brief 设置float型宽度和高度 
struct GS_API GsSizeF
{
	float Width,Height;
	/// \brief 缺省构造函数
	GsSizeF();
	/// \brief 重载构造函数
	GsSizeF(const GsSize& s);
	/// \brief 重载构造函数
	GsSizeF(const GsSizeF& s);
	/// \brief 重载构造函数
	GsSizeF(float w, float h);

	/// \brief 是否相等
	bool operator == (const GsSizeF& s)const;
	/// \brief 赋值
	GsSizeF &operator = (const GsSizeF& s);
	/// \brief 赋值
	GsSizeF &operator = (const GsSize& s);

	/// \brief 是否不等
	bool operator != (const GsSizeF& s)const;
};

/// \brief 设置Double型宽度和高度
struct GS_API GsSizeD
{
	double Width, Height;
	/// \brief 缺省构造函数
	GsSizeD();
	/// \brief 重载构造函数
	GsSizeD(const GsSize& s);
	/// \brief 重载构造函数
	GsSizeD(const GsSizeF& s);
	/// \brief 重载构造函数
	GsSizeD(const GsSizeD& s);

	/// \brief 重载构造函数
	GsSizeD(double w, double h);

	/// \brief 是否相等
	bool operator == (const GsSizeD& s)const;
	/// \brief 是否不等
	bool operator != (const GsSizeD& s)const;
	/// \brief 赋值
	GsSizeD &operator = (const GsSizeD& s);
	/// \brief 赋值
	GsSizeD &operator = (const GsSize& s);
	/// \brief 赋值
	GsSizeD &operator = (const GsSizeF& s);

};


/// \brief 创建一个int类型的矩形
struct GS_API GsRect
{
	int Left,Right,Top,Bottom;
	/// \brief 缺省构造函数
	GsRect();
	/// \brief 重载构造函数
	GsRect(const GsPT& pt,const GsSize& s);
	/// \brief 重载构造函数
	GsRect(const GsRect& rhs);
	/// \brief 重载构造函数
	GsRect(int x,int y,int w,int h);
	/// \brief 设置宽度
	int Width()const;
	/// \brief 设置高度
	int Height()const;
	/// \brief 偏移一个位置
	GsRect& Offset(const GsPT& pt);

	/// \brief 中心点
	GsPT Center()const;

	/// \brief 是否相交
	bool IsIntersects(const GsRect& rhs)const;

	/// \brief 计算两个相交的部分
	GsRect Intersects(const GsRect& rhs) const;

	/// \brief 是否和线相离
	bool IsDisjoin(int x1,int y1,int x2,int y2)const;

	/// \brief 是否相离
	bool IsDisjoin(const GsRect& rhs)const;

	/// \brief 是否包含
	bool IsContain(int x,int y)const;

	/// \brief 是否包含
	bool IsContain(const GsPT& pt)const;

	/// \brief 是否包含
	bool IsContain(const GsRect& rhs)const;

	/// \brief 定位点
	GsPT Location()const;
	
	/// \brief 大小
	GsSize Size() const;

	/// \brief 左上角
	GsPT LeftTop()const;
	
	/// \brief 右上角
	GsPT RightTop()const;
	
	/// \brief 左下角
	GsPT LeftBottom()const;
	
	/// \brief 右下角
	GsPT RightBottom()const;

	/// \brief 矩形偏移
	GsRect operator + (const GsPT& pt)const;

	/// \brief 矩形偏移
	GsRect operator - (const GsPT& pt)const;

	/// \brief 矩形偏移返回新的偏移后的矩形
	GsRect OffsetTo(const GsPT& pt)const;

	/// \brief 矩形合并
	GsRect operator + (const GsRect& rect)const;
	/// \brief 合并到自身
	GsRect& operator += (const GsRect& rect);
	
	/// \brief 矩形合并
	GsRect& Union(const GsRect& rect);

};

/// \brief 创建一个float类型的矩形
struct GS_API GsRectF
{
	float Left,Right,Top,Bottom;

	/// \brief 缺省构造函数
	GsRectF();
	/// \brief 构造函数重载
	GsRectF(const GsPTF& pt,const GsSizeF& s);
	/// \brief 构造函数重载
	GsRectF(const GsPT& pt,const GsSize& s);
	/// \brief 构造函数重载
	GsRectF(const GsRect& rhs);
	/// \brief 构造函数重载
	GsRectF(const GsRectF& rhs);
	/// \brief 构造函数重载
	GsRectF(int x,int y,int w,int h);
	/// \brief 构造函数重载
	GsRectF(float x,float y,float w,float h);
	/// \brief 设置宽度
	float Width()const;
	/// \brief 设置高度
	float Height()const;

	GsRectF& Offset(const GsPTF& pt);

	/// \brief 中心点
	GsPTF Center()const;

	/// \brief 是否相交
	bool IsIntersects(const GsRectF& rhs)const;
	/// \brief 计算两个相交的部分
	GsRectF Intersects(const GsRectF& rhs) const;

	/// \brief 是否和线相离
	bool IsDisjoin(float x1,float y1,float x2,float y2)const;

	/// \brief 是否相离
	bool IsDisjoin(const GsRectF& rhs)const;

	/// \brief 是否包含
	bool IsContain(float x,float y)const;

	/// \brief 是否包含
	bool IsContain(const GsPTF& pt)const;

	/// \brief 是否包含
	bool IsContain(const GsRectF& rhs)const;

	/// \brief 定位点
	GsPTF Location() const;
	
	/// \brief 大小
	GsSizeF Size()const;

	/// \brief 左上角
	GsPTF LeftTop()const;
	
	/// \brief 右上角
	GsPTF RightTop()const;
	
	/// \brief 左下角
	GsPTF LeftBottom()const;
	
	/// \brief 右下角
	GsPTF RightBottom()const;

	/// \brief 矩形合并
	GsRectF operator + (const GsRectF& rect)const;
	/// \brief 合并到自身
	GsRectF& operator += (const GsRectF& rect);
	
	/// \brief 矩形合并
	GsRectF& Union(const GsRectF& rect);


	/// \brief 矩形偏移
	GsRectF operator + (const GsPTF& pt)const;

	/// \brief 矩形偏移
	GsRectF operator - (const GsPTF& pt)const;

	/// \brief 矩形偏移返回新的偏移后的矩形
	GsRectF OffsetTo(const GsPTF& pt)const;


	/// \brief 矩形偏移
	GsRectF operator + (const GsPT& pt)const;

	/// \brief 矩形偏移
	GsRectF operator - (const GsPT& pt)const;

	/// \brief 矩形偏移返回新的偏移后的矩形
	GsRectF OffsetTo(const GsPT& pt)const;


};

/// \brief RGBA4通道颜色类型，默认为读取为char *
enum GsRGBAType
{
	eBGRA32,//BGRA是指将内存块转换为char *，依次读取分别为BGRA。而不是int，下同。
	eABGR32,

	eRGBA32,
	eARGB32,
};

/// \brief 图像编码类型
enum GsImageEncodeType
{
	eUnknownImage,
	ePNG,
	eJPG,
	eBMP,
	eDNG,
	eGIF
};

/// \brief 图像对象
class GS_API GsImage:public GsRefObject
{
protected:
	GsRGBAType m_eRGBAType;
	GsImage();
public:
	/// \brief 析构函数
	virtual ~GsImage();

	
	/// \brief 拷贝
	virtual bool CopyFrom(GsImage* pImage);

	/// \brief 拷贝外部数据到自身
	virtual bool FillImageData(const unsigned char* buff, int nLen, GsRGBAType eType);

	/// \brief 拷贝自身的数据到外部影像
	virtual int CopyImageData(unsigned char* buff, int nLen, GsRGBAType eType);

	/// \brief 将自身数据剪裁到目标图像中
	/// \param target 要存储数据的图像
	/// \param extent 要剪裁的数据范围
	/// \param offset 存储到目标数据的偏移位置
	virtual bool CropTo(GsImage* target, const GsRect& extent, const GsPT& offset);


	/// \brief 将自身数据剪裁到目标内存块中
	/// \param target 要存储数据的目标内存
	/// \param nStride 目标数据行字节长度
	/// \param extent 要剪裁的数据范围
	virtual bool CropTo(unsigned char* target, int nStride, const GsRect& extent);

	/// \brief 将自身数据剪裁到目标内存块中
	/// \param target 要存储数据的目标内存
	/// \param nStride 目标数据行字节长度
	/// \param extent 要剪裁的数据范围
	virtual bool CropTo(GsByteBuffer* target, int nStride, const GsRect& extent);

	/// \brief 获取图片颜色格式
	virtual GsRGBAType RGBAType();



	/// \brief 获得宽度
	virtual unsigned int Width() = 0;
	/// \brief 获得高度
	virtual unsigned int Height() = 0;
	
	/// \brief 获取图像的字节数组，并非所有图像都能够成功
	virtual const unsigned char* Bit() = 0;

	/// \brief 图像一行的字节长度
	virtual unsigned int Stride();

	/// \brief 获取某行的首指针
	virtual const unsigned char* Row(int r);

	/// \brief 保存图像为PNG格式文件
	virtual bool SavePNG(const char* strFile);
	
	/// \brief 保存图像为PNG格式的内存块
	virtual bool SavePNG(GsByteBuffer* pBuffer);

	/// \brief 保存图像为JPEG格式文件
	virtual bool SaveJPEG(const char* strFile,int nQuality=75);
	
	/// \brief 保存图像为JPEG格式的内存块
	virtual bool SaveJPEG(GsByteBuffer* pBuffer,int nQuality=75);
	 

	/// \brief 从文件载入生成图像
	static GsSmarterPtr<GsImage> LoadFrom(const char* strFile);

	/// \brief 从内存块载入生成图像对象
	static GsSmarterPtr<GsImage> LoadFrom(const unsigned char *pBlob,int nLen);

	/// \brief 判断一个影像编码内存块的编码类型
	static GsImageEncodeType EncodeType(const unsigned char* buff, int nLen);

	/// \brief 判断一个影像文件的编码类型
	static GsImageEncodeType EncodeType(const char* file);
};
/// \brief GsImagePtr
GS_SMARTER_PTR(GsImage);


/// \brief 影像格式解码器
class GS_API GsImageDecoder:public GsRefObject
{
protected:
	GsImageDecoder();
public:
	virtual ~GsImageDecoder();

	/// \brief 影像解码像素大小
	virtual GsSize Size() = 0;

	/// \brief 开始解码
	virtual bool BeginEncode() = 0;

	/// \brief 解码图像返回图像对象
	virtual GsImagePtr Encode();

	/// \brief 解码图像，将数据如写入传入的空图像对象。
	/// \details 传入的空图像大小需要和解码器一致。
	virtual bool Encode(GsImage* img) = 0;
};
/// \brief GsImageDecoderPtr
GS_SMARTER_PTR(GsImageDecoder);


/// \brief GIF格式解码
class GS_API GsGIFImageDecoder:public GsImageDecoder
{
private:
	int m_nDuration;
	GsSize m_Size;
	int m_nFrameCount;
	GsGrowByteBuffer m_srcBuffer;
	GsGrowByteBuffer m_dscBuffer;
	static int OnFrame(void* data, void* whdr);
	void Frame(void* whdr);
public:
	GsGIFImageDecoder(const char* file);
	GsGIFImageDecoder(const unsigned char *blob ,int nlen);
	/// \brief 影像解码像素大小
	virtual GsSize Size();

	/// \brief 开始解码
	virtual bool BeginEncode() ;

	/// \brief 解码图像，将数据如写入传入的空图像对象。
	/// \details 传入的空图像大小需要和解码器一致。
	virtual bool Encode(GsImage* img);

	/// \brief 图像的帧数量
	virtual int FrameCount();

	/// \brief 开始读取一帧图像
	virtual bool BeginFrame(int nFrameIndex);

	/// \brief 获取GIF播放时，帧间时间间隔
	virtual int Duration();
};

/// \brief GsGIFImageDecoderPtr
GS_SMARTER_PTR(GsGIFImageDecoder);

/// \brief 影像格式编码器
class GS_API GsImageEncoder :public GsRefObject
{
protected:
	GsImageEncoder();
public:
	virtual ~GsImageEncoder();
	/// \brief 开始编码
	virtual bool BeginEncode() = 0;

	/// \brief 写入影像或者一帧
	virtual bool Write(GsImage* img) = 0;

	/// \brief 结束编码
	virtual bool FinishEncode() = 0;
};
/// \brief GsImageEncoderPtr
GS_SMARTER_PTR(GsImageEncoder);

/// \brief GIF图像编码
class GS_API GsGIFImageEncoder :public GsImageEncoder
{
	void* m_ptrEncoder;
	GsSize m_Size;
	int m_nDuration;
	GsString m_strPath;
public:
	GsGIFImageEncoder(GsByteBuffer* buff);
	GsGIFImageEncoder(const char* file, GsSize& size, int nDuration);

	/// \brief 开始编码
	virtual bool BeginEncode();

	/// \brief 开始一帧
	virtual bool BeginFrame();

	/// \brief 写入影像或者一帧
	virtual bool Write(GsImage* img);

	/// \brief 结束编码
	virtual bool FinishEncode();
};
/// \brief GsGIFImageEncoder
GS_SMARTER_PTR(GsGIFImageEncoder);

/// \brief 针对渲染设备缓存后的Image
class GS_API GsCachedImage:public GsImage
{
protected:
	GsImagePtr m_ptrPrimitiveImage;
	GsStlMap<int, GsImagePtr> m_Cache;
public:
	GsCachedImage(GsImage* primitiveImage);
	~GsCachedImage();

	/// \brief 获取缓存的图像
	GsImage* CachedImage(int nCache);

	/// \brief 设置缓存的图像
	void CachedImage(int nCache, GsImage* img);


	/// \brief 获取图片颜色格式
	virtual GsRGBAType RGBAType();



	/// \brief 获得宽度
	virtual unsigned int Width() ;
	/// \brief 获得高度
	virtual unsigned int Height();

	/// \brief 获取图像的字节数组，并非所有图像都能够成功
	virtual const unsigned char* Bit();
};
/// \brief GsCachedImagePtr
GS_SMARTER_PTR(GsCachedImage);

/// \brief 图像处理类
class GS_API GsImageProcess
{ 
public:
	virtual ~GsImageProcess(){}
	/// \brief 返回位图影像的某行某列点像素地址
	virtual int Process(int r, int c, unsigned  int* pixel) = 0;
};


/// \brief 内存中存储的简单位图对象
class GS_API GsSimpleBitmap:public GsImage
{
protected:
	GsGrowByteBuffer m_Buffer;
	int m_nWidth;
	int m_nHeight;
	int m_nBpp;
public :
	/// \brief 重载构造函数
	GsSimpleBitmap(unsigned int nWidth,unsigned int nHeight,int nBpp = 32); 
	
	/// \brief 解码文件格式的图像
	GsSimpleBitmap(const char* strFile); 
	/// \brief 解码内存中的图像
	GsSimpleBitmap(const unsigned char* blob,int nLen);  
	/// \brief 析构函数
	virtual ~GsSimpleBitmap();
	/// \brief 拷贝
	virtual bool CopyFrom( GsImage* pImage );
	/// \brief 获得宽度
	virtual unsigned int Width();
	/// \brief 获得高度
	virtual unsigned int Height() ;
	/// \brief 获取图像的字节数组，并非所有图像都能够成功
	virtual const unsigned char* Bit() ;
	
	/// \brief 图像一行的字节长度
	virtual unsigned int Stride();
	
	/// \brief 遍历影像中各个像素
	virtual void ForEachPixel(GsImageProcess* pImageProcess, bool bParallel = false);
};
/// \brief GsSimpleBitmapPtr
GS_SMARTER_PTR(GsSimpleBitmap);


/// \brief 基于NetPbm的PAM格式文件影像
/// \details http://netpbm.sourceforge.net/
class GS_API GsPAMBitmap:public GsImage
{
	int m_nWidth;
	int m_nHeight;
	int m_nBpp;
	GsVector<unsigned char> m_RowData;
	std::fstream m_fStream;
	mutable std::fstream::pos_type m_nPixelBegin; 
public:
	GsPAMBitmap(const char* strFile);
	GsPAMBitmap(const char* strFile,int w,int h);

	/// \brief 获得宽度
	virtual unsigned int Width();
	/// \brief 获得高度
	virtual unsigned int Height() ;
	/// \brief 获取图像的字节数组，并非所有图像都能够成功
	virtual const unsigned char* Bit() ;
	
	/// \brief 图像一行的字节长度
	virtual unsigned int Stride();

	/// \brief 获取某行的首指针
	virtual const unsigned char* Row(int r);

	/// \brief 获取某行的首指针
	virtual const unsigned char* Row(int r,GsVector<unsigned char> *vec);

	/// \brief 写入某行的数据
	virtual void Row(int r,const unsigned char* pRowData,int nLen);
};
GS_SMARTER_PTR(GsPAMBitmap);



/// \brief 解析SVG为位图
class GS_API GsSVGImage:public Utility::GsImage
{
	float m_DPI;
	float m_Scale;
	void* m_pSVG;
	Utility::GsString m_SVG;
	Utility::GsImagePtr m_ptrImage;
	void CreateImage();
	void DestroyHandle();
	void CreateHandle();
public:
	/// \brief 根据SVG文件或者字符串构造位图
	GsSVGImage(const char* svg,bool bFileName);
	GsSVGImage();
	
	~GsSVGImage();
	
	/// \brief 获得宽度
	virtual unsigned int Width();
	/// \brief 获得高度
	virtual unsigned int Height();
	
	/// \brief 获取图像的字节数组，并非所有图像都能够成功
	virtual const unsigned char* Bit();

	/// \brief 获取SVG的xml字符串
	GsString SVG();

	/// \brief 设置SVG字符串或者SVG文件
	void SVG(const char* svg,bool bFileName);

	/// \brief 是否是有效的SVG
	bool IsValid();

	/// \brief SVG图像的宽和高
	GsSize SVGSize();

	/// \brief 获取SVG绘制时的缩放比率
	float SVGScale();

	/// \brief 设置SVG绘制时的缩放比率
	void SVGScale(float scale);

	/// \brief 获取图像显示分辨率
	float DPI();
	
	/// \brief 设置图像显示分辨率
	void DPI(float dpi);
	
};
/// \brief GsSVGImagePtr
GS_SMARTER_PTR(GsSVGImage);
UTILITY_ENDNS