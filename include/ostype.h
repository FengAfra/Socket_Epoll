#ifndef _OSTYPE_H_
#define _OSTYPE_H_



typedef int SOCKET;
typedef short uint16_t;

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <CSLog.h>
#include <unistd.h>
#include <fcntl.h>


#define NETLIB_OK		0
#define NETLIB_ERROR	-1


typedef void(*callback_t)(void* callback_data);


CSLog g_demolog = CSLog(false);

#define __FILENAME__  (strrchr(__FILE__, '/')) ? (strrchr(__FILE__, '/' + 1)):( __FILE__ )

#define sLogMessage(fmt, logLevel, args...) g_demolog.LogMessage(fmt, logLevel, __FILENAME__, __LINE__, __FUNCTION__, ##args)



#endif


