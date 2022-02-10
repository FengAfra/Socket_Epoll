
#include "ConnObject.h"

CConnObject::CConnObject(){

}

CConnObject::~CConnObject() {


}

int CConnObject::SendPdu(){

	return 0;
}


void CConnObject::Conn_callback(void * callback_data, NETLIB_MSG msg, SOCKET fd) {

	CConnObject pConnobj = s_msg_conn_map.find(fd);
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
	default:
		sLogMessage("!!!imconn_callback error msg: %d  failed", LOGLEVEL_ERROR, fd);
		break;
	}
}

void CConnObject::OnClose() {

}

void CConnObject::OnTimer() {

}

void CConnObject::OnConfirm() {

}

void CConnObject::OnRead() {

}

void CConnObject::OnWrite() {

}



