
#include "netlib.h"
#include "TestConnObj.h"

void listen_accept_new_client(void* callback_data, NETLIB_MSG msg,  SOCKET fd) {
	sLogMessage("listen_accept_new_client begin", LOGLEVEL_DEBUG);
	if(msg == NETLIB_MSG_CONNECT) {
		CTestConnObj* pTestConnobj = new CTestConnObj();
		pTestConnobj->OnNewCBFun(fd);
	}
	else {
		sLogMessage("!!!error msg: %d ", LOGLEVEL_ERROR, msg);
	}
	sLogMessage("listen_accept_new_client end", LOGLEVEL_DEBUG);
}

int main() {

	netlib_init();

	char serverip[64];
	memset(serverip, 0, sizeof(serverip));
	strcpy(serverip, "192.168.217.141");
	uint16_t serverport = 6781;
	
	netlib_listen(serverip, serverport, listen_accept_new_client, NULL);
	netlib_eventloop(-1);
	
	return 0;

}



