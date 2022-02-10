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



