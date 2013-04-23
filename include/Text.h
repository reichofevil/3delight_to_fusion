#ifndef _TEXT_H_
#define _TEXT_H_

#include "Parameter.h"
#include "Input.h"

#define TEXT_ID zMAKEID('Text')
//#define CLSID_DataType_Text TEXT_ID

class FuSYSTEM_API Text : public Parameter
{
	FuDeclareClass(Text, Parameter); // declare ClassID static member, etc.

public:
	CString String;

public:

	Text(const char *str = NULL);
	Text(const Registry *reg, const ScriptVal &table, const TagList &tags);
	Text(Text &param);
	virtual bool Save(ScriptVal &table, const TagList &tags);

	static const Text &Get(Input *in, Request *req) { return *(Text *) in->GetValue(req); }

	operator const char *() const
	{
#if defined(NO_MFC) || defined(WINCE)
		return NULL; //TODO: Fix!
#else
		return (const char *)String;
#endif
	}
	
	const char * operator =(const char *newstr)		{ String = newstr; return newstr; }
	virtual Parameter *Copy();

	virtual Parameter *InterpolateWith(double weight, Parameter *rightvalue, TagList *tags = NULL);

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(Text);
#endif

protected:
	virtual bool _IsValid();
};

class AutoText : public AutoParameter
{
	FuDeclareClass(AutoText, AutoParameter); // declare ClassID static member, etc.

public:
	AutoText(const char *str = NULL)	{ Param = new Text(str); }
	AutoText(Input *in, TimeStamp time) : AutoParameter(in, time) {}
	operator Text *()						{ return (Text *)Param; }
};

#endif
