#ifndef _SOURCEOPERATOR_H_
#define _SOURCEOPERATOR_H_

#include "ThreadedOperator.h"

enum SourceOperator_Attrs
{
	SO_Loop = TAG_INT + 200,
};


class FuSYSTEM_API SourceOperator : public ThreadedOperator
{
	FuDeclareClass(SourceOperator, ThreadedOperator); // declare ClassID static member, etc.

protected:
	Input *InFieldMode;
	Input *InGlobalStart, *InGlobalEnd;
	Input *InUseVidMode;
	Input *InWidth, *InHeight;
	Input *InPixelAspect;
	Input *InSourceDepth;
	bool ExtentSet;

	Output *OutImage;
	
	bool DragStart, DragEnd;  // erk!
	TimeStamp DragXInit, StartPos, EndPos;      // erk!

	List modelist;

public:
	int32 FieldMode;
	int32 Field, Scale;

	int32 width, height;
	float64 xaspect, yaspect;
	uint32 SourceDepth;

	Input *InClippingMode;
	FuID ClippingModeID;

	Input *InColorSpace, *InMakeNonLinear;

	uint8 Pad[256 - sizeof(Input *) * 3 - sizeof(FuID)];

public:
	SourceOperator(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~SourceOperator();
	virtual bool Save(ScriptVal &table, const TagList &tags);

	virtual void OnGlobalExtentChanged(TimeExtent *globext, TagList *tags = NULL);
	virtual void CheckRequest(Request *req);
	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);
	virtual void OnAddToFlow(TagList *tags = NULL);
	virtual TimeExtent GetValid(Request *req, TagList *tags = NULL);

	virtual void PreCalcPreProcess(Request *req);
	virtual void PreCalcProcess(Request *req);
	virtual void PreCalcPostProcess(Request *req);

	virtual void PreProcess(Request *req);
	virtual void PostProcess(Request *req);

	virtual TimeStamp GetClipLength();
	virtual bool CopyAttr(Tag tag, void *addr) const;

	virtual void KFDraw(KFContext *kfc, TagList *tags = NULL);
	virtual bool KFDown(KFContext *kfc, float32 x, float32 y, TagList *tags = NULL);
	virtual bool KFDrag(KFContext *kfc, float32 x, float32 y, TagList *tags = NULL);
	virtual void KFUp(KFContext *kfc, float32 x, float32 y, TagList *tags = NULL);
	virtual bool KFCommand(KFContext *kfc, float32 x, float32 y, KFCmd id, void *data, TagList *tags = NULL);
	virtual bool KFGetTip(KFContext *kfc, float32 x, float32 y, char *buf, TagList *tags = NULL);

	virtual void AddToMenu(Menu *menu, TagList &tags);
	virtual void HandleMenu(uint32 cmd, uint_ptr *data, TagList &tags);

	virtual void OnDelete(bool disconnect, TagList *tags = NULL);

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	void ClearModeList(void);

	// Just to make sure there's an implementation, so that derived classes don't
	// skip this should we ever want to do something in these functions
	virtual void GetAvailableEvents(EventList *events, const FuID &eventid = NOID);
	virtual bool OnEvent(const FuID &eventid, uint_ptr subid, TagList &tags);
};

// DFIDs for InFieldMode input
extern FuSYSTEM_API const FuID ProcessMode_FullFrames;
extern FuSYSTEM_API const FuID ProcessMode_PAL;
extern FuSYSTEM_API const FuID ProcessMode_NTSC;
extern FuSYSTEM_API const FuID ProcessMode_NTSC_rev;
extern FuSYSTEM_API const FuID ProcessMode_PAL_rev;
extern FuSYSTEM_API const FuID ProcessModeIDArray[6];
extern FuSYSTEM_API const int ProcessModeArray[5];

extern FuSYSTEM_API const FuID ClippingMode_None;
extern FuSYSTEM_API const FuID ClippingMode_Frame;
extern FuSYSTEM_API const FuID ClippingModeIDArray[3];

#endif

