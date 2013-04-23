#ifndef _YUVCORE_H_
#define _YUVCORE_H_

#include "TagList.h"

class Image;

// Color space
#define ColorSpace_Shift			0
#define CS_ITU_R_601					( 0 << ColorSpace_Shift)
#define CS_ITU_R_709					( 1 << ColorSpace_Shift)
#define CS_SMPTE_240M				( 2 << ColorSpace_Shift)

// Data format
#define DataFormat_Shift			8
#define DF_Abekas_422_8bit			( 0 << DataFormat_Shift)
#define DF_VideoPump_422_10bit	( 1 << DataFormat_Shift)
#define DF_DVS_422_10bit			( 2 << DataFormat_Shift)
#define DF_DVS_422_16bit			( 3 << DataFormat_Shift)
#define DF_TBX_4224_8bit			( 4 << DataFormat_Shift)
#define DF_Voodoo_422_8bit			( 5 << DataFormat_Shift)
#define DF_Voodoo_422_10bit		( 6 << DataFormat_Shift)
#define DF_Pandora_422_10bit		( 7 << DataFormat_Shift)
#define DF_Ltv_422_10bit			( 8 << DataFormat_Shift)
#define DF_Ltv_4224_16bit			( 9 << DataFormat_Shift)
#define DF_DPX_422_10bit			(10 << DataFormat_Shift)
#define DF_v210_422_10bit			(11 << DataFormat_Shift)

FuSYSTEM_API void *DF_RGB2YUV(Image *img, int32 dstwidth, int32 dstheight, void *buffer = NULL, uint32 colorformat = (CS_ITU_R_601 | DF_Abekas_422_8bit), int32 f1_offset = -1, int32 pictureorder = 1);
FuSYSTEM_API bool DF_YUV2RGB(Image *img, void *buffer, uint32 colorformat = (CS_ITU_R_601 | DF_Abekas_422_8bit), int32 f1_offset = -1, int32 pictureorder = 1);

#define YUVCORE_BASE OBJECT_BASE + TAGOFFSET

enum YUVCoreTags
{
	YUV_Width = YUVCORE_BASE | TAG_INT,
	YUV_Height,
	YUV_ColorSpace,
	YUV_DataFormat,
	YUV_RowModulo,
	YUV_F1_Offset,
	YUV_PictureOrder,
	YUV_ErrorFeedback,

	YUV_Buffer = YUVCORE_BASE | TAG_PTR,
	YUV_BufferRect,								// Ptr to a FuRectInt indicating which part of the image the data is for.
};

FuSYSTEM_API void *FuRGB2YUV(Image *img, Tag firsttag = _TAG_DONE, ...);
FuSYSTEM_API void *FuRGB2YUVTagList(Image *img, const TagList &tags);

FuSYSTEM_API bool FuYUV2RGB(Image *img, Tag firsttag = _TAG_DONE, ...);
FuSYSTEM_API bool FuYUV2RGBTagList(Image *img, const TagList &tags);

#endif // _YUVCORE_H_
