#ifndef _OBJECT_H_
#define _OBJECT_H_

#ifndef ASSERT
#define ASSERT(x)
#endif // ASSERT

#include "Types.h"
#include "DFID.h"

#include "ScriptVal.h"

#if !defined(USE_NOSCRIPTING)
#include "DFScript.h"
#endif

class TagList;
class FuSYSTEM_API Registry;
class UndoState;
class List;
class Node;
class Menu;
class EventList;
class Object;
class FusionDoc;
class RemoteApp;

#ifndef __OBJECT_DEFINED_
#define __OBJECT_DEFINED_

class FuSYSTEM_API _Object
#if !defined(USE_NOSCRIPTING)
 : public ScriptObject
#endif
{
public:
	_Object();
	_Object(const _Object &toCopy);
	virtual ~_Object();

	virtual void GetObjectInfo(const ScriptVal &what, ScriptVal &val);

#if !defined(USE_NOSCRIPTING)
	// ScriptObject overrides - these are empty placeholders.
	virtual void IncRef();
	virtual void DecRef();
	virtual ScriptInfo *GetScriptInfo() const;
	virtual void SetScriptInfo(ScriptInfo *si);
#endif
};
#endif

// Reference counting object
class FuSYSTEM_API RefObject : public _Object
{
public:
#if SPLIT_DFSYSTEM
	eyeon::AtomicInt UseCount;
#else
	int32 UseCount;
#endif

	RefObject();
	RefObject(const RefObject &toCopy);
	virtual ~RefObject();
	
	virtual void GetObjectInfo(const ScriptVal &what, ScriptVal &val);

	virtual void Use();			// For Usecounts
	virtual void Recycle();
	
	virtual void OnAddToList(List *list, Node *node, TagList *tags = NULL);
	virtual void OnRemoveFromList(List *list, Node *node, TagList *tags = NULL);
};

class FuSYSTEM_API Object : public RefObject
{
#ifdef _DEBUG_MEMLEAKS
protected:
	char m_ObjName[16];
#endif

	// FuDeclareClass(Object); // declare ClassID static member, etc.
public:
	const static FuID ClassID;

public:
	const Registry *m_RegNode;
	FusionDoc *DocPtr; // Not all Objects will have this!

	int32 UndoCount;

public:
	Object(); //TODO: Remove?
	Object(const Object &toCopy);
	Object(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~Object();
	
	virtual void GetObjectInfo(const ScriptVal &what, ScriptVal &val);

	// Load function is only used for certain object types that cannot be constructor-loaded, such as views :(
	virtual bool Load(const ScriptVal &table, const TagList &tags);
	virtual bool Save(ScriptVal &table, const TagList &tags);

	static bool RegInit(Registry *regnode);
	static void RegCleanup(Registry *regnode);

	virtual void Dump();

	const char *ObjectName();

	bool SetAttrs(Tag firsttag, ...);
	uint32 GetAttr(Tag tag) const;
	uint32 GetAttr(Tag tag, uint32 def) const;
	float64 GetAttrD(Tag tag) const;
	float64 GetAttrD(Tag tag, float64 def) const;
	void *GetAttrPtr(Tag tag) const;
	void *GetAttrPtr(Tag tag, void *def) const;
	bool GetAttrB(Tag tag) const;
	bool GetAttrB(Tag tag, bool def) const;
	const FuID GetAttrID(Tag tag) const;
	const FuID GetAttrID(Tag tag, const FuID &def) const;

	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;
	
	// Undo!
#ifndef SPLIT_DFSYSTEM
	virtual bool EndUndo(bool keep = true, TagList *tags = NULL);
	virtual void StartUndo(char *text, TagList *tags = NULL);

	virtual void RestoreState(UndoState *us, TagList *tags = NULL);
	virtual UndoState *SaveState(TagList *tags = NULL);
	virtual void CleanupState(UndoState *us, TagList *tags = NULL);
#endif

	virtual void Use();			// For Usecounts
	virtual void Recycle();

	virtual void OnAddToList(List *list, Node *node, TagList *tags = NULL);
	virtual void OnRemoveFromList(List *list, Node *node, TagList *tags = NULL);

	virtual void AddToMenu(Menu *menu, TagList &tags);
	virtual void HandleMenu(uint32 cmd, uint_ptr *data, TagList &tags);
	virtual void HandleMenu(const ScriptVal &cmd, const ScriptVal &data, TagList &tags);

	virtual void GetAvailableEvents(EventList *events, const FuID &eventid = NOID);
	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);
	bool HandleEventAttrs(const FuID &eventid, uint_ptr subid, Tag firsttag = _TAG_DONE, ...);

	bool Do(uint32 cmd, Tag firsttag = _TAG_DONE, ...);
	virtual bool Do(uint32 what, TagList &tags);
	virtual bool Do(uint32 what, ScriptVal &table);

	virtual void OnConnect(RemoteApp *app);
	virtual void OnDisconnect(RemoteApp *app);
	virtual void OnConnectFailed(RemoteApp *app);

	// Safe to call on NULL
	bool IsTypeOf(const FuID &id);
	bool IsValid();

#if !defined(USE_NOSCRIPTING)
	// ScriptObject overrides
	virtual void IncRef();
	virtual void DecRef();
	virtual ScriptInfo *GetScriptInfo() const;
	virtual void SetScriptInfo(ScriptInfo *si);
	
	DECLARE_LUATYPE(Object);
#endif

protected:
	virtual bool _IsValid();
};

// Safe dynamic cast function
template <class T> T *FuDynamicCast(Object *obj)
{
	return obj->IsTypeOf(T::ClassID) ? (T *)obj : NULL;
}

#define OBJECT_BASE 0

enum ObjectTags ENUM_TAGS
{
	OBJ_Version       = OBJECT_BASE + TAG_INT,

	OBJP_DocPtr        = OBJECT_BASE + TAG_PTR,
	OBJP_ObjPtr,
	OBJP_ProxyObjPtr,
	OBJP_ParentObjPtr,
	OBJP_Owner,

	OBJS_ID           = OBJECT_BASE + TAG_STRING,

	OBJT_Script_Data  = OBJECT_BASE + TAG_TAGLIST, // Custom Script Data
};

// These includes must be below Object definition.
#include "Debug.h"

// Base class to prevent implicit comparison to other DFPointers, via operator bool()
class FuPointerBase
{
private:
	void operator ==(const FuPointerBase &b) const;
	void operator !=(const FuPointerBase &b) const;
};


// Reference-Counted Pointer to any Object derived class
template <class ObjClass> class FuPointer : public FuPointerBase
{
protected:
	RefObject *m_Obj;

public:
	FuPointer(ObjClass *obj = NULL)
	{
		m_Obj = obj;

		if (m_Obj)
			m_Obj->Use();
	}

	FuPointer(ObjClass *obj, bool addref)
	{
		m_Obj = obj;

		if (m_Obj && addref)
			m_Obj->Use();
	}

	FuPointer(const FuPointer &p)
	{
		m_Obj = p.m_Obj;

		if (m_Obj)
			m_Obj->Use();
	}

	~FuPointer()
	{
		if (m_Obj)
			m_Obj->Recycle();
	}

	ObjClass *operator = (ObjClass *obj)
	{
		if (obj != (ObjClass *)m_Obj)
		{
			if (m_Obj)
				m_Obj->Recycle();

			m_Obj = obj;

			if (m_Obj)
				m_Obj->Use();
		}

		return obj;
	}

	const FuPointer &operator = (const FuPointer &p)
	{
		if (m_Obj != p.m_Obj)
			*this = (ObjClass *)p.m_Obj;

		return *this;
	}

	bool operator ==(const FuPointer &p) const { return (m_Obj == p.m_Obj); }
	bool operator !=(const FuPointer &p) const { return (m_Obj != p.m_Obj); }

	bool operator ==(FuPointer &p) const { return (m_Obj == p.m_Obj); }
	bool operator !=(FuPointer &p) const { return (m_Obj != p.m_Obj); }
	
	bool operator ==(const ObjClass *p) const { return (m_Obj == p); }
	bool operator !=(const ObjClass *p) const { return (m_Obj != p); }
	
	bool operator ==(ObjClass *p) const { return (m_Obj == p); }
	bool operator !=(ObjClass *p) const { return (m_Obj != p); }

	ObjClass * operator ->() const    { return (ObjClass *)m_Obj; }
	
	
	//TODO: Ideally we wouldn't need these...
	operator ObjClass *() const  { return (ObjClass *)m_Obj; }
	ObjClass &operator *() const { return *(ObjClass *)m_Obj; }
};

#endif
