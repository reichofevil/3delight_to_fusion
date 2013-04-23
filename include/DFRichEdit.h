#ifndef _DFRICHEDIT_H_
#define _DFRICHEDIT_H_

#include "types.h"
#include "DFScrollBar.h"

#include <richedit.h>

class FuSYSTEM_API DFRichEdit : public CRichEditCtrl
{
	FuDeclareClass(DFRichEdit, CRichEditCtrl); // declare ClassID static member, etc.

	DECLARE_DYNCREATE(DFRichEdit)

	// Construction
public:
	DFRichEdit();

// Attributes
public:

#if !defined(unix) && !defined(DFLIB)
	DFScrollViewObject svo;
#endif

// Operations
public:

	void SetDefCharFormat(const char *FaceName, int Height);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DFRichEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(DFRichEdit)
	afx_msg NCHITTEST_RESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // _DFRICHEDIT_H_
