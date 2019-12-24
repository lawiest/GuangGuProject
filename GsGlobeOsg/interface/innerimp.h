#pragma  once
#include "Layer.h"
#include <osg/ref_ptr>

template<class T>
class ReferenceImp:public GLOBE_NAME::GsReference
{
	osg::ref_ptr<T> m_ptr;
public:
	ReferenceImp(osg::ref_ptr<T>& ptr)
	{
		m_ptr = ptr;
	}
	
protected:
	virtual void* Handle()
	{
		return &m_ptr;
	}

};