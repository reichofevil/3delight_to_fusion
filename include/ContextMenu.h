#ifndef _ContextMenu_h_
#define _ContextMenu_h_

#include "Object.h"
#include "List.h"
#include "TagList.h"

class Menu;

struct MenuData
{
	Object *m_Obj;
	
	// Old
	uint32 m_ID;
	uint_ptr m_Data[8];

	// New
	ScriptVal m_NewID;
	ScriptVal m_NewData;

	WCHAR *m_Name;
};

class FuSYSTEM_API ContextMenu : public Object
{
	FuDeclareClass(ContextMenu, Object); // declare ClassID static member, etc.

protected:
public:
	//MENUTODO: Should not be public
	HWND m_Window;
	uint32 m_ID;
	List m_ItemList;
	List m_MenuList;
	List m_Strings;
	Menu *m_RootMenu;
	TagList m_Attrs;
	FuID m_MenuClass;

public:
	ContextMenu();
	virtual ~ContextMenu();
	
	virtual Menu *Create(const TagList &tags);

	virtual MenuData *DoMenu(int32 x, int32 y, bool lmbonly = false, uint32 align = 0);
	virtual void MenuDone(MenuData *md);
	virtual Menu *CreateMenu(const TagList &tags);
	
	uint32 GetNextID();

	WCHAR *AddString(const char *utf8);
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
	DFM_Object,
	DFM_ActiveWnd,
	DFM_FocusWnd,

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
