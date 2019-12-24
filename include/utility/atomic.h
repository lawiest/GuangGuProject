#pragma once 
#include "preconfig.h" 
UTILITY_NS 

 
/// \brief 数值原子化操作
class GS_API GsInterlocked
{
public: 
	/// \brief 修改1字节整数值
	/// \return 返回整数修改前的值
	static char Read(const volatile char *target);
	/// \brief 修改2字节整数值
	/// \return 返回整数修改前的值
	static short Read(const volatile short *target);
	/// \brief 修改4字节整数值
	/// \return 返回整数修改前的值
	static int Read(const volatile int *target);
	/// \brief 修改8字节整数值
	/// \return 返回整数修改前的值
	static long long Read(const volatile long long *target);

	/// \brief 修改1字节整数值
	/// \return 返回整数修改前的值
	static char Exchange(volatile char *target, char value);
	/// \brief 修改2字节整数值
	/// \return 返回整数修改前的值
	static short Exchange(volatile short *target, short value);
	/// \brief 修改4字节整数值
	/// \return 返回整数修改前的值
	static int Exchange(volatile int *target, int value);
	/// \brief 修改8字节整数值
	/// \return 返回整数修改前的值
	static long long Exchange(volatile long long *target, long long value);

	/// \brief 递增1字节整数值
	/// \return 返回整数修改后的值
	static char Increment(volatile char *target);
	/// \brief 递增2字节整数值
	/// \return 返回整数修改后的值
	static short Increment(volatile short *target);
	/// \brief 递增4字节整数值
	/// \return 返回整数修改后的值
	static int Increment(volatile int *target);
	/// \brief 递增8字节整数值
	/// \return 返回整数修改后的值
	static long long Increment(volatile long long *target);


	/// \brief 递减1字节整数值
	/// \return 返回整数修改后的值
	static char Decrement(volatile char *target);
	/// \brief 递减2字节整数值
	/// \return 返回整数修改后的值
	static short Decrement(volatile short *target);
	/// \brief 递减4字节整数值
	/// \return 返回整数修改后的值
	static int Decrement(volatile int *target);
	/// \brief 递减8字节整数值
	/// \return 返回整数修改后的值
	static long long Decrement(volatile long long *target);


	/// \brief 增加1字节整数值
	/// \return 返回整数修改后的值
	static char Add(volatile char *target,char value);
	/// \brief 增加2字节整数值
	/// \return 返回整数修改后的值
	static short Add(volatile short *target, short value);
	/// \brief 增加4字节整数值
	/// \return 返回整数修改后的值
	static int Add(volatile int *target, int value);
	/// \brief 增加8字节整数值
	/// \return 返回整数修改后的值
	static long long Add(volatile long long *target, long long value);

	/// \brief 1字节整数Or运算
	/// \return 返回整数修改后的值
	static char Or(volatile char *target, char value);
	/// \brief 2字节整数值Or运算
	/// \return 返回整数修改后的值
	static short Or(volatile short *target, short value);
	/// \brief 4字节整数值Or运算
	/// \return 返回整数修改后的值
	static int Or(volatile int *target, int value);
	/// \brief 8字节整数值Or运算
	/// \return 返回整数修改后的值
	static long long Or(volatile long long *target, long long value);

	/// \brief 1字节整数Xor运算
	/// \return 返回整数修改后的值
	static char Xor(volatile char *target, char value);
	/// \brief 2字节整数值Xor运算
	/// \return 返回整数修改后的值
	static short Xor(volatile short *target, short value);
	/// \brief 4字节整数值Xor运算
	/// \return 返回整数修改后的值
	static int Xor(volatile int *target, int value);
	/// \brief 8字节整数值Xor运算
	/// \return 返回整数修改后的值
	static long long Xor(volatile long long *target, long long value);

	/// \brief 1字节整数And运算
	/// \return 返回整数修改后的值
	static char And(volatile char *target, char value);
	/// \brief 2字节整数值And运算
	/// \return 返回整数修改后的值
	static short And(volatile short *target, short value);
	/// \brief 4字节整数值And运算
	/// \return 返回整数修改后的值
	static int And(volatile int *target, int value);
	/// \brief 8字节整数值And运算
	/// \return 返回整数修改后的值
	static long long And(volatile long long *target, long long value);

};
/// \brief 原子操作的整数
template<class T>
class  GsAtomic
{
	volatile T m_Value;
public:
	/// \brief 构造
	GsAtomic(const T& val):m_Value(val)
	{
		
	}
	/// \brief 构造
	GsAtomic():m_Value(0)
	{

	}
	~GsAtomic()
	{

	}
	/// \brief 增加整数
	T Add(const T& val)
	{
		return GsInterlocked::Add(&m_Value,val);
	}
	/// \brief 获取数值
	T Read()  const
	{ 
		return GsInterlocked::Read(&m_Value); 
	}
	/// \brief 修改数值
	void Write(const T& val)
	{
		GsInterlocked::Exchange(&m_Value,val);
	}
	/// \brief 增加1
	T Increment()
	{
		return GsInterlocked::Increment(&m_Value);
	}
	/// \brief 减少1
	T Decrement()
	{
		return GsInterlocked::Decrement(&m_Value);
	}
	
	/// \brief 等号操作符
	T operator =(const T& value)
	{
		return GsInterlocked::Exchange(&m_Value,value);
	}
	/// \brief ++操作符
	T operator ++(int)
	{
		T val = GsInterlocked::Exchange(&m_Value, m_Value);
		GsInterlocked::Increment(&m_Value);
		return val;
	}
	
	/// \brief ++操作符
	T operator ++()
	{
		return GsInterlocked::Increment(&m_Value);
	}
	/// \brief --操作符
	T operator --(int)
	{
		T val = GsInterlocked::Exchange(&m_Value, m_Value);
		GsInterlocked::Decrement(&m_Value);
		return val;
	}
	/// \brief --操作符
	T operator --()
	{
		return GsInterlocked::Decrement(&m_Value);
	}

	/// \brief 整数重载操作符
	operator T()   const
	{
		return GsInterlocked::Read(&m_Value);
	}
};
 

UTILITY_ENDNS