#include "netlib.h"
#include "EventDispatch.h"
int netlib_init() {

	return NETLIB_OK;

}




void netlib_eventloop(int wait_time) {

	CEventDispatch::GetInstance().StartDispatch(wait_time);
}

void netlib_stop_eventloop() {

	CEventDispatch::GetInstance().StopDispatch();
}



int netlib_bind(SOCKET fd, NETLIB_OPT opt, void* data) {

	if(data == NULL) {
		sLogMessage("data is NULL, socket=%d\n", LOGLEVEL_ERROR);
		return NETLIB_ERROR;
	}

	CBaseSocket *pSocket = CBaseSocket::FindBaseSocket(fd);
	if(!pSocket){
		sLogMessage("FindBaseSocket, socket=%d from map\n", LOGLEVEL_ERROR, fd);
		return NETLIB_ERROR;
	}

	switch (opt) {
	case NETLIB_OPT_SET_CALLBACK:
		pSocket->SetCallback((callback_t)data);
		break;
	case NETLIB_OPT_SET_CALLBACK_DATA:
		pSocket->SetCallbackData(data);
		break;
	default:
		sLogMessage("opt is error, socket=%d\n", LOGLEVEL_ERROR, fd);
		return NETLIB_ERROR;
	}
	return NETLIB_OK;
}


int netlib_listen(const char* server_ip, const uint16_t server_port, callback_t callback, void* callback_data) {

	CBaseSocket* pSocket = new CBaseSocket();
	if(!pSocket)
		return NETLIB_ERROR;
	

	if( NETLIB_ERROR == pSocket->Listen(server_ip, server_port, callback, callback_data) ) {
		delete pSocket;
		return NETLIB_ERROR;
	}

	return NETLIB_OK;
}

int netlib_recv(SOCKET fd, void * recvBuf, int len) {

	CBaseSocket* pSocket = CBaseSocket::FindBaseSocket(fd);
	if(!pSocket){
		sLogMessage("FindBaseSocket, socket=%d from map\n", LOGLEVEL_ERROR, fd);
		return NETLIB_ERROR;
	}

	int ret = pSocket->Recv(recvBuf, len);
	return ret;
}

int netlib_send(SOCKET fd, void * sendbuf, int len) {

	CBaseSocket* pSocket = CBaseSocket::FindBaseSocket(fd);
	if(!pSocket){
		sLogMessage("FindBaseSocket, socket=%d from map\n", LOGLEVEL_ERROR, fd);
		return NETLIB_ERROR;
	}

	int ret = pSocket->Send(sendbuf, len);
	
	return ret;
}

