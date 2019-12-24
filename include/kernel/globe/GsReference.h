#pragma once
#include <utility.h>

GLOBE_NS

class GS_API GsReference :public Utility::GsRefObject
{
protected:
	virtual void* Handle() = 0;
public:
	virtual ~GsReference()
	{

	}

	template<class T>
	T& CastTo()
	{
		T* p = (T*)Handle();
		return *p;
	}
};
GS_SMARTER_PTR(GsReference);

GLOBE_ENDNS

