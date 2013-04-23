#ifndef _NUMBER_H_
#define _NUMBER_H_

#include "Parameter.h"
#define NUMBER_ID CLSID_DataType_Number
//#define CLSID_DataType_Number NUMBER_ID

class FuSYSTEM_API Number : public Parameter
{
	FuDeclareClass(Number, Parameter); // declare ClassID static member, etc.

public:
	float64 Value;

public:
	Number(float64 value = 0.0);
	Number(const Registry *reg, const ScriptVal &table, const TagList &tags);
	Number(Number &param);
	virtual bool Save(ScriptVal &table, const TagList &tags);

	virtual ~Number();

	static const Number &Get(Input *in, Request *req) { return *(Number *) in->GetValue(req); }

	operator double() const			{ return Value; }
	double operator =(double newval)		{ return (Value = newval); }
	virtual Parameter *Copy();

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(Number);
#endif

protected:
	virtual bool _IsValid();
};

class FuSYSTEM_API AutoNumber : public AutoParameter
{
	FuDeclareClass(AutoNumber, AutoParameter); // declare ClassID static member, etc.

public:
	AutoNumber(float64 value = 0.0)	{ Param = new Number(value); }
	AutoNumber(Input *in, TimeStamp time) : AutoParameter(in, time) {}
	AutoNumber(Input *in, Request *req, TimeStamp time = TIME_UNDEFINED) : AutoParameter(in, req, time) {}
	operator Number *()					{ return (Number *)Param; }
};

inline float64 SafeGetNumber(Input *in, Request *req, TimeStamp time = TIME_UNDEFINED)
{
	if (in && req)
	{
		AutoNumber Val(in, req, time);

		if ((Parameter *)Val)
			return *Val;
	}
	
	return 0.0;
}

inline float64 SafeGetNumber(Input *in, TimeStamp time, uint32 flags = 0, uint32 flagmask = 0, float64 def = 0.0)
{
	if (in)
	{
		Number *num = (Number *) in->GetSourceAttrs(time, NULL, flags, flagmask);
		if (num)
		{
			def = *num;
			num->Recycle();
		}
	}
	
	return def;
}

#endif
