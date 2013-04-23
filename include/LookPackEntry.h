#ifndef _LOOKPACKENTRY_H_
#define _LOOKPACKENTRY_H_

#include "TagList.h"

class _LookPackInfo;
class SkinCategory;
class LookPackCache;

#ifndef DFENGINE
class LookPackEntry : public LockableObject
{
	FuDeclareClass(LookPackEntry, LockableObject); // declare ClassID static member, etc.

public:
	char *m_pName;

	int32 ItemUseCount;

	_LookPackInfo *m_pCurrInfo;
	const SkinCategory *m_pSkinCategory;
	LookPackCache *m_pCache;

	HBITMAP BMap;
	uint32 BMapRef;

	HDC DC;
	uint32 DCRef;

	uint32 bgcol;
	uint32 fgcol;

	bool onbmaplist;

public:
	LookPackEntry();
	LookPackEntry(const SkinCategory *category, const char *name);
	virtual ~LookPackEntry();

	void Init(void);

	virtual void ItemUse(void);
	virtual void ItemRecycle(void);

	virtual HBITMAP GetDIB(void);
	virtual void ReleaseDIB(void);

	virtual HDC GetDC(void);
	virtual void ReleaseDC(void);

	virtual FuID GetFullEntryName(void);

	virtual const ScriptVal &GetData(void);
};

class FuSYSTEM_API LookPackItem : public Object
{
	FuDeclareClass(LookPackItem, Object); // declare ClassID static member, etc.

protected:
	LookPackEntry *m_pEntry;
	Node *m_pLookPackNode;

	char *m_pName;

	bool m_bHasDefault;

public:
	LookPackItem();
	LookPackItem(char *name);
	virtual ~LookPackItem();

	virtual bool Load(void);
	virtual void Release(void);

	virtual bool IsLoaded(void);
	virtual uint32 GetCurrLPID(void);

	const ScriptVal &GetItemData(void);
	void SetHasDefault(bool has) { m_bHasDefault = has; }
};
#else
class FuSYSTEM_API LookPackItem : public Object
{
	FuDeclareClass(LookPackItem, Object); // declare ClassID static member, etc.

protected:
	char *m_pName;
	bool m_bLoaded;

public:
	LookPackItem() { m_pName = NULL; m_bLoaded = false; }
	LookPackItem(char *name) { m_pName = NULL; m_bLoaded = false; }
	virtual ~LookPackItem() { }

	virtual bool Load(void) { m_bLoaded = true; return true; }
	virtual void Release(void) { }

	virtual bool IsLoaded(void) { return m_bLoaded; }
	virtual uint32 GetCurrLPID(void) { return 0; }

	const ScriptVal &GetItemData(void) { return nil; }
	void SetHasDefault(bool has) { }
};
#endif

#endif // _LOOKPACKENTRY_H_
