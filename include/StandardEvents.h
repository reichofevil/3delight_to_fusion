#ifndef _STANDARDEVENTS_H_
#define _STANDARDEVENTS_H_

// Standard menu/toolbar events.
// These are events that cause things to happen

extern FuSYSTEM_API const FuID EventID_Beverage_Request;
extern FuSYSTEM_API const FuID EventID_Game_Request;

extern FuSYSTEM_API const FuID EventID_File_New;
extern FuSYSTEM_API const FuID EventID_File_Open;
extern FuSYSTEM_API const FuID EventID_File_Close;
extern FuSYSTEM_API const FuID EventID_File_Save;
extern FuSYSTEM_API const FuID EventID_File_SaveAs;
extern FuSYSTEM_API const FuID EventID_File_RecentFile;
extern FuSYSTEM_API const FuID EventID_File_StartRender;
extern FuSYSTEM_API const FuID EventID_File_RenderManager;
extern FuSYSTEM_API const FuID EventID_File_Bins;
extern FuSYSTEM_API const FuID EventID_File_Digitize_Output;
extern FuSYSTEM_API const FuID EventID_File_TapeFunctions;
extern FuSYSTEM_API const FuID EventID_File_Preferences;
extern FuSYSTEM_API const FuID EventID_File_Exit;

extern FuSYSTEM_API const FuID EventID_Edit_Undo;
extern FuSYSTEM_API const FuID EventID_Edit_Redo;
extern FuSYSTEM_API const FuID EventID_Edit_Cut;
extern FuSYSTEM_API const FuID EventID_Edit_Copy;
extern FuSYSTEM_API const FuID EventID_Edit_Paste;
extern FuSYSTEM_API const FuID EventID_Edit_PasteInstance;
extern FuSYSTEM_API const FuID EventID_Edit_PasteSettings;
extern FuSYSTEM_API const FuID EventID_Edit_Delete;
extern FuSYSTEM_API const FuID EventID_Edit_SelectAll;
extern FuSYSTEM_API const FuID EventID_Edit_SaveSettings;

extern FuSYSTEM_API const FuID EventID_View_FileBar;
extern FuSYSTEM_API const FuID EventID_View_ViewBar;
extern FuSYSTEM_API const FuID EventID_View_StatusBar;
extern FuSYSTEM_API const FuID EventID_View_CustomiseToolbars;
extern FuSYSTEM_API const FuID EventID_View_ShowDisplay;
extern FuSYSTEM_API const FuID EventID_View_ShowFlow;
extern FuSYSTEM_API const FuID EventID_View_ShowTimeline;
extern FuSYSTEM_API const FuID EventID_View_ShowSpline;
extern FuSYSTEM_API const FuID EventID_View_ShowDisplaySmall;
extern FuSYSTEM_API const FuID EventID_View_ShowControl;
extern FuSYSTEM_API const FuID EventID_View_TileSize;
extern FuSYSTEM_API const FuID EventID_View_ShowTilePics;
extern FuSYSTEM_API const FuID EventID_View_ShowToolNames;
extern FuSYSTEM_API const FuID EventID_View_ShowTimeCode;
extern FuSYSTEM_API const FuID EventID_View_ShowModifier;
extern FuSYSTEM_API const FuID EventID_View_ShowScript;

extern FuSYSTEM_API const FuID EventID_View_Tool;

extern FuSYSTEM_API const FuID EventID_Tools_AddTool;
extern FuSYSTEM_API const FuID EventID_Tools_ConnectTo;

extern FuSYSTEM_API const FuID EventID_Window_NewView;
extern FuSYSTEM_API const FuID EventID_Window_NewWindow;
extern FuSYSTEM_API const FuID EventID_Window_Cascade;
extern FuSYSTEM_API const FuID EventID_Window_Tile;
extern FuSYSTEM_API const FuID EventID_Window_Arrange;

extern FuSYSTEM_API const FuID EventID_Help_Help;
extern FuSYSTEM_API const FuID EventID_Help_WebHome;
extern FuSYSTEM_API const FuID EventID_Help_WebRegistration;
extern FuSYSTEM_API const FuID EventID_Help_WebUpdates;
extern FuSYSTEM_API const FuID EventID_Help_WebSupport;
extern FuSYSTEM_API const FuID EventID_Help_About;

extern FuSYSTEM_API const FuID EventID_ChangeTime;

// These are more notification type events. Things can be done because
// of these, but are generated more as a result of some other previous event
extern FuSYSTEM_API const FuID EventID_RenderStarted;
extern FuSYSTEM_API const FuID EventID_RenderFrameComplete;
extern FuSYSTEM_API const FuID EventID_RenderRenderComplete;

extern FuSYSTEM_API const FuID EventID_Doc_TimeChanged;

extern FuSYSTEM_API const FuID EventID_Doc_AddTool;
extern FuSYSTEM_API const FuID EventID_Doc_AddModifier;
extern FuSYSTEM_API const FuID EventID_Doc_AddConverter;
extern FuSYSTEM_API const FuID EventID_Doc_AddMask;

extern FuSYSTEM_API const FuID EventID_Doc_DropOp;

extern FuSYSTEM_API const FuID EventID_Doc_NameChange;
extern FuSYSTEM_API const FuID EventID_Doc_NoUpdate;
extern FuSYSTEM_API const FuID EventID_Doc_LaunchScript;

extern FuSYSTEM_API const FuID EventID_Doc_SetGUIMode;

extern FuSYSTEM_API const FuID EventID_Dragging;

extern FuSYSTEM_API const FuID EventID_Dummy;

extern FuSYSTEM_API const FuID EventID_Operator_MenuTop;
extern FuSYSTEM_API const FuID EventID_Operator_MenuBottom;
extern FuSYSTEM_API const FuID EventID_Operator_Options;
extern FuSYSTEM_API const FuID EventID_Operator_Settings;
extern FuSYSTEM_API const FuID EventID_Operator_Rename;
extern FuSYSTEM_API const FuID EventID_Operator_EditAll;
extern FuSYSTEM_API const FuID EventID_Operator_Delete;
extern FuSYSTEM_API const FuID EventID_Operator_Load;
extern FuSYSTEM_API const FuID EventID_Operator_LoadDef;
extern FuSYSTEM_API const FuID EventID_Operator_Save;
extern FuSYSTEM_API const FuID EventID_Operator_SaveDef;
extern FuSYSTEM_API const FuID EventID_Operator_ResetDef;
extern FuSYSTEM_API const FuID EventID_Operator_ShowControls;
extern FuSYSTEM_API const FuID EventID_Operator_Locked;
extern FuSYSTEM_API const FuID EventID_Operator_HoldOutput;
extern FuSYSTEM_API const FuID EventID_Operator_CacheToDisk;
extern FuSYSTEM_API const FuID EventID_Operator_PassThrough;
extern FuSYSTEM_API const FuID EventID_Operator_Cut;
extern FuSYSTEM_API const FuID EventID_Operator_Copy;
extern FuSYSTEM_API const FuID EventID_Operator_Paste;
extern FuSYSTEM_API const FuID EventID_Operator_PasteInstance;
extern FuSYSTEM_API const FuID EventID_Operator_PasteSettings;
extern FuSYSTEM_API const FuID EventID_Operator_SetRenderRange;
extern FuSYSTEM_API const FuID EventID_Operator_SwapInputs;
extern FuSYSTEM_API const FuID EventID_Operator_View;
extern FuSYSTEM_API const FuID EventID_Operator_ViewOn;
extern FuSYSTEM_API const FuID EventID_Operator_Recent;

extern FuSYSTEM_API const FuID EventID_Operator_Preview;
extern FuSYSTEM_API const FuID EventID_Operator_PreviewOn;

extern FuSYSTEM_API const FuID EventID_Operator_ForceCacheToRAM;
extern FuSYSTEM_API const FuID EventID_Operator_MenuMiddle;
extern FuSYSTEM_API const FuID EventID_Operator_GetToolAttrs;
extern FuSYSTEM_API const FuID EventID_Operator_Script;
extern FuSYSTEM_API const FuID EventID_Operator_RunScript;
extern FuSYSTEM_API const FuID EventID_Operator_GetTileText;
extern FuSYSTEM_API const FuID EventID_Operator_Renamed;
extern FuSYSTEM_API const FuID EventID_Operator_Deleted;
extern FuSYSTEM_API const FuID EventID_Operator_Cluster;
extern FuSYSTEM_API const FuID EventID_Operator_ScriptEdit;
extern FuSYSTEM_API const FuID EventID_Operator_ScriptEdit_Edit;
extern FuSYSTEM_API const FuID EventID_Operator_Display;

extern FuSYSTEM_API const FuID EventID_Operator_DeInstance;

extern FuSYSTEM_API const FuID EventID_Operator_ControlToolbar;
extern FuSYSTEM_API const FuID EventID_Operator_SetCurrentSettings;

extern FuSYSTEM_API const FuID EventID_Controls_PushPin;

extern FuSYSTEM_API const FuID EventID_Preview_Size;
extern FuSYSTEM_API const FuID EventID_Preview_Sizes;
extern FuSYSTEM_API const FuID EventID_Preview_TrimChanged;
extern FuSYSTEM_API const FuID EventID_Preview_DisplayImage;

extern FuSYSTEM_API const FuID EventID_Floating_SwapInputs;

extern FuSYSTEM_API const FuID EventID_PassThrough;
extern FuSYSTEM_API const FuID EventID_Locked;
extern FuSYSTEM_API const FuID EventID_HoldOutput;
extern FuSYSTEM_API const FuID EventID_DropObjects;
extern FuSYSTEM_API const FuID EventID_Rename;

extern FuSYSTEM_API const FuID EventID_Select_Paint_Stroke;
extern FuSYSTEM_API const FuID EventID_Select_Text;

extern FuSYSTEM_API const FuID EventID_Console_Output;

extern FuSYSTEM_API const FuID EventID_Invalidate_Extent;

extern FuSYSTEM_API const FuID EventID_PrefsChanged;

extern FuSYSTEM_API const FuID EventID_Param_SetRealQuality;
extern FuSYSTEM_API const FuID EventID_Param_SetQuality;
extern FuSYSTEM_API const FuID EventID_Param_IsComparable;
extern FuSYSTEM_API const FuID EventID_Param_Limit;
extern FuSYSTEM_API const FuID EventID_Param_PostLimit;
extern FuSYSTEM_API const FuID EventID_Param_FreeTempData;
extern FuSYSTEM_API const FuID EventID_Param_CacheMapOut;
extern FuSYSTEM_API const FuID EventID_Param_CacheMapIn;
extern FuSYSTEM_API const FuID EventID_Param_SwapOut;
extern FuSYSTEM_API const FuID EventID_Param_SetReadOnly;   // SubID is true/false
extern FuSYSTEM_API const FuID EventID_Param_IsWithin;

extern FuSYSTEM_API const FuID EventID_LUT_GetValue;
extern FuSYSTEM_API const FuID EventID_LUT_GetTable;
extern FuSYSTEM_API const FuID EventID_LUT_GetAttrs;
extern FuSYSTEM_API const FuID EventID_LUT_IsOneToOne;


extern FuSYSTEM_API const FuID EventID_Free_Memory;			// SubID is a ptr to a uint32 containing amount of memory needed. Set the uint32 to the amount actually freed.

extern FuSYSTEM_API const FuID EventID_DeviceChange;			// SubID is event type (e.g. DBT_DEVICEARRIVAL)

extern FuSYSTEM_API const FuID EventID_Console_Clear;

extern FuSYSTEM_API const FuID EventID_Operator_GetBaseColor;
extern FuSYSTEM_API const FuID EventID_Operator_GetColor;
extern FuSYSTEM_API const FuID EventID_Operator_SetColors;	// submenu parent
extern FuSYSTEM_API const FuID EventID_Operator_SetColor;

extern FuSYSTEM_API const FuID EventID_ToolbarControl_Menu;

extern FuSYSTEM_API const FuID EventID_App_ExitInstance;

extern FuSYSTEM_API const FuID EventID_Operator_Key_GetNum;
extern FuSYSTEM_API const FuID EventID_Operator_Key_GetTime;
extern FuSYSTEM_API const FuID EventID_Operator_Key_GetIndex;
extern FuSYSTEM_API const FuID EventID_Operator_Key_Delete;
extern FuSYSTEM_API const FuID EventID_Operator_Key_DeleteAll;



// ##### Toolbar controls/tags #####

// Toolbar event controls

#define TB_BASE		EVCTRL_USER
#define TB_USER		TB_BASE + 100

enum ReBarIDs
{
	TB_ReBarID_MainFrameLeft = 1,
	TB_ReBarID_MainFrameTop,
	TB_ReBarID_MainFrameRight,
	TB_ReBarID_MainFrameBottom,
	TB_ReBarID_LargePreviewLeft,
	TB_ReBarID_LargePreviewTop,
	TB_ReBarID_LargePreviewRight,
	TB_ReBarID_LargePreviewBottom,
	TB_ReBarID_SmallPreviewLeft,
	TB_ReBarID_SmallPreviewTop,
	TB_ReBarID_SmallPreviewRight,
	TB_ReBarID_SmallPreviewBottom,
};

enum ToolbarTags ENUM_TAGS
{
	TB_ControlStyle = TB_BASE | TAG_INT,
	TB_ControlSize,
	TB_ID,
	TB_ReBarID,
	TB_Visible,
	TB_Size,
	TB_Break,
	TB_Docked,
	TB_Title,
	TB_NoStyleMenu,

	TB_Toolbar = TB_BASE | TAG_TAGLIST | TAG_MULTI,

	TB_Name = TB_BASE | TAG_PTR,
};

typedef enum ToolbarStyles
{
	ToolbarStyle_Icons = 0,
	ToolbarStyle_IconsText,
	ToolbarStyle_Text,
} ToolbarControlStyle;

typedef enum ToolbarSizes
{
	ToolbarSize_Small = -1,
	ToolbarSize_Normal,
	ToolbarSize_Large,
} ToolbarControlSize;


#define TBC_BASE		TB_USER
#define TBC_USER		TBC_BASE + 100

// Toolbar Control tags

enum ToolbarControlTags ENUM_TAGS
{
	TBC_Toolbar = TBC_BASE | TAG_INT,	// The toolbar ID to attach to
	TBC_FixedWidth,							// All controls in the toolbar with this tag will be the same width
	TBC_NoSplitAfter,							// Always keep together with the control after this one
	TBC_ForceSplitAfter,						// As it says - probably shouldn't use this and TBC_NoSplitAfter at the same time - who knows what might happen.
	TBC_TextColor,
	TBC_DragNDrop,								// Indicates that this control should do drag'n'drop (if it can)
	TBC_Borderless,							// Draw the control with minimal borders
	TBC_SizeToEnd,								// This control extends right to the end of the row
	TBC_Bold,
	TBC_AcceptDrop,							// This control accepts dropped objects

	TBC_ToolbarPtr = TBC_BASE | TAG_PTR,
	TBC_MenuHandler,
	TBC_ToolbarControl,
	TBC_Menu,

	TBC_Scale = TBC_BASE | TAG_DOUBLE,

	TBC_MenuID = TBC_BASE | TAG_ID,
};

// Toolbar Button Control tags
#define TBAR_BUTTON_ID		"ToolbarButton"

#define TBBC_BASE		TBC_USER + 100

typedef enum ButtonTypes
{
	TBBCT_Normal = 0,
	TBBCT_Toggle,
	TBBCT_TriState,
	TBBCT_MX,
} ToolbarButtonTypes;

enum ToolbarButtonTags ENUM_TAGS
{
	TBBC_ColdDIB = TBBC_BASE | TAG_PTR,	// DIB/ResDIB
	TBBC_ColdDIB_ID,							// resource ID/lookpack string
	TBBC_ColdDIB_Reg,							// Registry node - only required if TBBC_ColdDIB_ID is a resource ID
	TBBC_HotDIB,								// DIB/ResDIB
	TBBC_HotDIB_ID,							// resource ID/lookpack string
	TBBC_HotDIB_Reg,							// Registry node - only required if TBBC_HotDIB_ID is a resource ID
	TBBC_PressedDIB,							// DIB/ResDIB
	TBBC_PressedDIB_ID,						// resource ID/lookpack string
	TBBC_PressedDIB_Reg,						// Registry node - only required if TBBC_PressedDIB_ID is a resource ID

	TBBC_Type = TBBC_BASE | TAG_INT,
	TBBC_MX_ID,
	TBBC_Anim,
};

// Toolbar Drop Down Control tags
#define TBAR_DROPDOWN_ID	"ToolbarPopDown"

#define TBDDC_BASE	TBBC_BASE + 100

enum ToolbarDropDownTags ENUM_TAGS
{
	TBDDC_List = TBDDC_BASE | TAG_PTR,			// A list that contains all the text entries for the drop down menu

	TBDDC_MaxEntries = TBDDC_BASE | TAG_INT,	// The maximum number of entries from the list to put in the menu
	TBDDC_SideButton,									// Only the button on the side should pop down the menu
	TBDDC_PopUp,										// Pop the menu up instead of down
	TBDDC_PopLeft,										// Pop the menu from the left side instead of the right
	TBDDC_PopSide,										// Menu arrow should face sideways
};

// Toolbar Separator Control tags
#define TBAR_SEPARATOR_ID	"ToolbarSeparator"

#define TBSPC_BASE	TBC_USER + 100

typedef enum SeparatorType
{
	TBSPT_Vertical,
	TBSPT_Horizontal,
} ToolbarSeparatorTypes;

typedef enum SeparatorStyle
{
	TBSPS_Etched,
	TBSPS_Flat,
} ToolbarSeparatorStyles;

enum ToolbarSeparatorTags ENUM_TAGS
{
	TBSPC_Type = TBSPC_BASE | TAG_INT,
	TBSPC_Style,
};

// Toolbar Combo Control tags
#define TBAR_COMBO_ID		"_TbCm"

// ##### Hotkey controls/tags #####

//#define CLSID_EventControl_Hotkey	zMAKEID('HKey')

#define HK_BASE		EVCTRL_USER + TAGOFFSET

enum HotKeyTags ENUM_TAGS
{
	HK_HotKey = HK_BASE | TAG_INT,
};


// ##### Menu tags #####

#define MNU_BASE		HK_BASE + TAGOFFSET

typedef enum MenuTypes
{
	MNUT_Check = 0,
	MNUT_MX,
	MNUT_Separator,
	MNUT_SubMenu,
} MenuItemTypes;

enum MenuTags ENUM_TAGS
{
	MNU_Parent = MNU_BASE | TAG_ID,
	MNU_Type = MNU_BASE | TAG_INT,
	MNU_MX_ID,
	MNU_RadialPos,
	MNU_LinearKeep,
	MNU_Multi,
};


// ##### DropObjects tags #####

#define DO_BASE		MNU_BASE + TAGOFFSET

enum DropObjectsTags ENUM_TAGS
{
	DO_OperatorRegID = DO_BASE | TAG_INT | TAG_MULTI,

	DO_File = DO_BASE | TAG_PTR | TAG_MULTI,			// File name
	DO_Operator,
	DO_Parameter,
};


// ##### CustomData tags #####

/* OBSOLETE
#define CDAT_BASE EVCTRL_USER

enum CustomDataEventTags ENUM_TAGS
{
	CDAT_Begin = CDAT_BASE | TAG_MULTI | TAG_INT,
	CDAT_End,

	CDAT_TableVal,
	CDAT_BooleanVal,
	CDAT_NilVal,
	CDAT_FunctionVal,
	CDAT_ObjectVal,

	CDATD_NumericKey = CDAT_BASE | TAG_MULTI | TAG_DOUBLE,
	CDATD_NumericVal,

	CDATS_StringKey = CDAT_BASE | TAG_MULTI | TAG_STRING,
	CDATS_StringVal,
	CDATS_CodeVal,
};
*/

// ##### Event_Edit_SaveSettings and EventID_Operator_Save tags #####

#define SS_BASE		DO_BASE + TAGOFFSET

enum SaveSettingsTags ENUM_TAGS
{
	SS_File = SS_BASE | TAG_PTR,			// File name
};

// ##### EventID_Doc_ tags #####

#define EVDOC_BASE		SS_BASE + TAGOFFSET

enum DocEventTags ENUM_TAGS
{
	EVDOC_Input = EVDOC_BASE | TAG_PTR,
};

// ##### EventID_Console_Output tags #####

#define ECON_BASE EVCTRL_USER

enum EventConsoleTags ENUM_TAGS
{
	ECON_TextColor = ECON_BASE | TAG_INT,
	ECON_BackColor,
	ECON_Bold,
	ECON_Italic,
	ECON_Underline,

	ECON_String = ECON_BASE | TAG_STRING,
};

enum EventConsoleSubID
{
	ECONID_Input   = 10,
	ECONID_Debug   = 20,
	ECONID_Utility = 30,
	ECONID_Log     = 40,
	ECONID_Script  = 50,
	ECONID_Error   = 60,
};

// ##### Invalidate_Extent tags #####

#define IEX_BASE		EVCTRL_USER + TAGOFFSET

enum InvalidateExtentTags ENUM_TAGS
{
	IEX_Input = IEX_BASE | TAG_PTR,

	IEXD_Time = IEX_BASE | TAG_DOUBLE,
};

// ##### Event_Operator_ tags #####

#define EVOP_BASE EVCTRL_USER + TAGOFFSET

enum EventOperatorTags ENUM_TAGS
{
	EVOP_Deleted = EVOP_BASE | TAG_INT,

	EVOP_TileText = EVOP_BASE | TAG_PTR,
	EVOP_Operator,

	EVOP_OldName = EVOP_BASE | TAG_STRING,
	EVOP_NewName,
};

// ##### EventID_LUT_ tags #####

#define EVLUT_BASE EVCTRL_USER + TAGOFFSET

enum EventLUTTags ENUM_TAGS
{
	EVLUT_Entries = EVOP_BASE | TAG_INT,
	EVLUT_OneToOne,
	EVLUT_Offset,
	EVLUT_Type,
	EVLUT_Flags, // See LookUpTable.h

	EVLUT_Value = EVOP_BASE | TAG_DOUBLE,
	EVLUT_Minimum,
	EVLUT_Maximum,
	EVLUT_MinimumValue,
	EVLUT_MaximumValue,
	EVLUT_StartSlope,
	EVLUT_EndSlope,
	EVLUT_StartIn,
	EVLUT_EndIn,
	EVLUT_Time,

	EVLUT_Result = EVOP_BASE | TAG_PTR,
	EVLUT_Context,
	EVLUT_Output,

};

// ##### Floating flow view tags #####

#define EVFFV_BASE EVOP_BASE + (TAGOFFSET + TAGOFFSET)

enum EventFFVTags ENUM_TAGS
{
	EVFFV_ParentFVI = EVFFV_BASE | TAG_PTR,
	EVFFV_FVI,
};

// ##### Event_SetGUIMode modes #####

enum EventGUIModes
{
	GUIM_Basic = 0,
	GUIM_Advanced,
};

// ##### EventID_Param_FreeTempData modes #####

// The modes indicate what temporary data should preferrably be kept. ie. free
// any temporary data except the data that is important for the mode supplied

enum EventFreeTempDataModes
{
	FTD_FreeAll = 0,
	FTD_Playback_Priority,
	FTD_Interactive_Priority,
	FTD_Rendering_Priority,
};

// ##### EventID_Preview_TrimChanged tags & modes #####

#define EVPTC_BASE EVFFV_BASE + TAGOFFSET

enum EventPTCTags ENUM_TAGS
{
	EVPTC_Preview = EVPTC_BASE | TAG_PTR,
};

enum EventPTC_SubID
{
	EVPTC_TrimChanged,
	EVPTC_GetTrim,
};


// ##### EventID_Preview_DisplayImage tags #####

#define EVPDI_BASE EVCTRL_USER + TAGOFFSET

enum EventPDITags ENUM_TAGS
{
	EVPDI_ForceWait = EVPDI_BASE | TAG_INT,

	EVPDI_Time = EVPDI_BASE | TAG_DOUBLE,

	EVPDI_ValidExtent = EVPDI_BASE | TAG_PTR,
};

// ##### EventID_DeviceChange tags #####

#define EVDC_BASE EVCTRL_USER + TAGOFFSET

enum EventDCTags ENUM_TAGS
{
	EVDC_Data = EVDC_BASE | TAG_PTR,
};


struct EventData
{
	EventData(const FuID &id) : eventid(id) { }

	const FuID eventid;
	uint_ptr subid;
	TagList *tags;
};

bool HandleOnGUI(Object *obj, uint32 eventid, uint_ptr subid, TagList &tags);


// EventID_Operator_GetColor/EventID_Operator_SetColor tags

#define OGC_BASE DO_BASE + TAGOFFSET

enum OperatorGetColorTags ENUM_TAGS
{
	OGC_ShowDlg = OGC_BASE | TAG_INT,

	OGC_TileColor = OGC_BASE | TAG_PTR, // COLORREF *
	OGC_TextColor,
	OGC_FillColor,
};


// EventID_Operator_Key_#? tags

#define OKEY_BASE DO_BASE + TAGOFFSET

enum OperatorKeyTags ENUM_TAGS
{
	OKEY_Index = OKEY_BASE | TAG_INT,
	OKEY_Direction,
	OKEY_Dokey_Pig_In_A_Pokey,

	OKEY_Time = OKEY_BASE | TAG_DOUBLE,

	OKEYP_Output = OKEY_BASE | TAG_PTR,
};


#endif // _STANDARDEVENTS_H_
