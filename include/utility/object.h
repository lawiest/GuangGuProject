 #pragma once
#include "preconfig.h" 
#include "atomic.h"
#include "stringhelp.h"
#include "event.h"

UTILITY_NS

/// \brief 不可拷贝对象，通过继承此类实现对象的不可拷贝
class GS_API GsNoneCopyable
{ 
public:
	GsNoneCopyable() {}

private:
    GsNoneCopyable(const GsNoneCopyable&);
	/// \brief 操作符重载，返回对象的引用
    GsNoneCopyable& operator=(const GsNoneCopyable&);


}; 



/// \brief 引用对象，通过继承此类实现对象对引用计数的支持
class GS_API GsRefObject:private GsNoneCopyable
{
protected:
	GsAtomic<int>  m_RefCount;

	/// \brief 当引用计数为0的时候会激发此函数，继承类可以覆盖此函数做特殊的实现
    ///\return 返回false则GsRefObject不会主动删除对象
	virtual bool OnFinalRelease();
public:
	GsRefObject();
	virtual ~GsRefObject();
	
	/// \brief 增加一个引用计数
    ///\return 返回增加后的引用计数值
	virtual int AddRef();
	/// \brief 减少一个引用计数
    ///\return 返回减少后的引用计数值
	virtual int Release();
	/// \brief 获取引用计数的值
    ///\return 返回当前引用计数的值
	virtual int RefCount();

	/// \brief 获取对象的hash值
	///\return 缺省为对象自身的指针
	virtual unsigned long long HashCode();

	/// \brief 转换自身的类型
	template<class T>
	T* CastTo()
	{
		return dynamic_cast<T*>(this);
	}

	/// \brief 判断自身是否是特定的类型
	template<class T>
	T* Is()
	{
		return dynamic_cast<T*>(this)?true:false;
	}
	/// \brief 但对象析构的时候发生
	GsDelegate<void(GsRefObject*)> OnDestructor;

public:

	void(operator delete)(void* in_pVoid)
	{
		GsByteAllocator::Deallocate(in_pVoid, 0);
	}
	void* (operator new)(size_t in_size)
	{
		return GsByteAllocator::Allocate(in_size);
	}
	void(operator delete[])(void* in_pVoid)
	{
		GsByteAllocator::Deallocate(in_pVoid, 0);
	}
	void* (operator new[])(size_t in_size)
	{
		return GsByteAllocator::Allocate(in_size);
	}
};

/// \brief 对GsRefObject的弱引用
class GS_API GsWeakReference :public GsRefObject
{
	GsRefObject* m_RefObject;

	/// \brief 但对象析构的时候发生
	void OnDestructor(GsRefObject* obj);
public:
	GsWeakReference(GsRefObject* ref);
	~GsWeakReference();
	
	/// \brief 获取引用的对象
	GsRefObject* LockReferenceObject();
	
	/// \brief 是否过期，引用的对象是否已经失效
	bool Expired();

};


/// \brief 智能指针模板对象，实现对GsRefObject对象的引用计数自动管理
template <class T>
class GsSmarterPtr
{
public:
	/// \brief 无参构造函数
	GsSmarterPtr() throw()
	{
		p = NULL;
	}
	/// \brief int参数类型构造函数
	GsSmarterPtr(int nNull) throw()
	{  
		p = NULL;
	}
	/// \brief long int参数类型构造函数
	GsSmarterPtr(long int nNull) throw()
	{  
		p = NULL;
	}
	/// \brief 模板构造函数，实现引用计数加1
	template<class O>
	GsSmarterPtr(O* lp,bool bAddRef = true) throw()
	{
		p = dynamic_cast<T*>(lp); // 类型转换
		if (p != NULL && bAddRef)
			p->AddRef(); // 引用计数加1
	}
	/// \brief 模板构造函数，实现引用计数加1
	template<class O>
	GsSmarterPtr(const GsSmarterPtr<O>& lp) throw()
	{
		p = dynamic_cast<T*>(lp.p); // 类型转换
		if (p != NULL)
			p->AddRef(); // 引用计数加1
	}
	/// \brief 构造函数，实现引用计数加1
	GsSmarterPtr(const GsSmarterPtr<T>& lp) throw()
	{
		p = lp.p;
		if (p != NULL)
			p->AddRef(); // 引用计数加1
	}
	/// \brief 构造函数，实现引用计数加1
	GsSmarterPtr(const GsRefObject* lp,bool bAddRef = true) throw()
	{
		p = dynamic_cast<T*>(lp); // 类型转换
		if (p != NULL && bAddRef)
			p->AddRef(); // 引用计数加1
	}
	/// \brief 不能被隐式调用的构造函数，实现引用计数加1
	explicit GsSmarterPtr(T* lp,bool bAddRef = true) throw()
	{
		p = lp;
		if (p != NULL && bAddRef)
			p->AddRef(); // 引用计数加1
	}

public: 
	/// \brief 析构函数，实现引用计数减1
	~GsSmarterPtr() throw()
	{
		if (p)
			p->Release(); // 引用计数减1
	}
	/// \brief 如果指针为空，就转换指针类型
	template<class Q>
	GsSmarterPtr<Q> As()
	{
		if(!p)
			return 0;
		return dynamic_cast<Q*>(p);
	}
	/// \brief 如果指针为空，就转换指针类型
	template<class Q>
	bool Is()
	{
		if(!p)
			return false;
		return dynamic_cast<Q*>(p) != 0;
	}
	/// \brief 类型转化模板
	operator T*() const throw()
 	{
		return p;
	}
	/// \brief 类型转化
	operator GsRefObject*() const throw()
	{
		return p;
	}
	/// \brief 类型转化
	explicit operator bool() const throw()
	{
		return 0 != p;
	}
	
	/// \brief 符号重载
	T& operator*() const
	{ 
		return *p;
	} 
	/// \brief 符号重载
	T** operator&() throw()
	{ 
		return &p;
	}
	/// \brief 符号重载
	T* operator->() const throw()
	{
		return p;
	}
	/// \brief 符号重载
	bool operator!() const throw()
	{
		return (p == 0);
	}
	/// \brief 符号重载
	bool operator<(T* pT) const throw()
	{
		return p < pT;
	}

	/// \brief 引用计数加1
	void AddRef()throw()
	{
		if(p)
          p->AddRef();
		
	}
	/// \brief 引用计数减1
	void Release() throw()
	{
		T* pTemp = p;
		if (pTemp)
		{
			p = NULL;
			pTemp->Release();
		}
	} 
	/// \brief 比较2个指针的值
	bool IsEqualObject(GsRefObject* pOther) throw()
	{
		/// \brief 如果2个object都为null，则返回true
		if (p == NULL && pOther == NULL)
			return true;	
		/// \brief 如果1个object为null，另一个不为null，则返回false
		if (p == NULL || pOther == NULL)
			return false;	
		/// \brief 如果2个object都不为null，则判断他们的值是否相等
		GsRefObject* p1 = dynamic_cast<GsRefObject*>(p);
		return p1 == pOther;
	} 
	/// \brief 绑定一个管理对象，引用计数不增加
	void Attach(T* p2) throw()
	{
		if (p)
			p->Release();
		p = p2;
	} 
	/// \brief 传递指针的值
	T* Detach() throw()
	{
		T* pt = p;
		p = NULL;
		return pt;
	}
	/// \brief 引用计数加1
	bool CopyTo(T** ppT) throw()
	{
		if (ppT == NULL)
			return false;
		*ppT = p;
		if (p)
			p->AddRef();
		return true;
	}
	 

		/// \brief 符号重载，若指针非空，则引用计数加1，否则减1
	T* operator=(T* lp) throw()
	{
		if(p != NULL)
			if(this->p == lp) // if (*this == lp) 指针与指针判断相等 不能用智能指针与普通指针进行判断
				return *this;
		if(lp)
			lp->AddRef();
		Attach(lp);
        return *this;
	}
	/// \brief 符号重载，若指针非空，则引用计数加1，否则减1
	T* operator=(const GsSmarterPtr<T>& lp) throw()
	{
        if(this->p != lp.p)
        {
			if(lp)
				lp->AddRef();
			Attach(lp);
		}
        return *this;
	}

	T* p;
}; 

template<class T,class Q>
bool operator==(const GsSmarterPtr<T>& lp1, const GsSmarterPtr<Q>& lp2) throw()
{	// test if GsSmarterPtr == GsSmarterPtr
	return (lp1.p == lp2.p);
}

template<class T,class Q>
bool operator!=(const GsSmarterPtr<T>& lp1, const GsSmarterPtr<Q>& lp2) throw()
{	// test if GsSmarterPtr != GsSmarterPtr
	return lp1.p != lp2.p;
}

template<class T>
bool operator==(const GsSmarterPtr<T>& lp, std::nullptr_t) throw()
{	// test if GsSmarterPtr == nullptr
	return lp.p == nullptr;
}

template<class T>
bool operator==(std::nullptr_t, const GsSmarterPtr<T>& lp) throw()
{	// test if nullptr == GsSmarterPtr
	return nullptr == lp.p;
}

template<class T>
bool operator!=(const GsSmarterPtr<T>& lp, std::nullptr_t) throw()
{	// test if GsSmarterPtr != nullptr
	return lp.p != nullptr;
}

template<class T>
bool operator!=(std::nullptr_t, const GsSmarterPtr<T>& lp) throw()
{	// test if nullptr != GsSmarterPtr
	return nullptr != lp.p;
}
 

/// \brief 弱指针
template<class T>
class GsWeakSmarterPtr
{
	GsWeakReference* m_WeakRef;
public:
	GsWeakSmarterPtr() :m_WeakRef(NULL)
	{
	} 
	GsWeakSmarterPtr(T* obj) :m_WeakRef(NULL)
	{
		if (!obj)
			return;
		m_WeakRef = new GsWeakReference(obj);
		m_WeakRef->AddRef();
	}

	GsWeakSmarterPtr(const GsSmarterPtr<T>& obj) :m_WeakRef(NULL)
	{
		if (!obj.p)
			return;
		m_WeakRef = new GsWeakReference(obj.p);
		m_WeakRef->AddRef();
	}
	GsWeakSmarterPtr(const GsWeakSmarterPtr<T>& weak) :m_WeakRef(NULL)
	{
		Reset(weak);
	}
	~GsWeakSmarterPtr()
	{
		if (m_WeakRef)
			m_WeakRef->Release();
	}

	/// \brief 是否过期，引用的对象是否已经失效
	bool Expired()
	{
		if (!m_WeakRef)
			return true;
		return m_WeakRef->Expired();
	}
	/// \brief 是否过期，引用的对象是否已经失效，兼容stl命名
	bool expired()
	{
		return Expired();
	}
	/// \brief 引用的对象是否有效，没有过期则有效
	operator bool()
	{
		return !Expired();
	}
	/// \brief 释放被管理对象的所有权 ,兼容stl命名
	void reset()
	{
		Reset();
	}

	/// \brief 释放被管理对象的所有权
	void Reset()
	{
		if (m_WeakRef)
			m_WeakRef->Release();
		m_WeakRef = NULL;
	}
	/// \brief 设置新的管理指针，释放原有的指针管理权
	void Reset(const GsWeakSmarterPtr<T>& weak)
	{
		if (weak.m_WeakRef)
			weak.m_WeakRef->AddRef();
		Reset();
		m_WeakRef = weak.m_WeakRef;
	}
	/// \brief 设置新的管理指针，释放原有的指针管理权
	void Reset(const GsSmarterPtr<T>& obj)
	{
		Reset(obj.p);
	}

	/// \brief 释放被管理对象的所有权 ,兼容stl
	void reset(T* obj)
	{
		Reset(obj);
	}
	void Reset(T* obj)
	{
		Reset();
		if (!obj)
			return;
		m_WeakRef = new GsWeakReference(obj);
		m_WeakRef->AddRef();
	}
	 
	GsWeakSmarterPtr<T>& operator=(const GsWeakSmarterPtr<T>& right)
	{
		Reset(right);
		return *this;
	}
	GsWeakSmarterPtr<T>& operator=(T* right)
	{
		Reset(right);
		return *this;
	}

	GsWeakSmarterPtr<T>& operator=(const GsSmarterPtr<T>& right)
	{
		Reset(right);
		return *this;
	}
	/// \brief 获取强指针的引用,兼容stl命名
	GsSmarterPtr<T> lock() const
	{
		return Lock();
	}
	/// \brief 获取强指针的引用
	GsSmarterPtr<T> Lock() const
	{
		if (!m_WeakRef)
			return GsSmarterPtr<T>();

		GsRefObject* ref = m_WeakRef->LockReferenceObject();
		if (!ref)
			return GsSmarterPtr<T>();

		//不增加引用计数，因为LockReferenceObject 已经增加了。
		return GsSmarterPtr<T>(ref, false);
	} 

};
/// \brief 基于对象自身引用计数的智能指针
#define GS_SMARTER_PTR(P) typedef UTILITY_NAME::GsSmarterPtr<P> P ## Ptr;\
						   typedef UTILITY_NAME::GsWeakSmarterPtr<P> P ## WPtr;
template	<class T>
class GsSharedPtr :public std::shared_ptr<T>
{

};
/// \brief 自身无引用计数机制的智能指针
#define GS_SHARED_PTR(P) typedef std::shared_ptr<P> P ## Ptr;\
						typedef std::weak_ptr<P > P ## WPtr;

/// \brief 动态库封装
class GS_API GsModule :public GsRefObject
{
	void* m_Handle;
public:
	GsModule(const char* fileName);
	~GsModule();

	/// \brief 是否有效
	bool IsValid();
	/// \brief 获取导出函数指针
	void* FunctionAddress(const char* functionName);

};
/// \brief GsModulePtr
GS_SMARTER_PTR(GsModule);

/// \brief 类工厂，用于注册类创建函数和创建类
class GS_API GsClassFactory
{
public:
	typedef GsRefObject* (*FactoryCreate_PTR)();

	/// \brief 注册类创建函数
	static void RegisterFactoryCreate(FactoryCreate_PTR fc_ptr,const char* className,const char* category = NULL);
	
	/// \brief 注册延迟加载的类创建函数
	static void RegisterDelayloadFactoryCreate(const char* funName, const char* className, const char* dllName, const char* category = NULL);

	/// \brief 创建类的实例
	/// \details  返回的对象的引用计数为1
	static GsRefObject* CreateInstance(const char* className);

	/// \brief 依据特定创建类的实例
	template<class T>
	static GsSmarterPtr<T> CreateInstanceT(const char* className)
	{
		GsRefObject* obj = CreateInstance(className);
		if (!obj) return 0;
		T* o = obj->CastTo<T>();
		if (!o)
		{
			obj->Release();
			return 0;
		}
		return GsSmarterPtr<T> (o);
	}

	/// \brief 获取特定类别的类名称
	static GsVector<GsString> ClassNameByCategory(const char* category);


};

#define DECLARE_CLASS_CREATE(class_name) extern "C" GS_API UTILITY_NAME::GsRefObject*  CreateClass## class_name ();
#define DECLARE_CLASS_CREATE_IMP(class_name) extern "C" GS_API UTILITY_NAME::GsRefObject*  CreateClass## class_name (){return new class_name;}

#define REG_CLASS_CREATE(class_name) UTILITY_NAME::GsClassFactory::RegisterFactoryCreate(CreateClass## class_name, #class_name);
#define REG_CLASS_CREATE_NS(class_name,ns) UTILITY_NAME::GsClassFactory::RegisterFactoryCreate(ns::CreateClass## class_name, #class_name);
#define REG_CLASS_CREATE_ALIAS(class_name,alias_name) UTILITY_NAME::GsClassFactory::RegisterFactoryCreate(CreateClass## class_name, alias_name);

#define REG_CLASS_CREATE_CATEGORY(class_name,category) UTILITY_NAME::GsClassFactory::RegisterFactoryCreate(CreateClass## class_name, #class_name,#category);
#define REG_CLASS_CREATE_NS_CATEGORY(class_name,ns,category) UTILITY_NAME::GsClassFactory::RegisterFactoryCreate(ns::CreateClass## class_name, #class_name,#category);
#define REG_CLASS_CREATE_ALIAS_CATEGORY(class_name,alias_name,category) UTILITY_NAME::GsClassFactory::RegisterFactoryCreate(CreateClass## class_name, alias_name,#category);

#define REG_DELAY_CLASS_CREATE(class_name,module_name) UTILITY_NAME::GsClassFactory::RegisterDelayloadFactoryCreate("CreateClass"#class_name, #class_name,module_name);
#define REG_DELAY_CLASS_CREATE_ALIAS(class_name,alias_name,module_name) UTILITY_NAME::GsClassFactory::RegisterDelayloadFactoryCreate("CreateClass"#class_name, alias_name,module_name);

#define REG_DELAY_CLASS_CREATE_CATEGORY(class_name,module_name,category) UTILITY_NAME::GsClassFactory::RegisterDelayloadFactoryCreate("CreateClass"#class_name, #class_name,module_name，#category);
#define REG_DELAY_CLASS_CREATE_ALIAS_CATEGORY(class_name,alias_name,module_name,category) UTILITY_NAME::GsClassFactory::RegisterDelayloadFactoryCreate("CreateClass"#class_name, alias_name,module_name,#category);

UTILITY_ENDNS
