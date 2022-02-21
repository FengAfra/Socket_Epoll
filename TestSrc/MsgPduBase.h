#ifndef _MSG_PDU_BASE_H_
#define _MSG_PDU_BASE_H_

#include "VarLenPdu.h"

#define MSG_PDU_HEADER_LEN	16

typedef struct {
	uint32_t	length;
	uint16_t	version;
	uint16_t	flag;
	uint16_t	service_id;
	uint16_t	command_id;
	uint16_t	seq_num;
	uint16_t	reversed;
}MsgPduHeader_t;


class CMsgPduBase {

public:
	CMsgPduBase();
	~CMsgPduBase();

	static CMsgPduBase* ReadPdu(uchar_t* buf, uint32_t len);
	static bool IsPduAvailable(uchar_t* buf, uint32_t len, uint32_t& pdu_len);
	uchar_t* GetBuffer();
	uint32_t GetLength() {return m_buf.GetWriteOffset();}

	void Write(uchar_t* buf, uint32_t len);

	uint16_t GetCommandId() {return m_MsgPduHeader.command_id;};

	//为了测试使用，将m_buf设置为public权限
	CValLenPdu			m_buf;
	
protected:

	MsgPduHeader_t		m_MsgPduHeader;
	//CValLenPdu			m_buf;

};


#endif

