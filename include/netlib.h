#ifndef _NET_LIB_H_
#define _NET_LIB_H_

#include "BaseSocket.h"

enum NETLIB_OPT {
	NETLIB_OPT_SET_CALLBACK,
	NETLIB_OPT_SET_CALLBACK_DATA,
};


int netlib_init();

void netlib_eventloop(int wait_time);

void netlib_stop_eventloop();

int netlib_bind(SOCKET fd, NETLIB_OPT opt, void* data);

int netlib_add_timer(callback_t callback, void *user_data, uint64_t interval);

int netlib_remove_timer(callback_t callback, void *user_data);

int netlib_listen(const char* server_ip, const uint16_t server_port, callback_t callback, void* callback_data);

SOCKET netlib_connect(const char* ip, uint16_t port, callback_t callback, void* callback_data);

int netlib_recv(SOCKET fd, void* recvBuf, int len);

int netlib_send(SOCKET fd, void* sendbuf, int len);

int netlib_close(SOCKET fd);

#endif


