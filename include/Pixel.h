#ifndef _PIXEL_H_
#define _PIXEL_H_

#include "types.h"

#if (defined(_M_IX86) || defined(_M_AMD64)) && (__INTEL_COMPILER >= 500 || _MSC_FULL_VER >= 12008804)
#include <emmintrin.h>
#elif (defined(_M_IX86) || defined(_M_AMD64)) && (__INTEL_COMPILER >= 400 || _MSC_FULL_VER >= 12008804)
#include <xmmintrin.h>
#elif (defined(_M_IX86) || (defined(_M_AMD64) && defined(__INTEL_COMPILER))) && (__INTEL_COMPILER >= 400 || _MSC_FULL_VER >= 12008804)
#include <mmintrin.h>
#endif

#include "float16.h"

#if !defined(__INTEL_COMPILER) || (__INTEL_COMPILER < 500)
#define __assume_aligned(x,y)
#endif

#if (defined(_M_IX86) || defined(_M_AMD64)) && (__INTEL_COMPILER >= 400 || _MSC_FULL_VER >= 12008804)
__declspec(align(4)) struct Pixel32
#else
struct Pixel32
#endif
{
	union
	{
		struct
		{
			uint8 B, G, R, A;
		};
		struct
		{
			int8 Bs, Gs, Rs, As;
		};
	};
};

#if (defined(_M_IX86) || defined(_M_AMD64)) && (__INTEL_COMPILER >= 400 || _MSC_FULL_VER >= 12008804)
__declspec(align(8)) struct Pixel64		// should be align(8) but according to Intel compiler docs, that currently doesn't work and align(16) should be used instead
#else
struct Pixel64
#endif
{
	union
	{
		struct
		{
			uint16 B, G, R, A;
		};
		struct
		{
			int16 Bs, Gs, Rs, As;
		};
	};
};

#if (defined(_M_IX86) || defined(_M_AMD64)) && (__INTEL_COMPILER >= 400 || _MSC_FULL_VER >= 12008804)
__declspec(align(16)) struct Pixel128
#else
struct Pixel128
#endif
{
	union
	{
		struct
		{
			uint32 B, G, R, A;
		};
		struct
		{
			int32 Bs, Gs, Rs, As;
		};
	};
};

#if (defined(_M_IX86) || defined(_M_AMD64)) && (__INTEL_COMPILER >= 400 || _MSC_FULL_VER >= 12008804)
__declspec(align(16)) struct Pixel256
#else
struct Pixel256
#endif
{
	union
	{
		struct
		{
			uint64 B, G, R, A;
		};
		struct
		{
			int64 Bs, Gs, Rs, As;
		};
	};
};

#if (defined(_M_IX86) || defined(_M_AMD64)) && (__INTEL_COMPILER >= 400 || _MSC_FULL_VER >= 12008804)
__declspec(align(8)) struct PixelHalfFlt
#else
struct PixelHalfFlt
#endif
{
	union
	{
		struct
		{
			float16 B, G, R, A;
		};
		struct
		{
			float16 Bs, Gs, Rs, As;
		};
	};
};

#if (defined(_M_IX86) || defined(_M_AMD64)) && (__INTEL_COMPILER >= 400 || _MSC_FULL_VER >= 12008804)
__declspec(align(16)) struct PixelFlt
#else
struct PixelFlt
#endif
{
	union
	{
		struct
		{
			float32 B, G, R, A;
		};
		struct
		{
			float32 Bs, Gs, Rs, As;
		};
	};
};

#if (defined(_M_IX86) || defined(_M_AMD64)) && (__INTEL_COMPILER >= 400 || _MSC_FULL_VER >= 12008804)
__declspec(align(16)) struct PixelDbl
#else
struct PixelDbl
#endif
{
	union
	{
		struct
		{
			float64 B, G, R, A;
		};
		struct
		{
			float64 Bs, Gs, Rs, As;
		};
	};
};

// This is the maximum number of bytes per pixel we can support.
#define PIXELSIZE 512

typedef void *LinePtr;
class SimplePixPtr;
class PixPtr;

#if (defined(_M_IX86) || defined(_M_AMD64)) && (__INTEL_COMPILER >= 400 || _MSC_FULL_VER >= 12008804)
__declspec(align(16)) class Pixel
#else
class Pixel
#endif
{
public:
	Pixel() { };
	Pixel(SimplePixPtr &ptr);
	Pixel(PixPtr &ptr);
	void Clear() { memset(Data, 0, PIXELSIZE); }

	union
	{
		struct
		{
			union
			{
				struct
				{
					uint32 B, G, R, A;
				};
				struct
				{
					int32 Bs, Gs, Rs, As;
				};
			};

			float32 Z;

			uint32 Coverage;
			uint32 ObjectID;
			uint32 MaterialID;

			float32 U,V;

			int32 NX,NY,NZ;

			uint32 BgB, BgG, BgR, BgA;
			uint32 RealB, RealG, RealR, RealA;

			float32 VectX, VectY;
			float32 BackVectX, BackVectY;
		};

		uint8 Data[PIXELSIZE];
	};

	Pixel &operator =(SimplePixPtr &ptr);
	Pixel &operator <<=(SimplePixPtr &ptr);
	Pixel &operator >>=(SimplePixPtr &ptr);

	Pixel &operator =(PixPtr &ptr);
	Pixel &operator <<=(PixPtr &ptr);
	Pixel &operator >>=(PixPtr &ptr);
};

#if (defined(_M_IX86) || defined(_M_AMD64)) && (__INTEL_COMPILER >= 400 || _MSC_FULL_VER >= 12008804)
__declspec(align(16)) class FltPixel
#else
class FltPixel
#endif
{
public:
	FltPixel() { };
	FltPixel(SimplePixPtr &ptr);
	FltPixel(PixPtr &ptr);
	void Clear() { memset(Data, 0, PIXELSIZE); }

	union
	{
		struct
		{
			union
			{
				struct
				{
					float32 B, G, R, A;
				};
				struct
				{
					float32 Bs, Gs, Rs, As;
				};
			};

			float32 Z;

			float32 Coverage;
			uint32 ObjectID;
			uint32 MaterialID;

			float32 U,V;

			float32 NX,NY,NZ;

			float32 BgB, BgG, BgR, BgA;

			float32 VectX, VectY;
			float32 BackVectX, BackVectY;
		};

		uint8 Data[PIXELSIZE];
	};

	FltPixel &operator =(SimplePixPtr &ptr);
	FltPixel &operator <<=(SimplePixPtr &ptr);
	FltPixel &operator >>=(SimplePixPtr &ptr);

	FltPixel &operator =(PixPtr &ptr);
	FltPixel &operator <<=(PixPtr &ptr);
	FltPixel &operator >>=(PixPtr &ptr);
};


typedef enum Channels
{
	CHAN_NONE = -4,			// used when uploading data into a texture on the graphics card to specify an empty/unused slot
	CHAN_MASK = -3,			// not real channels, but handy for other things
	CHAN_COLORALPHA = -2,
	CHAN_COLOR = -1,

	CHAN_RED = 0,
	CHAN_GREEN,
	CHAN_BLUE,
	CHAN_ALPHA,

	CHAN_BGR,
	CHAN_BGG,
	CHAN_BGB,
	CHAN_BGA,

	CHAN_REALR,
	CHAN_REALG,
	CHAN_REALB,
	CHAN_REALA,

	CHAN_Z,
	CHAN_U,
	CHAN_V,

	CHAN_COVERAGE,
	CHAN_OBJECT,
	CHAN_MATERIAL,

	CHAN_NORMALX,
	CHAN_NORMALY,
	CHAN_NORMALZ,

	CHAN_VECTORX,
	CHAN_VECTORY,

	CHAN_BACKVECTORX,
	CHAN_BACKVECTORY,
} ChannelType;

/*

  Color space conversion

*/

enum ColorSpaceType
{
	CS_RGB,
	CS_HLS,
	CS_YUV,
	CS_YIQ,
	CS_CMY,
	CS_HSV,
	CS_XYZ,
	CS_LAB,
};

#endif
