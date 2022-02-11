#ifndef _MSG_PDU_BASE_H_
#define _MSG_PDU_BASE_H_

#include "VarLenPdu.h"

#define MSG_PDU_HEADER_LEN	16


class CMsgPduBase {

public:
	CMsgPduBase();
	~CMsgPduBase();

	CMsgPduBase* ReadPdu(uchar_t* buf, uint32_t len);
	bool IsPduAvailable(uchar_t* buf, uint32_t len, uint32_t& pdu_len);

	void Write(uchar_t* buf, uint32_t len);
	
protected:

};


#endif

