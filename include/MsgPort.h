#ifndef _MSGPORT_H_
#define _MSGPORT_H_

#include "List.h"
#include "Message.h"

class FuSYSTEM_API MsgPort : public List
{
	FuDeclareClass(MsgPort, List); // declare ClassID static member, etc.

public:
	HANDLE Event;
	bool Allocated;
	
public:
	MsgPort(HANDLE event = NULL);
	virtual ~MsgPort();
	
	bool PutMsg(Message *msg);
	Message *GetMsg();
	bool Wait(uint32 timeout = INFINITE);		// in ms

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(MsgPort);
#endif
};

#endif
