#ifndef _HashList_h_
#define _HashList_h_

#include "List.h"

class FuSYSTEM_API HashList : public List
{
	FuDeclareClass(HashList, List); // declare ClassID static member, etc.

protected:
	uint32 GetHashValue(const FuID &id);

public:
	HashList(uint32 flags = LISTF_AutoDestruct);
	HashList(const Registry *reg, const ScriptVal &table, const TagList &tags);

	virtual Node *Add(Object *obj, const FuID &name, int32 pri = 0);
	virtual Node *AddAfter(Object *obj, const FuID &name, Node *prev);

	Object *Find(const FuID &name);
	Object *FindNoCase(const FuID &name);
	Node *FindNode(Object *obj);
	Node *FindNode(const FuID &name);
	Node *FindNodeNoCase(const FuID &name);

	bool ReHash(Object *obj, const FuID &name);
};


#endif // _HashList_h_
