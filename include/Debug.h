#if ((!defined(_Debug_h_) && defined(MODULENAME)) || ((!defined(_Debug_h_No_Module_) && !defined(MODULENAME))))

#if defined(MODULENAME)
	#define _Debug_h_
	#if defined(_Debug_h_No_Module_)
		#define _Debug_h_Second_Time_
	#endif
#else
	#define _Debug_h_No_Module_
	#if defined(_Debug_h_)
		#define _Debug_h_Second_Time_
	#endif
#endif

#if !defined(_Debug_h_Second_Time_)

#include "Types.h"

class List;

#define dprintf(args)  _dprintf(1, __FILE__, __LINE__, args)
#define dprintf1(args) _dprintf(1, __FILE__, __LINE__, args)
#define dprintf2(args) _dprintf(2, __FILE__, __LINE__, args)
#define dprintf3(args) _dprintf(3, __FILE__, __LINE__, args)

#define ddump(buf, len)   _ddump(1, buf, len)
#define ddump1(buf, len)  _ddump(1, buf, len)
#define ddump2(buf, len)  _ddump(2, buf, len)
#define ddump3(buf, len)  _ddump(3, buf, len)

#define dflush()	_dflush()

// define an assert that unconditionally breaks, rather than wasting time popping up an assert messagebox which can cause nested/reentrant asserts
#ifdef _DEBUG
#if !defined(LINUX_TEST) && !defined(_WIN64)
	#define uassert(x) { if (!(x)) { __asm int 3 } }
#else
//#include "DFScript.h"
#define uassert(x) if (!(x)) { FuASSERT(0, ("")); }
#endif
#else
	#define uassert(x)
#endif

class RegistryList;
FuSYSTEM_API extern RegistryList *DebugList;

#define OLD_REG_DEBUG TAG_INT + 1000
#define REG_DEBUG TAG_PTR + 1000

FuSYSTEM_API void SetThreadName(const char *name,  int32 id = -1);

#endif //!defined(_Debug_h_Second_Time_)

#if defined(RELEASE_VERSION) || defined(unix)
#if !defined(_Debug_h_Second_Time_)
	#define Status(msg)
	
#ifdef _STDAFX_H_
	inline CString GetStackTrace(uint32 flags = 0, char *sep = "\n", int maxdepth = 512) { return CString(""); }
#endif

	#define _dprintf(lev, file, line, args)
	#define _ddump(lev, buf, len)
	#define _dflush()
#endif //!defined(_Debug_h_Second_Time_)
#else
#if !defined(_Debug_h_Second_Time_)
	DLLExtern void Status(char *msg);
#ifdef _STDAFX_H_
	DLLExtern CString GetStackTrace(uint32 flags = 0, char *sep = "\n", int maxdepth = 512);
#endif
#endif //!defined(_Debug_h_Second_Time_)

	#if defined(MODULENAME) && !defined(SPLIT_DFSYSTEM) //TODO: Fix
		#undef _dprintf
		#undef _ddump
		#undef _dflush
		#define _dprintf(lev, file, line, args) if (debuglev >= lev)\
								{ int lasterr = GetLastError();\
									WaitForSingleObject(DebugMutex, INFINITE);\
									__dprintfx ## args ;\
									if (DebugLineInfo)\
										__dprintfl(file, line);\
									else __dprintfl(NULL, 0);\
									ReleaseMutex(DebugMutex);\
									SetLastError(lasterr);\
								} else NULL
		#define _ddump(lev, buf, len) if (debuglev >= lev) __ddump(buf, len); else NULL
		#define _dflush() __dflush()

		static int debuglev = 0;
		
		#if defined(REGISTRY_MAGIC)
			#undef FU_DECLARE_DEBUG_REGISTRY
			static Registry db_reg(&DebugList, MODULENAME, REG_DEBUG, &debuglev, TAG_DONE);
		#else
			#define FU_DECLARE_DEBUG_REGISTRY
		#endif // !defined(_Debug_h_Second_Time_)
	#else
		#define _dprintf(lev, file, line, args)
		#define _ddump(lev, buf, len)
		#define _dflush()
	#endif

#if !defined(_Debug_h_Second_Time_)
	DLLExtern void __dprintf(char *fmt, ...);
	DLLExtern void __dprintfx(char *fmt, ...);
	DLLExtern void __dprintfl(char *file, int line);
	DLLExtern void __ddump(void *buf, int len);
	DLLExtern void __dflush();
	DLLExtern bool DebugLineInfo;
	DLLExtern HANDLE DebugMutex;
#endif //!defined(_Debug_h_Second_Time_)
#endif

#ifdef DEBUGNAME
	#define _dprintf(lev, file, line, args)
	#define _ddump(lev, buf, len)
	#define _dflush()
#endif

#if !defined(_Debug_h_Second_Time_)
// Environment Variable flag
class EnvFlag
{
protected:
	bool m_Value;

public:
	EnvFlag(char *name, bool def = false)
	{
		char *str = getenv(name);

		m_Value = (str ? (stricmp(str, "true") == 0) : def);
	}
	
	operator bool() const { return m_Value; }
};
#endif //!defined(_Debug_h_Second_Time_)

/* In DFScript.h
#define FuASSERT(cond, msg) \
{\
	static bool enabled = true;\
	if (!(cond) && enabled)\
	{\
		int ret = AssertMsg ## msg;\
		if (ret == IDCANCEL)\
		{\
			{ __asm int 3 };\
		}\
		else if (ret == IDCONTINUE)\
		{\
			enabled = false;\
		}\
	}\
}
*/

#endif // _Debug_h_

