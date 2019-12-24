#pragma once
#include "preconfig.h" 
 
UTILITY_NS 

/// \brief 字节序枚举
enum GsEndian
{
	eBigEndian,
	eLittleEndian, 
};

/// \brief 字节序转换类
class GS_API GsEndianConverter
{
public:
	/// \brief 静态函数
	static GsEndian OsEndian ();
	
	/// \brief 是否是高字节序
	static bool IsBigEndian ();

	/// \brief 是否是低字节序
	static bool IsLittleEndian ();

	/// \brief 变量字节转换模板
	template<typename T>
	static void SwapRefT(T& value)
	{
		if(sizeof(T)==2)
		{
			/// \brief 转换16位
			SwapRef((unsigned short&)value);
			return;
		}
		
		if(sizeof(T)==4)
		{
			/// \brief 转换32位
			SwapRef((unsigned int&)value);
			return;
		}

		if(sizeof(T)==8)
		{
			/// \brief 转换64位
			SwapRef((unsigned long long&)value);
			return;
		}
 
	}

	/// \brief 数组字节转换模板
	template<typename T>
	static void SwapRefT(T* value,int nCount=1)
	{
		if(sizeof(T)==2)
		{
			/// \brief 转换16位
			for(int i=0;i<nCount;i++)
				SwapRef((unsigned short&)value[i]);
			return;
		}
		
		if(sizeof(T)==4)
		{
			/// \brief 转换32位
			for(int i=0;i<nCount;i++)
				SwapRef((unsigned int&)value[i]);
			return;
		}

		if(sizeof(T)==8)
		{
			/// \brief 转换64位
			for(int i=0;i<nCount;i++)
				SwapRef((unsigned long long&)value[i]);
			return;
		}
 
	}

/// \brief 类型转换模板
 template<typename T>
	static T SwapT(T value)
	{
		/// \brief 把变量转化为16位
		if(sizeof(T)==2)
			return static_cast<T>(Swap(static_cast<unsigned short>(value)));
		
		/// \brief 把变量转化为32位
		if(sizeof(T)==4)
			return static_cast<T>(Swap(static_cast<unsigned int>(value)));

		/// \brief 把变量转化为64位
		if(sizeof(T)==8)
			return static_cast<T>(Swap(static_cast<unsigned long long>(value)));

		return value;
	}

	static double Swap(double value) ;

	static float Swap(float value) ;

	static unsigned short Swap(unsigned short value) ;
	//转换单个32位
	static unsigned int Swap(unsigned int value) ;
	//转换单个64位
	static unsigned long long Swap(unsigned long long val);
   	//转换单个16位
	static void SwapRef(unsigned short& value);
	 
	//转换单个32位
	static void SwapRef(unsigned int& value);
	//转换单个64位
	static  void SwapRef(unsigned long long& val) ;

};


UTILITY_ENDNS