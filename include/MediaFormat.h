#ifndef _MEDIAFORMAT_H_
#define _MEDIAFORMAT_H_

#ifndef DFMANAGER

#ifndef DF_TAGS_ONLY

#include "Object.h"
#include "IOClass.h"
#include "RegistryLists.h"
#include "Operator.h"
#include "PipelineDoc.h"

class FuSYSTEM_API MediaFormat : public Object
{
	FuDeclareClass(MediaFormat, Object); // declare ClassID static member, etc.

protected:
	IOClass *ioc;

	int *StatusPtr;

	double Quality;
	bool Fast;

public:
	MediaFormat(const Registry *reg, const ScriptVal &table, const TagList &tags);
// virtual ~MediaFormat();

	bool IsThisFormat(IOClass *ioc, Tag firsttag = _TAG_DONE, ...);
	virtual bool IsThisFormat(IOClass *ioc, TagList &tags);
	virtual bool CheckOptions();

	bool LoadHeader(const FuID &dtype, IOClass *ioc, Tag firsttag = _TAG_DONE, ...);
	virtual bool LoadHeader(const FuID &dtype, IOClass *ioc, TagList &tags);
	bool SaveHeader(const FuID &dtype, IOClass *ioc, Parameter *data, Tag firsttag = _TAG_DONE, ...);
	virtual bool SaveHeader(const FuID &dtype, IOClass *ioc, Parameter *data, TagList &tags);
	Parameter *LoadData(const FuID &dtype, TimeStamp time, TimeStamp duration = 1.0, Tag firsttag = _TAG_DONE, ...);
	virtual Parameter *LoadData(const FuID &dtype, TimeStamp time, TimeStamp duration, TagList &tags);
	bool SaveData(const FuID &dtype, Parameter *data, TimeStamp time, Tag firsttag = _TAG_DONE, ...);
	virtual bool SaveData(const FuID &dtype, Parameter *data, TimeStamp time, TagList &tags);

	virtual bool Close();

	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	// Called when someone is looking for Previews to play...
	virtual bool GetPreviews(List *plist, uint32 modemask, CChildFrame *frame = NULL);

	virtual int32 GetNumStreams(const FuID &dtype);

	virtual void AddLoadInputs(Operator *op);
	virtual void NotifyChangedLoad(Operator *op, Input *in, Parameter *param, TimeStamp time);
	virtual void SetupLoad(Operator *op, Request *req);
	virtual void CleanupLoad(Operator *op);

	virtual void AddSaveInputs(Operator *op);
	virtual void NotifyChangedSave(Operator *op, Input *in, Parameter *param, TimeStamp time);
	virtual void SetupSave(Operator *op, Request *req);
	virtual void CleanupSave(Operator *op);
};

typedef void (* MFHookPtr)(Operator *op);
#define MFINPUT(x) STDINP('Fmt\0' | x)

typedef void (* MFNotifyHookPtr)(Operator *op, Input *in, Parameter *Param, TimeStamp time);

#endif

/** Attribute tags that an MediaFormat can be queried on, or set.
 ** attributes common to all instances, and capabilities of the format.
 **/

enum MediaFormat_Tags
{
	MF_IsPreviewSaver = TAG_INT + 100,
	MF_Fast,					// TRUE to take any shortcuts necessary to improve speed
	MF_StreamIndex,		// Default: 0  (currently unused)
	MFA_NumSamples,		// For Audio streams, loading too
	MF_LoadHeaderOnly,	// default: FALSE, returns NULL, but Width & Height are then valid,
								// and the MediaFormat will contain all header info
	MF_LoadHeaderEmpty,	// default: FALSE. Just like MF_LoadHeaderOnly, but generates
								// an empty parameter.
	MF_TimeCode,			// default: 0
	MF_StartSample,		// more accurate start point when loading
	MFA_NumChannels,		// For Audio streams
	MFA_BitsPerSample,	// For Audio streams

	MF_StatusPtr = TAG_PTR + 100,
	MF_Document,			// Useful for image creation
	MF_KeyCode,				// default: NULL (returns ptr to string)
	MFP_TimeCodeStr,		// default: NULL (returns ptr to string)
	// maybe others? Different types?

	MFD_FrameRate = TAG_DOUBLE + 100,		// Expected frame rate, in fps
	MFD_Quality,			// 0.0 to 1.0, subjective quality
	MFDA_Offset,			// In time/frames
	MFDA_SampleRate,		// For Audio streams (samples/sec)
};

/** Tags to use in an MediaFormat's Registry Node. These specify
 ** attributes common to all instances, and capabilities of the format.
 **/
enum MediaFormatReg_Tags
{
	MFREG_CanLoad = TAG_INT + REG_USER,						// Default: FALSE
	MFREG_CanSave,											// Default: FALSE
	MFREG_CanLoadMulti,									// Default: FALSE
	MFREG_CanSaveMulti,									// Default: FALSE
	MFREG_Priority,										// Default: 0
	MFREG_WantsIOClass,									// Default: TRUE
	MFREG_LoadLinearOnly,								// Default: FALSE	(currently ignored)
	MFREG_SaveLinearOnly,								// Default: FALSE	(currently ignored)
	MFREG_CanSaveCompressed,							// Default: FALSE	(currently ignored)
	MFREG_OneShotLoad,									// Default: FALSE (delete me after render)
	MFREG_OneShotSave,									// Default: FALSE (delete me after render)

	// Datatypes supported
	MFREG_CanLoadImages,									// Default: FALSE
	MFREG_CanSaveImages,									// Default: FALSE
	MFREG_CanLoadAudio,									// Default: FALSE
	MFREG_CanSaveAudio,									// Default: FALSE
	MFREG_CanLoadText,									// Default: FALSE
	MFREG_CanSaveText,									// Default: FALSE
	MFREG_CanLoadMIDI,									// Default: FALSE
	MFREG_CanSaveMIDI,									// Default: FALSE

	MFREG_ClipSpecificInputValues,					// Means that the contents of any input controls
																// produced are specific to the clip, meaning that
																// AddLoadInputs() should be called when the clip
																// changes - even if the format hasn't.
																// Default: FALSE

	MFREG_WantsUnbufferedIOClass,						// Default: FALSE (the IOClass supplied should be opened with IOC_OPEN_UNBUFFERED)

	MFREG_NoCacheFormatID,								// Default: FALSE. If this is TRUE then Clip will not remember the format ID. The file will then require resolving when the clip is loaded and Open()ed.

	MFREG_LoadSupportsDoD,								// Default: FALSE
	MFREG_SaveSupportsDoD,								// Default: FALSE

	// String tags
	MFREGS_FormatName = TAG_STRING + REG_USER,

	MFREGS_Extension = TAG_STRING | TAG_MULTI + REG_USER,
};

#endif
#endif
