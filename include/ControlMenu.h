#ifndef _CONTROLMENU_H_
#define _CONTROLMENU_H_

#error "This file is obsolete"

#include "Object.h"
#include "List.h"
#include "TagList.h"

#if !(defined(DFPLUGIN) || defined(FuPLUGIN))
#include "version.h"
#endif

class ChildFrame;
struct MM;

typedef enum RadialMenuPosEnums
{
	RP_N = 0,
	RP_NE,
	RP_E,
	RP_SE,
	RP_S,
	RP_SW,
	RP_W,
	RP_NW,
} RadialMenuPos;

#define MF_LINEARKEEP	MF_OWNERDRAW		// Keep this item/submenu even when there are no marking menus (ie. DF). It'll just get added to the end
#define MF_ISCHECK		MF_BYPOSITION		// This item is a checkable item - even though it may or may not currently be checked

class FuSYSTEM_API Menu : public Object
{
	FuDeclareClass(Menu, Object); // declare ClassID static member, etc.

protected:
#if (defined(USE_MARKINGMENUS))
	MM *hmenu;
#else
	HMENU hmenu;
#endif

	List Strings;

public:
	Menu *Parent;

	Menu();
	virtual ~Menu();

	uint32 AddRadialMenu(const char *label, RadialMenuPos pos, uint32 flags, Object *obj, uint32 id, uint_ptr d0 = 0, uint_ptr d1 = 0, uint_ptr d2 = 0, uint_ptr d3 = 0, uint_ptr d4 = 0, uint_ptr d5 = 0, uint_ptr d6 = 0, uint_ptr d7 = 0);
	uint32 AddMenu(const char *label, uint32 flags, Object *obj, uint32 id, uint_ptr d0 = 0, uint_ptr d1 = 0, uint_ptr d2 = 0, uint_ptr d3 = 0, uint_ptr d4 = 0, uint_ptr d5 = 0, uint_ptr d6 = 0, uint_ptr d7 = 0);
	void AddSeparator();
	Menu *AddRadialSubMenu(const char *label, RadialMenuPos pos, uint32 flags);
	Menu *AddSubMenu(const char *label, uint32 flags);

	void AddObj(Object *obj);

	void CheckRadioItem(uint32 first, uint32 last, uint32 active);

	uint32 GetLength(void);
	void Remove(void);
	void RemoveLastMenu(void);
};

class FuSYSTEM_API ContextMenu : public Menu
{
	FuDeclareClass(ContextMenu, Menu); // declare ClassID static member, etc.

protected:
	HWND hwnd;

	CPoint point;

public:
	TagList Attrs;
	uint32 ID;
	List ItemList;
	List MenuList;

	ContextMenu();
	virtual ~ContextMenu();
	
	void Create();

	bool DoMenu(CPoint pt, bool lmbonly = FALSE);
	void MenuDone(uint32 id);
};

enum DFM_Attrs
{
	DFM_Frame = TAG_PTR + 1,
	DFM_View,
	DFM_PreviewInfo,
	DFM_DC,
	DFM_OperatorControl,
	DFM_Input,
	DFM_Name,

	DFM_ViewID = TAG_INT + 1,
	DFM_XPos,
	DFM_YPos,
};

enum DFMV
{
	DFMV_Unknown,
	DFMV_Flow,
	DFMV_Time,
	DFMV_Preview,
	DFMV_Control,
	DFMV_KeyFrame,
	DFMV_Graph,
	DFMV_Transport,
};

#endif
