#ifndef _DFLISTBOX_H_
#define _DFLISTBOX_H_

#include "types.h"
#include "DFScrollBar.h"

class FuSYSTEM_API DFListBox : public CListBox
{
	FuDeclareClass(DFListBox, CListBox); // declare ClassID static member, etc.

	DECLARE_DYNAMIC(DFListBox)

// Construction
public:
	DFListBox();

// Attributes
public:

#if !defined(unix) && !defined(DFLIB)
	DFScrollViewObject svo;
#endif

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(DFListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~DFListBox();

	int CalcMinimumItemHeight();
	void DoNcPaint(CDC *dc);

	int m_cyText;

	// Generated message map functions
protected:
	//{{AFX_MSG(DFListBox)
	afx_msg void OnNcPaint();
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class FuSYSTEM_API DFCheckListBox : public CCheckListBox
{
	FuDeclareClass(DFCheckListBox, CCheckListBox); // declare ClassID static member, etc.

	DECLARE_DYNAMIC(DFCheckListBox)

// Construction
public:
	DFCheckListBox();

// Attributes
public:

#if !defined(unix) && !defined(DFLIB)
	DFScrollViewObject svo;
#endif

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(DFCheckListBox)
	public:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~DFCheckListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(DFCheckListBox)
	afx_msg void OnNcPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class FuSYSTEM_API DFDragListBox : public CDragListBox
{
	FuDeclareClass(DFDragListBox, CDragListBox); // declare ClassID static member, etc.

	DECLARE_DYNAMIC(DFDragListBox)

// Construction
public:
	DFDragListBox();

// Attributes
public:

#if !defined(unix) && !defined(DFLIB)
	DFScrollViewObject svo;
#endif

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(DFDragListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~DFDragListBox();

	int CalcMinimumItemHeight();

	int m_cyText;

	// Generated message map functions
protected:
	//{{AFX_MSG(DFDragListBox)
	afx_msg void OnNcPaint();
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // _DFLISTBOX_H_
