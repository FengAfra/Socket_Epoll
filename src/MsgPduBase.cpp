#include "MsgPduBase.h"

CMsgPduBase::CMsgPduBase() {

}

CMsgPduBase::~CMsgPduBase() {

}


CMsgPduBase* CMsgPduBase::ReadPdu(uchar_t * buf, uint32_t len) {
	//uint32_t pdu_len;
	//if(!IsPduAvailable(buf, len, pdu_len)) {	//判断m_in_buffer中是否存在一个完整的包
	//	return NULL;
	//}

	//uint16_t service_id = CByteStream::ReadUint16(buf + 8);
	//uint16_t command_id = CByteStream::ReadUint16(buf + 10);

	//测试用 begin
	if(len <= 0)
		return NULL;
	uint32_t pdu_len = len;
	//测试用 end
	
	CMsgPduBase* pMsgPdu = NULL;
	pMsgPdu = new CMsgPduBase();
	pMsgPdu->Write(buf, pdu_len);

	return pMsgPdu;
}


bool CMsgPduBase::IsPduAvailable(uchar_t * buf, uint32_t len, uint32_t & pdu_len) {
	if(len < MSG_PDU_HEADER_LEN) {
		sLogMessage("buffer length is less header length", LOGLEVEL_ERROR);
		return false;
	}

	pdu_len = CByteStream::ReadUInt32(buf);		//读取前4个字节，也就是length字段
	if(pdu_len > len) {	//包头的length的长度比buf中存放的数据长，也就是不够一个包
		sLogMessage("pdu_len=%d, len=%d", LOGLEVEL_ERROR, pdu_len, len);
		return false;
	}
	/*
	1、对包头中的length字段等于0的情况暂且不处理
	if(pdu_len == 0 ){
		sLogMessage("pdu_len=%d, len=%d", LOGLEVEL_ERROR, pdu_len, len);
		return false;
	}
	*/
	return true;
}

void CMsgPduBase::Write(uchar_t * buf, uint32_t len) {
	m_buf.Write(buf, len);
}

uchar_t* CMsgPduBase::GetBuffer() {
	return m_buf.GetBuffer();
}



