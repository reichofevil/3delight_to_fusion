#ifndef _DFIPADDRESSCTRL_H_
#define _DFIPADDRESSCTRL_H_

#include "types.h"

class FuSYSTEM_API DFIPAddressCtrl : public CIPAddressCtrl
{
//	FuDeclareClass(DFIPAddressCtrl); // declare ClassID static member, etc.

// Construction
public:
	DFIPAddressCtrl();

// Attributes
public:
	char className[50];

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(DFIPAddressCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~DFIPAddressCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(DFIPAddressCtrl)
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // _DFIPADDRESSCTRL_H_
