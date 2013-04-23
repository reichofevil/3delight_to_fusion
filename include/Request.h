#ifndef _REQUEST_H_
#define _REQUEST_H_

// Request Flags

// Bottom 8 bits of flags have an effect on the data returned
#define REQF_TypeMask        (0xff)

#define REQF_Quick           (1ul << 0)
#define REQF_StampOnly       (1ul << 1)
#define REQF_NoPreviewResize (1ul << 2)	     // Only used by PreviewSaver - don't do normal preview downscaling
#define REQF_PreCalc         (1ul << 3)        // No actual image processing happens - but images without data will be returned to workout image sizes, etc.
#define REQF_NoMotionBlur    (1ul << 4)        // No motion blur should be done.
#define REQF_MatchSize       (1ul << 5)        // 

#define REQF_NoPutMsg        (1ul << 14)       // If this completes the request, remove it from the RequestList, but don't PutMsg it back to the Op

// Top 17 bits are function/state flags
#define REQF_RenderAll       (1ul << 15)       // Render all tools (ie. selective update set to All. Defaults to 'Some'. Only used when REQF_Interactive not set
#define REQF_Completed       (1ul << 16)
#define REQF_Interactive     (1ul << 17)
#define REQF_Preview         (1ul << 18)

// We do not want anything that may require user intervention
// In general this means a batch/network render of some sort
#define REQF_Quiet           (1ul << 19)

#define REQF_Processing      (1ul << 20)
#define REQF_Intermediate    (1ul << 21)
#define REQF_Remote          (1ul << 22)
#define REQF_Pending         (1ul << 23)

// This flag is used for interactive cycling playback.
// It will typically not be aborted, nor will it abort
// other requests.
#define REQF_Playback        (1ul << 24)

// Used with SerialReqs processing
#define REQF_Serialised      (1ul << 25)

// Use this flag to indicate that it's not the main request time - stops DisplayImage and NotifyChanged
#define REQF_SecondaryTime   (1ul << 26)       // this should not generate notifychanged on any input
#define REQF_ForceNotify     (1ul << 27)       // force a notify on all inputs, even if not connected or matched to a req
#define REQF_RecycleWhenDone (1ul << 28)       // (included for completeness)
#define REQF_NoCache			  (1ul << 29)       // Do not cache the parameter
#define REQF_Background      (1ul << 30)       // Background processing. Kinda like interactive playback, only different :)

#define REQF_Failed          (1ul << 31)

#define REQF_All             (0xffffffff)

#ifndef DFMANAGER

#include "MsgPort.h"
#include "Parameter.h"

#define REQUEST_ID MAKEID('Req!')

class Input;
class Output;
class Operator;
class ImageDomain;



// Flags for GetInputFlags/SetInputFlags. Also for SetInputData/SetRequestData
#define REQIF_NoPutMsg          REQF_NoPutMsg         // If this completes the request, remove it from the RequestList, but don't PutMsg it back to the Op
#define REQIF_RecycleWhenDone   REQF_RecycleWhenDone  // this param not needed anymore
#define REQIF_SecondaryTime     REQF_SecondaryTime    // this should not generate notifychanged
#define REQIF_ForceNotify       REQF_ForceNotify      // force a notify even if not connected or matched to a req




struct ReqData
{
	Parameter *Data;
	TimeStamp Time, BaseTime;
	TimeExtent DataValid;
	bool Set;
	bool Current;
	uint32 Flags;

	int32 spin;

	TagList *Attrs;
};

class FuSYSTEM_API Request : public Message
{
	FuDeclareClass(Request, Message); // declare ClassID static member, etc.

protected:
	Operator *Op;

	int32 NumInputs, NumOutputs;
	int32 InputsPending, CurrentPending;

	ReqData *DataIn;
	ReqData *DataOut;

	// Data sent in request.
	int32 CurrentPri;

	//???	uint32 Error;

public:
	union
	{
		uint32 Flags;
		struct
		{
			unsigned Quick           : 1;
			unsigned StampOnly       : 1;
			unsigned NoPreviewResize : 1;
			unsigned PreCalc         : 1;
			unsigned NoMotionBlur    : 1;
			unsigned MatchSize       : 1;
			unsigned                 : 8;
			unsigned NoPutMsg        : 1;
			unsigned RenderAll       : 1;
			unsigned Completed       : 1;
			unsigned Interactive     : 1;
			unsigned Preview         : 1;
			unsigned Quiet           : 1;
			unsigned Processing      : 1;
			unsigned Intermediate    : 1;
			unsigned Remote          : 1;
			unsigned Pending         : 1;
			unsigned Playback        : 1;
			unsigned Serialised      : 1;
			unsigned SecondaryTime   : 1;
			unsigned ForceNotify     : 1;
			unsigned RecycleWhenDone : 1;
			unsigned NoCache         : 1;
			unsigned Background      : 1;
			unsigned Failed          : 1;
		} FlagBits;
	};

	TimeStamp Time, BaseTime;
	FuUnmappedPointer<Parameter> SourceImg;
	TimeExtent SourceImgValid;

	// This is used to store other requests for the same time, so
	// that they can be filled in and replied, if need be.
	Request *ReqChain;

	int32 spin;

	TagList *Attrs;

	bool IsMappedIn;

	int32 MinPri, MaxPri;

	Request *MotionBlurChain;

	Request(TimeStamp time, uint32 flags = 0, TimeStamp basetime = TIME_UNDEFINED);
	virtual ~Request();

	virtual void Dump();

	Request *Copy() { return new Request(Time, Flags, BaseTime); };
	Request *CopyWithDataIn();

	bool IsQuick() {return (Flags & REQF_Quick) == REQF_Quick; }
	bool IsStampOnly() {return (Flags & REQF_StampOnly) == REQF_StampOnly; }
	bool IsNoPreviewResize() {return (Flags & REQF_NoPreviewResize) == REQF_NoPreviewResize; }
	bool IsPreCalc() {return (Flags & REQF_PreCalc) == REQF_PreCalc; }
	bool IsNoMotionBlur() {return (Flags & REQF_NoMotionBlur) == REQF_NoMotionBlur; }
	bool IsMatchSize() {return (Flags & REQF_MatchSize) == REQF_MatchSize; }
	bool IsCompleted() {return (Flags & REQF_Completed) == REQF_Completed; }
	bool IsInteractive() {return (Flags & REQF_Interactive) == REQF_Interactive; }
	bool IsPreview() {return (Flags & REQF_Preview) == REQF_Preview; }
	bool IsQuiet() {return (Flags & REQF_Quiet) == REQF_Quiet; }
	bool IsProcessing() {return (Flags & REQF_Processing) == REQF_Processing; }
	bool IsIntermediate() {return (Flags & REQF_Intermediate) == REQF_Intermediate; }
	bool IsRemote() {return (Flags & REQF_Remote) == REQF_Remote; }
	bool IsPending() {return (Flags & REQF_Pending) == REQF_Pending; }
	bool IsPlayback() {return (Flags & REQF_Playback) == REQF_Playback; }
	bool IsSerialised() {return (Flags & REQF_Serialised) == REQF_Serialised; }
	bool IsSecondaryTime() {return (Flags & REQF_SecondaryTime) == REQF_SecondaryTime; }
	bool IsNoCache() {return (Flags & REQF_NoCache) == REQF_NoCache; }
	bool IsFailed() {return (Flags & REQF_Failed) == REQF_Failed; }

	TimeStamp GetTime() { return Time; }
	TimeStamp GetBaseTime() { return BaseTime; }
	uint32 GetFlags() { return Flags; }
	TagList *GetAttrs() { return Attrs; }

	int32 GetPri() { return CurrentPri; }
	int32 GetMinPri() { return MinPri; }
	int32 GetMaxPri() { return MaxPri; }
	int32 GetPending() { return CurrentPending; }
	void SetPri(int32 pri, int32 pending);
	void UpdateDoDs(bool resetdomains = false, uint32 matchflags = 0);

	bool SetOp(Operator *op);
	Operator * GetOp() { return Op; }

	void Lock(void);
	void Unlock(void);

	Parameter *GetInputData(Input *input, int slot = 0);
	TimeExtent GetInputDataValid(Input *input, int slot = 0);
	TimeStamp GetInputTime(Input *input, int slot = 0);
	TimeStamp GetInputBaseTime(Input *input, int slot = 0);
	uint32 GetInputFlags(Input *input, int slot = 0);
	TagList *GetInputAttrs(Input *input, int slot = 0);
	bool IsInputSet(Input *input, int slot = 0);
	bool IsInputCurrent(Input *input, int slot = 0);
	bool SetInputData(Input *input, Parameter *param, int slot = 0);
	bool SetInputData(Input *input, Parameter *param, const TimeExtent& valid, int slot = 0, uint32 flags = 0);	// see below for flags
	bool SetInputTime(Input *input, TimeStamp time, int slot = 0);
	bool SetInputBaseTime(Input *input, TimeStamp time, int slot = 0);
	bool SetInputFlags(Input *input, uint32 flags, int slot = 0);
	bool ClearInputData(Input *input, int slot = 0);
	bool ClearInputFlags(Input *input, uint32 flags, int slot = 0);
	bool SetInputAttrs(Input *input, const TagList &attrs, int slot = 0);
	bool SetInputCurrent(Input *input, bool current, int slot = 0);
	bool SetInputRoI(Input *input, const ImageDomain *domain, int slot = 0);
	bool SetInputDoD(Input *input, const ImageDomain *domain, int slot = 0);
	ImageDomain *GetInputRoI(Input *input, int slot = 0);
	ImageDomain *GetInputDoD(Input *input, int slot = 0);
	void UpdateInputDoD(Input *input, int slot = 0, bool resetdomain = false);
	bool DuplicateInputAttrs(Input *input, int slot = 0);
	void FreeAllInputs();

	void LockInputData(Input *input, int slot = 0);
	void UnlockInputData(Input *input, int slot = 0);

	Parameter *GetOutputData(Output *output);
	TimeExtent GetOutputDataValid(Output *output);
	TagList *GetOutputAttrs(Output *output);
	bool IsOutputSet(Output *output);
	bool IsOutputCurrent(Output *output);
	bool SetOutputData(Output *output, Parameter *param);
	bool SetOutputData(Output *output, Parameter *param, const TimeExtent& te);
	bool SetOutputAttrs(Output *output, const TagList &attrs);
	bool SetOutputCurrent(Output *output, bool current);
	bool SetAllOutputsCurrent(bool current);
	bool SetOutputRoI(Output *output, const ImageDomain *domain);
	bool SetOutputDoD(Output *output, const ImageDomain *domain);
	ImageDomain *GetOutputRoI(Output *output);
	ImageDomain *GetOutputDoD(Output *output);
	bool DuplicateOutputAttrs(Output *output);
	void FreeAllOutputs();

	void LockOutputData(Output *output);
	void UnlockOutputData(Output *output);

	void SetReqFlags(uint32 flags);
	void ClearReqFlags(uint32 flags);

	void SetReqAttrs(Tag firsttag = _TAG_DONE, ...);
	void SetReqAttrsTagList(const TagList &attrs);
	void SetReqAttrs(const TagList *attrs);
	bool SetRoI(const ImageDomain *domain);
	bool SetDoD(const ImageDomain *domain);
	ImageDomain *GetRoI();
	ImageDomain *GetDoD();
	bool DuplicateReqAttrs();

	void MapIn();
	void MapOut();

	bool IsSameRequest(Request *req, TimeExtent *te = NULL, bool AllowSwap = false);
	bool OutputsFailed(void);

	virtual void Trash();

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(Request);
#endif
};

#define REQUEST_REQ_BASE		0
enum RequestReqAttrs ENUM_TAGS
{
	RQA_MultiLayer = TAG_INT + REQUEST_REQ_BASE,			// bool

	RQA_BaseTime = TAG_DOUBLE + REQUEST_REQ_BASE,		// When doing motion blur sub-times, indicates the 'base' time

	RQA_DestImg = TAG_OBJECT + REQUEST_REQ_BASE,			// MaskPaint/MaskOperator use this to create mask image earlier

	RQA_DestImgTags = TAG_TAGLIST + REQUEST_REQ_BASE,	// MaskPaint/MaskOperator use this to create mask image earlier

	RQA_Request = TAG_PTR + REQUEST_REQ_BASE,				// Used by Operator to pass the Request to Output::SetRequestData
};

#endif
#endif
