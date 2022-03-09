
#include "ConnObject.h"

ConnMap_t CConnObject::s_msg_conn_map;
CConnObject::CConnObject(){
	m_busy = false;
}

CConnObject::~CConnObject() {


}

void CConnObject::Conn_callback(void * callback_data, NETLIB_MSG msg, SOCKET fd) {
	
	sLogMessage("CConnObject::Conn_callback BEGIN", LOGLEVEL_DEBUG);
	CConnObject* pConnobj = NULL;
	auto iter = s_msg_conn_map.find(fd);
	if(iter !=  s_msg_conn_map.end()) {
		pConnobj = iter->second;
	}
	if(!pConnobj) {
		sLogMessage("s_msg_conn_map find fd : %d  failed", LOGLEVEL_ERROR, fd);
		return;
	}
	switch(msg) {
	case NETLIB_MSG_CLOSE:
		pConnobj->OnClose();
		break;
	case NETLIB_MSG_READ:
		pConnobj->OnRead();
		break;
	case NETLIB_MSG_WRITE:
		pConnobj->OnWrite();
		break;
	case NETLIB_MSG_TIMER:
		pConnobj->OnTimer();
		break;
	case NETLIB_MSG_CONFIRM:
		pConnobj->OnConfirm();
		break;
	default:
		sLogMessage("!!!imconn_callback error msg: %d  failed", LOGLEVEL_ERROR, fd);
		break;
	}
	sLogMessage("CConnObject::Conn_callback END", LOGLEVEL_DEBUG);
}

void CConnObject::OnClose() {

}

void CConnObject::OnTimer() {

}

void CConnObject::OnConfirm() {

}

void CConnObject::OnRead() {
	/*
	1、读取socket的recvbuffer的内容，直到读取完一整个包，避免出现分包，粘包的处理。
	2、读取一整个包中的包头，包体，通过包体来进行相对应的处理。
	3、CConnObject类中有两个变长数组，可以存放此socket接收的数据和将要发送的数据
	*/
	sLogMessage("CConnObject::OnRead BEGIN", LOGLEVEL_DEBUG);
	while(true) {
		uint32_t free_buf_len = m_in_buffer.GetAllocSize() - m_in_buffer.GetWriteOffset();
		if(free_buf_len < READ_BUF_SIZE) {
			m_in_buffer.Extend(READ_BUF_SIZE);
		}
		sLogMessage("handle = %u, netlib_recv into, time = %u\n", LOGLEVEL_INFO, m_handler, get_tick_count());
		int ret = netlib_recv(m_handler, m_in_buffer.GetBuffer() + m_in_buffer.GetWriteOffset(), READ_BUF_SIZE);
		if(ret <= 0) {
			sLogMessage("netlib_recv 0 buffer, break", LOGLEVEL_INFO);
			break;
		}

		m_in_buffer.IncWriteOffset(ret);
	}
	//将会一个一个包的处理，每次处理完一个完整的包，都会将处理过后的包数据从m_in_buffer中删除
	CMsgPduBase* pMsgPdu = NULL;
	{
		//读取一个完整的包
		while((pMsgPdu = CMsgPduBase::ReadPdu(m_in_buffer.GetBuffer(), m_in_buffer.GetWriteOffset() ))) {
			uint32_t pdu_len = pMsgPdu->GetLength();
			sLogMessage("handle = %u, pdu_len into = %u\n",LOGLEVEL_INFO, m_handler, pdu_len);

			HandlePdu(pMsgPdu);
			printf("handle = %u, recvMsg=[%s]\n", m_handler, pMsgPdu->GetBuffer());
			sLogMessage("handle = %u, recvMsg=[%s]\n", LOGLEVEL_INFO, m_handler, pMsgPdu->GetBuffer());
				
			m_in_buffer.Read(NULL, pdu_len);	//将包的数据出队列
			delete pMsgPdu;
			pMsgPdu = NULL;
		}

	}
	sLogMessage("CConnObject::OnRead END", LOGLEVEL_DEBUG);
}

void CConnObject::OnWrite() {

	//EPOLLOUT在EPOLLET的情况下，在写缓冲区为空的时候，会触发一次，这个时候，需要return。只有在send失败的的时候，将其置为ture
	sLogMessage("CConnObject::OnWrite BEGIN", LOGLEVEL_DEBUG);
	if(!m_busy) {
		sLogMessage("CConnObject::OnWrite END", LOGLEVEL_DEBUG);
		return;
	}

	while(m_out_buffer.GetWriteOffset() > 0) {
		int send_size = m_out_buffer.GetWriteOffset();
		if (send_size > NETLIB_MAX_SOCKET_BUF_SIZE) {
			send_size = NETLIB_MAX_SOCKET_BUF_SIZE;
		}
		sLogMessage("onWrite, remain=%d ", LOGLEVEL_INFO, m_out_buffer.GetWriteOffset());
		int ret = netlib_send(m_handler, m_out_buffer.GetBuffer(), send_size);
		if (ret <= 0) {
			ret = 0;
			break;
		}
		m_out_buffer.Read(NULL, ret);
	}
	sLogMessage("CConnObject::OnWrite END", LOGLEVEL_DEBUG);
}

int CConnObject::SendMsgPdu(CMsgPduBase * pMsgPdu) {
	return Send(pMsgPdu->GetBuffer(), pMsgPdu->GetLength());
}

int CConnObject::Send(void* data, int len) {

	if (m_busy)
	{
		m_out_buffer.Write(data, len);
		return len;
	}
	//会有超过PDU的数据进行传输，这个时候需要将data分段传输
	int offset = 0;
	int remain = len;
	while(remain > 0) {
		int send_size = remain;
		if (send_size > NETLIB_MAX_SOCKET_BUF_SIZE) {
			send_size = NETLIB_MAX_SOCKET_BUF_SIZE;
		}
		int ret = netlib_send(m_handler,  data, len);
		if (ret <= 0) {
			ret = 0;
			break;
		}
		offset += ret;
		remain -= ret;
	}

	if (remain > 0)
	{
		m_out_buffer.Write((void*)data + offset, remain);
		m_busy = true;
		sLogMessage("not send all, remain=%d", LOGLEVEL_INFO, m_out_buffer.GetWriteOffset());
	}
	return len;
}


