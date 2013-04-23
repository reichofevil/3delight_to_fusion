#ifndef _DFPROGRESSCTRL_H_
#define _DFPROGRESSCTRL_H_

#include "types.h"

class FuSYSTEM_API DFProgressCtrl : public CProgressCtrl
{
//	FuDeclareClass(DFProgressCtrl); // declare ClassID static member, etc.

// Construction
public:
	DFProgressCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(DFProgressCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~DFProgressCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(DFProgressCtrl)
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // _DFPROGRESSCTRL_H_
