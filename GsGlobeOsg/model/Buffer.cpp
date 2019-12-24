
#include "Buffer.h"

Buffer::Buffer(){//默认构造函数
	_data=0;
	_size=0;
	_alloc=false;
	_curpos=0;
}
void Buffer::clear(){//清空Buffer
	if(_alloc && _data){
		delete  [] ((unsigned char*)_data);
	}
	_data = 0;
	_size=0;
	_alloc=false;
	_curpos=0;
}
Buffer::~Buffer(){//默认析构函数
	clear();
}
Buffer::Buffer(void * d,size_t s,bool b_alloc){//构造函数，如果b_alloc为真，则会新申请一块大小为s的内存块，并复制d的内容，反之，则指向d
	_size=s;
	_alloc=b_alloc;
	if(_alloc){
		_data = new char[_size];
		memcpy(_data,d,_size);
	}
	else{
		_data=d;
	}
	_curpos=0;
}
void Buffer::copy(const Buffer & b){//拷贝Buffer函数，如果b的_alloc成员为假，则拷贝后的Buffer对象与b公用一个内存块，否则为内存块拷贝
	set(b._data,b._size,b._alloc);
	_curpos=0;
}
Buffer::Buffer(const Buffer & b){//拷贝构造函数
	_data = 0;
	_size=0;
	_alloc=false;
	_curpos=0;
	copy(b);
}
Buffer Buffer::operator = (const Buffer & b){//等号赋值操作
	copy(b);
	return * this;
}
void Buffer::set(void * pv, size_t ss,bool b_alloc){//设置缓冲区，如果b_alloc为真，则会新申请一块大小为ss的内存块，并复制pv的内容，反之，则指向pv
	clear();
	_alloc = b_alloc;
	if(_alloc){
		_data = new char[ss];
		_size = ss;
		if(pv)
			memcpy(_data,pv,ss);
	}
	else{
		_data = pv;
		_size = ss;
	}
	_curpos=0;
}


void Buffer::attach(void * pv, size_t ss){
	clear();
	_data = pv;
	_size = ss;
	_alloc=true;
}
void* Buffer::detach(size_t & s){		
	s=_size;
	_alloc=false;
	void* p =_data;
	_data = 0;
	_curpos=0;
	return p;
}

void Buffer::write(size_t beginpos,void * cdata,size_t count)//在指定的位置beginpos处向后写入count个字符
{
	if(beginpos+count<=_size){
		memcpy(((char*)_data)+beginpos,cdata,count);
	}
	else{
		char * p = new char [beginpos+count];
		memcpy(p,_data,beginpos);
		memcpy(p+beginpos,cdata,count);
		delete [] ((char*)_data);
		_data = p;
		_size = beginpos+count;

		// 由于调用了delete [] ((char*)_data); _data = p;  因此，如果内存是以attach方式得到的，
		//则外部的管理者无法实现新内存的析构,故需添加下面的代码或者外面调用clear函数 [7-29-2010 孙卡]
		_alloc = true;
	}
	_curpos = beginpos + count;
}

void Buffer::write(void * cdata, size_t count)
{
	if(_curpos+count<=_size){
		memcpy(((char*)_data)+_curpos,cdata,count);
		_curpos += count;
	}
	else{
		/**修改_curpos增加为新值[2009-09-17cuiyt]*/
		_size = _curpos+count;
		char * p = new char [_size];
		memcpy(p,_data,_curpos);
		memcpy(p+_curpos,cdata,count);
		delete [] ((char*)_data);
		_data = p;
		_curpos += count;	//_curpos增加到当前位置[2009-09-17cuiyt]

		// 由于调用了delete [] ((char*)_data); _data = p;  因此，如果内存是以attach方式得到的，
		//则外部的管理者无法实现新内存的析构,故需添加下面的代码或者外面调用clear函数 [7-29-2010 孙卡]
		_alloc = true;
	}
}

bool Buffer::read(void * cdata, size_t count)
{
	if(_curpos+count<=_size){
		memcpy(cdata,((char*)_data)+_curpos,count);
		_curpos += count;
		return true;
	}
	else{
//		assert(0);
		return false;

	}
}

