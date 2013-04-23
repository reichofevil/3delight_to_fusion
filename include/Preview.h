#ifndef _PREVIEW_H_
#define _PREVIEW_H_

#include "Input.h"

class Image;
class Operator;
class CChildFrame;
struct SQC_Data;


#define PREVIEW_BASE INPUT_BASE + TAGOFFSET



class FuSYSTEM_API PreviewMessage : public Message
{
	FuDeclareClass(PreviewMessage, Message); // declare ClassID static member, etc.

public:
	Input *In;
	
	PreviewMessage(Input *in) : Message(MAKEID('Prev')) { In = in; }
};


// Modes the Preview will deal with. A few combinations are possible.
typedef enum PreviewModeEnums
{
	PVMODE_Nothing          = 0x0000,	// Get/Set : Do Nothing (exclusive with everything else!)
	PVMODE_DisplayImage     = 0x0001,	// Get/Set
	PVMODE_CopyImage        = 0x0002,	// Set
	PVMODE_SaveImage        = 0x0004,	// Set : specified by Registry Node tags [TODO]

	PVMODE_CreateAnim       = 0x0008,	// Get/Set
	PVMODE_RenderComplete   = 0x0010,	// Get/Set : This can be used to signal anim created
	PVMODE_SaveAnim         = 0x0020,	// Set
	PVMODE_CopyAnim         = 0x0040,	// Set

	PVMODE_PlayAnim         = 0x0080,	// Get/Set
	PVMODE_PlayAnimRev		= 0x0100,	// Get/Set
	PVMODE_StopAnim         = 0x0200,	// Get/Set
	PVMODE_GoToStart        = 0x0400,	// Set
	PVMODE_GoToEnd          = 0x0800,	// Set
	PVMODE_FrameAdvance		= 0x1000,	// Set
	PVMODE_FrameBack			= 0x2000,	// Set
	PVMODE_Cue					= 0x4000,	// Get/Set
	PVMODE_Review				= 0x8000,	// Get/Set

	PVMODE_Pause			 = 0x010000,	// Get/Set
	PVMODE_Record			 = 0x020000,	// Get/Set : Crash Record (Note: Must do PVMODE_PrepRecord first!)
	PVMODE_Edit				 = 0x040000,	// Get/Set : Edit (see PVATTR_EditMode below)
	PVMODE_PrepRecord		 = 0x080000,	// Get/Set : Crash Record preparation - do before PVMODE_Record

	PVMODE_Open				= 0x00100000,
	PVMODE_Close			= 0x00200000,
	PVMODE_Destroy			= 0x00400000,

	// synonyms
	PVMODE_Play				= PVMODE_PlayAnim,
	PVMODE_PlayRev			= PVMODE_PlayAnimRev,
	PVMODE_Stop				= PVMODE_StopAnim,
	PVMODE_Create			= PVMODE_CreateAnim,
	PVMODE_OnCreated		= PVMODE_RenderComplete,

} PreviewMode;

#define PVMODE_AnimExists (PVMODE_PlayAnim|PVMODE_PlayAnimRev|PVMODE_StopAnim|PVMODE_RenderComplete)
#define PV_MODECHECK	(PVMODE_CreateAnim|PVMODE_RenderComplete|PVMODE_SaveAnim|PVMODE_CopyAnim|PVMODE_PlayAnim|PVMODE_PlayAnimRev|PVMODE_StopAnim|PVMODE_GoToStart|PVMODE_GoToEnd)



// Preview Attributes
enum PreviewAttrsEnums ENUM_TAGS
{
	PVATTR_CurrentFrame = TAG_INT + PREVIEW_BASE, // Get/Set (same as PVMODE_GoToFrame)
	PVATTR_CurrentTime,					// Get/Set
	PVATTR_Length,							// Get
	PVATTR_Loops,							// Get/Set (0 is infinite, >1 may set infinite)
	PVATTR_Capabilities,					// Get : See below
	PVATTR_InFrame,						// Get/Set
	PVATTR_OutFrame,						// Get/Set
	PVATTR_HasFields,						// Get
	PVATTR_SetField,						// Set : 0=Frames, 1=Field 1, 2=Field 2
	PVATTR_VidStandard,					// Get : 0=N/A, 1=PAL, 2=NTSC, 3=SECAM, 4=PAL-M
	PVATTR_PrerollFrames,				// Get : (e.g. Should be pre-rolled 125 frames for best results)
	PVATTR_PreEditFrames,				// Get : (e.g. Edit must be issued 3 frames before desired point)
	PVATTR_EditCaps,						// Get capabilities (see below for flags)
	PVATTR_EditMode,						// Get/Set          (see below for flags)
	PVATTR_TimeCode,						// Get/Set : Use to set the TimeCode generator
	PVATTR_EEMode,							// Get/Set : E-E mode enable
	PVATTR_RenderFlags,					// Get/Set : Overrides Prefs for Preview renders (NULL == use Prefs)
	PVATTR_StartFrame,					// Get : Start point of this preview i.e. starting frame number or timecode
	PVATTR_Visible,						// Get/Set : WindowPreview uses this to indicate that a buffer isn't visible
	PVATTR_SerialNumber,					// Get : Some unique serial number or ID associated with the device e.g. hardware serial number of a DPS Reality.
	PVATTR_IsClockSource,				// Get/Set : Set to TRUE or FALSE to make this a clock source. Use PVATTR_UseClockSource to get the event.
	PVATTR_Reserved1,						// Reserved for past use
	PVATTR_UnitNumber,
	PVATTR_PingPong,						// Get/Set : true/false

	PVATTRD_PlayRate = TAG_DOUBLE + PREVIEW_BASE,	 // Get/Set (fps, -ve is reverse)
	PVATTRD_DefaultPlayRate,			// Get
	PVATTRD_Volume,						// Get/Set : Range 0.0-1.0
	PVATTRD_Offset,						// Get/Set : timestamp
	PVATTRD_PlayFrom,						// Set : timestamp

	PVATTR_Filename = TAG_PTR + PREVIEW_BASE,		// Get/Set : To Preview a file
	PVATTR_ParentOp,						// Get/Set : Usually needed with Filename
	PVATTR_Title,							// Get/Set
	PVATTR_Document,						// Get/Set
	PVATTR_ChildFrame,					// Get/Set
	PVATTR_SpeedUnits,					// Get : eg FPS, Hz
	PVATTR_ConfigHook,					// Get : = bool (*hook)(Preview *this)
	PVATTR_DeviceClosed,					//     Set : This is used to tell a SingleFilePreview that its WindowPreview is closing
	PVATTR_RenderFilename,				// Set : Used to override the filename that a device would create previews at
	PVATTR_Clip,							// Get/Set : To Preview an existing clip
	PVATTR_UseDevice,						// Get/Set : Use this Preview device for display, if possible
	PVATTR_UseClockSource,				// Get/Set : (event handle) This preview is slaved to this clock source's FrameEvent

	PVATTR_RenderGroups = TAG_STRING + PREVIEW_BASE,	// Set (Create): Slave groups to use for network renders
	PVATTR_RenderFrameSet,				// Set (Create): FrameSet string to use for network renders

	PVATTRID_DefFormatID = TAG_ID + PREVIEW_BASE,	// Get : Primary File Format ID

	PVATTRO_ImageDoD = TAG_OBJECT + PREVIEW_BASE,		// Stores DoD for displayed image

	// Synonyms
	PVATTR_Op = PVATTR_ParentOp,
};

// Capabilities bit flags (for PVATTR_Capabilities)
#define  PVCAP_CanPlay				0x00000001
#define  PVCAP_CanPlayRev			0x00000002
#define  PVCAP_CanCue				0x00000004
#define  PVCAP_CanReview			0x00000008
#define  PVCAP_CanGoToFrame		0x00000010
#define  PVCAP_CanGoToTime			0x00000020
#define  PVCAP_CanAdvanceFrame	0x00000040
#define  PVCAP_CanBackFrame		0x00000080
#define  PVCAP_CanGoStart			0x00000100
#define  PVCAP_CanGoEnd          0x00000200
#define  PVCAP_CanLoop				0x00000400
#define  PVCAP_CanSetInOutPoints 0x00000800
#define  PVCAP_CanPlayFields		0x00001000		// Can be told to play each individual field
#define  PVCAP_CanSetPlayRate    0x00002000
#define  PVCAP_CanRecord         0x00004000
#define  PVCAP_CanPause          0x00008000
#define  PVCAP_CanEdit				0x00010000		// Can assemble or insert specific channels, use flags below
#define  PVCAP_CanDisplayImage	0x00020000
#define  PVCAP_CanDoEEMode			0x00040000		// Supports E-E viewing
#define  PVCAP_CanPingPong			0x00080000

// Edit bit flags (for PVATTR_EditMode/PVATTR_EditCaps)
#define PVEDIT_Assemble				0x00000020		// Assemble mode (selects all channels)
#define PVEDIT_Insert				0x00000040		// Insert mode   (default)

#define PVEDIT_Video					0x00000010		// Channels for Insert mode
#define PVEDIT_Audio1				0x00000001
#define PVEDIT_Audio2				0x00000002
#define PVEDIT_TimeCode				0x00000004
#define PVEDIT_InsertAudio			(PVEDIT_Insert      | PVEDIT_Audio1 | PVEDIT_Audio2)
#define PVEDIT_InsertAV				(PVEDIT_InsertAudio | PVEDIT_Video)
#define PVEDIT_InsertAll			(PVEDIT_InsertAV    | PVEDIT_TimeCode)


///////////////////////////////////////////////////////////////
// Preview definition

class FuSYSTEM_API Preview : public Input
{
	FuDeclareClass(Preview, Input); // declare ClassID static member, etc.

protected:
	PreviewMode Mode;			// Current Mode of this Preview
	FusionDoc *Document;
	CChildFrame *Frame;		// Can be NULL if a Global Preview

	Output *OldSource;

	void SetDocument(FusionDoc *doc);

public:
	Operator *OutSaver, *PreviewOp, *ViewOp;
	Operator *OutLoader, *OutRunCmd;
	bool TempFile;
	int OldPreviewScale;			// temp storage for AutoPreviewScaling
	int OldPreviewSize;			// temp storage for PREF_PreviewSizeType
//NETTODO:	DFInterface *DFI;				// for net rendering

	char PreviewFilename[_MAX_PATH];
	int32 Width, Height, Length;
	float32 DefaultRate;
	uint32 RenderFlags;
	uint8 *RenderFrameArray;
	uint32 RenderFrameArraySize;
	int RenderStep;
	bool Looping;
	bool UseExistingFilename;
	bool PingPong;

	bool SkipDupCheck;			// setting this will force a notify of the next incoming param, even if it's a duplicate

	HANDLE FrameEvent;			// our own event. pulsed every frame when playing, if this preview is a clock source
	HANDLE UseFrameEvent;		// if non-NULL, playback will be synchronised to this external event

public:
	Preview(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~Preview();

	virtual void SetRequestData(Parameter *param, TimeStamp time, const TimeExtent& valid, uint32 flags = 0, TimeStamp basetime = TIME_UNDEFINED, TagList *tags = NULL);		// see Link.h for SRQ_ flags);
	virtual bool LinkTo(Output *out, bool ActiveConnect = false, TagList *tags = NULL);
	virtual void InvalidateCache(TagList *tags = NULL);
	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	virtual PreviewMode GetMode(Operator *op = NULL );
	virtual bool CanSetMode(PreviewMode mode, Operator *op = NULL);
	virtual bool SetMode(PreviewMode mode, Operator *op = NULL);
	virtual void SetPreview(Operator *op = NULL);

	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	virtual void RestoreState(UndoState *us, TagList *tags = NULL);
	virtual UndoState *SaveState(TagList *tags = NULL);
	virtual void CleanupState(UndoState *us, TagList *tags = NULL);

	virtual void DisplayImage(Image *img, TimeExtent* imgvalid = NULL, TimeStamp time = TIME_UNDEFINED);

	// Generic method (encouraged)
	using Object::Do;
	virtual bool Do(uint32 cmd, TagList& tags);							// override this to handle attrs

	// Common overridables
	virtual bool OpenTags(const TagList &tags);
	virtual bool Close();
	virtual bool PlayTags(const TagList &tags);
	virtual void Stop();
	virtual bool PrepRecordTags(const TagList &tags);
	virtual bool RecordTags(const TagList &tags);
	virtual bool CreateTags(const TagList &tags);
	virtual void OnCreated();
	virtual bool Destroy();
	virtual bool MakePreviewFilenameTags(const TagList &tags);
	virtual bool MakeSpoolFilenameTags(const TagList &tags);

	// varargs versions
	bool Open(Tag firsttag = _TAG_DONE, ...);
	bool Play(Tag firsttag = _TAG_DONE, ...);
	bool PlayRev(Tag firsttag = _TAG_DONE, ...);
	bool PrepRecord(Tag firsttag = _TAG_DONE, ...);
	bool Record(Tag firsttag = _TAG_DONE, ...);
	bool Create(Tag firsttag = _TAG_DONE, ...);
	bool MakePreviewFilename(Tag firsttag = _TAG_DONE, ...);
	bool MakeSpoolFilename(Tag firsttag = _TAG_DONE, ...);

	// misc
	static void ThreadAsyncWait(SQC_Data *data);
	static Operator *FindSaver(Operator *op);

	virtual bool LoadPreview(char *filename = NULL);
	virtual bool LoadPreviewHeader(char *filename = NULL);
	virtual bool SavePreviewAs(Operator *op, char *filename = NULL);

public:
};


/////////////////////////////////////////////////////////////////////////////
#endif
