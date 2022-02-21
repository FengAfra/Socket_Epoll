#ifndef _UTIL_H_
#define _UTIL_H_

#include "ostype.h"

class CRefObject {

public:
	CRefObject();
	virtual ~CRefObject();

	void AddRef();
	void RemoveRef();
	/*********************************
	 * 函数：SetMulFlag
	 * 功能：设置多线程安全标识，设置此标识，则多线程安全，对操作加mutex锁
	 * 参数：
	 * 返回值：
	**********************************/
	void SetMulFlag() {m_threadflag = true;}


private:
	int m_refCount;
	bool m_threadflag;
	pthread_mutex_t m_mutex;
};


uint64_t get_tick_count();

#endif



