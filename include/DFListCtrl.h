#ifndef _DFLISTCTRL_H_
#define _DFLISTCTRL_H_

#include "types.h"
#include "DFScrollBar.h"

class DIB;
class ResColor;

#define DFLCF_PathEllipses		0x00000001

class FuSYSTEM_API DFListCtrl : public CListCtrl
{
	FuDeclareClass(DFListCtrl, CListCtrl); // declare ClassID static member, etc.

	DECLARE_DYNCREATE(DFListCtrl)

	// Construction
public:
	DFListCtrl();

// Attributes
public:
	DIB *DIB_ImageList;
	DIB *DIB_HeaderImageList;

	ResColor *m_BackCol;
	bool m_BackCol_BG;

#if !defined(unix) && !defined(DFLIB)
	DFScrollViewObject svo;
#endif

	int32 m_EditingItem;

// Operations
public:
	void SetDIB_ImageList(DIB *dib);
	void SetDIB_HeaderImageList(DIB *dib);

	uint32 GetDFFlags(int32 index);
	void SetDFFlags(int32 index, uint32 flags);

	void PrePaint_Icon(NMLVCUSTOMDRAW *nmlv);
	void PaintItem_Icon(NMLVCUSTOMDRAW *nmlv);

	void PrePaint_Report(NMLVCUSTOMDRAW *nmlv);
	void PaintItem_Report(NMLVCUSTOMDRAW *nmlv);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DFListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~DFListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(DFListCtrl)
	afx_msg NCHITTEST_RESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // _DFLISTCTRL_H_
