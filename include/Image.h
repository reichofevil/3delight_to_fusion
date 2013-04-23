#ifndef _IMAGE_H_
#define _IMAGE_H_

#ifndef DFMANAGER

#include "Input.h"
#include "Parameter.h"
#include "Pixel.h"
#include "TagList.h"
#include "List.h"
#include "MemoryManager.h"
#include "DFRect.h"
#include "ImageDomain.h"


class FusionDoc;
class LookUpTable;

#ifndef DFLIB
	class Pbuffer;
	class ShaderARB;
	class GLTexture2D;
	class TexResource;
#endif

class ColorMatrix;
class ColorMatrixFull;
class LookUpTable;
class LookUpTable3D;

#ifndef MAX_IMAGE_CHANNELS
#define MAX_IMAGE_CHANNELS		128
#endif

class Image;

struct tagDIBSECTION;

typedef struct tagDIBSECTION DIBSECTION;
typedef struct tagBITMAPINFOHEADER BITMAPINFOHEADER;

typedef void (*MFPTR)(Image *img, void *data, int32 chunk);
typedef void (__fastcall *PIXFPTR)(Pixel &pix, void *ptr, void *auxptr, Image *img);
typedef void (__fastcall *PIXFFPTR)(FltPixel &pix, void *ptr, void *auxptr, Image *img);
typedef void (__fastcall *SAMPFPTR)(Image *img, int fx, int fy, Pixel &pix);
typedef void (__fastcall *SAMPFFPTR)(Image *img, int fx, int fy, FltPixel &pix);
typedef void (*SAMPAREAFPTR)(Image *img, int32 fx1, int32 fy1, int32 fx2, int32 fy2, int32 fx3, int32 fy3, int32 fx4, int32 fy4, Pixel &pix);
typedef void (*SAMPAREAFFPTR)(Image *img, int32 fx1, int32 fy1, int32 fx2, int32 fy2, int32 fx3, int32 fy3, int32 fx4, int32 fy4, FltPixel &pix);
typedef void (__fastcall *CHANFPTR)(void *ptr, void *dest, Image *img, int chan);
typedef void * (*COPYSCANFPTR)(void *dest, const void *src, size_t count);

typedef void (*MERGEAPPLYFPTR)(Pixel &fg, Pixel &bg, uint32 addsub, uint32 gain, void *data);				// dest is 'bg'
typedef void (*MERGEAPPLYFLTFPTR)(FltPixel &fg, FltPixel &bg, float32 addsub, float32 gain, void *data);	// dest is 'bg'

enum LegalVidType
{
	LVT_NTSC = 0,
	LVT_NHK,
	LVT_PAL_SECAM,
};

enum LegalMethod
{
	LM_Legal = 0,
	LM_Black,
	LM_White,
};

// CopyField() function codes
enum CFT_FromFieldTags		// for "from_field" arg
{
	CFT_FromField  = -1,		// source is a field (must have IMAT_Field set if used with CFT_Auto)
	CFT_FromField0 =  0,		// extract field 0 from source frame
	CFT_FromField1 =  1,		// extract field 1 from source frame
};
enum CFT_ToFieldTags			// for "to_field" arg
{
	CFT_ToField  = -1,		// dest is a field (i.e. half height; ignores interpolate)
	CFT_ToField0 =  0,		// dest is a frame; copy to field 0
	CFT_ToField1 =  1,		// dest is a frame; copy to field 1
	CFT_Auto     =  2,		// dest is a frame; copy to same fieldnum as src
};
enum CFT_InterpolateTags	// for "interpolate" arg - ignored if dest is not a frame
{
	CFT_OneFieldOnly = 1,	// don't touch other field
	CFT_Double,					// double this field
	CFT_Interpolate,			// interpolate to other field
};


// Merge/Transform
enum EdgeModes
{
	EM_Black,
	EM_Canvas = EM_Black,
	EM_Wrap,
	EM_Duplicate,
	EM_Mirror,
};

enum InterpModes
{
	IM_Nearest = 0,
	IM_BiLinear,
	IM_BiLinearSAT,
	IM_BiCubic,
	IM_CatRom,
};

// Image Process Tags.
// These tags can be passed to any Image process function (ie Merge, Blur etc).

#define IMGPR_BASE TAGOFFSET

enum ImageProcessTags
{
	IMGPR_DoProgress = TAG_INT + IMGPR_BASE,
	IMGPR_NoAbort,

	IMGPR_ProgressStart = TAG_DOUBLE + IMGPR_BASE,
	IMGPR_ProgressEnd,

	IMGPR_Operator = TAG_PTR + IMGPR_BASE,
};

enum TransformOptions
{
	XF_EdgeMode = TAG_INT,

	XF_XOffset = TAG_DOUBLE,
	XF_YOffset,
	XF_XAxis,
	XF_YAxis,
	XF_XSize,
	XF_YSize,
	XF_Angle,
};

enum RszTypes
{
	// filtering method to be used while resizing
	RSZ_TopLeft = 0,
	RSZ_Nearest,
	RSZ_FirstFilter,
	RSZ_Box = RSZ_FirstFilter,
	RSZ_BiLinear,
	RSZ_BiCubic,
	RSZ_BSpline,
	RSZ_CatRom,
	RSZ_Gaussian,
	RSZ_Mitchell,
	RSZ_Lanczos,
	RSZ_Sinc,			// Also uses windowing method
	RSZ_Bessel,			// Also uses windowing method
	RSZ_LastFilter = RSZ_Bessel,

	// windowing method to be used (for those filters that need it)
	RSZ_FirstWindow = 100,
	RSZ_Hanning = RSZ_FirstWindow,
	RSZ_Hamming,
	RSZ_Blackman,
	RSZ_Kaiser,
	RSZ_LastWindow = RSZ_Kaiser,
};

enum ResizeOptions
{
	RSZ_Width = TAG_INT,
	RSZ_Height,
	RSZ_Depth,
	RSZ_Filter,
	RSZ_Window,
	RSZ_EdgeMode,
	RSZ_XEdgeMode = RSZ_EdgeMode,
	RSZ_YEdgeMode,

	RSZ_XSize = TAG_DOUBLE,
	RSZ_YSize,
	RSZ_XOffset,
	RSZ_YOffset,

	RSZ_SrcBounds = TAG_PTR,		// Pointer to FuRectInt
};

enum BlurTypes
{
	BT_Box,
	BT_Soften,
	BT_Bartlett,
	BT_Sharpen,
	BT_Gaussian,
	BT_Hilight,
	BT_Blend,
	BT_Solarise,
	BT_MultiBox,
};

enum BlurOptions
{
	BLUR_Type = TAG_INT,
	BLUR_Red,
	BLUR_Green,
	BLUR_Blue,
	BLUR_Alpha,
	BLUR_ExpandToFit, // Only valid for BlurOfImage()
	BLUR_Passes,		// Used when BLUR_Type is BT_MultiBox

	BLUR_XSize = TAG_DOUBLE,
	BLUR_YSize,
	BLUR_Blend,
	BLUR_Normalize,
	BLUR_RedScale,
	BLUR_GreenScale,
	BLUR_BlueScale,
	BLUR_AlphaScale,

	BLUR_InChans = TAG_PTR,
	BLUR_OutChans,
	BLUR_ClipSource,	// FuRectInt defining source bounds to use
};

enum ApplyOperators
{
	MAO_Over,
	MAO_In,
	MAO_HeldOut,
	MAO_Atop,
	MAO_XOr,
};

enum MergeOptions
{
	MO_EdgeMode = TAG_INT,
	MO_DoZ,
	MO_UseOpenGL,
	MO_ApplyMode,
	MO_ApplyOperator,

	MO_MustDoCopy,                   // Default's to TRUE. If the fg is out of view, or there is no fg, can't just pass the bg as the result
	MO_BBoxOnly,                     // Only process bounding box, even if it may screw up a depth merge.

	MO_XOffset = TAG_DOUBLE,
	MO_YOffset,
	MO_XAxis,
	MO_YAxis,
	MO_XSize,
	MO_YSize,
	MO_Angle,
	MO_FgZOffset,
	MO_FgAlphaGain,
	MO_FgAddSub,
	MO_BgZOffset,
	MO_BgAlphaGain,
	MO_BgAddSub,

	MO_BurnIn,

	MO_FgRedGain,
	MO_FgGreenGain,
	MO_FgBlueGain,

	MO_CustomApplyMode = TAG_PTR,		// MERGEAPPLYFPTR
	MO_CustomApplyFltMode,				// MERGEAPPLYFLTFPTR
	MO_CustomApplyData,
	MO_DestImage,
	MO_MixRanges,
};

enum MergeApplyModes
{
	MAM_Merge = 0,
	MAM_Screen,
	MAM_Dissolve,
	MAM_Multiply,
	MAM_Overlay,
	MAM_SoftLight,
	MAM_HardLight,
	MAM_ColorDodge,		// Color Dodge
	MAM_ColorBurn,	// Color Burn
	MAM_Darken,
	MAM_Lighten,
	MAM_Difference,
	MAM_Exclusion,	// Exclusion
	MAM_Hue,
	MAM_Saturation,
	MAM_Color,
	MAM_Luminosity,
	MAM_LastMergeApplyMode = MAM_Luminosity,
};

enum ApplyColorMatrixOptions
{
	ACM_ClipBlack = TAG_INT,		// Default: FALSE
	ACM_ClipWhite,						// Default: FALSE

	ACM_PreGamma = TAG_DOUBLE,		// Default: 1.0
	ACM_PostGamma,						// Default: 1.0
};


extern "C" FuSYSTEM_API uint32 * const image_lut8to16;
extern "C" FuSYSTEM_API float32 * const image_lut8toflt;
extern "C" FuSYSTEM_API float32 * const image_lut16toflt;
extern "C" FuSYSTEM_API float16 * const image_lut8toflt16;
extern "C" FuSYSTEM_API float16 * const image_lut16toflt16;

// Does subtractive merging
#define INT_LERP(p,q,a,t)					((q) > (p)) ? ((p) + INT_MULT(a, ((q) - (p)), t)) : ((p) - INT_MULT(a, ((p) - (q)), t))
#define FLT_LERP(p,q,a)						((p) + (a * ((q) - (p))))
#define INT_SAFE_LERP(p,q,a,t)			((q) > (p)) ? (((p) + INT_MULT(a, ((q) - (p)), t)) & 0xFFFF) : (((p) - INT_MULT(a, ((p) - (q)), t)) & 0xFFFF)
//#define INT_LERP(p,q,a,t)				((p) + INT_MULT(a, ((q) - (p)), t))
//#define INT_SAFE_LERP(p,q,a,t)			(((p) + INT_MULT(a, ((q) - (p)), t)) & 0xFFFF)

// Does additive merging
#define INT_PRELERP(p,q,a,t)				((p) + (q) - INT_MULT(a, p, t))
#define FLT_PRELERP(p,q,a)					((p) + (q) - (a * p))

// Does blend
#define INT_BLEND(p,q,a,t)					(INT_MULT((p)-(q), a, t) + (q))
#define FLT_BLEND(p,q,a)					((((p)-(q)) * a) + (q))

#define FuLimit(x)	(((x) > 65535) ? 65535 : (((x) < 0) ? 0 : (x)))
#define FuLimitU(x)	(((x) > 65535) ? 65535 : (x))
#define FuLimitS(x)	(((x) > 32767) ? 32767 : (((x) < -32767) ? -32767 : (x)))

#define FuFLTLimit(x)	(((x) > 1.0f) ? 1.0f : (((x) < 0.0f) ? 0.0f : (x)))
#define FuFLTLimitU(x)	(((x) > 1.0f) ? 1.0f : (x))

#define FuLimit8(x)	(((x) > 255) ? 255 : (((x) < 0) ? 0 : (x)))
#define FuLimitU8(x)	(((x) > 255) ? 255 : (x))

typedef int32 pos;
#define ToPos(f) pos((f) * 65536)
#define FracPos(p) ((p) & 65535)
#define FloorPos(p) ((p) >> 16)
#define RoundPos(p) (((p) + 32768) >> 16)

extern FuSYSTEM_API Pixel clearp;
extern FuSYSTEM_API FltPixel fltclearp;

enum ChanTypes
{
	CHANTYPE_I8U = 0,
	CHANTYPE_I8S,
	CHANTYPE_I8UtoI16U,
	CHANTYPE_I8StoI16S,
	CHANTYPE_I16U,
	CHANTYPE_I16S,
	CHANTYPE_F16asI16U,
	CHANTYPE_F16asI16S,
	CHANTYPE_F16,
	CHANTYPE_F16asF32,
	CHANTYPE_F32asI16U,
	CHANTYPE_F32asI16S,
	CHANTYPE_F32,
};

class Operator;
class FuSYSTEM_API ImgMPData
{
public:
	Operator *Op;
	bool DoProgress;
	float32 ProgressStart, ProgressEnd;

public:
	ImgMPData();
	ImgMPData(Operator *_op, bool _doprogress, float32 _start, float32 _end) : Op(_op), DoProgress(_doprogress), ProgressStart(_start), ProgressEnd(_end) { };

	void SetProgress(float32 prog);
};

// ImageBuffer class, holds one or more related channels
class FuSYSTEM_API ImageBuffer : public Object
{
};

class FuSYSTEM_API Image : public Parameter
{
	FuDeclareClassX(Image, Parameter, Image); // declare ClassID static member, etc.
	FuDeclareClassX(Image, Parameter, Mask);

protected:
	void *_Data;
	volatile long TextureDirtyFlag;

protected:
	void RGBtoYUV(TagList &tags);
	void YUVtoRGB(TagList &tags);
	void RGBtoHLS(TagList &tags);
	void HLStoRGB(TagList &tags);
	void RGBtoCMY(TagList &tags);
	void CMYtoRGB(TagList &tags);
	void RGBtoYIQ(TagList &tags);
	void YIQtoRGB(TagList &tags);
	void RGBtoHSV(TagList &tags);
	void HSVtoRGB(TagList &tags);
	void RGBtoXYZ(TagList &tags);
	void XYZtoRGB(TagList &tags);
	void RGBtoLAB(TagList &tags);
	void LABtoRGB(TagList &tags);

	void Init(TagList &tags);

public:
	Image(const Registry *reg, const ScriptVal &table, const TagList &tags);

	Image(Tag firsttag = _TAG_DONE, ...);
	Image(TagList &tags);
	virtual ~Image();

	static const Image &Get(Input *in, Request *req) { return *(Image *) in->GetValue(req); }

	HANDLE GetDIB();
	HANDLE GetDIBResize(int32 w, int32 h);
	void FreeDIB(HANDLE bm);

	FusionDoc *Doc;

	LinePtr *ScanLine, *ScanLineAux;

	int32 Width, Height;
	int32 RowModulo, RowModuloAux;
	int32 Depth;               // An enum from DepthTags, not an actual bit depth

	FuRectInt ImageWindow, DataWindow, ValidWindow;

	int32 Quality;
	int32 RealQuality;

	float64 XScale, YScale;
	float64 XOffset, YOffset;

	void *RLUTOut;
	void *GLUTOut;
	void *BLUTOut;
	void *ALUTOut;

	void *RLUTIn;
	void *GLUTIn;
	void *BLUTIn;
	void *ALUTIn;

	COPYSCANFPTR CopyScanLine;
	COPYSCANFPTR CopyScanLineAux;

	int32 *ChanSize;
	int32 *ChanType;

	int32 PixelSize;		// Bytes per pixel for color channels including any pad
	int32 PixelSizeAux;	// Bytes per pixel for aux channels including any pad

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

	PIXFPTR GetPixelFunc;
	PIXFPTR SetPixelFunc;
	PIXFPTR GetAuxPixelFunc;
	PIXFPTR SetAuxPixelFunc;

	SAMPFPTR SampleBFunc;
	SAMPFPTR SampleDFunc;
	SAMPFPTR SampleWFunc;

	CHANFPTR GetChanFunc[MAX_IMAGE_CHANNELS];
	CHANFPTR SetChanFunc[MAX_IMAGE_CHANNELS];

	PIXFFPTR GetFltPixelFunc;
	PIXFFPTR SetFltPixelFunc;
	PIXFFPTR GetAuxFltPixelFunc;
	PIXFFPTR SetAuxFltPixelFunc;

	SAMPFFPTR FltSampleBFunc;
	SAMPFFPTR FltSampleDFunc;
	SAMPFFPTR FltSampleWFunc;

	SAMPAREAFFPTR FltSampleAreaBFunc;
	SAMPAREAFFPTR FltSampleAreaDFunc;
	SAMPAREAFFPTR FltSampleAreaWFunc;

	CHANFPTR GetFltChanFunc[MAX_IMAGE_CHANNELS];
	CHANFPTR SetFltChanFunc[MAX_IMAGE_CHANNELS];

	LockableObject SATLock;
	SAMPAREAFPTR SampleAreaBFunc;
	SAMPAREAFPTR SampleAreaDFunc;
	SAMPAREAFPTR SampleAreaWFunc;

	LinePtr *SATLines, *ySATLines;
	uint32 SATProxyBits, SATProxyMask;
	uint32 SATPad1, SATPad2, SATPad3;

	uint32 SATRef;
	bool SATFlagDestroy;

	float32 PrimaryDepthCanonicalisationCoefficient;

	bool IsDataContiguous, IsDataContiguousAux;						// If FALSE, image data could be made from multiple allocated chunks

	LockableObject LUTLock;

	MemPointer m_ImageMem, m_ImageMemAux;
	MemPointer m_SATMem, m_ySATMem;

	bool m_IsReadOnly;

	uint32 m_nMaxValidChan;
	uint_ptr SATMemSize, xSATMemSize, ySATMemSize;

	uint32 *m_Status;

	FuRectInt ValidDataWindow;

	void *CanvasColorData, *CanvasColorDataAux;
	bool m_bCanvasClear;

protected:
	int m_NumLayers;
	Image **m_LayerImages;
	FuID *m_LayerNames;

	FuSafeUnmappedPtr<Image> m_ProxyImage;
	FuSafeUnmappedPtr<Image> m_CroppedImage;

	bool IsOpaqueFlag;
	bool IsOpaqueFlagValid;

protected:
	bool AllocScanLines(void);					// RowModulo and Height must already be set.
	void FreeImageScanLines(void);

	bool AllocScanLinesAux(void);					// RowModulo and Height must already be set.
	void FreeImageScanLinesAux(void);

	void FreeSATLines(void);

#ifndef DFLIB
	bool DoMergeGL(Image *fg, Image *bg, Image *out, ShaderARB *shader, MergeApplyModes mam);
#endif

public:
	virtual bool CopyAttr(Tag tag, void *addr) const;

	virtual Parameter *Copy();
	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	Image(Image &param);

	virtual void Use();
	virtual void Recycle();

	void AddLayer(Image *img, const FuID &name);
	Image *FindLayerImage(const FuID &name);
	Image *GetLayerImage(int i);
	const FuID &GetLayerName(int i);
	int GetNumLayers();

	bool DoMultiProcess(MFPTR func, void *data, int32 numchunks, bool noabort = false);
	bool DoMultiProcess(MFPTR func, void *data, int32 numchunks, ImgMPData *mpdata, bool noabort = false);

	bool IsAInt8()			{ return (IsInt()   && PixelSize ==  1 && AChan == 0); }
	bool IsAInt16()		{ return (IsInt()   && PixelSize ==  2 && AChan == 0); }
	bool IsAFlt16()		{ return (IsFloat() && PixelSize ==  2 && AChan == 0); }
	bool IsAFlt32()		{ return (IsFloat() && PixelSize ==  4 && AChan == 0); }
	bool IsBGRAInt32()	{ return (IsInt()   && PixelSize ==  4 && BChan == 0 && GChan == 1 && RChan == 2 && AChan ==  3); }
	bool IsBGRAInt64()	{ return (IsInt()   && PixelSize ==  8 && BChan == 0 && GChan == 2 && RChan == 4 && AChan ==  6); }
	bool IsBGRAFlt64()	{ return (IsFloat() && PixelSize ==  8 && BChan == 0 && GChan == 2 && RChan == 4 && AChan ==  6); }
	bool IsBGRAFlt128()	{ return (IsFloat() && PixelSize == 16 && BChan == 0 && GChan == 4 && RChan == 8 && AChan == 12); }

	bool IsSimple()		{ return (IsBGRAInt32() || IsBGRAInt64() || IsBGRAFlt64() || IsBGRAFlt128()); }

	bool IsInt()			{ return (ChanType[0] == CHANTYPE_I8UtoI16U || ChanType[0] == CHANTYPE_I16U); }
	bool IsInt32()			{ return (ChanType[0] == CHANTYPE_I8UtoI16U); }
	bool IsInt64()			{ return (ChanType[0] == CHANTYPE_I16U); }
	bool IsFloat()			{ return (ChanType[0] == CHANTYPE_F16asI16U || ChanType[0] == CHANTYPE_F32asI16U); }
	bool IsFloat64()		{ return (ChanType[0] == CHANTYPE_F16asI16U); }
	bool IsFloat128()		{ return (ChanType[0] == CHANTYPE_F32asI16U); }

	bool IsDeep()			{ return PixelSizeAux > 0; }

	bool IsContiguous()  { return IsDataContiguous; }
	bool IsReadOnly()    { return m_IsReadOnly; }

	bool IsAuxShared(Image *img);

	// Image manipulation functions.
	void FlipX();
	void FlipY();

	bool UseSAT();
	void RecycleSAT();

	void Clear(void);
	void ClearWithin(const FuRectInt &rect);
	void ClearWithout(const FuRectInt &rect);

	void Fill(int32 r = 0, int32 g = 0, int32 b = 0, int32 a = 65535);
	void Fill(Pixel &p);
	void Fill(FltPixel &p);
	void FillWithin(Pixel &p, const FuRectInt &rect);
	void FillWithin(FltPixel &p, const FuRectInt &rect);
	void FillWithout(Pixel &p, const FuRectInt &rect);
	void FillWithout(FltPixel &p, const FuRectInt &rect);

	bool CSConvert(ColorSpaceType from, ColorSpaceType to, Tag firsttag = _TAG_DONE, ...);
	bool CSConvertTagList(ColorSpaceType from, ColorSpaceType to, TagList &tags);

	void Gamma(float64 rg, float64 gg, float64 bg, float64 ag);
	void Gain(float64 rg, float64 gg, float64 bg, float64 ag);
	void Saturate(float64 rs, float64 gs, float64 bs);

	void AlphaDivide(Image *dest);
	void AlphaMultiply(Image *dest);

	void OXMerge(Image *fg, int32 xoff, int32 yoff);
	void OMerge(Image *fg, int32 xoff, int32 yoff);
	void OReplace(Image *fg, int32 xoff, int32 yoff);

	void RGBtoGUV();
	void BGRAtoBGR(void *dest);
	void BGRtoBGRA(void *src = NULL, uint16 alpha = 65535);

	void BIHToImage(BITMAPINFOHEADER *bih);
	void BIHToImage(BITMAPINFOHEADER *bih, void *data);

	bool ImageCopy(Image *dst, bool noabort = false);
	bool ImageCrop(Image *dst, int32 xoff, int32 yoff, bool noabort = false);
	bool ImageBlur(Image *dest, int32 type, bool red, bool green, bool blue, bool alpha, double xblur, double yblur, double blend, double normalize);

	Image *CopyOfImage(bool noabort = false);
	Image *CopyOfImage(Tag firsttag, ...);
	Image *CopyOfImageTagList(TagList &tags);

	Image *CropOfImage(int32 xoff, int32 yoff, int32 width, int32 height, bool noabort = false);
	Image *ScaleOfImage(int scale);
	Image *ScaleOfImage(float64 xscale, float64 yscale);
	Image *BlurOfImage(int32 type, bool red, bool green, bool blue, bool alpha, double xblur, double yblur, double blend, double normalize);
	Image *LegalOfImage(int32 vidmode, int32 method, float32 amplitude = 75.0, float32 softclip = 0.0);
	Image *CopyField(int from_field = CFT_FromField, Image *dest_frame = NULL, int to_field = CFT_Auto, int interpolate = CFT_OneFieldOnly);

	void ReverseFieldDominance(bool ShiftUp = false);

	void SamplePixelB(int32 fx, int32 fy, Pixel &p) { SampleBFunc(this, fx, fy, p); }
	void SamplePixelD(int32 fx, int32 fy, Pixel &p) { SampleDFunc(this, fx, fy, p); }
	void SamplePixelW(int32 fx, int32 fy, Pixel &p) { SampleWFunc(this, fx, fy, p); }

	void SampleAreaB(int32 fx1, int32 fy1, int32 fx2, int32 fy2, int32 fx3, int32 fy3, int32 fx4, int32 fy4, Pixel &p) { SampleAreaBFunc(this, fx1, fy1, fx2, fy2, fx3, fy3, fx4, fy4, p); }
	void SampleAreaD(int32 fx1, int32 fy1, int32 fx2, int32 fy2, int32 fx3, int32 fy3, int32 fx4, int32 fy4, Pixel &p) { SampleAreaDFunc(this, fx1, fy1, fx2, fy2, fx3, fy3, fx4, fy4, p); }
	void SampleAreaW(int32 fx1, int32 fy1, int32 fx2, int32 fy2, int32 fx3, int32 fy3, int32 fx4, int32 fy4, Pixel &p) { SampleAreaWFunc(this, fx1, fy1, fx2, fy2, fx3, fy3, fx4, fy4, p); }

	void SamplePixelB(int32 fx, int32 fy, FltPixel &p) { FltSampleBFunc(this, fx, fy, p); }
	void SamplePixelD(int32 fx, int32 fy, FltPixel &p) { FltSampleDFunc(this, fx, fy, p); }
	void SamplePixelW(int32 fx, int32 fy, FltPixel &p) { FltSampleWFunc(this, fx, fy, p); }

	void SampleAreaB(int32 fx1, int32 fy1, int32 fx2, int32 fy2, int32 fx3, int32 fy3, int32 fx4, int32 fy4, FltPixel &p) { FltSampleAreaBFunc(this, fx1, fy1, fx2, fy2, fx3, fy3, fx4, fy4, p); }
	void SampleAreaD(int32 fx1, int32 fy1, int32 fx2, int32 fy2, int32 fx3, int32 fy3, int32 fx4, int32 fy4, FltPixel &p) { FltSampleAreaDFunc(this, fx1, fy1, fx2, fy2, fx3, fy3, fx4, fy4, p); }
	void SampleAreaW(int32 fx1, int32 fy1, int32 fx2, int32 fy2, int32 fx3, int32 fy3, int32 fx4, int32 fy4, FltPixel &p) { FltSampleAreaWFunc(this, fx1, fy1, fx2, fy2, fx3, fy3, fx4, fy4, p); }

	void Draw(DIBSECTION *ds, uint32 sx, uint32 sy, uint32 vsw, uint32 vsh, uint32 rx, uint32 ry, uint32 rw, uint32 rh, int over, int chan, bool smooth, LookUpTable *lutr, LookUpTable *lutg, LookUpTable *lutb, LookUpTable *luta);
	void Draw(DIBSECTION *ds, uint32 sx, uint32 sy, uint32 vsw, uint32 vsh, uint32 rxo, uint32 ryo, uint32 rx, uint32 ry, uint32 rw, uint32 rh, int over, int chan, bool smooth, bool normalise, float32 gain, LookUpTable *lutr, LookUpTable *lutg, LookUpTable *lutb, LookUpTable *luta);

	bool Transform(Image *dest, float64 xoff, float64 yoff, float64 xaxis, float64 yaxis, float64 xsize, float64 ysize, float64 angle, EdgeModes edge);
	Image *TransformOf(float64 xoff, float64 yoff, float64 xaxis, float64 yaxis, float64 xsize, float64 ysize, float64 angle, EdgeModes edge);

	Image *MergeOf(Image *img, float64 alphagain, float64 addsub, float64 xoff, float64 yoff, float64 xaxis, float64 yaxis, float64 xsize, float64 ysize, float64 angle, EdgeModes edge, bool DoZ, float32 fgZOffset, float32 bgZOffset, float64 bgalphagain, float32 bgaddsub);
	Image *PixMergeOf(Image *img, float64 alphagain, float64 addsub);

	bool ImageMerge(Image *dest, Image *img, Tag firsttag = _TAG_DONE, ...);
	bool ImageMergeTagList(Image *dest, Image *img, TagList &tags);
	Image *MergeOfImage(Image *img, Tag firsttag = _TAG_DONE, ...);
	Image *MergeOfImageTagList(Image *img, TagList &tags);
	Image *GLMergeOfImageTagList(Image *img, TagList &tags);

	Image *SimpleMergeOfImage(Image *dest, Image *img, float64 xoff, float64 yoff, float64 addsub, float64 burnin, float64 rgain, float64 ggain, float64 bgain, float64 again, bool DoZ = false, float32 fgZOffset = 0.0);

	bool Transform(Image *dest, Tag firsttag = _TAG_DONE, ...);
	bool TransformTagList(Image *dest, TagList &tags);
	Image *TransformOfImage(Tag firsttag = _TAG_DONE, ...);
	Image *TransformOfImageTagList(TagList &tags);

	// The simple way...
	Image *ResizeOfImage(int32 w, int32 h);

	bool Resize(Image *dest, Tag firsttag = _TAG_DONE, ...);
	bool ResizeTagList(Image *dest, TagList &tags);
	Image *ResizeOfImage(Tag firsttag = _TAG_DONE, ...);
	Image *ResizeOfImageTagList(TagList &tags);

	// If you want to keep the filter contribution calculation data around...
	bool ResizePreCalc(Image *dest, void *&x_filter_data, void *&y_filter_data, Tag firsttag = _TAG_DONE, ...);
	bool ResizePreCalcTagList(Image *dest, void *&x_filter_data, void *&y_filter_data, TagList &tags);
	static void ResizePostCalc(void *&x_filter_data, void *&y_filter_data);

	bool Resize(Image *dest, void *x_filter_data, void *y_filter_data, Tag firsttag = _TAG_DONE, ...);
	bool ResizeTagList(Image *dest, void *x_filter_data, void *y_filter_data, TagList &tags);
	Image *ResizeOfImage(void *x_filter_data, void *y_filter_data, Tag firsttag = _TAG_DONE, ...);
	Image *ResizeOfImageTagList(void *x_filter_data, void *y_filter_data, TagList &tags);

	static float64 GetResizeFilterSupport(RszTypes filter);

	bool Blur(Image *dest, Tag firsttag = _TAG_DONE, ...);
	bool BlurTagList(Image *dest, TagList &tags);
	Image *BlurOfImage(Tag firsttag = _TAG_DONE, ...);
	Image *BlurOfImageTagList(TagList &tags);

	bool GetBlurSupport(BlurTypes type, float64 &xblur, float64 &yblur, int32 passes = 4);
	static bool GetBlurSupport(BlurTypes type, ImageDomain *domain, float64 &xblur, float64 &yblur, int32 passes = 4);

	// Can be used to apply matrix operations to the RGB channels
	bool ApplyMatrix(Image *dest, ColorMatrix &mat, Tag firsttag = _TAG_DONE, ...);
	bool ApplyMatrixTagList(Image *dest, ColorMatrix &mat, TagList &tags);
	Image *ApplyMatrixOfImage(ColorMatrix &mat, Tag firsttag = _TAG_DONE, ...);
	Image *ApplyMatrixOfImageTagList(ColorMatrix &mat, TagList &tags);

	// Can be used to apply matrix operations to the RGBA channels
	bool ApplyMatrix(Image *dest, ColorMatrixFull &mat, Tag firsttag = _TAG_DONE, ...);
	bool ApplyMatrixTagList(Image *dest, ColorMatrixFull &mat, TagList &tags);
	Image *ApplyMatrixOfImage(ColorMatrixFull &mat, Tag firsttag = _TAG_DONE, ...);
	Image *ApplyMatrixOfImageTagList(ColorMatrixFull &mat, TagList &tags);

	Image *MakeBgOf();

	bool MakeLookUpTableOf(int type, float32 rangemin, float32 rangemax, LookUpTable **plutR, LookUpTable **plutG = NULL, LookUpTable **plutB = NULL, LookUpTable **plutA = NULL);
	bool MakeLookUpTable3DOf(int type, float32 rangemin, float32 rangemax, LookUpTable3D **plutR, LookUpTable3D **plutG, LookUpTable3D **plutB);

	bool CheckAbort();

	void SetQuality(int32 quality);

	bool MapIn();
	bool MapOut();

	void SetReadOnly(bool ro);
	void SetValidWindow(const FuRectInt &valid);

	void GetCanvasColor(Pixel &p);
	void SetCanvasColor(Pixel &p);
	void GetCanvasColor(FltPixel &p);
	void SetCanvasColor(FltPixel &p);
	void GetCanvasColorChan(int32 chan, int32 &val);
	void GetCanvasColorChan(int32 chan, uint32 &val);
	void GetCanvasColorChan(int32 chan, float32 &val);
	void SetCanvasColorChan(int32 chan, int32 &val);
	void SetCanvasColorChan(int32 chan, uint32 &val);
	void SetCanvasColorChan(int32 chan, float32 &val);
	void GetCanvasColorFltChan(int32 chan, int32 &val);
	void GetCanvasColorFltChan(int32 chan, uint32 &val);
	void GetCanvasColorFltChan(int32 chan, float32 &val);
	void SetCanvasColorFltChan(int32 chan, int32 &val);
	void SetCanvasColorFltChan(int32 chan, uint32 &val);
	void SetCanvasColorFltChan(int32 chan, float32 &val);

	void CopyCanvasColor(Image *dest);
	bool IsCanvasColorClear(void);

	bool IsMultiLayer() const;

#ifndef DFLIB
	// If we follow these rules, multithreading and managing the bpuffer/RAM updates becomes
	// quite a bit saner.  Code outside of Process() that modifies the pbuffers contents (eg.
	// merging, motion blur, masking) must follow these rules also.
	// * If an operator outputs a pbuffer, the contents of the RAM must be the same or older
	//   than the contents of the pbuffer.
	// * Only the operator that creates the pbuffer can modify it or *move* it back into RAM.
	// * Operators other than the operator that created the pbuffer can only *copy* the
	//   pbuffer back into RAM.
	// * Whenever an operator writes to a pbuffer it needs to set the 'PbufferDirty' flag.

	// Copies the RGBA channels of the image onto the graphics card, creating the
	// texture if it doesn't already exist.
	bool CopyImageToGLTexture();

	// This will call newTexture->Use()
	void AttachTexture(GLTexture2D *newTexture);

	// Downloads the contents of the texture stomping all over the contents of the RGBA channels
	// in this image.  The contents of the texture are left preserved.
	bool Download(bool force = false);

	// Uploads the contents of this image into its texture
	bool Upload();

	// Uploads the contents of the img into this image's texture
	bool Upload(Image *img);

	// Downloads the contents of the texture into another image
	bool DownloadIntoImage(Image *img);

	void SetTextureDirtyFlag(bool isDirty = true);
	bool TextureDirty() { return tex && TextureDirtyFlag != 0; }		// TODO

	bool HasTexture();

	//void FreeTexture();

	LockableObject TexPtrLock;		// lock for tex pointer

	GLTexture2D *tex;		// for now this remains public...

	// When an Image is uploaded to a texture on the video card the texture is cached back into the image so that it can be
	// easily found next time it is needed.
	void CacheTexture(GLTexture2D *tex);
	GLTexture2D *GetCachedTexture(F3D_DataType depth);			// depth can F3D_MATCH
	void ClearTextureCache();

	bool IsOpaque();
	
	List TextureList;				// A list of TextureResource3D associated with this image
#endif

	FuSYSTEM_API friend Image *NewImage(Tag firsttag = _TAG_DONE, ...);
	FuSYSTEM_API friend Image *NewImageTagList(TagList &tags);
	FuSYSTEM_API friend void DeleteImage(Image *img);

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(Image);
#endif

protected:
	virtual bool _IsValid();
};





#ifdef _DEBUG
//#define NewImage				new Image
//#define NewImageTagList		new Image
#endif

#define IMAGE_BASE		(PARAMETER_BASE + TAGOFFSET)

enum ImageTags
{
	IMG_Width = IMAGE_BASE + TAG_INT,
	IMG_Height,
	IMG_Depth,
	IMG_Channel_Deprecated, // Don't use me.  I'm not TAG_MULTI
	IMG_ColorSpace,
	IMG_ProxyScale,

	IMG_CopyChannels,             // Defaults to TRUE
	IMG_PassThroughAux,				// Defaults to FALSE

	IMG_NoData,
	IMG_Quality,
	IMG_MotionBlurQuality,

	IMAT_OriginalWidth,
	IMAT_OriginalHeight,
	IMAT_Field,

	IMAT_FieldCombined,				// This image from a Saver was created by interlacing 2 other fields.

	IMAT_UpdateRectX1,
	IMAT_UpdateRectY1,
	IMAT_UpdateRectX2,
	IMAT_UpdateRectY2,

	IMAT_NormaliseChan,

	IMAT_HasMotionBlur,

	IMG_DataLeft,
	IMG_DataRight,
	IMG_DataTop,
	IMG_DataBottom,

	IMG_ValidLeft,
	IMG_ValidRight,
	IMG_ValidTop,
	IMG_ValidBottom,

	IMG_MatchSize,
	
	IMG_MultiLayer,					// bool

	IMG_PassThrough,					// Defaults to FALSE

	IMG_Channel = IMAGE_BASE + (TAG_INT | TAG_MULTI),

	IMG_XScale = TAG_DOUBLE,
	IMG_YScale,
	IMG_XOffset,
	IMG_YOffset,

	IMG_ZThresh,

	IMAT_OriginalXScale,
	IMAT_OriginalYScale,

	IMAT_NormaliseOffset,
	IMAT_NormaliseScale,
	IMAT_NormaliseDummy,

	IMG_CanonicalisationCoefficient = IMAGE_BASE + (TAG_DOUBLE | TAG_MULTI),

	IMG_Like = IMAGE_BASE + TAG_PTR,
	IMG_Document,
	IMG_StaticData,
	IMG_BIH,								// Ptr to a BITMAPINFOHEADER
	IMG_GroupMatrix,
	IMG_StatusPtr,

	IMAT_NormalisePreLUTR,
	IMAT_NormalisePreLUTG,
	IMAT_NormalisePreLUTB,
	IMAT_NormalisePreLUTA,
	IMAT_NormaliseLUTR,
	IMAT_NormaliseLUTG,
	IMAT_NormaliseLUTB,
	IMAT_NormaliseLUTA,

	IMG_DataWindow,					// FuRectInt ptr
	IMG_ValidWindow,					// FuRectInt ptr

	IMG_CanvasColor,
	IMG_CanvasColorFlt,
};

enum DepthTags
{
	IMDP_8bitInt = 1,
	IMDP_16bitInt,
	IMDP_16bitFloat,
	IMDP_32bitFloat,
	IMDP_32bitInt,
	IMDP_64bitInt,
	IMDP_64bitFloat,
	IMDP_128bitFloat,
};

struct blursum
{
	int32 b;
	int32 g;
	int32 r;
	int32 a;
};

class SimplePixPtr
{
public:
	Image *Img;
	int32 PixelSize;
	uint8 *p;

	PIXFPTR GetPixelFunc;
	PIXFPTR SetPixelFunc;

	PIXFFPTR GetFltPixelFunc;
	PIXFFPTR SetFltPixelFunc;

	SimplePixPtr() { }
	SimplePixPtr(Image *img) { SetImg(img); }
	SimplePixPtr(Image *img, int32 x, int32 y) { SetImg(img, x, y); }

	void SetImg(Image *img)
	{
		Img = img;
		PixelSize = Img->PixelSize;
		GetPixelFunc = Img->GetPixelFunc;
		SetPixelFunc = Img->SetPixelFunc;
		GetFltPixelFunc = Img->GetFltPixelFunc;
		SetFltPixelFunc = Img->SetFltPixelFunc;
		p = NULL;
	}

	void SetImg(Image *img, int32 x, int32 y)
	{
		SetImg(img); GotoXY(x, y);
	}

	void GotoXY(int32 x, int32 y) { ASSERT(x >= Img->DataWindow.left && x < Img->DataWindow.right && y >= Img->DataWindow.bottom && y < Img->DataWindow.top); if (Img->ScanLine) { p = ((uint8 *)Img->ScanLine[y]) + x * PixelSize; } }
	void XOffset(int32 x) { p += x * PixelSize; }
	void NextPixel() { p += PixelSize; }
	void PrevPixel() { p -= PixelSize; }

	void GetPixel(int32 x, int32 y, Pixel &pix) { GotoXY(x, y); GetPixel(pix); }
	void SetPixel(int32 x, int32 y, Pixel &pix) { GotoXY(x, y); SetPixel(pix); }
	void GetPixel(Pixel &pix) { GetPixelFunc(pix, p, p, Img); }
	void SetPixel(Pixel &pix) { SetPixelFunc(pix, p, p, Img); }
	void GetNextPixel(Pixel &pix) { GetPixel(pix); NextPixel(); }
	void GetPrevPixel(Pixel &pix) { GetPixel(pix); PrevPixel(); }
	void SetNextPixel(Pixel &pix) { SetPixel(pix); NextPixel(); }
	void SetPrevPixel(Pixel &pix) { SetPixel(pix); PrevPixel(); }

	void CopyPixel(SimplePixPtr &dest) { Pixel pix; GetPixel(pix); dest.SetPixel(pix); }
	void CopyNextPixel(SimplePixPtr &dest) { Pixel pix; GetNextPixel(pix); dest.SetNextPixel(pix); }
	void CopyPrevPixel(SimplePixPtr &dest) { Pixel pix; GetPrevPixel(pix); dest.SetPrevPixel(pix); }

	void SetChan(int32 chan, int32 &val)   { Img->SetChanFunc[chan](p, &val, Img, chan); }
	void SetChan(int32 chan, uint32 &val)  { Img->SetChanFunc[chan](p, &val, Img, chan); }
	void SetChan(int32 chan, float32 &val) { Img->SetChanFunc[chan](p, &val, Img, chan); }
	void GetChan(int32 chan, int32 &val)   { Img->GetChanFunc[chan](p, &val, Img, chan); }
	void GetChan(int32 chan, uint32 &val)  { Img->GetChanFunc[chan](p, &val, Img, chan); }
	void GetChan(int32 chan, float32 &val) { Img->GetChanFunc[chan](p, &val, Img, chan); }

	void SetNextChan(int32 chan, int32 &val)   { SetChan(chan, val); NextPixel(); }
	void SetNextChan(int32 chan, uint32 &val)  { SetChan(chan, val); NextPixel(); }
	void SetNextChan(int32 chan, float32 &val) { SetChan(chan, val); NextPixel(); }
	void GetNextChan(int32 chan, int32 &val)   { GetChan(chan, val); NextPixel(); }
	void GetNextChan(int32 chan, uint32 &val)  { GetChan(chan, val); NextPixel(); }
	void GetNextChan(int32 chan, float32 &val) { GetChan(chan, val); NextPixel(); }

	void SetPrevChan(int32 chan, int32 &val)   { SetChan(chan, val); PrevPixel(); }
	void SetPrevChan(int32 chan, uint32 &val)  { SetChan(chan, val); PrevPixel(); }
	void SetPrevChan(int32 chan, float32 &val) { SetChan(chan, val); PrevPixel(); }
	void GetPrevChan(int32 chan, int32 &val)   { GetChan(chan, val); PrevPixel(); }
	void GetPrevChan(int32 chan, uint32 &val)  { GetChan(chan, val); PrevPixel(); }
	void GetPrevChan(int32 chan, float32 &val) { GetChan(chan, val); PrevPixel(); }

	Pixel &operator =(Pixel &pix);
	Pixel &operator <<=(Pixel &pix);
	Pixel &operator >>=(Pixel &pix);

	void GetPixel(int32 x, int32 y, FltPixel &pix) { GotoXY(x, y); GetPixel(pix); }
	void SetPixel(int32 x, int32 y, FltPixel &pix) { GotoXY(x, y); SetPixel(pix); }
	void GetPixel(FltPixel &pix) { GetFltPixelFunc(pix, p, p, Img); }
	void SetPixel(FltPixel &pix) { SetFltPixelFunc(pix, p, p, Img); }
	void GetNextPixel(FltPixel &pix) { GetPixel(pix); NextPixel(); }
	void GetPrevPixel(FltPixel &pix) { GetPixel(pix); PrevPixel(); }
	void SetNextPixel(FltPixel &pix) { SetPixel(pix); NextPixel(); }
	void SetPrevPixel(FltPixel &pix) { SetPixel(pix); PrevPixel(); }

	void CopyFltPixel(SimplePixPtr &dest) { FltPixel pix; GetPixel(pix); dest.SetPixel(pix); }
	void CopyNextFltPixel(SimplePixPtr &dest) { FltPixel pix; GetNextPixel(pix); dest.SetNextPixel(pix); }
	void CopyPrevFltPixel(SimplePixPtr &dest) { FltPixel pix; GetPrevPixel(pix); dest.SetPrevPixel(pix); }

	void SetFltChan(int32 chan, int32 &val)   { Img->SetFltChanFunc[chan](p, &val, Img, chan); }
	void SetFltChan(int32 chan, uint32 &val)  { Img->SetFltChanFunc[chan](p, &val, Img, chan); }
	void SetFltChan(int32 chan, float32 &val) { Img->SetFltChanFunc[chan](p, &val, Img, chan); }
	void GetFltChan(int32 chan, int32 &val)   { Img->GetFltChanFunc[chan](p, &val, Img, chan); }
	void GetFltChan(int32 chan, uint32 &val)  { Img->GetFltChanFunc[chan](p, &val, Img, chan); }
	void GetFltChan(int32 chan, float32 &val) { Img->GetFltChanFunc[chan](p, &val, Img, chan); }

	void SetNextFltChan(int32 chan, int32 &val)   { SetFltChan(chan, val); NextPixel(); }
	void SetNextFltChan(int32 chan, uint32 &val)  { SetFltChan(chan, val); NextPixel(); }
	void SetNextFltChan(int32 chan, float32 &val) { SetFltChan(chan, val); NextPixel(); }
	void GetNextFltChan(int32 chan, int32 &val)   { GetFltChan(chan, val); NextPixel(); }
	void GetNextFltChan(int32 chan, uint32 &val)  { GetFltChan(chan, val); NextPixel(); }
	void GetNextFltChan(int32 chan, float32 &val) { GetFltChan(chan, val); NextPixel(); }

	void SetPrevFltChan(int32 chan, int32 &val)   { SetFltChan(chan, val); PrevPixel(); }
	void SetPrevFltChan(int32 chan, uint32 &val)  { SetFltChan(chan, val); PrevPixel(); }
	void SetPrevFltChan(int32 chan, float32 &val) { SetFltChan(chan, val); PrevPixel(); }
	void GetPrevFltChan(int32 chan, int32 &val)   { GetFltChan(chan, val); PrevPixel(); }
	void GetPrevFltChan(int32 chan, uint32 &val)  { GetFltChan(chan, val); PrevPixel(); }
	void GetPrevFltChan(int32 chan, float32 &val) { GetFltChan(chan, val); PrevPixel(); }

	FltPixel &operator =(FltPixel &pix);
	FltPixel &operator <<=(FltPixel &pix);
	FltPixel &operator >>=(FltPixel &pix);
};

inline Pixel::Pixel(SimplePixPtr &ptr)
{
	ptr.GetPixel(*this);
}

inline Pixel &Pixel::operator =(SimplePixPtr &ptr)
{
	ptr.GetPixel(*this);
	return *this;
}

inline Pixel &Pixel::operator <<=(SimplePixPtr &ptr)
{
	ptr.GetPixel(*this);
	ptr.PrevPixel();
	return *this;
}

inline Pixel &Pixel::operator >>=(SimplePixPtr &ptr)
{
	ptr.GetPixel(*this);
	ptr.NextPixel();
	return *this;
}

inline Pixel &SimplePixPtr::operator =(Pixel &pix)
{
	SetPixel(pix);
	return pix;
}

inline Pixel &SimplePixPtr::operator <<=(Pixel &pix)
{
	SetPixel(pix);
	PrevPixel();
	return pix;
}

inline Pixel &SimplePixPtr::operator >>=(Pixel &pix)
{
	SetPixel(pix);
	NextPixel();
	return pix;
}


inline FltPixel::FltPixel(SimplePixPtr &ptr)
{
	ptr.GetPixel(*this);
}

inline FltPixel &FltPixel::operator =(SimplePixPtr &ptr)
{
	ptr.GetPixel(*this);
	return *this;
}

inline FltPixel &FltPixel::operator <<=(SimplePixPtr &ptr)
{
	ptr.GetPixel(*this);
	ptr.PrevPixel();
	return *this;
}

inline FltPixel &FltPixel::operator >>=(SimplePixPtr &ptr)
{
	ptr.GetPixel(*this);
	ptr.NextPixel();
	return *this;
}

inline FltPixel &SimplePixPtr::operator =(FltPixel &pix)
{
	SetPixel(pix);
	return pix;
}

inline FltPixel &SimplePixPtr::operator <<=(FltPixel &pix)
{
	SetPixel(pix);
	PrevPixel();
	return pix;
}

inline FltPixel &SimplePixPtr::operator >>=(FltPixel &pix)
{
	SetPixel(pix);
	NextPixel();
	return pix;
}

class PixPtr : public SimplePixPtr
{
public:
	uint8 *auxp;
	int32 PixelSizeAux;

	uint8 **chanptrs[MAX_IMAGE_CHANNELS];

	PixPtr() { }
	PixPtr(Image *img) { SetImg(img); }
	PixPtr(Image *img, int32 x, int32 y) { SetImg(img, x, y); }

	void SetImg(Image *img)
	{
		SimplePixPtr::SetImg(img);
		PixelSizeAux = Img->PixelSizeAux;
		GetPixelFunc = Img->GetAuxPixelFunc;
		SetPixelFunc = Img->SetAuxPixelFunc;
		GetFltPixelFunc = Img->GetAuxFltPixelFunc;
		SetFltPixelFunc = Img->SetAuxFltPixelFunc;

		auxp = NULL;

		for (int32 c = 0; c < MAX_IMAGE_CHANNELS; c++)
			chanptrs[c] = c > CHAN_ALPHA ? &auxp : &p;
	}

	void SetImg(Image *img, int32 x, int32 y)
	{
		SetImg(img); GotoXY(x, y);
	}

	void GotoXY(int32 x, int32 y) { SimplePixPtr::GotoXY(x, y); if (Img->ScanLineAux) { auxp = ((uint8 *)Img->ScanLineAux[y]) + x * PixelSizeAux; } }
	void XOffset(int32 x) { SimplePixPtr::XOffset(x); auxp += x * PixelSizeAux; }
	void NextPixel() { SimplePixPtr::NextPixel(); auxp += PixelSizeAux; }
	void PrevPixel() { SimplePixPtr::PrevPixel(); auxp -= PixelSizeAux; }

	void GetPixel(int32 x, int32 y, Pixel &pix) { GotoXY(x, y); GetPixel(pix); }
	void SetPixel(int32 x, int32 y, Pixel &pix) { GotoXY(x, y); SetPixel(pix); }
	void GetPixel(Pixel &pix) { GetPixelFunc(pix, p, auxp, Img); }
	void SetPixel(Pixel &pix) { SetPixelFunc(pix, p, auxp, Img); }
	void GetNextPixel(Pixel &pix) { GetPixel(pix); NextPixel(); }
	void GetPrevPixel(Pixel &pix) { GetPixel(pix); PrevPixel(); }
	void SetNextPixel(Pixel &pix) { SetPixel(pix); NextPixel(); }
	void SetPrevPixel(Pixel &pix) { SetPixel(pix); PrevPixel(); }

	void CopyPixel(PixPtr &dest) { Pixel pix; GetPixel(pix); dest.SetPixel(pix); }
	void CopyNextPixel(PixPtr &dest) { Pixel pix; GetNextPixel(pix); dest.SetNextPixel(pix); }
	void CopyPrevPixel(PixPtr &dest) { Pixel pix; GetPrevPixel(pix); dest.SetPrevPixel(pix); }

	void SetChan(int32 chan, int32 &val)   { Img->SetChanFunc[chan](*chanptrs[chan], &val, Img, chan); }
	void SetChan(int32 chan, uint32 &val)  { Img->SetChanFunc[chan](*chanptrs[chan], &val, Img, chan); }
	void SetChan(int32 chan, float32 &val) { Img->SetChanFunc[chan](*chanptrs[chan], &val, Img, chan); }
	void GetChan(int32 chan, int32 &val)   { Img->GetChanFunc[chan](*chanptrs[chan], &val, Img, chan); }
	void GetChan(int32 chan, uint32 &val)  { Img->GetChanFunc[chan](*chanptrs[chan], &val, Img, chan); }
	void GetChan(int32 chan, float32 &val) { Img->GetChanFunc[chan](*chanptrs[chan], &val, Img, chan); }

	void SetNextChan(int32 chan, int32 &val)   { SetChan(chan, val); NextPixel(); }
	void SetNextChan(int32 chan, uint32 &val)  { SetChan(chan, val); NextPixel(); }
	void SetNextChan(int32 chan, float32 &val) { SetChan(chan, val); NextPixel(); }
	void GetNextChan(int32 chan, int32 &val)   { GetChan(chan, val); NextPixel(); }
	void GetNextChan(int32 chan, uint32 &val)  { GetChan(chan, val); NextPixel(); }
	void GetNextChan(int32 chan, float32 &val) { GetChan(chan, val); NextPixel(); }

	void SetPrevChan(int32 chan, int32 &val)   { SetChan(chan, val); PrevPixel(); }
	void SetPrevChan(int32 chan, uint32 &val)  { SetChan(chan, val); PrevPixel(); }
	void SetPrevChan(int32 chan, float32 &val) { SetChan(chan, val); PrevPixel(); }
	void GetPrevChan(int32 chan, int32 &val)   { GetChan(chan, val); PrevPixel(); }
	void GetPrevChan(int32 chan, uint32 &val)  { GetChan(chan, val); PrevPixel(); }
	void GetPrevChan(int32 chan, float32 &val) { GetChan(chan, val); PrevPixel(); }

	Pixel &operator =(Pixel &pix);
	Pixel &operator <<=(Pixel &pix);
	Pixel &operator >>=(Pixel &pix);

	void GetPixel(int32 x, int32 y, FltPixel &pix) { GotoXY(x, y); GetPixel(pix); }
	void SetPixel(int32 x, int32 y, FltPixel &pix) { GotoXY(x, y); SetPixel(pix); }
	void GetPixel(FltPixel &pix) { GetFltPixelFunc(pix, p, auxp, Img); }
	void SetPixel(FltPixel &pix) { SetFltPixelFunc(pix, p, auxp, Img); }
	void GetNextPixel(FltPixel &pix) { GetPixel(pix); NextPixel(); }
	void GetPrevPixel(FltPixel &pix) { GetPixel(pix); PrevPixel(); }
	void SetNextPixel(FltPixel &pix) { SetPixel(pix); NextPixel(); }
	void SetPrevPixel(FltPixel &pix) { SetPixel(pix); PrevPixel(); }

	void CopyFltPixel(PixPtr &dest) { FltPixel pix; GetPixel(pix); dest.SetPixel(pix); }
	void CopyNextFltPixel(PixPtr &dest) { FltPixel pix; GetNextPixel(pix); dest.SetNextPixel(pix); }
	void CopyPrevFltPixel(PixPtr &dest) { FltPixel pix; GetPrevPixel(pix); dest.SetPrevPixel(pix); }

	void SetFltChan(int32 chan, int32 &val)   { Img->SetFltChanFunc[chan](*chanptrs[chan], &val, Img, chan); }
	void SetFltChan(int32 chan, uint32 &val)  { Img->SetFltChanFunc[chan](*chanptrs[chan], &val, Img, chan); }
	void SetFltChan(int32 chan, float32 &val) { Img->SetFltChanFunc[chan](*chanptrs[chan], &val, Img, chan); }
	void GetFltChan(int32 chan, int32 &val)   { Img->GetFltChanFunc[chan](*chanptrs[chan], &val, Img, chan); }
	void GetFltChan(int32 chan, uint32 &val)  { Img->GetFltChanFunc[chan](*chanptrs[chan], &val, Img, chan); }
	void GetFltChan(int32 chan, float32 &val) { Img->GetFltChanFunc[chan](*chanptrs[chan], &val, Img, chan); }

	void SetNextFltChan(int32 chan, int32 &val)   { SetFltChan(chan, val); NextPixel(); }
	void SetNextFltChan(int32 chan, uint32 &val)  { SetFltChan(chan, val); NextPixel(); }
	void SetNextFltChan(int32 chan, float32 &val) { SetFltChan(chan, val); NextPixel(); }
	void GetNextFltChan(int32 chan, int32 &val)   { GetFltChan(chan, val); NextPixel(); }
	void GetNextFltChan(int32 chan, uint32 &val)  { GetFltChan(chan, val); NextPixel(); }
	void GetNextFltChan(int32 chan, float32 &val) { GetFltChan(chan, val); NextPixel(); }

	void SetPrevFltChan(int32 chan, int32 &val)   { SetFltChan(chan, val); PrevPixel(); }
	void SetPrevFltChan(int32 chan, uint32 &val)  { SetFltChan(chan, val); PrevPixel(); }
	void SetPrevFltChan(int32 chan, float32 &val) { SetFltChan(chan, val); PrevPixel(); }
	void GetPrevFltChan(int32 chan, int32 &val)   { GetFltChan(chan, val); PrevPixel(); }
	void GetPrevFltChan(int32 chan, uint32 &val)  { GetFltChan(chan, val); PrevPixel(); }
	void GetPrevFltChan(int32 chan, float32 &val) { GetFltChan(chan, val); PrevPixel(); }

	FltPixel &operator =(FltPixel &pix);
	FltPixel &operator <<=(FltPixel &pix);
	FltPixel &operator >>=(FltPixel &pix);
};

inline Pixel::Pixel(PixPtr &ptr)
{
	ptr.GetPixel(*this);
}

inline Pixel &Pixel::operator =(PixPtr &ptr)
{
	ptr.GetPixel(*this);
	return *this;
}

inline Pixel &Pixel::operator <<=(PixPtr &ptr)
{
	ptr.GetPixel(*this);
	ptr.PrevPixel();
	return *this;
}

inline Pixel &Pixel::operator >>=(PixPtr &ptr)
{
	ptr.GetPixel(*this);
	ptr.NextPixel();
	return *this;
}

inline Pixel &PixPtr::operator =(Pixel &pix)
{
	SetPixel(pix);
	return pix;
}

inline Pixel &PixPtr::operator <<=(Pixel &pix)
{
	SetPixel(pix);
	PrevPixel();
	return pix;
}

inline Pixel &PixPtr::operator >>=(Pixel &pix)
{
	SetPixel(pix);
	NextPixel();
	return pix;
}


inline FltPixel::FltPixel(PixPtr &ptr)
{
	ptr.GetPixel(*this);
}

inline FltPixel &FltPixel::operator =(PixPtr &ptr)
{
	ptr.GetPixel(*this);
	return *this;
}

inline FltPixel &FltPixel::operator <<=(PixPtr &ptr)
{
	ptr.GetPixel(*this);
	ptr.PrevPixel();
	return *this;
}

inline FltPixel &FltPixel::operator >>=(PixPtr &ptr)
{
	ptr.GetPixel(*this);
	ptr.NextPixel();
	return *this;
}

inline FltPixel &PixPtr::operator =(FltPixel &pix)
{
	SetPixel(pix);
	return pix;
}

inline FltPixel &PixPtr::operator <<=(FltPixel &pix)
{
	SetPixel(pix);
	PrevPixel();
	return pix;
}

inline FltPixel &PixPtr::operator >>=(FltPixel &pix)
{
	SetPixel(pix);
	NextPixel();
	return pix;
}


#endif
#endif
