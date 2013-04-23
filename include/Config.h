#ifndef _CONFIG_H_
#define _CONFIG_H_

#pragma pack(push, 8)

#ifdef unix

#define FuSYSTEM_API
#define DLLExtern extern

#define _winver 0x0500
#define _MSC_VER 0x1300

#define GetWindowTextAU(wnd, buf, len) ::GetWindowTextW(wnd, (LPWSTR)(LPSTR)buf, len)
#define SetWindowTextAU(wnd, buf) ::SetWindowTextW(wnd, (LPCWSTR)(LPCSTR)buf)
#define _winmajor 4
#define _winminor 0

#define DIR_SEP_S "/"
#define DIR_SEP_C '/'
#define ALT_DIR_SEP_S "\\"
#define ALT_DIR_SEP_C '\\'

#define DEFAULT_TEXT_EDITOR		"gedit"

//#define wcscpy ERROR_wcscpy
//#define wcslen ERROR_wcslen

#ifndef WCHAR
typedef wchar_t WCHAR;
#endif

#define WCSCPY wcscpy
#define WCSLEN wcslen
#define WCSCHR wcschr

#define _getcwd(buf, len) (buf[0] = 0)

/*	#if 0 //defined(__cplusplus)
inline WCHAR *WCSCPY(WCHAR *dest, const WCHAR *src)
{
	WCHAR *ptr = dest;

	do
	{
		*ptr++ = *src++;
	} while (*src);

	return dest;
}

inline int WCSLEN(const WCHAR *str)
{
	const WCHAR *ptr = str;
	while (*ptr != 0)
		ptr++;

	return ptr - str;
}

inline WCHAR * WCSCHR(const WCHAR *str, WCHAR ch)
{
	WCHAR *ptr = (WCHAR *) str;
	while (*ptr != 0)
	{
		if (*ptr == ch)
			return ptr;

		ptr++;
	}

	return NULL;
}
// #endif
*/

#define _mm_load_ps		_mm_loadu_ps
#define _mm_store_ps		_mm_storeu_ps
#define _mm_stream_ps	_mm_storeu_ps

#else		// Windows
#if defined(PIPELINE_NODLL) || defined(SPLIT_DFSYSTEM)
	#define FuSYSTEM_API
	#define DLLExtern extern
#elif PIPELINE_DLL
	#define FuSYSTEM_API DLLExport
	#define DLLExtern extern DLLExport
#else
	#define FuSYSTEM_API DLLImport
	#define DLLExtern DLLImport
#endif

#ifdef WINCE
static char *getenv(const char *name) { return ""; }

#define stricmp			_stricmp
#define __iscsym(c)		(isalnum(c)||((c)=='_')) 
#define __iswcsym(c)		(iswalnum(c)||((c)=='_')) 
#define __iscsymf(c)		(isalpha(c)||((c)=='_')) 
#define __iswcsymf(c)	(iswalpha(c)||((c)=='_')) 

#define SetThreadIdealProcessor(x,y)
#define WaitForSingleObjectEx(x,y,z)	WaitForSingleObject(x,y)
#endif

#define WCSCPY wcscpy
#define WCSLEN wcslen
#define WCSCHR wcschr

#ifdef LINUX_TEST
  #define DIR_SEP_S "/"
  #define DIR_SEP_C '/'
  #define ALT_DIR_SEP_S "\\"
  #define ALT_DIR_SEP_C '\\'
#else
  #define DIR_SEP_S "\\"
  #define DIR_SEP_C '\\'
  #define ALT_DIR_SEP_S "/"
  #define ALT_DIR_SEP_C '/'
#endif

#define DEFAULT_TEXT_EDITOR		"notepad.exe"

#define GetWindowTextAU(wnd, buf, len) ::GetWindowTextA(wnd, (LPSTR)buf, len)
#define SetWindowTextAU(wnd, buf) ::SetWindowTextA(wnd, (LPCSTR)buf)

#if __INTEL_COMPILER >= 600
	#ifdef InterlockedDecrement
	#undef InterlockedDecrement
	#endif
	#define InterlockedDecrement(x)								_InterlockedDecrement((LPLONG)(x))

	#ifdef InterlockedCompareExchange
	#undef InterlockedCompareExchange
	#endif
	#define InterlockedCompareExchange(x,y,z)					_InterlockedCompareExchange((LPLONG)(x),(LONG)(y),(LONG)(z))

	#ifdef InterlockedCompareExchangePointer
	#undef InterlockedCompareExchangePointer
	#endif
	#ifdef _WIN64
		#define InterlockedCompareExchangePointer(x,y,z)	_InterlockedCompareExchangePointer(x,y,z)
	#else
		#define InterlockedCompareExchangePointer(x,y,z)	(void *)_InterlockedCompareExchange((LPLONG)(x),(LONG)(y),(LONG)(z))
	#endif

	#ifdef InterlockedExchange
	#undef InterlockedExchange
	#endif
	#define InterlockedExchange(x,y)								_InterlockedExchange((LPLONG)(x),(LONG)(y))

	#ifdef InterlockedExchangeAdd
	#undef InterlockedExchangeAdd
	#endif
	#define InterlockedExchangeAdd(x,y)							_InterlockedExchangeAdd((LPLONG)(x),(LONG)(y))

	#ifdef InterlockedExchangePointer
	#undef InterlockedExchangePointer
	#endif
	#ifdef _WIN64
		#define InterlockedExchangePointer(x,y)				_InterlockedExchangePointer(x,y)
	#else
		#define InterlockedExchangePointer(x,y)				(void *)_InterlockedExchange((LPLONG)(x),(LONG)(y))
	#endif

	#ifdef InterlockedIncrement
	#undef InterlockedIncrement
	#endif
	#define InterlockedIncrement(x)								_InterlockedIncrement((LPLONG)(x))
#endif

#define __endexcept

#endif

#if defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC) || _MSC_VER >= 1400
#else
    #define __restrict
#endif

#pragma pack(pop)

#endif
