#include "BaseSocket.h"
#include "EventDispatch.h"


CSLog g_demolog = CSLog(false);


CBaseSocket::CBaseSocket() {

}

CBaseSocket::~CBaseSocket() {

}


/*********************************
 * 函数：OnRead
 * 功能：对外提供，如果socket有read事件之后，需要调用此函数，来确定是处理新连接还是读事件，主要是epoll用来调用
 * 参数：
 * 返回值：
**********************************/
void CBaseSocket::OnRead() {

	if( m_state == SOCKET_STATE_LISTENING) {
		//如果状态机是SOCKET_STATE_LISTENING，说明响应的是监听socket，此时是接收新连接
	}
	else {
		//如果状态机不是SOCKET_STATE_LISTENING，说明响应的是连接socket，是客户端数据到达
		u_long avail = 0;
		int ret = ioctl(m_socket, FIONREAD, &avail);
		if ((BASESOCKET_ERROR == ret) || (avail == 0)) {
			m_callback(m_callback_data, NETLIB_MSG_CLOSE, m_socket);
		}
		else {
			m_callback(m_callback_data, NETLIB_MSG_READ, m_socket);
		}
	}

}


/*********************************
 * 函数：OnWrite
 * 功能：对外提供，如果socket有write事件之后，需要调用此函数，来向对手socket发送消息，主要是epoll用来调用
 * 参数：
 * 返回值：
**********************************/
void CBaseSocket::OnWrite() {
	CEventDispatch::GetInstance().RemoveEvent(m_socket);

	if( m_state == SOCKET_STATE_CONNECTING) {
		int error = 0;
		socklen_t len = sizeof(error);

		getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (void*)&error, &len);
		if (error) {
			m_callback(m_callback_data, NETLIB_MSG_CLOSE, m_socket);
		} else {
			m_state = SOCKET_STATE_CONNECTED;
			m_callback(m_callback_data, NETLIB_MSG_CONFIRM, m_socket);
		}
	}
	else {
		m_callback(m_callback_data, NETLIB_MSG_WRITE, m_socket);
	}
}

/*********************************
 * 函数：OnClose
 * 功能：对外提供，如果socket有close事件之后，需要调用此函数，来改变状态，主要是epoll用来调用，并且，需要回调函数，将业务处理结束，然后才能关闭
 * 参数：
 * 返回值：
**********************************/
void CBaseSocket::OnClose() {
	m_state = SOCKET_STATE_CLOSING;
	m_callback(m_callback_data, NETLIB_MSG_CLOSE, m_socket);
}


/***************************重写socket的函数**********************************/
/*********************************
 * 函数：Listen
 * 功能：对外提供，用来绑定监听socket的IP，端口，回调函数以及回调函数的参数
 * 参数：
 *		const char* server_ip：监听socket的IP
 *		const uint16_t server_port：监听socket的端口
 *		callback_t callback：accept之后的回调函数
 *		void* callback_data：回调函数的参数
 * 返回值：
 *		成功：0
 *		失败：-1
**********************************/
int CBaseSocket::Listen(const char* server_ip, const uint16_t server_port, callback_t callback, void* callback_data) {

	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(!m_socket)
		return NETLIB_ERROR;


	m_server_ip = server_ip;
	m_server_port = server_port;
	m_callback = callback;
	m_callback_data = callback_data;

	struct sockaddr_in serv_addr;
	_SetAddr(server_ip, server_port, serv_addr);

	//bind函数，将sockaddr结构体与socket绑定，如果失败，释放socket
	int ret = bind(m_socket, (const struct sockaddr*) &serv_addr, sizeof(serv_addr));
	if(ret) {
		sLogMessage("bind failed, ip = %s, port = %u, error = %d", LOGLEVEL_ERROR, server_ip, server_port, errno);
		close(m_socket);
		return NETLIB_ERROR;
	}

	//设置socket的属性
	_SetReuseAddr(m_socket);
	_SetNonBlock(m_socket);


	//开始监听端口，接收fd连接
	ret = listen(m_socket, BACK_LOG);
	if(ret) {
		sLogMessage("listen failed, ip = %s, port = %u, error = %d", LOGLEVEL_ERROR, server_ip, server_port, errno);
		close(m_socket);
		return NETLIB_ERROR;
	}

	//socket状态机，其中监听socket只可能处于一个状态，就是SOCKET_STATE_LISTENING
	m_state = SOCKET_STATE_LISTENING;
		
	sLogMessage("CBaseSocket::Listen on ip = %s, port = %u, socket = %d", LOGLEVEL_INFO, server_ip, server_port, m_socket);

	/*
	1、将当前fd添加到unorder_map结构体中，使eventdispatch通过fd找到相关的CBaseSocket对象。
	2、将当前fd添加到CEventDispatch的epoll中，可以监听到fd。
	*/
	AddBaseSocket(this);
	CEventDispatch::GetInstance().AddEvent(m_socket);
	
	return NETLIB_OK;
	
}

/*********************************
 * 函数：Connect
 * 功能：对外提供，当作客户端向服务端发起连接请求
 * 参数：
 *		const char * remote_ip：远端socket的IP
 *		const uint16_t remote_port：远端socket的端口
 *		callback_t callback：远端回应请求之后的回调函数
 *		void* callback_data：回调函数的参数
 * 返回值：
 *		成功：SOCKET文件描述符
 *		失败：-1
**********************************/
SOCKET CBaseSocket::Connect(const char * remote_ip, const uint16_t remote_port, callback_t callback, void * callback_data) {

	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(!m_socket)
		return NETLIB_ERROR;


	m_remote_ip = remote_ip;
	m_remote_port = remote_port;
	m_callback = callback;
	m_callback_data = callback_data;

	struct sockaddr_in remote_addr;
	_SetAddr(remote_ip, remote_port, remote_addr);

	//bind函数，将sockaddr结构体与socket绑定，如果失败，释放socket
	int ret = connect(m_socket, (const struct sockaddr*) &remote_addr, sizeof(remote_addr));
	if(ret) {
		sLogMessage("bind failed, ip = %s, port = %u, error = %d", LOGLEVEL_ERROR, remote_ip, remote_port, errno);
		close(m_socket);
		return NETLIB_ERROR;
	}

	//设置socket的属性
	_SetReuseAddr(m_socket);
	_SetNonBlock(m_socket);

	//socket状态机，其中请求连接socket当前处于此状态，就是SOCKET_STATE_CONNECTING
	m_state = SOCKET_STATE_CONNECTING;
		
	sLogMessage("CBaseSocket::Listen on ip = %s, port = %u, socket = %d", LOGLEVEL_INFO, remote_ip, remote_port, m_socket);

	/*
	1、将当前fd添加到unorder_map结构体中，使eventdispatch通过fd找到相关的CBaseSocket对象。
	2、将当前fd添加到CEventDispatch的epoll中，可以监听到fd。
	*/
	AddBaseSocket(this);
	CEventDispatch::GetInstance().AddEvent(m_socket);
	
	return m_socket;
}

/*********************************
 * 函数：Recv
 * 功能：对外提供，用来接收客户端发送消息
 * 参数：
 *		void * buf：接收的消息内容
 *		int len：接收的消息长度
 * 返回值：
 *		成功：接收的消息长度
 *		失败：0
**********************************/
int CBaseSocket::Recv(void * buf, int len) {
	return recv(m_socket, buf, len, 0);
}

/*********************************
 * 函数：Send
 * 功能：对外提供，用来向客户端发送消息
 * 参数：
 *		void * buf：发送的消息内容
 *		int len：发送的消息长度
 * 返回值：
 *		成功：发送的消息长度
 *		失败：0
**********************************/
int CBaseSocket::Send(void * buf, int len) {
	/*
	1、判断socket的状态，是否是处于链接中的状态
	if(m_state != SOCKET_STATE_CONNECTED) {
		return BASESOCKET_ERROR;
	}
	2、发送消息，并对返回值进行校验
		2.1、如果发送成功，皆大欢喜
		2.2、如果发送失败，需要将此socket加入到发送队列，也就是CEventDispatch循环中
	*/

	int ret = send(m_socket, buf, len, 0);
	if(ret < 0){
		if(errno == EAGAIN || errno == EWOULDBLOCK) {
			sLogMessage("send failed, errno in EAGAIN,EWOULDBLOCK, socket = %d, buf = '%s'", LOGLEVEL_INFO, m_socket, buf);
			CEventDispatch::GetInstance().AddEvent(m_socket);
			ret = 0;
		}
		else {
			sLogMessage("send failed, errno is %d,  socket = %d, buf = '%s'", LOGLEVEL_INFO, errno,  m_socket, buf);
		}
			
	}
	return ret;
}


/*********************************
 * 函数：Close
 * 功能：对外提供，关闭socket
 * 参数：
 * 返回值：
 *		成功：0
 *		失败：-1
**********************************/
int CBaseSocket::Close() {
	/*
	1、从eventdispatch循环中将此fd删除，避免多余的fd在epoll中
	2、从g_socket_map容器中删除这个fd。可以不用使用这个fd
	*/
	CEventDispatch::GetInstance().RemoveEvent(m_socket);
	RemoveBaseSocket(this);
	close(m_socket);
	RemoveRef();
	return BASESOCKET_OK;
}

/***************************内部函数，private类型的，主要是类内部使用，不对外部开放**********************************/

/*********************************
 * 函数：_SetAddr
 * 功能：内部函数，用来绑定IP，端口和const sockaddr_in结构体
 * 参数：
 *		const char * ip：需要绑定的IP
 *		const uint16_t port：需要绑定的端口
 *		struct sockaddr_in & addr：需要绑定的结构体
 * 返回值：
**********************************/
void CBaseSocket::_SetAddr(const char * ip, const uint16_t port, struct sockaddr_in & addr) {
	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);
	
	//如果IP是域名的话，需要通过gethostbyname来将域名与IP对应上，实现绑定
	if(addr.sin_addr.s_addr == INADDR_ANY) {

		struct hostent *host = gethostbyname(ip);
		if(host == NULL) {
			sLogMessage("gethostbyname failed, ip = %s, port = %u", LOGLEVEL_ERROR, ip, port);
			return;
		}

		addr.sin_addr.s_addr = *(uint32_t*)host->h_addr;
	}
	

}

/*********************************
 * 函数：_SetReuseAddr
 * 功能：内部函数，设置此socket可重用
 * 参数：
 *		SOCKET socketfd：socket文件描述符
 * 返回值：
**********************************/
void CBaseSocket::_SetReuseAddr(SOCKET socketfd) {

	int reuse = 1;

	int ret = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
	if(ret) {
		sLogMessage("setsockopt Reuse failed, error = %d, socket = %d", LOGLEVEL_ERROR, errno, socketfd);
	}
}

/*********************************
 * 函数：_SetNonBlock
 * 功能：内部函数，设置此socket非阻塞
 * 参数：
 *		SOCKET socketfd：socket文件描述符
 * 返回值：
**********************************/
void CBaseSocket::_SetNonBlock(SOCKET socketfd) {

	/*
	1、获取文件状态标识
	2、设置文件状态标识
	*/
	int flag = fcntl(socketfd, F_GETFL);
	
	flag = flag | O_NONBLOCK;
	
	if( NETLIB_ERROR == fcntl(socketfd, F_SETFL, flag)){
		sLogMessage("fcntl O_NONBLOCK failed, error = %d, socket = %d", LOGLEVEL_ERROR, errno, socketfd);

	}
}

/*********************************
 * 函数：_AcceptNewSocket
 * 功能：内部函数，接收一个新的客户端连接
 * 参数：
 * 返回值：
**********************************/
void CBaseSocket::_AcceptNewSocket() {

	struct sockaddr_in clientaddr;
	memset(&clientaddr, 0, sizeof(clientaddr));
	socklen_t client_len = sizeof(clientaddr);
	SOCKET client_fd = 0;
	char ip_str[64] = {0};
	/*
	BaiDu
	1、这里为什么不用if，要用while，一直循环
	*/
	while ( (client_fd = accept(m_socket, (struct sockaddr *)& clientaddr, &client_len)) != BASESOCKET_ERROR ) {
		CBaseSocket *client_Socket = new CBaseSocket();

		uint32_t ip = ntohl(clientaddr.sin_addr.s_addr);
		uint16_t port = ntohs(clientaddr.sin_port);

		//点分十进制法，将IP地址输出
		snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);

		
		sLogMessage("AcceptNewSocket, socket=%d from %s:%d\n", LOGLEVEL_INFO, client_fd, ip_str, port);

		//设置正在连接的客户端socket的一系列属性
		client_Socket->SetSocket(client_fd);
		client_Socket->SetRemoteAddr(ip_str);
		client_Socket->SetRemotePort(port);
		client_Socket->SetCallback(m_callback);
		client_Socket->SetCallbackData(m_callback_data);
		client_Socket->SetState(SOCKET_STATE_CONNECTED);

		
		//设置socket的属性
		_SetReuseAddr(client_fd);
		_SetNonBlock(client_fd);

		/*
		1、将连接的client_fd添加到unorder_map结构体中，使eventdispatch通过fd找到相关的CBaseSocket对象。
		2、将连接的client_fd添加到CEventDispatch的epoll中，可以监听到fd。
		*/
		AddBaseSocket(client_Socket);
		CEventDispatch::GetInstance().AddEvent(client_fd);

		/*
		BaiDu：
		1、连接到新的客户端client之后，需要重新绑定新的回调函数，并设置NETLIB的状态机，当前处于connected状态
		*/
		m_callback( m_callback_data , NETLIB_MSG_CONNECT, client_fd);

	}

}

/***************************static 静态函数，主要负责对静态数据unorder_map结构体的处理**********************************/
/*********************************
 * 函数：AddBaseSocket
 * 功能：static函数，对静态数据成员g_socket_map进行添加，保存g_socket_map的socket和CBaseSocket的对应关系
 * 参数：
 *		CBaseSocket* pSocket：CBaseSocekt对象
 * 返回值：
**********************************/
void CBaseSocket::AddBaseSocket(CBaseSocket* pSocket){
	g_socket_map.insert(make_pair(pSocket->GetSocket(), pSocket));
}

/*********************************
 * 函数：RemoveBaseSocket
 * 功能：static函数，对静态数据成员g_socket_map进行移除，保存g_socket_map的socket和CBaseSocket的对应关系
 * 参数：
 *		CBaseSocket* pSocket：CBaseSocekt对象
 * 返回值：
**********************************/
void CBaseSocket::RemoveBaseSocket(CBaseSocket* pSocket) {

	g_socket_map.erase(pSocket->GetSocket());

}

/*********************************
 * 函数：FindBaseSocket
 * 功能：static函数，对静态数据成员g_socket_map进行查找，查找socket文件描述符对应的CBaseSocket对象
 * 参数：
 *		SOCKET fd：socket文件描述符
 * 返回值：
**********************************/
CBaseSocket* CBaseSocket::FindBaseSocket(SOCKET fd) {
	CBaseSocket* pSocket = NULL;

	auto iter = g_socket_map.find(fd);
	if(iter !=  g_socket_map.end()) {
		pSocket = iter->second;
		pSocket->AddRef();
	}

	return pSocket;
}




