#ifndef _BASE_INPUTS_3D_H_
#define _BASE_INPUTS_3D_H_

/** 
 * @file  3D_BaseInputs.h 
 * @brief base classes for inputs
 */

#include "3D_DataTypes.h"



class Data3D;
class Operator3D;
class RenderMessage;
class ManagedInputManager;


#define INPUTS_BASE					(OBJECT_BASE + TAGOFFSET)
#define MTL_SELECT_COMBOBOX_BASE	(INPUTS_BASE + TAGOFFSET)






/**
 * The base class for the 'inputs' classes whose job are to encapsulate a set of inputs and attach them to an 'owner' tool.  For example,
 * the Shape3D tool maintains several sets of SurfaceInputs3D for the different geometry types (cylinder, box, sphere, etc).  Plugins that
 * desire to inject inputs into existing 3D tools will inherit this class to create their own input set.
 */
class FuSYSTEM_API Inputs3D : public Object
{
	FuDeclareClass(Inputs3D, Object);

public:
	Operator3D *Owner;										/**< The tool that this set of inputs has been attached to. */
	Inputs3D *ParentInputs;									/**< If this set of inputs was added as a set of subinputs to another Inputs3D object it will be pointed to here.  Otherwise this is NULL and these inputs are directly attached to an operator. */
	int32 NextMainInputValue;

	std::vector<bool> CloseNest;
	List SubInputs;											/**< SubInputs are Inputs3D that have been injected into these inputs */

	bool OnAddToFlowCalled;

	Registry *m_DataReg;										// registry for associated data object

	TagList Attrs;												/**< List of attributes specific to this set of inputs.  Use SetAttrsTagList() to set in values. */	

protected:
	ManagedInputManager *m_InputManager;

	// These can be overridden, but do not call these directly.  Use CallOnAddToFlow() and CallOnRemoveFromFlow() to invoke them.
	virtual void OnAddToFlow(TagList *tags = NULL);
	virtual void OnRemoveFromFlow(TagList *tags = NULL);

public:
	/** 
	 * Inputs3D is a base class for inheriting inputs from.  Adding of inputs must not occur in the constructor but rather 
	 * in AddInputs().  You should call RegisterInputs() on any top level Inputs3D before adding them.
	 * @param reg
	 * @param table
	 * @param tags
	 */
	Inputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~Inputs3D();

	static bool RegInit(Registry *regnode);
	static void RegCleanup(Registry *regnode);

	ManagedInputManager *GetInputManager()				{ return m_InputManager; }

	/**
	 * Adds the input the Owner tool.  Does special handling for LINK_Main inputs to avoid conflicts.
	 */
	Input *AddInput(const FuID &name, const FuID &id, Tag firsttag, ...);
	virtual Input *AddInputTagList(const FuID &name, const FuID &id, TagList &tags);

	/**
	 * Creates and adds the inputs.  When overriding this method, you should call the base class version.  If AddInputs()
	 * is called outside of a tools constructor, it must be wrapped in a BeginAddInputs/EndAddInputs pair.  Before a tool calls
	 * AddInputs() it should call RegisterInputs().
	 */
	bool AddInputs(Tag firstTag = _TAG_DONE, ...);
	virtual bool AddInputsTagList(TagList &tags);

	virtual Input *BeginControlNest(const char *name, const FuID &id, bool expand, Tag firsttag = _TAG_DONE, ...);
	virtual void EndControlNest();

	/**
	 * If the Owner tool calls AddInputs() outside of its ctor, it must wrap the AddInputs() call with BeginAddInputs()/EndAddInputs().
	 * These two functions are a hack to make sure NotifyChanged/OnAddToFlow is called when inputs are added outside of the ctor.
	 */
	virtual void BeginAddInputs(Tag firsttag = _TAG_DONE, ...);
	virtual void EndAddInputs(Tag firsttag = _TAG_DONE, ...);

	/**
	 * Gets the next available number for use with an inputs LINK_Main.  The LINK_Main values need to be unique.  This function
	 * is not for outputs.  This function helps avoid duplicate LINK_Main values.  When a connection is dragged to another tool 
	 * it is connected to the lowest LINK_Main with compatibile datatypes.
	 * @param reserve if true the returned value is reserved and will not be returned again
	 */
	virtual int GetNextMainValue(bool reserve = true, TagList *tags = NULL);

	/**
	 * Begins an input group.  An input group is a nest control that prefixes all contained controls with the groupID.
	 * @param name     the name that will be given to the nest associated with the group (if reopening an existing group to add more inputs, the name is ignored in favor of the original name)
	 * @param id       the id will be prefixed to inputs added between BeginGroup/EndGroup
	 * @param showNest if false the nest label is invisible and none of the elements of the group are indented in the control view
	 * @param expand   whether the nest should be expanded by default
	 */
	Input *BeginGroup(const char *name, const FuID &id, bool expand = false, bool showNest = true, Tag firsttag = _TAG_DONE, ...);
	virtual Input *BeginGroupTagList(const FuID &name, const FuID &id, bool expand, bool showNest, TagList &tags);

	/**
	 * Ends an input group 
	 */
	void EndGroup(Tag firsttag = _TAG_DONE, ...);
	virtual void EndGroupTagList(TagList &tags);


	/**
	 * Counts the number of inputs in the group.  The supplied id is appended to Owner->m_CurrentGroup. 
	 */
	virtual int NumGroupInputs(const FuID &id, TagList *tags = NULL);		

	virtual void BeginIndent();
	virtual void EndIndent();

	/** 
	 * Shows/hides the inputs.  Override this to show/hide the inputs in your class.  You should always call BaseClass::ShowInputs() 
	 * to allow the base class to show/hide any inputs it has.  Sometimes there's no need to override ShowInputs() unless you think another 
	 * tool will be planning to show/hide your inputs.  For example, when Shape3D switches from torus to cylinder it needs to call
	 * TorusInputs->ShowInputs(false) and CylinderInputs->ShowInputs(true).  Another example is when switching renderer type in the 
	 * Renderer3D tool - it needs to hide the old renderers inputs and show the new renderer inputs.  Be aware that ShowInputs() uses
	 * Operator::ShowInputControls() and Operator::HideInputControls() which internally keep ref counts to allow nested control sets 
	 * to show/hide themselves correctly.
	 */
	virtual void ShowInputs(bool visible);

	virtual int AddControlPageTagList(const char *name, TagList &tags);
	int AddControlPage(const char *name, Tag firsttag = NULL, ...);

	/** 
	 * Sets the tags into Attrs.  Instead of accessing Attrs directly, one should use this function because the class needs to be able
	 * to react to changes in its Attrs.
	 */
	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	/**
	 * Calls OnAddToFlow() if it already hasn't been called.
	 */
	virtual void CallOnAddToFlow(TagList *tags = NULL);
	virtual void CallOnRemoveFromFlow(TagList *tags = NULL);

	virtual void Use();
	virtual void Recycle();

	virtual Data3D *CreateData(Request *req);

	/** 
	 * Process() should get the inputs values from the request and use them to initialize the supplied "data" object.
	 * @param data  if its NULL you should create a new one
	 */
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);
	Data3D *Process(Request *req, Data3D *data, Tag firstTag = _TAG_DONE, ...);

	/**
	 * Adds all subinput sets that want to inject themselves into this set of inputs.  Returns the number of sets that were injected.
	 * AddAllSubInputs() will search the ClassRegistry for any inputs objects who wish to add themselves to this object and call their
	 * REGP_InjectInputs3DFunc to do the injection.
	 */
	virtual void AddAllSubInputs(TagList *tags = NULL);

	/**
	 * Adds the supplied set of subinputs into this object.  Other inputs that wish to inject themselves into these inputs call this.
	 * @param in the inputs to inject (ignored if NULL)
	 */
	virtual void AddSubInputs(Inputs3D *subInputs, TagList *tags = NULL);

	virtual void CheckRequest(Request *req);

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);

	virtual void GetAvailableEvents(EventList *events, const FuID &eventid = NOID);
	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);
	virtual bool OnEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	virtual void PreProcess(Request *req);
	virtual void PostProcess(Request *req);

	virtual void PreCalcPreProcess(Request *req);
	virtual void PreCalcPostProcess(Request *req);
	virtual void PreCalcProcess(Request *req);

	virtual void ConnectInputs(HashList *flow, TagList *tags = NULL);

	virtual bool OnConnect(Link *src, Link *dest, TagList *tags = NULL);
	virtual bool OnConnected(Link *link, Link *oldsrc, Link *newsrc, TagList *tags = NULL);
	virtual void OnDataTypeChanged(Link *link, FuID &oldDataType, TagList *tags = NULL);
	virtual void OnDelete(bool disconnect = true, TagList *tags = NULL);
	virtual void OnReplace(TagList *tags = NULL);

	virtual void OnStartRender(RenderMessage *rmsg, TagList *tags = NULL);
	virtual void OnEndRender(TagList *tags = NULL);
	virtual void OnGlobalExtentChanged(TimeExtent *globext, TagList *tags = NULL);

	virtual void InvalidateCache(Input *in = NULL, TagList *tags = NULL);
};



#endif