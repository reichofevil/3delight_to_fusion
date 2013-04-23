#ifndef _DFSTRING_H_
#define _DFSTRING_H_

#include "types.h"

#define DFChar WCHAR

int FuSYSTEM_API DFString_MultiByteLength(char *text);

// 

struct DFStringData
{
	long nRefs;				// reference count
	int nDataLength;		// length of data (including terminator)
	int nAllocLength;		// length of allocation
	// WCHAR data[nAllocLength]

	WCHAR* data()			// WCHAR* to managed data
		{ return (WCHAR*)(this+1); }
};

class FuSYSTEM_API FuString
{
public:
// Constructors

	// constructs empty FuString
	FuString();
	// copy constructor
	FuString(const FuString& stringSrc);
	// from a single character
	FuString(WCHAR ch, int nRepeat = 1);
	// from an ANSI string (converts to WCHAR)
	FuString(LPCSTR lpsz);
	// from a UNICODE string (converts to WCHAR)
	FuString(LPCWSTR lpsz);
	// subset of characters from an ANSI string (converts to WCHAR)
	FuString(LPCSTR lpch, int nLength);
	// subset of characters from a UNICODE string (converts to WCHAR)
	FuString(LPCWSTR lpch, int nLength);
	// from unsigned characters
	FuString(const unsigned char* psz);

// Attributes & Operations

	// get data length
	int GetLength() const;
	// TRUE if zero length
	BOOL IsEmpty() const;
	// clear contents to empty
	void Empty();

	// return single character at zero-based index
	WCHAR GetAt(int nIndex) const;
	// return single character at zero-based index
	WCHAR operator[](int nIndex) const;
	// set a single character at zero-based index
	void SetAt(int nIndex, WCHAR ch);
	// return pointer to const string
	operator LPCWSTR() const;

	// overloaded assignment

	// ref-counted copy from another FuString
	const FuString& operator=(const FuString& stringSrc);
	// set string content to single character
	const FuString& operator=(WCHAR ch);

	const FuString& operator=(char ch);

	// copy string content from ANSI string (converts to WCHAR)
	const FuString& operator=(LPCSTR lpsz);
	// copy string content from UNICODE string (converts to WCHAR)
	const FuString& operator=(LPCWSTR lpsz);
	// copy string content from unsigned chars
	const FuString& operator=(const unsigned char* psz);

	// string concatenation

	// concatenate from another FuString
	const FuString& operator+=(const FuString& string);

	// concatenate a single character
	const FuString& operator+=(WCHAR ch);

	// concatenate an ANSI character after converting it to WCHAR
	const FuString& operator+=(char ch);

	// concatenate a UNICODE character after converting it to WCHAR
	const FuString& operator+=(LPCWSTR lpsz);

	friend FuString AFXAPI operator+(const FuString& string1,
			const FuString& string2);
	friend FuString AFXAPI operator+(const FuString& string, WCHAR ch);
	friend FuString AFXAPI operator+(WCHAR ch, const FuString& string);
	friend FuString AFXAPI operator+(const FuString& string, char ch);
	friend FuString AFXAPI operator+(char ch, const FuString& string);
	friend FuString AFXAPI operator+(const FuString& string, LPCWSTR lpsz);
	friend FuString AFXAPI operator+(LPCWSTR lpsz, const FuString& string);

	// string comparison

	// straight character comparison
	int Compare(LPCWSTR lpsz) const;
	// compare ignoring case
	int CompareNoCase(LPCWSTR lpsz) const;
	// NLS aware comparison, case sensitive
	int Collate(LPCWSTR lpsz) const;
	// NLS aware comparison, case insensitive
	int CollateNoCase(LPCWSTR lpsz) const;

	// simple sub-string extraction

	// return nCount characters starting at zero-based nFirst
	FuString Mid(int nFirst, int nCount) const;
	// return all characters starting at zero-based nFirst
	FuString Mid(int nFirst) const;
	// return first nCount characters in string
	FuString Left(int nCount) const;
	// return nCount characters from end of string
	FuString Right(int nCount) const;

	//	characters from beginning that are also in passed string
	FuString SpanIncluding(LPCWSTR lpszCharSet) const;
	// characters from beginning that are not also in passed string
	FuString SpanExcluding(LPCWSTR lpszCharSet) const;

	// upper/lower/reverse conversion

	// NLS aware conversion to uppercase
	void MakeUpper();
	// NLS aware conversion to lowercase
	void MakeLower();
	// reverse string right-to-left
	void MakeReverse();

	// trimming whitespace (either side)

	// remove whitespace starting from right edge
	void TrimRight();
	// remove whitespace starting from left side
	void TrimLeft();

	// trimming anything (either side)

	// remove continuous occurrences of chTarget starting from right
	void TrimRight(WCHAR chTarget);
	// remove continuous occcurrences of characters in passed string,
	// starting from right
	void TrimRight(LPCWSTR lpszTargets);
	// remove continuous occurrences of chTarget starting from left
	void TrimLeft(WCHAR chTarget);
	// remove continuous occcurrences of characters in
	// passed string, starting from left
	void TrimLeft(LPCWSTR lpszTargets);

	// advanced manipulation

	// replace occurrences of chOld with chNew
	int Replace(WCHAR chOld, WCHAR chNew);
	// replace occurrences of substring lpszOld with lpszNew;
	// empty lpszNew removes instances of lpszOld
	int Replace(LPCWSTR lpszOld, LPCWSTR lpszNew);
	// remove occurrences of chRemove
	int Remove(WCHAR chRemove);
	// insert character at zero-based index; concatenates
	// if index is past end of string
	int Insert(int nIndex, WCHAR ch);
	// insert substring at zero-based index; concatenates
	// if index is past end of string
	int Insert(int nIndex, LPCWSTR pstr);
	// delete nCount characters starting at zero-based index
	int Delete(int nIndex, int nCount = 1);

	// searching

	// find character starting at left, -1 if not found
	int Find(WCHAR ch) const;
	// find character starting at right
	int ReverseFind(WCHAR ch) const;
	// find character starting at zero-based index and going right
	int Find(WCHAR ch, int nStart) const;
	// find first instance of any character in passed string
	int FindOneOf(LPCWSTR lpszCharSet) const;
	// find first instance of substring
	int Find(LPCWSTR lpszSub) const;
	// find first instance of substring starting at zero-based index
	int Find(LPCWSTR lpszSub, int nStart) const;

	// simple formatting

	// printf-like formatting using passed string
	void AFX_CDECL Format(LPCWSTR lpszFormat, ...);
	// printf-like formatting using referenced string resource
	void AFX_CDECL Format(UINT nFormatID, ...);
	// printf-like formatting using variable arguments parameter
	void FormatV(LPCWSTR lpszFormat, va_list argList);

	// formatting for localization (uses FormatMessage API)

	// format using FormatMessage API on passed string
	void AFX_CDECL FormatMessage(LPCWSTR lpszFormat, ...);
	// format using FormatMessage API on referenced string resource
	void AFX_CDECL FormatMessage(UINT nFormatID, ...);

	// input and output
#if defined(_DEBUG) && !defined(NO_MFC)
	friend CDumpContext& AFXAPI operator<<(CDumpContext& dc,
				const FuString& string);
#endif
	friend CArchive& AFXAPI operator<<(CArchive& ar, const FuString& string);
	friend CArchive& AFXAPI operator>>(CArchive& ar, FuString& string);

	// load from string resource
	BOOL LoadString(UINT nID);

//#ifndef _UNICODE
//	// ANSI <-> OEM support (convert string in place)
//
//	// convert string from ANSI to OEM in-place
//	void AnsiToOem();
//	// convert string from OEM to ANSI in-place
//	void OemToAnsi();
//#endif

#if !defined(_AFX_NO_BSTR_SUPPORT) && !defined(NO_MFC)
	// OLE BSTR support (use for OLE automation)

	// return a BSTR initialized with this FuString's data
	BSTR AllocSysString() const;
	// reallocates the passed BSTR, copies content of this FuString to it
	BSTR SetSysString(BSTR* pbstr) const;
#endif

	// Access to string implementation buffer as "C" character array

	// get pointer to modifiable buffer at least as long as nMinBufLength
	LPWSTR GetBuffer(int nMinBufLength);
	// release buffer, setting length to nNewLength (or to first nul if -1)
	void ReleaseBuffer(int nNewLength = -1);
	// get pointer to modifiable buffer exactly as long as nNewLength
	LPWSTR GetBufferSetLength(int nNewLength);
	// release memory allocated to but unused by string
	void FreeExtra();

	// Use LockBuffer/UnlockBuffer to turn refcounting off

	// turn refcounting back on
	LPWSTR LockBuffer();
	// turn refcounting off
	void UnlockBuffer();

// Implementation
public:
	~FuString();
	int GetAllocLength() const;

protected:
	LPWSTR m_pchData;   // pointer to ref counted string data

	// implementation helpers
	DFStringData* GetData() const;
	void Init();
	void AllocCopy(FuString& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;
	void AllocBuffer(int nLen);
	void AssignCopy(int nSrcLen, LPCWSTR lpszSrcData);
	void ConcatCopy(int nSrc1Len, LPCWSTR lpszSrc1Data, int nSrc2Len, LPCWSTR lpszSrc2Data);
	void ConcatInPlace(int nSrcLen, LPCWSTR lpszSrcData);
	void CopyBeforeWrite();
	void AllocBeforeWrite(int nLen);
	void Release();
	static void PASCAL Release(DFStringData* pData);
	static int PASCAL SafeStrlen(LPCWSTR lpsz);
	static void FASTCALL FreeData(DFStringData* pData);

public: // Yes some sometimes we do something ourselves!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Does UTF-8 conversion
	void FromMultiByte(const char * string);
	void FromMultiByte(CString& string);
	CString ToMultiByte();

	// Does ANSI conversion
	void FromMultiByteANSI(const char * string);
	void FromMultiByteANSI(CString& string);
	CString ToMultiByteANSI();

		// Metrics
	int GetLineCount();
	FuString GetLine(int index);

	int GetTabCount();
	FuString GetTab(int index);
	
	int GetWordCount();
	FuString GetWord(int index);

	int GetCharCount();
	DFChar GetChar(int index);
	
	int SpacesBeforeWord(int index);
	int InterWordCount();
	int InterCharCount();

	int LTWCToPosition(int Line, int Tab, int Word, int Char); // Wrong rather not use it

};

// Compare helpers
bool AFXAPI operator==(const FuString& s1, const FuString& s2);
bool AFXAPI operator==(const FuString& s1, LPCWSTR s2);
bool AFXAPI operator==(LPCWSTR s1, const FuString& s2);
bool AFXAPI operator!=(const FuString& s1, const FuString& s2);
bool AFXAPI operator!=(const FuString& s1, LPCWSTR s2);
bool AFXAPI operator!=(LPCWSTR s1, const FuString& s2);
bool AFXAPI operator<(const FuString& s1, const FuString& s2);
bool AFXAPI operator<(const FuString& s1, LPCWSTR s2);
bool AFXAPI operator<(LPCWSTR s1, const FuString& s2);
bool AFXAPI operator>(const FuString& s1, const FuString& s2);
bool AFXAPI operator>(const FuString& s1, LPCWSTR s2);
bool AFXAPI operator>(LPCWSTR s1, const FuString& s2);
bool AFXAPI operator<=(const FuString& s1, const FuString& s2);
bool AFXAPI operator<=(const FuString& s1, LPCWSTR s2);
bool AFXAPI operator<=(LPCWSTR s1, const FuString& s2);
bool AFXAPI operator>=(const FuString& s1, const FuString& s2);
bool AFXAPI operator>=(const FuString& s1, LPCWSTR s2);
bool AFXAPI operator>=(LPCWSTR s1, const FuString& s2);

// conversion helpers
int AFX_CDECL _dfwcstombsz(char* mbstr, const WCHAR* wcstr, size_t count);
int AFX_CDECL _dfmbstowcsz(WCHAR* wcstr, const char* mbstr, size_t count);

// Globals
#ifdef _AFXDLL
const FuString& AFXAPI DFGetEmptyString();
#define dfEmptyString DFGetEmptyString()
#else
extern LPCWSTR _afxPchNil;
#define dfEmptyString ((FuString&)*(FuString*)&_afxPchNil)
#endif

// FuString
inline DFStringData* FuString::GetData() const
	{ ASSERT(m_pchData != NULL); return ((DFStringData*)m_pchData)-1; }
inline void FuString::Init()
	{ m_pchData = dfEmptyString.m_pchData; }
#ifndef _AFXDLL
inline FuString::FuString()
	{ m_pchData = dfEmptyString.m_pchData; }
#endif
inline FuString::FuString(const unsigned char* lpsz)
	{ Init(); *this = (LPCSTR)lpsz; }
inline const FuString& FuString::operator=(const unsigned char* lpsz)
	{ *this = (LPCSTR)lpsz; return *this; }
inline const FuString& FuString::operator+=(char ch)
	{ *this += (WCHAR)ch; return *this; }
inline const FuString& FuString::operator=(char ch)
	{ *this = (WCHAR)ch; return *this; }
inline FuString AFXAPI operator+(const FuString& string, char ch)
	{ return string + (WCHAR)ch; }
inline FuString AFXAPI operator+(char ch, const FuString& string)
	{ return (WCHAR)ch + string; }

inline int FuString::GetLength() const
	{ return GetData()->nDataLength; }
inline int FuString::GetAllocLength() const
	{ return GetData()->nAllocLength; }
inline BOOL FuString::IsEmpty() const
	{ return GetData()->nDataLength == 0; }
inline FuString::operator LPCWSTR() const
	{ return m_pchData; }
inline int PASCAL FuString::SafeStrlen(LPCWSTR lpsz)
	{ return (lpsz == NULL) ? 0 : lstrlenW(lpsz); }

// FuString support (windows specific)
inline int FuString::Compare(LPCWSTR lpsz) const
	{ ASSERT(AfxIsValidString(lpsz)); return wcscmp(m_pchData, lpsz); }    // MBCS/Unicode aware
inline int FuString::CompareNoCase(LPCWSTR lpsz) const
	{ ASSERT(AfxIsValidString(lpsz)); return wcsicmp(m_pchData, lpsz); }   // MBCS/Unicode aware
// FuString::Collate is often slower than Compare but is MBSC/Unicode
//  aware as well as locale-sensitive with respect to sort order.
inline int FuString::Collate(LPCWSTR lpsz) const
	{ ASSERT(AfxIsValidString(lpsz)); return wcscoll(m_pchData, lpsz); }   // locale sensitive
inline int FuString::CollateNoCase(LPCWSTR lpsz) const
	{ ASSERT(AfxIsValidString(lpsz)); return wcsicoll(m_pchData, lpsz); }   // locale sensitive

inline WCHAR FuString::GetAt(int nIndex) const
{
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < GetData()->nDataLength);
	return m_pchData[nIndex];
}
inline WCHAR FuString::operator[](int nIndex) const
{
	// same as GetAt
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < GetData()->nDataLength);
	return m_pchData[nIndex];
}
inline bool AFXAPI operator==(const FuString& s1, const FuString& s2)
	{ return s1.Compare(s2) == 0; }
inline bool AFXAPI operator==(const FuString& s1, LPCWSTR s2)
	{ return s1.Compare(s2) == 0; }
inline bool AFXAPI operator==(LPCWSTR s1, const FuString& s2)
	{ return s2.Compare(s1) == 0; }
inline bool AFXAPI operator!=(const FuString& s1, const FuString& s2)
	{ return s1.Compare(s2) != 0; }
inline bool AFXAPI operator!=(const FuString& s1, LPCWSTR s2)
	{ return s1.Compare(s2) != 0; }
inline bool AFXAPI operator!=(LPCWSTR s1, const FuString& s2)
	{ return s2.Compare(s1) != 0; }
inline bool AFXAPI operator<(const FuString& s1, const FuString& s2)
	{ return s1.Compare(s2) < 0; }
inline bool AFXAPI operator<(const FuString& s1, LPCWSTR s2)
	{ return s1.Compare(s2) < 0; }
inline bool AFXAPI operator<(LPCWSTR s1, const FuString& s2)
	{ return s2.Compare(s1) > 0; }
inline bool AFXAPI operator>(const FuString& s1, const FuString& s2)
	{ return s1.Compare(s2) > 0; }
inline bool AFXAPI operator>(const FuString& s1, LPCWSTR s2)
	{ return s1.Compare(s2) > 0; }
inline bool AFXAPI operator>(LPCWSTR s1, const FuString& s2)
	{ return s2.Compare(s1) < 0; }
inline bool AFXAPI operator<=(const FuString& s1, const FuString& s2)
	{ return s1.Compare(s2) <= 0; }
inline bool AFXAPI operator<=(const FuString& s1, LPCWSTR s2)
	{ return s1.Compare(s2) <= 0; }
inline bool AFXAPI operator<=(LPCWSTR s1, const FuString& s2)
	{ return s2.Compare(s1) >= 0; }
inline bool AFXAPI operator>=(const FuString& s1, const FuString& s2)
	{ return s1.Compare(s2) >= 0; }
inline bool AFXAPI operator>=(const FuString& s1, LPCWSTR s2)
	{ return s1.Compare(s2) >= 0; }
inline bool AFXAPI operator>=(LPCWSTR s1, const FuString& s2)
	{ return s2.Compare(s1) <= 0; }

#ifndef NO_MFC
#include "afxplex_.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CFixedAlloc

class CFixedAlloc
{
// Constructors
public:
	CFixedAlloc(UINT nAllocSize, UINT nBlockSize = 64);

// Attributes
	UINT GetAllocSize() { return m_nAllocSize; }

// Operations
public:
	void* Alloc();  // return a chunk of memory of nAllocSize
	void Free(void* p); // free chunk of memory returned from Alloc
	void FreeAll(); // free everything allocated from this allocator

// Implementation
public:
	~CFixedAlloc();

protected:
	struct CNode
	{
		CNode* pNext;   // only valid when in free list
	};

	UINT m_nAllocSize;  // size of each block from Alloc
	UINT m_nBlockSize;  // number of blocks to get at a time
	CPlex* m_pBlocks;   // linked list of blocks (is nBlocks*nAllocSize)
	CNode* m_pNodeFree; // first free node (NULL if no free nodes)
	CRITICAL_SECTION m_protect;
};

#ifndef _DEBUG

// DECLARE_FIXED_ALLOC -- used in class definition
#define DECLARE_FIXED_ALLOC(class_name) \
public: \
	void* operator new(size_t size) \
	{ \
		ASSERT(size == s_alloc.GetAllocSize()); \
		UNUSED(size); \
		return s_alloc.Alloc(); \
	} \
	void* operator new(size_t, void* p) \
		{ return p; } \
	void operator delete(void* p) { s_alloc.Free(p); } \
	void* operator new(size_t size, LPCSTR, int) \
	{ \
		ASSERT(size == s_alloc.GetAllocSize()); \
		UNUSED(size); \
		return s_alloc.Alloc(); \
	} \
protected: \
	static CFixedAlloc s_alloc \

// IMPLEMENT_FIXED_ALLOC -- used in class implementation file
#define IMPLEMENT_FIXED_ALLOC(class_name, block_size) \
CFixedAlloc class_name::s_alloc(sizeof(class_name), block_size) \

#else //!_DEBUG

#define DECLARE_FIXED_ALLOC(class_name)     // nothing in debug
#define IMPLEMENT_FIXED_ALLOC(class_name, block_size)   // nothing in debug

#endif //!_DEBUG

#endif // _DFSTRING_H_
