#ifndef _Menu_h_
#define _Menu_h_

#include "Object.h"
#include "List.h"
#include "TagList.h"
#include "ResColour.h"
#include "ContextMenu.h"

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
	
	friend class ContextMenu;

protected:
	ContextMenu *m_Root;
	Menu *m_Parent;
	int32 m_NumItems;
	int32 m_WrapLimit;
//	ResColor m_Color;

protected:
	virtual ~Menu(); // No stack objects allowed.
	static Menu *CreateMenu(ContextMenu *root, const TagList &tags);

public:
	Menu(const Registry *reg, const ScriptVal &table, const TagList &tags);

	// Legacy stuff:
	uint32 AddSeparator();
	uint32 AddRadialMenu(const char *label, RadialMenuPos pos, uint32 flags, Object *obj, uint32 id, uint_ptr d0 = 0, uint_ptr d1 = 0, uint_ptr d2 = 0, uint_ptr d3 = 0, uint_ptr d4 = 0, uint_ptr d5 = 0, uint_ptr d6 = 0, uint_ptr d7 = 0);
	uint32 AddMenu(const char *label, uint32 flags, Object *obj, uint32 id, uint_ptr d0 = 0, uint_ptr d1 = 0, uint_ptr d2 = 0, uint_ptr d3 = 0, uint_ptr d4 = 0, uint_ptr d5 = 0, uint_ptr d6 = 0, uint_ptr d7 = 0);
	Menu *AddRadialSubMenu(const char *label, RadialMenuPos pos, uint32 flags);
	Menu *AddSubMenu(const char *label, uint32 flags);

	// New stuff
	uint32 AddSeparator(const TagList &tags);
	uint32 AddSeparator(Tag firsttag, ...);
	uint32 AddMenu(const char *label, Object *obj, const FuID &id, const ScriptVal &data, const TagList &tags);
	uint32 AddMenu(const char *label, Object *obj, const FuID &id, const ScriptVal &data, Tag firsttag, ...);
	Menu *AddSubMenu(const char *label, Object *obj, const FuID &id, const ScriptVal &data, const TagList &tags);
	Menu *AddSubMenu(const char *label, Object *obj, const FuID &id, const ScriptVal &data, Tag firsttag, ...);
	
	void AddObj(Object *obj);

	void CheckRadioItem(uint32 first, uint32 last, uint32 active);
	void SetItemAttrs(uint32 id, const TagList &tags);

	uint32 GetLength();
	void Remove();
	void RemoveLastMenu();
	void RemoveMenu(uint32 id);
	void SetParent(Menu *parent);

protected:
	// pure virtual overrides
	virtual bool _AddSeparator(uint32 id, const TagList &tags) = 0;
	virtual bool _AddMenuItem(uint32 id, WCHAR *label, const TagList &tags) = 0;
	virtual bool _AddSubMenu(uint32 id, WCHAR *label, Menu *menu, const TagList &tags) = 0;
	virtual void _CheckRadioItem(uint32 first, uint32 last, uint32 active) = 0;
	virtual bool _SetMenuAttrs(const TagList &tags) = 0;
	virtual bool _SetItemAttrs(uint32 id, const TagList &tags) = 0;
	virtual bool _RemoveItem(uint32 id, const TagList &tags) = 0;
	
	virtual uint32 _DoMenu(HWND handler, int32 x, int32 y, bool lmbonly, const TagList &tags) = 0;
};

enum MENU_Justifcation
{
	// Bottom 2 bits
	MENUJ_Left = 1,
	MENUJ_HCenter = 2,
	MENUJ_Right = 3,

	// Next 2 bits
	MENUJ_Top = 4,
	MENUJ_VCenter = 8,
	MENUJ_Bottom = 12,
};

#define MENU_BASE OBJECT_BASE + TAGOFFSET

enum Menu_Tags
{
	MENU_Disabled = MENU_BASE | TAG_INT,       // Boolean, menu is disabled
	MENU_IsCheck,                              // Boolean, menu is 'checkable'
	MENU_IsMXCheck,                            // Boolean, menu is 'checkable' as part of an MX group
	MENU_Checked,                              // Boolean, menu is currently checked
	MENU_Bold,                                 // Boolean, display text in bold
	MENU_Italic,                               // Boolean, display text in italic
	MENU_Underline,                            // Boolean, display text in underline
	MENU_Horizontal,                           // Boolean, lay out menu horizontally
	MENU_LabelJustification,                   // Label justification, see MENU_Justification enum
	MENU_ImageJustification,                   // Image justification, see MENU_Justification enum
	MENU_Section,                              // Section for this menu, see MENU_Section enum
	MENU_WrapLimit,                            // Wrap items at this point (default 50 items)
	MENU_Alignment,
	MENU_ManagerMenu,


	MENUP_DIB = MENU_BASE | TAG_PTR,           // DIB *, image for this item
	MENUP_DIBHot,                              // DIB *, image for this item when 'hot' (pointed to)
	MENUP_BackColor,                          // ResColor *, background color for this item
	MENUP_BackColorHot,                       // ResColor *, background color for this item when 'hot' (pointed to)
	MENUP_TextColor,                          // ResColor *, text color for this item
	MENUP_TextColorHot,                       // ResColor *, text color for this item when 'hot' (pointed to)
	
	MENUD_FontSize = MENU_BASE | TAG_DOUBLE,   // Size of font

	MENUS_FontName = MENU_BASE | TAG_STRING,   // Name of font
};

#endif
