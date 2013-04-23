#ifndef _FUIDPARAM_H_
#define _FUIDPARAM_H_

#include "Parameter.h"

class FuSYSTEM_API FuIDParam : public Parameter
{
	FuDeclareClass(FuIDParam, Parameter); // declare ClassID static member, etc.

public:
	FuID Value;

public:
	FuIDParam(const FuID &value);
	FuIDParam(const Registry *reg, const ScriptVal &table, const TagList &tags);
	FuIDParam(const FuIDParam &param);
	virtual bool Save(ScriptVal &table, const TagList &tags);

	static const FuIDParam &Get(Input *in, Request *req) { return *(FuIDParam *) in->GetValue(req); }

	virtual ~FuIDParam();

	operator FuID() const						{ return Value; }
	FuID operator =(const FuID &newval)		{ return (Value = newval); }
	virtual Parameter *Copy();

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(FuIDParam);
#endif

protected:
	virtual bool _IsValid();
};

class FuSYSTEM_API AutoFuIDParam : public AutoParameter
{
	FuDeclareClass(AutoFuIDParam, AutoParameter); // declare ClassID static member, etc.

public:
	AutoFuIDParam(const FuID &value)	{ Param = new FuIDParam(value); }
	AutoFuIDParam(Input *in, TimeStamp time) : AutoParameter(in, time) {}
	AutoFuIDParam(Input *in, Request *req, TimeStamp time = TIME_UNDEFINED) : AutoParameter(in, req, time) {}
	operator FuIDParam *()					{ return (FuIDParam *)Param; }
};

inline FuID SafeGetFuIDParam(Input *in, Request *req, TimeStamp time = TIME_UNDEFINED)
{
	if (in && req)
	{
		AutoFuIDParam Val(in, req, time);

		if ((Parameter *)Val)
			return *Val;
	}
	
	return NOID;
}

inline FuID SafeGetFuIDParam(Input *in, TimeStamp time, uint32 flags = 0, uint32 flagmask = 0, FuID def = NOID)
{
	if (in)
	{
		FuIDParam *num = (FuIDParam *) in->GetSourceAttrs(time, NULL, flags, flagmask);
		if (num)
		{
			def = *num;
			num->Recycle();
		}
	}
	
	return def;
}

// pass an id and an array of IDs, terminated by NOID
inline int GetDFIDIndex(const FuID id, const FuID idarray[])
{
	int i;
	for (i = 0; idarray[i] != NOID && idarray[i] != id; i++)
		;
	return (idarray[i] != NOID) ? i : -1;
}

inline int GetDFIDIndex(const FuID id, const FuID idarray[], int idArraySize)
{
	for (int i = 0; i < idArraySize; i++)
		if (idarray[i] == id)
			return i;
	return -1;
}

#endif
