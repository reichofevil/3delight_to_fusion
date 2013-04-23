#ifndef _INPUTCONTROL_H_
#define _INPUTCONTROL_H_

#include "TagList.h"
#include "Input.h"
#include "Request.h"
#include "RegistryLists.h"
#include "ContextMenu.h"
#include "StandardControls.h"
#include "ResMath.h"

class FusionDoc;
class ControlWnd;

enum InputControl_Messages
{
	ICMSG_UPDATE_DISPLAY = WM_USER + 100,
};

class FuSYSTEM_API InputControl : public Object
{
	FuDeclareClass(InputControl, Object); // declare ClassID static member, etc.

protected:
	class ExpressionData;

public:
	HWND window;

	bool UndoSet;
	bool Passive;
	bool Visible;
	bool Disabled;

	char *Name;
	Input *PrevInput;

	List InputList;
	ListObj<ExpressionData> m_ExpressionList;
	
	uint32 OpID;
	uint32 ControlGroup;
	
	ControlWnd *CtrlWnd;
	FusionDoc *Document;

	float64 ControlWidth;

	HFONT Font;

	uint32 DeferUpdate;
	bool Recalc;

	bool InteractivePassive;

	bool TabStop;

	bool ic_sparebool;

	FuID m_InputGroup;

	uint8 ic_pad[8];

public:
	InputControl(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~InputControl();

	LRESULT PreHandleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT HandleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void GetInputsAt(List &inputs, int32 x, int32 y, TagList *tags = NULL);

	virtual bool SetAttrsTagList(Input *inp, const TagList& tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;
	
	virtual bool Open(HWND parent, CRect& size, TagList *tags = NULL);
	virtual void Close(TagList *tags = NULL);

	virtual void GetMinSize(int32 &minwidth, int32 &minheight, TagList *tags = NULL);
	virtual void GetMaxSize(int32 &maxwidth, int32 &maxheight, TagList *tags = NULL);
	virtual void SetSize(int32 &width, int32 &height, TagList *tags = NULL);
	bool SetSizeElement(CWnd &element, ResPosition &position, int32 width, int32 height, int32 offsetx = 0, int32 offsety = 0, TagList *tags = NULL);

	virtual bool AddInput(Input *in, TagList *tags = NULL);
	virtual bool RemoveInput(Input *in, TagList *tags = NULL);
	virtual void SetInputs(Parameter *param, TimeStamp time, uint32 id, bool intermediate = FALSE, TagList *tags = NULL);
	virtual void ResetUndo(TagList *tags = NULL);
	
	virtual void NotifyChanged(Input *input, Parameter *param, TagList *tags = NULL);
	virtual void UpdateDisplay(Input *input, TagList *tags = NULL);
	virtual void OnConnect(Input *inp, TagList *tags = NULL);
	virtual void OnDisconnect(Input *inp, TagList *tags = NULL);

	virtual Input *GetInputFrom(HWND wnd, TagList *tags = NULL);

	virtual void SetUndo(uint32 id, TagList *tags = NULL);
	virtual void OnOK();
	virtual void OnCancel();
	virtual void OnDropFiles(HDROP hDropInfo);

	virtual void DrawBevel(HDC dc, RECT *rect);

	void MoveWindow(CRect &rect);

	virtual void AddToMenu(Menu *menu, TagList &tags);
	virtual void HandleMenu(uint32 cmd, uint_ptr *data, TagList &tags);

	// non-virtual for a reason! (Would break compatibility big-time)
	bool HasValueChanged(bool Integer, double NewValue, double OldValue, double Fuzz);

	void RecalcControlSize(void);

	virtual void AddExpressionEdit(Input *in, TagList *tags = NULL);
	virtual bool CreateExpressionEdit(int controlid, CRect &size, TagList *tags = NULL);
	virtual ExpressionData *GetExpressionData(int controlid, TagList *tags = NULL);
	virtual void ExpressionChanged(Input *in, TagList *tags = NULL);

	void SetExpression(Input *in, char *exp);
	void SetExpressions(uint32 id, char *exp);

	bool GetDisabledTags(const TagList &tags, bool def);
};

#endif
