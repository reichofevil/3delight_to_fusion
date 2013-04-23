#ifndef _LOOKUPTABLE_H_
#define _LOOKUPTABLE_H_

#define USE_NEW_LOOKUPTABLE

#include "Parameter.h"
#include "Input.h"

class Output;

#define LOOKUPTABLE_ID zMAKEID('LUT!')
//#define CLSID_DataType_LookUpTable LOOKUPTABLE_ID

enum LUT_SpecialAttrs
{
	LUT_Imported = TAG_INT + 1150,	// NULL
	//LUT_KeyEntryIndexes = TAG_PTR,
};


#define LOOKUPTABLE_BASE (PARAMETER_BASE + TAGOFFSET)

enum LOOKUPTABLE_ATTRS
{
	LUT_Entries = LOOKUPTABLE_BASE | TAG_INT,
	LUT_OneToOne,
	LUT_Offset,
	LUT_Type,
	LUT_Flags,

	LUT_Value = LOOKUPTABLE_BASE | TAG_DOUBLE,
	LUT_Minimum,
	LUT_Maximum,
	LUT_MinimumValue,
	LUT_MaximumValue,
	LUT_StartSlope,
	LUT_EndSlope,
	LUT_StartIn,
	LUT_EndIn,
	LUT_Time,

	LUT_Result = LOOKUPTABLE_BASE | TAG_PTR,
	LUT_Context,
};

// LUT_Flags and EVLUT_Flags defines
#define LUTF_Quick (1<<0)

enum LUTTypes
{
	LUTTYPE_NONE = 0,
	LUTTYPE_UINT8,
	LUTTYPE_UINT16,
	LUTTYPE_UINT32,
	LUTTYPE_UINT64,
	LUTTYPE_INT8,
	LUTTYPE_INT16,
	LUTTYPE_INT32,
	LUTTYPE_INT64,
	LUTTYPE_FLOAT32,
	LUTTYPE_FLOAT64,
};

class FuSYSTEM_API LUT : public Object
{
	FuDeclareClass(LUT, Object); // declare ClassID static member, etc.

public:
	LUT(int32 numentries, LUTTypes type);
	virtual ~LUT();

	LUT(const LUT &lut);

	LUT(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual bool Save(ScriptVal &table, const TagList &tags);

	float64 GetValue(float64 inval);
	
	static uint32 EntrySize(LUTTypes type);

public:
	int32 m_NumEntries;
	LUTTypes m_Type;

	void *m_Table;

	int32 m_Offset;
	int32 _pad1;

	float64 m_Minimum, m_Maximum;
	float64 m_MinimumValue, m_MaximumValue;

	float64 m_StartSlope, m_EndSlope;
	float64 m_StartIn, m_EndIn;
};

class Image;

class FuSYSTEM_API LookUpTable : public Parameter
{
	FuDeclareClass(LookUpTable, Parameter); // declare ClassID static member, etc.

protected:
	FuPointer<LUT> m_Cache;

	Object *m_Source;
	Output *m_Output;
	void *m_Context;
	TimeStamp m_Time;

	bool m_OneToOne;
	float64 m_StartSlope;
	float64 m_EndSlope;
	float64 m_StartIn;
	float64 m_EndIn;
	
	uint8 m_Pad[256];

public:
	LookUpTable(FuPointer<LUT> lut);
	LookUpTable(Object *obj, Output *out, TimeStamp time, void *context = NULL);
	LookUpTable();
	
	virtual ~LookUpTable();
	
	LookUpTable(LookUpTable &param);
	virtual Parameter *Copy();
	
	LookUpTable(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual bool Save(ScriptVal &table, const TagList &tags);

	static const LookUpTable &Get(Input *in, Request *req) { return *(LookUpTable *) in->GetValue(req); }

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	bool IsOneToOne();

	void SetOneToOne(bool set);
	
	// Attach/detach ourselves from our source spline.  Can only be done on locally copied LUTs.
	void Attach(Object *source);
	void Detach();

//	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	float64 GetValue(float64 inval);

	virtual FuPointer<LUT> _GetTable(TagList &tags);		// shoulda always been virtual :-(
	FuPointer<LUT> GetTable(TagList &tags);					// still used by Krokodove :-(
	FuPointer<LUT> GetTable(Tag firsttag = NULL, ...);

	static void ApplyLookUpTables(Image *img, LookUpTable *lutr, LookUpTable *lutg, LookUpTable *lutb, LookUpTable *luta);

	// Seeing as we can't make the above virtual, we'll redirect them to these versions..
	virtual bool _IsOneToOne();
	virtual void _SetOneToOne(bool set);
	virtual void _Attach(Object *source);
	virtual void _Detach();
	virtual bool CanDetach();
	virtual float64 _GetValue(float64 inval);

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(LookUpTable);
#endif

protected:
	virtual bool _IsValid();
};

#endif
