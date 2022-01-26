#include "util.h"

CRefObject::CRefObject() {

	pthread_mutex_init(&m_mutex, NULL);
	m_refCount = 1;
	m_threadflag= false;
}


CRefObject::~CRefObject() {


}

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





