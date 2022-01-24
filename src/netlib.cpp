#include "netlib.h"

int netlib_init() {

	return NETLIB_OK;

}

int netlib_listen(const char* server_ip, const uint16_t server_port, callback_t callback, void* callback_data) {

	CBaseSocket* pSocket = new CBaseSocket();
	if(!pSocket)
		return NETLIB_ERROR;
	

	if( NETLIB_ERROR == pSocket.Listen(server_ip, server_port, callback, callback_data) ) {
		delete pSocket;
		return NETLIB_ERROR;
	}

	return NETLIB_OK;
}





