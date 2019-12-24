#pragma once
#include "GeomathSE.h"

//using namespace geostar;

extern geostar::geo_factory& _gf();
extern geostar::geo_algorithm& _ga();
extern geostar::geo_factory* _gf2();
extern geostar::geo_algorithm* _ga2();

namespace geostar {

struct safe_ga
{
	geostar::geo_algorithm*	m_a;
	geostar::geo_factory*	m_f;

	safe_ga()
	{
		m_f = 0;
		m_a = _ga2();		
	}

	safe_ga(double tol)
	{
		m_f = 0;
		m_a = _ga2();
		m_a->tolerance(tol);
	}

	~safe_ga()
	{ 
		m_a->release(); 
		if(m_f) m_f->release();
	}

    double prop_value(geo_object* obj,int mode)
	{
		return m_a->prop_value(obj,mode);
	}

	geo_object* prop_geo(geo_object* obj,int mode)
	{
		return m_a->prop_geo(obj,mode);
	}

	bool prop_geo_2(geo_object* obj,int mode,double* paras)
	{
		return m_a->prop_geo_2(obj,mode,paras);
	}

	geo_object* nearest(geo_object* a,geo_object* b)
	{
		return m_a->nearest(a,b);
	}

	geo_object* xy2t(geo_object* p,const double* xy,double* t)
	{
		return m_a->xy2t(p,xy,t);
	}

	bool t2xy(geo_object* s,double t,double* xy)
	{
		return m_a->t2xy(s,t,xy);
	}

	bool spr_query_is(geo_object* a,geo_object* b,int mode)
	{
		return m_a->spr_query_is(a,b,mode);
	}

	bool spr_is(int spr,int mode)
	{
		return m_a->spr_is(spr,mode);
	}

	int spr_query(geo_object* a,geo_object* b)
	{
		return m_a->spr_query(a,b);
	}

	void spr_query_n2n(geo_reader* P,geo_reader* Q,geo_writer* R,int mode)
	{
		return m_a->spr_query_n2n(P,Q,R,mode);
	}

	geo_object* spi_create(geo_object* obj,int mode)
	{
		return m_a->spi_create(obj,mode);
	}

	geo_object* gcombine(geo_object* a,geo_object* b,int mode)
	{
		return m_a->gcombine(a,b,mode);
	}

	void gsplit(geo_object* K,geo_object* x,geo_writer* W)
	{
		return m_a->gsplit(K,x,W);
	}

	void n_union(geo_reader* R,geo_writer* W)
	{
		return m_a->n_union(R,W);
	}

	int gcheck_1(geo_object* obj,int mode)
	{
		return m_a->gcheck_1(obj,mode);
	}

	void line_buffer(geo_object* R,double radius,int narc,geo_writer* W)
	{
		return m_a->line_buffer(R,radius,narc,W);
	}

	void line_clean(geo_object* obj,geo_writer* W)
	{
		return m_a->line_clean(obj,W);
	}

	void build(geo_reader* R,geo_writer* W,int mode)
	{
		return m_a->build(R,W,mode);
	}

	void overlay(geo_reader* A,geo_reader* B,geo_writer* W,double min_tol,int mode)
	{
		return m_a->overlay(A,B,W,min_tol,mode);
	}

	int left_right(geo_object* p,double x,double y)
	{
		return m_a->left_right(p,x,y);
	}

	geo_reader* new_reader(void* user,reader_callback func)
	{
		return _gf().new_reader(user,func);
	}

	geo_reader* new_reader_n(int n,geo_reader** ppr)
	{
		return _gf().new_reader_n(n,ppr);
	}    

	geo_writer* new_writer(void* user,writer_callback func)
	{   
		return _gf().new_writer(user,func);
	}

	geo_reader* open_reader(string_t format,string_t filename)
	{
		return _gf().open_reader(format,filename);
	}

	geo_writer* open_writer(string_t format,string_t filename)
	{
		return _gf().open_writer(format,filename);
	}

	geo_trans* create_trans(string_t scrip)
	{
		return _gf().create_trans(scrip);
	}

	int dim_g(geo_object* p)
	{ 
		return _gf().dim_g(p); 
	}

	int sub_n(geo_object* p)
	{ 
		return _gf().sub_n(p); 
	}

	geo_object* sub_at(geo_object* p,int i)
	{ 
		return _gf().sub_at(p,i); 
	}
	    
	int point_n(geo_object* p)
	{ 
		return _gf().point_n(p); 
	}

	const void* point_at(geo_object* p,int i,int vt)
	{ 
		return _gf().point_at(p,i,vt); 
	}

	geo_object* create_single(int gdim,int pnum,int cdim,const double* xyz,int flag,const void* vt_p)
	{
		return _gf().create_single(gdim,pnum,cdim,xyz,flag,vt_p); 
	}

	geo_object* create_multi(int gdim,int snum,geo_object** subs,int flag)
	{
		return _gf().create_multi(gdim,snum,subs,flag);
	}

	geo_object* ora_create(int i_n,const int *i_p,int c_n,int cdim,const double *c_p,int flag)
	{
		return _gf().ora_create(i_n,i_p,c_n,cdim,c_p,flag);
	}

	bool ora_get_info(geo_object* obj,int* i_n,int** i_p,int* c_n,int* cdim,double **c_p)
	{
		if(m_f==0) m_f = _gf2();
		return m_f->ora_get_info(obj,i_n,i_p,c_n,cdim,c_p);
	}
	
	geo_object* t2path(geo_object* p,double t0,double t1)
	{
		return _ga().t2path(p, t0, t1);
	}
};

}