#ifndef _CUSTOMBUTTON_H_
#define _CUSTOMBUTTON_H_

#include "types.h"

class InputControl;
class ResBitmap;
class ResColor;
class DIB;

//#define USE_TAHOMA

#ifndef WM_CHANGEUISTATE
#define WM_CHANGEUISTATE                0x0127
#define WM_UPDATEUISTATE                0x0128
#define WM_QUERYUISTATE                 0x0129

#define UIS_SET                         1
#define UIS_CLEAR                       2
#define UIS_INITIALIZE                  3

#define UISF_HIDEFOCUS                  0x1
#define UISF_HIDEACCEL                  0x2
#endif

#define CSTBN_GADGETDOWN		0x5000

enum CustomButton_LabelPositions
{
	CBLP_Bottom,
	CBLP_Right,
	CBLP_Left
};

enum CustomButton_LabelAlignment
{
	CBLA_Center,
	CBLA_Left,
	CBLA_Right
};

#define DIB_BACK		0
#define DIB_NORM		1
#define DIB_PRESS		2
#define DIB_SEL		3
#define DIB_DIS		4
#define DIB_PRESSDIS	5
#define DIB_PUSH		6
#define DIB_HOVER		7
#define DIB_MAX		8

#define SHOW_BASIC_BUTTON		0x01
#define SHOW_ICON					0x02
#define SHOW_LABEL				0x04
#define SHOW_ELLIPSIS			0x08

enum CustomButton_Messages
{
	CBMSG_SET_TEXTA = WM_USER + 100,		// Can be used with PostMessage(), unlike WM_SETTEXT
	CBMSG_SET_TEXTW,
};

class FuSYSTEM_API CustomButton : public CButton
{
//	FuDeclareClass(CustomButton); // declare ClassID static member, etc.

// Construction
public:
	CustomButton();
	DECLARE_DYNCREATE(CustomButton)

protected:
	WCHAR *Label, *ToolTip;
	uint32 LabelLength, ToolTipLength;
	CSize LabelSize;
	int LabelPos, LabelAlign;

	CSize IconSize;
	HFONT Font;

	bool IsDisabled, ButtonDown, Hover, HoverDown;
	bool AllowIconScaling, PushType;
	int32 Check;

public:
	bool Selected, Pushed;

	DIB *ColdDIB, *HotDIB;
	DIB *DIBs[3][DIB_MAX];
	DIB *ModifyDIB;

	ResColor *NormCol, *SelCol, *DisCol, *PushCol;

#ifdef unix
	bool isUnicode;
#endif

protected:
	uint32 ShowFlags;

	float coldH, coldL, coldS, coldO;
	char pad[34];

	uint32 realStyle;
	bool ignoreMouse, ColdFromHot;

	void SetDIBs(int32 index, DIB *back, DIB *normal, DIB *selected, DIB *disabled,
		DIB *pressed, DIB *presseddisabled, DIB *pushed, DIB *hover);
	void MakeColdFromHot(float coldDIBH = 0.0f, float coldDIBL = 1.0f, float coldDIBS = 0.0f, float coldDIBOpacity = 0.75f);

public:
	bool DoShowIcon();

	void SetLabel(LPCSTR label);
	void SetToolTip(LPCSTR tooltip);
	virtual void SetColdDIB(DIB *cold);
	virtual void SetHotDIB(DIB *hot, float coldDIBH = 0.0f, float coldDIBL = 1.0f, float coldDIBS = 0.0f, float coldDIBOpacity = 0.75f);
	virtual void SetVisibility(bool showBasicButton, bool showIcon, bool showLabel, bool showToolTip, int labelPos = CBLP_Bottom, int labelAlign = CBLA_Center);

	void ShowEllipsis(bool showEllipsis);

	void SetDisabled(bool disabled) { IsDisabled = disabled; }
	void SetAllowScaling(bool allow) { AllowIconScaling = allow; }

	void SetPushType(bool push) { PushType = push; }
	void SetPushed(bool pushed) { Pushed = pushed; }

	void SizeToContent (void);

	void SetBasicDIBs(DIB *back = NULL, DIB *normal = NULL, DIB *selected = NULL, DIB *disabled = NULL, DIB *pressed = NULL, DIB *presseddisabled = NULL);

	void SetBasicDIBs(DIB *back, DIB *normal, DIB *selected, DIB *disabled,
		DIB *pressed, DIB *presseddisabled, DIB *pushed, DIB *hover = NULL);
	void SetSmallDIBs(DIB *back = NULL, DIB *normal = NULL, DIB *selected = NULL, DIB *disabled = NULL,
		DIB *pressed = NULL, DIB *presseddisabled = NULL, DIB *pushed = NULL, DIB *hover = NULL);
	void SetTinyDIBs(DIB *back = NULL, DIB *normal = NULL, DIB *selected = NULL, DIB *disabled = NULL,
		DIB *pressed = NULL, DIB *presseddisabled = NULL, DIB *pushed = NULL, DIB *hover = NULL);

	void SetAllDIBs(DIB *back = NULL, DIB *normal = NULL, DIB *selected = NULL, DIB *disabled = NULL,
		DIB *pressed = NULL, DIB *presseddisabled = NULL, DIB *pushed = NULL, DIB *hover = NULL);

	void SetBasicCols(ResColor *norm = NULL, ResColor *sel = NULL, ResColor *dis = NULL, ResColor *push = NULL);
	void SetModifyDIB(DIB *modify = NULL);

	BOOL Create( LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CustomButton)
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:
	virtual void DrawItemImage_BasicButton(DIB *tmp, DIB *button, DIB *hover = NULL, int32 backW = -1, int32 backH = -1);
	virtual void DrawItemImage_Icon(DIB *back, DIB *icon, DIB *hoverButton, int32 backW = -1, int32 backH = -1, bool useBorder = TRUE);
	virtual void DrawItemImage_Label(HDC dc, DRAWITEMSTRUCT *lpDrawItemStruct);

public:
	virtual void DrawItemImage(DRAWITEMSTRUCT *lpDrawItemStruct);

// Implementation
public:
	virtual ~CustomButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CustomButton)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif //_CUSTOMBUTTON_H_
