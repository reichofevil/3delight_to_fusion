#ifndef _TIMEEXTENTLIST_H_
#define _TIMEEXTENTLIST_H_

#include "List.h"
#include "TimeExtent.h"

class TimeExtentNode;


// Flags for TimeExtentLists
#define TLISTF_Base				LISTF_NextBase
#define TLISTF_RemoveDups    (TLISTF_Base << 0) // (Default) delete all node's objects upon our destruction

#define TLISTF_NextBase			0x10000            // Use this as the next subclass base


class FuSYSTEM_API TimeExtentList : public List
{
	FuDeclareClass(TimeExtentList, List); // declare ClassID static member, etc.

protected:
	TimeExtentNode *NewNode(Node *prev = NULL, Object *obj = NULL, uint32 id = NULL, const TimeExtent &extent = TimeExtent());
	void FreeNode(TimeExtentNode *node);

public:
	TimeExtentList(uint32 flags = LISTF_AutoDestruct | TLISTF_RemoveDups);
	TimeExtentList(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~TimeExtentList();

	static void RegCleanup(Registry *reg);

	virtual bool Add(Object *obj, const TimeExtent &extent = TimeExtent());
	virtual bool Add(Object *obj, const TimeExtent &extent, bool RemDup);

	virtual Node *RemoveNode(Node *node);

	Object *Find(TimeStamp time);						// Return first object at 'time'

	virtual Node *FindNode(Object *obj) { return List::FindNode(obj); }
	virtual Node *FindNode(Object *obj, TimeStamp time);
	virtual Node *FindNode(TimeStamp time);
};

#endif
