#ifndef _MATTECORE_H_
#define _MATTECORE_H_

#include "ThreadedOperator.h"

struct ChromaValues
{
	int32 rl;
	int32 rh;
	int32 gl;
	int32 gh;
	int32 bl;
	int32 bh;
	int32 ll;
	int32 lh;
	int32 color;
	int32 xoff;
	int32 yoff;
	int32 pick;
	double sat;
	double spill;
	int32 spillM;
	int32 chunks;
	int32 soft;
	Image *src;
	Image *dst;
	int32 key;

	float32 flt_rl;
	float32 flt_rh;
	float32 flt_gl;
	float32 flt_gh;
	float32 flt_bl;
	float32 flt_bh;
	float32 flt_ll;
	float32 flt_lh;
	float32 flt_soft;
};

struct bull
{
	int32 chunks;
};

struct AlphaBlurData
{
	int32 NumChunks;
	Image *src;
	Image *dst;
	int32 SrcChnl;
	int32 DstChnl;
	double blur;
	int32 steps;
};

struct LevelsData
{
	Image *src;
	Image *dst;
	int32 SrcChnl;
	int32 DstChnl;
	uint16  *lut;
	int32 chunks;
	int32 rh,gh,bh;
	float32 flt_rh,flt_gh,flt_bh;
	double rgam;
};

extern FuSYSTEM_API const FuID fitmode_Crop;
extern FuSYSTEM_API const FuID fitmode_Stretch;
extern FuSYSTEM_API const FuID fitmode_Inside;
extern FuSYSTEM_API const FuID fitmode_Width;
extern FuSYSTEM_API const FuID fitmode_Height;
extern FuSYSTEM_API const FuID fitmode_Outside;

FuSYSTEM_API void DoApplyLUT_Multi(Image *in, struct LevelsData *bd, int y);
FuSYSTEM_API void DoGarbageMatte(Image *imgin, Image *maskimg, int mode, bool multiply = FALSE, bool invert = FALSE);
FuSYSTEM_API void DoLevelsFlt(Image *src, double contrast, double gamma, float low, float high, int32 SrcChnl, int32 DstChnl);
FuSYSTEM_API void DoLevels(Image *src, double contrast, double gamma, int low, int high, int32 SrcChnl, int32 DstChnl);

FuSYSTEM_API void MatteCore_DoCopyChannel(Image *in, Image *out, int32 SrcChnl, int32 DstChnl);
FuSYSTEM_API void MatteCore_DoFringeFlt(Image *in, Image *out, double rgam, float32 rh, float32 gh, float32 bh);
FuSYSTEM_API void MatteCore_DoFringe(Image *in, Image *out, double rgam, int32 rh, int32 gh, int32 bh);
FuSYSTEM_API void MatteCore_DoInvert(Image *in);
FuSYSTEM_API void MatteCore_DoPostMultiply(Image *in);
FuSYSTEM_API void MatteCore_RGBtoGUV(Image *in);
FuSYSTEM_API void MatteCore_RGBtoRGBY(Image *in);
FuSYSTEM_API void MatteCore_DoSpill(Image *in, ChromaValues *vi);
FuSYSTEM_API void MatteCore_DoSoftenChannel(Image *in, Image *out, int32 SrcChnl, int32 DstChnl, double blur, int32 steps);
FuSYSTEM_API void MatteCore_DoSoftenChannel(Image *in, Image *out, int32 SrcChnl, int32 DstChnl, double blur, int32 steps, const TagList &tags);
FuSYSTEM_API bool MatteCore_DoMultiBox(Image *in, Image *out, int32 SrcChnl, int32 DstChnl, double blur, int32 passes);
FuSYSTEM_API bool MatteCore_DoMultiBox(Image *in, Image *out, int32 SrcChnl, int32 DstChnl, double blur, int32 passes, const TagList &tags);

FuSYSTEM_API void DoMaskTransform(Image *src, Image *dst, int32 chan, bool inv, float32 low, float32 hi, float32 xoff, float32 yoff, bool quick, int32 objid, int32 matid);
FuSYSTEM_API Image *DoMaskTransform(Image *src, Image *dstlike, int32 chan, bool inv, float32 low, float32 hi, float32 xoff, float32 yoff, bool quick, int32 objid, int32 matid, const FuID &fitmode, const FuRectInt *valid = NULL);

enum MXfTypes
{
	// filtering method to be used while resizing
	MXF_Channel = TAG_INT,
	MXF_Invert,
	MXF_Quick,
	MXF_ObjectID,
	MXF_MaterialID,
	MXF_ClipBlack,
	MXF_ClipWhite,

	MXF_Low = TAG_DOUBLE,
	MXF_High,
	MXF_XOffset,
	MXF_YOffset,

	MXF_FitMode = TAG_ID,

	MXF_RoI = TAG_PTR,
};

FuSYSTEM_API void MaskTransform(Image *src, Image *dst, Tag firsttag = _TAG_DONE, ...);
FuSYSTEM_API void MaskTransformTagList(Image *src, Image *dst, const TagList &tags);
FuSYSTEM_API Image *MaskTransformOf(Image *src, Image *dstlike, Tag firsttag = _TAG_DONE, ...);
FuSYSTEM_API Image *MaskTransformOfTagList(Image *src, Image *dstlike, const TagList &tags);

#endif
