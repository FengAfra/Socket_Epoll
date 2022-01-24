#ifndef _NET_LIB_H_
#define _NET_LIB_H_

#include "BaseSocket.h"




int netlib_init();

int netlib_listen(const char* server_ip, const uint16_t server_port, callback_t callback, void* callback_data);

void netlib_eventloop(int wait_time);



#endif


