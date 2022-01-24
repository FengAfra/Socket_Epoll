
#include "BaseSocket.h"


CBaseSocket::CBaseSocket() {

}

CBaseSocket::~CBaseSocket() {

}

int CBaseSocket::Listen(const char* server_ip, const uint16_t server_port, callback_t callback, void* callback_data) {

	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(!m_socket)
		return NETLIB_ERROR;


	m_server_ip = server_ip;
	m_server_port = server_port;
	m_callback = callback;
	m_callback_data = callback_data;

	struct sockaddr_in serv_addr;
	_SetAddr(m_server_ip, server_port, serv_addr);

	//bind函数，将sockaddr结构体与socket绑定，如果失败，释放socket
	int ret = bind(m_socket, (const struct sockaddr*) &serv_addr, sizeof(serv_addr));
	if(ret) {
		sLogMessage("bind failed, ip = %s, port = %u, error = %d", LOGLEVEL_ERROR, ip, port, errno);
		close(socket);
		return NETLIB_ERROR;
	}

	//设置socket的属性
	_SetReuseAddr(m_socket);
	_SetNonBlock(m_socket);

	ret = listen(m_socket, BACK_LOG);
	if(ret) {
		sLogMessage("listen failed, ip = %s, port = %u, error = %d", LOGLEVEL_ERROR, ip, port, errno);
		close(socket);
		return NETLIB_ERROR;
	}
	
	sLogMessage("CBaseSocket::Listen on ip = %s, port = %u, socket = %d", LOGLEVEL_INFO, ip, port, m_socket);

	return NETLIB_OK;
	
}

/**
BaiDu：
1、hostent结构体是什么
2、什么时候inet_addr(ip)会变成INADDR_ANY
*/
void CBaseSocket::_SetAddr(const char * ip, const uint16_t port, sockaddr_in & addr) {
	memset(&addr, 0, sizeof(&addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);
	
	//如果IP是域名的话，需要通过gethostbyname来将域名与IP对应上，实现绑定
	if(addr.sin_addr.s_addr == INADDR_ANY) {

		struct hostent *host = gethostbyname(ip);
		if(host == NULL) {
			sLogMessage("gethostbyname failed, ip = %s, port = %u", LOGLEVEL_ERROR, ip, port);
			return NETLIB_ERROR;
		}

		addr.sin_addr.s_addr = host->h_addr;
	}
	

}


void CBaseSocket::_SetReuseAddr(SOCKET socketfd) {

	int reuse = 1;

	int ret = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
	if(ret) {
		sLogMessage("setsockopt Reuse failed, error = %d, socket = %d", LOGLEVEL_ERROR, error, socketfd);
	}
}

void CBaseSocket::_SetNonBlock(SOCKET socketfd) {

	/*
	1、获取文件状态标识
	2、设置文件状态标识
	*/
	int flag = fcntl(socketfd, F_GETFL);
	
	flag = flag | O_NONBLOCK;
	
	if( NETLIB_ERROR == fcntl(socketfd, F_SETFL, flag)){
		sLogMessage("fcntl O_NONBLOCK failed, error = %d, socket = %d", LOGLEVEL_ERROR, error, socketfd);

	}
}






