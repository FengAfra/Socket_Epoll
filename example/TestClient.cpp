
#include "netlib.h"
#include "TestConnObj.h"

void* ThreadFun(void* arg) {

	CTestConnObj* pTestConnobj = (CTestConnObj*)arg;

	netlib_init();
	
	char serverip[64];
	memset(serverip, 0, sizeof(serverip));
	strcpy(serverip, "127.0.0.1");
	uint16_t serverport = 6781;
	
	SOCKET handler = netlib_connect(serverip, serverport, CConnObject::Conn_callback, NULL);

	
	CTestConnObj::s_msg_conn_map.insert(make_pair(handler, pTestConnobj));

	netlib_eventloop(-1);

}


int main() {

	CTestConnObj* pTestConnobj = new CTestConnObj();

	pthread_t thread_id;
	pthread_create(&thread_id, NULL, ThreadFun, (void*)pTestConnobj);

	char Msg[10];
	strcpy(Msg, "123123");
	cout<<strlen(Msg)<<endl;
	CMsgPduBase* pMsgPdu = new CMsgPduBase();
	pMsgPdu->m_buf.Write(Msg, strlen(Msg));
	pTestConnobj->HandlePdu(pMsgPdu);
	while(true);


	/*
	while(true) {
		string Msg;
		char quit;
		getline(cin,Msg);
		quit = Msg.at(0);
		if(quit == 'q' || quit == 'Q')
			return 0;
	
		cout<<Msg<<Msg.length()<<endl;
		pTestConnobj->m_out_buffer.Write((void*)Msg.c_str(), Msg.length());
		//cout<<Msg<<endl;
	}
	*/
	return 0;


}



