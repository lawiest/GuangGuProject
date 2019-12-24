#pragma once
#include "memorymanager.h"
UTILITY_NS

/// \brief 加密对象
class GS_API GsEncrypt
{
protected:
	GsEncrypt();
public:
	virtual ~GsEncrypt();
	
	/// \brief 将数据加密输出到ByteBuffer中
	virtual int Encrypt(const unsigned char* pOriData,int nLen,GsByteBuffer* pOutput) = 0;
	/// \brief 将数据解密输出到ByteBuffer中
	virtual int Decrypt(const unsigned char* pEnData,int nLen,GsByteBuffer* pOutput) = 0;


	/// \brief 将数据加密输出到ByteBuffer中.返回Base64编码的字符串
	virtual GsString Encrypt(const unsigned char* pOriData,int nLen);
	/// \brief 将加密后的以base64字符串表示的数据解密出来
	virtual int Decrypt(const char* strEncryptBase64,GsByteBuffer* pOutput);

};

/// \brief 基于DES算法的加密对象
class GS_API GsDESEncrypt:public GsEncrypt
{
	GsString m_Password;
public:
	GsDESEncrypt(const char* password);
	~GsDESEncrypt();
	using GsEncrypt::Decrypt;
	using GsEncrypt::Encrypt;

	/// \brief 将数据加密输出到ByteBuffer中
	virtual int Encrypt(const unsigned char* pOriData,int nLen,GsByteBuffer* pOutput);
	/// \brief 将数据解密输出到ByteBuffer中
	virtual int Decrypt(const unsigned char* pEnData,int nLen,GsByteBuffer* pOutput);
};

UTILITY_ENDNS