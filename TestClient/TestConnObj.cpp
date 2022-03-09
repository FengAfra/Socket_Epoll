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
	sLogMessage("CTestConnObj::OnNewCBFun BEGIN", LOGLEVEL_DEBUG);
	m_handler = fd;
	m_login_time = get_tick_count();
	s_msg_conn_map.insert(make_pair(fd, this));

	netlib_bind(fd, NETLIB_OPT_SET_CALLBACK, (void*)Conn_callback);
	netlib_bind(fd, NETLIB_OPT_SET_CALLBACK_DATA, (void*)this);
	sLogMessage("CTestConnObj::OnNewCBFun END", LOGLEVEL_DEBUG);
}

void CTestConnObj::HandlePdu(CMsgPduBase* pMsgPdu) {

	/*
	1、根据传输过来的类对象pMsgPdu来进行处理，pMsgPdu对象存放了所有的接收数据，包括commondid等消息
	*/

	switch(pMsgPdu->GetCommandId()) {
	case 0:
		break;
	case 1:
		break;
	default:
		sLogMessage("wrong msg, cmd id=%d", LOGLEVEL_INFO, pMsgPdu->GetCommandId());
		_HandlePduTestFun(pMsgPdu);
		break;
	}
}


void CTestConnObj::_HandlePduTestFun(CMsgPduBase * pMsgPdu) {

    sLogMessage("_HandlePduTestFun", LOGLEVEL_INFO);
	SendMsgPdu(pMsgPdu);
}


