#pragma once
#include "zipfile.h"

UTILITY_NS
	

	/// \brief PNG数据的颜色类型
	enum GsPNGColorType
	{
		eUnknownPNGColorType,
		eGrayScale1,
		eGrayScale2,
		eGrayScale4,
		eGrayScale8,
		eGrayScale16,
		eRGB8,
		eBGR8,
		eRGB16,
		eBGR16,
		ePalette1,
		ePalette2,
		ePalette4,
		ePalette8,
		eAlphaGrayScale8,
		eAlphaGrayScale16,
		eARGB8,
		eRGBA8,
		eABGR8,
		eBGRA8,
		eARGB16,
		eRGBA16,
		eABGR16,
		eBGRA16,
	};

	/// \brief PNG调色板
	struct GS_API  GsPNGPalette
	{
		GsPNGPalette();
		bool		WithAlpha;
		unsigned int Size;
		unsigned int RGBAColor[256];
	};

	/// \brief 轻量PNG读写
	/// \details https://www.w3.org/TR/PNG
	class GS_API GsPNG:GsZlibStream
	{
	public:
		/// \brief PNG读写过程中的错误
		enum ErrorCode
		{
			eNoError,
			eGeneralError,
		};
		/*
		   -------------
		   |C | B | D |
		   ------------
		   |A | x |   |
		   ------------

		Type	Name	Filter Function	Reconstruction Function
		0	None	Filt(x) = Orig(x)	Recon(x) = Filt(x)
		1	Sub	Filt(x) = Orig(x) - Orig(a)	Recon(x) = Filt(x) + Recon(a)
		2	Up	Filt(x) = Orig(x) - Orig(b)	Recon(x) = Filt(x) + Recon(b)
		3	Average	Filt(x) = Orig(x) - floor((Orig(a) + Orig(b)) / 2)	Recon(x) = Filt(x) + floor((Recon(a) + Recon(b)) / 2)
		4	Paeth	Filt(x) = Orig(x) - PaethPredictor(Orig(a), Orig(b), Orig(c))	Recon(x) = Filt(x) + PaethPredictor(Recon(a), Recon(b), Recon(c))

		x	the byte being filtered;
		a	the byte corresponding to x in the pixel immediately before the pixel containing x(or the byte immediately before x, when the bit depth is less than 8);
		b	the byte corresponding to x in the previous scanline;
		c	the byte corresponding to b in the pixel immediately before the pixel containing b(or the byte immediately before b, when the bit depth is less than 8).
		*/
		enum FilterTypes
		{
				eFTNone =0,
				eFTSub = 1,
				eFTUp =2,
				eFTAverage = 3,
				eFTPaeth =4,
		};
	private:
		ErrorCode m_Error;
		unsigned int m_CRC;
		GsString m_ErrorMessage;
		int m_Row;
		int m_ReadRGBA8RowCount;
		int m_W, m_H, m_Depth;
		GsGrowByteBuffer m_Cache, m_Cache1, m_CacheBefore, m_Cachel7;
		GsPNGPalette m_Palette;
		GsPNGColorType m_Type;
		int m_ScanCount;
		
		GsVector<unsigned int>  m_vPassW;
		GsVector<unsigned int>  m_vPassH;
		//GsVector<size_t>		m_vFilterPassStart;
		//GsVector<size_t>		m_vPaddedPassStart;
		//GsVector<size_t>		m_vPassStart;

		/*
		IHDR	No	Shall be first
		PLTE	No	Before first IDAT
		IDAT	Yes	Multiple IDAT chunks shall be consecutive
		IEND	No	Shall be last
		cHRM	No	Before PLTE and IDAT
		gAMA	No	Before PLTE and IDAT
		iCCP	No	Before PLTE and IDAT.If the iCCP chunk is present, the sRGB chunk should not be present.
		sBIT	No	Before PLTE and IDAT
		sRGB	No	Before PLTE and IDAT.If the sRGB chunk is present, the iCCP chunk should not be present.
		bKGD	No	After PLTE; before IDAT
		hIST	No	After PLTE; before IDAT
		tRNS	No	After PLTE; before IDAT
		pHYs	No	Before IDAT
		sPLT	Yes	Before IDAT
		tIME	No	None
		iTXt	Yes	None
		tEXt	Yes	None
		zTXt	Yes	None
		*/
		enum ChunkType
		{
			eErrorChunk,
			eIHDR,
			ePLTE,
			eIDAT,
			eIEND,
			
			ecHRM,
			egAMA,
			eiCCP,
			esBIT,
			esRGB,
			ebKGD,
			ehIST,
			etRNS,
			epHYs,
			epPLT,
			etIME,
			eiTXt,
			etEXt,
			ezTXt
		};
		ChunkType ReadChunkHead(unsigned int& crc, unsigned int& length);
		const unsigned char* ReadChunkData(unsigned int crc, unsigned int length, bool &crcOK);

		virtual void Put8(unsigned char a);
		void Put32(unsigned int v);
		void BeginChunk(const char* id, int len);
		void EndChunk(); 
		
		const unsigned char *Read(int& nSize);

		void Error(ErrorCode code);
		void Error(const char* msg);
		void Succeed();

		template<class T>
		T Error(ErrorCode  nCode, T result)
		{
			Error(nCode);
			return result;
		}

		template<class T>
		T Error(const char* msg, T result)
		{
			Error(msg);
			return result;
		}
		template<class T>
		T Succeed(T result)
		{
			Succeed();
			return result;
		}
		
		virtual const unsigned char* OnNextInputBuffer(unsigned int& size);
		virtual void OnWrite(const unsigned char* data, int nLen);
		virtual void OnChunkData(const unsigned char* data, int nSize);

		bool DirectCopyType();
		void CopyRow(const unsigned char* row, unsigned char* cache, int nStride);
		//每行使用不同的过滤算法压缩有再压缩的，这里先实现zip解压后的像素，反过滤，还原像素
		void Filtering(FilterTypes etype, unsigned char* row, int length);
	protected:

		/// \brief 覆盖此函数以实现解码或编码PNG时读写数据
		virtual int OnPNGData(unsigned char* data, int nLen,bool bWrite=true) = 0;
	public:
		/// \brief 缺省构造，解码PNG
		GsPNG();
		/// \brief 缺省构造，编码PNG
		GsPNG(int w, int h, GsPNGColorType type = eRGBA8);
		virtual ~GsPNG() {};

		/// \brief 最近错误编码
		ErrorCode LastErrorCode();
		/// \brief 错误描述
		const char* LastErrorMessage();
		/// \brief 编码或者解码多少行
		int Row();
		/// \brief 根据颜色类型和宽度计算一行的字节长度
		static int Stride(GsPNGColorType type, int width);
		/// \brief 判断8个字节长度的文件头是否是PNG数据标签
		static bool IsPNGSignature(const unsigned char* head);

		/// \brief 高度
		int Height();
		/// \brief 宽度
		int Width();
		/// \brief 行字节长度
		int Stride();
		/// \brief 颜色类型
		GsPNGColorType  ColorType();
		/// \brief 调色板
		GsPNGPalette& Palette();
		/// \brief 开始编码
		/// \details   BeginSave();
		/// \details   for(...){
		/// \details      SaveRow(..);}
		/// \details   FinishSave();
		bool BeginSave();

		/// \brief 编码一行
		void SaveRow(const unsigned char* row);
		/// \brief 结束编码
		void FinishSave();

		/// \brief 开始解码
		/// \details   BeginRead();
		/// \details   while(ReadRow(..){...}
		/// \details   FinishRead();
		bool BeginRead();

		/// \brief 解码一行
		bool ReadRow(unsigned char* row);
		
		/// \brief 解码一行,以RGBA8的格式输出
		bool ReadRGBA8Row(unsigned char* row);

		/// \brief 结束解码
		void FinishRead();

		/// \brief 获取扫描次数
		int ScanCount();
	};

	/// \brief 从内存解码或者编码PNG
	class GS_API  GsMemoryPNG :public GsPNG
	{
		const unsigned char* m_buff;
		int m_Offset,m_nSize;
		GsByteBuffer* m_pBuffer;
		GsGrowByteBuffer  m_Buffer;
	protected:
		/// \brief 读写数据
		virtual int OnPNGData(unsigned char* data, int nLen, bool bWrite = true);
	public:
		/// \brief 从内存块解码
		GsMemoryPNG(const unsigned char* buff,int nSize);
		/// \brief 从内存块解码
		GsMemoryPNG(GsByteBuffer* buff);
		
		/// \brief 编码到内存
		GsMemoryPNG(int w, int h, GsPNGColorType type = eRGBA8);

		/// \brief 从内存块编码
		GsMemoryPNG(GsByteBuffer* buff,int w,int h, GsPNGColorType type = eRGBA8);
		
		~GsMemoryPNG();

		/// \brief 获取编码用的内存
		GsByteBuffer* MemoryBuffer();
	};

	/// \brief 从文件解码或编码PNG
	class GS_API GsFilePNG :public GsPNG
	{
		FILE* m_F;
	protected:

		/// \brief 从文件读写
		virtual int OnPNGData(unsigned char* data, int nLen, bool bWrite = true);
		 
	public:
		/// \brief 从文件解码
		GsFilePNG(const char* file);
		/// \brief 编码到文件
		GsFilePNG(const char* file, int w, int h, GsPNGColorType type = eRGBA8);
		~GsFilePNG();
	};

UTILITY_ENDNS