#if !defined(_DFSTATIC_H_)
#define _DFSTATIC_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DFStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DFStatic window

class FuSYSTEM_API DFStatic : public CStatic
{
	FuDeclareClass(DFStatic, CStatic); // declare ClassID static member, etc.

// Construction
public:
	DFStatic();

// Attributes
public:
	WCHAR *Label;
	uint32 LabelLength;

	char className[50];
	uint32 realStyle;

	void SetLabel(LPCSTR label);
	void SetLabelInt(int32 val);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DFStatic)
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~DFStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(DFStatic)
	afx_msg void OnNcPaint();
	//}}AFX_MSG
	afx_msg LRESULT OnSetText(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DFSTATIC_H_)
