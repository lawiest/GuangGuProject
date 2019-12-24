#pragma once
#include "preconfig.h" 
#include "vectorhelp.h"
UTILITY_NS 

/// \brief 一个二维码对象
class GS_API GsQRCodePage
{
	void* m_QRCode;
public:
	GsQRCodePage(void* code = NULL);
	~GsQRCodePage();
	GsQRCodePage(const GsQRCodePage& rhs);

	GsQRCodePage& operator =(const GsQRCodePage& rhs);
	
	/// \brief 二维码的大小
	int Size();
	/// \brief 根据行列获取是否存在色块。
	/*
	  ■■■■■■■            ■    ■    ■■  ■■■■■■■      
      ■          ■  ■■■      ■■■  ■■    ■          ■      
      ■  ■■■  ■          ■■    ■■■      ■  ■■■  ■      
      ■  ■■■  ■      ■    ■■    ■        ■  ■■■  ■      
      ■  ■■■  ■          ■        ■  ■    ■  ■■■  ■      
      ■          ■  ■■  ■■■    ■■    ■  ■          ■      
      ■■■■■■■  ■  ■  ■  ■  ■  ■  ■  ■■■■■■■      
                      ■■        ■■■    ■■                      
              ■■■■  ■■■■  ■■        ■  ■■      ■        
        ■    ■    ■    ■■■■      ■■■      ■        ■      
      ■    ■    ■■    ■  ■■■  ■■■■  ■■■■  ■  ■      
        ■■■■■        ■  ■  ■  ■■  ■■  ■  ■■■          
          ■      ■          ■  ■■    ■    ■      ■■■        
          ■■■          ■■      ■■  ■■■■■■■  ■■        
      ■■■■    ■    ■■  ■■  ■■      ■■■■  ■  ■■      
      ■■■■  ■    ■■■■  ■        ■      ■    ■    ■      
      ■■  ■  ■■    ■        ■■    ■■      ■■■            
      ■■  ■  ■  ■  ■■■■      ■■  ■■■    ■■■          
            ■  ■■■■■  ■■■■■  ■■  ■■■    ■    ■      
              ■■  ■    ■■■■    ■        ■  ■                
      ■■    ■  ■■  ■■■      ■■■■■■■■■■  ■■        
                      ■■■  ■■■    ■  ■■      ■              
      ■■■■■■■  ■■■■■      ■      ■  ■  ■■■          
      ■          ■  ■■    ■■        ■■■      ■■  ■        
      ■  ■■■  ■  ■■    ■  ■■■      ■■■■■  ■■■      
      ■  ■■■  ■        ■      ■■  ■■      ■                
      ■  ■■■  ■    ■■■  ■  ■■  ■■■■■      ■■■      
      ■          ■      ■      ■  ■  ■          ■■■■■      
      ■■■■■■■        ■■  ■■  ■        ■  ■■■■    
	*/
	bool IsSet(int r,int c);

};


/// \brief 二维码生成的级别。
enum GsQREncodeLevel {
	eQR_ECLEVEL_L = 0, ///< lowest
	eQR_ECLEVEL_M,
	eQR_ECLEVEL_Q,
	eQR_ECLEVEL_H      ///< highest
} ;

/// \brief 二维码编码模式
enum GsQREncodeMode
{
	eQR_MODE_NUL = -1,  ///< Terminator (NUL character). Internal use only
	eQR_MODE_NUM = 0,   ///< Numeric mode
	eQR_MODE_AN,        ///< Alphabet-numeric mode
	eQR_MODE_8,         ///< 8-bit data mode
	eQR_MODE_KANJI,     ///< Kanji (shift-jis) mode
	eQR_MODE_STRUCTURE, ///< Internal use only
	eQR_MODE_ECI,       ///< ECI mode
	eQR_MODE_FNC1FIRST,  ///< FNC1, first position
	eQR_MODE_FNC1SECOND, ///< FNC1, second position
} ;

/// \brief 二维码编码数据类型
enum GsQRCodeDataType
{
	/// \brief 未知类型，错误类型
	eQRCodeUnknown,
	
	/// \brief 字符串二维编码
	eQRCodeString,
	/// \brief 任意二进制数据
	eQRCodeBinary,
};

/// \brief 二维码生成类。
class GS_API GsQRCode
{
	void* m_QRCodeList;
	GsVector<GsQRCodePage> m_Pages;
	GsQREncodeLevel m_eLevel;
	GsQREncodeMode m_eMode;
public:
	GsQRCode();
	GsQRCode(const char* strData);
	GsQRCode(const unsigned char* pData,int nLen);
	virtual ~GsQRCode();

	/// \brief 二维码的编码级别
	/// \details 级别越高，容错性越好
	GsQREncodeLevel Level();
	/// \brief 二维码的编码级别
	/// \details 级别越高，容错性越好
	void Level(GsQREncodeLevel eLevel);

	/// \brief 二维码的编码模式
	GsQREncodeMode Mode();
	/// \brief 二维码的编码模式
	void Mode(GsQREncodeMode eMode);

	/// \brief 任意字符串进行编码
	bool Encode(const char* strData);
	/// \brief 任意内存块进行编码
	bool Encode(const unsigned char* pData,int nLen);

	/// \brief 生成的二维码数量
	int PageCount();

	/// \brief 获取一个二维码数据
	/// \details 返回对象的生命周期应保持小于GsQRCode对象自身
	GsQRCodePage  QRCodePage(int i );
	
	/// \brief 重置
	void Reset();
};
UTILITY_ENDNS