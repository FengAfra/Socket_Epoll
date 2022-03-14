
#include "netlib.h"
#include "TestConnObj.h"

void* ThreadFun(void* arg) {

	CTestConnObj* pTestConnobj = new CTestConnObj();

	char serverip[64];
	memset(serverip, 0, sizeof(serverip));
	strcpy(serverip, "192.168.217.141");
	uint16_t serverport = 6781;

	char Msg[30];
	strcpy(Msg, "http://www.cmsoft.cn QQ:10865600");
	cout<<strlen(Msg)<<endl;
	CMsgPduBase* pMsgPdu = new CMsgPduBase();
	pMsgPdu->m_buf.Write(Msg, strlen(Msg));



	SOCKET handler = netlib_connect(serverip, serverport, CConnObject::Conn_callback, NULL);
	pTestConnobj->SetHandler(handler);
	CTestConnObj::s_msg_conn_map.insert(make_pair(handler, pTestConnobj));
	sleep(5);
	pTestConnobj->HandlePdu(pMsgPdu);
	while(true);
	return NULL;
}


int main() {

	
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, ThreadFun, NULL);
	netlib_init();
	netlib_eventloop(-1);

	while(true);


	return 0;


}



