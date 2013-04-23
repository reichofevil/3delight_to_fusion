#ifndef _LINK_H_
#define _LINK_H_

#include "LockableObject.h"
#include "Parameter.h"
#include "Request.h"
#include "HashList.h"
class Operator;
class Parameter;

class FuSYSTEM_API Link : public LockableObject
{
	FuDeclareClass(Link, LockableObject); // declare ClassID static member, etc.

protected:

public:
	FuID m_DataType;    // Primary DataType

	FuID *AllowedDataTypes;			// Array of acceptable datatypes (not inc. primary DT)
	uint32 NumAllowedDataTypes;		// array size

	char *Name;
	Operator *Owner;
	int Offset;
	
	FuID m_ID;//, GroupID;
	int m_Main;
	
	FuPointer<Parameter> Cache;
	TimeExtent CacheValid;

	TagList Attrs;
#ifndef USE_NOSCRIPTING
	ScriptVal m_CustomData;
#endif

	FuID InstanceOp;
	List InstanceList;

	HashList *m_DependencyList;

	uint32 m_CacheMode; // Caching Flags

	int m_Visible;

	uint8 pad[24];

	Link(const FuID &name, const FuID &dt, Operator *owner);

	bool IsID(const char *id) const;
	bool IsInGroup(void) const;
	bool IsInGroup(const char *group) const;
	bool IsSameGroup(const char *id) const;
	bool IsSameGroup(const Link *link) const;

	virtual ~Link();

	virtual bool IsConnected() const;
	virtual bool IsUsed(TagList *tags = NULL) const;
	virtual bool IsConnectedTo(Link *link, TagList *tags = NULL) const { return false; }
	virtual int32 GetNumConnects() const { return 0; }

	virtual bool IsDataTypeOK(const FuID &dt, TagList *tags = NULL) const;
	virtual bool IsDataTypeOK(Link *link, TagList *tags = NULL);

	virtual bool SetAttrsTagList(const TagList& tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	virtual void InvalidateCache(TagList *tags = NULL);
	virtual void InvalidateCache(const TimeExtent& te, TagList *tags = NULL);

	Link(const Registry *reg, const ScriptVal &table, const TagList &tags);
	bool Save(ScriptVal &table, const TagList &tags);

	virtual void RestoreState(UndoState *us, TagList *tags = NULL);
	virtual void CleanupState(UndoState *us, TagList *tags = NULL);
	virtual UndoState *SaveState(TagList *tags = NULL);

#if !defined(USE_NOSCRIPTING)
	DECLARE_LUATYPE(Link);
#endif
};

#define LINK_BASE LOCKABLEOBJECT_BASE + TAGOFFSET

enum LinkTags ENUM_TAGS
{
	LINK_Version = LINK_BASE | TAG_INT,
	LINK_Main,

	LINK_ForceSave,				// Will always save into the comp, even if everything is default
	LINK_DoNotSave,

	LINK_OldMain,					// deprecated: use LINK_Visible

	LINK_CacheMode,

	LINK_Visible,					// use to show/hide main/mask inputs/outputs in the flow view

	LINKS_Name = LINK_BASE | TAG_STRING + 20, // Avoid conflict with packet LINKS_ tags :(
	
	LINKID_ID = LINK_BASE | TAG_ID,
	LINKID_AddAfterID,			// Add this link after the one with this ID

	LINKID_DataType = LINK_BASE | TAG_ID | TAG_MULTI,
	LINKID_AllowedDataType,		// TODO: Remove
	LINKID_LegacyID,				// Old ID for this link, update any old connections
	LINKID_OverideDefLink,		// overrides the selection of a default input when an output is drag-connected to a tool (both the input/output must define this ID and they will be preferentially connected over the input with the lowest LINK_Main)

	LINKP_Source = LINK_BASE | TAG_PTR,
	LINKP_OwnerOp,
	LINKP_Instance,

	LINKP_IsClone,
};

#define LINK_REQ_BASE		REQUEST_REQ_BASE + TAGOFFSET
enum LinkReqAttrs ENUM_TAGS
{
	LINKRA_NoDiskWrite = TAG_INT + LINK_REQ_BASE,
};

#endif
