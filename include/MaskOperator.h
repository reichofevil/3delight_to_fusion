#ifndef _MASKOPERATOR_H_
#define _MASKOPERATOR_H_

#include "ThreadedOperator.h"

class FuSYSTEM_API MaskOperator : public ThreadedOperator
{
	FuDeclareClass(MaskOperator, ThreadedOperator); // declare ClassID static member, etc.

protected:
	Input *InLevel, *InSoftEdge, *InType, *InPasses, *InPaintMode;
	Input *InThickness, *InInvert, *InSolid, *InShowControls;
	Input *InFieldMode, *InUseVidMode, *InMaskWidth, *InMaskHeight, *InPixelAspect, *InMaskDepth;
	Input *InSizeMode, *InClippingMode;
	Output *OutMask;

private:
	Image  *Img;

public:
	int Width, Height;
	double XScale, YScale;
	double XOffset, YOffset;
	
	bool ControlsVisible;
	bool m_bDoneAutoSize;

	uint8 Pad[256];

protected:
	Image *CreateMaskImage(Request *req, TagList *overridetags = NULL);
	ImageDomain *GetMaskDoD(Request *req);

public:
	MaskOperator(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~MaskOperator();
	virtual void OnDelete(bool disconnect = true, TagList *tags = NULL);

	virtual void CheckRequest(Request *req);
	virtual TimeExtent GetValid(Request *req, TagList *tags = NULL);

	virtual void PreCalcPreProcess(Request *req);
	virtual void PreCalcProcess(Request *req);
	virtual void PreCalcPostProcess(Request *req);

	virtual void PreProcess(Request *req);
	virtual void PostProcess(Request *req);
	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);

	virtual bool OnConnect(Link *src, Link *dest, TagList *tags = NULL);
	virtual void OnLastDisconnect(TagList *tags = NULL);

	virtual void InvalidateCache(Input *in = NULL, TagList *tags = NULL);
	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);
	virtual void ShowControls(bool show);
	virtual bool CopyAttr(Tag tag, void *addr) const;		// for OP_ToolText messages

	virtual void AddToMenu(Menu *menu, TagList &tags);
	virtual void HandleMenu(uint32 cmd, uint_ptr *data, TagList &tags);

	void ClearModeList(void);

	bool IsVisibleToolEnabled(Output *out);
	Image *GetMaskImage(void);
	void SetMaskImage(Image *img);

	bool DoSoften(Image *in, Image *out, double xblur, double yblur, double blend);
	bool DoGauss(Image *in, Image *out, double xblur, double yblur);
	bool DoMultiBox(Image *in, Image *out, double xblur, double yblur, int32 passes);

	Image *DoMaskBlur(Image *in, Image *out, double xblur, double yblur, BlurTypes type, int32 passes, bool clip);
};


enum MaskOperator_RegTags ENUM_TAGS
{
	MOR_NoDIBNeeded = TAG_INT + 100,
};

#endif
