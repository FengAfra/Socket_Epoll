
#include "netlib.h"

void connect_server_function(void* callback_data, NETLIB_MSG msg,  SOCKET fd) {

}


int main() {

	netlib_init();

	char serverip[64];
	memset(serverip, 0, sizeof(serverip));
	strcpy(serverip, "127.0.0.1");
	uint16_t serverport = 6781;
	
	netlib_connect(serverip, serverport, connect_server_function, NULL);
	
	netlib_eventloop(-1);
	
	return 0;


}



