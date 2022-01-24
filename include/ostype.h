/**
BaiDu：
1、hostent结构体是什么
2、什么时候inet_addr(ip)会变成INADDR_ANY
3、unorder_map和hash_map的区别
4、extern CSLog和单独在.h文件中声明有错误
5、单例模式
*/



#ifndef _OSTYPE_H_
#define _OSTYPE_H_


#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <CLog4cplus/CSLog.h>
#include <unistd.h>
#include <fcntl.h>
#include <unordered_map>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>



using namespace std;

typedef int SOCKET;
//typedef short uint16_t;


#define NETLIB_OK		0
#define NETLIB_ERROR	-1


//typedef std::unordered_map unordered_map_t;

typedef void(*callback_t)(void* callback_data);


extern CSLog g_demolog;


#define __FILENAME__  (strrchr(__FILE__, '/')) ? (strrchr(__FILE__, '/' + 1)):( __FILE__ )

#define sLogMessage(fmt, logLevel, args...) g_demolog.LogMessage(fmt, logLevel, __FILENAME__, __LINE__, __FUNCTION__, ##args)



#endif


