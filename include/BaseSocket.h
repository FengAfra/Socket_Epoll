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

	/*********************************
	 * 函数：GetSocket
	 * 功能：获取对象CBaseSocket所含有的socket文件描述符
	 * 参数：
	 * 返回值：
	 *		SOCKET：socket文件描述符
	**********************************/
	SOCKET GetSocket(){return m_socket;}
	/*********************************
	 * 函数：SetSocket
	 * 功能：设置对象CBaseSocket所含有的socket文件描述符
	 * 参数：
	 *		SOCKET fd：socket描述符
	 * 返回值：
	**********************************/
	void SetSocket(SOCKET fd) {m_socket = fd;}
	/*********************************
	 * 函数：SetRemoteAddr
	 * 功能：设置对象CBaseSocket中所要连接的socket的地址
	 * 参数：
	 *		const char* ip：远端socket的IP地址
	 * 返回值：
	**********************************/
	void SetRemoteAddr(const char* ip) {m_remote_ip = ip;}
	/*********************************
	 * 函数：SetRemotePort
	 * 功能：设置对象CBaseSocket中所要连接的socket的端口
	 * 参数：
	 *		const uint16_t port：远端socket的监听端口
	 * 返回值：
	**********************************/
	void SetRemotePort(const uint16_t port) {m_remote_port = port;}
	/*********************************
	 * 函数：SetCallback
	 * 功能：设置对象CBaseSocket中socket所需要做的回调函数
	 * 参数：
	 *		callback_t callback：回调函数，类型是callback_t
	 * 返回值：
	**********************************/
	void SetCallback(callback_t callback) {m_callback = callback;}
	/*********************************
	 * 函数：SetCallbackData
	 * 功能：设置对象CBaseSocket中回调函数的参数
	 * 参数：
	 *		void* callback_data：回调函数的参数
	 * 返回值：
	**********************************/
	void SetCallbackData(void* callback_data) {m_callback_data = callback_data;}
	/*********************************
	 * 函数：SetState
	 * 功能：设置对象CBaseSocket中socket的状态，参看SOCKET_STATE结构体
	 * 参数：
	 *		SOCKET_STATE state：socket所处于的某种状态
	 * 返回值：
	**********************************/
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


