#ifndef _VARIABLE_LENGTH_ARRAY_H_
#define _VARIABLE_LENGTH_ARRAY_H_

#include "ostype.h"

typedef unsigned char uchar_t;

class CValLenPdu {
public:
	CValLenPdu();
	~CValLenPdu();

	uchar_t* GetBuffer() {return m_buffer;}
	uint32_t GetAllocSize() {return m_alloc_size;}
	uint32_t GetWriteOffset() {return m_write_offset;}

	void Extend(uint32_t len);
	void IncWriteOffset(uint32_t len) {m_write_offset += len;}

	uint32_t	Write(void* buf, uint32_t len);
	uint32_t	Read(void* buf, uint32_t len);
	
private:
	//变长数组，存放数据
	uchar_t*			m_buffer;

	//变长数组所能存放的数据总量，也就是变长数组的长度
	uint32_t			m_alloc_size;

	//变长数组已经填充的数据的长度。也就是数组中存放了多长的数据
	uint32_t			m_write_offset;
};

class CByteStream {
public:
	CByteStream();
	~CByteStream();

	static int16_t ReadInt16(uchar_t* buf);
	static uint16_t ReadUInt16(uchar_t* buf);
	static int32_t ReadInt32(uchar_t* buf);
	static uint32_t ReadUInt32(uchar_t* buf);
	
private:
};



#endif

