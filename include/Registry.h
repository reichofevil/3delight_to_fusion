#ifndef _REGISTRY_H_
#define _REGISTRY_H_

#include "TagList.h"

class List;

class ScriptInfo;
class RegistryList;
struct tagHELPINFO;
typedef struct tagHELPINFO HELPINFO;

#define REGISTRY_MAGIC 'Reg!'

class FuSYSTEM_API Registry : public TagList
{
	FuDeclareClass(Registry, TagList); // declare ClassID static member, etc.

protected:
	bool CheckOut(void);
	void CheckIn(void);

public:
	uint32 m_ClassType;				// was: m_RegMagic (NB REG_ClassType is still valid)

	Registry *m_Parent;
	ScriptInfo *m_ScriptInfo;
	
	char *m_Name;
	FuID m_ID;

	RegistryList **m_RegList;

public:
	Registry(RegistryList **list, const FuID &id, Tag firsttag, ...);
	virtual ~Registry();

	bool Init();
	void Cleanup();
	bool ShowHelp(HELPINFO *pHelpInfo, Object *object = NULL) const;
	bool ShowHelp(HELPINFO *pHelpInfo, const Object *object, const char *subtopic) const;

	Object *New(const ScriptVal &table, const TagList &tags) const;
	Object *New(const ScriptVal &table, const TagList &tags, FusionDoc *doc) const;

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	bool IsPlugin();

	bool IsClassType(const FuID &id) const;				// checks for ID class types
	bool IsRegClassType(const uint32 ct_type) const;	// checks for REG_ClassType types (inc MX classes)

#ifndef USE_NOSCRIPTING
	void lw_maptags(const FuID context, bool totags, lua_State *L, int tags_index, TagList &tags, bool doparent);
	void script_maptags(const FuID context, bool totags, ScriptVal &val, TagList &tags, bool doparent);
#endif
};

typedef Object *(*FPTR) (...);
typedef bool (*INITFUNCPTR) (Registry *reg);
typedef Object *(*NEWFUNCPTR) (const Registry *reg, const ScriptVal &table, const TagList &tags);


#if defined(FU_DECLARE_DEBUG_REGISTRY)
	static Registry db_reg(&DebugList, MODULENAME, REG_DEBUG, &debuglev, TAG_DONE);
#endif

#endif
