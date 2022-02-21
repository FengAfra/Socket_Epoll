#ifndef _EVENT_DISPATCH_H_
#define _EVENT_DISPATCH_H_

#include "BaseSocket.h"

typedef int EPOLL_HANDLE;
#define EVENTDISPATCH_OK 0
#define EVENTDISPATCH_ERROR -1

#define EPOLL_SIZE  1024

typedef struct _TmierItem {
	callback_t	callback;
	void*		callback_data;
	uint64_t	interval;
	uint64_t	next_tick;
}TimerItem;

//是reactor的触发器，epoll相关的函数都在此调用
//需要添加定时器，定时的向已有的fd中发送心跳包，确定fd是否存活
class CEventDispatch {

public:
	
	~CEventDispatch();
	static CEventDispatch& GetInstance(); 

	void StartDispatch(int       wait_time);
	void StopDispatch();

	void AddEvent(SOCKET fd);
	void RemoveEvent(SOCKET fd);

	void AddTimer(callback_t callback, void* user_data, uint64_t interval);
	void RemoveTimer(callback_t callback, void* user_data);

protected:
	CEventDispatch();


private:
	void _CheckTimer();

private:
	CEventDispatch(const CEventDispatch&)=default;
	CEventDispatch& operator=(const CEventDispatch&)= default;


private:
	//单例模式，一个程序只有一个epoll
	static CEventDispatch* 	m_pEventDispatch;
	EPOLL_HANDLE 		m_epfd;
	bool 				m_running;

	list<TimerItem*>	m_timer_list;
	
};





#endif

