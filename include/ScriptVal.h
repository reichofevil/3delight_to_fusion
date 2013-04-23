#ifndef _ScriptVal_h_
#define _ScriptVal_h_

#pragma pack(push, 8)

#ifndef USE_NOSCRIPTING

#include "MemBlock.h"

class RemoteApp;
class RemoteObject;
struct Packet;
class ScriptSymbol;
class FuID;

enum ScriptValType
{
	SV_Nil = 0,
	SV_Number,
	SV_String,
	SV_Boolean,
	SV_Table,
	SV_Function,
	SV_RemoteObject,
	SV_LocalObject,
	SV_Code,
	SV_MemBlock,
	SV_Args,
	SV_Ref,
	SV_StringCopy,

	SV_LastAllowed = 255 // Hopefully will catch any attempt to step beyond this with duplicate ID error.
};

// ScriptVal flags.  Must not use bits 0-7, as these are reserved for type enum, above.

#define SVF_SaveCompact         (1<< 8) // Save in a more compact form, ie on one line if possible
#define SVF_ReadOnly            (1<< 9) // Can't change this value

// Table Flags
#define SVTF_SaveNoImplicitKeys (1<<20) // Don't omit sequential numeric keys when writing table
#define SVTF_SaveOrdered        (1<<21) // Maintain order of string keys when saving/loading this table

// Number flags
#define SVNF_Float              (1<<20) // This number came from a float; limit precision when saving

// Args flags
#define SVAF_DeferReply         (1<<21) // Don't reply right away.  Also detaches args, such that they won't be deleted.

enum ScriptError
{
	SERR_None = 0,
	SERR_Other,
	SERR_InvalidArgs,
	SERR_UnsupportedOperation,
	SERR_NotLoggedIn,
	SERR_UnknownPacketType,
	SERR_InvalidObject,
	SERR_ScriptingNotInitialized,
	SERR_InvalidIndex,
	SERR_UnknownSymbol,
	SERR_InvalidRemoteObject,
};

class RemoteObject;
class MemBlock;
class MapTagList;
class TableEntry;
class ScriptObject;
class ScriptArgs;


// API_REV: Add some padding.

class FuSCRIPT_API ScriptVal
{
protected:
	union
	{
		ScriptValType m_Type;   // 8 bits
		uint32        m_Flags;  // 24 bits
	};
	union _m_Value
	{
		float64        m_Number;
		struct _m_String
		{
			char *     m_String;
			uint32     m_Hash; 
			char *     m_StringCopy;
		} m_String;
		bool           m_Boolean;
		RemoteObject  *m_RemoteObject;
		ScriptObject *m_LocalObject;
		uint64        m_Function;
		struct _m_MemBlock
		{ // Must match MemBlock, above.
			void *m_Ptr;
			uint32 m_Size;
		} m_MemBlock;
		struct _m_Code
		{
			void *m_Ptr;
			uint32 m_Size;
			uint32 m_Args;
		} m_Code;
		struct _m_Table
		{
			TableEntry *m_Table;
			TableEntry *m_Last;
			char       *m_Constructor;
			char       *m_ConstructorCopy;
		} m_Table;
		struct _m_Args
		{
			TableEntry *m_Table;
			RemoteApp  *m_App;
			uint32      m_Context;
			uint8       m_NumReturns;
			uint8       m_pad[3];
		} m_Args;
	} m_Value;

public:
	ScriptVal(ScriptValType type, const void *data = NULL);
	virtual ~ScriptVal();

public:	
	ScriptVal(const ScriptVal &val); 
	ScriptVal &operator =(const ScriptVal &val);
	ScriptVal &operator =(bool val);
	ScriptVal &operator =(int val);
	ScriptVal &operator =(int32 val);
	ScriptVal &operator =(uint32 val);
	ScriptVal &operator =(int64 val);
	ScriptVal &operator =(uint64 val);
	ScriptVal &operator =(float32 val);
	ScriptVal &operator =(float64 val);
	ScriptVal &operator =(const char *str);
	ScriptVal &operator =(const FuID &id);
	ScriptVal &operator =(const MemBlock &val);
	ScriptVal &operator =(ScriptObject *obj);

	// Nil
	ScriptVal();
	
	// Number
	ScriptVal(float32 val);
	ScriptVal(float64 val);
	ScriptVal(int val);
	ScriptVal(int32 val);
	ScriptVal(uint32 val);
	ScriptVal(int64 val);
	ScriptVal(uint64 val);
	
	// String
	ScriptVal(const char *val);
	ScriptVal(const char *val, int len);
	ScriptVal(char *val);
	ScriptVal(char *val, int len);
	ScriptVal(const FuID &val);
	
	// Boolean
	ScriptVal(bool val);
	
	// MemBlock
	ScriptVal(const MemBlock &val);

	//Code,
	ScriptVal(const void *code, uint32 len, uint32 args);

	// LocalObject
	ScriptVal(ScriptObject *val);

	// RemoteObject
	//ScriptVal(ScriptObject *val) { m_Type = SV_Nil; Set(SV_LocalObject, &val); }
	
	// Table
	// Function

	bool IsNumber() const;
	bool IsString() const;
	bool IsMemBlock() const;
	bool IsCode() const;
	bool IsBoolean() const;
	bool IsRemoteObject() const;
	bool IsLocalObject(bool mustbereal = false) const;
	bool IsFunction() const;
	bool IsTable() const;
	bool IsArgs() const;
	bool IsNil() const;

	float64 ToNumber(float64 def = 0.0) const;
	int32 ToInteger(int32 def = 0) const;
	const char *ToString(const char *def = NULL) const;
	bool ToBoolean(bool def = false) const;
	RemoteObject *ToRemoteObject(RemoteObject *def = NULL) const;
	MemBlock ToMemBlock() const;
	uint64 ToFunction(uint64 def = NULL) const;
	ScriptObject *ToLocalObject(ScriptObject *def = NULL);

	// ToCode()...
	void *ToCodePtr() const;
	uint32 ToCodeLen() const;
	uint32 ToCodeArgs() const;
	
	ScriptVal &Set(ScriptValType type, const void *data = NULL);
	ScriptVal &Set(ScriptValType type, const char **data, int len);
	bool Get(ScriptValType type, void *data) const;
	const void *GetData() const;

	ScriptValType Type() const { return (ScriptValType) (m_Type & 0xff); }
	
	void CleanUp();

//DFSTODO: 	void ToTags(RemoteApp *app, TagList &tags, bool first = true);
//DFSTODO: 	void FromTags(RemoteApp * app, TagList &tags, Tag *&tagptr);

	uint32 ToTextLen(int indent = 0);
	char * ToText(char *buf, int indent = 0);
	bool   FromText(const char *buf, const char *pre, char *errbuf, int errlen);
	bool   FromText(const char *buf, const char *pre, char *errbuf, int errlen, bool freebuf);

	bool   FromTextByName(const char *buf, const char *name, char *errbuf = NULL, int errlen = 0, bool freebuf = false);

	ScriptVal &AddKey(const ScriptVal &key);

	ScriptVal &FindKey(const ScriptVal &key);
	ScriptVal &FindKey(float64 val);
	ScriptVal &FindKey(float32 val)							{ return FindKey((float64)val); }
	ScriptVal &FindKey(int val)								{ return FindKey((float64)val); }
	ScriptVal &FindKey(int32 val)								{ return FindKey((float64)val); }
	ScriptVal &FindKey(uint32 val)							{ return FindKey((float64)val); }
	ScriptVal &FindKey(int64 val)								{ return FindKey((float64)val); }
	ScriptVal &FindKey(uint64 val)							{ return FindKey((float64)val); }
	ScriptVal &FindKey(const char *keystr);
	ScriptVal &FindKey(const char *keystr, int len);

	const ScriptVal &FindKeyConst(const ScriptVal &key) const;
	const ScriptVal &FindKeyConst(float64 val) const;
	const ScriptVal &FindKeyConst(float32 val) const	{ return FindKeyConst((float64)val); }
	const ScriptVal &FindKeyConst(int val) const			{ return FindKeyConst((float64)val); }
	const ScriptVal &FindKeyConst(int32 val) const		{ return FindKeyConst((float64)val); }
	const ScriptVal &FindKeyConst(uint32 val) const		{ return FindKeyConst((float64)val); }
	const ScriptVal &FindKeyConst(int64 val) const		{ return FindKeyConst((float64)val); }
	const ScriptVal &FindKeyConst(uint64 val) const		{ return FindKeyConst((float64)val); }
	const ScriptVal &FindKeyConst(const char *keystr) const;
	const ScriptVal &FindKeyConst(const char *keystr, int len) const;
	
	ScriptVal *FindKeyNoCase(const ScriptVal &key);
	ScriptVal *FindKeyNoCase(const char *keystr);
	ScriptVal *FindKeyNoCase(const char *keystr, int len);

	const ScriptVal *FindKeyNoCaseConst(const ScriptVal &key) const;
	const ScriptVal *FindKeyNoCaseConst(const char *keystr) const;
	const ScriptVal *FindKeyNoCaseConst(const char *keystr, int len) const;

	ScriptVal *FindKeyNest(const ScriptVal &key, bool create);
	ScriptVal *FindKeyNest(const FuID &key, bool create);
	ScriptVal *FindKeyNest(const char *keystr, bool create);

	const ScriptVal *FindKeyNestConst(const ScriptVal &key) const;
	const ScriptVal *FindKeyNestConst(const FuID &key) const;
	const ScriptVal *FindKeyNestConst(const char *keystr) const;

	ScriptVal *FindKeyNoCaseNest(const ScriptVal &key, bool create);
	ScriptVal *FindKeyNoCaseNest(const FuID &key, bool create);
	ScriptVal *FindKeyNoCaseNest(const char *keystr, bool create);

	const ScriptVal *FindKeyNoCaseNestConst(const ScriptVal &key) const;
	const ScriptVal *FindKeyNoCaseNestConst(const FuID &key) const;
	const ScriptVal *FindKeyNoCaseNestConst(const char *keystr) const;

	ScriptVal &operator [](const ScriptVal &key);
	ScriptVal &operator [](float64 val)						{ return FindKey(val); }
	ScriptVal &operator [](float32 val)						{ return FindKey((float64)val); }
	ScriptVal &operator [](int val)							{ return FindKey((float64)val); }
	ScriptVal &operator [](int32 val)						{ return FindKey((float64)val); }
	ScriptVal &operator [](uint32 val)						{ return FindKey((float64)val); }
	ScriptVal &operator [](int64 val)						{ return FindKey((float64)val); }
	ScriptVal &operator [](uint64 val)						{ return FindKey((float64)val); }
	ScriptVal &operator [](const char *keystr)			{ return FindKey(keystr); }
	ScriptVal &operator [](const FuID &id)					{ return FindKey((const char *)id); }

	const ScriptVal &operator [](const ScriptVal &key) const;
	const ScriptVal &operator [](float64 val) const		{ return FindKeyConst(val); }
	const ScriptVal &operator [](float32 val) const		{ return FindKeyConst((float64)val); }
	const ScriptVal &operator [](int val) const			{ return FindKeyConst((float64)val); }
	const ScriptVal &operator [](int32 val) const		{ return FindKeyConst((float64)val); }
	const ScriptVal &operator [](uint32 val) const		{ return FindKeyConst((float64)val); }
	const ScriptVal &operator [](int64 val) const		{ return FindKeyConst((float64)val); }
	const ScriptVal &operator [](uint64 val) const		{ return FindKeyConst((float64)val); }
	const ScriptVal &operator [](const char *keystr) const { return FindKeyConst(keystr); }
	const ScriptVal &operator [](const FuID &id) const	{ return FindKeyConst((const char *)id); }

//	ScriptObject *ToLocalObject() const;
//	bool GetCode() const;
//	bool GetFuscriptvalnction() const;

	// Table access
	const char *GetConstructor() const;
	void SetConstructor(const char *str);
	void SetConstructor(const FuID &id);
	void SetTable(TableEntry *te);

	bool IsEmpty() const;

	int NumKeys() const;
	
	bool NextKey(TableEntry *&te) const;
	bool Exists(const ScriptVal &tk) const;	

	bool operator ==(const ScriptVal &sv) const;
	bool operator ==(float64 val) const;
	bool operator ==(const char *str) const;
	bool operator ==(const FuID &id) const;

	bool operator !=(const ScriptVal &sv) const;
	bool operator !=(float64 val) const;
	bool operator !=(const char *str) const;
	bool operator !=(const FuID &id) const;

	ScriptVal &operator +=(const ScriptVal &);
	ScriptVal &operator -=(const ScriptVal &);

	void Dump(int indent = 0, bool newline = true);

	ScriptVal GetKey(const char *key);
	ScriptVal GetKey(float64 key);
	void SetKey(const char *key, const ScriptVal &val);
	void SetKey(float64 key, const ScriptVal &val);

	// Args functions
	RemoteApp *GetApp();
	void SetApp(RemoteApp *app);

	void Return(const ScriptVal &val);
	void ReturnNil();

	void ReturnNumber(float64 val);
	void ReturnString(const char *val);
	void ReturnMemBlock(const MemBlock &val);
	void ReturnCode(const void *code, uint32 len, uint32 args = 0xffff);
	void ReturnBoolean(bool val);
	void ReturnFunction(ScriptSymbol *val);
	void ReturnObject(ScriptObject *val);
	void ReturnRemoteObject(RemoteObject *val);

	// Error return
	void SetError(ScriptError se, const char *err = NULL);

	// Print a string on the console of the script interpreter upon return.
	void Print(const char *str);

	uint32 GetFlags(uint32 flags) const { return m_Flags & (flags & 0xffffff00); }
	void SetFlags(uint32 flags) { m_Flags |= (flags & 0xffffff00); }
	void ClearFlags(uint32 flags) { m_Flags &= ~(flags & 0xffffff00); }

	ScriptVal &operator =(const ScriptValType type);
	bool operator ==(const ScriptValType type) const;

	// Packet Stuff
	static Packet *ToPacket(RemoteObject *obj, int cmd, ScriptVal *ctrl, ScriptVal *val = NULL);
	static Packet *ToPacket(RemoteApp *app, uint32 target, int cmd, ScriptVal *ctrl, ScriptVal *val = NULL);

	static Packet *ToReply(RemoteObject *obj, Packet *pkt, ScriptVal *ctrl, ScriptVal *val = NULL);
	static Packet *ToReply(RemoteApp *app, uint32 context, ScriptVal *ctrl, ScriptVal *val = NULL);

	static bool FromPacket(RemoteObject *obj, Packet *pkt, ScriptVal *ctrl, ScriptVal *val = NULL);
	static bool FromPacket(RemoteApp *app, Packet *pkt, ScriptVal *ctrl, ScriptVal *val = NULL);


	bool ToPkt(RemoteApp *app, uint8 *&ptr) const;
	bool FromPkt(RemoteApp *app, uint8 *&ptr);
	uint32 CalcPktSize() const;

	ScriptVal CallMemberA(const char *name, ScriptArgs &args);

#define A(x) const ScriptVal &a_##x
	ScriptVal CallMember(const char *name);
	ScriptVal CallMember(const char *name, A(1));
	ScriptVal CallMember(const char *name, A(1), A(2));
	ScriptVal CallMember(const char *name, A(1), A(2), A(3));
	ScriptVal CallMember(const char *name, A(1), A(2), A(3), A(4));
	ScriptVal CallMember(const char *name, A(1), A(2), A(3), A(4), A(5));
	ScriptVal CallMember(const char *name, A(1), A(2), A(3), A(4), A(5), A(6));
	ScriptVal CallMember(const char *name, A(1), A(2), A(3), A(4), A(5), A(6), A(7));
	ScriptVal CallMember(const char *name, A(1), A(2), A(3), A(4), A(5), A(6), A(7), A(8));
#undef A

	template <class T> void TableFromArray(T *array, int num)
	{
		Set(SV_Table); // We're now an empty table

		m_Value.m_Table.m_Table = m_Value.m_Table.m_Last = NULL;

		if (num > 0)
		{
			TableEntry *te, *prev = NULL;

			// Do it backwards... its easier that way.
			int i = num;

			te = new TableEntry();
			te->m_Next = prev;
			prev = te;

			te->m_Key = i;
			te->m_Value = array[i-1];

			i--;

			m_Value.m_Table.m_Last = prev;

			for (; i > 0; i--)
			{
				te = new TableEntry();
				te->m_Next = prev;
				prev = te;

				te->m_Key = i;
				te->m_Value = array[i-1];
			}

			m_Value.m_Table.m_Table = prev;
		}

		SetFlags(SVF_SaveCompact);
	}
	
	template <class T> void TableToArray(T *array, int num) const
	{
		TableEntry *te = NULL;
		
		if ((m_Type&0xff) == SV_Table || (m_Type&0xff) == SV_Args)
		{
			te = m_Value.m_Table.m_Table;

			// Find the first numeric key
			while (te && !te->m_Key.IsNumber())
				te = te->m_Next;
		}
			
		for (int i = 1; i <= num; i++)
		{
			while (te && te->m_Key.IsNumber() && te->m_Key.ToNumber() < i)
				te = te->m_Next;
			
			if (te && te->m_Key.IsNumber() && te->m_Key.ToNumber() == i)
				array[i-1] = te->m_Value.ToNumber();
			else
				array[i-1] = 0;
		}
	}

	template <class KT, class VT> void TableFromArrays(KT *keys, VT *vals, int num)
	{
		Set(SV_Table); // We're now an empty table

		m_Value.m_Table.m_Table = m_Value.m_Table.m_Last = NULL;

		if (num > 0)
		{
			TableEntry *te, *prev = NULL;

			// Do it backwards... its easier that way.
			int i = num;

			te = new TableEntry();
			te->m_Next = prev;
			prev = te;

			te->m_Key = keys[i-1];
			te->m_Value = vals[i-1];

			i--;

			m_Value.m_Table.m_Last = prev;

			for (; i > 0; i--)
			{
				te = new TableEntry();
				te->m_Next = prev;
				prev = te;

				te->m_Key = keys[i-1];
				te->m_Value = vals[i-1];
			}

			m_Value.m_Table.m_Table = prev;
		}

		SetFlags(SVF_SaveCompact);
	}

	template <class KT, class VT> void TableToArrays(KT *keys, VT *vals, int num) const
	{
		TableEntry *te = NULL;
		
		if ((m_Type&0xff) == SV_Table || (m_Type&0xff) == SV_Args)
		{
			te = m_Value.m_Table.m_Table;

			// Find the first numeric key
			while (te && !te->m_Key.IsNumber())
				te = te->m_Next;
		}
			
		for (int i = 1; i <= num; i++)
		{
			if (te && te->m_Key.IsNumber() && te->m_Value.IsNumber())
			{
				keys[i-1] = te->m_Key.ToNumber();
				vals[i-1] = te->m_Value.ToNumber();

				te = te->m_Next;
			}
		}
	}
};

#ifndef nil // argh, stupid mac stuff.  Should be in a namespace anyway...
FuSCRIPT_API extern const ScriptVal nil;
#endif

FuSCRIPT_API void CreateUUID(ScriptVal &uuid);
FuSCRIPT_API ScriptVal CreateUUID();

class FuSCRIPT_API ScriptArgs : public ScriptVal
{
public:
	ScriptArgs(RemoteApp *app, uint32 context = 0) : ScriptVal(SV_Args, NULL) { SetApp(app); m_Value.m_Args.m_Context = context; m_Value.m_Args.m_NumReturns = 0; }

	int NumArgs() { return NumKeys()-1; }
	ScriptArgs *DeferReply();
	bool SendReply();
};

FuSCRIPT_API uint32 GetHashValue(const char *str);
FuSCRIPT_API uint32 GetHashValue(const char *str, int strlen);	// not null-terminated

#ifndef _DEBUG
#undef new
#endif

// TableEntry
class FuSCRIPT_API TableEntry
{
public:
	TableEntry *m_Next;
	ScriptVal m_Key;
	ScriptVal m_Value;

public:
	TableEntry() {}
	TableEntry(const ScriptVal &key) : m_Key(key) {}
	TableEntry(float64 key) : m_Key(key) {}
	TableEntry(const char *key, int keylen) : m_Key(key, keylen) {}

#ifndef _DEBUG
public:
	static void HeapCompact(void);

	// public member functions for TableEntry class
	// declaration of new & delete operator overloads
	void*   operator    new(size_t nSize);
	void    operator    delete(void* pObj);
#endif
};


class FuSCRIPT_API PrivateTableEntryHeap
{
	HANDLE m_hHeap;

public:
	PrivateTableEntryHeap();
	~PrivateTableEntryHeap();
	void StartHeap();
	void StopHeap();
};


#else
// Placeholder, to keep save/load prototypes happy.
class ScriptVal {};
#endif

#pragma pack(pop)

#endif
