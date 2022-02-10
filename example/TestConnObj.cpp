#include "TestConnObj.h"

CTestConnObj::CTestConnObj() {


}

CTestConnObj::~CTestConnObj() {

}


void CTestConnObj::OnNewCBFun(SOCKET fd) {
	/*
	1、重新绑定回调参数，
		1.1、如果是server端，则是当新连接到来的时候，通过accept函数来触发此函数
		1.2、如果是client端，则可能是连接完成给的回调，待定
	*/
	m_handler = fd;
	m_login_time = get_tick_count();
	s_msg_conn_map.insert(make_pair(fd, this));

	netlib_bind(fd, NETLIB_OPT_SET_CALLBACK, (void*)Conn_callback);
	netlib_bind(fd, NETLIB_OPT_SET_CALLBACK_DATA, (void*)this);
}





