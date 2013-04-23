#ifndef _PIPELINE_H_
#define _PIPELINE_H_

#if !defined(__AFXWIN_H__) && !defined(NO_MFC)
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "HashList.h"
#include "TagList.h"
#include "DFScript.h"
//#include "version.h"

/////////////////////////////////////////////////////////////////////////////
// FusionApp:
// See Pipeline.cpp for the implementation of this class
//

struct MemoryInfo
{
	uint64 m_PhysicalTotal;
	uint64 m_PhysicalAvail;
	uint64 m_PageFileTotal;
	uint64 m_PageFileAvail;
	uint64 m_VirtualTotal;
	uint64 m_VirtualAvail;
	uint64 m_VirtualExtendedAvail;
};

enum PipelineAttrs ENUM_TAGS
{
	// Type of DF
	PA_IsPost = TAG_INT,
	PA_IsEngine,
	PA_IsNTSC,
	PA_IsNFR,
	PA_IsEducational,
	PA_IsDemo,
	PA_IsMaya,
	PA_IsManager,
	PA_IsService,
	PA_IsConsole,

	// CPU/OS related capabilities
	PA_IsMMX = TAG_INT + TAGOFFSET,
	PA_IsSSE,
	PA_IsSSE2,
	PA_IsSSE3,
	PA_IsFTZ,
	PA_IsDAZ,

	// Licensing info
	PA_IsNetworkLicense = TAG_INT + TAGOFFSET + TAGOFFSET,
	PA_IsRealityLicense,
	PA_IsQuattrusLicense,
	PA_IsToasterLicense,
	PA_IsLightWaveLicense,
	PA_IsAltitudeLicense,

	PA_NetworkServer = TAG_STRING | TAG_MULTI,
	PA_LicensePrefix,
};

#if !defined(NO_MFC) && (!defined(DFLIB) || defined(WINCE))
class FusionApp : public CWinApp, public Object
#else
class FusionApp : public Object
#endif
{
protected:
	List *DllList;

	bool Batch;

public:
#ifndef _DEBUG
	long *OldData;
#endif
	int32 NumProcessors;
	uint32 Flags;

	CString ExecutableDir, HelpPath;
	char *oldHelpPath;
	char SerialNumber[64];

	HashList GlobalPreviewList;

#if !defined(NO_MFC)
	CMultiDocTemplate *FlowTemplate;
	CMultiDocTemplate *DFCGTemplate;
#endif

#if !defined(WINCE)
	HELPINFO HelpInfo;
#endif
	HINSTANCE GraphicsInstanceHandle;

	HashList RecentSettingsList;

	uint32 SerNum;
	uint8  SerFlags;
	uint8  SerCheck;
	uint8  Pad[2];

	uint16 AppVersion[4];

	TagList Attrs;					// So many things are going here, it'd just be easier if they were all in tags - so its defined here, just nothing uses it yet.

	HCURSOR HCS_Drag, HCS_DragLeft, HCS_DragRight;

protected:
	uint32 LastIdle;
	bool ForceIdle;

	bool Loading;

	FusionDoc *m_CurrentDoc;

	List m_IdleList;

public:
	uint32 m_WinVerMajor, m_WinVerMinor;
	uint32 m_PageSize;

	HCURSOR Arrow, StdArrow, WaitArrow;

	bool m_DoLogging;
	CString m_LogName;

	bool m_QuietLicense;

#ifndef USE_NOSCRIPTING
	ScriptVal m_OperatorClipBoard;
	ScriptVal m_ParameterClipBoard;
#endif

	bool m_DoNetLog, m_bVerboseLog;

	char SubscriptionVersion[64];

	const char *m_pUIFontName;
	HFONT m_hUIFont, m_hUIBoldFont;

	ScriptVal m_ExtraArgs;

public:
	FusionApp();
	virtual ~FusionApp();

	bool GetVersion(const char *name, uint32 &vhi, uint32 &vlo, char *querystr = NULL);
	bool CheckVersion(const char *fullpath, bool checkproduct = true, bool quiet = false);

	virtual void AddToSettingsList(const FuID &OpID, const char *FileName);

	virtual void SetBatch(bool batch);
	virtual bool GetBatch(void);

	virtual void SaveRecentFileList(void);

	virtual UINT Message(UINT type, char *format, ...);
	virtual void Message(char *format, ...);

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	virtual bool ShowHTMLHelp(const char *topic = NULL);
	virtual void WalrusMsg(char *msg);

	virtual bool IsFeaturePossible(uint32 code1, uint32 code2, char *feature, char *version = NULL, char *licensepath = NULL);
	virtual bool CheckOutFeature(uint32 code1, uint32 code2, char *feature, char *version = NULL, char *licensepath = NULL);
	virtual bool CheckInFeature(uint32 code1, uint32 code2, char *feature, char *version = NULL, char *licensepath = NULL);

	virtual bool IsLoading(void);

	virtual HANDLE AddIdleEvent(void);
	virtual void RemoveIdleEvent(HANDLE event);
	virtual void SetIdle(void);

	virtual bool CopyAttr(Tag tag, void *addr) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FusionApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual LRESULT ProcessWndProcException(CException* e, const MSG* pMsg);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
#if !defined(WINCE)
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
#endif
	virtual CDocument *OpenDocumentFile(LPCTSTR lpszFileName);
	virtual void AddToRecentFileList(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

	virtual UINT TitleMessage(LPCTSTR title, UINT type, char *format, ...);

// Implementation
protected:
	HINSTANCE LoadSupportDLL(const char *name);									// name only (no dir, no extension)
	void LoadAllPlugins();
	void LoadPluginDir(const char *dirname, const char *ext = ".plugin");	// full path to dir, with trailing '\'
	HINSTANCE LoadPlugin(const char *filename, bool silentfail = false);	// full path
	void FreeAllDLLs(void);
	void ScanNSetReg(HINSTANCE hIns, const char *filename);					// full path
	void RegisterSystemSounds();
	bool LoadLookPacks();
	void LoadLookDir(const char *dirname, const char *ext);
	bool DoLicensing();
	void CleanupLicensing();
	bool CheckCorruptDongle();
	bool CheckRegistration();

	//{{AFX_MSG(FusionApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	afx_msg void OnUpdateRecentFileMenu(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	// Bloody protected members!
	virtual void DoFileOpen();

	virtual void SetCurrentDoc(FusionDoc *doc);
	virtual FusionDoc *GetCurrentDoc();

	virtual void GetMemoryInfo(MemoryInfo &mi);

	virtual CString GetOnlineHelpApp(void);

	virtual int DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);

	virtual uint32 MessageTagList(const TagList &tags, const char *contentformat = NULL, ...) const;

	virtual CString GetFeatureError(const char *feature, const char *version = NULL);

	virtual bool ShowHTMLHelp(const char *topic, bool forceOnline);
};

FuSYSTEM_API FusionApp *GetApp();

extern FusionApp theApp;

// This is a global pointer, accessible from any thread.
extern FuSYSTEM_API FusionApp *App;

// MessageBox functions that can be called before the app's MainFrame is around
enum AppMessage_Attrs
{
	AppMsg_Type = TAG_INT,
	AppMsg_ForceAsync,

	AppMsg_Option_DoNotShow,

	AppMsgS_Title = TAG_STRING,

	AppMsgS_MainText,
	AppMsgS_ContentText,
	AppMsgS_FootnoteText,

	AppMsgS_AddDefButton = TAG_STRING | TAG_MULTI,
	AppMsgS_AddButton,

	AppMsgP_IconReg = TAG_PTR,		// Used for resource IDs. If NULL, IDs are system IDs.

	AppMsgP_MainIcon,					// Resource ID
	AppMsgP_ContentIcon,				// Resource ID
	AppMsgP_FootnoteIcon,			// Resource ID

	AppMsgP_Parent,					// HWND
};

#define APPMSG_FLAG_DONOTSHOW		(1ul<<31)

extern uint32 Pipeline_MessageTagList(const TagList &tags, const char *contentformat = NULL, ...);

extern UINT Pipeline_Message(char *title, UINT type, char *format, ...);
extern UINT Pipeline_Message(UINT type, char *format, ...);
extern void Pipeline_Message(char *format, ...);


#endif // _PIPELINE_H_
