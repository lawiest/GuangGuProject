#pragma once
#include "preconfig.h" 
#include "stlhelp.h"

UTILITY_NS

template<class T>
class GsVector :public std::vector<T, GsAllocator<T> >
{
public:
	GsVector()
	{

	}
	GsVector(unsigned int n):std::vector<T, GsAllocator<T> >(n)
	{

	}
	GsVector(T* start,T* end) :std::vector<T, GsAllocator<T> >(start,end)
	{

	}
	GsVector(const GsVector& v) :std::vector<T, GsAllocator<T> >(v)
	{

	}
	GsVector(unsigned int n, T v) :std::vector<T, GsAllocator<T> >(n,v)
	{

	}
};

template<class K>
class GsSet : public std::set<K, std::less<K>, GsAllocator<K > > 
{

};

template<class K,class V>
class GsStlMap :public std::map < K, V, std::less<K>, GsAllocator < std::pair<const K, V> > >
{

};
UTILITY_ENDNS