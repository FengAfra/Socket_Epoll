#ifndef _CONN_OBJECT_H_
#define _CONN_OBJECT_H_

#include "ostype.h"
#include "util.h"
#include "VarLenPdu.h"
#include "netlib.h"
#include "MsgPduBase.h"

class CConnObject;

typedef unordered_map<SOCKET, CConnObject*> ConnMap_t;

#define READ_BUF_SIZE	2048

class CConnObject : public CRefObject {

public:
	CConnObject();
	virtual ~CConnObject();
	virtual int SendMsgPdu(CMsgPduBase* pMsgPdu);
	virtual int Send(void* data, int len);
	virtual void OnNewCBFun(SOCKET fd) {m_handler = fd;} 
	virtual void OnConfirm();
	virtual void OnWrite();
	virtual void OnRead();
	virtual void OnClose();
	virtual void OnTimer();
	//OnRead之后可以通过此函数进行接收消息的处理，并组装发送消息
	virtual void HandlePdu(CMsgPduBase* pMsgPdu) {}

	static void Conn_callback (void* callback_data, NETLIB_MSG msg,  SOCKET fd);

	static ConnMap_t	s_msg_conn_map;

protected:

	SOCKET				m_handler;
	CValLenPdu			m_in_buffer;
	CValLenPdu			m_out_buffer;
	bool				m_busy;

};


#endif

