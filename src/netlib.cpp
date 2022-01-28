#include "netlib.h"
#include "EventDispatch.h"


/*********************************
 * 函数：netlib_init
 * 功能：netlib_init初始化
 * 参数：
 * 返回值：
 *		成功：0
 *		失败：-1
**********************************/
int netlib_init() {

	return NETLIB_OK;
}

/*********************************
 * 函数：netlib_bind
 * 功能：绑定参数，主要用于绑定CBaseSocket的内部信息，比如回调函数，回调函数参数等
 * 参数：
 *		SOCKET fd：socket文件描述符
 *		NETLIB_OPT opt：需要绑定的类型是什么，参看enum NETLIB_OPT
 *		void* data：需要绑定的值是什么
 * 返回值：
 *		成功：0
 *		失败：-1
**********************************/
int netlib_bind(SOCKET fd, NETLIB_OPT opt, void* data) {

	if(data == NULL) {
		sLogMessage("data is NULL, socket=%d\n", LOGLEVEL_ERROR);
		return NETLIB_ERROR;
	}

	CBaseSocket *pSocket = CBaseSocket::FindBaseSocket(fd);
	if(!pSocket){
		sLogMessage("FindBaseSocket, socket=%d from map\n", LOGLEVEL_ERROR, fd);
		return NETLIB_ERROR;
	}

	switch (opt) {
	case NETLIB_OPT_SET_CALLBACK:
		pSocket->SetCallback((callback_t)data);
		break;
	case NETLIB_OPT_SET_CALLBACK_DATA:
		pSocket->SetCallbackData(data);
		break;
	default:
		sLogMessage("opt is error, socket=%d\n", LOGLEVEL_ERROR, fd);
		return NETLIB_ERROR;
	}
	pSocket->RemoveRef();
	return NETLIB_OK;
}


int netlib_add_timer(callback_t callback, void *user_data, uint64_t interval) {
	CEventDispatch::GetInstance().AddTimer(callback, user_data, interval);
	return NETLIB_OK;
}

int netlib_remove_timer(callback_t callback, void *user_data) {
	CEventDispatch::GetInstance().RemoveTimer(callback, user_data);
	return NETLIB_OK;
}


/*********************************
 * 函数：netlib_listen
 * 功能：监听socket创建，业务层调用
 * 参数：
 *		const char* server_ip：监听socket的IP
 *		const uint16_t server_port：监听socket的端口
 *		callback_t callback：accept之后的回调函数
 *		void* callback_data：回调函数的参数
 * 返回值：
 *		成功：0
 *		失败：-1
**********************************/
int netlib_listen(const char* server_ip, const uint16_t server_port, callback_t callback, void* callback_data) {

	CBaseSocket* pSocket = new CBaseSocket();
	if(!pSocket)
		return NETLIB_ERROR;
	

	if( NETLIB_ERROR == pSocket->Listen(server_ip, server_port, callback, callback_data) ) {
		delete pSocket;
		return NETLIB_ERROR;
	}

	return NETLIB_OK;
}

/*********************************
 * 函数：netlib_connect
 * 功能：业务调用，主要用来当作客户端去申请连接服务端
 * 参数：
 *		const char* ip：远端的socket的IP
 *		const uint16_t port：远端的socket的端口
 *		callback_t callback：远端响应的回调函数
 *		void* callback_data：回调函数的参数
 * 返回值：
 *		成功：SOCKET文件描述符
 *		失败：-1
**********************************/
SOCKET netlib_connect(const char * ip, uint16_t port, callback_t callback, void * callback_data) {

	CBaseSocket *pSocket = new CBaseSocket();
	if(!pSocket)
		return NETLIB_ERROR;

	SOCKET fd = pSocket->Connect(ip, port, callback, callback_data);
	if(NETLIB_ERROR == fd) {
		delete pSocket;
	}
	
	return fd;
}

/*********************************
 * 函数：netlib_recv
 * 功能：业务调用，当需要接受消息的时候使用
 * 参数：
 *		SOCKET fd：接收消息的socket描述符
 *		void * recvBuf：接收消息存放的字符串
 *		int len：接收消息的长度
 * 返回值：
 *		成功：0
 *		失败：-1
**********************************/
int netlib_recv(SOCKET fd, void * recvBuf, int len) {

	CBaseSocket* pSocket = CBaseSocket::FindBaseSocket(fd);
	if(!pSocket){
		sLogMessage("FindBaseSocket, socket=%d from map\n", LOGLEVEL_ERROR, fd);
		return NETLIB_ERROR;
	}

	int ret = pSocket->Recv(recvBuf, len);
	pSocket->RemoveRef();
	return ret;
}

/*********************************
 * 函数：netlib_send
 * 功能：业务调用，当需要发送消息的时候使用
 * 参数：
 *		SOCKET fd：发送消息的socket描述符
 *		void * recvBuf：发送消息存放的字符串
 *		int len：发送消息的长度
 * 返回值：
 *		成功：0
 *		失败：-1
**********************************/
int netlib_send(SOCKET fd, void * sendbuf, int len) {

	CBaseSocket* pSocket = CBaseSocket::FindBaseSocket(fd);
	if(!pSocket){
		sLogMessage("FindBaseSocket, socket=%d from map\n", LOGLEVEL_ERROR, fd);
		return NETLIB_ERROR;
	}

	int ret = pSocket->Send(sendbuf, len);
	pSocket->RemoveRef();
	return ret;
}

/*********************************
 * 函数：netlib_close
 * 功能：业务调用，当需要关闭socket时候使用
 * 参数：
 *		SOCKET fd：socket描述符
 * 返回值：
 *		成功：0
 *		失败：-1
**********************************/
int netlib_close(SOCKET fd) {

	CBaseSocket* pSocket = CBaseSocket::FindBaseSocket(fd);
	if(!pSocket){
		sLogMessage("FindBaseSocket, socket=%d from map\n", LOGLEVEL_ERROR, fd);
		return NETLIB_ERROR;
	}

	int ret = pSocket->Close();

	pSocket->RemoveRef();
	return ret;
}

/*********************************
 * 函数：netlib_eventloop
 * 功能：开启事件监听循环，调用CEventDispatch的实例，并开启循环
 * 参数：
 *		int wait_time：循环等待时间
 * 返回值：
**********************************/
void netlib_eventloop(int wait_time) {

	CEventDispatch::GetInstance().StartDispatch(wait_time);
}

/*********************************
 * 函数：netlib_stop_eventloop
 * 功能：停止事件监听循环，调用CEventDispatch的实例，并停止循环
 * 参数：
 * 返回值：
**********************************/
void netlib_stop_eventloop() {

	CEventDispatch::GetInstance().StopDispatch();
}


