#ifndef _IMAGEDOMAIN_H_
#define _IMAGEDOMAIN_H_

#include "types.h"
#include "Object.h"
#include "DFRect.h"

class Parameter;
class Request;
class Input;
class Output;

#ifndef REQF_All
#define REQF_All             (0xffffffff)
#endif

#ifndef MAX_IMAGE_CHANNELS
#define MAX_IMAGE_CHANNELS		128
#endif

extern FuSYSTEM_API const FuRectInt FuDomainRectNull;
extern FuSYSTEM_API const FuRectInt FuDomainRectAll;

class LayerDomain : public RefObject
{
public:
	LayerDomain();
	LayerDomain(const LayerDomain &domain);
	LayerDomain(const Parameter *param);

	virtual ~LayerDomain();

	const Registry *ImageRegNode;

	int32 Depth, PixelSize, PixelSizeAux;

	union
	{
		struct
		{
			int32 RChan;         // int8 or int16
			int32 GChan;
			int32 BChan;
			int32 AChan;

			int32 BgRChan;       // int8 or int16
			int32 BgGChan;
			int32 BgBChan;
			int32 BgAChan;

			int32 RealRChan;     // int8 or int16
			int32 RealGChan;
			int32 RealBChan;
			int32 RealAChan;

			int32 ZChan;         // float32
			int32 UChan;
			int32 VChan;

			int32 CoverageChan;  // int16
			int32 ObjectChan;
			int32 MaterialChan;

			int32 NormalXChan;   // int16
			int32 NormalYChan;
			int32 NormalZChan;

			int32 VectorXChan;   // float32
			int32 VectorYChan;

			int32 BackVectorXChan;   // float32
			int32 BackVectorYChan;
		};

		int32 Chan[MAX_IMAGE_CHANNELS];
	};

	void *CanvasColorData;
	void *CanvasColorDataAux;
	bool bCanvasClear;

#ifndef USE_NOSCRIPTING
	ScriptVal CustomData;
#endif
};

class FuSYSTEM_API RectDomain : public Object
{
public:
	FuRect rect;
};

class FuSYSTEM_API ImageDomain : public Object
{
public:
	ImageDomain();
	ImageDomain(const ImageDomain &domain);
	ImageDomain(const ImageDomain &domain, int32 l, int32 t, int32 r, int32 b) { SetDomain(domain); SetValid(l, t, r, b); }
	ImageDomain(const ImageDomain &domain, const FuRectInt &valid) { SetDomain(domain); ValidWindow = valid; }
	ImageDomain(const Parameter *param, Request *req = NULL);

	virtual ~ImageDomain();

	virtual void Use();			// For Usecounts
	virtual void Recycle();

	Parameter *CreatePreCalc(FusionDoc *doc, uint32 flags);

public:
	const Registry *ImageRegNode;

	int32 OriginalWidth, OriginalHeight;
	float64 OriginalXScale, OriginalYScale;

	int32 Width, Height;
	float64 XScale, YScale;

	float64 XOffset, YOffset;
	int32 Field, Depth, PixelSize, PixelSizeAux;

	int32 ProxyScale;
	bool AutoProxy;

	FuRectInt ImageWindow, ValidWindow;

	union
	{
		struct
		{
			int32 RChan;         // int8 or int16
			int32 GChan;
			int32 BChan;
			int32 AChan;

			int32 BgRChan;       // int8 or int16
			int32 BgGChan;
			int32 BgBChan;
			int32 BgAChan;

			int32 RealRChan;     // int8 or int16
			int32 RealGChan;
			int32 RealBChan;
			int32 RealAChan;

			int32 ZChan;         // float32
			int32 UChan;
			int32 VChan;

			int32 CoverageChan;  // int16
			int32 ObjectChan;
			int32 MaterialChan;

			int32 NormalXChan;   // int16
			int32 NormalYChan;
			int32 NormalZChan;

			int32 VectorXChan;   // float32
			int32 VectorYChan;

			int32 BackVectorXChan;   // float32
			int32 BackVectorYChan;
		};

		int32 Chan[MAX_IMAGE_CHANNELS];
	};

	void *CanvasColorData;
	void *CanvasColorDataAux;
	bool bCanvasClear;

#ifndef USE_NOSCRIPTING
	ScriptVal CustomData;
#endif
	uint8 pad1[128];

protected:
	int m_NumLayers;
	LayerDomain **m_LayerDomains;
	FuID *m_LayerNames;

	uint8 pad2[128];

protected:
	uint32 ReqFlags;

	const Registry *SourceImageRegNode;

	int32 SourceWidth, SourceHeight;
	float64 SourceXScale, SourceYScale;

	float64 SourceXOffset, SourceYOffset;
	int32 SourceField, SourceDepth;

	int32 SourceProxyScale;

	uint8 pad3[128];

public:
	void SetDomain(const ImageDomain &domain);

	bool operator == (const ImageDomain &domain) const;
	ImageDomain operator | (const ImageDomain &domain) const;
	ImageDomain operator & (const ImageDomain &domain) const;
	ImageDomain & operator |= (const ImageDomain &domain);
	ImageDomain & operator &= (const ImageDomain &domain);

	ImageDomain operator | (const FuRectInt &rect) const;
	ImageDomain operator & (const FuRectInt &rect) const;
	ImageDomain & operator |= (const FuRectInt &rect);
	ImageDomain & operator &= (const FuRectInt &rect);

	bool IsEmpty() const															{ return ValidWindow.IsNull(); }
	void SetEmpty()																{ ValidWindow.SetNull(); }

	bool IsWithin(const ImageDomain &domain) const;

	void SetValid(int32 l, int32 b, int32 r, int32 t)					{ ValidWindow.Set(l, b, r, t); }
	void GetValid(int32 &l, int32 &b, int32 &r, int32 &t) const		{ ValidWindow.Get(l, b, r, t); }

	void ValidSize(int32 &w, int32 &h) const								{ w = ValidWindow.Width(); h = ValidWindow.Height(); }
	int32 ValidWidth() const													{ return ValidWindow.Width(); }
	int32 ValidHeight() const													{ return ValidWindow.Height(); }

	void ImageSize(int32 &w, int32 &h) const								{ w = ImageWindow.Width(); h = ImageWindow.Height(); }
	int32 ImageWidth() const													{ return ImageWindow.Width(); }
	int32 ImageHeight() const													{ return ImageWindow.Height(); }

	void Offset(int32 x, int32 y)												{ ValidWindow.Offset(x, y); }
	void Inflate(int32 x, int32 y)											{ ValidWindow.Inflate(x, y); }
	void Normalise()																{ ValidWindow.Normalise(); }
	void FlipX()																	{ ValidWindow.FlipX(); }
	void FlipY()																	{ ValidWindow.FlipY(); }

	void ClipToImageWindow();
	void ClipTo(const FuRectInt &window);
	void ClipTo(const ImageDomain &domain);
	void ClipTo(const ImageDomain *domain);

	uint32 GetFlags() { return ReqFlags; }
	bool IsComparable(const Parameter *param, uint32 Flags) const;

	LayerDomain *FindLayerDomain(const FuID &name);
	LayerDomain *GetLayerDomain(int i);
	const FuID &GetLayerName(int i);
	int GetNumLayers();

	bool IsMultiLayer(void);

	// These don't modify DFR_NULL or -DFR_NULL, so can be used on a 'ValidWindow'
	static void Scale(FuRectInt &rect, float32 x) { Scale(rect, x, x); }
	static void Scale(FuRectInt &rect, float32 x, float32 y);
};

class FuSYSTEM_API AutoImageDomain
{
protected:
	ImageDomain *Domain;

public:
	// Adds an extra ref count to an existing ImageDomain
	AutoImageDomain(const AutoImageDomain &domain)	{ if (Domain = domain.Domain) Domain->Use(); }
	AutoImageDomain(ImageDomain *domain)	{ Domain = domain; }

	// Creates a copy of an ImageDomain
	AutoImageDomain(const ImageDomain &domain, const ImageDomain *valid = NULL)	{ Domain = new ImageDomain(domain); if (valid) Domain->ValidWindow = valid->ValidWindow; }
	AutoImageDomain(const ImageDomain &domain, const FuRectInt &valid)				{ Domain = new ImageDomain(domain); Domain->ValidWindow = valid; }

	~AutoImageDomain()							{ if (Domain) Domain->Recycle(); }

	ImageDomain *operator = (ImageDomain *newdomain);
	const AutoImageDomain &operator = (const AutoImageDomain &p);

	operator ImageDomain *() const			{ return (ImageDomain *) Domain; }
	const ImageDomain *operator ->() const	{ return (ImageDomain *) Domain; }
	ImageDomain *operator ->()					{ return (ImageDomain *) Domain; }
};

class FuSYSTEM_API AutoImageDoD : public AutoImageDomain
{
public:
	// Adds an extra ref count to an existing ImageDomain
	AutoImageDoD(const AutoImageDomain &domain) : AutoImageDomain(domain) { }
	AutoImageDoD(ImageDomain *domain) : AutoImageDomain(domain) { }

	// Creates a copy of an ImageDomain
	AutoImageDoD(const ImageDomain &domain, const ImageDomain *valid = NULL) : AutoImageDomain(domain, valid)	{ }
	AutoImageDoD(const ImageDomain &domain, const FuRectInt &valid) : AutoImageDomain(domain, valid)				{ }

	AutoImageDoD(Request *req);
	AutoImageDoD(Request *req, Input *inp, int slot = 0);
	AutoImageDoD(Request *req, Output *out);

	AutoImageDoD(Input *inp, TimeStamp time, uint32 flags, uint32 flagmask = REQF_All);

	ImageDomain *operator = (ImageDomain *newdomain) { return AutoImageDomain::operator = (newdomain); }
	const AutoImageDoD &operator = (const AutoImageDoD &p) { return (const AutoImageDoD &) AutoImageDomain::operator = (p); }
};

class FuSYSTEM_API AutoImageRoI : public AutoImageDomain
{
public:
	// Adds an extra ref count to an existing ImageDomain
	AutoImageRoI(const AutoImageDomain &domain) : AutoImageDomain(domain) { }
	AutoImageRoI(ImageDomain *domain) : AutoImageDomain(domain) { }

	// Creates a copy of an ImageDomain
	AutoImageRoI(const ImageDomain &domain, const ImageDomain *valid = NULL) : AutoImageDomain(domain, valid)	{ }
	AutoImageRoI(const ImageDomain &domain, const FuRectInt &valid) : AutoImageDomain(domain, valid)				{ }

	AutoImageRoI(Request *req);
	AutoImageRoI(Request *req, Input *inp, int slot = 0);
	AutoImageRoI(Request *req, Output *out);

	ImageDomain *operator = (ImageDomain *newdomain) { return AutoImageDomain::operator = (newdomain); }
	const AutoImageRoI &operator = (const AutoImageRoI &p) { return (const AutoImageRoI &) AutoImageDomain::operator = (p); }
};

#endif // _IMAGEDOMAIN_H_
