
#include "EventDispatch.h"


CEventDispatch::CEventDispatch() {

	m_epfd = epoll_create(1);

	if( EVENTDISPATCH_ERROR == m_epfd )
		sLogMessage(" epoll_create failed , error = %d", LOGLEVEL_ERROR, errno);
}

CEventDispatch::~CEventDispatch() {
	close(m_epfd);
}

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

void CEventDispatch::AddEvent(SOCKET fd) {

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLPRI | EPOLLERR | EPOLLHUP ;
	ev.data.fd = fd;

	if(epoll_ctl(m_epfd, EPOLL_CTL_ADD, fd, &ev)) {
		sLogMessage("epoll_ctl EPOLL_CTL_ADD failed , fd = %d, error = %d", LOGLEVEL_ERROR, fd, errno);
	}
}


void CEventDispatch::RemoveEvent(SOCKET fd) {

	if(epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, NULL)) {
		sLogMessage("epoll_ctl EPOLL_CTL_DEL failed , fd = %d, error = %d", LOGLEVEL_ERROR, fd, errno);
	}
}


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

		}
	
	}

}


void CEventDispatch::StopDispatch() {
	m_running = false;
}


