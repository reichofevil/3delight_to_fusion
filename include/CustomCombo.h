#ifndef _CUSTOMCOMBO_H_
#define _CUSTOMCOMBO_H_

#include "types.h"
#include "DFListBox.h"

class FuSYSTEM_API DFComboBox : public CComboBox
{
	FuDeclareClass(DFComboBox, CComboBox); // declare ClassID static member, etc.

// Construction
public:
	DFComboBox();
	DECLARE_DYNCREATE(DFComboBox)

	DFListBox m_DropDownList;
	bool Hover;

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DFComboBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~DFComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(DFComboBox)
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()
};

#endif //_CUSTOMCOMBO_H_
