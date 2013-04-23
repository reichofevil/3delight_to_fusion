#ifndef _DFTREECTRL_H_
#define _DFTREECTRL_H_

#include "types.h"
#include "DFScrollBar.h"

class ResColor;

class FuSYSTEM_API DFTreeCtrl : public CTreeCtrl
{
	FuDeclareClass(DFTreeCtrl, CTreeCtrl); // declare ClassID static member, etc.

	DECLARE_DYNCREATE(DFTreeCtrl)

	// Construction
public:
	DFTreeCtrl();

// Attributes
public:
	ResColor *m_BackCol;
	bool m_BackCol_BG;

#if !defined(unix) && !defined(DFLIB)
	DFScrollViewObject svo;
#endif

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DFTreeCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(DFTreeCtrl)
	afx_msg NCHITTEST_RESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcPaint();
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // _DFTREECTRL_H_
