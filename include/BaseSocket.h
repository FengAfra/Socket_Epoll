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


#define BACK_LOG 64

class CBaseSocket;

typedef unordered_map<SOCKET, CBaseSocket*> SocketMap;


class CBaseSocket {

public:

	CBaseSocket();
	~CBaseSocket();

	SOCKET GetSocket();

public:
	static void AddBaseSocket(CBaseSocket* pSocket);

	static void RemoveBaseSocket(CBaseSocket* pSocket);
	
	static CBaseSocket* FindBaseSocket(SOCKET fd);


public:

	/*重写listen函数，主要实现绑定IP端口和回调函数*/
	int Listen(const char* server_ip, const uint16_t server_port, callback_t callback, void* callback_data);

	int Connect();

	int Send();

	int Recv();

	int Close();

	int Accept();

private:
	/*由于监听socket和连接socket都需要设置socketaddr结构体，所以设置private函数，用于绑定sockaddr和ip端口*/
	void _SetAddr(const char* ip, const uint16_t port, struct sockaddr_in& addr);

	void _SetReuseAddr(SOCKET socketfd);
	void _SetNonBlock(SOCKET socketfd);

	
private:

	//每个CBaseSocket对象都有一个socket和回调函数以及回调函数的参数
	SOCKET 		m_socket;
	callback_t	m_callback;
	void*		m_callback_data;
		

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


