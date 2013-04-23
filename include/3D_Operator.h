#ifndef _OPERATOR_3D_H_
#define _OPERATOR_3D_H_

/** 
 * @file  3D_Operator.h 
 * @brief contains the Operator3D base class
 */

#include "3D_DataTypes.h"
#include "ThreadedOperator.h"

class Inputs3D;
class ManagedInputManager;


/**
 * Operator3D acts as the host tool for a set of Inputs3D and is the baseclass for most 3D tools
 */
class FuSYSTEM_API Operator3D : public ThreadedOperator
{
	FuDeclareClass(Operator3D, ThreadedOperator);

public:
	int NextMainValue;	/**< the LINK_Main value to be assigned to the next created main input */

	List Inputs;			/**< a list of top-level Inputs3D that are registered to receive notifications (usually this just has one entry - the base/host set of inputs) */

protected:
	//ManagedInputManager *m_InputManager;

	virtual void CheckRequest(Request *req);

public:
	Operator3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	Operator3D(const Operator3D &toCopy);
	virtual ~Operator3D();	

	//ManagedInputManager *GetInputManager();

	/**
	 * Gets the next available number for use with an inputs LINK_Main.  The LINK_Main values need to be unique.  This function
	 * is not for outputs.  This function helps avoid duplicate LINK_Main values.  When a connection is dragged to another tool 
	 * it is connected to the lowest LINK_Main with compatibile datatypes.
	 * @param reserve if true the returned value is reserved and will not be returned again
	 */
	virtual int GetNextMainValue(bool reserve = true, TagList *tags = NULL);

	/**
	 * Registers the supplied set of inputs with this operator.  Once the inputs are registered, the operator will route notifications
	 * for various message to them.  eg. NotifyChanged/OnConnect/PreProcess/OnAddToFlow/etc.  The inputs should be registered after you
	 * create then but before you call inputs->AddInputs().
	 */
	virtual void RegisterInputsTagList(Inputs3D *inputs, TagList &tags);
	void RegisterInputs(Inputs3D *inputs, Tag firstTag = _TAG_DONE, ...);

	/**
	 * There is no need to call this manually (the dtor calls it automatically).
	 */
	virtual void UnregisterAllInputs();

	// Creates uninitialized data object of the right type.
	virtual Data3D *CreateData(Request *req);

	// Override to read the values from the inputs into the data object.  Or create your own data object and override Process() directly.
	virtual Data3D *ProcessData(Request *req, Data3D *data);


	/**
	 * APITODO: These hide the Operator versions.  We need to do this because Operator::AddInputTagList() is not virtual.  Be careful
	 * when calling AddInput() you're always calling it on a compile time Operator3D pointer/reference.
	 */
	Input *AddInput(const FuID &name, const FuID &id, Tag firsttag = _TAG_DONE, ...);
	Input *AddInputTagList(const FuID &name, const FuID &id, TagList &tags);

	static bool RegInit(Registry *regnode);
	static void RegCleanup(Registry *regnode);

	virtual void Use();
	virtual void Recycle();

	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);

	virtual void OnAddToFlow(TagList *tags = NULL);
	virtual void OnRemoveFromFlow(TagList *tags = NULL);

	virtual void GetAvailableEvents(EventList *events, const FuID &eventid = NOID);
	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);
	virtual bool OnEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	virtual void PreProcess(Request *req);
	virtual void Process(Request *req);
	virtual void PostProcess(Request *req);

	virtual void PreCalcPreProcess(Request *req);
	virtual void PreCalcPostProcess(Request *req);
	virtual void PreCalcProcess(Request *req);

	virtual bool OnConnect(Link *src, Link *dest, TagList *tags = NULL);
	virtual bool OnConnected(Link *link, Link *oldsrc, Link *newsrc, TagList *tags = NULL);
	virtual void ConnectInputs(HashList *flow, TagList *tags = NULL);
	virtual void OnDataTypeChanged(Link *link, FuID &oldDataType, TagList *tags = NULL);
	virtual void OnDelete(bool disconnect = true, TagList *tags = NULL);
	virtual void OnReplace(TagList *tags = NULL);

	virtual void OnStartRender(RenderMessage *rmsg, TagList *tags = NULL);
	virtual void OnEndRender(TagList *tags = NULL);
	virtual void OnGlobalExtentChanged(TimeExtent *globext, TagList *tags = NULL);

	virtual void InvalidateCache(Input *in = NULL, TagList *tags = NULL);

	virtual bool CreateInputControls(ControlWnd *wnd, HWND parent, CRect& wrect, int page, TagList *tags = NULL);

	virtual int GetToolTip(int x, int y, char *buf, RECT *rect, TagList *tags = NULL);
	virtual void OnActive(bool activated, CChildFrame *frame, TagList *tags = NULL);

	virtual TimeExtent GetValid(Request *req, TagList *tags = NULL);
	virtual void AddToMenu(Menu *menu, TagList &tags);
	virtual void HandleMenu(uint32 cmd, uint_ptr *data, TagList &tags);
	virtual void HandleMenu(const ScriptVal &cmd, const ScriptVal &data, TagList &tags);

	virtual void AddControls(PreviewInfo *pi, TagList *tags = NULL);
	virtual void UpdateControls(TagList *tags = NULL);
	virtual void GLDrawControls(OperatorControl *oc, PreviewInfo *pi, TagList *tags = NULL);
	virtual DragType ControlDown(OperatorControl *oc, PreviewInfo *pi, CPoint point, TagList *tags = NULL);
	virtual void ControlMove(OperatorControl *oc, PreviewInfo *pi, CPoint point, TagList *tags = NULL);
	virtual void ControlUp  (OperatorControl *oc, PreviewInfo *pi, CPoint point, TagList *tags = NULL);
	virtual uint_ptr ControlCommand(OperatorControl *oc, PreviewInfo *pi, const FuID &id, uint_ptr data, TagList *tags = NULL);
	virtual bool ControlGetTip(OperatorControl *oc, PreviewInfo *pi, char *buf, TagList *tags = NULL);

	virtual bool ReplaceWith(Operator *op, uint32 flags = (ORW_UseOldSettings|ORW_UseNewOperator), TagList *tags = NULL);

	/**
	 * Utility function for converting the results of InMaterial->GetValue(req) which could return an Image/MtlGraph3D 
	 * into a MtlData3D.  The returned MtlData3D must be recycled.
	 */
	virtual MtlData3D *ConvertToMtlData(Number *n);
};


#endif