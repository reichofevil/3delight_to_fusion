#ifndef _STANDARDCONTROLS_H_
#define _STANDARDCONTROLS_H_

#ifndef DFMANAGER

//#include "InputControl.h"
//#include "PreviewControl.h"

#include "Input.h"				// its needed for the definition of INPUT_BASE

#include "Number.h"
#include "Point.h"
#include "Image.h"
#include "Text.h"
#include "DFIDParam.h"

#define ICTAG						(INPUT_BASE + TAGOFFSET)
#define PCTAG						(INPUT_BASE + TAGOFFSET + 0x8000)
#define ICTAG_USER				(ICTAG + TAGOFFSET)
#define PCTAG_USER				(PCTAG + TAGOFFSET)

#define INID_Mask             "EffectMask"
#define INID_Clip             "Clip"
#define INID_Gamma            "Gamma"
#define INID_Strength         "Strength"
#define INID_SoundFile        "SoundFile"
#define INID_SoundOffset      "SoundOffset"
#define INID_MissingFrames    "MissingFrames"
#define INID_AspectMode       "AspectMode"
#define INID_ProcessMode      "ProcessMode"
#define INID_PixelAspect      "PixelAspect"
#define INID_Polyline         "Polyline"
#define INID_Polyline2        "Polyline2"
#define INID_FileFormat       "FileFormat"
#define INID_TimeCurve        "TimeCurve" // IDTODO: we shouldn't really need some of these...
#define INID_Center           "Center"
#define INID_Blend            "Blend"
#define INID_LeftAudio        "LeftAudio"
#define INID_RightAudio       "RightAudio"
#define INID_BorderWidth      "BorderWidth"
#define INID_Size					"Size"
#define INID_Width				"Width"
#define INID_Height				"Height"

#define OUTID_Preview         "Preview"



#define ICPAGE_GLOBAL	-1
#define ICPAGE_COMMON	-2


// Input Controls

enum InputControl_Attrs ENUM_TAGS
{
	IC_Value = ICTAG | TAG_INT,		// Pass a number, ptr, etc to set primary Input to
	IC_Default,								// As above, but does not affect connected objects
	IC_Priority,							// Default 0
	IC_Steps,
	IC_DisplayedPrecision,
	IC_ControlPage,
	IC_TimeType,
	IC_StepRestrict,
	IC_Visible,
	IC_FixedWidth,
	IC_ForceWrap,
	IC_Disabled,
	IC_TabStop,

	IC_AddData = ICTAG | TAG_INT | TAG_MULTI,	// ** NOTE ** user data is not saved

	ICD_Value = ICTAG | TAG_DOUBLE,	// Pass a double to set primary Input to
	ICD_Default,							// As above, but does not affect connected objects
	ICD_MinAllowed,
	ICD_MaxAllowed,
	ICD_MinVal,
	ICD_MaxVal,
	ICD_Center,
	ICD_Width,

	ICP_Owner = ICTAG | TAG_PTR,

	ICP_AddDataPtr = ICTAG | TAG_PTR | TAG_MULTI,	// ** NOTE ** user data ptr is not saved (naturally)

	ICS_Name = ICTAG | TAG_STRING,
	ICS_LowName = ICS_Name, // These two are for Sliders' tag compat, as in uses this range :-/
	ICS_HighName,
	ICS_ControlPage,

	ICS_AddString = ICTAG | TAG_STRING | TAG_MULTI,

	ICID_AddAfterID = ICTAG | TAG_ID,	// place my control just after this input's control
};

// Slider Control
#define SLIDERCONTROL_ID CLSID_InputControl_Slider

enum SliderGradientPreset
{
	// Same as GradientPreset in Gradient.h
	SGP_None,
	SGP_SolidBlack,
	SGP_SolidWhite,
	SGP_BlackToWhite,
	SGP_WhiteToBlack,
	SGP_BlackToTransparant,
	SGP_WhiteToTransparant,

	// Control specific presets
	SGP_DarkToLight = 100,
	SGP_LightToDark,
	SGP_GreyToColor,
	SGP_ColorToGrey,
};

enum SliderControl_Attrs ENUM_TAGS
{
	IC_TickFreq = ICTAG_USER | TAG_INT,

	SLC_Gradient_Preset,
	SLC_LowGradient_Preset = SLC_Gradient_Preset,
	SLC_HighGradient_Preset,

	SLC_Gradient_ColorSpace,
	SLC_LowGradient_ColorSpace = SLC_Gradient_ColorSpace,
	SLC_HighGradient_ColorSpace,

	SLCS_LowName = ICTAG | TAG_STRING,
	SLCS_HighName, // don't add more tags here.  These conflict with IC_
	//SLCS_ = ICTAG_USER | TAG_STRING,  // use this instead

	SLCO_Gradient = ICTAG_USER | TAG_OBJECT,
	SLCO_LowGradient = SLCO_Gradient,
	SLCO_HighGradient,
};

#define AUTO_TICKS 0


// Screw Control
#define SCREWCONTROL_ID CLSID_InputControl_Screw

enum ScrewControl_Attrs ENUM_TAGS
{
	SCRC_TickFreq = ICTAG_USER | TAG_INT,
};

#define SCRC_AUTO_TICKS 0


// Range Control

#define RANGECONTROL_ID CLSID_InputControl_Range

enum RangeControl_Attrs ENUM_TAGS
{
	RNGC_TickFreq = ICTAG_USER | TAG_INT,
	RNGC_DisplayLength,
	RNGC_RoundEnds,
	RNGC_FixedWidth,
	RNGC_Loop,

	RNGC_DisplayFlags,

	RNGCD_LowOuterLength = ICTAG_USER | TAG_DOUBLE,
	RNGCD_HighOuterLength,
	RNGCD_LoopLimit,
	RNGCD_DisplayOffset,

	RNGCS_LowName = ICTAG_USER | TAG_STRING,
	RNGCS_MidName,
	RNGCS_HighName,
};

#define RNGC_AUTO_TICKS 0

enum RangeControl_ControlIDs
{
	RANGECONTROL_LOW = 0,
	RANGECONTROL_HIGH,
};

// Magic range hints
enum Range_Hints
{
	RCHint_DragMin,		// Generated if the knob is dragged
	RCHint_DragMax,
	RCHint_DragBoth,

	RCHint_SetMin,			// Generated if the edits have values entered
	RCHint_SetMax,
	RCHint_SetBoth,
};

enum Range_SpecialAttrs ENUM_TAGS
{
	RC_DragHint = TAG_INT + 1000,		// The tag to look for to find the hints
};

// Flag options for RNGC_DisplayFlags
#define RCDF_Outer				(1 << 1)
#define RCDF_Length				(1 << 2)


// Levels Control

#define LEVELSCONTROL_ID CLSID_InputControl_Levels

enum LevelsControl_Attrs ENUM_TAGS
{
	LVLC_ = ICTAG_USER | TAG_INT,

	LVLCS_LowName = ICTAG_USER | TAG_STRING,
	LVLCS_MidName,
	LVLCS_HighName,
};

enum LevelsControl_ControlIDs
{
	LEVELSCONTROL_LOW = 0,
	LEVELSCONTROL_MID,
	LEVELSCONTROL_HIGH,
};

// Magic range hints
enum Levels_Hints
{
	LCHint_DragMin,		// Generated if the knob is dragged
	LCHint_DragMax,
	LCHint_DragBoth,

	LCHint_SetMin,			// Generated if the edits have values entered
	LCHint_SetMax,
	LCHint_SetBoth,
};

enum Levels_SpecialAttrs ENUM_TAGS
{
	LC_DragHint = TAG_INT + 1000,		// The tag to look for to find the hints
};

// Levels Output

#define LEVELSOUTPUTCONTROL_ID CLSID_InputControl_LevelsOutput

enum LevelsOutputControl_Attrs ENUM_TAGS
{
	LVLOC_GradientColor = ICTAG_USER | TAG_INT,

	LVLOCS_LowName = ICTAG_USER | TAG_STRING,
	LVLOCS_HighName,
};

enum LevelsOutputControl_ControlIDs
{
	LEVELSOUTPUTCONTROL_LOW = 0,
	LEVELSOUTPUTCONTROL_HIGH,
};

// ColorWheel Control

#define COLORWHEELCONTROL_ID CLSID_InputControl_ColorWheel

enum ColorWheelControl_Attrs ENUM_TAGS
{
	CWCC_ = ICTAG_USER | TAG_INT,

	CWCCS_LowName = ICTAG_USER | TAG_STRING,
	CWCCS_MidName,
	CWCCS_HighName,
};

enum ColorWheelControl_ControlIDs
{
	COLORWHEELCONTROL_HUE = 0,
	COLORWHEELCONTROL_SATURATION,
	COLORWHEELCONTROL_TINTANGLE,
	COLORWHEELCONTROL_TINTLENGTH,

	COLORWHEELCONTROL_RANGE,
};

// Color Suppression

#define COLORSUPPRESSIONCONTROL_ID CLSID_InputControl_ColorSuppression

enum ColorSuppressionControl_Attrs ENUM_TAGS
{
	CSCC_ = ICTAG_USER | TAG_INT,

	CSCCS_LowName = ICTAG_USER | TAG_STRING,
	CSCCS_MidName,
	CSCCS_HighName,

	CSCCD_AngleMaster = ICTAG_USER | TAG_DOUBLE,
	CSCCD_AngleLow,
	CSCCD_AngleMid,
	CSCCD_AngleHigh,
};

enum ColorSuppressionControl_ControlIDs
{
	COLORSUPPRESSIONCONTROL_RED = 0,
	COLORSUPPRESSIONCONTROL_YELLOW,
	COLORSUPPRESSIONCONTROL_GREEN,
	COLORSUPPRESSIONCONTROL_CYAN,
	COLORSUPPRESSIONCONTROL_BLUE,
	COLORSUPPRESSIONCONTROL_MAGENTA,

	COLORSUPPRESSIONCONTROL_RANGE,
};

// Histogram Control

#define HISTOGRAMCONTROL_ID CLSID_InputControl_Histogram

enum HistogramControl_Attrs ENUM_TAGS
{
	HISC_ImageInput = ICTAG_USER | TAG_INT,
	HISC_Type,

	HISC_ShowInput,
	HISC_ShowReference,
	HISC_ShowCurves,
	HISC_ShowOutput,

	HISC_ShowButtons,
	HISC_ShowGuides,

	HISCD_GuideLow = ICTAG_USER | TAG_DOUBLE,
	HISCD_GuideGamma,
	HISCD_GuideHigh,
};

enum HistogramControl_ControlIDs
{
	HISTOGRAMCONTROL_MAIN = 0,
	HISTOGRAMCONTROL_REFERENCE,
	HISTOGRAMCONTROL_OUTPUT,
	HISTOGRAMCONTROL_CURVE,
};

// ColorRanges Control

#define COLORRANGESCONTROL_ID CLSID_InputControl_ColorRanges

enum ColorRangesControl_Attrs ENUM_TAGS
{
	CRCC_ShowMidtones = ICTAG_USER | TAG_INT,	// Default TRUE

	CRCCP_InputRangeSelection = ICTAG_USER | TAG_PTR,

	CRCCS_LowName = ICTAG_USER | TAG_STRING,
	CRCCS_MidName,
	CRCCS_HighName,
};

// Offset Control
#define OFFSETCONTROL_ID CLSID_InputControl_Offset

enum OffsetControl_Attrs ENUM_TAGS
{
	OFCD_ValueX = ICTAG_USER | TAG_DOUBLE,
	OFCD_ValueY,

	OFCD_DisplayXScale,
	OFCD_DisplayYScale,

	OFCS_XName = ICTAG_USER | TAG_STRING,
	OFCS_YName,
};


// Font Control
#define FONTCONTROL_ID CLSID_InputControl_Font

#define FontFlag_StrikeOut		(1 << 0)
#define FontFlag_Underline		(1 << 1)
#define FontFlag_Bold			(1 << 2)
#define FontFlag_Italic			(1 << 3)

enum FontControl_Attrs ENUM_TAGS
{
	FC_SetSize = ICTAG_USER | TAG_INT,
	FC_SetFlags,
};

// Color Control
#define COLORCONTROL_ID CLSID_InputControl_Color

enum ColorControlColorSpace
{
	CLRS_HSV = 0,
	CLRS_HLS,
	CLRS_YUV,
};

enum ColorControl_Attrs ENUM_TAGS
{
	CLRC_ColorSpace = ICTAG_USER | TAG_INT,
	CLRC_ShowWheel,
};

#define CLRIC_PointControlID		65536


// Gradient Control
#define GRADIENTCONTROL_ID CLSID_InputControl_Gradient

enum GradientControl_Attrs ENUM_TAGS
{
	GRDC_ShowWheel = ICTAG_USER | TAG_INT,
	GRDC_ColorSpace,
};

// Clip Control
#define CLIPCONTROL_ID CLSID_InputControl_Clip

enum ClipControl_Attrs ENUM_TAGS
{
	IC_IsSaver = ICTAG_USER | TAG_INT,         // Longs/ptrs/bools
	IC_ClipLength,

	ICP_Clip = ICTAG_USER | TAG_PTR,

	ICS_ClipName = ICTAG_USER | TAG_STRING,
	
	ICID_DefaultFormat = ICTAG_USER | TAG_ID,    // Format ID
};


// Checkbox Control
#define CHECKBOXCONTROL_ID CLSID_InputControl_Checkbox

enum CheckBoxControl_Attrs ENUM_TAGS
{
	CBC_TriState = ICTAG_USER | TAG_INT,
		// Use IC_Value to set state: pass 0 off, 1 on, -1 tristated
};

// Button Control
#define BUTTONCONTROL_ID CLSID_InputControl_Button

enum ButtonJustification
{
	BCA_Left = 0,
	BCA_CenterdLeft,
	BCA_Center,
	BCA_CenterdRight,
	BCA_Right,
};

enum ButtonControl_Attrs ENUM_TAGS
{
	BTNC_Align = ICTAG_USER | TAG_INT,		// See enums above

	BTNCP_ColdDIB = ICTAG_USER | TAG_PTR,	// DIB/ResDIB
	BTNCP_ColdDIB_ID,								// resource ID/lookpack string
	BTNCP_ColdDIB_Reg,								// Registry node - only required if CTP_ColdDIB_ID is a resource ID
};

// MultiButton Control

#define MULTIBUTTONCONTROL_ID			CLSID_InputControl_MultiButton
#define MULTIBUTTONIDCONTROL_ID		CLSID_InputControl_MultiButtonID

enum MultiButtonControlTypes
{
	MBTNCT_Normal = 0,
	MBTNCT_Toggle,
	MBTNCT_TriState,
	MBTNCT_MX,
};

enum MultiButtonLabelAlign
{
	MBTNLA_Center,
	MBTNLA_Left,
	MBTNLA_Right
};

// API_REV: Make strings tags TAG_STRING
enum MultiButtonControl_Attrs ENUM_TAGS
{
	MBTNC_Type = ICTAG_USER | TAG_INT,

	MBTNC_Align,

	MBTNC_ShowBasicButton,			// Default TRUE
	MBTNC_ShowIcon,					// Default TRUE
	MBTNC_ShowLabel,					// Default FALSE (unless no DIB is provided)
	MBTNC_ShowToolTip,				// Default TRUE
	MBTNC_ShowName,					// Default TRUE, unless the name is ""

	MBTNC_Tab,							// Default FALSE, gives the buttons a "tab-like" look

	MBTNC_ButtonWidth,				// Default 26 (becomes the "minimum" width if MBTNC_ButtonStretchToFit is TRUE
	MBTNC_Width = MBTNC_ButtonWidth,
	MBTNC_ButtonHeight,				// Default 26
	MBTNC_Height = MBTNC_ButtonHeight,
	MBTNC_StretchToFit,				// Default FALSE

	MBTNC_Reserved1,					// Was MBTNC_Select, which still works
	MBTNC_Reserved2,

	MBTNC_NoIconScaling,				// Default FALSE

	MBTNC_AlphaSort,					// MultiButtonIDControls only: Default FALSE

	MBTNC_LabelAlign,					// Default MBTNLA_Center

	MBTNC_AddData = IC_AddData,	// MultiButtonIDControls only: optional, follows each button string
											// ** NOTE ** user data is not saved

	MBTNC_Select = ICTAG_USER | TAG_INT | TAG_MULTI,	// If later on you want to change a certain button, you can select it with this tag, followed by its index
	MBTNC_Disabled,
	MBTNC_SetDisabled,				// Must have MBTNC_Disabled for each button to store the disabled state

	MBTNCD_ColdFromHot_H = ICTAG_USER | TAG_DOUBLE | TAG_MULTI,	// Defines HLS with which the Cold DIB can be derived from the hot one (Default 0.0, 1.0, 0.0)
	MBTNCD_ColdFromHot_L,	
	MBTNCD_ColdFromHot_S,	
	MBTNCD_ColdFromHot_Opacity,	// Defines opacity with which the Cold DIB can be derived from the hot one (Default 0.75)

	MBTNCD_ButtonWidth,				// 1.0
	MBTNCD_ButtonHeight,				// 1.0

	MBTNC_AddButton = ICS_AddString,

	MBTNCS_ToolTip = ICTAG_USER | TAG_STRING | TAG_MULTI,					// If not provided the name will be shown
	MBTNCS_SetName,					// MultiButtonIDControl must have MBTNCID_AddID for each button
	MBTNCS_SetToolTip,				// Must have MBTNCS_ToolTip for each button to store the set tooltip

	MBTNCID_AddID = INPID_AddID,	// MultiButtonIDControls only: optional, follows each button string
	MBTNCID_SelectID,					// MultiButtonIDControls only: Select a button by ID - must have MBTNCID_AddID for each button

	MBTNCP_ColdDIB = ICTAG_USER | TAG_PTR | TAG_MULTI,					// DIB/ResDIB
	MBTNCP_ColdDIB_ID,				// resource ID/lookpack string
	MBTNCP_ColdDIB_Reg,				// Registry node - only required if MBTNCP_ColdDIB_ID is a resource ID

	MBTNCP_HotDIB,						// DIB/ResDIB
	MBTNCP_HotDIB_ID,					// resource ID/lookpack string
	MBTNCP_HotDIB_Reg,				// Registry node - only required if MBTNCP_HotDIB_ID is a resource ID

	MBTNCP_GlowDIB,					// DIB/ResDIB
	MBTNCP_GlowDIB_ID,				// resource ID/lookpack string
	MBTNCP_GlowDIB_Reg,				// Registry node - only required if MBTNCP_GlowDIB_ID is a resource ID

	MBTNCP_ModifyDIB,					// DIB/ResDIB
	MBTNCP_ModifyDIB_ID,				// resource ID/lookpack string
	MBTNCP_ModifyDIB_Reg,			// Registry node - only required if MBTNCP_GlowDIB_ID is a resource ID

	MBTNCP_AddDataPtr = ICP_AddDataPtr,  // MultiButtonIDControls only: optional, follows each button string
													 // ** NOTE ** user data ptr is not saved (naturally)
};


// Combo Control
#define COMBOCONTROL_ID CLSID_InputControl_Combo

#define CCLP_Horizontal			0
#define CCLP_Vertical			1

enum ComboControl_Attrs ENUM_TAGS
{
	CC_LabelPosition = ICTAG_USER | TAG_INT,
	CC_AlphaSort,									// ComboIDControls only: Default FALSE

	CC_AddData = IC_AddData,					// ComboIDControls only: optional user data, returned with IDP_UserData in the FuIDParam Attrs
														// ** NOTE ** user data is not saved
	CCS_AddString = ICS_AddString,

	CCID_AddID = INPID_AddID,					// ComboIDControls only: optional, follows each entry string

	CCP_AddDataPtr = ICP_AddDataPtr,			// ComboIDControls only: optional user data, returned with IDP_UserData in the FuIDParam Attrs
														// ** NOTE ** user data ptr is not saved (naturally)
};

// ComboID Control, as above but for CLSID_DataType_FuID inputs
#define COMBOIDCONTROL_ID CLSID_InputControl_ComboID

enum DFIDParam_SpecialAttrs ENUM_TAGS
{
	IDP_UserData = TAG_INT + 1000,										// The tag to look for to find the user data
	IDP_UserDataPtr = TAG_PTR + 1000,										// The tag to look for to find the user data
};

// Image Control
#define IMAGECONTROL_ID CLSID_InputControl_Image

enum ImageControl_Attrs ENUM_TAGS
{
	IMGCS_ToolName = ICTAG_USER | TAG_STRING,
	IMGCD_BoxWidth = ICTAG_USER | TAG_DOUBLE,		// (0.64) percentage of line occupied by the drop box
};

// Pattern Control

#define PATTERNCONTROL_ID CLSID_InputControl_Pattern

enum PatternControl_Attrs ENUM_TAGS
{
	PATC_Type = ICTAG_USER | TAG_INT,
	PATC_NormalisePattern,
	PATC_NormaliseMatch,

	PATCD_Diff = ICTAG_USER | TAG_DOUBLE,
};

enum PatternControl_ControlIDs
{
	PATTERNCONTROL_CHANNEL = 0,
	PATTERNCONTROL_SHOW,
	PATTERNCONTROL_MAIN,
	PATTERNCONTROL_MATCH,
};


// File Control
#define FILECONTROL_ID CLSID_InputControl_File

enum FileControl_Attrs ENUM_TAGS
{
	FC_IsSaver = ICTAG_USER | TAG_INT,
	FC_PathBrowse, // Be a path (dir) browser, rather than a file browser.
	FC_ClipBrowse, // Be a clip browser, rather that a generic file browser.

	FCS_FilterString = ICTAG_USER | TAG_STRING,
};

// Fontfile Control
#define FONTFILECONTROL_ID CLSID_InputControl_FontFile

enum FontfileControl_Attrs ENUM_TAGS
{
	FFC_IsSaver = ICTAG_USER | TAG_INT,
	FFC_PathBrowse, // Be a path (dir) browser, rather than a file browser.

	FFCS_FilterString = ICTAG_USER | TAG_STRING,
};

enum FontfileControl_ControlIDs
{
	FONTFILECONTROL_FAMILYNAME = 0,
	FONTFILECONTROL_STYLENAME,
	FONTFILECONTROL_UNDERLINE,
	FONTFILECONTROL_STRIKEOUT,
};

// Custom Filter Control
#define CUSTOMFILTERCONTROL_ID CLSID_InputControl_CustomFilter

enum CustomFilterControl_Attrs ENUM_TAGS
{
	CFC_FixedSize = ICTAG_USER | TAG_INT,	// Default FALSE
};

// Custom Edit/Text Control
#define TEXTEDITCONTROL_ID CLSID_InputControl_TextEdit

enum TextEditControl_Attrs ENUM_TAGS
{
	TEC_Lines = ICTAG_USER | TAG_INT,
	TEC_Wrap,
	TEC_ReadOnly,
	TEC_FontSize,
	TEC_DeferSetInputs,	// Doesn't set inputs every keystroke, only when focus is lost
	TEC_CharLimit,			// 0 = no limit

	TECS_FontName = ICTAG_USER | TAG_STRING,
};

// List Control
#define LISTCONTROL_ID		CLSID_InputControl_List
#define LISTIDCONTROL_ID	CLSID_InputControl_ListID

enum ListControl_Attrs ENUM_TAGS
{
	LC_Rows = ICTAG_USER | TAG_INT,
	LC_AlphaSort,								// ListIDControls only: Default FALSE

	LC_AddData = IC_AddData,				// ListIDControls only: optional user data, returned with IDP_UserData in the FuIDParam Attrs
													// ** NOTE ** user data is not saved
	LCS_AddString = ICS_AddString,

	LCID_AddID = INPID_AddID,					// ListIDControls only: optional, follows each entry string

	LCP_AddDataPtr = ICP_AddDataPtr,		// ListIDControls only: optional user data, returned with IDP_UserData in the FuIDParam Attrs
													// ** NOTE ** user data ptr is not saved (naturally)
};

// CheckList Control
#define CHECKLISTCONTROL_ID CLSID_InputControl_CheckList

enum CheckListControl_Attrs ENUM_TAGS
{
	CLC_Rows = ICTAG_USER | TAG_INT,

	CLC_Comment,
};

enum CheckListControl_ControlIDs
{
	CHECKLISTCONTROL_SELECTION = 0,
	CHECKLISTCONTROL_ADD,
	CHECKLISTCONTROL_REMOVE,
	CHECKLISTCONTROL_ENABLED_BASE = 1024,
	CHECKLISTCONTROL_NAME_BASE = 2048,
};

#define CHECKLISTCONTROL_MAX (CHECKLISTCONTROL_NAME_BASE - CHECKLISTCONTROL_ENABLED_BASE)	// 1024

// Library Control
#define LIBRARYCONTROL_ID CLSID_InputControl_Library

enum LibraryControl_Attrs ENUM_TAGS
{
	LIBC_Rows = ICTAG_USER | TAG_INT,
	LIBC_Cols,
	LIBC_TextPlusControls,

	LIBCS_LibraryName = ICTAG_USER | TAG_STRING,
	LIBCS_LibraryFileName,
	LIBC_Comment,

	LIBCID_ItemID = ICTAG_USER | TAG_ID,
};

enum LibraryControl_ControlIDs
{
	LIBRARYCONTROL_STAMP = 0,
	LIBRARYCONTROL_ADD,
	LIBRARYCONTROL_RETRIEVE,
	LIBRARYCONTROL_RETRIEVESPECIAL,
	LIBRARYCONTROL_REMOVE,
};

// Clip List Control
#define CLIPLISTCONTROL_ID CLSID_InputControl_ClipList

enum ClipListControl_Attrs ENUM_TAGS
{
	CLCP_List = ICTAG_USER | TAG_PTR,
};

// Spline Control
#define SPLINECONTROL_ID CLSID_InputControl_Spline

enum SplineControlColorSpace
{
	SPCCS_RGB = 0,
	SPCCS_YUV,
	SPCCS_HLS,
	SPCCS_YIQ,
	SPCCS_CMY,

	SPCCS_RGB_R = 100,
	SPCCS_RGB_G,
	SPCCS_RGB_B,
	SPCCS_YUV_Y,
	SPCCS_YIQ_Y = SPCCS_YUV_Y,
	SPCCS_YUV_U,
	SPCCS_YUV_V,
	SPCCS_YIQ_I,
	SPCCS_YIQ_Q,
	SPCCS_HLS_H,
	SPCCS_HSV_H = SPCCS_HLS_H,
	SPCCS_HLS_L,
	SPCCS_HLS_S,
	SPCCS_HSV_S,
	SPCCS_HSV_V,
	SPCCS_CMY_C,
	SPCCS_CMY_M,
	SPCCS_CMY_Y,
};

enum SplineControl_Attrs ENUM_TAGS
{
	// TAG_INT
	SPC_AddSpline = ICTAG_USER | TAG_INT,
	SPC_SplineColor,

	SPC_Show,
	SPC_ColorSpace,
	SPC_Baked_Beans,
	SPC_Spaghetti,

	SPC_Channel,

	SPC_HorizontalGridLines,
	SPC_VerticalGridLines,

	SPC_Margin,
	SPC_BarSize,

	SPC_ShowInOut,
	SPC_ShowPick,

	SPC_InColorSpace,
	SPC_OutColorSpace,

	// TAG_DOUBLE
	SPCD_Brightness = ICTAG_USER | TAG_DOUBLE,
	SPCD_Contrast,
	SPCD_Height,

	SPCD_HScaleDefaultStart,
	SPCD_HScaleDefaultEnd,
	SPCD_VScaleDefaultStart,
	SPCD_VScaleDefaultEnd,

	SPCD_BgResColor = ICTAG_USER | TAG_PTR,
};

#define LABELCONTROL_ID CLSID_InputControl_Label

enum LabelControl_Attrs ENUM_TAGS
{
	LBLC_Align = ICTAG_USER | TAG_INT,
	LBLC_DropDownButton,						// default FALSE - specifies that this label should have an arrow button
	LBLC_NumInputs,							// Specifies how many following inputs to show/hide with drop-down button. 0 == all (use with LBLC_InputGroupID)
	LBLC_NestLevel,							// Specifies the nesting level of the following inputs

	LBLCP_DIB = ICTAG_USER | TAG_PTR,		// DIB/ResDIB
	LBLCP_DIB_ID,								// resource ID/lookpack string
	LBLCP_DIB_Reg,								// Registry node - only required if MBTNCP_ColdDIB_ID is a resource ID

	LBLCP_Input = ICTAG_USER | TAG_PTR | TAG_MULTI,	// Shows/hide these specific inputs too

	LBLCID_InputGroup = ICTAG_USER | TAG_ID,			// Shows/hide only inputs with this Group ID
};

// Preview Controls

enum PreviewControl_Attrs ENUM_TAGS
{
	PC_GrabPriority = PCTAG | TAG_INT,
	PC_Visible,
	PC_FastSampleRate,                    // default FALSE - allows a tablet to send much faster ControlMove() messages
	PC_FollowSelected,
	PC_HideWhileDragging,
	PC_CoordSpace,
	PC_WantsMouseMoves,                   // default FALSE - Control wants mouse moves even if the button is up
	PC_Disabled,

	PCD_OffsetX = PCTAG | TAG_DOUBLE,
	PCD_OffsetY,
	PCD_OffsetZ,
	PCD_OffsetX_2DN,
	PCD_OffsetY_2DN,
	PCD_OffsetZ_2DN,
	PCD_OffsetX_2D,
	PCD_OffsetY_2D,
	PCD_OffsetZ_2D,
	PCD_OffsetX_3D,
	PCD_OffsetY_3D,
	PCD_OffsetZ_3D,
	PCD_HotspotOffsetX,
	PCD_HotspotOffsetY,

	PCS_Name = PCTAG | TAG_STRING,
};

enum PCCoordSpace
{
	PCCS_First = 0,

	PCCS_2D_Normalised,
	PCCS_2D,
	PCCS_3D,

	PCCS_Last
};

// Rectangle Control
#define RECTANGLECONTROL_ID CLSID_PreviewControl_Rectangle
 
enum RectangleControl_Attrs ENUM_TAGS
{
	RC_PenStyle = PCTAG_USER | TAG_INT,
	RC_DoLockAspect,								// default TRUE, if RCD_LockAspect is present
	RC_BlendMode,									// See below - valid if RCP_Image specified

	RCD_SetX = PCTAG_USER | TAG_DOUBLE,
	RCD_SetY,
	RCD_SetAngle,
	RCD_LockAspect,
	RCD_BlendAmount,								// valid if RCP_Image specified

	RCD_DisplayXScale,
	RCD_DisplayYScale,
	RCD_SetSize,
	RCD_SetAxisX,
	RCD_SetAxisY,

	RCP_Center = PCTAG_USER | TAG_PTR,
	RCP_Angle,
	RCP_Image,
	RCP_Size,
	RCP_Axis,
	RCP_FlipX,
	RCP_FlipY,

	RCID_Center = PCTAG_USER | TAG_ID,		// IDs of Inputs
	RCID_Angle,
	RCID_Image,
	RCID_Size,
	RCID_Axis,
	RCID_FlipX,
	RCID_FlipY,
};

enum RectangleControl_BlendMode_Attrs
{
	RCBM_None = 0,
	RCBM_Subtractive,
	RCBM_Additive,
};

// Rectangle Control
#define IMGOVERLAYCONTROL_ID CLSID_PreviewControl_ImgOverlay
 
enum ImgOverlayControl_Attrs ENUM_TAGS
{
	IO_DoLockAspect = PCTAG_USER | TAG_INT, // default TRUE, if IOD_LockAspect is present
	IO_BlendMode,									// See below
	IO_DoMouseDrag,								// enables direct overlay drag (LMB), rotate (Alt+LMB), scale (Ctrl+Alt+LMB)

	IOD_SetX = PCTAG_USER | TAG_DOUBLE,
	IOD_SetY,
	IOD_SetAngle,
	IOD_LockAspect,
	IOD_BlendAmount,

	IOD_DisplayXScale,
	IOD_DisplayYScale,
	IOD_SetSize,
	IOD_SetAxisX,
	IOD_SetAxisY,
	IOD_SetWidth,
	IOD_SetHeight,

	IOP_Center = PCTAG_USER | TAG_PTR,		// these are ptrs to Inputs
	IOP_Angle,
	IOP_Size,
	IOP_Axis,
	IOP_FlipX,
	IOP_FlipY,
	IOP_Width,
	IOP_Height,

	IOID_Center = PCTAG_USER | TAG_ID,		// IDs of Inputs
	IOID_Angle,
	IOID_Size,
	IOID_Axis,
	IOID_FlipX,
	IOID_FlipY,
	IOID_Width,
	IOID_Height,

	IOP_SetImage = PCTAG_USER | TAG_OBJECT,		// this is a ptr to an Image
};

enum ImgOverlayControl_BlendMode_Attrs
{
	IOBM_None = 0,
	IOBM_Subtractive,
	IOBM_Additive,
};

// Tracker Control
#define TRACKERCONTROL_ID CLSID_PreviewControl_Tracker
 
enum TrackerControl_Attrs ENUM_TAGS
{
	TCP_ImageReference = PCTAG_USER | TAG_PTR,
	TCP_ShowName,
	TCP_ShowZoom,
	TCP_ZoomScale,
	TCP_InteractivePointList,
	TCP_Operation,

	TCID_ImageReference = PCTAG_USER | TAG_ID,
	TCID_ShowName,
	TCID_ShowZoom,
	TCID_ZoomScale,
	TCID_Operation,

	TCS_Name = PCTAG_USER | TAG_STRING,
};

enum TrackerControl_ControlIDs
{
	TRACKERCONTROL_CENTER = 0,
	TRACKERCONTROL_WIDTH,
	TRACKERCONTROL_HEIGHT,
	TRACKERCONTROL_SEARCHWIDTH, 
	TRACKERCONTROL_SEARCHHEIGHT,
};

// Positioner Control
#define POSITIONERCONTROL_ID CLSID_PreviewControl_Positioner
 
enum PositionerControl_Attrs ENUM_TAGS
{
	POSC_EnableOffsets = PCTAG_USER | TAG_INT,
	POSC_ShowAllOffsets,

	POSCP_PositionerType = PCTAG_USER | TAG_PTR,
	POSCP_IndexBase,

	POSCP_CornerBase =  PCTAG_USER | TAG_PTR + 16,
	POSCP_CornerAlternativeBase =  PCTAG_USER | TAG_PTR + 16 + 256,

	POSCID_PositionerType = PCTAG_USER | TAG_ID,
	POSCID_IndexBase0,
	POSCID_IndexBase1,
	POSCID_IndexBase2,
	POSCID_IndexBase3,
	POSCID_CornerBase0 = PCTAG_USER | TAG_ID + 16,
	POSCID_CornerBase1,
	POSCID_CornerBase2,
	POSCID_CornerBase3,
	POSCID_CornerAlternativeBase0 =  PCTAG_USER | TAG_ID + 16 + 256,
	POSCID_CornerAlternativeBase1,
	POSCID_CornerAlternativeBase2,
	POSCID_CornerAlternativeBase3,
};

enum PositionerControl_ControlIDs
{
	POSITIONERCONTROL_UNUSED = 0,
	POSITIONERCONTROL_BASE_X = 256,
	POSITIONERCONTROL_BASE_Y = 512,
};

// Point Control
#define POINTCONTROL_ID CLSID_PreviewControl_Point

enum PointControl_Attrs ENUM_TAGS
{
	//= PCTAG_USER | TAG_INT,

	PCD_SetPrevPointX = PCTAG_USER | TAG_DOUBLE,
	PCD_SetPrevPointY,

	PCP_PrevPoint = PCTAG_USER | TAG_PTR,

	PCID_PrevPoint = PCTAG_USER | TAG_ID,
};


// Crop Control
#define CROPCONTROL_ID CLSID_PreviewControl_Crop

enum CropControl_Attrs ENUM_TAGS
{
	CRPC_Normalised = PCTAG_USER | TAG_INT,		// Default = true

	CRPCP_Image = PCTAG_USER | TAG_PTR,

	CRPCID_Image = PCTAG_USER | TAG_ID,		// IDs of Inputs
};

enum CropControl_ControlIDs
{
	CROPCONTROL_LEFT = 0,
	CROPCONTROL_BOTTOM,
	CROPCONTROL_RIGHT,
	CROPCONTROL_TOP,
	CROPCONTROL_WIDTH,
	CROPCONTROL_HEIGHT,
};


// Crosshair Control
#define CROSSHAIRCONTROL_ID CLSID_PreviewControl_Crosshair

enum CrosshairControl_Attrs ENUM_TAGS
{
	CHC_Style = PCTAG_USER | TAG_INT,

	CHCD_FollowX = PCTAG_USER | TAG_DOUBLE,
	CHCD_FollowY,
	CHCD_FollowOrigX,
	CHCD_FollowOrigY,
	CHCD_CurrentX,
	CHCD_CurrentY,
	/* Moved to PC
	CHCD_OffsetX,
	CHCD_OffsetY,
	*/
};

enum CrosshairStyles
{
	CHS_NormalCross = 0,
	CHS_DiagonalCross,
	CHS_Rectangle,
	CHS_Circle,
};

// EllipseControl
#define ELLIPSECONTROL_ID CLSID_PreviewControl_Ellipse	// To override CircleControls
 
enum EllipseControl_Attrs ENUM_TAGS
{
	EC_GiveRadius = PCTAG_USER | TAG_INT,	// true/false, output radius instead of width (default is false)
	EC_SetRotOrder,

	ECD_SetX = PCTAG_USER | TAG_DOUBLE,
	ECD_SetY,
	ECD_SetZ,
	ECD_SetAngle,
	ECD_SetAspect,
	ECD_LockAspect,
	ECD_SetSize,
	ECD_SetSizeX,
	ECD_SetSizeY,
	ECD_SetSizeZ,
	ECD_SetRotX,
	ECD_SetRotY,
	ECD_SetRotZ,
	ECD_SetPivotX,
	ECD_SetPivotY,
	ECD_SetPivotZ,

	/* Move to PC
	ECD_OffsetX,
	ECD_OffsetY,
	ECD_OffsetZ,
	*/

	ECP_Center = PCTAG_USER | TAG_PTR,
	ECP_Size,
	ECP_Aspect,
	ECP_Angle,
	ECP_SizeX,
	ECP_SizeY,
	ECP_SizeZ,
	ECP_RotX,
	ECP_RotY,
	ECP_RotZ,
	ECP_CenterX,
	ECP_CenterY,
	ECP_CenterZ,
	ECP_RotOrder,
	ECP_Pivot,
	ECP_PivotX,
	ECP_PivotY,
	ECP_PivotZ,

	ECID_Center = PCTAG_USER | TAG_ID,
	ECID_Size,
	ECID_Aspect,
	ECID_Angle,
	ECID_SizeX,
	ECID_SizeY,
	ECID_SizeZ,
	ECID_RotX,
	ECID_RotY,
	ECID_RotZ,
	ECID_CenterX,
	ECID_CenterY,
	ECID_CenterZ,
	ECID_RotOrder,
	ECID_Pivot,
	ECID_PivotX,
	ECID_PivotY,
	ECID_PivotZ,
};

// Angle Control
#define ANGLECONTROL_ID CLSID_PreviewControl_Angle

enum AngleControl_Attrs ENUM_TAGS
{
	ACD_Style        = PCTAG_USER | TAG_INT,

	ACD_MinRadius    = PCTAG_USER | TAG_DOUBLE,
	ACD_RadiusScale,
	ACD_FollowX,
	ACD_FollowY,
	ACD_FollowAngle,
	ACD_CurrentAngle,

	ACP_Center = PCTAG_USER | TAG_PTR,
	ACP_Radius,
	ACP_CenterX,
	ACP_CenterY,
	ACP_CenterZ,

	ACID_Center = PCTAG_USER | TAG_ID,
	ACID_Radius,
	ACID_CenterX,
	ACID_CenterY,
	ACID_CenterZ,
};

enum AngleControlStyles
{
	ACD_Handle = 0,
	ACD_Circular,
};

// PolylineControl tags
#define POLYLINECONTROL_ID CLSID_PreviewControl_Polyline

enum PolylineControl_Attrs ENUM_TAGS
{
	PLC_DrawMode = PCTAG_USER | TAG_INT,
	PLC_RecordTime,								// bools (default: FALSE)
	PLC_RecordPressure,							// these three apply to tablets
	PLC_RecordHeight,
	PLC_RecordAngleXY,
	PLC_RecordAngleZ,
	PLC_RecordRotation,
	PLC_CanPublishPoints,
	PLC_CurrentTimeOnly,
	PLC_ShowKeyPoints,
	PLC_ShowHandles,
	PLC_StopRendering,
	PLC_Animated,
	PLC_HandleColor,
	PLC_FollowExternMatchPoint,
	PLC_ShowPolyLine,
	PLC_DrawStyle,
	PLC_OuterPolyLine,
	PLC_NoConversion,
	PLC_ShowPointsOnHover,
	PLC_KeyPointColor,

	PLCD_SetX         = PCTAG_USER | TAG_DOUBLE,
	PLCD_SetY,
	PLCD_SetAngle,
	PLCD_SetSize,
	PLCD_SetAngleX,
	PLCD_SetAngleY,

	PLCP_Center = PCTAG_USER | TAG_PTR,		// Ptrs to Inputs
	PLCP_Angle,
	PLCP_Size,
	PLCP_Displacement,
	PLCP_CurrentTimePoint,
	PLCP_AngleX,
	PLCP_AngleY,
	PLCP_AngleZ,
	PLCP_InPolyLine2,
	PLCP_CenterX,
	PLCP_CenterY,
	PLCP_CenterZ,

	PLCID_Center = PCTAG_USER | TAG_ID,		// FuIDs of Inputs
	PLCID_Angle,
	PLCID_Size,
	PLCID_Displacement,
	PLCID_CurrentTimePoint,
	PLCID_AngleX,
	PLCID_AngleY,
	PLCID_AngleZ,
	PLCID_InPolyLine2,
	PLCID_CenterX,
	PLCID_CenterY,
	PLCID_CenterZ,
};

// XYOffsetControl
#define XYOFFSETCONTROL_ID CLSID_PreviewControl_XYOffset

enum XYOffsetControl_Attrs ENUM_TAGS
{
	XYOCP_AxisInput = PCTAG_USER | TAG_PTR,
};


// TransformControl
#define TRANSFORMCONTROL_ID CLSID_PreviewControl_Transform

enum TransformControl_Attrs ENUM_TAGS
{
	XFC_Mode = PCTAG_USER | TAG_INT,
	XFC_Relative,
	XFC_Style,								// Use CrosshairStyles
	XFC_CanWorldScale,

	/* Moved to PC
	XFCD_OffsetX = PCTAG_USER | TAG_DOUBLE,
	XFCD_OffsetY,
	XFCD_OffsetZ,
	*/

	XFCP_PrevInputX = PCTAG_USER | TAG_PTR,
	XFCP_PrevInputY,
	XFCP_PrevInputZ,

	XFCP_TargetMatrix,					// Pointer to Matrix4

	XFCID_PrevInputX = PCTAG_USER | TAG_ID,
	XFCID_PrevInputY,
	XFCID_PrevInputZ,
};

enum TransformControl_IDs
{
	XFPC_XYPosControlID = 1,	// Point type
	XFPC_XPosControlID,			// Number types
	XFPC_YPosControlID,
	XFPC_ZPosControlID,
	XFPC_XRotControlID,
	XFPC_YRotControlID,
	XFPC_ZRotControlID,
	XFPC_XScaleControlID,
	XFPC_YScaleControlID,
	XFPC_ZScaleControlID,
	XFPC_XYAxisControlID,		// Point type
	XFPC_XAxisControlID,			// Number types
	XFPC_YAxisControlID,
	XFPC_ZAxisControlID,	
	XFPC_RotOrderControlID,

	XFPC_MaxID,
};


// LightControl
#define LIGHTCONTROL_ID CLSID_PreviewControl_Light

#define LTCTAG_USER				(PCTAG_USER + TAGOFFSET)

enum LightControl_Attrs ENUM_TAGS
{
	LTC_Style = LTCTAG_USER | TAG_INT,
};

enum LightControl_IDs
{
	LTC_OuterAngleControlID = XFPC_MaxID,
	LTC_InnerAngleControlID,
};


// CameraControl
#define CAMERACONTROL_ID CLSID_PreviewControl_Camera

#define CMCTAG_USER				(PCTAG_USER + TAGOFFSET)

enum CameraControl_Attrs ENUM_TAGS
{
	CMC_UseDepth = CMCTAG_USER | TAG_INT,
};

enum CameraControl_IDs
{
	CMC_FLength = XFPC_MaxID,
	CMC_ApertureWidth,
	CMC_ApertureHeight,
	CMC_ImagePlaneDepth,
	CMC_EyeSeparation,
	CMC_ConvergenceDistance,
	CMC_ProjectionType,
	CMC_PerspNearClip,
	CMC_PerspFarClip,
	CMC_PlaneOfFocus,
	CMC_StereoMethod,
	CMC_FrustumVis,
	CMC_ViewVectorVis,
	CMC_PerspNearClipVis,
	CMC_PerspNearClipSub,
	CMC_PerspFarClipVis,
	CMC_PerspFarClipSub,
	CMC_PlaneOfFocusVis,
	CMC_PlaneOfFocusSub,
	CMC_ConvergenceDistanceVis,
	CMC_ConvergenceDistanceSub,
	CMC_LensShiftX,
	CMC_LensShiftY
};


// ControlTab tags - used with Operator::AddControlPage()
enum ControlTab_Attrs ENUM_TAGS
{
	CT_NoExtraSpace = TAG_INT,

	CT_Msg,

	CT_AlwaysFullSaturation,
	CT_Visible,
	CT_Priority,				// 0 is default, fusion tab is -1000

	CTP_ColdDIB = TAG_PTR,	// DIB/ResDIB
	CTP_ColdDIB_ID,			// resource ID/lookpack string
	CTP_ColdDIB_Reg,			// Registry node - only required if CTP_ColdDIB_ID is a resource ID
	CTP_HotDIB,					// DIB/ResDIB
	CTP_HotDIB_ID,				// resource ID/lookpack string
	CTP_HotDIB_Reg,			// Registry node - only required if CTP_HotDIB_ID is a resource ID

	CTP_Param,

	CTS_Name = TAG_STRING,
	CTS_FullName,

	CTID_ID = TAG_ID,
};


// MeshControl tags
#define MESHCONTROL_ID CLSID_PreviewControl_Mesh

enum MeshControl_Attrs ENUM_TAGS
{
	MSC_DrawMode   = PCTAG_USER | TAG_INT,
	MSC_ShowKeyPoints,
	MSC_ShowHandles,
	MSC_ShowSubdivisions,
	MSC_ShowGrid,
	MSC_StopRendering,
	MSC_HandleColor,
	MSC_KeyPointColor,
	MSC_Animated,

	MSCP_BR         = PCTAG_USER | TAG_PTR,				// Ptrs to Inputs
	MSCP_XSize,
	MSCP_YSize,
	MSCP_Focal,
	MSCP_XRot,
	MSCP_YRot,
	MSCP_ZRot,
	MSCP_ZMove,
	MSCP_Center,
	MSCP_Angle,
	MSCP_Size,
	MSCP_Subdivision,
	MSCP_MagnetDistance,
	MSCP_MagnetStrength,
	MSCP_MagnetType,
	MSCP_MagnetActive,
	MSCP_AutoSmooth,

	MSCID_BR         = PCTAG_USER | TAG_ID,				// FuIDs of Inputs
	MSCID_XSize,
	MSCID_YSize,
	MSCID_Focal,
	MSCID_XRot,
	MSCID_YRot,
	MSCID_ZRot,
	MSCID_ZMove,
	MSCID_Center,
	MSCID_Angle,
	MSCID_Size,
	MSCID_Subdivision,
	MSCID_MagnetDistance,
	MSCID_MagnetStrength,
	MSCID_MagnetType,
	MSCID_MagnetActive,
	MSCID_AutoSmooth,

	MSCD_SetBottom = PCTAG_USER | TAG_DOUBLE,
	MSCD_SetRight,
	MSCD_SetXSize,
	MSCD_SetYSize,
	MSCD_SetFocal,
	MSCD_SetXRot,
	MSCD_SetYRot,
	MSCD_SetZRot,
	MSCD_SetZMove,
	MSCD_SetX,
	MSCD_SetY,
	MSCD_SetAngle,
	MSCD_SetSize,
	MSCD_SetSubdivision,
	MSCD_SetMagnetDistance,
	MSCD_SetMagnetStrength,
	MSCD_SetMagnetType,
	MSCD_SetMagnetActive,
	MSCD_SetAutoSmooth
};

// AutoRotoControl tags
#define AUTOROTOCONTROL_ID CLSID_PreviewControl_AutoRoto

#endif
#endif
