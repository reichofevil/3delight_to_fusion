#if !defined(_DFSCROLLBAR_H_)
#define _DFSCROLLBAR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"

#if !defined(unix) && !defined(DFLIB)

class ResColor;

typedef LRESULT (*ScrollWindowProc)(CWnd *, UINT, WPARAM, LPARAM);

class FuSYSTEM_API DFScrollBarObject
{
public:
	DFScrollBarObject();
	~DFScrollBarObject();

	void GetScrollData(SCROLLINFO *si);
	void SetScrollData(SCROLLINFO *si, int32 newThumbTop, uint32 message);
	void SendScrollMessage(void);
	void DoPaint(CDC *dc);

protected:
	CScrollBar *m_pWnd;
	HWND m_Owner;

	uint32 m_TimerID, m_TimerCount;

	bool m_MinEnable, m_MaxEnable;

	bool m_MinHover, m_MaxHover, m_ThumbHover, m_MinChannelHover, m_MaxChannelHover;
	bool m_MinPressed, m_MaxPressed, m_ThumbPressed, m_MinChannelPressed, m_MaxChannelPressed;

	int32 m_DownOffset;

	int32 m_Length, m_Width;
	int32 m_ThumbSize, m_ChannelSize, m_ThumbTop, m_ThumbBottom;

	CRect m_MinRect, m_MaxRect, m_ThumbRect, m_MinChannelRect, m_MaxChannelRect;

public:
	void SetWindow(CWnd *pWnd) { m_pWnd = (CScrollBar *) pWnd; }

	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam, ScrollWindowProc oldProc);
};

struct ScrollData
{
	bool m_Show, m_MinEnable, m_MaxEnable;

	bool m_MinHover, m_MaxHover, m_ThumbHover, m_MinChannelHover, m_MaxChannelHover;
	bool m_MinPressed, m_MaxPressed, m_ThumbPressed, m_MinChannelPressed, m_MaxChannelPressed;

	int32 m_DownOffset;

	int32 m_Length, m_Width;
	int32 m_ThumbSize, m_ChannelSize, m_ThumbTop, m_ThumbBottom;

	bool m_IsCaptured;
	int32 m_TrackPos;

	CRect m_ClientRect;
	CRect m_MinRect, m_MaxRect, m_ThumbRect, m_MinChannelRect, m_MaxChannelRect;
};

class FuSYSTEM_API DFScrollViewObject : public Object
{
	FuDeclareClass(DFScrollViewObject, Object); // declare ClassID static member, etc.

public:
	DFScrollViewObject();
	virtual ~DFScrollViewObject();

protected:
	void GetScrollData(uint32 type, SCROLLINFO *si);
	void SetScrollData(uint32 type, SCROLLINFO *si, int32 newThumbTop, uint32 message);
	void SendScrollMessage(uint32 type);
	void DoPaint(uint32 type, CDC *dc);
	void DoMouseMove(uint32 type, WPARAM wParam, CPoint pt, bool nc = TRUE);
	void DoLButtonDown(uint32 type, WPARAM wParam, CPoint pt);
	void DoLButtonUp(uint32 type, WPARAM wParam, CPoint pt);
	void DoMouseLeave(void);

	uint32 m_TimerID, m_TimerCount;
	bool m_DelayReleaseCapture;

	CRect m_FullClient;
	CPoint m_ClientOffset;
	ScrollData sData[2];

	WNDPROC m_OldProc;

	ResColor *m_BackCol;
	bool m_BackCol_BG;

	COLORREF m_BackColRef;

public:
	static const SIZE sizeDefault;

	CWnd *m_pWnd;

	void SetWindow(CWnd *pWnd, bool bReplaceWndProc = FALSE);
	void SetBkColor(ResColor *backCol, bool useBG = FALSE);
	void SetBkColor(COLORREF backCol);

	BOOL EnableScrollBar(UINT nSBFlags, UINT nArrowFlags = ESB_ENABLE_BOTH);
	BOOL GetScrollInfo(int fnBar, LPSCROLLINFO lpsi);
	int GetScrollPos(int nBar);
	BOOL GetScrollRange(int nBar, LPINT lpMinPos, LPINT lpMaxPos);
	BOOL SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE);
	int SetScrollPos(int nBar, int nPos, BOOL bRedraw = TRUE);
	BOOL SetScrollRange(int nBar, int nMinPos, int nMaxPos, BOOL bRedraw = TRUE);
	BOOL ShowScrollBar(int nBar, BOOL bShow = TRUE);

//	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam, ScrollWindowProc oldProc);

	static LRESULT CALLBACK SuperProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

class FuSYSTEM_API DFScrollBar : public CScrollBar
{
	FuDeclareClass(DFScrollBar, CScrollBar); // declare ClassID static member, etc.

public:
	DFScrollBar();

public:
	DFScrollBarObject so;

	static LRESULT CallOldWindowProc(CWnd *pWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DFScrollBar)
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~DFScrollBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(DFScrollBar)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(unix) && !defined(DFLIB)

#ifdef unix
  #define Default_NCPAINT() Default()
#else
  #define Default_NCPAINT() { \
	if (App->m_WinVerMajor >= 5) \
	{ \
		HRGN emptyrgn = CreateRectRgn(0, 0, 0, 0); \
		_AFX_THREAD_STATE *pThreadState = AfxGetThreadState(); \
		DefWindowProc(pThreadState->m_lastSentMsg.message, WPARAM(emptyrgn), pThreadState->m_lastSentMsg.lParam); \
		DeleteObject(emptyrgn); \
	} \
	else \
		Default(); \
  }
#endif

#endif // !defined(_DFSCROLLBAR_H_)
