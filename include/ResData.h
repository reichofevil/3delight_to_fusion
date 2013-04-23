#ifndef _RESDATA_H_
#define _RESDATA_H_

#include "LookPack.h"

#include "ResMath.h"

class FuSYSTEM_API ResData : public LookPackItem
{
	FuDeclareClass(ResData, LookPackItem); // declare ClassID static member, etc.

protected:
	const ScriptVal *data;

	const ScriptVal &GetData(void);
	const ScriptVal &GetData(const ScriptVal &key);

	void GetRect(const ScriptVal &key, RECT *rect);
	void GetPosition(const ScriptVal &key, ResPosition *position);
	void GetPoint(const ScriptVal &key, POINT *point);
	void GetSize(const ScriptVal &key, SIZE *size);

public:
	ResData(char *name);
	virtual ~ResData();

	virtual bool Load(void);
	virtual void Release(void);

	TagList *GetTags(void);
//	bool IsLoaded(void) { return (!(tags == NULL)); }

	void GetRect(RECT *rect);
	void GetRect(int32 key, RECT *rect);
	void GetRect(const char *key, RECT *rect);

	void GetPosition(ResPosition *position);
	void GetPosition(int32 key, ResPosition *position);
	void GetPosition(const char *key, ResPosition *position);

	inline void GetPosition(const ResPosition *position) { GetPosition((ResPosition *) position); }
	inline void GetPosition(int32 key, const ResPosition *position) { GetPosition(key, (ResPosition *) position); }
	inline void GetPosition(const char *key, const ResPosition *position) { GetPosition(key, (ResPosition *) position); }

	void GetPoint(POINT *point);
	void GetPoint(int32 key, POINT *point);
	void GetPoint(const char *key, POINT *point);

	void GetSize(SIZE *size);
	void GetSize(int32 key, SIZE *size);
	void GetSize(const char *key, SIZE *size);
};

#endif // _RESDATA_H_
