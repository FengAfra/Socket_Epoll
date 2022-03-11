#include "VarLenPdu.h"


CValLenPdu::CValLenPdu() {

	m_buffer = NULL;
	m_alloc_size = 0;
	m_write_offset = 0;
}

CValLenPdu::~CValLenPdu() {
	m_alloc_size = 0;
	m_write_offset = 0;

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

uint32_t CValLenPdu::Write(void * buf, uint32_t len) {
	sLogMessage("Write begin ", LOGLEVEL_DEBUG );
	if(m_write_offset + len > m_alloc_size) {
		Extend(len);
	}

	if(buf) {
		memcpy(m_buffer + m_write_offset, buf, len);
	}
	m_write_offset += len;
	sLogMessage("Write end ", LOGLEVEL_DEBUG );
	return len;
}

uint32_t CValLenPdu::Read(void * buf, uint32_t len) {
	if(0 == len) 
		return len;

	if(len > m_write_offset)
		len = m_write_offset;

	if(buf) 
		memcpy(buf, m_buffer, len);

	m_write_offset -= len;
	memmove(m_buffer, m_buffer + len, m_write_offset);
	return len;
}



/***************************************************************************************/

CByteStream::CByteStream() {

}

CByteStream::~CByteStream() {

}

int16_t CByteStream::ReadInt16(uchar_t * buf) {
	int16_t data = 0;
	//= (buf[0] << 8) | buf[1];
	return data;
}


uint16_t CByteStream::ReadUInt16(uchar_t * buf) {
	uint16_t data = 0;
	//= (buf[0] << 8) | buf[1];
	return data;
}

int32_t CByteStream::ReadInt32(uchar_t * buf) {
	uint32_t data = 0;
	//= (buf[0] << 8) | buf[1];
	return data;
}


uint32_t CByteStream::ReadUInt32(uchar_t * buf) {
	uint32_t data = 0;
	//= (buf[0] << 8) | buf[1];
	return data;
}







