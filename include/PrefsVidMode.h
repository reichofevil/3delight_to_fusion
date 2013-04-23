#ifndef _PREFS_VIDMODE_H_
#define _PREFS_VIDMODE_H_

#include "types.h"
#include "List.h"
#include "PrefsPage.h"

// PrefsVidMode.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PrefsVidMode dialog

class ModeSpec : public Object
{
	FuDeclareClass(ModeSpec, Object); // declare ClassID static member, etc.

public:
	char		Name[80];
	uint32	Width, Height;
	bool		HasFields;
	float64	FrameRate;
	float64	AspectX, AspectY;
	float64	FramesPerFeet;
	float64  GuideX1, GuideY1, GuideX2, GuideY2;
	float64  GuideRatio;
};

class FuSYSTEM_API PrefsVidMode : public PrefsPage
{
	FuDeclareClass(PrefsVidMode, PrefsPage); // declare ClassID static member, etc.

// Construction
public:
	PrefsVidMode(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~PrefsVidMode();

// Dialog Data
	//{{AFX_DATA(PrefsVidMode)
	enum { IDD = IDD_PREF_VIDMODE };
	DWORD	DefHeight;
	DWORD	DefWidth;
	bool	HasFields;
	float64	FrameRate;
	float64 AspectX;
	float64 AspectY;
	CString	ModeName;
	int FullDepth;
	int PreviewDepth;
	int InteractiveDepth;
	int DropFrame;
	float64 GuideX1;
	float64 GuideY1;
	float64 GuideX2;
	float64 GuideY2;
	float64 GuideRatio;
	bool DepthLock;
	//}}AFX_DATA

#ifndef DFLIB
	CustomButton	m_New;
	CustomButton	m_Del;
	CustomButton	m_Copy;
	CustomCheckBox	m_HasFieldsCheck;
	CustomCheckBox	m_DepthLock;
	DFComboBox	m_ModeCombo;
	DFComboBox	m_PreviewDepthCombo;
	DFComboBox	m_InteractiveDepthCombo;
	DFComboBox	m_FullDepthCombo;
	TimeCodeEdit	m_GuideY2Edit;
	TimeCodeEdit	m_GuideY1Edit;
	TimeCodeEdit	m_GuideX2Edit;
	TimeCodeEdit	m_GuideX1Edit;
	TimeCodeEdit	m_GuideRatioEdit;
	TimeCodeEdit	m_AspectYEdit;
	TimeCodeEdit	m_AspectXEdit;
	TimeCodeEdit	m_FramesPerFeetEdit;
	TimeCodeEdit	m_FrameRateEdit;
	TimeCodeEdit	m_HeightEdit;
	TimeCodeEdit	m_WidthEdit;
	DFSpinButtonCtrl	m_FrameRateSpin;
	DFSpinButtonCtrl	m_FramesPerFeetSpin;
	CustomButton	m_DepthGroup;
	CustomButton	m_SettingsGroup;
#endif

	int TimeCodeType;
	float32 FramesPerFeet;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PrefsVidMode)
	public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnKillActive();
	virtual BOOL OnInitDialog();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	ModeSpec *GetNthMode (int32 n);		// Not very efficient - but it'll do!
	bool SetModeFromValues(int32 w, int32 h, float64 xscale, float64 yscale, ModeSpec **ms = NULL);

	// Pass 0.0 for xscale and/or yscale to match any value
	// Pass retms to get matching values
	// Pass doc to set the doc's prefs accordingly
	static bool SetModeFromValues(int32 w, int32 h, float64 xscale, float64 yscale, ModeSpec **retms, FusionDoc *doc);

	bool AllowFloat;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(PrefsVidMode)
	afx_msg void OnSelchangeDepthFull();
	afx_msg void OnSelchangeDepthPreview();
	afx_msg void OnSelchangeDepthInteractive();
	afx_msg void OnSelchangeModeList();
	afx_msg void OnDeleteVidMode();
	afx_msg void OnNewVidMode();
	afx_msg void OnCopyVidMode();
	afx_msg void OnDeltaposFpsSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnControlClicked(UINT nID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void ModeToControls(ModeSpec *ms);
	bool SaveModeList();
	void AbortAllFlows();
	void SetAllPrefs();

	ModeSpec *CurrentMode;
	int ModeNum;
	List ModeList;
};

FuSYSTEM_API extern PrefsVidMode *PVidMode;

#endif
