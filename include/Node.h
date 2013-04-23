#ifndef _NODE_H_
#define _NODE_H_

#include "Object.h"

#ifndef _DEBUG
#undef new

extern HANDLE SmallChangeHeap;
#endif

class FuSYSTEM_API Node : public _Object
{
//	FuDeclareClass(Node); // declare ClassID static member, etc.

protected:
	Node *NextPtr;
	Node *PrevPtr;
	
	Object *ObjPtr;

protected:		// These are the exclusive domain of Lists! Hands off.
	Node(Node *prev = NULL, Object *obj = NULL, uint_ptr id = 0, int32 pri = 0);
	~Node();

public:
	uint_ptr ID;
	int32 Priority;

public:
	Node *Next(void) const { return NextPtr; }
	Node *Prev(void) const { return PrevPtr; }
	Object *Obj(void) const { return ObjPtr; }

	friend class List;
	friend class VectorList;

#ifndef _DEBUG
public:
	// public member functions for Node class
	// declaration of new & delete operator overloads
	void*   operator    new(size_t nSize);
	void    operator    delete(void* pObj);
#endif
};

#endif
