#ifndef _CLIP_H_
#define _CLIP_H_

#ifndef DFMANAGER

#include "Object.h"
#include "ImageFormat.h"

//#define CLSID_DataType_Clip zMAKEID('Clip')

class CChildFrame;

struct ClipData
{
	char m_Filename[MAX_PATH+1];
	int32	m_StartFrame;
	int32	m_NumFrames;
	int32 m_Width;
	int32 m_Height;
	float64 m_XScale, m_YScale;
	ImageFormat *m_pImageFmt;
	Registry *m_pFmtReg;
	IOClass *m_pIOC;
	bool m_bMultiFrame;
	bool m_bHeaderLoaded;
	bool m_bHeaderSaved;
	bool m_bWantsIOClass;
	bool m_bLengthSetManually;
	uint32 m_TimeCodeStart;
	char m_KeyCode[32];
	char m_TimeCode[32];

	char m_MappedFilename[MAX_PATH+1];
};

class FuSYSTEM_API Clip : public Parameter
{
	FuDeclareClass(Clip, Parameter); // declare ClassID static member, etc.

public:
	FuID m_ID;

	ClipData m_MainClip;
	ClipData m_ProxyClip;

	ClipData *m_pCurrentClip;

	bool m_bSaving;
	bool m_bIsPreviewSaver;
	bool m_bReverse;						// Clip runs in reverse

	uint32 m_Loop;							// Clip loops this many times
	uint32 m_TrimIn, m_TrimOut;				// Skip this many initial/final frames
	uint32 m_ExtendFirst, m_ExtendLast;	// Repeat (new) first/last frame this many times

	// These are here for convenience in talking with PlayList
	TimeStamp m_GlobalStart, m_GlobalEnd;	// Where this clip lives

	int32 m_ImportMode;							// 0-Normal, 1-2:3 Pullup, 2-3:2 PU, 3-
	int32 m_ImportType;							// 0-Standard: AA BB BC CD DD, 1-Advanced: AA BB BC CC DD
	int32 m_InitialFrame;

	int32 m_AspectMode;
	int32 m_PullOffset;							// 0-AA, 1-BB, 2-BC, 3-CD, 4-DD

	uint32 m_Depth, m_AlphaMode;

#ifdef DEMO_VERSION
public:
	int lx,ly, dx,dy;
#endif

	bool m_bUseAlt;
	int32 m_LastError;

	int64 m_NextCluster;

	void Init(const char *fname, bool open, bool save, FusionDoc *doc);
	void GetCacheFilenameAndCopy(const char *cachepath, char *seqname, FusionDoc *doc, bool force = false);

public:
	Clip(const char *fname, bool save = FALSE, FusionDoc *doc = NULL);
	Clip(const Registry *reg, const ScriptVal &table, const TagList &tags);
	Clip(Clip &param);
	virtual ~Clip();

	static void RegCleanup(Registry *regnode);

	bool Save(ScriptVal &table, const TagList &tags);

	static const Clip &Get(Input *in, Request *req) { return *(Clip *) in->GetValue(req); }

	virtual Parameter *Copy();

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	virtual bool Open();
	virtual void Close();

	virtual Image *GetFrame(int32 frame, FusionDoc *doc, Tag firsttag = _TAG_DONE, ...);
	virtual bool PutFrame(int32 frame, Image *img, Tag firsttag = _TAG_DONE, ...);

	int32 GetLength();
	char *GetName();			// GetMainName()
	char *GetAltName();
	char *GetCurrentName();
	char *GetCurrentMappedName();
	static ImageFormat *ResolveFormat(const char *fname, FusionDoc *doc, const FuID &fromID);
	static ImageFormat *ResolveFormat(IOClass *ioc, FusionDoc *doc);
	static ImageFormat *ResolveFormat(IOClass *ioc);
	static ImageFormat *ResolveFormat(const char *fname, FusionDoc *doc);
	static ImageFormat *ResolveFormat(const char *fname);
	static bool GetLocalCachePath(char *cachepath, int buflen = MAX_PATH, FusionDoc *doc = NULL);

	IOClass *CreateIOClass(void);

	virtual bool GetPreviews(List *plist, uint32 modemask, CChildFrame *frame = NULL);

	virtual Image *GetFrameTagList(int32 frame, FusionDoc *doc, const TagList &tags);
	virtual bool PutFrameTagList(int32 frame, Image *img, const TagList &tags);

	void SetStartFrame(int32 frame);

	void SetAlternateClip(char *fname);

	// Useful functions
public: 
	static int GetSeqNumber(char *path);
	static int GetSeqNumber(char *path, int *filepos, int *digitstartpos, int *digitendpos);
	static int SetSeqNumber(char *path, int seq, int digits = 0);

protected:
	bool LookForFile(char *fname);
	int SetStartLength();
	void ExtractField(Image **pimg, int oldf, int field);
	void ScaleFrame(Image **pimg, int ollds, int sfactor);

public:
	bool CopyMainAttr(Tag tag, void *addr);
	uint32 GetMainAttr(Tag tag);
	uint32 GetMainAttr(Tag tag, uint32 def);
	float64 GetMainAttrD(Tag tag);
	float64 GetMainAttrD(Tag tag, float64 def);
	void *GetMainAttrPtr(Tag tag);
	void *GetMainAttrPtr(Tag tag, void *def);

	bool CopyAltAttr(Tag tag, void *addr);
	uint32 GetAltAttr(Tag tag);
	uint32 GetAltAttr(Tag tag, uint32 def);
	float64 GetAltAttrD(Tag tag);
	float64 GetAltAttrD(Tag tag, float64 def);
	void *GetAltAttrPtr(Tag tag);
	void *GetAltAttrPtr(Tag tag, void *def);

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(Clip);
#endif

protected:
	virtual bool _IsValid();
};

class FuSYSTEM_API AutoClip : public AutoParameter
{
	FuDeclareClass(AutoClip, AutoParameter); // declare ClassID static member, etc.

public:
	AutoClip(const char *fname, FusionDoc *doc, bool save = FALSE);
	AutoClip(const char *fname, bool save = FALSE);
	AutoClip(Input *in, TimeStamp time) : AutoParameter(in, time)	{ }
	operator Clip *()																{ return (Clip *)Param; }
};


#define CLIP_BASE (PARAMETER_BASE + TAGOFFSET)

// Clip Attributes
enum ClipAttrTags ENUM_TAGS
{
	CAT_Width = CLIP_BASE + TAG_INT,	// Get
	CAT_Height,							// Get
	CAT_Length,							// Get
	CAT_IsMultiframe,					// Get
	CAT_IsSaving,						// Get
	CAT_IsPreviewSaver,				// Get

	CAT_TrimIn,							// Get/Set	(Frames, 
	CAT_TrimOut,						// Get/Set
	CAT_ExtendFirst,					// Get/Set
	CAT_ExtendLast,					// Get/Set
	CAT_Loop,							// Get/Set
	CAT_Reverse,						// Get/Set

	CAT_ImportMode,					// Get/Set (see below for values)
	CAT_StartFrame,					// Get     (for non-Multiframe formats, first file sequence number)

	CAT_AspectMode,					// Get/Set
	CAT_PullOffset,					// Get/Set PullUp/PullDown sequence offset

	CAT_Depth,							// Get/Set

	CAT_AlphaMode,
	CAT_TimeCode,						// Get

	CAT_ImportType,					// Get/Set (see below for values)

	// These are here for convenience in talking to PlayList
	CATD_GlobalStart = CLIP_BASE + TAG_DOUBLE,	// Get/Set
	CATD_GlobalEnd,					// Get/Set

	CATD_FrameRate,					// Get (clip's default playback rate)

	CATD_XScale,						// Get
	CATD_YScale,						// Get

	CATP_Filename = CLIP_BASE + TAG_PTR,		// Get
	CATP_ImageFormat,					// Get
	CATP_FormatReg,					// Get/Set
	CATP_IOClass,						// Get
	CATP_MainFilename,				// Get/Set
	CATP_AlternateFilename,			// Get/Set
	CATP_KeyCode,						// Get
	CATP_TimeCodeStr,					// Get

	CATID_FormatID = CLIP_BASE + TAG_ID,		// Get
};

// Tags for passing to GetFrame() and PutFrame()
enum ClipFrameTags ENUM_TAGS
{
	CFT_Field = TAG_INT,			// Pass 0/1 to get field 0/1. Defaults to -1 (full frame)
	CFT_Scale,						// Scale down by this (integer) factor. Check format's RegTags!
	CFT_Alternate,					// Use the alternate clip
	CFT_ForceCache,				// Cache this, even if the defaults say not to

	CFTS_CachePath = TAG_STRING, // Check here for the filename, and copy to here if not found
};

enum ClipImportModeEnums		// values for CAT_ImportMode
{
	CIMT_Normal = 0,
	CIMT_23Pullup,
	CIMT_23Pulldown,
};

enum ClipImportTypeEnum			// values for CAT_ImportType
{
	CTYP_Standard = 0,			// Standard/Normal 2:3, ie. AA BB BC CD DD
	CTYP_Advanced,					// "Advanced" 2:3       ie. AA BB BC CC DD
};

enum ClipPullOffsetEnums		// values for CAT_PullOffset
{
	CPO_AA = 0,
	CPO_BB,
	CPO_BC,
	CPO_CD,
	CPO_DD,
};

enum ClipAspectModeEnums
{
	CASPT_ImageFormat = 0,		// Let image format define aspect, if it doesn't it falls back to FrameFormat
	CASPT_FrameFormat,			// From the default frame format
	CASPT_Custom,
};

enum ClipDepthEnums
{
	CDPTH_Format = 0,				// Whatever the format decides is the best depth
	CDPTH_Default,					// Whatever the flow's depth currently is
	CDPTH_8bitInt,					// 8 bit int per primary
	CDPTH_16bitInt,				// 16 bit int per primary
	CDPTH_16bitFloat,				// 16 bit float per primary
	CDPTH_32bitFloat,				// 32 bit float per primary
};

#endif
#endif
