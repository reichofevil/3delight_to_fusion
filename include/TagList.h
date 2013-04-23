#ifndef _TAGLIST_H_
#define _TAGLIST_H_

#include "Types.h"
#include "MemBlock.h"

#ifdef LINUX
#include "Linux.h"
#endif

#ifndef DFI_EXTERNAL
#include "Object.h"
#else
//#include "DFScript.h"
//class Object;
#endif


#include <stdarg.h>


class FuID;

#ifndef DFI_EXTERNAL
class FuSYSTEM_API TagList : public Object
{
	FuDeclareClass(TagList, Object); // declare ClassID static member, etc.

#else
class FuSCRIPT_API TagList
{
public:
	// Important! Keep taglist the same size even if it does not inherit from Object
#ifdef _DEBUG_MEMLEAKS
protected:
	char m_ObjName[16];
#endif
	void *m_RegNode;
	void *DocPtr; // Not all Objects will have this!

	int32 UseCount;
	int32 UndoCount;
#endif

protected:
	int32 TState;				// Its not *Tag*State anymore - it's *Thread*State
	int32 ThreadID;

public:
	bool Allocated;
	Tag *Tags, *Tail;
	int32 Spare;

	void Copy(int32 spare = 8);

public:
	TagList(Tag *tags, bool copy = false);
	TagList(Tag firsttag, ...);
	TagList();
	virtual ~TagList();
	
	static uint32 Size(Tag tag);

	uint32  GetLong(Tag *tagptr) const;
	bool    GetBool(Tag *tagptr) const;
	float64 GetDouble(Tag *tagptr) const;

	const char *GetString(Tag *tagptr) const;
	const char *GetSafeString(Tag *tagptr) const;
	const FuID  GetID(Tag *tagptr) const;

	MemBlock GetMemBlock(Tag *tagptr) const;
	TagList *GetTagList(Tag *tagptr) const;

	void   *GetPtr(Tag *tagptr) const;
	Object *GetObject(Tag *tagptr) const;

	Tag *First() const;
	Tag *Find(Tag tag) const;
	bool FindQuery(Tag tag) const;					// Doesn't return a tagptr because the taglist isn't locked when it returns!

	Tag *Next(Tag *fromptr) const;					// quite safe :-)
	Tag *FindNext(Tag tag, Tag *fromptr) const;

	bool Get(Tag tag, void *buf) const;
	bool Get(Tag tag, int &var) const;
	bool Get(Tag tag, int32 &var) const;
	bool Get(Tag tag, uint32 &var) const;
	bool Get(Tag tag, float64 &var) const;
	bool Get(Tag tag, char * &var) const;
	bool Get(Tag tag, Object * &var) const;
	bool Get(Tag tag, MemBlock &var) const;
	bool Get(Tag tag, bool &var) const;
	bool Get(Tag tag, FuID &var) const;

	uint32  GetLong(Tag tag, uint32 def) const;
	bool    GetBool(Tag tag, bool def) const;
	float64 GetDouble(Tag tag, float64 def) const;

	const char *GetString(Tag tag, const char *def) const;
	const char *GetSafeString(Tag tag, const char *def) const;
	const FuID  GetID(Tag tag, const FuID &def) const;

	MemBlock GetMemBlock(Tag tag, MemBlock def) const;
	TagList *GetTagList(Tag tag, TagList *def) const;

	void   *GetPtr(Tag tag, void *def) const;
	Object *GetObject(Tag tag, Object *def) const;

	void Add(Tag tag, int32 val);
	void Add(Tag tag, uint32 val);
	void Add(Tag tag, int val);
	void Add(Tag tag, float64 val);
	void Add(Tag tag, const char *val);
	void Add(Tag tag, void *val);						// APIREV: remove (should only need 'const void *')
	void Add(Tag tag, const void *val);
	void Add(Tag tag, MemBlock val);
	void Add(Tag tag, bool val);
	void Add(Tag tag, const FuID &val);

	void AddData(Tag tag, void *addr);

	void Set(Tag tag, const void *val, bool addnew = true);
	void Set(Tag tag, int val, bool addnew = true);
	void Set(Tag tag, int32 val, bool addnew = true);
	void Set(Tag tag, uint32 val, bool addnew = true);
	void Set(Tag tag, float64 val, bool addnew = true);
	void Set(Tag tag, const char *val, bool addnew = true);
	void Set(Tag tag, MemBlock val, bool addnew = true);
	void Set(Tag tag, bool val, bool addnew = true);
	void Set(Tag tag, const FuID &val, bool addnew = true);

	void Set(Tag *tagptr, int val);
	void Set(Tag *tagptr, int32 val);
	void Set(Tag *tagptr, uint32 val);
	void Set(Tag *tagptr, float64 val);
	void Set(Tag *tagptr, const char *val);
	void Set(Tag *tagptr, const void *val);
	void Set(Tag *tagptr, MemBlock val);
	void Set(Tag *tagptr, bool val);
	void Set(Tag *tagptr, const FuID &val);

	void SetData(Tag *tagptr, void *addr);
	void SetData(Tag tag, void *addr, bool addnew = true);
	void SetTags(const TagList *tags, bool addnew = true);

#ifndef DFI_EXTERNAL
	bool SetAttrs(Tag firsttag, ...);
#endif
	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	void Clear();
	bool IsEmpty() const;

#ifndef DFI_EXTERNAL
	bool Load(uint8 *buffer, int &buflen);			// returns bytes loaded in buflen
	bool Save(uint8 *buffer, int &buflen) const;	// pass NULL buffer to get minimum buflen
#endif

	void ObtainLock(void) const;
	void ReleaseLock(void) const;

#ifndef RELEASE_VERSION
	void ObtainWriteLock(void);
	void ReleaseWriteLock(void);
#endif
};

//extern int TagSize[];
//#define TAGSIZE(tag) ((tag)>>TAG_SIZE_SHIFT ? TagSize[(tag)>>TAG_SIZE_SHIFT] : 0)

#if defined(unix)
	#define TAGARGS(firsttag, ptr) ptr = &firsttag
#elif (defined(_WIN64))
	#define TAGARGS(firsttag, ptr)\
	Tag TAGARGS_tags[200];\
	{\
	va_list vl;\
	Tag tag;\
	int size = 0;\
	\
		va_start(vl, firsttag);\
		for (tag = firsttag; tag != _TAG_DONE; tag = va_arg(vl, uint32))\
		{\
			TAGARGS_tags[size++] = tag;\
			\
			switch ((tag >> TAG_SIZE_SHIFT) & 0xF)\
			{\
				case 0 :			/* TAG_CONTROL */\
					switch (tag)\
					{\
						case TAG_MORE:\
						case TAG_IGNOREPTR:\
							*(void **)(&TAGARGS_tags[size++]) = va_arg(vl, void *);\
							break;\
						case _TAG_DONE :\
						case TAG_SKIP :\
						case TAG_IGNORE :\
							TAGARGS_tags[size++] = va_arg(vl, int32);\
							break;\
						default:\
							ASSERT(FALSE);\
							break;\
					}\
					break;\
				case 1 :			/* TAG_INT */\
					TAGARGS_tags[size++] = va_arg(vl, int32);\
					break;\
				case 2 :			/* TAG_DOUBLE */\
					*(float64 *)(&TAGARGS_tags[size++]) = va_arg(vl, float64);\
					break;\
				case 3 :			/* TAG_STRING */\
				case 4 :			/* TAG_OBJECT */\
				case 5 :			/* TAG_TAGLIST */\
				case 6 :			/* TAG_PTR */\
				case 8 :			/* TAG_ID */\
					*(void **)(&TAGARGS_tags[size++]) = va_arg(vl, void *);\
					break;\
				case 7 :			/* TAG_MEMBLOCK */\
					*(void **)(&TAGARGS_tags[size++]) = va_arg(vl, void *);\
					*(uint64 *)(&TAGARGS_tags[size++]) = (uint64) va_arg(vl, uint32);\
					break;\
			}\
		}\
		va_end(vl);\
	\
		TAGARGS_tags[size++] = _TAG_DONE;\
		ptr = TAGARGS_tags;\
	}
#elif !(defined(_M_IX86))
	#define TAGARGS(firsttag, ptr)\
	Tag TAGARGS_tags[200];\
	{\
	va_list vl;\
	Tag tag;\
	int size = 0;\
	\
		va_start(vl, firsttag);\
		for (tag = firsttag; tag != _TAG_DONE; tag = va_arg(vl, Tag))\
		{\
			TAGARGS_tags[size++] = tag;\
			\
			switch (tag >> TAG_SIZE_SHIFT)\
			{\
				case 0 :			/* TAG_CONTROL */\
					switch (tag)\
					{\
						case _TAG_DONE :\
						case TAG_SKIP :\
						case TAG_IGNORE :\
						case TAG_MORE :\
						case TAG_IGNOREPTR :\
							break;\
						default:\
							ASSERT(FALSE);\
							break;\
					}\
				case 1 :			/* TAG_INT */\
					TAGARGS_tags[size++] = va_arg(vl, int32);\
					break;\
				case 2 :			/* TAG_DOUBLE */\
					*(float64 *)(&TAGARGS_tags[size]) = va_arg(vl, float64);\
					size += 2;\
					break;\
				case 3 :			/* TAG_STRING */\
				case 4 :			/* TAG_OBJECT */\
				case 5 :			/* TAG_TAGLIST */\
				case 6 :			/* TAG_PTR */\
				case 8 :			/* TAG_ID */\
					*(void **)(&TAGARGS_tags[size++]) = va_arg(vl, void *);\
					break;\
				case 7 :			/* TAG_MEMBLOCK */\
					*(void **)(&TAGARGS_tags[size++]) = va_arg(vl, void *);\
					*(uint32*)(&TAGARGS_tags[size++]) = va_arg(vl, uint32);\
					break;\
			}\
		}\
		va_end(vl);\
	\
		TAGARGS_tags[size++] = _TAG_DONE;\
		ptr = TAGARGS_tags;\
	}
#else
	#define TAGARGS(firsttag, ptr) ptr = &firsttag
#endif

#ifndef USE_NOSCRIPTING

enum TagMapEntryType
{
	TMET_None = 0,
	TMET_Normal,
	TMET_Bool,
	TMET_Enum,
	TMET_ObjPtr,
};

struct TagMapEntry
{
	const char *m_Name;
	uint32 m_Tag;
	TagMapEntryType m_Type;
	void *m_Data;
};

struct TagEnum
{
	int val;
	const char *str;
};


#define begin_tag_map(name) static TagMapEntry name[] = {
#define end_tag_map()	{ NULL, 0, TMET_None, NULL} };
#define map_tag(name) { #name, name, TMET_Normal, NULL }
#define map_tag_bool(name) { #name, name, TMET_Bool, NULL }
#define map_tag_enum(name, etbl) { #name, name, TMET_Enum, etbl }
#define map_tag_objptr(name, type) { #name, name, TMET_ObjPtr, type }
#define begin_tag_enum(name) static TagEnum name[] = {
#define end_tag_enum() NULL, NULL };

FuSYSTEM_API int lua_maptags(TagMapEntry *map, bool totags, lua_State *L, int index, TagList &tags);
FuSYSTEM_API int scriptval_maptags(TagMapEntry *map, bool totags, ScriptVal &val, TagList &tags);

typedef void (*LWMAPTAGFUNC)(const FuID context, bool totags, lua_State *L, int tags_index, TagList &tags);
typedef void (*SCRIPTMAPTAGFUNC)(const FuID context, bool totags, ScriptVal &val, TagList &tags);

static void lw_maptags(const FuID context, bool totags, lua_State *L, int tags_index, TagList &tags);
static void script_maptags(const FuID context, bool totags, ScriptVal &val, TagList &tags);

#endif

/*
	size *= sizeof(int32);\
	ptr = (Tag *)malloc(size);\
	memcpy(ptr, tags, size);\
*/

#endif
