#ifndef _DFID_h_
#define _DFID_h_

#pragma pack(push, 8)

#include "Types.h"

#if defined(USE_NOSCRIPTING) || defined(_XBOX)
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

class FuSCRIPT_API FuID
{
protected:
	// This may be a 'normal' string, or a magic ref-counted string. Do not use directly.
	char *m_Name;

	void Cleanup();

public:
	FuID();
	~FuID();

	FuID(const char *str, bool copy = true);
//	FuID(char *str); // Always copies
	FuID(const char *str1, const char *str2); // concat
	
	FuID(const FuID &id);
	const FuID &operator = (const FuID &id);

	void Set(const char *str, bool copy);
	void Set(const FuID &id);
	void Set(const char *str1, const char *str2); // concat
	
	operator const char *() const;
	//const char *operator = (const char *str);

	FuID operator +(const char *str) const;
	FuID operator +(const FuID &id) const;
	const FuID &operator +=(const char *str);
	FuSCRIPT_API friend FuID operator+(const char *str, const FuID &id);

	bool operator == (const char *str) const;
	bool operator != (const char *str) const;

	bool IsInGroup(void) const;
	bool IsInGroup(const char *str) const;
	bool IsSameGroup(const char *str) const;
	
	bool IsValidSymbol() const;

	bool IsValid() const;

	bool IsEmpty() const;
	int GetLength() const;
	int Find(char ch, int start=0) const;
	FuID Mid(int start, int count=-1) const;
	FuID Left(int count) const;

	uint32 GetHashValue() const;
};


extern FuSCRIPT_API const FuID NOID;


#define FuDeclareClassX(thisclass, baseclass, name)\
	public:\
		const static FuID name##ClassID;\
		static Registry name##Reg;\
	private:\

#define FuDeclareClass(thisclass, baseclass) \
	public:\
		const static FuID ClassID;\
		static Registry Reg;\
	private:\

#pragma pack(pop)

#endif
