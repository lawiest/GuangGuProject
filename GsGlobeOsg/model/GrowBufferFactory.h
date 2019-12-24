#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <utility.h>
typedef unsigned char byte;
typedef unsigned int        UINT;
typedef unsigned long long    UINT64;

class CGrowBufferFactory:std::list<GeoStar::Utility::GsGrowByteBuffer*>
{
private:
	OpenThreads::Mutex m_Mutex;

public:
	CGrowBufferFactory()
	{
	}
	~CGrowBufferFactory()
	{
		Clear();
	}
	void Clear()
	{
		//添加线程安全
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_Mutex);

		std::list<GeoStar::Utility::GsGrowByteBuffer*>::iterator it = this->begin();
		for(;it != this->end();it++)
			delete (*it);
		this->clear();
	}

	GeoStar::Utility::GsGrowByteBuffer* Instance(size_t nSize)
	{
		//添加线程安全
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_Mutex);

		if(!this->empty())
		{
			GeoStar::Utility::GsGrowByteBuffer* data = this->front();
			data->Allocate(nSize);
			this->pop_front();
			return data;
		}
		GeoStar::Utility::GsGrowByteBuffer* data = new GeoStar::Utility::GsGrowByteBuffer(nSize);
		return data;
	}

	void Recycle(GeoStar::Utility::GsGrowByteBuffer* data)
	{  
		//添加线程安全
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_Mutex);

		if (data == NULL)
			return;

		this->push_back(data);
		//while(this->size()>5)
		//{
		//	GsGrowByteBuffer* pData = this->front();
		//	this->pop_front();
		//	delete pData;
		//}
	}
};

