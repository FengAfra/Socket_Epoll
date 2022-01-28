#ifndef _CONN_OBJECT_H_
#define _CONN_OBJECT_H_

#include "ostype.h"
#include "util.h"

class CConnObject : public CRefObject {

public:
	CConnObject();
	virtual ~CConnObject();

	int SendPdu();

};


#endif

