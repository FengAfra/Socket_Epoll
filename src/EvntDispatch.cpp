
#include "EventDispatch.h"


/*********************************
 * 函数：CEventDispatch
 * 功能：CEventDispatch构造函数，主要用来create一个epoll
 * 参数：
 * 返回值：
**********************************/
CEventDispatch::CEventDispatch() {

	m_epfd = epoll_create(1);

	if( EVENTDISPATCH_ERROR == m_epfd )
		sLogMessage(" epoll_create failed , error = %d", LOGLEVEL_ERROR, errno);
}

/*********************************
 * 函数：~CEventDispatch
 * 功能：CEventDispatch析构函数，主要用来删除一个epoll描述符
 * 参数：
 * 返回值：
**********************************/
CEventDispatch::~CEventDispatch() {
	close(m_epfd);
}

/*********************************
 * 函数：GetInstance
 * 功能：单例模式场景，返回一个单例模式的实例，该方法使用C++11新特性，线程安全
 * 参数：
 * 返回值：
 *		CEventDispatch&：一个CEventDispatch实例
**********************************/
CEventDispatch& CEventDispatch::GetInstance() {

	/*
	1. 利⽤静态局部变量特性，延迟加载；
	2. 利⽤静态局部变量特性，系统⾃动回收内存，⾃动调⽤析构函数；
	3. 静态局部变量初始化时，没有 new 操作带来的cpu指令reorder操作；
	4. c++11 静态局部变量初始化时，具备线程安全；
	*/
	static CEventDispatch instance;
 	return instance;
}

/*********************************
 * 函数：AddEvent
 * 功能：使用EPOLL_CTL_ADD，将一个socket添加到epoll中
 * 参数：
 *		SOCKET fd：socket文件描述符
 * 返回值：
**********************************/
void CEventDispatch::AddEvent(SOCKET fd) {

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLPRI | EPOLLERR | EPOLLHUP ;
	ev.data.fd = fd;

	if(epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, &ev)) {
		sLogMessage("epoll_ctl EPOLL_CTL_ADD failed , fd = %d, error = %d", LOGLEVEL_ERROR, fd, errno);
	}
}

/*********************************
 * 函数：RemoveEvent
 * 功能：使用EPOLL_CTL_DEL，将一个socket从epoll中移除
 * 参数：
 *		SOCKET fd：socket文件描述符
 * 返回值：
**********************************/
void CEventDispatch::RemoveEvent(SOCKET fd) {

	if(epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, NULL)) {
		sLogMessage("epoll_ctl EPOLL_CTL_DEL failed , fd = %d, error = %d", LOGLEVEL_ERROR, fd, errno);
	}
}

void CEventDispatch::AddTimer(callback_t callback, void* user_data, uint64_t interval) {

	auto iter = m_timer_list.begin();
	for(; iter != m_timer_list.end(); iter ++) {
	
		if((*iter)->callback == callback && (*iter)->callback_data == user_data) {
			(*iter)->interval = interval;
			(*iter)->next_tick = get_tick_count() + interval;
			return;
		}	
	}
	
	TimerItem *timer = new TimerItem;
	timer->callback = callback;
	timer->callback_data = user_data;
	timer->interval = interval;
	timer->next_tick = get_tick_count() + interval;
	m_timer_list.push_back(timer);

}

void CEventDispatch::RemoveTimer(callback_t callback, void* user_data) {
	auto iter = m_timer_list.begin();
	for(;iter != m_timer_list.end(); iter ++) {
	
		if((*iter)->callback == callback && (*iter)->callback_data == user_data) {
			m_timer_list.erase(iter);
			delete (*iter);
			return;
		}	
	}
}


void CEventDispatch::_CheckTimer() {
	uint64_t curr_tick = get_tick_count();
	auto iter = m_timer_list.begin();

	for (; iter != m_timer_list.end(); )
	{
		TimerItem* pItem = *iter;
		iter++;		// iterator maybe deleted in the callback, so we should increment it before callback
		if (curr_tick >= pItem->next_tick)
		{
			pItem->next_tick += pItem->interval;
			pItem->callback(pItem->callback_data, NETLIB_MSG_TIMER, 0);
		}
	}
}

/*********************************
 * 函数：StartDispatch
 * 功能：开始循环，使用epoll_wati开启循环
 * 参数：
 *		int wait_time：循环等待的时间
 * 返回值：
**********************************/
void CEventDispatch::StartDispatch(int wait_time) {

	struct epoll_event event[EPOLL_SIZE] = {0};

	//如果已经有在跑的实例，那就需要返回，避免启动多个Dispatch
	if(m_running)
		return;

	m_running = true;
	
	while(m_running) {

		int nready = epoll_wait(m_epfd, event, EPOLL_SIZE, wait_time);

		for(int i  = 0; i < nready; i ++) {

			SOCKET ev_fd = event[i].data.fd;

			CBaseSocket * pSocket = CBaseSocket::FindBaseSocket(ev_fd);
			if(!pSocket) {
				sLogMessage("FindBaseSocket failed , fd = %d not in g_socket_map", LOGLEVEL_INFO, ev_fd );

				continue;
			}
			
			if(event[i].events & EPOLLIN) {
				sLogMessage("EPOLLIN , fd = %d", LOGLEVEL_INFO, ev_fd );
				pSocket->OnRead();
			}

			if(event[i].events & EPOLLOUT) {
				sLogMessage("EPOLLOUT , fd = %d", LOGLEVEL_INFO, ev_fd );
				pSocket->OnWrite();
			}

			if(event[i].events & ( EPOLLHUP | EPOLLERR | EPOLLPRI ) ) {
				sLogMessage("EPOLLHUP | EPOLLERR | EPOLLPRI , fd = %d", LOGLEVEL_INFO, ev_fd );
				pSocket->OnClose();
			}	
			pSocket->RemoveRef();
		}
		_CheckTimer();
	}

}

/*********************************
 * 函数：StopDispatch
 * 功能：终止循环等待socket事件
 * 参数：
 * 返回值：
**********************************/
void CEventDispatch::StopDispatch() {
	m_running = false;
}


