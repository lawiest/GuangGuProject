#pragma once
#include "GeomathSE.h"

//geo_object 的智能指针
namespace geostar {

struct gobjptr
{
    geostar::geo_object*   m_ptr;
	gobjptr(geostar::geo_object* p,int n):m_ptr(p)
    { 
        
    }
    gobjptr(geostar::geo_object* p=0):m_ptr(p)
    { 
        if(p)p->addref(); 
    }
    gobjptr(const gobjptr& p):m_ptr(p.m_ptr)
    { 
        if(m_ptr) m_ptr->addref(); 
    }

    ~gobjptr()
    { 
        if(m_ptr) m_ptr->release(); 
    }

    void clear()
    {
        if(m_ptr) m_ptr->release();
        m_ptr = 0;
    }

    gobjptr& operator=(geostar::geo_object* p)
    {
        if(p) p->addref();
        if(m_ptr) m_ptr->release();
        m_ptr = p;
        return *this;
    }
    
    gobjptr& operator=(const gobjptr& p)
    { 
        return operator=((geostar::geo_object*)p.m_ptr); 
    }
    
    gobjptr& attach(geostar::geo_object* p)
    {
        if(m_ptr) m_ptr->release();
        m_ptr = p;
        return *this;
    }
    
    geostar::geo_object* operator ->(){ return m_ptr; }
    operator bool(){ return m_ptr!=0; }
    operator geostar::geo_object*(){ return m_ptr; }
	template<class T>
	T* cast()
	{
		return dynamic_cast<T*>(m_ptr);
	}
};

inline geostar::geo_object* gaddref(geostar::geo_object* g){ if(g)g->addref(); return g; }
inline void grelease(geostar::geo_object* g){ if(g)g->release(); }

//geo_object的缺省实现
template<class type>
struct Tobject : type
{
    volatile int m_ref;
    Tobject():m_ref(0){}
	virtual ~Tobject(){}
    virtual string_t type_info()const{ return 0; }
    virtual void* type_cast(string_t){ return 0; }
    virtual int i4_get(int i){ return 0; }
    virtual bool i4_set(int i,int v){ return false; }

    virtual int addref(){ return ++m_ref; }
    virtual int release(){
        if(--m_ref>0) return m_ref;
        delete this;
        return 0;
    }
};

};