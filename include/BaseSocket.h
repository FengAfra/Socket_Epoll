/**
*
*
*
*
*
*
*
*
*
*
*
*
*
*/

#ifndef _BASE_SOCKET_H_
#define _BASE_SOCKET_H_

#include "ostype.h"
#include "util.h"


#define BACK_LOG 64
#define BASESOCKET_ERROR	-1
#define BASESOCKET_OK		0

class CBaseSocket;

typedef unordered_map<SOCKET, CBaseSocket*> SocketMap;

/*
状态1：服务端fd处于监听状态，服务充当服务端。
状态2；客户端fd处于请求连接状态，服务充当客户端
状态3：客户端fd已经连接成功状态，服务充当服务端
*/
enum SOCKET_STATE {
	SOCKET_STATE_LISTENING,
	SOCKET_STATE_CONNECTING,
	SOCKET_STATE_CONNECTED,
	SOCKET_STATE_CLOSING,
};

class CBaseSocket : public CRefObject {

public:

	CBaseSocket();
	~CBaseSocket();

	SOCKET GetSocket(){return m_socket;}
	void SetSocket(SOCKET fd) {m_socket = fd;}
	void SetRemoteAddr(const char* ip) {m_remote_ip = ip;}
	void SetRemotePort(const uint16_t port) {m_remote_port = port;}
	void SetCallback(callback_t callback) {m_callback = callback;}
	void SetCallbackData(void* callback_data) {m_callback_data = callback_data;}
	void SetState(SOCKET_STATE state) {m_state = state;}
	
	void OnRead();
	void OnWrite();
	void OnClose();

public:
	static void AddBaseSocket(CBaseSocket* pSocket);

	static void RemoveBaseSocket(CBaseSocket* pSocket);
	
	static CBaseSocket* FindBaseSocket(SOCKET fd);


public:

	/*重写listen函数，主要实现绑定IP端口和回调函数*/
	int Listen(const char* server_ip, const uint16_t server_port, callback_t callback, void* callback_data);

	SOCKET Connect(const char* remote_ip, const uint16_t remote_port, callback_t callback, void * callback_data);

	int Send(void* buf, int len);

	int Recv(void* buf, int len);

	int Close();


private:
	/*由于监听socket和连接socket都需要设置socketaddr结构体，所以设置private函数，用于绑定sockaddr和ip端口*/
	void _SetAddr(const char* ip, const uint16_t port, struct sockaddr_in& addr);

	void _SetReuseAddr(SOCKET socketfd);
	void _SetNonBlock(SOCKET socketfd);

	void _AcceptNewSocket();

	
private:

	//每个CBaseSocket对象都有一个socket和回调函数以及回调函数的参数
	SOCKET 		m_socket;
	callback_t	m_callback;
	void*		m_callback_data;
	SOCKET_STATE	m_state;
		

	//socket分为监听socket和连接socket
	//连接socket是远程连接的句柄，为远程的IP和端口
	string 		m_remote_ip;
	uint16_t 	m_remote_port;

	//监听socket是本地用于监听端口的socket，为服务端，本地的ip和端口
	string 		m_server_ip;
	uint16_t	m_server_port;

	static SocketMap	g_socket_map;

};


#endif


