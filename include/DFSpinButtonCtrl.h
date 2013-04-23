#ifndef _DFSPINBUTTONCTRL_H_
#define _DFSPINBUTTONCTRL_H_

#include "types.h"

class FuSYSTEM_API DFSpinButtonCtrl : public CSpinButtonCtrl
{
//	FuDeclareClass(DFSpinButtonCtrl); // declare ClassID static member, etc.

// Construction
public:
	DFSpinButtonCtrl();

// Attributes
public:
	bool HoverUp, HoverDown;
	bool PressedUp, PressedDown;

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(DFSpinButtonCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~DFSpinButtonCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(DFSpinButtonCtrl)
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()
};

#endif // _DFSPINBUTTONCTRL_H_
