#ifndef _LOOKPACK_H_
#define _LOOKPACK_H_

#include "HashList.h"
#include "LookPackEntry.h"

#if defined(VISION)
#define DEF_LOOKPACK_ID		MAKEID('Visn')
#define DEF_LOOKPACK_FILE	"Vision.fuskin"
//#elif defined(ROTATION)
//#define DEF_LOOKPACK_ID		MAKEID('Rotn')
//#define DEF_LOOKPACK_FILE	"Rotation.fuskin"
#else
#define DEF_LOOKPACK_ID		MAKEID('Fu61')
#define DEF_LOOKPACK_FILE	"Fu61.fuskin"
#endif

#define DEF_TEXT_COLOR					RGB(192, 192, 192)	//
#define DEF_SEL_TEXT_COLOR				RGB(192, 192, 192)	//
#define DEF_SEL_TEXT_BACK_COLOR		RGB( 70,  85, 132)	//
#define DEF_DIS_TEXT_COLOR				RGB(128, 128, 128)	//

#define DEF_EDIT_TEXT_COLOR			RGB(192, 192, 192)	//
#define DEF_EDIT_TEXT_BACK_COLOR		RGB( 45,  45,  45)	//

#define DEF_BACK_COLOR					RGB( 52,  52,  52)	//
#define DEF_SHINE_COLOR					RGB( 70,  70,  70)	//
#define DEF_SHADOW_COLOR				RGB( 34,  34,  34)	//

#define DEF_INNER_COLOR					RGB( 52,  52,  52)	//
#define DEF_INNER_BACK_COLOR			RGB( 52,  52,  52)	//
#define DEF_OUTER_COLOR					RGB( 70,  70,  70)	//
#define DEF_OUTER_BACK_COLOR			RGB( 34,  34,  34)	//

#ifndef DFENGINE
class LookPackCache : public LockableObject
{
	FuDeclareClass(LookPackCache, LockableObject); // declare ClassID static member, etc.

public:
	_LookPackInfo *m_pCurrInfo;
	const char *m_pName;
	Node *m_pCacheNode;

	int32 ItemUseCount;

	HBITMAP BMap;
	HBITMAP OldBM;
	uint32 BMapRef;

	HDC DC;
	uint32 DCRef;

public:
	LookPackCache(_LookPackInfo *info, const char *name);
	virtual ~LookPackCache();

	virtual void ItemUse(void);
	virtual void ItemRecycle(void);

	virtual HBITMAP GetDIB(void);
	virtual void ReleaseDIB(void);

	virtual HDC GetDC(void);
	virtual void ReleaseDC(void);
};

// Internal structures used by the LookPack system, and the LookPack prefs system
class _LookPackInfo : public Object
{
	FuDeclareClass(LookPackInfo, Object); // declare ClassID static member, etc.

public:
	_LookPackInfo(char *name, char *fname, uint32 id);
	virtual ~_LookPackInfo();

	virtual const ScriptVal *GetData(const SkinCategory *cat, const char *name) const  = NULL;

	virtual LookPackCache *GetCache(const LookPackEntry *entry);

	virtual const char *GetBitmapName(const LookPackEntry *entry) const = NULL;
	virtual BITMAPINFO *GetBitmap(const char *name) const = NULL;
	virtual void FreeBitmap(BITMAPINFO *bmi) const = NULL;

public:
	char *m_pName;
	char *m_pFileName;
	uint32 ID;

	HashList m_Bitmaps;
};

class FuSkinInfo : public _LookPackInfo
{
	FuDeclareClass(FuSkinInfo, _LookPackInfo); // declare ClassID static member, etc.

public:
	FuSkinInfo(char *name, char *fname, uint32 id, const ScriptVal &skin);
	virtual ~FuSkinInfo();

	virtual const ScriptVal *GetData(const SkinCategory *cat, const char *name) const;

	virtual const char *GetBitmapName(const LookPackEntry *entry) const;
	virtual BITMAPINFO *GetBitmap(const char *name) const;
	virtual void FreeBitmap(BITMAPINFO *bmi) const;

	bool m_bCompressed;

public:
	const ScriptVal m_SkinData;
};

class SkinCategory : public Object
{
	FuDeclareClass(SkinCategory, Object); // declare ClassID static member, etc.

public:
	char *m_pName;

	const SkinCategory *m_pParent;
	List *m_pChildren;

	List m_Entries;
	List m_Skins;
	List m_Priorities;

	SkinCategory(const SkinCategory *parent, const char *name);
	virtual ~SkinCategory();

	virtual _LookPackInfo *GetInfo(const LookPackEntry *entry) const;
	virtual FuID GetFullName(void) const;
};

class FuSYSTEM_API LookPack : public LockableObject
{
	FuDeclareClass(LookPack, LockableObject); // declare ClassID static member, etc.

public:
	LookPack();
	virtual ~LookPack();

	virtual bool LoadSkin(char *FileName);     // Zip file

	virtual LookPackEntry *GetEntry(char *name, bool hasdef = FALSE);

	virtual void PrefsChanged(void);
	virtual void LoadSettings(void);
	virtual void SaveSettings(void);

protected:
	virtual LookPackEntry *GetEntry(const SkinCategory *cat, char *name);
	virtual LookPackEntry *CreateEntry(SkinCategory *cat, char *name);

	virtual void LoadCategorySettings(SkinCategory *cat, char *regparent);

	virtual void RemovePrevSettings(HKEY parent, char *regname);
	virtual void SaveCategorySettings(const SkinCategory *cat, char *regparent);

	virtual bool LoadSkinCategory(SkinCategory *parent, const ScriptVal &Skin, const char *FileName, const FuSkinInfo *skininfo);

public:
	List m_LookPackList;
	List m_ResList;

	SkinCategory *m_pSkinRoot;
};
#else
class LookPack;
#endif

FuSYSTEM_API extern LookPack *LookState;

#include "ResBitmap.h"
#include "ResColour.h"
#include "ResData.h"

#endif // _LOOKPACK_H_
