#ifndef _FU_TYPES_H_
#define _FU_TYPES_H_

#pragma pack(push, 8)

// Disable some irritating warnings.
#pragma warning(disable : 4018 4065 4127 4201 4237 4244 4245 4305 4514 4706 4996 4867)
#pragma warning(error   : 4356 4003)

//TODO: Remove me
#pragma warning(disable : 4267)

#define DLLExport __declspec(dllexport)
#define DLLImport __declspec(dllimport)

#if defined(unix)
	#define FuSYSTEM_API
	#define DLLExtern extern
#else
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
#endif

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) (P)
#endif

// Some generic typedefs for portability.
typedef char               int8;
typedef unsigned char     uint8;
typedef short              int16;
typedef unsigned short    uint16;
typedef long               int32;
typedef unsigned long     uint32;
typedef __int64            int64;
typedef unsigned __int64  uint64;

typedef float             float32;
typedef double            float64;

#ifndef _W64
#define _W64
#endif

#ifdef _WIN64
typedef __int64                 int_ptr;
typedef unsigned __int64       uint_ptr;
#else
typedef _W64 long               int_ptr;
typedef _W64 unsigned long     uint_ptr;
#endif

#ifndef ULONG_PTR
#ifdef _WIN64
typedef __int64					INT_PTR;
typedef unsigned __int64		UINT_PTR;

typedef __int64					LONG_PTR;
typedef unsigned __int64		ULONG_PTR;
#else
typedef _W64 int					INT_PTR;
typedef _W64 unsigned int		UINT_PTR;

typedef _W64 long					LONG_PTR;
typedef _W64 unsigned long		ULONG_PTR;
#endif
#endif

#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif

// For DF5, bool is now a real C++ bool.
// #define bool              uint32
//#define TRUE              1
//#define FALSE             0

typedef float64           TimeStamp;
#define TIME_UNDEFINED    -1000000000

#define MIN_TIME          -1000000
#define MAX_TIME          1000000

#define TIME_FUZZ         0.00005
#define FL_ERR            0.000001
#define MIN_SIZE          0.00001

// Tag stuff
typedef uint_ptr            Tag;

#define _TAG_DONE        (TAG_CONTROL | 0)
#define TAG_IGNORE       (TAG_CONTROL | 1)
#define TAG_SKIP         (TAG_CONTROL | 2)
#define TAG_MORE         (TAG_CONTROL | 3)
#define TAG_ERROR        (TAG_CONTROL | 4)
#define TAG_IGNOREPTR    (TAG_CONTROL | 5)

#ifdef RELEASE_VERSION
	#define TAG_DONE	     _TAG_DONE
#else
	#define TAG_DONE	     _TAG_DONE, TAG_ERROR, TAG_ERROR, TAG_ERROR,\
							     TAG_ERROR, TAG_ERROR, TAG_ERROR, TAG_ERROR,\
							     TAG_ERROR, TAG_ERROR, TAG_ERROR, TAG_ERROR,\
							     TAG_ERROR, TAG_ERROR, TAG_ERROR, TAG_ERROR
#endif

// This defines the maximum number of tags a given class can use.
// 64k tags per class allows for a class-hierarchy 4k levels deep.
#define TAGOFFSET	        0x00010000

#define TAG_SIZE_SHIFT    28

// Flags to be used in combination with the following tag types
#define TAG_GLOBAL        0x02000000
#define TAG_MULTI         0x08000000

// Tag types
#define TAG_TYPE_MASK    (15ul << TAG_SIZE_SHIFT)

#define TAG_CONTROL      (0ul << TAG_SIZE_SHIFT)
#define TAG_INT          (1ul << TAG_SIZE_SHIFT)
#define TAG_DOUBLE       (2ul << TAG_SIZE_SHIFT)
#define TAG_STRING       (3ul << TAG_SIZE_SHIFT)
#define TAG_OBJECT       (4ul << TAG_SIZE_SHIFT)
#define TAG_TAGLIST      (5ul << TAG_SIZE_SHIFT)
#define TAG_PTR          (6ul << TAG_SIZE_SHIFT)
#define TAG_MEMBLOCK     (7ul << TAG_SIZE_SHIFT)
#define TAG_ID           (8ul << TAG_SIZE_SHIFT)				// FuID

// All application wide, global tags should be the most recent base + TAGOFFSET
#define EVTAG_BASE        TAG_GLOBAL
#define EVCTRL_BASE       EVTAG_BASE + TAGOFFSET
#define EVCTRL_USER       EVCTRL_BASE + TAGOFFSET

// Need to put ENUM_TAGS after every enum declaration, if it's declaring tags, or bad sign-extending may happen
// e.g. enum MyTags ENUM_TAGS
//      { etc }
#if _MSC_VER >= 1400 && !defined(__INTEL_COMPILER) && defined(_WIN64)
 #define ENUM_TAGS			: Tag
#else
 #define ENUM_TAGS
#endif

#ifndef NULL
#define NULL  0
#endif

// C++ doesn't have these as standard macros like C does, so we define them here

//template <class T> T min (T i, T j) { return ((i < j) ? i : j); }
//template <class T> T max (T i, T j) { return ((i > j) ? i : j); }
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

// Multiplication of two 16 bits numbers with 16 bit result
// ie. 0-65535 * 0-65535 with a 0-65535 result
// Does it with perfect accuracy and without a division anywhere :-)
// Note: the variable passed into 't' should be a uint32 that
// will be used for temporary storage by the macro
#define INT_MULT(a,b,t)				((t) = (a) * (b) + 0x8000, ( ((t >> 16) + t) >> 16))

// Multiplication of two 8 bit numbers with 8 bit result. 
// ie. 0-255 * 0-255 with a 0-255 result
// Does it with perfect accuracy and without a division anywhere :-)
// Note: the variable passed into 't' should be a uint32 that
// will be used for temporary storage by the macro
#define INT_MULT8(a,b,t)			((t) = (a) * (b) + 0x80, ( ((t >> 8) + t) >> 8))

// Multiplication of one 16 bit number and one 8 bit number with a
// 16 bit result. ie. 0-65535 * 0-255 with a 0-65535 result
// Does it with perfect accuracy and without a division anywhere :-)
// Note: the variable passed into 't' should be a uint32 that
// will be used for temporary storage by the macro
#define INT_MULT16_8(a,b,t)		((t) = (a) * (b) + 0x80, (((((t >> 8) + t) >> 8) + t) >> 8))

// This will divide a 16 bit number by 257 - ie. it will convert
// it to a 8 bit value. It does it with perfect round-to-nearest accuracy,
// and without a single divide anywhere :-)
#define INT_DIV257(a)				((((-int32(a)+0x80)>>8)+((a)+0x80))>>8)

typedef void *HANDLE;

const float64 PI    = 3.1415926535897932384626433832795;
//#define PI				3.1415926535897932384626433832795

#ifndef M_PI
#define M_PI			PI
#endif
#define TWOPI			(PI * 2.0)

const float64 SQRT2 = 1.4142135623730950488016887242097;

#ifndef M_SQRT2
#define M_SQRT2		1.4142135623730950488016887242097
#endif 

#define DegToRad(a)	((a)*PI/180.0)
#define RadToDeg(a)	((a)*180.0/PI)

#define MAKEID(x)		(uint32)((((x)&0x000000ff)<<24)|(((x)&0x0000ff00)<<8)|(((x)&0x00ff0000)>>8)|(((x)&0xff000000)>>24))
#define zMAKEID(x)		Error_MAKEID!!!

// These datatypes are used in various places in the 3D system.  
//  * For GLTexture::Upload() - if you specify a type it will use that type if its available.  If its not available, it will upload to 8bit.
enum F3D_DataType
{
	F3D_INVALID_DATA_TYPE = -1,

	F3D_UINT8 = 0,
	F3D_UINT16,					// try not to use for texture/framebuffer types - on Nvidia you'll be given 8bit most of the time if you ask for 16bit, ATI support isn't too shabby
	F3D_UINT32,					// nyi - insufficient driver support
	F3D_FLOAT16,
	F3D_FLOAT32,	
	F3D_FLOAT64,				// nyi - insufficient driver support
	F3D_NUM_DATATYPES,

	F3D_MATCH,					// match the texture datatype to the image datatype
};




#include "dfmath.h"

#include <float.h>
#include <stdlib.h>
#include <string.h>

#include "Config.h"

/**********************************************************
 ** Everything from here onwards is only
 ** for compatibility - DO NOT USE
 **********************************************************/

// Note:  In the Matrix4f notation the indices are reversed from the order that one would expect.  eg.  If Rx, Ry, and Rz are the 
//        rotation matrices and v is 4x1 column vector, then RO_XYZ would be equivalent to the matrix R = Rz * Ry * Rx.  It would 
//        be applied to a 4x1 column vector to give v' = Rz * Ry * Rx * v.  Because Matrix4 is a transposed version of Matrix4f and 
//        operates on row vectors on the left, the transform is v' = v * Rx * Ry * Rz and the indices are correctly ordered.
enum RotationOrders
{
	RO_XYZ = 0,
	RO_XZY,
	RO_YXZ,
	RO_YZX,
	RO_ZXY,
	RO_ZYX,
	RO_NUM_ROT_ORDERS
};


#if defined(__cplusplus) && (_MSC_VER >= 1310) && !defined(WINCE) && (_MSC_VER < 1600)
#ifndef unix
inline int64 abs(int64 val) { return _abs64(val); }
#else
inline int8 abs(int8 val) { return val >= 0 ? val : -val; }
//inline int32 abs(int32 val) { return val >= 0 ? val : -val; }
inline int64 abs(int64 val) { return val >= 0 ? val : -val; }
//inline long abs(long val) { return val >= 0 ? val : -val; }
#endif
inline float64 fmod(float32 a, float64 b) { return fmod(float64(a), float64(b)); }
#endif

#if defined(__cplusplus)
inline int isalpha(char c) { return isalpha(int(unsigned char(c))); }
inline int isupper(char c) { return isupper(int(unsigned char(c))); }
inline int islower(char c) { return islower(int(unsigned char(c))); }
inline int isdigit(char c) { return isdigit(int(unsigned char(c))); }
inline int isxdigit(char c) { return isxdigit(int(unsigned char(c))); }
inline int isspace(char c) { return isspace(int(unsigned char(c))); }
inline int ispunct(char c) { return ispunct(int(unsigned char(c))); }
inline int isalnum(char c) { return isalnum(int(unsigned char(c))); }
inline int isprint(char c) { return isprint(int(unsigned char(c))); }
inline int isgraph(char c) { return isgraph(int(unsigned char(c))); }
inline int iscntrl(char c) { return iscntrl(int(unsigned char(c))); }
inline int toupper(char c) { return toupper(int(unsigned char(c))); }
inline int tolower(char c) { return tolower(int(unsigned char(c))); }
#endif

#pragma pack(pop)

#endif
