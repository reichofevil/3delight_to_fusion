#ifndef _PREFSPAGE_H_
#define _PREFSPAGE_H_
// PrefsPage.h : header file
//
#include "PipelineDoc.h"
#ifndef DFLIB
#include "CustomButton.h"
#include "CustomCheckBox.h"
#include "CustomRadioButton.h"
#include "CustomCombo.h"
#include "DFStatic.h"
#include "TimeCodeEdit.h"
#include "DFSpinButtonCtrl.h"
#include "DFTreeCtrl.h"
#include "DFListCtrl.h"
#include "DFListBox.h"
#include "DFSliderCtrl.h"
#include "DFIPAddressCtrl.h"
#endif
#include "DFDialog.h"

/////////////////////////////////////////////////////////////////////////////
// PrefsPage dialog

class FuSYSTEM_API PrefsPage : public CPropertyPage, public Object
{
	FuDeclareClass(PrefsPage, Object); // declare ClassID static member, etc.

// Construction
public:
	PrefsPage(const Registry *reg, const ScriptVal &table, const TagList &tags);
	~PrefsPage();

	FusionDoc *doc;
	bool m_Changed;

	bool IsFirstSetActive(void);
	void SetFirstSetActive(bool first);

	void SetChanged(bool changed = true);
	bool HasChanged(bool clear = true);
	
	bool DisableIfLocked(int id, const FuID &pref);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(PrefsPage)
	public:
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnSetActive();
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(PrefsPage)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#define PREFPAGE_BASE OBJECT_BASE + TAGOFFSET

enum PPGTags
{
	PPG_TemplateID = PREFPAGE_BASE | TAG_INT,
};

#endif
