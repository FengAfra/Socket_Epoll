#ifndef _UTIL_H_
#define _UTIL_H_

#include "ostype.h"

class CRefObject {

public:
	CRefObject();
	virtual ~CRefObject();

	void AddRef();
	void RemoveRef();
	void SetMulFlag() {m_threadflag = true;}


private:
	int m_refCount;
	bool m_threadflag;
	pthread_mutex_t m_mutex;
};

#endif



