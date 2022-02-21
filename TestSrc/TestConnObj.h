#ifndef _TEST_CONN_OBJ_H_
#define _TEST_CONN_OBJ_H_

#include "ConnObject.h"
#include "netlib.h"

class CTestConnObj : public CConnObject {
public:
	CTestConnObj();
	virtual ~CTestConnObj();

	virtual void OnNewCBFun(SOCKET fd);
	virtual void HandlePdu(CMsgPduBase* pMsgPdu);

	void SetHandler(SOCKET handler) {m_handler = handler;}

private:
	void _HandlePduTestFun(CMsgPduBase* pMsgPdu);
private:
	/*
	1、继承基类的SOCKET，用来绑定SOCKET。通过SOCKET来贯穿CBaseSocket，CEventDispatch，netlib，和CConnObject
	SOCKET	m_handler;
	2、静态成员变量，保存的是系统所有的SOCKET与CConnObject对象的链接
	static ConnMap_t	s_msg_conn_map;
	*/

	//记录客户端登录时间
	uint64_t	m_login_time;
	
};



#endif

