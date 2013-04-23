#ifndef _CUSTOMCHECKBOX_H_
#define _CUSTOMCHECKBOX_H_

#include "CustomButton.h"

class FuSYSTEM_API CustomCheckBox : public CustomButton
{
//	FuDeclareClass(CustomCheckBox); // declare ClassID static member, etc.

// Construction
public:
	CustomCheckBox();
	DECLARE_DYNCREATE(CustomCheckBox)

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CustomCheckBox)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

public:
	virtual void SetHotDIB(DIB *hot, float coldDIBH = 0.0, float coldDIBL = 1.0, float coldDIBS = 0.0, float coldDIBOpacity = 0.75);
	virtual void SetVisibility(bool showBasicButton, bool showIcon, bool showLabel, bool showToolTip, int labelPos = CBLP_Bottom, int labelAlign = CBLA_Center);

protected:
	virtual void DrawItemImage_BasicButton(DIB *tmp, DIB *button, DIB *hover = NULL, int32 backW = -1, int32 backH = -1);
	virtual void DrawItemImage_Icon(DIB *back, DIB *icon, DIB *hoverButton, int32 backW = -1, int32 backH = -1, bool useBorder = TRUE);
	virtual void DrawItemImage_Label(HDC dc, DRAWITEMSTRUCT *lpDrawItemStruct);

// Implementation
public:
	virtual ~CustomCheckBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CustomCheckBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif //_CUSTOMCHECKBOX_H_
