#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "List.h"

class MsgPort;

class FuSYSTEM_API Message : public Object
{
	FuDeclareClass(Message, Object); // declare ClassID static member, etc.

public:
	uint32 ID;
	MsgPort *ReplyPort;
	Object *Source;
	int32 Priority;
#ifndef USE_NOSCRIPTING
	ScriptArgs *m_ScriptMsg;
#endif

public:
	Message(uint32 id, int32 pri = 0);
	virtual void Reply();

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(Message);
#endif
};

#endif
