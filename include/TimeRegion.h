#ifndef _TIMEREGION_H_
#define _TIMEREGION_H_

#include "List.h"
#include "TimeExtent.h"

class FuSYSTEM_API TimeRegion : public ListT<TimeExtent>
{
	FuDeclareClass(TimeRegion, List); // declare ClassID static member, etc.

public:
	TimeExtent Bounds;

	TimeRegion();
	TimeRegion(const Registry *reg, const ScriptVal &table, const TagList &tags);

	virtual bool Save(ScriptVal &table, const TagList &tags);

	bool Clear();

	bool Union(TimeExtent te);
	bool Intersect(TimeExtent te);

	bool Union(TimeRegion &tr);
	bool Intersect(TimeRegion &tr);

	void Invert();

	TimeStamp GetMin() { return Bounds.Start; }
	TimeStamp GetMax() { return Bounds.End; }
	bool IsWithin(TimeStamp time, TimeExtent *range = NULL);
	bool IsEmpty()		{ 	return Bounds.IsEmpty(); }

	bool operator ==(TimeRegion &tr);

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(TimeRegion);
#endif
};

#ifndef USE_NOSCRIPTING
extern FuSYSTEM_API LuaType *g_TimeRegionType;
#endif

#endif

