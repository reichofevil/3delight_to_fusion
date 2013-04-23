#ifndef _INPUT_H_
#define _INPUT_H_

#ifndef DFMANAGER

#include "Link.h"
#include "Taglist.h"
#include "DFSafePtr.h"
#include "HashList.h"

#define CLASS_INPUT CLSID_Input

class InputControl;
class Operator;
class Output;
class Parameter;
class Number;
class Message;
class PreviewInfo;
class Matrix4;
class ImageDomain;
class ManagedInput;

// Note: DoNotifyChanged is actually a flags uint32 (bool)
#define IDNC_DoNotifyChanged		(1L << 0)		// Bit 0: DoNotifyChanged
#define IDNC_ForceSetSource		(1L << 1)		// Bit 1: Force SetSource on non-external input even during render
#define IDNC_DontModifyDoc			(1L << 2)		// Bit 2: Don't flag document as modified for this SetSource.
#define IDNC_DontInvalidate		(1L << 3)		// Bit 3: Don't invalidate caches for this SetSource.
//        Note: If this input is connected the source tool may still do so.

class FuSYSTEM_API Input : public Link
{
	FuDeclareClass(Input, Link); // declare ClassID static member, etc.

public:
	// Flags
	bool IgnoreDefault;
	bool External;
	bool Active;
	bool Required;
	bool Disabled;
	bool DoNotifyChangedFlag;
	bool Integer;
	bool ForceNotify;
	bool Passive;
	bool InteractivePassive;
	bool InitialNotify;
	bool ConnectRequired;

	int32 Priority, NumSlots;

	FuID ICClass, PCClass;
	uint32 ICGroup, ICID, ICPage;
	uint32 PCGroup, PCID;

	// These are only used by numeric inputs (type DT_Float) - these are completely ignored... use the xxxxxx64 variables below instead!!!
	float64 MinAllowed, MaxAllowed;  // Absolute Min/Max - values are clipped to these
	float64 MinScale, MaxScale;      // Suggested scaling Min/Max

	Output *Source;

public:
	List InputControlList;
	List PreviewControlList;

	FuID SourceOp;
	FuID SourceOut;
	char *StatusText;

	int32 ICHideCount, PCHideCount;

	FuID InstanceInp;
	Input *RealInput;

	ThreadSafePtr<Matrix4> ControlTransform;
	ThreadSafePtr<Matrix4> ControlRotation;

	uint32 NestCount;
	uint32 NestControlCount;
	Input *ParentNest;

	int32 DeferInvalidate; // Nesting Count
	int32 RenderDeferInvalidate; // Nesting Count

protected:
	char *m_Expression;
	HashList *m_ExpSourceList;

public:
	uint16 InitialNestControlCount;
	uint16 DoNestIndent;
	ManagedInput *m_ManagedInput;

	uint8 pad[20 - sizeof(ManagedInput *)];

public:
	Input(const FuID &name, const FuID &dt, Operator *owner);
	Input(const Registry *reg, const ScriptVal &table, const TagList &tags);
	
	virtual ~Input();
	
	virtual bool Save(ScriptVal &table, const TagList &tags);

	virtual void OnDisconnect(TagList *tags = NULL);
	virtual bool LinkTo(Output *out, bool ActiveConnect = true, TagList *tags = NULL);
	virtual void InvalidateCache(TagList *tags = NULL);
	virtual void InvalidateCache(const TimeExtent& te, TagList *tags = NULL);
	virtual bool CheckRecursiveConnect(Link *lnk, TagList *tags = NULL);
	bool DoCheckRecursiveConnect(Link *lnk, TagList *tags = NULL);
	virtual bool OnConnected(Link *link, Link *oldsrc, Link *newsrc, TagList *tags = NULL);
	bool DoOnConnected(Link *link, Link *oldsrc, Link *newsrc, TagList *tags = NULL);

	virtual bool SendRequest(TimeStamp time, uint32 flags = 0, TagList *tags = NULL);
	virtual bool PutMsg(Message *msg);

	virtual void SetRequestData(Parameter *param, TimeStamp time, const TimeExtent& valid, uint32 flags = 0, TimeStamp basetime = TIME_UNDEFINED, TagList *tags = NULL);       // see Link.h for SRQ_ flags
	virtual bool SetSource(Parameter *param, TimeStamp time, uint32 DoNotifyChanged = IDNC_DoNotifyChanged, TagList *tags = NULL);  // see flag defines above
	Parameter *GetSource(TimeStamp time);
	Parameter *GetSourceAttrs(TimeStamp time, TimeExtent *te, uint32 flags, uint32 flagmask = REQF_All, Tag firsttag = _TAG_DONE, ...);
	virtual Parameter *GetSourceAttrsTagList(TimeStamp time, TimeExtent *te, uint32 flags, uint32 flagmask, const TagList &attrs);

	// APIREV: Change GetSourceDomainTagList() args to match GetSourceDomain()
	ImageDomain *GetSourceDomain(TimeStamp time, uint32 flags, uint32 flagmask = REQF_All, Tag firsttag = _TAG_DONE, ...);
	virtual ImageDomain *GetSourceDomainTagList(TimeStamp time, const TagList &attrs);

	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	virtual bool IsUsed(TagList *tags = NULL) const;
	virtual bool IsConnected() const;
	virtual bool IsConnectedTo(Link *link, TagList *tags = NULL);
	virtual int32 GetNumConnects() const;
	virtual bool IsExternal() const;
	virtual void SetExternal(bool ext);

	InputControl *CreateControl(List *list, TagList *tags = NULL);
	void AddControls(PreviewInfo *pi, bool dosource = true, bool sourceonly = false, TagList *tags = NULL);

	// NOTE: Only called by InputControls!
	virtual void RemoveInputControl(InputControl *ic);

	inline Number *GetValue(Request *req, int32 slot = 0) { return (Number *)req->GetInputData(this, slot); }
	inline void SetValue(Request *req, Parameter *param, int32 slot = 0) { req->SetInputData(this, param, slot); }

	virtual void RestoreState(UndoState *us, TagList *tags = NULL);
	virtual UndoState *SaveState(TagList *tags = NULL);
	virtual void CleanupState(UndoState *us, TagList *tags = NULL);

	virtual void CachedExtent(const TimeExtent *te, TagList *tags = NULL);

	virtual void AddToMenu(Menu *menu, TagList &tags);
	virtual void HandleMenu(uint32 cmd, uint_ptr *data, TagList &tags);

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	virtual void ShowInputControls(bool setctrls = true);
	virtual void HideInputControls(bool setctrls = true);

	virtual void ShowPreviewControls(bool setctrls = true);
	virtual void HidePreviewControls(bool setctrls = true);

	virtual bool InputControlsVisible();
	virtual bool PreviewControlsVisible();

	virtual void SetExpression(const char *str, TagList *tags = NULL);
	virtual const char *GetExpression(TagList *tags = NULL);

	HashList *ExpSourceList();

	void SetDefaultParam(FuID datatype, FusionDoc *doc, const TagList &attrs, TimeStamp time = TIME_UNDEFINED, uint32 DoNotifyChanged = IDNC_DoNotifyChanged);

#if !defined(USE_NOSCRIPTING)
	DECLARE_LUATYPE(Input);
#endif
};

#define INPUT_BASE LINK_BASE + TAGOFFSET

enum InputAttrTags ENUM_TAGS
{
	INP_External = INPUT_BASE | TAG_INT,
	INP_Active,
	INP_Required,
	INP_Connected,
	INP_Priority,
	INP_Disabled,
	IC_ControlGroup,
	IC_ControlID,
	PC_ControlGroup,
	PC_ControlID,
	INP_DoNotifyChanged,
	INP_InitialNotify,						// Defaults to TRUE - is only done if INP_DoNotifyChanged is also TRUE
	INP_Integer,
	INP_NumSlots,

	INP_ForceNotify,
	INP_UserData,                       // This UserData gets saved and loaded by the input - the ones below do not
	INP_Passive,
	INP_SubType,
	INP_OpMenu,

	INP_SendRequest,                    // (default true), Request data from this input for current time before Process()

	INP_SaveVisible,
	INP_SetupPriority,

	INP_AcceptTransform,
	INP_AcceptsGLImages,						// Input is able to accept images uploaded into OpenGL offscreen buffers
	INP_AcceptsMasks,							// Input is able to accept single-channel CLSID_DataType_Mask images (usually for previews)

	INP_Level,									// Used for selectively retrieving settings. Example: Text+: level -1 = all inputs, level 0 = all inputs except image format and text, level 1 = shading only, no layout

	INP_InteractivePassive,					// Sort-of like INP_Passive, except that this will only be passive interactively, and will generate Undo events.
	INP_IgnoreValidExtent,					// This input's valid extent should be ignored completely, if it fails

	INP_DefaultData,

	INP_ConnectRequired,						// If the input is connected it acts like INP_Required, but can be unconnected.

	INP_MatchSize,								// For CLSID_DataType_Mask inputs, to ask the upstream mask tools to render at Owner's size. Only works if this Input has a priority lower than the Owner's MainInput
	INP_IgnoreFailedExtent,					// This input may be deliberately failed within its valid extent

	INP_DelayDefault,							// Delay creation of default param till OnAddToFlow()
	
	INP_AcceptsDoD,

	INP_SupportsMultiLayer,

	INP_MinAllowed = INPUT_BASE | TAG_DOUBLE,
	INP_MaxAllowed,
	INP_MinScale,
	INP_MaxScale,
	INP_Default,
	INP_DefaultX,
	INP_DefaultY,

	INP_UserData2,
	INP_UserData3,

	INP_LegacyDefault,
	INP_LegacyDefaultX,
	INP_LegacyDefaultY,

	INPS_UserString1 = INPUT_BASE | TAG_STRING,
	INPS_UserString2,
	INPS_UserString3,

	INPS_DefaultText,
	INPS_StatusText,

	INPS_LegacyDefaultText,

	INPT_UserTagList = INPUT_BASE | TAG_TAGLIST,

	INPP_Source = INPUT_BASE | TAG_PTR,

	INPP_DefaultClip,
	INPP_Set,

	INPP_DefaultParam,						// Don't forget to Recycle() whatever you pass to this!
	INPP_SourceImgInput,						// Auto-attach the contents of this Input to the SourceImg of any Requests to this input
	INPP_SourceImgParamPtr,					// Auto-attach the contents of this FuPointer<Parameter> to the SourceImg of any Requests to this input
	INPP_SourceImgParamValid,				// ptr to TimeExtent: Valid extent of the parameter referred to by INPP_SourceImgParamPtr

	INPP_DefaultDataPtr,
	INPP_UserDataPtr,

	INPP_NormalisedRoI,						// ptr to FuRect: For Previews to indicate RoI

	INPID_InputControl = INPUT_BASE | TAG_ID,
	INPID_PreviewControl,
	INPID_AddModifier,						// Automatically creates/connects a modifier of the giver ClassID
													// (Does not overrider user's default, if any).
	INPID_DefaultID,
	INPID_UserID1,

	INPID_LegacyDefaultID,

	INPID_AddID = INPUT_BASE | TAG_ID | TAG_MULTI,
};

// APIREV: Change GetSourceDomainTagList to take 'uint32 flags, uint32 flagmask'
#define INPUT_REQ_BASE		LINK_REQ_BASE + TAGOFFSET
enum InputReqAttrs ENUM_TAGS
{
	INPRA_Flags = TAG_INT + INPUT_REQ_BASE,
	INPRA_FlagMask,

	INPRA_RequestList = TAG_PTR + INPUT_REQ_BASE,			// Fill Req's on this list, instead of any on the Op's RequestList.
};

enum InputSubTypes
{
	IST_None = 0,
	IST_XSize,
	IST_YSize,
	IST_XPos,
	IST_YPos,
	IST_Angle,
	IST_ColorR,
	IST_ColorG,
	IST_ColorB,
	IST_ColorA,
};

enum InputUndoTags ENUM_TAGS		// For Undo
{
	IN_SourceOwnerID = TAG_INT + 100,

	IN_PCVisible,
	IN_ICVisible,
	IN_INPDisabled,
	IN_Offset,
	IN_ICDisabled,
	IN_PCDisabled,

	IN_Cache = TAG_OBJECT + 100,

	IN_SourceID = TAG_STRING + 100,
	IN_Expression,
};


#endif
#endif

