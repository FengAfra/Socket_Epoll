#ifndef _CONN_OBJECT_H_
#define _CONN_OBJECT_H_

#include "ostype.h"
#include "util.h"

class CConnObject;

typedef unordered_map<SOCKET, CConnObject*> ConnMap_t;



class CConnObject : public CRefObject {

public:
	CConnObject();
	virtual ~CConnObject();
	virtual void OnNewCBFun(SOCKET fd) {m_handler = fd;} 
	virtual void OnConfirm();
	virtual void OnWrite();
	virtual void OnRead();
	virtual void OnClose();
	virtual void OnTimer();
	virtual int SendPdu();

	static void Conn_callback (void* callback_data, NETLIB_MSG msg,  SOCKET fd);
	
private:
	SOCKET		m_handler;

	static ConnMap_t	s_msg_conn_map;
};


#endif

