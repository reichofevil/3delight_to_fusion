#ifndef _OPERATOR_H_
#define _OPERATOR_H_

#ifndef DFMANAGER

#include "Object.h"
#include "TimeList.h"
#include "HashList.h"
#include "TimeExtent.h"
#include "TimeRegion.h"
#include "Request.h"
#include "RegistryLists.h"
#include "PreviewControl.h"
#include "Input.h"
#include "Output.h"
#include "StandardControls.h"

#if !(defined(DFPLUGIN) || defined(FuPLUGIN))
#ifndef DFLIB
#include "Inline.h"
#include "ContextHelpID.h"
#include "Resource.h"
#endif
#endif

//class InputControl;
class Link;
class IOClass;
class FusionDoc;
class ControlWnd;
class OperatorControl;
class RenderMessage;
class CChildFrame;
class DIB;
class Preview;
class UserControlInfo;


#define ABORT_ID		MAKEID('Abrt')
#define QUIT_ID		MAKEID('Quit')
#define OKAY_ID		MAKEID('Okay')
#define COMPLETE_ID	MAKEID('Done')
#define WAIT_ID		MAKEID('Wait')
#define CONTINUE_ID	MAKEID('Cont')
#define EVENT_ID		MAKEID('Evnt')

// A good priority for masks...
#define MASK_PRI			-1000
#define MOTIONBLUR_PRI	2000

// Used to track what Previews are available on what frames
class PreviewFrameInfo
{
public:
	Preview *Preview;
	CChildFrame *Frame;		// can be NULL
};

class FuSYSTEM_API EventMessage : public Message
{
	FuDeclareClass(EventMessage, Message); // declare ClassID static member, etc.

public:
	EventMessage(const FuID &eventid, uint_ptr subid, TagList &tags);

	const FuID EventID;
	uint_ptr SubID;
	TagList Tags;

	bool handled;
};

#if !defined(USE_NOSCRIPTING)
class LuaScript;

class ScriptData : public LockableObject
{
	FuDeclareClass(ScriptData, LockableObject); // declare ClassID static member, etc.

public:
	ScriptData();

	LuaScript *ObtainScript(Operator *op);
	void ReleaseScript(LuaScript *pScript);

	Input *InStartScript, *InFrameScript, *InEndScript;
	bool PrevHasStartScript, PrevHasFrameScript, PrevHasEndScript;
	int HasScriptCount;

	ListObj<LuaScript> m_Scripts;
	FuPointer<LuaScript> m_Script;
	bool m_DoneStart;
};
#else
class ScriptData;
#endif

class ControlPage : public Object
{
	FuDeclareClass(ControlPage, Object); // declare ClassID static member, etc.

public:
	const char *Name;
	int Number;

	TagList Attrs;
};

typedef enum Status_Values
{
	STATUS_OK,
	STATUS_ABORT,
	STATUS_QUIT
} StatusType;

enum TilePic_Channel_Types
{
	TPC_COLOR = 0,
	TPC_RED,
	TPC_GREEN,
	TPC_BLUE,
	TPC_ALPHA,
};

// Replace With flags
#define ORW_UseNewSettings 0x1
#define ORW_UseOldSettings 0x0
#define ORW_UseNewOperator 0x2
#define ORW_UseOldOperator 0x0
#define ORW_UseStaticSettings 0x4 // Currently at new inputs at time 0.0, are put in old inputs at current time
#define ORW_SpecifiedLevelOnly 0x8

class KFContext;
typedef uint32 KFCmd;

// Cache Modes
#define CMOP_MASK     0x0000ffff
#define CMOP_Always   (1 << 0) // Always Cache
#define CMOP_TimeCost (1 << 1) // Cache if we've exceeded time cost
#define CMOP_Valid    (1 << 2) // Cache if this parameter will be useful later
#define CMOP_Active   (1 << 3) // Cache if the operator is active
#define CMOP_Viewed   (1 << 4) // Cache if the operator is being viewed
#define CMOP_Source   (1 << 5) // Cache if this parameter will be useful later and the operator is a source tool
#define CMOP_Loader   (1 << 6) // Cache if this parameter will be useful later and the operator is a loader
#define CMOP_Branch   (1 << 7) // Cache if the operator has multiple downstream branches
#define CMOP_Disk     (1 << 8) // Cache to disk
#define CMOP_ForceRAM (1 << 9) // Force Cache to RAM
// Input Cache Modes
#define CMIN_MASK     0x00ff0000
#define CMIN_Always   (1 << 16) // Always Cache
#define CMIN_TimeCost (1 << 17) // Cache if we've exceeded time cost
#define CMIN_Valid    (1 << 18) // Cache if this parameter will be useful later
#define CMIN_Active   (1 << 19) // Cache if the operator is active
#define CMIN_Viewed   (1 << 20) // Cache if the operator is being viewed
#define CMIN_Source   (1 << 21) // Cache if this parameter will be useful later and the operator is a source tool
#define CMIN_Loader   (1 << 22) // Cache if this parameter will be useful later and the operator is a loader
#define CMIN_Branch   (1 << 23) // Cache if the operator has multiple downstream branches
// Output Cache Modes
#define CMOU_MASK     0xff000000
#define CMOU_Always   (1 << 24) // Always Cache
#define CMOU_TimeCost (1 << 25) // Cache if we've exceeded time cost
#define CMOU_Valid    (1 << 26) // Cache if this parameter will be useful later
#define CMOU_Active   (1 << 27) // Cache if the operator is active
#define CMOU_Viewed   (1 << 28) // Cache if the operator is being viewed
#define CMOU_Source   (1 << 29) // Cache if this parameter will be useful later and the operator is a source tool
#define CMOU_Loader   (1 << 30) // Cache if this parameter will be useful later and the operator is a loader
#define CMOU_Branch   (1 << 31) // Cache if the operator has multiple downstream branches
//
#define CM_Always   (CMOP_Always|CMIN_Always|CMOU_Always)
#define CM_TimeCost (CMOP_TimeCost|CMIN_TimeCost|CMOU_TimeCost)
#define CM_Valid    (CMOP_Valid|CMIN_Valid|CMOU_Valid)
#define CM_Active   (CMOP_Active|CMIN_Active|CMOU_Active)
#define CM_Viewed   (CMOP_Viewed|CMIN_Viewed|CMOU_Viewed)
#define CM_Source   (CMOP_Source|CMIN_Source|CMOU_Source)
#define CM_Loader   (CMOP_Loader|CMIN_Loader|CMOU_Loader)
#define CM_Branch   (CMOP_Branch|CMIN_Branch|CMOU_Branch)

class LayerControls;

class FuSYSTEM_API Operator : public Object
{
	FuDeclareClass(Operator, Object); // declare ClassID static member, etc.

//protected:
public:
	HDC DefaultDC;
	Node *MiniPicPtr;
	FuPointer<Image> MiniPicImage;
	bool IsBeingLoaded;		// Set if this op has been loaded
	bool BuildTile;
	bool Selected;
	bool HasFailed;			// Set if the last operation failed
	bool SerialReqs;
	bool Created;
	bool Locked;				// Set if the tool cannot be changed
	bool PassThrough;			// if set, tool passes images through unchanged
	bool HoldOutput;
	bool CtrlWZoom;
	bool NameSet;
	bool NotHeldYet;
	bool DrawFailed;

	int32 RenderActive;
	int32 MaskRenderActive;

	uint32 Override;

#ifdef DEMO_VERSION
public:
	int lx,ly, dx,dy;
#endif

	Input *InMask, *InUseBG;
	Input *InUseObj, *InUseMat;
	Input *InObjID, *InMatID;
	Input *InDoMBlur, *InMBShutter, *InMBQuality;
	Input *InInvertMask;
	Input *InMultiplyMask;
	Input *InMBCenterBias, *InMBSampleSpread;
	Input *InProcessR, *InProcessG, *InProcessB, *InProcessA;
	Input *InBlend, *InBlendZero;
	Input *InComments;

	// Bitmap mask style inputs
	Input *InMaskCenter, *InMaskChannel;
	Input *InMaskLow, *InMaskHigh;

	Input *MainInput;

	Output *OutPreview;

	FuPointer<Parameter> Cache;
	LockableObject CacheLock;
	TimeExtent *CacheValid;

	List CurrentPreviews;		// List of FramePreviews

public:
	TagList Attrs;					// General attributes (see below for list)
#ifndef USE_NOSCRIPTING
	ScriptVal m_CustomData;
#endif

	char Name[256];

	int32 PrevCtrlScrollPos;
	int32 MostRecentPage;

	TimeStamp LastInteractiveTime;

	int32 WaitNest;

	uint32 CacheMode; // Caching Flags
	int16 ActiveForceCache; // Nesting Count
	int16 ViewedForceCache; // Nesting Count
	int32 DeferInvalidate; // Nesting Count
	int32 RenderDeferInvalidate; // Nesting Count

	char *GetName();
	void SetName(const char *name);
	bool RenameCopy(FusionDoc *doc = NULL);

	virtual void GetTitle(char *buf);
	virtual void GetStatus(char *buf, int maxlen);

	float32 GetProgress();
	void SetProgress(float32 progress);

	LockableObject GUILock;

	uint32 ID;

	HashList InputList;
	HashList OutputList;

	List PreviewControlList;
	List ControlPageList;
	int CurrentPage;

	uint32 NestCount;
	Input *CurrentNest;

	Output *MainOutput;
	int32 NumInputs;
	int32 NumOutputs;
	int32 TempImages;
	int32 UsedImages;
	
	StatusType Status;
	bool Visible;
	uint32 TilePicChannel;

	TimeList RequestList;
	HANDLE BusyMutex;
	FusionDoc *Document;

	LockableObject RegionLock;

	TimeRegion *OpRegion; // This is the operator-defined region for valid output
	TimeRegion *Region;   // This is the intersection of OpRegion and all inputs' Regions

	double TimeCost, LastFrame;
	
	// Information for Status Bar
	uint32 ImageWidth, ImageHeight;
	float64 ImageAspectX, ImageAspectY;
	uint32 ImageField, ImageDepth;

	bool PrevHasInfo;
	void *OldCustomData;

	TimeRegion *EnabledRegion;							// Outside this, the op is disabled.
	TimeStamp KFGrabTime, KFGrabLeft, KFGrabRight;
	uint32 KFGrabFlags;

	FuPointer<Operator> GroupParent;
	List Children;

	FuID SourceOp;
	FuPointer<Operator> InstanceParent;
	List InstanceChildren;

	bool Deleting, Replacing;
	bool DiskCacheEnabled;
	bool DiskCacheLocked;

	TimeList *SerialisedRequests;							// used by tools with SerialReqs == TRUE

	float32 Progress;
	uint32 LastUpdateTicks;

	uint16 ProgressScale, ProgressCount;
	uint16 MBProgressScale, MBProgressCount;

	bool CtrlWShown;

	bool OnFlow;

	ScriptData *m_ScriptData;

	Request *m_CurrentReq;

	int32 DeferRegionChanged; // Nesting Count

	FuID m_CurrentGroup;

	List AddInputsList;
	int32 AddInputsNest;

	int32 DeferCheck;		// Generic GUI thread only count.

	UserControlInfo *m_UserControls;

	Input *InMBDisableMotion, *InMBReferenceFrame;
	Input *InMaskFitMode;

	Object *m_pToolViewInfo;

	FuRectInt m_OperatorDataWindow;		// Remembers the last pre-mask  Image DataWindow for tooltips (like ImageWidth, ImageHeight, etc. above)
	FuRectInt m_OutputDataWindow;			// Remembers the last post-mask Image DataWindow for tooltips (like ImageWidth, ImageHeight, etc. above)
	TimeExtentList m_DoDList;

	LayerControls *m_LayerControls;

	Input *InDomainAdjustMode, *InDomainAdjustLeft, *InDomainAdjustBottom, *InDomainAdjustRight, *InDomainAdjustTop;
	Input *InMaskClipBlack, *InMaskClipWhite;

	uint8 Pad[232 - sizeof(FuRectInt) * 2 - sizeof(TimeExtentList) - sizeof(LayerControls *) - (sizeof(Input *) * 7)];

protected:
	static bool CreateRegBitmap(Registry *regnode, Tag RegID, Tag RegBMID, Tag RegDCID);
	static void DeleteRegBitmap(Registry *regnode, Tag RegBMID, Tag RegDCID);
	virtual bool PreProcessMsg(Message *msg);
	virtual bool HandleMsg(Message *msg);
	virtual bool AddRequest(Request *req);
	virtual bool HandleRequest(Request *req);
	virtual void CheckRequest(Request *req);
	virtual void ChainDuplicateRequests(Request *req, TimeExtent &te);
	void ChainDuplicateRequests(List *list, Request *req, TimeExtent &te);
	Request *ChainRequest(Request *headreq, Request *chainreq);

	Request *PrepareMotionBlur(Request *req, bool &domb, int &quality, bool &useref);
	void ApplyMotionBlur(Request *req, bool domb, int quality, bool useref, bool passThrough, Request *basereq);

	void UpdateChainReqParams(Request *req, Request *chain);
	void UpdateChainInputRoIs(Request *req, Request *chain);

	ImageDomain *GetDoDTagList(Request *req, const TagList *attrs = NULL);

// event handlers
	void DoEvent_Operator_ViewOn(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_PreviewOn(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_Delete(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_SwapInputs(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_EditAll(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_Rename(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_ShowControls(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_PassThrough(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_Locked(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_HoldOutput(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_CacheToDisk(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_ForceCacheToRAM(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_Cluster(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_SetRenderRange(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_Load(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_Save(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_RunScript(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_EditScript(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_Recent(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_LoadDef(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_SaveDef(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_ResetDef(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_DeInstance(uint_ptr subid, TagList &tags);
	void DoEvent_Operator_SetCurrentSettings(uint_ptr subid, TagList &tags);

	TimeStamp SnapToTime(TimeStamp t);

public:
	Operator(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~Operator();
	virtual bool Save(ScriptVal &table, const TagList &tags);

	void OnLoadLinks(const ScriptVal &table, const TagList &tags);

	virtual bool OnLoadLink(Link *link, const ScriptVal &table, const TagList &tags);
	virtual bool OnSaveLink(Link *link, ScriptVal &table, const TagList &tags);

	// Class init & cleanup
	static bool RegInit(Registry *regnode);
	static void RegCleanup(Registry *regnode);
	
	Input  *FindInput (const char *id);
	Output *FindOutput(const char *id);
	
	Input  *FindMainInput (int num); // 1 = PRIMARY, 2 = SECONDARY etc
	Output *FindMainOutput(int num);

	bool RemoveInput(Input *in);
	bool RemoveOutput(Output *out);

	Input *AddInput(const FuID &name, const FuID &id, Tag firsttag = _TAG_DONE, ...);
	Input *AddInputTagList(const FuID &name, const FuID &id, TagList &tags);
	Output *AddOutput(const FuID &name, const FuID &id, Tag firsttag = _TAG_DONE, ...);
	Output *AddOutputTagList(const FuID &name, const FuID &id, TagList &tags);

	Input *CloneInput(Input *in, const FuID &id, Tag firsttag = _TAG_DONE, ...);
	Input *CloneInputTagList(Input *in, const FuID &id, TagList &tags);
	Output *CloneOutput(Output *out, const FuID &id, Tag firsttag = _TAG_DONE, ...);
	Output *CloneOutputTagList(Output *out, const FuID &id, TagList &tags);

	int AddControlPage(const char *Name, Tag firsttag = _TAG_DONE, ...);
	int AddControlPageTagList(const char *Name, TagList &tags);
	bool RemoveControlPage(const char *name);
	bool RemoveControlPage(int index);
	ControlPage *FindControlPage(const char *name);
	ControlPage *FindControlPage(int index);

	Input *BeginControlNest(const FuID &name, const FuID &_id, bool expand = false, Tag firsttag = _TAG_DONE, ...);
	Input *BeginControlNestTagList(const FuID &name, const FuID &_id, bool expand, TagList &tags);
	void EndControlNest(void);

	// Groups are like control nests except they prefix all the inputs added between BeginGroup/EndGroup with the group ID.  Groups
	// may also be begun/ended multiple times making them useful when injecting inputs into a Inputs3D object.
	Input *BeginGroup(const char *name, const FuID &id, bool expand = false, bool showNest = true, Tag firsttag = _TAG_DONE, ...);
	/*virtual*/ Input *BeginGroupTagList(const FuID &name, const FuID &id, bool expand, bool showNest, TagList &tags);
	void EndGroup(Tag firsttag = _TAG_DONE, ...);
	/*virtual*/ void EndGroupTagList(TagList &tags);
	/*virtual*/ int NumGroupInputs(const FuID &id, TagList *tags = NULL);	

	void BeginAddInputs(Tag firsttag = _TAG_DONE, ...);
	void BeginAddInputsTagList(TagList &tags);
	void EndAddInputs(Tag firsttag = _TAG_DONE, ...);
	void EndAddInputsTagList(TagList &tags);

	void DoInitialNotify(List *inputs);

	virtual void ConnectInputs(HashList *flow, TagList *tags = NULL);

	// OnConnect() is called when a connection/disconnection is being attempted to *this* tool.  Returning false will disallow the connection.
	//   You must call the baseclass version of OnConnect and fail if it returns false.
	virtual bool OnConnect(Link *src, Link *dest, TagList *tags = NULL);

	// OnConnected() is a notification after a connection/disconnection has been made on this tool or any upstream tool.  This is useful
	//   (for example) to detect when Displace3D needs to update its camera list combobox when a camera has been added somewhere upstream of it.
	//   When overriding this functio you should call and return the results of BaseClass::OnConnected().
	virtual bool OnConnected(Link *link, Link *oldsrc, Link *newsrc, TagList *tags = NULL);
	bool DoOnConnected(Link *link, Link *oldsrc, Link *newsrc, TagList *tags = NULL);
	virtual void OnLastDisconnect(TagList *tags = NULL);
	virtual void OnDataTypeChanged(Link *link, FuID &oldDataType, TagList *tags = NULL);
	virtual void OnDelete(bool disconnect = true, TagList *tags = NULL);
	virtual void OnReplace(TagList *tags = NULL);

	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;		// for OP_ToolText messages
	
	virtual bool PutMsg(Message *msg);
	virtual void Abort(bool wait = true, TagList *tags = NULL);
	virtual void InvalidateCache(Input *in = NULL, TagList *tags = NULL);
	virtual void InvalidateCache(const TimeExtent& te, Input *in = NULL, Tag firsttag = _TAG_DONE, ...);

	virtual bool IsSpline();

	virtual bool CanConnect(Link *src, Link *dest, TagList *tags = NULL);
	virtual bool CheckRecursiveConnect(Link *lnk, TagList *tags = NULL);
	bool DoCheckRecursiveConnect(Link *lnk, TagList *tags = NULL);
	virtual void SetCacheMode(uint32 mode, TagList *tags = NULL);

	virtual bool CreateInputControls(ControlWnd *wnd, HWND parent, CRect& wrect, int page, TagList *tags = NULL);

	virtual void OnAddToFlow(TagList *tags = NULL);
	virtual void OnRemoveFromFlow(TagList *tags = NULL);

	// Subclass Defined

	virtual void OnStartRender(RenderMessage *rmsg, TagList *tags = NULL);
	virtual void OnEndRender(TagList *tags = NULL);
	virtual void OnGlobalExtentChanged(TimeExtent *globext, TagList *tags = NULL);
	virtual void PreProcess(Request *req);
	virtual void PostProcess(Request *req);
	virtual void Process(Request *req);           // Standard process function. (Required)

	virtual void PreCalcPreProcess(Request *req);
	virtual void PreCalcPostProcess(Request *req);
	virtual void PreCalcProcess(Request *req);

	virtual bool SetSource(Output *output, Parameter *param, TimeStamp time, TagList *tags = NULL);
	virtual bool MakeMiniPic(Request *req, TagList *tags = NULL);
	virtual bool ReplaceWith(Operator *op, uint32 flags = (ORW_UseOldSettings|ORW_UseNewOperator), TagList *tags = NULL);

	// OpenGL support 
	virtual bool SupportsProcessGL(Request *req);
	virtual bool ProcessGL(Request *req);
//	virtual void CheckUploadImages(Request *req);

	// Optionally called when an input's value changes
	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);

	// Time Region stuff
	virtual void RegionChanged(Input *in, TagList *tags = NULL);
	virtual void SetRegion(TimeRegion *tr, TagList *tags = NULL);
	virtual TimeRegion *GetRegion(TagList *tags = NULL);
	virtual void GetExtent(TimeExtent &te, TagList *tags = NULL);
	void GetEnabledRegion(TimeRegion &tr, TagList *tags = NULL);

	virtual void LockRegion();
	virtual void UnlockRegion();
	virtual bool UpdateTile(TagList *tags = NULL);
	virtual bool UpdateOperator(uint32 reqflags, TagList *tags = NULL);

	//
	virtual void AddControls(PreviewInfo *pi, TagList *tags = NULL);
	virtual void UpdateControls(TagList *tags = NULL);
	virtual void GLDrawControls(OperatorControl *oc, PreviewInfo *pi, TagList *tags = NULL);
	virtual DragType ControlDown(OperatorControl *oc, PreviewInfo *pi, CPoint point, TagList *tags = NULL);
	virtual void ControlMove(OperatorControl *oc, PreviewInfo *pi, CPoint point, TagList *tags = NULL);
	virtual void ControlUp  (OperatorControl *oc, PreviewInfo *pi, CPoint point, TagList *tags = NULL);
	virtual uint_ptr ControlCommand(OperatorControl *oc, PreviewInfo *pi, const FuID &id, uint_ptr data, TagList *tags = NULL);
	virtual bool ControlGetTip(OperatorControl *oc, PreviewInfo *pi, char *buf, TagList *tags = NULL);

	virtual void RestoreState(UndoState *us, TagList *tags = NULL);
	virtual UndoState *SaveState(TagList *tags = NULL);
	virtual void CleanupState(UndoState *us, TagList *tags = NULL);

	virtual bool Display(CChildFrame *frame, TagList *tags = NULL);
	virtual int GetToolTip(int x, int y, char *buf, RECT *rect, TagList *tags = NULL);
	virtual void OnActive(bool activated, CChildFrame *frame, TagList *tags = NULL);

	virtual void KFDraw(KFContext *kfc, TagList *tags = NULL);
	virtual bool KFDown(KFContext *kfc, float32 x, float32 y, TagList *tags = NULL);
	virtual bool KFDrag(KFContext *kfc, float32 x, float32 y, TagList *tags = NULL);
	virtual void KFUp(KFContext *kfc, float32 x, float32 y, TagList *tags = NULL);
	virtual bool KFCommand(KFContext *kfc, float32 x, float32 y, KFCmd id, void *data, TagList *tags = NULL);
	virtual bool KFGetTip(KFContext *kfc, float32 x, float32 y, char *buf, TagList *tags = NULL);

	void LockTool(bool lock);
	bool IsLocked() const;
	void DisableTool(bool disable);
	bool IsDisabled() const;
	void HoldTool(bool hold);
	bool IsHeld() const;
	void SetCtrlWZoom(bool Zoom);
	bool IsCtrlWZoom(void) const;
	bool HasInfo(void);
	bool HasDiskCache(void);
	bool HasScript(void);

	bool IsAnimated(void);

	void SetRenderActive(bool on);
	bool IsRenderActive(void);
	void SetMaskRenderActive(bool on);
	bool IsMaskRenderActive(void);

	virtual TimeExtent GetValid(Request *req, TagList *tags = NULL);
	virtual void AddToMenu(Menu *menu, TagList &tags);
	virtual void HandleMenu(uint32 cmd, uint_ptr *data, TagList &tags);
	virtual void HandleMenu(const ScriptVal &cmd, const ScriptVal &data, TagList &tags);
	// Called on Load... from the ControlWnd context menu to see what the
	// preferred settings dir is
	virtual char *GetSettingsDir(void);

	// And this determines what files are shown in the CFileDialog
	virtual char *GetSettingsDirFilter(void);

	virtual void CachedExtent(const TimeExtent *te, bool skipthis = false, TagList *tags = NULL);

	virtual void GetAvailableEvents(EventList *events, const FuID &eventid = NOID);
	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);
	virtual bool OnEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	virtual TimeStamp GetNextKeyTime(TimeStamp time, TagList *tags = NULL);
	virtual TimeStamp GetPrevKeyTime(TimeStamp time, TagList *tags = NULL);

	virtual bool GetDiskCacheName(char *buffer, int maxbuflen, bool dironly = false, TagList *tags = NULL);
	virtual void CreateScriptState(TagList *tags = NULL);

	virtual void SetCurrentGroup(const FuID &id = NOID, TagList *tags = NULL);		// deprecated: use BeginGroup/EndGroup

	virtual void SetGroupParent(Operator *parent, TagList *tags = NULL);
	virtual void SetInstanceParent(Operator *parent, bool doinputs = true, TagList *tags = NULL);
	bool IsChild(Operator *child);

	Input *CreateUserControl(const FuID &id, ScriptVal &ctrl, TagList *tags = NULL);

	int GetCurrentSettings();
	void SetCurrentSettings(int set);

	void EnableMiniPic(void);
	void DisableMiniPic(void);

	HDC GetMiniPic(void);

	void BeginIndent();
	void EndIndent();

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(Operator);
#endif
};

uint32 FuSYSTEM_API GetSerialNumber();

#define OPERATOR_BASE OBJECT_BASE + TAGOFFSET

// Possible attributes for Attrs member
enum OpAttrs_Enum  ENUM_TAGS
{
	OP_Dummy = OPERATOR_BASE | TAG_INT,					// (bool) used to remember an unknown tool's original status
	OP_Exported,												// Used by script exporting
	OP_TileColor,
	OP_TextColor,
	OP_ShineColor,
	OP_ShadowColor,
	OP_FillColor,
	OP_CurrentSettings,
	OP_CheckDependencies,

	OP_ToolView = OPERATOR_BASE | TAG_PTR,				// ptr to a ToolView object (e.g. GridFlow)
	OP_ToolViewInfo,											// data ptr: owned by the immediately preceding OP_ToolView object
	OP_ToolText,												// ptr to a buffer containing extra info from a tool. Displayed in tooltips and on the status bar
	OP_GetUpstreamRegion,									// (Region*) get intersection of all upstream regions
	OP_DummyData,												// Used to remember an unknown tool's original saved data
	OP_SplViewOpInfo,											// ptr to the first OpInfo object

	OPD_GetRegionFrameLength = OPERATOR_BASE | TAG_DOUBLE,	// (TimeStamp) return 1.0/0.5/etc to override framelength, otherwise TIME_UDEFINED

	OPID_SettingsDir = OPERATOR_BASE | TAG_ID,
	OPID_SettingsFilter,
};

enum OpToolViewAttrs_Enum ENUM_TAGS
{
	OPTV_Operator = OPERATOR_BASE | TAG_PTR,		// ptr back to the operator that the OP_ToolViewInfo is attached
};


#define OPERATOR_REQ_BASE		INPUT_REQ_BASE + TAGOFFSET
enum OperatorReqAttrs ENUM_TAGS
{
	OPRA_SkipProcess = TAG_INT + OPERATOR_REQ_BASE,

	OPRAO_ImageDoD = TAG_OBJECT + OPERATOR_REQ_BASE,	// Ideally these would be in Image.h, but it doesn't know about OPERATOR_REQ_BASE
	OPRAO_ImageRoI,												// The 'normal' method, using pixel co-ords - ptr to an ImageDomain object
	OPRAO_NormalisedRoI,											// Normalised co-ords, only used in specialised cases - ptr to a RectDomain object

	OPRAO_RootImage,
};


#define OPERATOR_REG_BASE		REG_USER + TAGOFFSET
enum OperatorRegTags ENUM_TAGS
{
	OPREGID_ToolViewInfo = OPERATOR_REG_BASE | TAG_ID,
};

#define OVR_Blend						(1l << 0)
#define OVR_ObjMat					(1l << 1)
#define OVR_MotionBlur				(1l << 2)
#define OVR_Channels					(1l << 3)
#define OVR_Mask						(1l << 4)
#define OVR_PreCalc					(1l << 5)

#endif
#endif
