#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned char byte;
typedef unsigned int        UINT;
typedef unsigned long long    UINT64;

//class Buffer;
//class BufferManager;

class /*/*//*/*/ Buffer{
	void * _data;//ָ���ڴ��ͷ��ַ
	size_t _size;//�ڴ���С
	bool _alloc;//�Ƿ���Buffer���Լ�������ڴ棬�������Ϊ�棬��֮Ϊ��
	size_t _curpos;//��ǰλ�ã�Ĭ��Ϊ0
public:
	inline  void const *  data() const {return _data;}//��ȡ�ڴ���ͷ��ַ
	inline void *  data(){return _data;}//��ȡ�ڴ���ͷ��ַ
	inline size_t   size() const {return _size;}//��ȡ�ڴ���С

	unsigned char* allocbuff(unsigned int nBufferSize);
public:

	Buffer();//Ĭ�Ϲ��캯��
	void clear();//���Buffer
	~Buffer();//Ĭ����������
	Buffer(void * d,size_t s,bool b_alloc=false);//���캯�������b_allocΪ�棬���������һ���СΪs���ڴ�飬������d�����ݣ���֮����ָ��d

	void copy(const Buffer & b);//����Buffer���������b��_alloc��ԱΪ�٣��򿽱����Buffer������b����һ���ڴ�飬����Ϊ�ڴ�鿽��

	Buffer(const Buffer & b);//�������캯��

	Buffer operator = (const Buffer & b);//�ȺŸ�ֵ����

	void set(void * pv, size_t ss,bool b_alloc=false);//���û����������b_allocΪ�棬���������һ���СΪss���ڴ�飬������pv�����ݣ���֮����ָ��pv

	inline void setPos(size_t pos){	
		if(_size>0){
			_curpos = pos;
			assert(pos <= _size && pos >= 0);
		}			
	}

	inline size_t getPos() {	return _curpos;	}

	inline void * getCurPtr() { return (void*)(((char *)_data)+_curpos);}//��ȡ��ǰλ��ָ��


	void attach(void * pv, size_t ss);
	void* detach(size_t & s);

	/** �����ڴ�ֵ����ָ����λ��beginpos�����д��count���ַ�
	* @param [in] beginpos ��ʼд��λ��  
	* @param [in] cdata ����д�������ָ��
	* @param [in] count ����д������ݴ�С
	* @return
	*/
	void write(size_t beginpos,void * cdata,size_t count);

	/** �����ڴ�ֵ����Buffer����Ĭ�ϵ�λ�ã����д��count���ַ�
	* д������󣬵�ǰָ������ڴ��ͷ��λ���Զ�����ƶ�countλ
	* @param [in] cdata ����д�������ָ��
	* @param [in] count ����д������ݴ�С
	* @return
	*/
	void write(void * cdata, size_t count);

	/** ��ȡ�ڴ�ֵ����Buffer����Ĭ�ϵ�λ�ã�����ȡcount���ַ�
	* ��ȡ�����󣬵�ǰָ������ڴ��ͷ��λ���Զ�����ƶ�countλ
	* @param [in] cdata ������ֵ������ָ��
	* @param [in] count	����ȡ�����ݴ�С
	* @return
	*/
	bool read(void* cdata, size_t count);

};

/** @} */ 

/** @defgroup IBufferIO �����д�ӿ�
�����࣬����Ҫд�����ݿ��еĶ������Ҫʵ�ִ˴��麯��  
*  @{
*/
class /*DLL_EXPORT*/ IBufferIO{
public:
	virtual bool writeBuffer(Buffer& buf)=0;
	virtual bool readBuffer(const Buffer & buf)=0;
};
/** @} */ 

class BufferManager
{
public:
	static unsigned char* AllocBuff(unsigned int nBufferSize)
	{
		unsigned char* pBuff = (unsigned char*)malloc(nBufferSize);
		return pBuff;
	}

	static void FreeBuffer(unsigned char* pBuffHead, unsigned int nBufferSize)
	{
		if (pBuffHead)
		{
			free(pBuffHead);
			pBuffHead = NULL;
		}
	}
};
