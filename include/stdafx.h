// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#ifndef NO_STDAFX

#include "dfmath.h"

#ifdef _XBOX
#include "xtl.h"
#else
#ifndef LINUX
#ifndef NO_MFC

#ifndef _STDAFX_H_
#define _STDAFX_H_

#ifdef USE_DFPLUGIN

#define COMPANY_ID 0
#include "../DFPlugin/DFPlugin.h"

#else

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef WINCE
	#if (_MSC_VER >= 1300 || defined(_WIN64))
	// Stupid MFC incompatibilities :(
	#define WINVER			0x0501
	#define _WIN32_WINNT	0x0501
	#define _WIN32_IE		0x0600
	#else
	#define WINVER			0x0400
	#define _WIN32_WINNT	0x0400
	#define _WIN32_IE		0x0500
	#endif
#else
	#define WINVER _WIN32_WCE

	#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

	#include <ceconfig.h>
	#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	#define SHELL_AYGSHELL
	#endif
#endif

#define COMPANY_ID        ""   
#define COMPANY_ID_DOT    ""   // to make it easier for 3D tools be part of both Fu3D.plugin and the 3D SDK
#define COMPANY_ID_SPACE  ""
#define COMPANY_ID_DBS    ""   // double back slash \\

#include <afxwin.h>         // MFC core and standard components

#include <winsock2.h>

#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#ifdef WINCE
	#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	#ifndef _DEVICE_RESOLUTION_AWARE
	#define _DEVICE_RESOLUTION_AWARE
	#endif
	#endif

	#ifdef _DEVICE_RESOLUTION_AWARE
	#include "DeviceResolutionAware.h"
	#endif

	#include <aygshell.h>
	#pragma comment(lib, "aygshell.lib") 

	// TODO: temporary code, until the CE compilers correctly implement the /MT[d], /MD[d] switches, and MFCCE fixes some #pragma issues
	#ifdef _DLL // /MD
		#if defined(_DEBUG)
			#pragma comment(lib, "msvcrtd.lib")
		#else
			#pragma comment(lib, "msvcrt.lib")
		#endif
	#else // /MT
		#if defined(_DEBUG)
			#pragma comment(lib, "libcmtd.lib")
		#else
			#pragma comment(lib, "libcmt.lib")
		#endif
	#endif

	#if (_WIN32_WCE < 0x500) && ( defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP) )
		#pragma comment(lib, "ccrtrtti.lib")
		#ifdef _X86_	
			#if defined(_DEBUG)
				#pragma comment(lib, "libcmtx86d.lib")
			#else
				#pragma comment(lib, "libcmtx86.lib")
			#endif
		#endif
	#endif
#endif

#if defined(_DEBUG) && !defined(unix) && !defined(WINCE)

#include <malloc.h>
#include <stdlib.h>

#undef DEBUG_NEW
#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW
#define malloc(x)			_malloc_dbg(x, _NORMAL_BLOCK, __FILE__, __LINE__)
#define calloc(x, y)		_calloc_dbg(x, y, _NORMAL_BLOCK, __FILE__, __LINE__)
#define realloc(x, y)	_realloc_dbg(x, y, _NORMAL_BLOCK, __FILE__, __LINE__)

#define HAS_DEBUG_NEW

#endif // _DEBUG

// Make them work with any thread
#define AfxGetApp() GetApp()
#define AfxGetMainWnd() (GetApp()->m_pMainWnd)
#if (_MFC_VER >= 0x700)
#define AfxGetModuleThreadState() (AfxGetModuleState()->m_thread.GetDataNA())
#endif
#if _MFC_VER >= 0x800
 #define NCHITTEST_RESULT		LRESULT
#else
 #define NCHITTEST_RESULT		UINT
#endif

#endif // USE_DFPLUGIN

#endif // _STDAFX_H_

#else // NO_MFC


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
}


#undef DEBUG_NEW
#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW
#define malloc(x)			_malloc_dbg(x, _NORMAL_BLOCK, __FILE__, __LINE__)
#define calloc(x, y)		_calloc_dbg(x, y, _NORMAL_BLOCK, __FILE__, __LINE__)
#define realloc(x, y)	_realloc_dbg(x, y, _NORMAL_BLOCK, __FILE__, __LINE__)

#define HAS_DEBUG_NEW

#endif // _DEBUG

#include <winsock2.h>
#include <windows.h>

#endif // NO_MFC

#else // LINUX Native
#define NO_MFC

#endif
#endif

#endif
