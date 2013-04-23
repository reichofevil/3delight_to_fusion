#ifndef _THREADEDOPERATOR_H_
#define _THREADEDOPERATOR_H_

#include "Operator.h"

class ThreadedOperator;

typedef void (*OMFPTR)(Operator *op, Request *req, int32 chunk);
typedef void (ThreadedOperator::*OPMPFUNC)(Request *req, int32 chunk);

class FuSYSTEM_API ThreadedOperator : public Operator
{
	FuDeclareClass(ThreadedOperator, Operator); // declare ClassID static member, etc.

public:
	uint32 ThreadID;
	HANDLE ThreadHandle;

	OMFPTR ChunkFunc;
	int32 NumChunks;
	int32 CurrentChunk;
	Request *CurrentReq;

	MsgPort *HandleRequestPort;

	uint8 Pad[256];

protected:
	MsgPort OpPort;

public:
	ThreadedOperator(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~ThreadedOperator();

	virtual void OnAddToFlow(TagList *tags = NULL);
	virtual void OnRemoveFromFlow(TagList *tags = NULL);

	virtual uint32 ThreadMain();
	virtual bool PutMsg(Message *msg);

	void DoMultiProcess(Request *req, int32 numchunks);
	virtual void ProcessChunk(Request *req, int32 chunk);
	virtual void QuickProcessChunk(Request *req, int32 chunk);

	void DoMultiProcess(OMFPTR func, Request *req, int32 numchunks);
	void DoMultiProcess(OPMPFUNC func, Request *req, int32 numchunks);

	virtual bool HandleRequest(Request *req);
	virtual void ChainDuplicateRequests(Request *req, TimeExtent &te);

	// Just to make sure there's an implementation, so that derived classes don't
	// skip this should we ever want to do something in these functions
	virtual void GetAvailableEvents(EventList *events, const FuID &eventid = NOID);
	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);
	virtual bool OnEvent(const FuID &eventid, uint_ptr subid, TagList &tags);
};

#endif
