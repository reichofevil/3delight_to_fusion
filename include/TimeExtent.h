#ifndef _TIMEEXTENT_H_
#define _TIMEEXTENT_H_

#include "Object.h"

class FuSYSTEM_API TimeExtent
{
public:
	TimeStamp Start;
	TimeStamp End;

	TimeStamp FrameLength;
	
	TimeExtent(TimeStamp start = TIME_UNDEFINED, TimeStamp end = TIME_UNDEFINED, TimeStamp length = TIME_UNDEFINED);
	TimeExtent(const TimeExtent &te);
	
	TimeExtent(const ScriptVal &table, const TagList &tags);
	bool Save(ScriptVal &table, const TagList &tags);
	
	TimeExtent & operator &= (const TimeExtent &ext);
	TimeExtent & operator |= (const TimeExtent &ext);

	bool IsWithin(TimeStamp time) const;
	bool IsWithin(const TimeExtent &te) const;
	bool Intersects(const TimeExtent &te) const;
	bool IsEmpty()		{ 	return (fabs(Start - TIME_UNDEFINED) < TIME_FUZZ && fabs(End - TIME_UNDEFINED) < TIME_FUZZ); }

	bool operator ==(const TimeExtent &te) const;
};

#ifndef USE_NOSCRIPTING
extern FuSYSTEM_API LuaType *g_TimeExtentType;
#endif

#endif

