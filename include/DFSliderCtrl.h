#if !defined(_DFSLIDERCTRL_H_)
#define _DFSLIDERCTRL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class FuSYSTEM_API DFSliderCtrl : public CSliderCtrl
{
	FuDeclareClass(DFSliderCtrl, CSliderCtrl); // declare ClassID static member, etc.

public:
	DFSliderCtrl();

public:
	char className[50];

	bool Hover;
	bool Pressed;

protected:
	bool IsPainting;

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DFSliderCtrl)
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~DFSliderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(DFSliderCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(_DFSLIDERCTRL_H_)
