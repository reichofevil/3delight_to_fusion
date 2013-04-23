#ifndef _LIST_H_
#define _LIST_H_

#include "LockableObject.h"
#include "Node.h"


// Flags for lists
#define LISTF_Base				0x001
#define LISTF_AutoDestruct		(LISTF_Base << 0)	// (Default) delete all node's objects upon our destruction
#define LISTF_NotifyObjects   (LISTF_Base << 1)	// Call object's OnAddToList()/OnRemoveToList()
#define LISTF_NoDupObjects    (LISTF_Base << 2)	// Disallow duplicate objects on this list

#define LISTF_NextBase			0x100			// Use this as the next subclass base


class FuSYSTEM_API List : public LockableObject
{
	FuDeclareClass(List, LockableObject); // declare ClassID static member, etc.

protected:
	Node *Head;
	Node *Tail;

	uint32 Flags;							// See above
	uint32 Count;							// Number of Nodes, not counting head/tail

	Node *NewNode(Node *prev = NULL, Object *obj = NULL, uint_ptr id = 0, int32 pri = 0);
	void FreeNode(Node *node);

public:
	List(uint32 flags = LISTF_AutoDestruct);		// See above
	List(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~List();
	
	static void List::RegCleanup(Registry *reg);

	virtual Node *Add(Object *obj, uint_ptr id = NULL, int32 pri = 0);
	virtual Node *AddAfter(Object *obj, uint_ptr id, Node *prev);

	virtual Node *Remove(Object *obj);
	virtual Node *RemoveNode(Node *node);

	Node *Detach(Object *obj);									// Like Remove(), but node is not destroyed
	Node *DetachNode(Node *node);
	Node *InsertNode(Node *node, Node *prev = NULL);	// Re-inserts an existing node

	virtual void RemoveAll(void);

	virtual bool IsEmpty(void);

	virtual Object *Find(uint_ptr id);
	Object *Find(FuID &id);

	virtual Node *FindNode(Object *obj);
	Node *FindNodeID(uint_ptr id);
	Node *FindNodeID(FuID &id);

	virtual Object *First();
	virtual Object *Last();
	
	virtual Node *HeadNode() { return Head; }
	virtual Node *TailNode() { return Tail; }
	virtual Node *NextNode(Node *node);
	virtual Node *PrevNode(Node *node);

	Object *NextObj(Node *&node);
	Object *PrevObj(Node *&node);

	virtual Object *operator += (Object *obj) { Add(obj); return obj; }
	virtual Object *operator -= (Object *obj) { Remove(obj); return obj; }

	virtual uint32 GetCount() const   { return Count; }

	// These don't require a lock when calling.  Can potentially cause
	// differences in iteration forwards vs backwards in certain (race) conditions.
	void NoLock_Append(Node *newnode);
	void NoLock_Append(Object *obj, uint_ptr id);

	// These two require read locks, also with the above issues
	void ReadLocked_AddAfter(Node *newnode, Node *prev);
	void ReadLocked_AddAfter(Object *obj, uint_ptr id, Node *prev);

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(List);
#endif
};


// Template Object list class
template <class ObjClass> class /*FuSYSTEM_API*/ ListObj : public List
{
public:
	ListObj(uint32 flags = 0) : List(flags)									{ }	// NOTE: different from List! LISTF_AutoDestruct is not default
	ListObj(const Registry *reg, const ScriptVal &table, const TagList &tags) : List(reg,table,tags) {}

	virtual Node *Add(ObjClass *obj, uint_ptr id = NULL, int32 pri = 0) { return List::Add((Object *)obj, id, pri); }
	virtual Node *AddAfter(ObjClass *obj, uint_ptr id, Node *prev)		{ return List::AddAfter((Object *)obj, id, prev); }

	virtual Node *Remove(ObjClass *obj)											{ return List::Remove((Object *)obj); }
	virtual ObjClass *FindObj(uint_ptr id)										{ return (ObjClass *)List::Find(id); }
	virtual ObjClass *FindObj(FuID &id)											{ return (ObjClass *)List::Find(id); }
	virtual Node *FindNode(ObjClass *obj)										{ return List::FindNode((Object *)obj); }

	virtual ObjClass *FirstObj()													{ return (ObjClass *)List::First(); }
	virtual ObjClass *LastObj()													{ return (ObjClass *)List::Last(); }

	ObjClass *NextObj(Node *&node)												{ return (ObjClass *)List::NextObj(node); }
	ObjClass *PrevObj(Node *&node)												{ return (ObjClass *)List::PrevObj(node); }
	ObjClass *Obj(Node *node) const												{ return (ObjClass *)(node? node->Obj() : NULL); }

	virtual ObjClass *operator += (ObjClass *obj)							{ List::Add((Object *)obj); return (ObjClass *)obj; }
	virtual ObjClass *operator -= (ObjClass *obj)							{ List::Remove((Object *)obj); return (ObjClass *)obj; }
};

// Template arbitrary object list class
// if LISTF_AutoDestruct, this class calls delete instead of Recycle(),
// Also, ignores LISTF_NotifyObjects
template <class ObjClass> class /*FuSYSTEM_API*/ ListT : public ListObj<ObjClass>
{
public:
	ListT(uint32 flags = 0) : ListObj<ObjClass> (flags & ~LISTF_NotifyObjects)	{ }	// NOTE: different from List! LISTF_AutoDestruct is not default
	ListT(const Registry *reg, const ScriptVal &table, const TagList &tags) : ListObj<ObjClass>(reg,table,tags) {}

	virtual void RemoveAll(void)
	{
		ObtainLock();

		while (!IsEmpty())
		{
			Node *node = Head->Next();
			void *obj = (void *)node->Obj();
			RemoveNode(node);
			if (obj && (Flags & LISTF_AutoDestruct))
				delete (ObjClass *)obj;
		}

		ReleaseLock();
	}

	virtual ~ListT()
	{
		RemoveAll();	// virtual functions can be called? Just won't get derived versions.
	}
};


// iterator lock enums
enum LockEnum
{
	LIT_Unlocked = -1,
	LIT_ReadLock = 0,
	LIT_WriteLock = 1,
};

// List iterator
template <class ObjClass> class /*FuSYSTEM_API*/ ListIterator
{
	List &list;
	Node *node;
	LockEnum lock;

	void Lock(LockEnum lk = LIT_ReadLock)	{ if (lock == LIT_Unlocked) { if (lk == LIT_WriteLock) list.ObtainLock(); else list.ObtainReadLock(); lock = lk; } }
	void Unlock()					{ if (lock == LIT_WriteLock) list.ReleaseLock(); else if (lock == LIT_ReadLock) list.ReleaseReadLock(); lock = LIT_Unlocked; }

public:
	ListIterator(List *l, LockEnum lk = LIT_ReadLock) :list(*l)	{ node = NULL; lock = LIT_Unlocked; Lock(lk); }
	ListIterator(List& l, LockEnum lk = LIT_ReadLock) :list(l)	{ node = NULL; lock = LIT_Unlocked; Lock(lk); }
	ListIterator(ListIterator<ObjClass>& lit) :list(lit.list) { node = lit.node; lock = LIT_Unlocked; Lock(lit.lock); }
	~ListIterator()				{ Finish(); }
	void Finish()					{ node = NULL; Unlock(); }
	void Restart(LockEnum lk = LIT_Unlocked)	{ if (lk == LIT_Unlocked) lk = (lock == LIT_Unlocked ? LIT_ReadLock : lock); Unlock(); node = NULL; Lock(lk); }
	bool Next()						{ node = list.NextNode(node); return (node != NULL); }
	bool Prev()						{ node = list.PrevNode(node); return (node != NULL); }
	ObjClass *NextObj()			{ return Next()? (ObjClass *)node->Obj() : NULL; }
	ObjClass *PrevObj()			{ return Prev()? (ObjClass *)node->Obj() : NULL; }
	ObjClass *Obj() const		{ return node? (ObjClass *)node->Obj() : NULL; }
	Node *GetNode() const		{ return node; }
	uint_ptr ID() const			{ return node? node->ID : 0; }
	int32 Priority() const		{ return node? node->Priority : 0; }

	void RemoveAll()
	{
		if (lock == LIT_WriteLock)
		{
			list.RemoveAll();
			Restart();
		}
		else
			ASSERT(0);
	}

	// for foward iteration only
	ObjClass *RemoveNode()
	{
		if (node && lock == LIT_WriteLock)
		{
			::Node *tnode = node;
			ObjClass *obj = (ObjClass *)node->Obj();

			Prev();
			list.RemoveNode(tnode);
			return obj;
		}
		else
		{
			ASSERT(0);			// Probably called Restart(), which by default will set it back to a read lock! Why??!!?!?!?
			return NULL;
		}
	}
};



#endif

