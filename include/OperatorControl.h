#ifndef _OPERATORCONTROL_H_
#define _OPERATORCONTROL_H_

//#define CLSID_PreviewControl_Operator zMAKEID('OpCo')

#include "PreviewControl.h"
#include "InputControl.h"

class FuSYSTEM_API OperatorControl : public PreviewControl
{
	FuDeclareClass(OperatorControl, PreviewControl); // declare ClassID static member, etc.


// Attributes
protected:
	Operator *Op;

public:
	TagList Attrs;

	OperatorControl(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~OperatorControl();

	virtual bool SetAttrsTagList(Input *inp, const TagList& tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;
	virtual Operator *GetOperator(TagList *tags = NULL) { return Op; }

	virtual void GLDrawControls(PreviewInfo *pi, TagList *tags = NULL);
	virtual DragType ControlDown(PreviewInfo *pi, CPoint point, TagList *tags = NULL);
	virtual void ControlMove(PreviewInfo *pi, CPoint point, TagList *tags = NULL);
	virtual void ControlUp  (PreviewInfo *pi, CPoint point, TagList *tags = NULL);
	virtual uint_ptr ControlCommand(PreviewInfo *pi, const FuID &id, uint_ptr data, TagList *tags = NULL);
	virtual bool ControlGetTip(PreviewInfo *pi, char *buf, TagList *tags = NULL);

	virtual void SetInactive(TagList *tags = NULL);
	virtual void SetView(PreviewInfo *pi, TagList *tags = NULL);

	virtual void AddToMenu(Menu *menu, TagList &tags);
//	virtual void HandleMenu(uint32 cmd, uint_ptr *data, TagList &tags);
};

enum OperatorControl_Attrs
{
	OC_Operator = ICTAG_USER | TAG_PTR,
};

#endif
