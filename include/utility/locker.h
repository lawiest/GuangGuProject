#pragma once
#include "preconfig.h" 
#include "listhelp.h" 
#include "vectorhelp.h" 
#include "atomic.h"
#include "object.h"
UTILITY_NS 
/// \brief 线程锁
class GS_API  GsLock
{
protected:
	void* m_pMutex;
	GsLock(int );

public:
	GsLock();
	virtual ~GsLock();
	
	/// \brief 锁定对象
	virtual void Lock(); 

	/// \brief 释放锁定
	virtual void Unlock();
};
/// \brief GsLockPtr
GS_SHARED_PTR(GsLock);

/// \brief 可递归线程锁，同一线程可以多次调用不会阻塞
class GS_API GsRecursiveLock:public GsLock
{ 
public:
	GsRecursiveLock();
	virtual ~GsRecursiveLock();
	/// \brief 锁定对象
	virtual void Lock();

	/// \brief 释放锁定
	virtual void Unlock();
};
/// \brief GsRecursiveLockPtr
GS_SHARED_PTR(GsRecursiveLock);

/// \brief 信号
class GS_API GsSignaler
{ 
	void* m_pSingaler;
public:
	GsSignaler();
	virtual ~GsSignaler(); 

	/// \brief 设置信号
	virtual void Signal();
	 
	/// \brief 等待信号
	virtual void Wait();

	/// \brief 等待信号或者超时
	///\return 返回false则表示超时
	virtual bool Wait(unsigned int milliseconds);
};
/// \brief GsSignalerPtr
GS_SHARED_PTR(GsSignaler);


/// \brief 读写线程锁
class GS_API  GsRWLock
{
	void* m_Handle;
public:
	GsRWLock();
	virtual ~GsRWLock();

	/// \brief 写锁定对象
	virtual void Lock();
	
	/// \brief 释放写锁定
	virtual void Unlock();

	/// \brief 读锁定对象
	virtual void ReadLock(); 

	/// \brief 释放读锁定
	virtual void ReadUnlock();
}; 
/// \brief GsRWLockPtr
GS_SHARED_PTR(GsRWLock);


/// \brief 自动加锁的对象
class GS_API GsAutoLocker
{
	GsRWLock* m_RW;
	GsLock* m_L;
	bool m_bRead;
public:
	/// \brief 缺省构造函数
	GsAutoLocker(GsLock* L);
	/// \brief 构造函数重载
	GsAutoLocker(GsRWLock* l,bool bReadLock = false);
	/// \brief 构造函数重载
	GsAutoLocker(GsLock& L);
	/// \brief 构造函数重载
	GsAutoLocker(GsRWLock& l,bool bReadLock = false);

	virtual ~GsAutoLocker();

	/// \brief 释放锁定
	void Unlock();

};


/// \brief 锁定对象
#define GS_LOCK_IT(P)  UTILITY_NAME::GsAutoLocker L_(P);

/// \brief 读锁定对象
#define GS_RLOCK_IT(P)  UTILITY_NAME::GsAutoLocker L_(P,true);

/// \brief 锁定自身
#define GS_LOCK_THIS  UTILITY_NAME::GsAutoLocker L_(this);

/// \brief 读锁定自身
#define GS_RLOCK_THIS  UTILITY_NAME::GsAutoLocker L_(this,true);

/// \brief 安全对象，组合锁对象形成安全的对象
template<class T,class L=GsRWLock>
class GsSafeObject:public T,public L
{
public:
	using   T::operator=;
};
/// \brief 安全类型，组合锁对象形成安全的类型，适用于轻量的对象
template<class T,class L=GsRWLock>
class GsSafeType:public L
{
public:
	GsSafeType(){}
	GsSafeType(T v):Value(v) {
		
	}
	T Value;
};

/// \brief 动态线程锁池
class GS_API GsDynamicLockPool
{
public:
	/// \brief 申请一个动态锁
	static GsLockPtr Request(unsigned long long key);
	
	/// \brief 申请一个动态锁
	static GsLockPtr Request(const void* key);

};


/// \brief 动态线程锁
class GS_API GsDynamicLock
{
	GsLockPtr m_pLock;
public:
	GsDynamicLock(const volatile void* key, bool bFake = false);
	GsDynamicLock(const void* key,bool bFake = false);
	GsDynamicLock(unsigned long long key, bool bFake = false);
	~GsDynamicLock();
};

/// \brief 将this当做key进行动态锁
#define GS_DYNAMICLOCK UTILITY_NAME::GsDynamicLock _D_LK_THIS(this);

/// \brief 指定Key当动态锁
#define GS_DYNAMICLOCK_IT(x) UTILITY_NAME::GsDynamicLock _D_LK_IT(x);


UTILITY_ENDNS