#include "util.h"

CRefObject::CRefObject() {

	pthread_mutex_init(&m_mutex, NULL);
	m_refCount = 1;
	m_threadflag= false;
}


CRefObject::~CRefObject() {


}


/*********************************
 * 函数：AddRef
 * 功能：增加对象的调用次数，初始值为1
 * 参数：
 * 返回值：
**********************************/
void CRefObject::AddRef() {

	if(m_threadflag) {
		pthread_mutex_lock(&m_mutex);
		m_refCount ++ ;
		pthread_mutex_unlock(&m_mutex);
	}
	else {
		m_refCount ++ ;
	}

}

/*********************************
 * 函数：RemoveRef
 * 功能：减少对象的调用次数，若调用次数减少为0，则删除此对象
 * 参数：
 * 返回值：
**********************************/
void CRefObject::RemoveRef() {

	if(m_threadflag) {
		pthread_mutex_lock(&m_mutex);
		m_refCount -- ;
		if(m_refCount == 0) {
			delete this;
			return;
		}
		pthread_mutex_unlock(&m_mutex);
	}
	else {
		m_refCount -- ;
		if(m_refCount == 0) {
			delete this;
			return;
		}
	}
}


uint64_t get_tick_count() {

	struct timeval tval;
	uint64_t ret_tick;

	gettimeofday(&tval, NULL);

	ret_tick = tval.tv_sec * 1000L + tval.tv_usec / 1000L;
	return ret_tick;
}




