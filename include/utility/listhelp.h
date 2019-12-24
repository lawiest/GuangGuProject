#pragma once
#include "preconfig.h" 
#include "stlhelp.h"

UTILITY_NS 
//#define GsList std::list // 定义变量GsList
template<class T>
class GsDeque :public std::deque<T, GsAllocator<T> >
{
public:
	GsDeque()
	{

	}
};
template<class T>
class GsStack :public std::stack<T,GsDeque<T> >
{
public:
	GsStack()
	{
	}
};

template<class T>
class GsQueue :public std::queue<T,GsDeque<T> >
{
public:
	GsQueue()
	{

	}
};


template<class T>
class GsList :public std::list<T, GsAllocator<T> >
{
public:
	GsList()
	{

	}
};



UTILITY_ENDNS