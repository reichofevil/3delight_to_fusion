#ifndef _HASHTABLELIST_H_
#define _HASHTABLELIST_H_

// Similar to List, only it uses a 256-entry hash table for object pointer comparisons
// NOTE: The list is not ordered, as such. Priority is used for order index.
// AddAfter() will always add to the tail of the appropriate hashed sublist, fwiw.


#include "List.h"


// HashTableList

class FuSYSTEM_API HashTableList : public List
{
	FuDeclareClass(HashTableList, List);			// declare ClassID static member, etc.

protected:
	Node *m_Table[256];									//	Object hash index
	uint32 m_NextIndex;

	uint8 GetHashValue(uint_ptr id);

public:
	HashTableList(uint32 flags = LISTF_AutoDestruct);
	HashTableList(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~HashTableList();

	virtual Node *Add(Object *obj, uint_ptr id = NULL, int32 pri = 0);
	virtual Node *AddAfter(Object *obj, uint_ptr id, Node *prev);

	virtual Node *RemoveNode(Node *node);

	virtual Object *Find(uint_ptr id);
	Node *FindNodeID(uint_ptr id);

	Node **GetOrderIndex(uint32 &size);				// caller must free, contains NULLs
	Node **GetGaplessOrderIndex(uint32 &size);	// caller must free, no NULLs
};


#endif // _HASHTABLELIST_H_
