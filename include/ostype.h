/**
BaiDu：
1、hostent结构体是什么
2、什么时候inet_addr(ip)会变成INADDR_ANY
3、unorder_map和hash_map的区别
4、extern CSLog和单独在.h文件中声明有错误
5、单例模式
6、ioctl如何判断socket正常读取
7、发送消息，并对返回值进行校验，如果发送失败，需要将此socket加入到发送队列，也就是CEventDispatch循环中
8、为什么Onwrite会有两个不同的状态，而且还要有NETLIB_MSG_CONFIRM这个请求消息
9、客户端连接服务端的时候，什么时候才会触发CTestConnObj的OnNewCBFun函数
10、UtilPdu里边的CSimpleBuffer类为什么会有前边的编译属性。__declspec(dllexport)和__declspec(dllimport)的区别
11、对包头中的length字段等于0的情况暂且不处理
12、server端问题：如果使用EventDispatch的AddEvent函数，会导致将客户端加入到epoll之后，立马触发EPOLLOUT事件
13、server端问题：如果使用正常的m_in_buffer接收客户端传输的消息，接收不到数据
14、CSLOG问题，如果使用CSLOG打印日志，会有一部分内存是越界的，导致程序有别的数据输出
*/

/*
切记：
1、有两个map。且都是static类型的，是线程安全的。
	1.1、第一个为Basesocket的，存放<socket, CBaseSocket*>，通过fd来关联CBaseSocket类对象
	1.2、第二个为前台业务的，存放<socket, CImConn>。通过fd来关联需要处理的对象
		1.2.1、感觉有替代的方法，使用函数绑定，将函数和相关的commondid绑定即可。待定
2、ValLenPdbu类中的Extend的扩展数组的方法是内核方法。可以有效扩展，不需要重新申请，然后复制
3、切记，ConnObject文件中的m_busy非常重要。用于将触发的不需要的EPOLLOUT调用成为空函数，直接返回。
	nginx发送完成后会将发送的回调函数设置成一个空函数（这个函数只是定义里面什么都没有做）。nginx为什么不移除呢？因为反复添加、移除EPOLLOUT性能不友好，总是在用户层和内核层来回切换。
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
#include <sys/ioctl.h>
#include <list>
#include <sys/time.h>





using namespace std;

typedef int SOCKET;
//typedef short uint16_t;


#define NETLIB_OK		0
#define NETLIB_ERROR	-1


//typedef std::unordered_map unordered_map_t;

enum NETLIB_MSG {
	NETLIB_MSG_CONNECT,
	NETLIB_MSG_WRITE,
	NETLIB_MSG_READ,
	NETLIB_MSG_CLOSE,
	NETLIB_MSG_CONFIRM,
	NETLIB_MSG_TIMER
};

typedef void(*callback_t)(void* callback_data, NETLIB_MSG msg,  SOCKET fd);


extern CSLog g_demolog;


#define __FILENAME__  (strrchr(__FILE__, '/')) ? (strrchr(__FILE__, '/' + 1)):( __FILE__ )

#define sLogMessage(fmt, logLevel, args...) g_demolog.LogMessage(fmt, logLevel, __FILENAME__, __LINE__, __FUNCTION__, ##args)



#endif


