#ifndef _TIMELIST_H_
#define _TIMELIST_H_

#include "List.h"

class TimeNode;


// Flags for TimeLists
#define TLISTF_Base				LISTF_NextBase
#define TLISTF_RemoveDups    (TLISTF_Base << 0) // (Default) delete all node's objects upon our destruction

#define TLISTF_NextBase			0x10000            // Use this as the next subclass base


class FuSYSTEM_API TimeList : public List
{
	FuDeclareClass(TimeList, List); // declare ClassID static member, etc.

protected:
	TimeNode *NewNode(Node *prev = NULL, Object *obj = NULL, uint32 id = NULL, TimeStamp time = 0.0);
	void FreeNode(TimeNode *node);

public:
	TimeList(uint32 flags = LISTF_AutoDestruct | TLISTF_RemoveDups);
	TimeList(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~TimeList();

	static void RegCleanup(Registry *reg);

	virtual bool Add(Object *obj, TimeStamp time = 0);
	virtual bool Add(Object *obj, TimeStamp time, bool RemDup);

	virtual Node *RemoveNode(Node *node);

	Object *Find(TimeStamp time);						// Return first object at 'time'

	virtual Node *FindNode(Object *obj) { return List::FindNode(obj); }
	virtual Node *FindNode(Object *obj, TimeStamp time);
};


#endif

