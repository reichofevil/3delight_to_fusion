#ifndef _TIMECODEEDIT_H_
#define _TIMECODEEDIT_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if !defined(unix) && !defined(DFLIB)
#include "DFScrollBar.h"
#endif

#include "ResData.h"

#define TCFLG_FLDS				0x00000001		// Include fields in the result
#define TCFLG_FRMS				0x00000002		// Include frames in the result
#define TCFLG_SECS				0x00000004		// Include seconds in the result
#define TCFLG_MINS				0x00000008		// Include minutes in the result
#define TCFLG_HRS					0x00000010		// Include hours in the result
#define TCFLG_ALL					0x0000001F

#define TCFMT_PREFS				0					// Format the value to what the user's prefs are
#define TCFMT_FRAME				1					// Force the output to frames
#define TCFMT_SMPTE				2					// Force the output to SMPTE TimeCode format
#define TCFMT_FEET				3					// Force the output to Feet+Frame

#define TCDRP_PREFS				0					// Use the prefs to decide on Drop-Frame
#define TCDRP_OFF					1					// Force off drop-frame
#define TCDRP_ON					2					// Force on drop-frame

#define TCRATE_DEFAULT			1000000000

#define TCMSGF_DISABLED			0x00000001
#define TCMSGF_NOFORCEUPDATE	0x00000002

enum TimeCodeEdit_Messages
{
	TCMSG_UPDATE_DISPLAY = WM_USER + 100,
};

class FusionDoc;

class FuSYSTEM_API TimeCodeEdit : public CEdit
{
	FuDeclareClass(TimeCodeEdit, CEdit); // declare ClassID static member, etc.

	DECLARE_DYNCREATE(TimeCodeEdit)

	// Construction
public:
	TimeCodeEdit();

// Attributes
public:

	bool TimeType;
	bool Integer;
	bool Disabled;
	bool FPSSet;
	bool IsDropFrame;
	bool DropFrameSet;

	double FPS;

	bool LimitSet;

	char *MessageText;
	bool Hover;

	ResPosition Position; // position, last set by CreateControl()

	char StupidBloodyPersistentCharacterStringThatsNotGoingToGoAwayWhenWM_SETTEXTIsPosted[1024];
	uint32 CreateThreadId;

#if !defined(unix) && !defined(DFLIB)
	DFScrollViewObject svo;
#endif

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TimeCodeEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void CreateControl(ResPosition &position, HWND hWndParent, HMENU nIDorHMenu, HFONT font = NULL); // Create as mostly used in controls
	virtual ~TimeCodeEdit();

	void SetDisabled(bool IsDisabled);
	void SetInteger(bool IsInteger);
	void SetTimeType(bool IsTimeType);
	void SetValue(double Val, int32 Precision, FusionDoc *doc = NULL);
	double GetValue(double PrevVal, FusionDoc *doc = NULL);
	void SetFPS(double fps);
	void UnSetFPS(void);
	double GetFPS(void);
	void SetDropFrame(bool iSDropFrame);
	void UnSetDropFrame();
	bool GetDropFrame();
	void SetMessageText(char *message);
	bool IsExpression();
	bool GetExpression(char *buf, int32 size);

	// Value       = frame number/time value to be converted
	// String      = pointer to char array - never needs to be > 20 chars
	// IsInteger   = Force to integer/frame boundary
	// FieldFlags  = Flags to indicate what fields should be placed in the output (see flags above)
	// AllRequired = All fields, even if not needed should be in output
	// DisplayedPrecision = Number of digits after the decimal point (if not formatted to timecode)
	// Format      = Format the output to TimeCode format, even if prefs says otherwise
	// ForceFields = Force fields into the output, even if prefs/video mode says otherwise
	// DropFrame   = Can specify what to do about drop-frame
	// FrameRate   = FrameRate to do calculations at
	static void FormatTimeCode(TimeStamp Value, char *String,
		bool IsInteger = FALSE, uint32 DisplayedPrecision = 5, uint32 FieldFlags = TCFLG_ALL, bool AllRequired = FALSE,
		uint32 Format = TCFMT_PREFS, bool ForceFields = FALSE, uint32 DropFrame = TCDRP_PREFS, double FrameRate = TCRATE_DEFAULT, FusionDoc *doc = NULL);

	static TimeStamp ParseTimeCode(const char *String, TimeStamp DefValue,
		bool IsInteger = FALSE, uint32 DropFrame = TCDRP_PREFS, double FrameRate = TCRATE_DEFAULT, uint32 DefFormat = TCFMT_PREFS, FusionDoc *doc = NULL);

	static void SetValueToString(char *Str, double Val, int32 Precision, FusionDoc *doc = NULL, bool IsInteger = FALSE, bool TimeType = FALSE,
		bool IsDropFrame = FALSE, bool DropFrameSet = FALSE, bool FPSSet = FALSE, double FPS = 0.0);
	
	// Generated message map functions
protected:
	//{{AFX_MSG(TimeCodeEdit)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg NCHITTEST_RESULT OnNcHitTest(CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

// TransportEdit control
class FuSYSTEM_API TransportEdit : public TimeCodeEdit
{
	FuDeclareClass(TransportEdit, TimeCodeEdit); // declare ClassID static member, etc.

	DECLARE_DYNCREATE(TransportEdit)

public:
	TransportEdit():TimeCodeEdit() { TCTypeSet = FALSE; PadTCSet = FALSE; doc = NULL; }

	double GetValue(double PrevVal);
	void SetValue(double Val, int32 Precision);

	void SetTCType(int tctype);
	int GetTCType();
	void UnSetTCType();

	void SetPadTC(bool pad);
	bool GetPadTC();
	void UnSetPadTC();

	uint16 id;
	int TCType;
	bool PadTC, TCTypeSet, PadTCSet;

	FusionDoc *doc;

protected:
	//{{AFX_MSG(TransportEdit)
	afx_msg void OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnKillFocus( CWnd* );
	afx_msg void OnSetFocus( CWnd* );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // _TIMECODEEDIT_H_
