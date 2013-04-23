#ifndef _UNDOSTATE_H_
#define _UNDOSTATE_H_

#include "TagList.h"
#include "HashTableList.h"

class UndoState : public TagList
{
	FuDeclareClass(UndoState, TagList); // declare ClassID static member, etc.

public:
	Object *Obj;
	ScriptVal m_Data;

	UndoState(Object *obj);
	~UndoState();
	
	void Restore();
};

class UndoContext : public HashTableList
{
	FuDeclareClass(UndoContext, HashTableList); // declare ClassID static member, etc.

public:

#ifndef SPLIT_DFSYSTEM
	CString Text;
#endif

	UndoContext(const char *text);
	~UndoContext();
};

#endif
