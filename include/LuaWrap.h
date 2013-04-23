#ifndef _LuaWrap_h_
#define _LuaWrap_h_

#pragma pack(push, 8)

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN64
typedef unsigned __int64 Tag;
#else
typedef unsigned long Tag;
#endif

#if (!defined(EYEONSCRIPT_NO_LUA) && (!defined(FuPLUGIN) || defined(FuPLUGIN_SCRIPTING)))
extern "C"
{
#ifdef LINUX
	#include "/home/peter/RCS/Pipeline/Lua/include/lua.h"
	#include "/home/peter/RCS/Pipeline/Lua/include/lauxlib.h"
#else
	#include "U:/RCS/Pipeline/Lua511/Src/lua.h"
	#include "U:/RCS/Pipeline/Lua511/Src/lauxlib.h"
#endif
};
#else
struct lua_State;
#endif


#undef lua_unboxpointer
#define lua_unboxpointer(L, index) (lua_touserdata(L, index) ? *(void **)lua_touserdata(L, index) : NULL)

#define LUA_ERROR(state, msg) { lua_pushstring(state, msg); lua_error(state); }

typedef void (*LWREGFUNC)(lua_State *L);
void lw_register(lua_State *L);

bool FuSCRIPT_API lw_flaunttable(lua_State *L, const char *name, const char *item = NULL);

class LuaType;

class FuSCRIPT_API LuaClass
{
public:
	LuaClass() { };
	virtual ~LuaClass() { };

	static LuaType *s_LuaType;

	INT_PTR lw_safe_push(lua_State *L)
	{
#if (!defined(EYEONSCRIPT_NO_LUA) && (!defined(FuPLUGIN) || defined(FuPLUGIN_SCRIPTING)))
		if (this)
		{
			return lw_push(L);
		}
		else
		{
			if (L)
			{
				lua_pushnil(L);
				return 1;
			}
			else
				return NULL;
		}
#else
		return NULL;
#endif
	}

#if (!defined(EYEONSCRIPT_NO_LUA) && (!defined(FuPLUGIN) || defined(FuPLUGIN_SCRIPTING)))
	virtual INT_PTR lw_push(lua_State *L) { if (L) { lua_pushnil(L); return 1; } else return NULL; }
#else
	virtual INT_PTR lw_push(lua_State *L) { return 0; }
#endif
};

#if (!defined(EYEONSCRIPT_NO_LUA) && (!defined(FuPLUGIN) || defined(FuPLUGIN_SCRIPTING)))

FuSCRIPT_API bool lua_istrue(lua_State *L, int index);
FuSCRIPT_API bool lua_istype(lua_State *L, int index, char *name);
FuSCRIPT_API void *lua_totype(lua_State *L, int index, char *name);
FuSCRIPT_API int lua_absindex(lua_State *L, int index);
FuSCRIPT_API float64 lua_tonumber_def(lua_State *L, int index, float64 def);
FuSCRIPT_API const char *lua_tostring_def(lua_State *L, int index, const char *def);
FuSCRIPT_API bool lua_toboolean_def(lua_State *L, int index, bool def);

template <class type> type *lua_toobj(lua_State *L, int index)
{
	return ((LuaObject<type> *) lua_unboxpointer(L, index))->m_Object;
}

#define SetMetaCall(name, call) \
	lua_pushstring(L, name); \
	lua_pushcfunction(L, call); \
	lua_settable(L, -3);

class FuSCRIPT_API LuaType;

template <class T> class FuSCRIPT_API LuaObject
{
public:
	LuaType *m_Type;
	T *m_Object;
	uint32 m_Flags;
};

extern FuSCRIPT_API LuaType *g_LuaTypeList;

#endif

#define LWPF_NotOwned (1<<0)  // Pushed object is not 'owned' by the lua state.  Don't Use or Recycle.

class FuSCRIPT_API LuaType;
class FuSCRIPT_API _LuaType
{
public:
	typedef int (_LuaType::*LUAFUNC)(lua_State *L, void *_this, LuaType *type);

	struct LuaFuncTable
	{
		const char *m_Name;
		LUAFUNC m_Func;
		uint32 m_Hash;
	};

	typedef int (_LuaType::*LUAMEMBER)(lua_State *L, void *_this, LuaType *type, bool set);

	struct LuaMemberTable
	{
		const char *m_Name;
		LUAMEMBER m_Func;
		uint32 m_Hash;
	};

public:
	LuaType *m_Next;
	LuaType *m_Parent;
	const char *m_Name;
	uint32 m_Flags;
	uint32 m_Type;
	LuaFuncTable *m_FuncTable;
	LuaMemberTable *m_MemberTable;
	ScriptVal *m_Data;

public:
	_LuaType(const char *name, LuaType *parent, LuaFuncTable *functable, LuaMemberTable *membertable, uint32 type, uint32 flags);
	virtual ~_LuaType() = 0;

	INT_PTR lw_push(lua_State *L, void *_this);
	INT_PTR lw_push(lua_State *L, void *_this, uint32 flags);
	bool lw_register(lua_State *L);
	bool lw_register(lua_State *L, int index);
};

// Abstract class, for interface only
class FuSCRIPT_API LuaType : public _LuaType
{
public:
	LuaType(const char *name, LuaType *parent, LuaFuncTable *functable, LuaMemberTable *membertable, uint32 type, uint32 flags);
	virtual ~LuaType()/* = 0*/;

	virtual int lw_create(lua_State *L) = 0;
	virtual int lw_typename(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_istypeof(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_tostring(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_call(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_newindex(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_index(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_add(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_sub(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_mul(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_div(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_pow(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_unm(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_eq(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_lt(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_le(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_concat(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_use(lua_State *L, void *_this, LuaType *type) = 0;
	virtual int lw_recycle(lua_State *L, void *_this, LuaType *type) = 0;

	virtual int lw_do(lua_State *L, void *_this, LuaType *type, int what, void *data) = 0;
	virtual int lw_do(lua_State *L, void *_this, LuaType *type, const char *what, void *data) = 0;

	virtual void *lw_to_type(void *_this) = 0;
	virtual void *lw_from_type(void *_this) = 0;
};

// This must match the above class vfunc table *EXACTLY*
template <class T> class LuaTypeT : public _LuaType
{
public:
	LuaTypeT(const char *name, LuaType *parent, LuaFuncTable *functable, LuaMemberTable *membertable, uint32 type, uint32 flags)
		: _LuaType(name, parent, functable, membertable, type, flags) {}
	virtual ~LuaTypeT() {}

	virtual int lw_create(lua_State *L) { return 0; }
	virtual int lw_typename(lua_State *L, T *_this, LuaType *type) { return m_Parent ? m_Parent->lw_typename(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_istypeof(lua_State *L, T *_this, LuaType *type) { return m_Parent ? m_Parent->lw_istypeof(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_tostring(lua_State *L, T *_this, LuaType *type) { return m_Parent ? m_Parent->lw_tostring(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_call(lua_State *L, T *_this, LuaType *type)     { return m_Parent ? m_Parent->lw_call(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_newindex(lua_State *L, T *_this, LuaType *type) { return m_Parent ? m_Parent->lw_newindex(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_index(lua_State *L, T *_this, LuaType *type)    { return m_Parent ? m_Parent->lw_index(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_add(lua_State *L, T *_this, LuaType *type)      { return m_Parent ? m_Parent->lw_add(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_sub(lua_State *L, T *_this, LuaType *type)      { return m_Parent ? m_Parent->lw_sub(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_mul(lua_State *L, T *_this, LuaType *type)      { return m_Parent ? m_Parent->lw_mul(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_div(lua_State *L, T *_this, LuaType *type)      { return m_Parent ? m_Parent->lw_div(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_pow(lua_State *L, T *_this, LuaType *type)      { return m_Parent ? m_Parent->lw_pow(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_unm(lua_State *L, T *_this, LuaType *type)      { return m_Parent ? m_Parent->lw_unm(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_eq(lua_State *L, T *_this, LuaType *type)       { return m_Parent ? m_Parent->lw_eq(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_lt(lua_State *L, T *_this, LuaType *type)       { return m_Parent ? m_Parent->lw_lt(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_le(lua_State *L, T *_this, LuaType *type)       { return m_Parent ? m_Parent->lw_le(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_concat(lua_State *L, T *_this, LuaType *type)   { return m_Parent ? m_Parent->lw_concat(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_use(lua_State *L, T *_this, LuaType *type)      { return m_Parent ? m_Parent->lw_use(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }
	virtual int lw_recycle(lua_State *L, T *_this, LuaType *type)  { return m_Parent ? m_Parent->lw_recycle(L, m_Parent->lw_to_type(lw_from_type(_this)), type) : 0; }

	virtual int lw_do(lua_State *L, void *_this, LuaType *type, int what, void *data)  { return m_Parent ? m_Parent->lw_do(L, m_Parent->lw_to_type(lw_from_type(_this)), type, what, data) : 0; }
	virtual int lw_do(lua_State *L, void *_this, LuaType *type, const char *what, void *data)  { return m_Parent ? m_Parent->lw_do(L, m_Parent->lw_to_type(lw_from_type(_this)), type, what, data) : 0; }

	virtual void *lw_to_type(void *_this)  { return _this; }
	virtual void *lw_from_type(void *_this)  { return _this; }
};


#define _EXPAND(x) x
#define _JOIN(x,y) _EXPAND(x) ## _EXPAND(y)

#define DECLARE_SIMPLE_LUATYPE(cl) \
	friend class _JOIN(LuaType_,cl);

#define DECLARE_LUATYPE(cl) DECLARE_SIMPLE_LUATYPE(cl) \
	static LuaType *s_LuaType; \
	virtual INT_PTR lw_push(lua_State *L);

#define BEGIN_SIMPLE_LUATYPE(name, cl, basetype) \
class _JOIN(LuaType_,cl) : public LuaTypeT<cl> \
{ \
public: \
	_JOIN(LuaType_,cl)() : LuaTypeT<cl>(name, basetype, (_LuaType::LuaFuncTable *)FuncTable, (_LuaType::LuaMemberTable *)MemberTable, 0, 0) {}; \
	typedef int (_JOIN(LuaType_,cl)::*LUAFUNC)(lua_State *L, cl *_this, LuaType *type); \
	typedef int (_JOIN(LuaType_,cl)::*LUAMEMBER)(lua_State *L, cl *_this, LuaType *type, bool set); \
	static struct LuaFuncTable { const char *m_Name; LUAFUNC m_Func; uint32 m_Hash; } FuncTable[]; \
	static struct LuaMemberTable { const char *m_Name; LUAMEMBER m_Func; uint32 m_Hash; } MemberTable[];
	
#define END_SIMPLE_LUATYPE(cl) \
	};\
static _JOIN(LuaType_,cl) _JOIN(s_LuaType_,cl);

#define BEGIN_LUATYPE(name) BEGIN_SIMPLE_LUATYPE(name, ThisClass, BaseClass::s_LuaType)
	
#define END_LUATYPE() END_SIMPLE_LUATYPE(ThisClass) \
	LuaType *ThisClass::s_LuaType = (LuaType *) &_JOIN(s_LuaType_,ThisClass); \
	INT_PTR ThisClass::lw_push(lua_State *L) { return L ? s_LuaType->lw_push(L, this) : (INT_PTR)s_LuaType; }

#define LW_GET_MEMBER(type, name) \
	if (strcmp(key, #name) == 0) \
	{ \
		lua_push##type(L, _this->name); \
		return 1; \
	}

#define LW_SET_MEMBER(type, name) \
	if (strcmp(key, #name) == 0) \
	{ \
		_this->name = lua_to##type(L, 2); \
		return 0; \
	}


#pragma pack(pop)

#endif // _LuaWrap_h_
