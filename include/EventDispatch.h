#ifndef _EVENT_DISPATCH_H_
#define _EVENT_DISPATCH_H_

#include "BaseSocket.h"

typedef int EPOLL_HANDLE;
#define EVENTDISPATCH_OK 0
#define EVENTDISPATCH_ERROR -1

#define EPOLL_SIZE  1024



//是reactor的触发器，epoll相关的函数都在此调用
class CEventDispatch {

public:
	
	~CEventDispatch();
	static CEventDispatch& GetInstance(); 

	void StartDispatch(int       wait_time);

	void AddEvent(SOCKET fd);


protected:
	CEventDispatch();


private:
	CEventDispatch(const CEventDispatch&)=default;
	CEventDispatch& operator=(const CEventDispatch&)= default;


private:
	//单例模式，一个程序只有一个epoll
	static CEventDispatch* 	m_pEventDispatch;

	EPOLL_HANDLE 		m_epfd;
	
};





#endif

