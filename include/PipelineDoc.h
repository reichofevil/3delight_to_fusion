/** 
 * @file  PipelineDoc.h 
 * @brief interface of the FusionDoc class
 */

#ifndef _PIPELINE_DOC_H_
#define _PIPELINE_DOC_H_

#include "Aardvark.h"

#ifndef DFMANAGER

#include "MsgPort.h"
#include "RegistryLists.h"
#include "TimeExtent.h"
#include "Request.h"
#include "StandardEvents.h"
#ifndef DFLIB
#include "UndoState.h"
#endif

#ifdef _DEBUG
#undef new						// MFC-STL-MS memory debugging conflicts
#endif

#include <set>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef RELEASE_VERSION
#define CHECK_UNDONEST(doc)		NULL
#else
#define CHECK_UNDONEST(doc)		doc->CheckUndoNest()
#endif

class RenderThread;
class Operator;
class CChildFrame;
class FusionApp;
class PrefsPage;
class LuaScript;
struct HeartbeatThread_Data;
class Cluster;
class Preview;
class FrameSet;
struct ConsoleMessage;
class UndoContext;
class FuString;

class PathMapFileList;
class SnapGuidesContainer;

/**
 * the document class for Fusion
 */
#ifndef NO_MFC
class FuSYSTEM_API FusionDoc : public CDocument, public Object
{
	FuDeclareClass(FusionDoc, Object); // declare ClassID static member, etc.

#else
class FuSYSTEM_API FusionDoc : public Object
{
	FuDeclareClass(FusionDoc, Object); // declare ClassID static member, etc.

#endif

public:
#ifdef DF_LIGHT
	bool islight;
#endif

protected: // create from serialization only
	FusionDoc();
	DECLARE_DYNCREATE(FusionDoc)

// Attributes
public:
	bool TilePics;
	bool Rendering;
	bool DoRender;
	bool HiQ;
	bool MotionBlur;
	bool UserAbort;
	bool Quality;
	bool Proxy;
	bool QuitWhenRendered;
	bool Aborting;
	bool OldProxy, OldHiQ, OldMotionBlur, OldTilePics;
	bool AutoProxy;

	int32 Batch;  // Now a nest count
	int32 RenderDepth;

	TimeStamp CurrentTime;
	TimeStamp RenderStartTime, RenderEndTime;
	TimeExtent GlobalExtent;
	char *Hbuf;

	// Render Thread Data
	uint32 ThreadID;
	HANDLE ThreadHandle;
	TimeStamp Start, End;			// Render times
	
	uint32 StartTime, FrameTime;

public:
	FusionApp *TheApp;			// Ptr to our App object, as AfxGetApp() doesn't work for non-CWinThreads :-(
	HashList *TheFlow;				// The central list of all operators!
	bool FlowLocked;

	uint32 CurrentID;
	bool LockTime;

	List FrameList;					// All CChildFrames relating to this doc
	List FlowViewList;				// All CPipelineViews relating to this doc
	List ControlViewList;			// All ControlViews relating to this doc
	List PreviewList;					// All Previews relating to this doc (not really used)

	List MiscObjects;					// Anything here gets cleaned up when the doc destructs

	MsgPort RenderPort;				// Needed public by Preview in PipelineView.cpp..

	// Undo Stuff
	List *UndoList, *RedoList;
	UndoContext *UndoPtr, *RedoPtr, *SavePtr;
	int UndoNest;

	TimeStamp MinFrameLength;

	int32 NumImages, MinImages;
	HANDLE MemEvent, MemMutex;
	
	double MaxTimeCost;

	int ProxyScale;

public:
	TimeStamp LastSuccessfulFrame;		// The last frame that rendered OK

	uint32 ElapsedTime, FinishTime;
	uint32 LastFrameTime, AverageFrameTime;

	uint32 LastProgressUpdate;

	bool m_CloseOnDisconnect;
	bool m_IsNetRendering;

	LockableObject ProxyLock;

public:
	HeartbeatThread_Data *HeartbeatData;

#ifndef USE_NOSCRIPTING
	ScriptVal m_PrefTable;
	ScriptVal m_QuoteTable;
	ScriptVal m_CustomData;
#endif

	bool automodified;
	uint32 LastFlags;

	bool Undoing;

	bool UseNetwork;

public:
	bool Serializing;
	bool Closing;

	uint64 totalimagemem;
	uint32 totalmemcount;
	uint32 avgimagemem;

	FrameSet *RenderFrameSet;
	CString *RenderSlaveClasses;
	int RenderStep;

	//
	bool Playing;
	TimeStamp PlayStartTime, PlayEndTime;

	bool AutoSave;

	int PendingProxyScale;

	bool PreAborting;
	int32 ReqCount;

	// The last prefs page shown for this document
	PrefsPage *LastPrefsPage, *PrevLastPrefsPage;

	// Our scripting state
	FuPointer<LuaScript> m_Script;
	
	bool PlayLoop;

	// The render thread class!!
	RenderThread *m_RenderThread;

	// The cluster class
	Cluster *m_Cluster;

	// Sound Preview
	Preview *m_Sound;
	HANDLE RenderFrameEvent;	// for syncing

	// For Script tool positioning
	int32 XPos, YPos;

	// PrintF History
	ConsoleMessage *m_ConsoleHistory, *m_ConsoleTail;

	bool Hidden;
	bool m_DFCG;

	// Current audio filename && offset
	const char *AudioFilename;
	double AudioOffset;

	int32 FrameTimeRunningTotal;

	COLORREF RenderIndicatorCol;
	int32 RenderIndicatorDir;
	uint32 RenderIndicatorTime;

	uint32 m_ConsoleHistorySize;
	uint32 m_ConsoleSeq;

	List *m_AddMultipleList;

	int32 OldSelectiveUpdate;

	PathMapFileList *ToolScripts;

	SnapGuidesContainer *m_GuidesContainer;

   bool LoadCompError;
	bool m_AutoPos;
	bool m_AutoRenderRange, m_DoAutoRenderRange;

	int32 m_ClosePriority;		// When should this doc be closed relative to all the others, if Fusion is quit?

	int32 m_LockNestMonster;  // Count of locks that will prevent expressions etc from being able to get a read lock on the flow.
	bool PlayPingPong;

	CString m_RealPath;

	bool m_DoNetLog;
	int m_NumNetLog;
	Lock m_NetLogLock;
	ScriptVal m_NetLog;

	List m_ViewedList;		// List of tools with MainOutput or OutPreview with non-Active connections
	List m_SinkList;			// List of CT_Sink tools
	
	uint32 m_ExitCode;

	/** 
	 * An object ID is a user assigned positive number given to renderable geometry.  This value typically comes from the SurfaceInputs3D 
	 * objectID inputs and is passed through SurfaceData3D::ObjectIDs down to Geometry3D::ObjectID.  By default objects get assigned a 
	 * unique ID, but the user can override this through the UI.  The ID of 0 is reserved to mean 'no object is present in that pixel' and
	 * the ID of -1 is reserved to mean the ID has not been yet set (negative IDs will typically be ignored by any Fusion API funcs).
	 * At rendertime any unset IDs will be assigned an arbitrary unused value, this can vary depending on what tools are on the flow and
	 * how the scene graph is composed (bad!) so it is good for all surface tools to explicitly have a user adjustable object ID input.
	 */
	class ObjectID
	{
	public:
		int32 ID;
		mutable int32 UseCount;
		bool operator< (const ObjectID &rhs) const { return ID  < rhs.ID; }
		bool operator==(const ObjectID &rhs) const { return ID == rhs.ID; }
		ObjectID(int32 id, int32 useCnt) : ID(id), UseCount(useCnt) {}
	};

	LockableObject ObjectIDLock;		/**< Protects ObjectIDs and NextObjectID */
	std::set<ObjectID> ObjectIDs;		/**< Set of currently reserved (usecnt = 0) and in-use object IDs. */
	int32 NextObjectID;					/**< This is the next objectID that *may* be free.  It is an optimization to avoid scanning the set each time to the lowest free ID.  */


	/** 
	 * A material ID is a user assigned positive number given to a material.  This value typically comes from the MaterialInputs3D 
	 * objectID input and is passed through MaterialData3D::ObjectID.  Note that materials consist of a tree of connected nodes and
	 * the ID of the bottom material in the tree is used by default by Fusion.  By default materials get assigned a unique ID, but 
	 * the user can override this through the UI.  The ID of 0 is reserved to mean 'no material is present in that pixel' and
	 * the ID of -1 is reserved to mean the ID has not been yet set (negative IDs will typically be ignored by any Fusion API funcs).
	 * At rendertime any unset IDs will be assigned an arbitrary unused value, this can vary depending on what tools are on the flow and
	 * how the scene graph is composed (bad!) so it is good for all material tools to explicitly have a user adjustable material ID input.
	 */
	class MaterialID
	{
	public:
		int32 ID;
		mutable int32 UseCount;
		bool operator< (const MaterialID &rhs) const { return ID  < rhs.ID; }
		bool operator==(const MaterialID &rhs) const { return ID == rhs.ID; }
		MaterialID(int32 id, int32 useCnt) : ID(id), UseCount(useCnt) {}
	};

	LockableObject MaterialIDLock;		/**< Protects MaterialIDs and NextMaterialID */
	std::set<MaterialID> MaterialIDs;	/**< Set of currently reserved (usecnt = 0) and in-use material IDs. */
	int32 NextMaterialID;					/**< This is the next materialID that *may* be free.  It is an optimization to avoid scanning the set each time to the lowest free ID.  */

	int32 MaxSerializeTools, SerializeChildren;
	int64 SerializeTime;
	char *SerializeProgressMessage;
	void UpdateSerializeProgress();

// Operations
protected:
	void DisplayRenderTime(TimeStamp t);
	void DisplayLastRenderTime(TimeStamp t);
	bool CheckForBadValues(char *buf, bool fix);

public:
	virtual void Use();
	virtual void Recycle();

	// bool StartFlow();
	void StartFlowRange(TimeStamp start, TimeStamp length);
	void RenderInteractive(Operator *SrcOp, Operator *DestOp = NULL, uint32 flags = REQF_StampOnly|REQF_Quick|REQF_NoMotionBlur);
	bool AbortFlow();
	bool AbortFlow(bool force, bool lock = FALSE);
	bool SyncRender();
	uint32 RenderMain();
	void FrameRendered(TimeStamp time, uint32 flags);
	void RenderStart(uint32 flags);
	void RenderComplete(uint32 flags);
	void ReallyRenderComplete(uint32 flags);
	bool IsRendering()		{ return Rendering; }
	bool IsLocked()			{ return FlowLocked; }
	bool IsHiQ()				{ return HiQ; }
	bool IsMotionBlur()		{ return MotionBlur; }
	bool IsProxy()				{ return Proxy; }
	bool GetQuality()			{ return Quality; }
	bool IsRenderFlagged()	{ return DoRender; }
	bool IsBatch()				{ return (Batch > 0); }
	bool IsAborting()			{ return Aborting; }
	bool IsAutoProxy()		{ return AutoProxy; }
	bool IsAutoRenderRangeFlagged() { return m_DoAutoRenderRange; }

	void SetBatch(bool on = TRUE);
	TimeStamp GetMinFrameLength()	{ return MinFrameLength; }
	void SetHiQ(bool on);
	void SetMotionBlur(bool on);
	void SetProxy(bool on);
	void SetAutoProxy(Operator *op, bool on);
	void FlagRender()            { if (GetCurrentThreadId() != ThreadID) DoRender = TRUE; }
	void FlagRender(bool force)  { if (force || (GetCurrentThreadId() != ThreadID)) DoRender = TRUE; }
	void SetQuitWhenRendered(bool on);
	void FlagAutoRenderRange()   { if (m_AutoRenderRange) m_DoAutoRenderRange = true; }

	int32 AreAllOutputsSaved(void);	// See flags below for return value
	int32 GetNumNoUpdate(void);
	int32 GetNumPassThrough(void);
	int32 GetNumLocked(void);

	bool AddOperator(Operator *op, CPoint *pos = NULL);
	bool ConnectOps(Operator *out, Operator *in, const FuID &dt = CLSID_DataType_Image);
	bool ConnectLink(Input *inp, Output *out);

	// These can be used to bracket multiple calls to AddToFlow() to avoid order-dependant issues.
	void StartAddMultiple();
	void EndAddMultiple();

	void AddToFlow(Operator *op, bool setid = TRUE);
	void RemoveFromFlow(Operator *op);

	void InvalidateFlow();

#if !(defined(DFLIB))
	Operator *GetOperatorAt(int x, int y);
	Operator *GetOperatorAt(CPoint *pos) { return (pos ? GetOperatorAt(pos->x, pos->y) : NULL); }
	bool PutOperatorAt(Operator *op, int x, int y);
	bool PutOperatorAt(Operator *op, CPoint *pos) { return (pos ? PutOperatorAt(op, pos->x, pos->y) : FALSE); }
#endif

	void PostToAll( UINT message, WPARAM wParam = 0, LPARAM lParam = 0 );
	void SendToAll( UINT message, WPARAM wParam = 0, LPARAM lParam = 0 );
	void UpdateOperator(Operator *op);
	bool ShowPics()				{ return TilePics; }

	bool SetCurrentTime(TimeStamp time, bool render = TRUE);
	bool SetRenderRange(TimeStamp start, TimeStamp end);
	bool SetGlobalExtent(TimeStamp start, TimeStamp end);
	void UpdateAutoRenderRange();									// call this if something has changed that might
	
	TimeStamp GetRenderStart() { return RenderStartTime; }
	TimeStamp GetRenderEnd()   { return RenderEndTime;   }
	TimeStamp GetCurrentTime()	{ return CurrentTime;     }

	TimeStamp GetGlobalStart() { return GlobalExtent.Start; }
	TimeStamp GetGlobalEnd()   { return GlobalExtent.End;   }

	TimeStamp GetLastSuccessfulFrame() { return LastSuccessfulFrame; }

	Operator *GetActiveOp(CChildFrame *frame = NULL);
	void SetActiveOp(Operator *op, CChildFrame *frame = NULL);

	void GetGlobalExtent(TimeExtent &te) { te = GlobalExtent; }

	TimeStamp GetNextKeyTime(TimeStamp time, TagList *tags = NULL);
	TimeStamp GetPrevKeyTime(TimeStamp time, TagList *tags = NULL);

	void Undo();
	void Redo();
	
	UndoState *SetUndo(Object *obj);
	void StartUndo(const char *Text, TagList *tags = NULL);
	bool EndUndo(bool keep = TRUE, TagList *tags = NULL);
	char *GetUndoText() { return ""; }
	char *GetRedoText() { return ""; }

	int32 StartUndoNest(const char *Text, TagList *tags = NULL);
	bool EndUndoNest(int32 nest, bool keep = TRUE, TagList *tags = NULL);
	void CheckUndoNest(void);

	CChildFrame *GetCurrentFrame();

	bool DoAutoSave(void);
	void GetAutoSaveName(CString& Name);

	virtual void GetAvailableEvents(EventList *events, const FuID &eventid = NOID);
	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	// Doc Prefs functions

#ifndef USE_NOSCRIPTING
	const ScriptVal &GetPref(const FuID &pref);
	void SetPref(const FuID &pref, const ScriptVal &value);
#endif
	void LockPrefs();
	void UnlockPrefs();
	void ReadLockPrefs();
	void ReadUnlockPrefs();

	uint32 GetPrefLong(const FuID &pref);
	bool GetPrefBool(const FuID &pref);
	float64 GetPrefDouble(const FuID &pref);
	const char *GetPrefString(const FuID &pref);
	const char *GetPrefFilePath(const FuID &pref);
	const char *GetPrefDirPath(const FuID &pref);
	void SetPrefLong(const FuID &pref, uint32 value);
	void SetPrefBool(const FuID &pref, bool value);
	void SetPrefDouble(const FuID &pref, float64 value);
	void SetPrefString(const FuID &pref, const char *value);
	void SetPrefFilePath(const FuID &pref, const char *value);
	void SetPrefDirPath(const FuID &pref, const char *value);

#if !(defined(DFLIB))
	int MapPath(char *path);
	int MapPath(CString &path);
	int MapPath(FuString &path);
	int MapPath(FuPath &path);
	FuID GetMappedPath(const char *path);

	int ReverseMapPath(char *path);
	int ReverseMapPath(CString &path);
	int ReverseMapPath(FuString &path);
	int ReverseMapPath(FuPath &path);
	FuID GetReverseMappedPath(const char *path);

	void GetCompPathMap(ScriptVal &map, TagList *tags = NULL);
	void GetCompPathMapA(ScriptVal &map, Tag firsttag, ...);
#endif

	int32 GetRenderDepth() { return RenderDepth; }
	void SetRenderDepth(int is) { RenderDepth = is; }

	bool IsUndoing() { return Undoing; }

	void SetUsingNetwork(bool Use);
	bool UsingNetwork();
	void ReorderOperator(Operator *op);

	// From PipelineView
	void DeleteOperator(Operator *op);
	void Delete(List *list);

	bool Copy(List *list, Operator *op = NULL, CChildFrame *frame = NULL, ScriptVal *table = NULL, Tag firsttag = _TAG_DONE, ...);
	bool CopyTagList(List *list, Operator *op, CChildFrame *frame, ScriptVal *table, const TagList &tags);
	bool Copy(Operator *op, CChildFrame *frame = NULL, ScriptVal *table = NULL);
	bool Cut(List *list, Operator *op = NULL, CChildFrame *frame = NULL, ScriptVal *table = NULL, Tag firsttag = _TAG_DONE, ...);
	bool CutTagList(List *list, Operator *op, CChildFrame *frame, ScriptVal *table, const TagList &tags);
	bool Cut(Operator *op, CChildFrame *frame = NULL, ScriptVal *table = NULL);

	void Paste(Operator *op, CChildFrame *frame = NULL);
	void PasteSettings(Operator *op, CChildFrame *frame = NULL);

	bool GetMacroName(const char *filename, CString &name);

	void DetachFloatViewFrames(void);

	bool IsAutoSave();

	bool IsPlaying();
	bool IsPlayingForward();
	bool IsPlayingBackward();
	bool IsLoopPlay();
	bool IsPingPongPlay();

	void PlayFlowRange(TimeStamp start, TimeStamp end);
	void StopPlayFlow();


	void PostUpdateAllViews(CView *pSender, LPARAM lHint = 0L, CObject *pHint = NULL);

	uint32 ScriptCommand(const char *cmd, bool wait = false, bool echo = true);
	uint32 ScriptCommandF(const char *fmt, ...);
	
	uint32 PyScriptCommand(const char *cmd, bool wait = false, bool echo = true);
	uint32 PyScriptCommandF(const char *fmt, ...);

	uint32 ScriptEvent(const char *cmd, const char *args, ...);
	uint32 AsyncScriptEvent(const char *cmd, const char *args, ...);

	uint32 RunScript(const char *name, bool wait = false, bool echo = false);

	void PrefsChanged(void);

	uint32 GetRenderThreadID() { return ThreadID; }

	bool EnableCluster(Operator *op);
	void OnOperatorRenamed(Operator *op, char *oldname, char *newname);
	void OnOperatorDeleted(Operator *op, bool deleted);

	// Transport button handlers
	void TransportPlay(TagList *tags);
	void TransportStop(TagList *tags);
	void TransportBack(TagList *tags);
	void TransportRev(TagList *tags);
	void TransportHome(TagList *tags);
	void TransportCue(TagList *tags);
	void TransportEnd(TagList *tags);
	void TransportLoop(TagList *tags);

	void PrintF(enum EventConsoleSubID id, const char *fmt, ...);

	bool InitSound(char *filename = NULL, TimeStamp offset = TIME_UNDEFINED);
	void CleanupSound();
	void StartSound(bool forwards = true, TimeStamp from = TIME_UNDEFINED);
	void StopSound();

	void DisplayRenderTime(TimeStamp t, uint32 flags);
	void DisplayLastRenderTime(TimeStamp t, uint32 flags);

	void SetDFCG(bool set);

	void SetCurrentAudioFile(const char *filename, double offset = 0.0);
	const char *GetCurrentAudioFile(double *offset = NULL);

	bool BreakScript(int32 level = 100, TagList *tags = NULL);
	void CallScriptHotkey(int32 key);
	void UpdateRenderIndicator(void);

	void InitScripting();

	void ResetIsBeingLoaded();

	bool IsClipboardValid();		// call this to find out if there's something we can paste (not guaranteed to be usable)

	// ObjectID
	int32 CreateObjectID();					/**< Returns a new ObjectID.  The returned ObjectID has a usecount of 0 but is now reserved. */
	void ReserveObjectID(int32 id);		/**< Reserves the ObjectID so that CreateObjectID() will not return it.  The reserved ObjectID has a usecount of 0. If the id passed in is less than 1 it is ignored.  It is possible to reserved an ID that is already in use. */
	void UseObjectID(int32 id);			/**< Adds a use count to the ObjectID.  If the ID has not yet been reserved calling this will reserve it and assign a use count of 1.  Note that the ID could already be in use. If the id passed in is less than 1 it is ignored. */
	void RecycleObjectID(int32 id);		/**< Decrements the use count on the given ObjectID.  When the usecnt reaches 0 the ObjectID is released back to the set of available IDs. If the id passed in is less than 1 it is ignored. */
	void CheckForLeakedObjectIDs();		/**< Called during shutdown to check for leaked object IDs */
	void GetUnusedObjectIDs(std::vector<int32> &unused, int n);	/**< Returns the next n unused objectIDs, but does not reserve them.  This is used by the renderer to get IDs for objects with uninitialized IDs (ie. set to -1). */

	// MaterialID
	int32 CreateMaterialID();				/**< Returns a new MaterialID.  The returned MaterialID has a usecount of 0 but is now reserved. */
	void ReserveMaterialID(int32 id);	/**< Reserves the MaterialID so that CreateMaterialID() will not return it.  The reserved MaterialID has a usecount of 0. If the id passed in is less than 1 it is ignored.  It is possible to reserved an ID that is already in use. */
	void UseMaterialID(int32 id);			/**< Adds a use count to the MaterialID.  If the ID has not yet been reserved calling this will reserve it and assign a use count of 1.  Note that the ID could already be in use. If the id passed in is less than 1 it is ignored. */
	void RecycleMaterialID(int32 id);	/**< Decrements the use count on the given MaterialID.  When the usecnt reaches 0 the MaterialID is released back to the set of available IDs. If the id passed in is less than 1 it is ignored. */
	void CheckForLeakedMaterialIDs();	/**< Called during shutdown to check for leaked material IDs */
	void GetUnusedMaterialIDs(std::vector<int32> &unused, int n);	/**< Returns the next n unused material IDs, but does not reserve them.  This is used by the renderer to get IDs for materials with uninitialized IDs (ie. set to -1). */


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FusionDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual BOOL IsModified();
	virtual void SetModifiedFlag(BOOL bModified = TRUE);
	virtual void ReportSaveLoadException(LPCTSTR lpszPathName, CException* e, BOOL bSaving, UINT nIDPDefault);
	virtual void SetPathName( LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE );
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual BOOL SaveModified();
	protected:
	//}}AFX_VIRTUAL

public: // Argh. Stupid Protected MFC function
	BOOL DoSaveModified() { return SaveModified(); }

// Implementation
public:
	virtual ~FusionDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(FusionDoc);
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(FusionDoc)
	afx_msg void OnSave();
	afx_msg void OnSaveAs();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class RenderThread;
};

FuSYSTEM_API bool SaveBranch(Operator *op, ScriptVal &table);
FuSYSTEM_API bool SaveOperator(Operator *op, ScriptVal &table);
FuSYSTEM_API bool SaveOperatorUnique(Operator *op, ScriptVal &table, TagList *savedOps = NULL);

FuSYSTEM_API Operator *LoadOperator(const ScriptVal &table, FusionDoc *doc = NULL);

FuSYSTEM_API Operator *LoadOperatorList(const ScriptVal &table, HashList *list, FusionDoc *doc = NULL, Tag firsttag = _TAG_DONE, ...);
FuSYSTEM_API Operator *LoadOperatorListTagList(const ScriptVal &table, HashList *list, FusionDoc *doc, const TagList &tags);

FuSYSTEM_API Operator *FindVisibleOperator(Operator *op);
FuSYSTEM_API bool SkipObject(const ScriptVal &table, const TagList &tags, uint32 currdata = 0);
FuSYSTEM_API bool SkipObject(char *&ptr, const char *bufend, TagList *tags = NULL);

FuSYSTEM_API Object *CreateObj(uint32 type, const char *name, const ScriptVal &table, const TagList &tags);
FuSYSTEM_API Object *CreateObj(const Registry *reg, const ScriptVal &table, const TagList &tags);

FuSYSTEM_API bool LoadFile(const char *filename, ScriptVal &table);
FuSYSTEM_API bool LoadFile(FILE *file, ScriptVal &table);
FuSYSTEM_API bool LoadBuffer(const char *buf, ScriptVal &table);

#define FUSIONDOC_BASE OBJECT_BASE + TAGOFFSET

enum FusionDoc_Enum  ENUM_TAGS
{
	// tags for Copy/CopyTagList and Cut/CutTagList
	DOC_Copy_IsMacro = FUSIONDOC_BASE + TAG_INT,
	DOC_Compress,						// Default is based on prefs

	// Used by LoadOperatorList
	DOC_LoadList_Flat,				// All sub-tools (ie. in groups, etc.) are put in the returned
	DOC_LoadList_ConnectInputs,
	DOC_LoadList_AddToFlow,
	DOC_LoadList_FakeAddToFlow,

	DOCP_OperatorList = FUSIONDOC_BASE + TAG_PTR,
	DOCP_Load_NoWarnings,			// hint used when loading operators
};


// Update hints from the Document
enum DocHints
{
	DCMSG_UPDATE_TILE,
	DCMSG_TIME_CHANGED,
	DCMSG_RENDERRANGE_CHANGED,
	DCMSG_FLOW_SIZE_CHANGED,
	DCMSG_GLOBALEXTENT_CHANGED,
	DCMSG_SETTINGS_CHANGED,					// All other settings 
	DCMSG_TIMECODE_CHANGED,
	DCMSG_LOOKPACK_CHANGED,
	DCMSG_PREFS_CHANGED,
	DCMSG_KEY_POINTS_CHANGED,
};


struct UpdateAllViewsData
{
	CView   *pSender;
	CObject *pHint;
	uint32   lHint;
};

/////////////////////////////////////////////////////////////////////////////

// Console History

struct ConsoleMessage
{
	ConsoleMessage *m_Next;

	EventConsoleSubID m_ID;
	uint32 m_TimeStamp;
	uint32 m_Style;
	uint32 m_Sequence;

	char m_String[1];
};

/////////////////////////////////////////////////////////////////////////////

#else // DFMANAGER

// Fake FusionDoc class to avoid prefs nightmares
class FuSYSTEM_API FusionDoc
{
public:
	// Doc Prefs functions
	const ScriptVal &GetPref(const FuID &pref) { return ::GetPref(pref); }
	void SetPref(const FuID &pref, const ScriptVal &value) { ::SetPref(pref, value); }

	uint32 GetPrefLong(const FuID &pref) { return ::GetPrefLong(pref); }
	bool GetPrefBool(const FuID &pref) { return ::GetPrefBool(pref); }
	float64 GetPrefDouble(const FuID &pref) { return ::GetPrefDouble(pref); }
	const char *GetPrefString(const FuID &pref) { return ::GetPrefString(pref); }
	const char *GetPrefFilePath(const FuID &pref) { return ::GetPrefFilePath(pref); }
	const char *GetPrefDirPath(const FuID &pref) { return ::GetPrefDirPath(pref); }
	void SetPrefLong(const FuID &pref, uint32 value) { ::SetPrefLong(pref, value); }
	void SetPrefBool(const FuID &pref, bool value) { ::SetPrefBool(pref, value); }
	void SetPrefDouble(const FuID &pref, float64 value) { ::SetPrefDouble(pref, value); }
	void SetPrefString(const FuID &pref, const char *value) { ::SetPrefString(pref, value); }
	void SetPrefFilePath(const FuID &pref, const char *value) { ::SetPrefFilePath(pref, value); }
	void SetPrefDirPath(const FuID &pref, const char *value) { ::SetPrefDirPath(pref, value); }

	void PrintF(int id, const char *fmt, ...);
};
#endif // DFMANAGER

// Used as return values from AreAllOutputsSaved()

enum AllOutputsSaved
{
	DOC_AllOutputsSaved = 0,
	DOC_SomeOutputsSaved,
	DOC_NoOutputsSaved,
};

// DOC_EXECUTE stuff
typedef void (*DOCEXECHookPtr)(void *data);
extern FuSYSTEM_API void DocExecute(DOCEXECHookPtr func, void *data, bool sync = FALSE);
extern FuSYSTEM_API void DocExecuteDelay(DOCEXECHookPtr func, void *data, uint32 delay);


// These are messages posted from non-GUI threads, eg the renderer, the operators
// Views receive these, and if appropriate, update whatever they show.
enum DocMessages
{
	DOC_UPDATE_TILE = WM_USER + 0x1000, // 5120 - LPARAM is Op ptr
	DOC_SHOW_NEW_TIME,                  // 5121 - LPARAM is new time ptr
	DOC_UPDATE_VALUE,                   // 5122 - LPARAM is IC ptr, WPARAM is Input ptr
	DOC_START_RENDER,                   // 5123 - LPARAM is flags for render
	DOC_STOP_RENDER,                    // 5124 -
	DOC_ACTIVE_OP,                      // 5125 - LPARAM is Op ptr, WPARAM is frame
	DOC_TOOLBAROP_DRAGGING,	            // 5126 - WPARAM is point, LPARAM is &regnode
	DOC_TOOLBAROP_DROPPED,              // 5127 - WPARAM is point, LPARAM is &OperatorArrayEntry
	DOC_GLOBALEXTENT_CHANGED,           // 5128 - LPARAM is TimeExtent ptr
	DOC_REMOVE_CONTROLS,                // 5129 - LPARAM is Operator ptr
	DOC_ADD_CONTROLS,                   // 5130 - LPARAM is Operator ptr
	DOC_CHECK_HASP,                     // 5131 - LPARAM is ptr to buffer with codes
	DOC_OPERATOR_DRAW,                  // 5132 - LPARAM is Operator, WPARAM is CDC of the view that caused the change, or NULL
	DOC_SPLINE_DRAW,                    // 5133 - LPARAM is Spline, WPARAM is CDC of the view that caused the change, or NULL
	DOC_SPLINE_SELECTED,                // 5134 - LPARAM is Spline, WPARAM is Operator to be selected
	DOC_SPLINE_REMOVED,                 // 5135 - LPARAM is Spline, WPARAM is view from which the Spline is selected
	DOC_POINT_DISPLAY,                  // 5136 - LPARAM is Spline, WPARAM is KeyPoint to be displaied
	DOC_POINT_DISPLAY_RESET,            // 5137 - LPARAM is Spline
	DOC_NEW_FRAME_LENGTH,               // 5138 - 
	DOC_PREVIEW_ADDED,                  // 5139 - LPARAM is Preview, WPARAM is operator/NULL
	DOC_PREVIEW_REMOVED,                // 5140 - LPARAM is Preview, WPARAM is operator/NULL
	DOC_OP_ADDED,                       // 5141 - LPARAM is Operator ptr. LPARAM==NULL => end of adding Op.
	DOC_OP_REMOVED,                     // 5142 - LPARAM is Operator ptr. LPARAM==NULL => end of removing Op.
	DOC_OP_DRAGGING,                    // 5143 - WPARAM is point, LPARAM is &operator
	DOC_OP_DROPPED,                     // 5144 - WPARAM is point, LPARAM is &operator
	DOC_OP_RENAMED,                     // 5145 - LPARAM is &operator
	DOC_EXECUTE,                        // 5146 - Post this to TheApp: WPARAM is DOCEXECHookPtr, LPARAM is data (passed as arg)
	DOC_PVIEW_DRAGGING,                 // 5147 - LPARAM is the co-ords (in client space);
	DOC_SETCOLOR,                       // 5148 - WPARAM is InputList ptr
	DOC_GETCOLOR,                       // 5149 - WPARAM is InputList ptr, LPARAM is the co-ords (in client space)
	DOC_RQ_LOADDOC,                     // 5150 - WPARAM is item number to load, LPARAM is previous doc (if any)

	DOC_CBOX_DRAW,                      // 5151 - LPARAM is ControlBox, WPARAM = NULL
	DOC_CBOX_RESET,                     // 5152 - LPARAM == NULL, WPARAM is view from which message is sent to all the other views.
	DOC_RENDER_AND_QUIT,                // 5153 - WPARAM is start, LPARAM is end.
	DOC_DEPENDANTS_UPDATE,              // 5154 - LPARAM == affected spline, WPARAM == view to be excepted from updating.

	DOC_CONTROLWND_UPDATE,              // 5155 - 
	DOC_RESET_VIEW,                     // 5156 - LPARAM == NULL, WPARAM == view to be excluded (that sent a message)
	DOC_EDITFOCUS_LOST,                 // 5157 - LPARAM == PreviewControl whose draw mode been changed, WPARAM == NULL.
	DOC_SPLINE_COUNT,                   // 5158 - WPARAM == total spline count, LPARAM == Aded/Removed Operator

	DOC_MESSAGEBOX,                     // 5159 - LPARAM is pointer to string, and must not go away. WPARAM is HWND of messagebox parent.
	DOC_RENDERTEXT,                     // 5160 - WPARAM is pointer to text string, LPARAM is (optional) text resource ID instead of WPARAM

	DOC_POINT_UPDATE,                   // 5161 - LPARAM is Spline WPARAM is selected point.
	DOC_EDIT_ENABLED,                   // 5162 - LPARAM is Op ptr, WPARAM is frame

	DOC_LINK_ADDED,                     // 5163 - WPARAM is Input ptr, LPARAM is new Output ptr
	DOC_LINK_REMOVED,                   // 5164 - WPARAM is Input ptr, LPARAM is old Output ptr
	DOC_EDITFOCUS_GAINED,               // 5165 - LPARAM == PreviewControl whose edit focus been changed, WPARAM == NULL.

	DOC_OP_REPLACED,                    // 5166 - WPARAM is Operator to be replaced, LPARAM is Operator to replace with
	DOC_ADD_OP,                         // 5167 - LPARAM is Operator to be added, WPARAM is flow position (in gridview style grid positions - NOT client co-ords).

	DOC_TIMELINEPREFS_CHANGED,          // 5168 - LPARAM is bits of changed prefs (uint32)
	DOC_PREVIEW_CLOSED,                 // 5169 - WPARAM is NULL, LPARAM is preview

	DOC_UPDATE_ALL_VIEWS,               // 5170 - WPARAM is doc *, LPARAM is UpdateAllViewsData *

	DOC_START_PLAY,                     // 5171 - 
	DOC_STOP_PLAY,                      // 5172 - 

	DOC_CUSTOM_BACKGROUND,              // 5173 - WPARAM is the custom button wanting to know its background DIB. return a ptr to a DIB/ResDIB
	DOC_CUSTOM_BORDER_INNER,            // 5174 - WPARAM is the window asking for border colors. LPARAM is a pointer to a ResColor pointer.
	DOC_CUSTOM_BORDER_OUTER,            // 5175 - WPARAM is the window asking for border colors. LPARAM is a pointer to a ResColor pointer.

	XCP_CLEANUP_FLOWINFO,               // 5176 - for external control ports (TCP/IP or other)

	DOC_REFRESH_OP,                     // 5177 - LPARAM is Op ptr - used to regenerate PreviewView toolbars, PreviewControls etc.
	DOC_SHOW_NEW_KEYTIME,               // 5178 - WPARAM is NULL, LPARAM is TRUE(forward)/FALSE(backward).
	DOC_UPDATE_DLG_DATA,                // 5179 - WPARAM is NULL, LPARAM is NULL
	DOC_HANDLE_EVENT,                   // 5180 - WPARAM is Object *, LPARAM is EventData *
	DOC_OP_SELECTED,                    // 5181 - LPARAM is Op ptr (or NULL if multiple), WPARAM is view to be excluded (that sent the message), if relevant

	DOC_POST_MESSAGE,                   // 5182 - 

	DOC_EXECUTE_DELAY,                  // 5183 - 

	DOC_GET_INPUT_AT,                   // 5184 - 
	DOC_DRAG_RESULT,                    // 5185 - 

	DOC_SPLINE_DRAW_WITH,               // 5186 - LPARAM is ptr to a OpInfo, WPARAM - CDC to draw with
	DOC_SPLINE_DISPLAY,                 // 5187 - LPARAM is ptr to a Op, WPARAM == NULL
	
	DOC_UPDATE_BINITEM,                 // 5188 -
	DOC_SET_EXPRESSION,						// 5189 - LPARAM is Input ptr, WPARAM == NULL
	DOC_UPDATE_NON_SPLINE,					// 5190 - LPARAM is modifier ptr, WPARAM == NULL

	DOC_DRAG_HOVER,                     // 5191 - LPARAM is mouse pos 

	DOC_INSTANCE_CHANGED,					// 5192 - LPARAM is op whose InstanceParent has changed, WPARAM is old InstanceParent

	DOC_DRAG_START,                     // 5193 - 
};

class PostMessageData : public Object
{
	FuDeclareClass(PostMessageData, Object); // declare ClassID static member, etc.

public:
	FusionDoc *m_Doc;
	UINT m_Message;
	WPARAM m_wParam;
	LPARAM m_lParam;

	PostMessageData(FusionDoc *doc, UINT message, WPARAM wParam = 0, LPARAM lParam = 0) { m_Doc = doc; m_Message = message; m_wParam = wParam; m_lParam = lParam; }
};

#ifndef DFLIB
#include "FuPath.h"

class FuSYSTEM_API FuPathMap : public FuPath
{
public:
	FuPathMap(const char *path, FusionDoc *doc);
};
#endif

#endif
