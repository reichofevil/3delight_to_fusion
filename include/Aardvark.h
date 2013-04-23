#ifndef _AARDVARK_H_
#define _AARDVARK_H_

//#include "TagList.h"
#ifndef USE_NOSCRIPTING

#include "DFScript.h"
#include "LockableObject.h"
#include "List.h"

class FuString;
class FuPath;

FuSYSTEM_API void LockPrefs();
FuSYSTEM_API void UnlockPrefs();
FuSYSTEM_API void ReadLockPrefs();
FuSYSTEM_API void ReadUnlockPrefs();

FuSYSTEM_API bool LoadPrefs(const char *prefname = NULL, const char *masterprefs = NULL);
FuSYSTEM_API void SavePrefs();								// preserved for possible compat.
FuSYSTEM_API void SavePrefs(const char *prefname);
FuSYSTEM_API const ScriptVal &GetPrefTable();
FuSYSTEM_API const ScriptVal &GetDefPrefTable();

// Generic pref functions
FuSYSTEM_API const ScriptVal &GetPref(const FuID &pref);
FuSYSTEM_API void SetPref(const FuID &pref, const ScriptVal &value);

// Type-conversion pref functions
FuSYSTEM_API uint32 GetPrefLong(const FuID &pref);
FuSYSTEM_API bool GetPrefBool(const FuID &pref);
FuSYSTEM_API float64 GetPrefDouble(const FuID &pref);
FuSYSTEM_API const char *GetPrefString(const FuID &pref);
FuSYSTEM_API const char *GetPrefFilePath(const FuID &pref);
FuSYSTEM_API const char *GetPrefDirPath(const FuID &pref);
//FuSYSTEM_API TagList *GetPrefTagList(const FuID &pref);

FuSYSTEM_API void SetPrefLong(const FuID &pref, uint32 value);
FuSYSTEM_API void SetPrefBool(const FuID &pref, bool value);
FuSYSTEM_API void SetPrefDouble(const FuID &pref, float64 value);
FuSYSTEM_API void SetPrefString(const FuID &pref, const char *value);
FuSYSTEM_API void SetPrefFilePath(const FuID &pref, const char *value);
FuSYSTEM_API void SetPrefDirPath(const FuID &pref, const char *value);
//FuSYSTEM_API void SetPrefTagList(const FuID &pref, TagList *value);

// Other stuff
FuSYSTEM_API int MapPath(char *path); // Uses global pref tags
FuSYSTEM_API int MapPath(CString &path); // Uses global pref tags
FuSYSTEM_API int MapPath(FuString &path); // Uses global pref tags
FuSYSTEM_API int MapPath(FuPath &path); // Uses global pref tags
FuSYSTEM_API FuID GetMappedPath(const char *path); // Uses global pref tags

FuSYSTEM_API int ReverseMapPath(char *path); // Uses global pref tags
FuSYSTEM_API int ReverseMapPath(CString &path); // Uses global pref tags
FuSYSTEM_API int ReverseMapPath(FuString &path); // Uses global pref tags
FuSYSTEM_API int ReverseMapPath(FuPath &path); // Uses global pref tags
FuSYSTEM_API FuID GetReverseMappedPath(const char *path); // Uses global pref tags

FuSYSTEM_API int MapPath(char *path, const ScriptVal &map);
FuSYSTEM_API int MapPath(FuPath &path, const ScriptVal &map, TagList *tags = NULL);

FuSYSTEM_API int ReverseMapPath(char *path, const ScriptVal &map);
FuSYSTEM_API int ReverseMapPath(FuPath &path, const ScriptVal &map, TagList *tags = NULL);

FuSYSTEM_API void GetGlobalPathMap(ScriptVal &map, TagList *tags = NULL);
FuSYSTEM_API void GetGlobalPathMapA(ScriptVal &map, Tag firsttag, ...);

enum GetPathMapTags ENUM_TAGS
{
	GPM_WithDefaults = TAG_INT,	// Default TRUE
	GPM_WithBuiltins,            // Default TRUE
};

// Pref defaults
class FuSYSTEM_API DefPref
{
protected:
	DefPref();
public:
	DefPref(const FuID &pref, const ScriptVal &value);
	DefPref(const DefPref &dp);
	~DefPref();
};

class FuSYSTEM_API DefPrefInt : public DefPref
{
	FuDeclareClass(DefPrefInt, DefPref); // declare ClassID static member, etc.

public:
	DefPrefInt(const FuID &pref, int defval);
	DefPrefInt(const FuID &pref, unsigned int defval);
	DefPrefInt(const FuID &pref, uint32 defval);
};

class FuSYSTEM_API DefPrefBool : public DefPref
{
	FuDeclareClass(DefPrefBool, DefPref); // declare ClassID static member, etc.

public:
	DefPrefBool(const FuID &pref, bool defval);
};

class FuSYSTEM_API DefPrefDouble : public DefPref
{
	FuDeclareClass(DefPrefDouble, DefPref); // declare ClassID static member, etc.

public:
	DefPrefDouble(const FuID &pref, double defval);
};

class FuSYSTEM_API DefPrefString : public DefPref
{
	FuDeclareClass(DefPrefString, DefPref); // declare ClassID static member, etc.

public:
	DefPrefString(const FuID &pref, const char *defval);
};

class FuSYSTEM_API DefPrefPath : public DefPref
{
	FuDeclareClass(DefPrefPath, DefPref); // declare ClassID static member, etc.

public:
	DefPrefPath(const FuID &pref, const char *defval);
};

#define _PREF(pref, name) const FuID pref(name, false)

#define COMPPREF_GROUP "Comp."
#define COMPPREF(pref, name) _PREF(pref, COMPPREF_GROUP name)

#define GLOBALPREF_GROUP "Global."
#define GLOBALPREF(pref, name) _PREF(pref, GLOBALPREF_GROUP name)

#define _EXPORT_PREF(pref) extern FuSYSTEM_API const FuID pref;
#define EXPORT_COMPPREF(pref) _EXPORT_PREF(pref)
#define EXPORT_GLOBALPREF(pref) _EXPORT_PREF(pref)

#define _EXTERN_PREF(pref) extern const FuID pref;
#define EXTERN_COMPPREF(pref) _EXTERN_PREF(pref)
#define EXTERN_GLOBALPREF(pref) _EXTERN_PREF(pref)


////////////////////////////////////////////////////////////////////////////////////
// Prefs
////////////////////////////////////////////////////////////////////////////////////


  EXPORT_COMPPREF(PREF_Info_Comments);
  EXPORT_COMPPREF(PREF_Info_Author);
  EXPORT_COMPPREF(PREF_Info_CreateDate);
  EXPORT_COMPPREF(PREF_Info_ModifyDate);
  EXPORT_COMPPREF(PREF_Info_Client);
  EXPORT_COMPPREF(PREF_Info_Job);

EXPORT_GLOBALPREF(PREF_MainMode);
EXPORT_GLOBALPREF(PREF_MainUseWindowsDefaults);
EXPORT_GLOBALPREF(PREF_MainLeft);
EXPORT_GLOBALPREF(PREF_MainTop);
EXPORT_GLOBALPREF(PREF_MainWidth);
EXPORT_GLOBALPREF(PREF_MainHeight);

EXPORT_GLOBALPREF(PREF_VFXPediaHelp);

  EXPORT_COMPPREF(PREF_FFName);
  EXPORT_COMPPREF(PREF_FFTimeCodeType);
  EXPORT_COMPPREF(PREF_FFWidth);
  EXPORT_COMPPREF(PREF_FFHeight);
  EXPORT_COMPPREF(PREF_FFFields);
  EXPORT_COMPPREF(PREF_FFDepthFull);
  EXPORT_COMPPREF(PREF_FFDepthPreview);
  EXPORT_COMPPREF(PREF_FFDepthInteractive);
  EXPORT_COMPPREF(PREF_FFDropFrame);
  EXPORT_COMPPREF(PREF_FFTimeCodeRadio);
  EXPORT_COMPPREF(PREF_FFDepthLock);
  EXPORT_COMPPREF(PREF_FFDepthLoader);
  EXPORT_COMPPREF(PREF_FFAspectLoader);
  EXPORT_COMPPREF(PREF_FFRate);
  EXPORT_COMPPREF(PREF_FFAspectX);
  EXPORT_COMPPREF(PREF_FFAspectY);
  EXPORT_COMPPREF(PREF_FFPerFeet);
  EXPORT_COMPPREF(PREF_FFGuideX1);
  EXPORT_COMPPREF(PREF_FFGuideY1);
  EXPORT_COMPPREF(PREF_FFGuideX2);
  EXPORT_COMPPREF(PREF_FFGuideY2);
  EXPORT_COMPPREF(PREF_FFGuideRatio);

  EXPORT_COMPPREF(PREF_SplineEditor_AutoSnap);
  EXPORT_COMPPREF(PREF_SplineEditor_GuidesAutoSnap);
  EXPORT_COMPPREF(PREF_SplineEditor_IndependentHandles);
  EXPORT_COMPPREF(PREF_SplineEditor_AutoScale);
  EXPORT_COMPPREF(PREF_SplineEditor_FollowActive);
  EXPORT_COMPPREF(PREF_SplineEditor_ShowKeyMarkers);
  EXPORT_COMPPREF(PREF_SplineEditor_ShowTips);
  EXPORT_COMPPREF(PREF_SplineEditor_AutoScaleVal);

  EXPORT_COMPPREF(PREF_TimelineKeysAutoSnap);
  EXPORT_COMPPREF(PREF_TimelineGuidesAutoSnap);
  EXPORT_COMPPREF(PREF_ViewFilterMode);

  EXPORT_COMPPREF(PREF_LUTView_IndependentHandles);
  EXPORT_COMPPREF(PREF_LUTView_ShowKeyMarkers);
  EXPORT_COMPPREF(PREF_LUTView_ShowTips);
  EXPORT_COMPPREF(PREF_LUTView_AutoScaleVal);

  EXPORT_COMPPREF(PREF_TransportFrameStep);

  EXPORT_COMPPREF(PREF_AVISaverHandler);
  EXPORT_COMPPREF(PREF_AVISaverKeyCheck);
  EXPORT_COMPPREF(PREF_AVISaverKey);
  EXPORT_COMPPREF(PREF_AVISaverDataCheck);
  EXPORT_COMPPREF(PREF_AVISaverDataRate);
  EXPORT_COMPPREF(PREF_AVISaverQuality);

  EXPORT_COMPPREF(PREF_AVIPreviewHandler);
  EXPORT_COMPPREF(PREF_AVIPreviewKeyCheck);
  EXPORT_COMPPREF(PREF_AVIPreviewKey);
  EXPORT_COMPPREF(PREF_AVIPreviewDataCheck);
  EXPORT_COMPPREF(PREF_AVIPreviewDataRate);
  EXPORT_COMPPREF(PREF_AVIPreviewQuality);

  EXPORT_COMPPREF(PREF_PreviewSizeType);				// See enums in PrefsPreview.h for values
  EXPORT_COMPPREF(PREF_PreviewOutDevice);				// (ID) Device to display SingleFilePreview on (1=large, 2=small, 3+ = floating, ID=device)
  EXPORT_COMPPREF(PREF_PreviewWidth);
  EXPORT_COMPPREF(PREF_PreviewHeight);
  EXPORT_COMPPREF(PREF_PreviewKeepAspect);
  EXPORT_COMPPREF(PREF_PreviewHiQ);
  EXPORT_COMPPREF(PREF_PreviewScale);
  EXPORT_COMPPREF(PREF_PreviewType);					// 0=AVI or 1=FlipBook
  EXPORT_COMPPREF(PREF_PreviewDepth);					// 16,24,32 or 0=Match screen
  EXPORT_COMPPREF(PREF_PreviewSkipFrames);			// (bool) skip frames to maintain framerate
  EXPORT_COMPPREF(PREF_PreviewActiveLoaders);		// (bool) Automatically preview Loaders when they are selected
  EXPORT_COMPPREF(PREF_PreviewActiveSavers);			// (bool) Automatically preview Savers when they are selected

EXPORT_GLOBALPREF(PREF_PreviewToolBarMode);
EXPORT_GLOBALPREF(PREF_PreviewToolBarSize);

EXPORT_GLOBALPREF(PREF_SplineSelectorLeft);
EXPORT_GLOBALPREF(PREF_SplineSelectorTop);
EXPORT_GLOBALPREF(PREF_SplineSelectorRight);
EXPORT_GLOBALPREF(PREF_SplineSelectorBottom);

EXPORT_GLOBALPREF(PREF_LayoutCreateFloat);

EXPORT_GLOBALPREF(PREF_AllowNetworkRender);
EXPORT_GLOBALPREF(PREF_NetServerIP);
EXPORT_GLOBALPREF(PREF_NetServerName);
EXPORT_GLOBALPREF(PREF_IsRenderServer);
  EXPORT_COMPPREF(PREF_EngineMemOverride);
  EXPORT_COMPPREF(PREF_DoNetworkRenders);
EXPORT_GLOBALPREF(PREF_NetVerboseLogging);
EXPORT_GLOBALPREF(PREF_NetFrameTimeout);
EXPORT_GLOBALPREF(PREF_RenderOnAll);
EXPORT_GLOBALPREF(PREF_Network_MailRecipients);		// send netrender notification mail to these addresses
EXPORT_GLOBALPREF(PREF_Network_MailSender);			// override From address
EXPORT_GLOBALPREF(PREF_Network_MailSMTPServer);		// specify SMTP server (if empty, MX lookup is used)
EXPORT_GLOBALPREF(PREF_Network_MailOnJobDone);		// send mail when job succeeded
EXPORT_GLOBALPREF(PREF_Network_MailOnJobFailure);	// send mail when job failed
EXPORT_GLOBALPREF(PREF_Network_MailOnQueueDone);	// send mail when queue completed
EXPORT_GLOBALPREF(PREF_Network_MailLogLines);		// attach this many recent log lines to fail notice

EXPORT_GLOBALPREF(PREF_Console_MaxHistory);
EXPORT_GLOBALPREF(PREF_Console_MaxLog);

EXPORT_GLOBALPREF(PREF_GrabDistance);
EXPORT_GLOBALPREF(PREF_AutoControlClose);
EXPORT_GLOBALPREF(PREF_AutoControlHide);
EXPORT_GLOBALPREF(PREF_NextControlOnEnter);
EXPORT_GLOBALPREF(PREF_AutoControlSelected);
EXPORT_GLOBALPREF(PREF_CombinedColorWheel);
EXPORT_GLOBALPREF(PREF_HoverZoomWheel);

  EXPORT_COMPPREF(PREF_AutoSmoothSplines);
  EXPORT_COMPPREF(PREF_AutoSmoothBSplines);
  EXPORT_COMPPREF(PREF_AutoSmoothPolylines);
  EXPORT_COMPPREF(PREF_AutoSmoothLUTs);
  EXPORT_COMPPREF(PREF_AutoSmoothPaths);
  EXPORT_COMPPREF(PREF_AutoSmoothBSplinePolylines);
  EXPORT_COMPPREF(PREF_AutoSmoothMeshes);
  EXPORT_COMPPREF(PREF_ExportSplineMode);

EXPORT_GLOBALPREF(PREF_MemPercentCacheLimit);		// integer % of total physical ram cache limit
EXPORT_GLOBALPREF(PREF_MemAutoPercentCacheLimit);
EXPORT_GLOBALPREF(PREF_MemMargin);
  EXPORT_COMPPREF(PREF_Parallelism);
  EXPORT_COMPPREF(PREF_RenderSimulBranch);
  EXPORT_COMPPREF(PREF_RenderCacheValid);
  EXPORT_COMPPREF(PREF_RenderCacheSource);
  EXPORT_COMPPREF(PREF_RenderCacheLoader);
  EXPORT_COMPPREF(PREF_RenderCacheBranch);
  EXPORT_COMPPREF(PREF_InteractiveSimulBranch);
  EXPORT_COMPPREF(PREF_InteractiveResponse);
  EXPORT_COMPPREF(PREF_InteractiveAlwaysCache);
  EXPORT_COMPPREF(PREF_InteractiveCacheActive);
  EXPORT_COMPPREF(PREF_InteractiveCacheViewed);

  EXPORT_COMPPREF(PREF_AutoSave);
  EXPORT_COMPPREF(PREF_AutoSaveRender);
  EXPORT_COMPPREF(PREF_AutoSaveDelay);

  EXPORT_COMPPREF(PREF_LeftPrevCtrlActiveColor);
  EXPORT_COMPPREF(PREF_LeftPrevCtrlInactiveColor);
  EXPORT_COMPPREF(PREF_LeftRulersShow);
  EXPORT_COMPPREF(PREF_ViewLeftPickW);
  EXPORT_COMPPREF(PREF_ViewLeftPickH);
  EXPORT_COMPPREF(PREF_ViewLeftFarZ);
  EXPORT_COMPPREF(PREF_ViewLeftNearZ);
  EXPORT_COMPPREF(PREF_ViewLeftEnableLUTs);
  EXPORT_COMPPREF(PREF_ViewLeftLUTPlugin);

  EXPORT_COMPPREF(PREF_LargePrevCtrlActiveColor);
  EXPORT_COMPPREF(PREF_LargePrevCtrlInactiveColor);
  EXPORT_COMPPREF(PREF_LargeRulersShow);
  EXPORT_COMPPREF(PREF_ViewLargePickW);
  EXPORT_COMPPREF(PREF_ViewLargePickH);
  EXPORT_COMPPREF(PREF_ViewRightFarZ);
  EXPORT_COMPPREF(PREF_ViewRightNearZ);
  EXPORT_COMPPREF(PREF_ViewRightEnableLUTs);
  EXPORT_COMPPREF(PREF_ViewRightLUTPlugin);

EXPORT_GLOBALPREF(PREF_FloatPrevCtrlActiveColor);
EXPORT_GLOBALPREF(PREF_FloatPrevCtrlInactiveColor);
EXPORT_GLOBALPREF(PREF_FloatRulersShow);
EXPORT_GLOBALPREF(PREF_ViewFloatPickW);
EXPORT_GLOBALPREF(PREF_ViewFloatPickH);
  EXPORT_COMPPREF(PREF_ViewFloatFarZ);
  EXPORT_COMPPREF(PREF_ViewFloatNearZ);
  EXPORT_COMPPREF(PREF_ViewFloatEnableLUTs);
  EXPORT_COMPPREF(PREF_ViewFloatLUTPlugin);

EXPORT_GLOBALPREF(PREF_VideoSplash);

  EXPORT_COMPPREF(PREF_EnablePathsReverseMapping);

  EXPORT_COMPPREF(PREF_PathMap_Comp);
EXPORT_GLOBALPREF(PREF_PathMap_Global);
  EXPORT_COMPPREF(PREF_PathMapSettings_Comp);
EXPORT_GLOBALPREF(PREF_PathMapSettings_Global);

  EXPORT_COMPPREF(PREF_LastFile);
EXPORT_GLOBALPREF(PREF_LastCompFile);
EXPORT_GLOBALPREF(PREF_LastQueueFile);
EXPORT_GLOBALPREF(PREF_LastSettingFile);

EXPORT_GLOBALPREF(PREF_FBX_Import_LastFile);
EXPORT_GLOBALPREF(PREF_FBX_Import_EnableAxisConversion);
EXPORT_GLOBALPREF(PREF_FBX_Import_UpAxis);
EXPORT_GLOBALPREF(PREF_FBX_Import_EnableUnitConversion);
EXPORT_GLOBALPREF(PREF_FBX_Import_ScaleFileUnitsBy);
EXPORT_GLOBALPREF(PREF_FBX_Import_CreateInverseCameraTransforms);
EXPORT_GLOBALPREF(PREF_FBX_Import_FPS);

  EXPORT_COMPPREF(PREF_FilterModeName);

EXPORT_GLOBALPREF(PREF_Script_LoginName);
EXPORT_GLOBALPREF(PREF_Script_LoginPassword);
EXPORT_GLOBALPREF(PREF_Script_EditorPath);
EXPORT_GLOBALPREF(PREF_Script_LoginRequired);
  EXPORT_COMPPREF(PREF_Script_CompData);				// Not really a Pref. Used for persisting arbitrary script data.
EXPORT_GLOBALPREF(PREF_Script_GlobalData);			// Not really a Pref. Used for persisting arbitrary script data.

  EXPORT_COMPPREF(PREF_Cluster_RenderRange);
  EXPORT_COMPPREF(PREF_Cluster_SideFrames);
  EXPORT_COMPPREF(PREF_Cluster_SlaveClasses);

  EXPORT_COMPPREF(PREF_FreehandPrecDlgLeft);
  EXPORT_COMPPREF(PREF_FreehandPrecDlgTop);

  EXPORT_COMPPREF(PREF_FreehandLUTPrecDlgLeft);
  EXPORT_COMPPREF(PREF_FreehandLUTPrecDlgTop);

EXPORT_GLOBALPREF(PREF_PopupEditLeft);
EXPORT_GLOBALPREF(PREF_PopupEditTop);

EXPORT_GLOBALPREF(PREF_PopupEditXYLeft);
EXPORT_GLOBALPREF(PREF_PopupEditXYTop);

EXPORT_GLOBALPREF(PREF_FloatingListBoxLeft);
EXPORT_GLOBALPREF(PREF_FloatingListBoxTop);
EXPORT_GLOBALPREF(PREF_FloatingListBoxRight);
EXPORT_GLOBALPREF(PREF_FloatingListBoxBottom);
EXPORT_GLOBALPREF(PREF_FloatingListBoxShow);

EXPORT_GLOBALPREF(PREF_DefNumberID);
EXPORT_GLOBALPREF(PREF_DefPointID);
EXPORT_GLOBALPREF(PREF_DefBackgroundID);
EXPORT_GLOBALPREF(PREF_DefMergeID);
EXPORT_GLOBALPREF(PREF_DefMergeConnect);

  EXPORT_COMPPREF(PREF_DefBSplineDegree);
  EXPORT_COMPPREF(PREF_DefBSplinePolylineDegree);

  EXPORT_COMPPREF(PREF_DefTrackerPointsShow);

  EXPORT_COMPPREF(PREF_SelectPointsFromMultiPolies);

  EXPORT_COMPPREF(PREF_DefTrackerPath);

  EXPORT_COMPPREF(PREF_DefPolyLineEditMode);

EXPORT_GLOBALPREF(PREF_StatusWinLeftPos);
EXPORT_GLOBALPREF(PREF_StatusWinTopPos);

EXPORT_GLOBALPREF(PREF_StampFormat);
EXPORT_GLOBALPREF(PREF_StampQuality);

  EXPORT_COMPPREF(PREF_ProxyScale);
  EXPORT_COMPPREF(PREF_AutoProxy);
  EXPORT_COMPPREF(PREF_AutoProxyScale);
EXPORT_GLOBALPREF(PREF_SummedAreaTablesProxy);
  EXPORT_COMPPREF(PREF_BackgroundRender);
  EXPORT_COMPPREF(PREF_SelectiveUpdate);

EXPORT_GLOBALPREF(PREF_ShowRenderSettings);			// Display the render settings dialog (overridden by holding down Shift)
  EXPORT_COMPPREF(PREF_SelectionSync);
EXPORT_GLOBALPREF(PREF_AutoClipBrowse);
EXPORT_GLOBALPREF(PREF_SummarizeLoadErrors);
EXPORT_GLOBALPREF(PREF_ChildForce);
EXPORT_GLOBALPREF(PREF_SimpleCopies);

  EXPORT_COMPPREF(PREF_ColorMode);
  EXPORT_COMPPREF(PREF_ClipFrameNumberStyle);		// see below for enum values

EXPORT_GLOBALPREF(PREF_DeckComPort);

EXPORT_GLOBALPREF(PREF_AutoEmptyComp);

EXPORT_GLOBALPREF(PREF_NumBeatTimeouts);
EXPORT_GLOBALPREF(PREF_BeatInterval);
EXPORT_GLOBALPREF(PREF_DisableDirectReads);
EXPORT_GLOBALPREF(PREF_DisableCancelIO);
EXPORT_GLOBALPREF(PREF_ReadAheadBuffers);
EXPORT_GLOBALPREF(PREF_LoadCompTimeout);				// time to allow for comps to be loaded in net renders
EXPORT_GLOBALPREF(PREF_LastDroppedTimeout);			// time to wait for a slave to be restarted & rejoin, if the last one gets kicked, instead of failing the render
EXPORT_GLOBALPREF(PREF_ImageViewerTextureDepth);	// 0=Auto, 1=Int8, 2=Native
EXPORT_GLOBALPREF(PREF_ImageViewerDoSmoothResize);	// 0=Auto, 1=Int, 2=Flt16, 3=Flt32
EXPORT_GLOBALPREF(PREF_UseFloat16Textures);			// The image view may use float16 textures to save memory
EXPORT_GLOBALPREF(PREF_AutoDetectGraphicsMemory);
EXPORT_GLOBALPREF(PREF_GraphicsMemory);
EXPORT_GLOBALPREF(PREF_Use10BitFramebuffer);
EXPORT_GLOBALPREF(PREF_DisableViewLUTShaders);		// View LUTs are applied by software rather than OpenGL shaders
EXPORT_GLOBALPREF(PREF_ViewLUTMinRange);				// Minimum range for HDR View LUTs
EXPORT_GLOBALPREF(PREF_ViewLUTMaxRange);				// Maximum range for HDR View LUTs
EXPORT_GLOBALPREF(PREF_BorderlessToolbars);			// The vertical toolbar on the view is shown as borderless
EXPORT_GLOBALPREF(PREF_ImageOverlay);					// Image overlay in RectangleControl (0=none, 1=outside, 2=all)
  EXPORT_COMPPREF(PREF_CloneOverlayMethod);			// Default overlay method (0=blend, 1=submerge, 2=addmerge)
  EXPORT_COMPPREF(PREF_CloneOverlayBlend);			// Default overlay transparency
  EXPORT_COMPPREF(PREF_DisableFlowReordering);		// Don't bother trying to keep the flow in "layer" order

  EXPORT_COMPPREF(PREF_AudioPlayback);					// 0 = mute, 1 = play, other settings later?

EXPORT_GLOBALPREF(PREF_SpiralEditLeft);
EXPORT_GLOBALPREF(PREF_SpiralEditTop);
EXPORT_GLOBALPREF(PREF_RectangleEditLeft);
EXPORT_GLOBALPREF(PREF_RectangleEditTop);

EXPORT_GLOBALPREF(PREF_Timeline_CurrentTime);
EXPORT_GLOBALPREF(PREF_Timeline_AnimatedTools);
EXPORT_GLOBALPREF(PREF_Timeline_SelectedTools);
EXPORT_GLOBALPREF(PREF_Timeline_DisplayPointValues);
EXPORT_GLOBALPREF(PREF_Timeline_ShowSpreadsheet);
EXPORT_GLOBALPREF(PREF_Timeline_ShowTips);
EXPORT_GLOBALPREF(PREF_Timeline_AutoSnapPoints);
EXPORT_GLOBALPREF(PREF_Timeline_AutoSnapGuides);
EXPORT_GLOBALPREF(PREF_Timeline_ToolDisplayMode);
EXPORT_GLOBALPREF(PREF_Timeline_AutoSnapPointsToGuides);
EXPORT_GLOBALPREF(PREF_Timeline_FilterToUse);
EXPORT_GLOBALPREF(PREF_Timeline_AutoScale);
EXPORT_GLOBALPREF(PREF_Timeline_AutoScaleVal);

EXPORT_GLOBALPREF(PREF_DiskCache_DisableLocalCaching);
  EXPORT_COMPPREF(PREF_DiskCache_NetGroups);
  EXPORT_COMPPREF(PREF_DiskCache_CacheLoaders);
  EXPORT_COMPPREF(PREF_DiskCache_CacheLoadersMultiframe);
  EXPORT_COMPPREF(PREF_DiskCache_CacheLoadersLimitedSize);
  EXPORT_COMPPREF(PREF_DiskCache_CacheLoadersMaxSize);
  EXPORT_COMPPREF(PREF_DiskCache_CacheLoadersOnMissingOrig);
  EXPORT_COMPPREF(PREF_DiskCache_CacheLoadersNoLocal);
  EXPORT_COMPPREF(PREF_DiskCache_CacheLoadersDiskCache);
  EXPORT_COMPPREF(PREF_DiskCache_CacheLoadersSeparator);

EXPORT_GLOBALPREF(PREF_OpenCL_Enable);
EXPORT_GLOBALPREF(PREF_OpenCL_Device);
EXPORT_GLOBALPREF(PREF_OpenCL_Verbose);
EXPORT_GLOBALPREF(PREF_OpenCL_ShowPerf);
EXPORT_GLOBALPREF(PREF_OpenCL_CompileOptions);


EXPORT_GLOBALPREF(PREF_Child);
	// This contains a numeric table, each element containing sub-items:
	const char * const CHILD_Mode                  = "Mode";
	const char * const CHILD_Left                  = "Left";
	const char * const CHILD_Top                   = "Top";
	const char * const CHILD_Width                 = "Width";
	const char * const CHILD_Height                = "Height";
	const char * const CHILD_UseWindowsDefaults    = "UseWindowsDefaults";
	const char * const CHILD_OpenOnNew             = "OpenOnNew";

EXPORT_GLOBALPREF(PREF_FloatView);
	// This contains a numeric table, each element containing sub-items:
	const char * const FLOATVIEW_Mode    = "Mode";
	const char * const FLOATVIEW_Left    = "Left";
	const char * const FLOATVIEW_Top     = "Top";
	const char * const FLOATVIEW_Width   = "Width";
	const char * const FLOATVIEW_Height  = "Height";

////////////////////////////////////
// PREFTODO: Unsorted
////////////////////////////////////

  EXPORT_COMPPREF(PREF_GlobalStart);
  EXPORT_COMPPREF(PREF_GlobalEnd);
EXPORT_GLOBALPREF(PREF_AutoSummedAreaTables);
EXPORT_GLOBALPREF(PREF_SaveCompressedComps);
EXPORT_GLOBALPREF(PREF_CheckForUpdates);
EXPORT_GLOBALPREF(PREF_DevLicenceAgreement);			// Display licence agreement if true

EXPORT_GLOBALPREF(PREF_EventControlState);
EXPORT_GLOBALPREF(PREF_ToolbarState);
  EXPORT_COMPPREF(PREF_RenderSettings);


////////////////////////////////////
// Enums for pref values  //PREFTODO: Rework

enum ClipFrameNumberStyle_enum
{
	CNS_ZeroBased = 0,
	CNS_OneBased,
	CNS_SequenceNumbers,
};

enum TimeCode_enum
{
	TC_PRF_Frame = 0,
	TC_PRF_SMPTE,
	TC_PRF_Feet,
};



#endif
#endif
