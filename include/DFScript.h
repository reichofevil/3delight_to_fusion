#if ((!defined(_DFScript_h_) && defined(ThisClass)) || ((!defined(_DFScript_h_No_Class_) && !defined(ThisClass))))

#pragma pack(push, 8)

#if defined(ThisClass)
	#define _DFScript_h_
	#if defined(_DFScript_h_No_Class_)
		#define _DFScript_h_Second_Time_
	#endif
#else
	#define _DFScript_h_No_Class_
	#if defined(_DFScript_h_)
		#define _DFScript_h_Second_Time_
	#endif
#endif

// re-include debug.h if need be
#if defined(_DFScript_h_) && defined(_Debug_h_No_Module_)
	#if !defined(MODULENAME) // No MODULENAME?  Use ThisClass
		#define ___STR(x) #x
		#define __STR(x) ___STR(x)
		#define MODULENAME __STR(ThisClass)
	#endif
	#include "Debug.h"
#endif

#include "DFID.h"

#if (!defined(EYEONSCRIPT_NO_LUA) && (!defined(FuPLUGIN) || defined(FuPLUGIN_SCRIPTING)))
#ifndef NO_PRAGMA_LIB
	#if _MSC_VER >= 1600
		#ifdef _M_AMD64
			#ifdef _DEBUG
				#pragma comment(lib, "U:/RCS/Pipeline/Lua511/vc10/Debug64/Lua5.1.lib")
			#else
				#pragma comment(lib, "U:/RCS/Pipeline/Lua511/vc10/Release64/Lua5.1.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib, "U:/RCS/Pipeline/Lua511/vc10/Debug/Lua5.1.lib")
			#else
				#pragma comment(lib, "U:/RCS/Pipeline/Lua511/vc10/Release/Lua5.1.lib")
			#endif
		#endif
	#else
		#ifdef _M_AMD64
			#ifdef _DEBUG
				#pragma comment(lib, "U:/RCS/Pipeline/Lua511/Debug64/Lua5.1.lib")
			#else
				#pragma comment(lib, "U:/RCS/Pipeline/Lua511/Release64/Lua5.1.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib, "U:/RCS/Pipeline/Lua511/Debug/Lua5.1.lib")
			#else
				#pragma comment(lib, "U:/RCS/Pipeline/Lua511/Release/Lua5.1.lib")
			#endif
		#endif
	#endif
#endif
#endif

#ifndef HAS_DEBUG_NEW
#if defined(_DEBUG)

#include <malloc.h>
#include <stdlib.h>
#include <crtdbg.h>

inline void* __cdecl operator new(size_t nSize, const char *lpszFileName, int nLine)
{
	return _malloc_dbg(nSize, _NORMAL_BLOCK, lpszFileName, nLine);
}

inline void __cdecl operator delete(void* p, const char *lpszFileName, int nLine)
{
	_free_dbg(p, _NORMAL_BLOCK);

	UNREFERENCED_PARAMETER(nLine);
	UNREFERENCED_PARAMETER(lpszFileName);
}


#undef DEBUG_NEW
#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW
#define malloc(x)			_malloc_dbg(x, _NORMAL_BLOCK, __FILE__, __LINE__)
#define calloc(x, y)		_calloc_dbg(x, y, _NORMAL_BLOCK, __FILE__, __LINE__)
#define realloc(x, y)	_realloc_dbg(x, y, _NORMAL_BLOCK, __FILE__, __LINE__)

#define HAS_DEBUG_NEW

#endif // _DEBUG
#endif

#include <malloc.h>
#include <string.h>

#ifndef ThisClass
//#define ThisClass void
//#error "'ThisClass' must be defined before including DFScript.h!"
#endif

#ifndef _TYPES_H_
typedef long               int32;
typedef unsigned long     uint32;
typedef unsigned __int64  uint64;
typedef double            float64;

#ifdef _WIN64
typedef uint64 Tag;
#else
typedef uint32 Tag;
#endif
#endif

#ifndef NULL
#define NULL  0
#endif

typedef void *HANDLE;

class ScriptVal;
struct Packet;
class ScriptInfo;
class ScriptSymbol;

#define _EXPAND(x) x
#define _JOIN(x,y) _EXPAND(x) ## _EXPAND(y)

#define THIS_SCRIPTINFO _JOIN(__ScriptInfo_, ThisClass)
#define THISCLASS_SCRIPTING _JOIN(ThisClass, _Scripting)


extern "C" FuSCRIPT_API char *FormatAssertMsg(const char *format, ...);
extern "C" FuSCRIPT_API int AssertDlg(const char *file, int line, const char *cond, char *msg);


#ifdef _WIN64
	#define _FuASSERTbreak()	{	*(char *)0 = 0; }
#else
	#define _FuASSERTbreak()	{ __asm int 3 }
#endif

// This one is always available
#define _FuASSERT(cond, msg) \
{\
	static bool enabled = true;\
	if (!(cond) && enabled)\
	{\
		int ret = AssertDlg(__FILE__, __LINE__, #cond, FormatAssertMsg ## msg );\
		if (ret == IDCANCEL)\
			_FuASSERTbreak()\
		else if (ret == IDCONTINUE)\
		{\
			enabled = false;\
		}\
	}\
}

#if !defined(RELEASE_VERSION) || defined(DEV_VERSION) || defined(DO_NOT_DISTRIBUTE)
	#define FuASSERT(cond, msg) _FuASSERT(cond, msg)
#else
	#define FuASSERT(cond, msg) NULL
#endif

#if defined(USE_NOSCRIPTING) || defined(_XBOX)
	// In a non-scripting or xbox config, we're likely linked straight in
	#define FuSCRIPT_API
#else
	#ifndef FuSCRIPT_API
		#ifdef DFSCRIPTDLL_EXPORTS
			#define FuSCRIPT_API __declspec(dllexport)
		#else
			#define FuSCRIPT_API __declspec(dllimport)
		#endif
	#endif
#endif

class ScriptInfo;

#ifndef _DFScript_h_Second_Time_

#include "LuaWrap.h"

// Abstract baseclass for anything scriptable
class FuSCRIPT_API ScriptObject : public LuaClass
{
public:
	ScriptObject();
	virtual ~ScriptObject();

	static int lw_create(lua_State *L);

	// Just in case...
	virtual bool _Do(uint32 what, ScriptVal &args);
	
	virtual void IncRef() = 0;
	virtual void DecRef() = 0;
	
	virtual ScriptInfo *GetScriptInfo() const = 0;
	virtual void SetScriptInfo(ScriptInfo *si) = 0;

	virtual void GetObjectInfo(const ScriptVal &what, ScriptVal &val);
	virtual bool DoInfo(const ScriptVal &what, ScriptVal &val, const FuID &topic);
};

#endif

#include "ScriptVal.h"


#ifndef USE_NOSCRIPTING

#ifndef _DFScript_h_Second_Time_
enum SCFlags
{
	SC_SERVER = 1 << 0,
	SC_CLIENT = 1 << 1,
};
#endif

class RemoteApp;
class RemoteObject;

#define SCRIPTCONFIG_VERSION 1

#ifndef _DFScript_h_Second_Time_

struct ScriptConfig
{
	// Members
	int Version;
	int Flags;
	char *Name;
	void *UserData;

	// Callbacks
	bool (*IsDebugMode)(void *data);
	ScriptObject *(*GetApp)(void *data);

	void (*OnConnect)(void *data, RemoteApp *app);
	void (*OnDisconnect)(void *data, RemoteApp *app);
	void (*OnConnectFailed)(void *data, RemoteApp *app);

	bool (*FromLua)(lua_State *L, int index, ScriptVal &val, void *arg);
	bool (*ToLua)(RemoteApp *app, lua_State *L, ScriptVal &val, void *arg);
};
#endif

extern FuSCRIPT_API const ScriptVal &GetAppUUID();
extern FuSCRIPT_API void ReturnCustomData(ScriptArgs &args, const char *name, const ScriptVal &table);

#define CODEARGS_Execute 0x80000000 // Execute returned function immediately


// Bottom 8 bits are a type
#ifndef _DFScript_h_Second_Time_
enum ScriptSymbolTypes
{
	SST_Any            = 0,   // Used only for FindSymbol() to match any type - not a valid symbol type.

	SST_Function       = 1,   // A function
	SST_Variable,             // A variable (can be read-only, write-only or read-write)
	SST_Constant,             // A Constant
	SST_Help,                 // A help topic, without any associated code

	// Script Method overrides
	// Values above 128 are single instance symbols (only one of each allowed per class)
	// and will be dispatched using an array for speed, rather than symbol lookup into the list
	SST_Create = 128,               // 128
	SST_Call,                       // 129
	SST_GetTable,                   // 130
	SST_SetTable,                   // 131
	SST_Add,                        // 132
	SST_Sub,                        // 133
	SST_Mul,                        // 134
	SST_Div,                        // 135
	SST_Pow,                        // 136
	SST_Unm,                        // 137
	SST_EQ,                         // 138
	SST_LE,                         // 139
	SST_LT,                         // 140
	SST_Concat,                     // 141
	SST_ToString,                   // 142
	SST_CallFunction,               // 143
	SST_CallMemberFunction,         // 144
	SST_GetSetAttrs,                // 145
	SST_GetCustomData,              // 146
	SST_SetCustomData,              // 147
	SST_GetObject,                  // 148
	SST_OnConnect,                  // 149
	SST_OnConnectFailed,            // 150
	SST_OnDisconnect,               // 151
	SST_IsDebug,                    // 152

	// Catch any over-run
	SST_Last = 255,
};
#endif

#define SST_GetSetAttrs_GUIR (SST_GetSetAttrs|SST_GUIThreadR)
#define SST_GetSetAttrs_GUIW (SST_GetSetAttrs|SST_GUIThreadW)
#define SST_GetSetAttrs_GUIRW (SST_GetSetAttrs|SST_GUIThread)

#define SST_GetCustomData_GUI (SST_GetCustomData|SST_GUIThread)
#define SST_SetCustomData_GUI (SST_SetCustomData|SST_GUIThread)

#define SST_PacketOffset    47 // Adjustment to convert old-style events to new-sty


#define SST_TypeMask 0x000000ff

// Next 8 bits are type defined flags
// SST_Variable flags
#define SST_VarRead       (1<<8)
#define SST_VarWrite      (1<<9)

// Top 16 bits are global flags

#define SST_CodeTypeMask  (0xf << 16) // 4 bits for code type
#define SST_CodeType_Any  (0x0 << 16) // Works with any code-type
#define SST_CodeType_Lua  (0x1 << 16) // Lua specific function

#define SST_GUIThreadR    (1<<20) // Must be called on the GUI thread for read
#define SST_GUIThreadW    (1<<21) // Must be called on the GUI thread for write
#define SST_GUIThreadRW   (SST_GUIThreadR|SST_GUIThreadW)
#define SST_GUIThread     SST_GUIThreadRW
#define SST_Hidden        (1<<22) // Don't display symbol in help etc

#define SST_Debug         (1<<31) // Symbol used for debug purposes; not available in general scripting

typedef void (*SCRIPTFUNC)(void *obj, ScriptArgs &args, ScriptSymbol *sym, bool set);

#define SS_VERSION 1 // Currently at version 1 of ScriptSymbol

#ifndef _DFScript_h_Second_Time_
class FuSCRIPT_API ScriptSymbol
{
public:
	uint32 m_Version;

	ScriptSymbol *m_Next;
	ScriptInfo *m_Owner;

	const char *m_Name;
	uint32 m_Type;
	SCRIPTFUNC m_Func;
	const char *m_ShortHelp;
	const char *m_LongHelp;

public:
	ScriptSymbol(uint32 ver, const char *name, uint32 type, SCRIPTFUNC func, ScriptInfo &si, const char *shortstr, const char *longstr);
	
	void Call(ScriptObject *obj, ScriptArgs &args, bool set);
};
#endif

#define SI_VERSION 1 // Currently at version 1 of ScriptInfo

#ifndef _DFScript_h_Second_Time_
class FuSCRIPT_API ScriptInfo
{
public:
	uint32 m_Version;

	ScriptInfo *m_Next;
	ScriptInfo *m_Parent;

	const char *m_Name;
	const char *m_ShortHelp;
	const char *m_LongHelp;

	ScriptSymbol *m_Symbols;

	ScriptSymbol *m_Methods[128];
	
	ScriptVal m_ActionTable;

public:
	ScriptInfo(uint32 ver, const char *name, const char *shorthelp, const char *longhelp);
	~ScriptInfo();
	
	ScriptSymbol *FindSymbol(const char *name, uint32 type = SST_Any);

	virtual void *_FromObj(void *obj) = NULL;
	virtual void *_ToObj(void *obj)   = NULL;
	
	void *FromObj(ScriptObject *obj) { return _FromObj((void *)obj); }
	ScriptObject *ToObj(void *obj) { return (ScriptObject *) _ToObj(obj); }
};
#endif



#ifndef _DFScript_h_Second_Time_
template <class Type> class ScriptInfoT : public ScriptInfo
{
//TODO:	FuDeclareClass(ScriptInfoT); // declare ClassID static member, etc.

public:
	ScriptInfoT(uint32 ver, const char *name, const char *shorthelp, const char *longhelp) : ScriptInfo(ver, name, shorthelp, longhelp) {}

	virtual void *_FromObj(void *obj) { return (Type *)(ScriptObject *)obj; }
	virtual void *_ToObj(void *obj) { return (ScriptObject *)((Type *)obj); }
};
#endif

void FuSCRIPT_API _SetScriptInfo(void *obj, ScriptInfo &si);
void FuSCRIPT_API _InitScriptInfo(ScriptInfo &si, ScriptInfo *parent);

#ifndef USE_NOSCRIPTING
#define ENABLESCRIPTING(obj) _SetScriptInfo(obj, THIS_SCRIPTINFO)

#define SCRIPTCLASS(name, shortstr, longstr) \
	static ScriptInfoT<ThisClass> THIS_SCRIPTINFO (SI_VERSION, name, shortstr, longstr)
#endif






bool FuSCRIPT_API DoHelp(ScriptArgs &args, ScriptInfo *info, const char *name);

#define BEGIN_SCRIPTING() \
	class THISCLASS_SCRIPTING : public ThisClass \
	{ \
	public: \
		class Scripting \
		{ \
		public: \
			Scripting() {};

#define END_SCRIPTING() \
		}; \
		friend class Scripting; \
	}; \
	static THISCLASS_SCRIPTING::Scripting THISCLASS_SCRIPTING;

#define SCRIPT(name, type, shortstr, longstr) \
	class cl_##name \
	{ \
	public: \
		cl_##name() \
		{ \
			static ScriptSymbol _SS_##name(SS_VERSION, #name, type, (SCRIPTFUNC)_SF_##name, THIS_SCRIPTINFO, shortstr, longstr);\
		} \
	} _##name; \
	static void _SF_##name(THISCLASS_SCRIPTING *_this, ScriptArgs &args, ScriptSymbol *sym, bool set)


#define SCRIPTCONST(name, type, value, shortstr, longstr) \
	class cl_##name \
		{ \
		public: \
			cl_##name() \
			{ \
				static ScriptSymbol _SS_##name(SS_VERSION, #name, SST_Constant, (SCRIPTFUNC)_SF_##name, THIS_SCRIPTINFO, shortstr, longstr);\
			} \
		} _##name; \
	static void _SF_##name(THISCLASS_SCRIPTING *_this, ScriptArgs &args, ScriptSymbol *sym, bool set) \
	{ \
		args.Return ## type (value); \
	} 

#define SCRIPTMETHOD(type) \
	class cl_##type \
	{ \
	public: \
		cl_##type() \
		{ \
			static ScriptSymbol _SS_##type(SS_VERSION, NULL, type, (SCRIPTFUNC)_SF_##type, THIS_SCRIPTINFO, "", "");\
		} \
	} _##type; \
	static void _SF_##type(THISCLASS_SCRIPTING *_this, ScriptArgs &args, ScriptSymbol *sym, bool set)

#define SCRIPTHELP(name, shortstr, longstr) \
	static ScriptSymbol _SS_##name(SS_VERSION, #name, SST_Help, NULL, THIS_SCRIPTINFO, shortstr, longstr);


#define STYPE_Number (double)
#define STYPE_Boolean FALSE != 

#define SCRIPTDEBUG(member, stype, ctype) \
	SCRIPT(member, SST_Variable|SST_VarRead|SST_VarWrite|SST_Debug, "Member Variable (" #stype ")", "") \
	{ \
		if (set) \
		{ \
			if (args[1].Is##stype()) \
				_this->member = ctype (args[1].To##stype()); \
		} \
		else \
			args.Return##stype (STYPE_##stype(_this->member)); \
	}

#define SCRIPTDEBUG_Integer(member) SCRIPTDEBUG(member, Number, int32)
#define SCRIPTDEBUG_Number(member) SCRIPTDEBUG(member, Number, double)
#define SCRIPTDEBUG_Boolean(member) SCRIPTDEBUG(member, Boolean, bool)
//#define SCRIPTDEBUG_String(member) __SCRIPTDEBUG(String, member)

#define SCRIPTDEBUG_Object(member) \
	SCRIPT(member, SST_Variable|SST_VarRead|SST_VarWrite|SST_Debug, "Member Variable (Object)", "") \
	{ \
		if (set) \
			args.SetError(SERR_Other, "You can't assign a member object!"); \
		else \
			args.ReturnObject((ScriptObject *)&(_this->member)); \
	}

#define SCRIPTDEBUG_ObjPtr(member) \
	SCRIPT(member, SST_Variable|SST_VarRead|SST_VarWrite|SST_Debug, "Member Variable (ObjPtr)", "") \
	{ \
		if (set) \
		{ \
			ScriptObject *ptr = args[1].ToLocalObject(); \
			((void *&)_this->member) = ((ScriptInfo *)(_this->member->GetScriptInfo()))->FromObj(ptr); \
		} \
		else \
			args.ReturnObject((ScriptObject *)_this->member); \
	}


#define CALLPARENT() \
{ \
	if (ScriptInfo *si = (ScriptInfo *) sym->m_Owner->m_Parent) \
	{ \
		ScriptSymbol *ss = (ScriptSymbol *) si->FindSymbol(sym->m_Name, sym->m_Type); \
		if (ss) \
			ss->Call(((ScriptInfo *)sym->m_Owner)->ToObj(_this), args, set); \
	} \
}


/*DFSTODO:
void FuSCRIPT_API _SetScriptData(ScriptVal &val, TagList *tags, const char *name);
ScriptVal FuSCRIPT_API _GetScriptData(TagList *tags, const char *name);
*/

#ifndef _DFScript_h_Second_Time_
/*DFSTODO:
inline void  SetScriptData(ScriptVal &val, TagList *tags, const char *name) { _SetScriptData(val, tags, name); }
inline ScriptVal GetScriptData(TagList *tags, const char *name) { return (const ScriptVal &) _GetScriptData(tags, name); }
*/
#endif

extern FuSCRIPT_API ScriptInfo *g_ScriptInfoList;
extern FuSCRIPT_API ScriptInfo *g_RootScriptInfo;

class RemoteApp;

extern FuSCRIPT_API struct ScriptConfig *g_ScriptConfig;

FuSCRIPT_API void CleanupScripting();
FuSCRIPT_API void InitScripting(ScriptConfig &cfg);

FuSCRIPT_API bool StartScripting(HANDLE hinstance, const ScriptVal &_info);
FuSCRIPT_API void StopScripting();

FuSCRIPT_API void _RemoveScriptObject(void *obj);

FuSCRIPT_API uint32 GetHashValue(const char *str);

FuSCRIPT_API void StripName(char *dst, const char *src);
FuSCRIPT_API bool CompareName(const char *str1, const char *str2);
FuSCRIPT_API bool IsValidName(const char *name);

FuSCRIPT_API void StrFromIP(uint32 ipaddr, char *ipstr, int buflen);
FuSCRIPT_API uint32 IPFromStr(const char *ipstr);
FuSCRIPT_API bool IsLocalIP(const char *ipstr);
FuSCRIPT_API bool IsLocalIP(uint32 ipaddr);

FuSCRIPT_API const char *GetSerializePrologue();
FuSCRIPT_API bool FileExists(const char *path);
FuSCRIPT_API bool ConvertToUNCPath(const char *path, char *dest);

FuSCRIPT_API int stricoll_smart(const char *string1, const char *string2);
FuSCRIPT_API int wcsicoll_smart(const wchar_t *string1, const wchar_t *string2);

#ifdef LINUX_TEST
FuSCRIPT_API void ConvertWindowsToLinuxFilename(const char *windows, char *linux);
FuSCRIPT_API void ConvertLinuxToWindowsFilename(const char *linux, char *windows);
#endif

#ifndef _DFScript_h_Second_Time_
FuSCRIPT_API void SendNotify(const ScriptVal &who, const char *what, ScriptObject *from, const ScriptVal &data1 = nil, const ScriptVal &data2 = nil, const ScriptVal &data3 = nil, const ScriptVal &data4 = nil, const ScriptVal &data5 = nil, const ScriptVal &data6 = nil, const ScriptVal &data7 = nil, const ScriptVal &data8 = nil);
#endif


#ifndef _DFScript_h_Second_Time_
inline void RemoveScriptObject(ScriptObject *obj)
{
	_RemoveScriptObject(obj);
}
#endif

// Script Notify/Event Stuff

/*
FuSCRIPT_API bool _SendScriptEvent(void *obj, const char *event, ScriptVal &msg);

inline bool SendScriptEvent(ScriptObject *obj, const char *event, ScriptVal &msg)
{
	return _SendScriptEvent(obj, event, msg);
}
*/

// Tag Mapping

#ifndef _DFScript_h_Second_Time_
class FuSCRIPT_API MapTag
{
public:
	ScriptValType m_Type;
	Tag m_Tag;
	char *m_Name;

	uint32 m_Children;
	MapTag *m_Child;

public:
	MapTag();
	MapTag(ScriptValType type, char *name, Tag tag);
	MapTag(const MapTag &map);
	
	const MapTag &operator =(const MapTag &map);
	
	~MapTag();
};

class FuSCRIPT_API MapBoolean : public MapTag
{
public:
	MapBoolean(char *name, Tag tag);
};

class FuSCRIPT_API MapNumber : public MapTag
{
public:
	MapNumber(char *name, Tag tag);
};

class FuSCRIPT_API MapString : public MapTag
{
public:
	MapString(char *name, Tag tag);
};


/*DFSTODO:
class FuSCRIPT_API MapTagList : public MapTag
{
public:
	MapTagList();
	MapTagList(char *name, Tag tag, const MapTag child, ...);
	MapTagList(char *name, Tag tag, const MapTagList *ptr);

	bool FromTags(TagList &tags, ScriptVal &val);
	bool ToTags(TagList &tags, ScriptVal &val);
};


class FuSCRIPT_API MapTags : public MapTagList
{
public:
	MapTags(const MapTag child, ...);
};

extern FuSCRIPT_API MapTag EndMap;
*/
#endif

#else
#define ENABLESCRIPTING(x)
#define SCRIPTCLASS(x, y, z) static int THIS_SCRIPTINFO;
#endif

#pragma pack(pop)

#ifndef _DFScript_h_Second_Time_
// Simple lock
class FuSCRIPT_API Lock
{
	void *m_Data;
#ifdef _DEBUG
	uint32 m_ThreadID;
	int32 m_Count;
#endif

public:
	Lock();
	~Lock();

	void ObtainLock();
	void ReleaseLock();
};
#endif


#endif
