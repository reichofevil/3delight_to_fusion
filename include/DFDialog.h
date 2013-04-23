#ifndef _DFDIALOG_H_
#define _DFDIALOG_H_

#include "Object.h"

class FuSYSTEM_API DFDialog : public CDialog, public Object
{
	FuDeclareClass(DFDialog, Object); // declare ClassID static member, etc.

	DECLARE_DYNAMIC(DFDialog)

	HINSTANCE m_hInstance;

// Construction
public:
	DFDialog();
	DFDialog(UINT nIDTemplate, CWnd *pParent = NULL);
	DFDialog(LPCTSTR lpszTemplateName, CWnd *pParent = NULL);

	BOOL Create(LPCTSTR lpszTemplateName, CWnd *pParentWnd = NULL);
	BOOL Create(UINT nIDTemplate, CWnd *pParentWnd = NULL);

	BOOL CreateTopLevel(LPCTSTR lpszTemplateName);
	BOOL CreateTopLevel(UINT nIDTemplate);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DFDialog)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~DFDialog();

protected:
	BOOL DFDialogCreate(LPCTSTR lpszTemplateName, CWnd *pParentWnd);
	BOOL DFDialogCreate(UINT nIDTemplate, CWnd *pParentWnd);

protected:
	// Generated message map functions
	//{{AFX_MSG(DFDialog)
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg HBRUSH OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnIdleUpdateCmdUI();
	//}}AFX_MSG
	afx_msg void OnTearOff(UINT nCmdID);
	afx_msg void OnUpdateTearOff(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

extern FuSYSTEM_API BOOL DF_SubclassWindow(CWnd *pWnd, HWND hWnd);
extern FuSYSTEM_API HWND DF_UnsubclassWindow(CWnd *pWnd);

extern FuSYSTEM_API void DF_DDX_Text(CDataExchange *pDX, int nIDC, CString &value);
extern FuSYSTEM_API void DF_DDX_Control(CDataExchange *pDX, int nIDC, CWnd &rControl);

extern FuSYSTEM_API void DDX_Check(CDataExchange* pDX, int nIDC, bool& value);

#endif // _DFDIALOG_H_
