#include "VarLenPdu.h"


CValLenPdu::CValLenPdu() {

	m_buffer = NULL;
}

CValLenPdu::~CValLenPdu() {
	if(m_buffer){
		free(m_buffer);
		m_buffer = NULL;
	}
}

void CValLenPdu::Extend(uint32_t len) {
	m_alloc_size = m_write_offset + len;
	m_alloc_size += m_alloc_size>>2;	//右移运算符，扩大四分之一长度
	uchar_t* new_buf = (uchar_t*)realloc(m_buffer, m_alloc_size);
	m_buffer = new_buf;
}





/***************************************************************************************/

CByteStream::CByteStream() {

}

CByteStream::~CByteStream() {

}

int16_t CByteStream::ReadInt16(uchar_t * buf) {
	int16_t data;
	//= (buf[0] << 8) | buf[1];
	return data;
}


uint16_t CByteStream::ReadUInt16(uchar_t * buf) {
	uint16_t data;
	//= (buf[0] << 8) | buf[1];
	return data;
}

int32_t CByteStream::ReadInt32(uchar_t * buf) {
	int32_t data;
	//= (buf[0] << 8) | buf[1];
	return data;
}


uint32_t CByteStream::ReadUInt32(uchar_t * buf) {
	uint32_t data;
	//= (buf[0] << 8) | buf[1];
	return data;
}







