#ifndef _LibUtil_h_
#define _LibUtil_h_

#include "types.h"
#include <string.h>

#include "LockableObject.h"

#if defined(DFLIB)
#include "LibInit.h"
#include "RegistryLists.h"
#include "MemIO.h"
#define DF_TAGS_ONLY
#include "MediaFormat.h"
#include "ImageFormat.h"
#undef DF_TAGS_ONLY
#endif

class Image;
class Operator;
class IOClass;
class Clip;
class ImageFormat;
class Request;
class MemIOBuffer;

#define IIOType_Read		0
#define IIOType_Write	1

enum DFLibImageIO_Tags
{
	IIO_Type = TAG_INT,
};

#define OPTSAVE_AVI_COMPRESSOR		IFINPUT(0)
#define OPTSAVE_AVI_KEYFRAMES			IFINPUT(1)
#define OPTSAVE_AVI_DATARATE			IFINPUT(2)
#define OPTSAVE_AVI_QUALITY			IFINPUT(3)
#define OPTSAVE_AVI_FRAMERATE			IFINPUT(4)
#define OPTSAVE_AVI_SAVEALPHA			IFINPUT(5)			// 0 = off, 1 = on

#define OPTSAVE_BMP_SAVEALPHA			IFINPUT(0)			// 0 = off, 1 = on

#define OPTSAVE_CIN_BYPASS				IFINPUT(14)			// 0 = do log->lin, 1 = bypass
#define OPTSAVE_CIN_LOCKRGB			IFINPUT(13)			// 0 = unlock, 1 = lock
#define OPTSAVE_CIN_RED_BLACK			IFINPUT(1)			// Value from 0 to 1023, default 95
#define OPTSAVE_CIN_RED_WHITE			IFINPUT(2)			// Value from 0 to 1024, default 685
#define OPTSAVE_CIN_RED_SOFTCLIP		IFINPUT(3)			// Value from 0 to 50, default 0
#define OPTSAVE_CIN_RED_FILMGAMMA	IFINPUT(0)			// Value from 0.0 to 5.0, default 0.6
#define OPTSAVE_CIN_RED_OUTGAMMA		IFINPUT(15)			// Value from 0.0 to 5.0, default 1.0
#define OPTSAVE_CIN_GREEN_BLACK		IFINPUT(5)			// Value from 0 to 1023, default 95
#define OPTSAVE_CIN_GREEN_WHITE		IFINPUT(6)			// Value from 0 to 1024, default 685
#define OPTSAVE_CIN_GREEN_SOFTCLIP	IFINPUT(7)			// Value from 0 to 50, default 0
#define OPTSAVE_CIN_GREEN_FILMGAMMA	IFINPUT(4)			// Value from 0.0 to 5.0, default 0.6
#define OPTSAVE_CIN_GREEN_OUTGAMMA	IFINPUT(16)			// Value from 0.0 to 5.0, default 1.0
#define OPTSAVE_CIN_BLUE_BLACK		IFINPUT(9)			// Value from 0 to 1023, default 95
#define OPTSAVE_CIN_BLUE_WHITE		IFINPUT(10)			// Value from 0 to 1024, default 685
#define OPTSAVE_CIN_BLUE_SOFTCLIP	IFINPUT(11)			// Value from 0 to 50, default 0
#define OPTSAVE_CIN_BLUE_FILMGAMMA	IFINPUT(8)			// Value from 0.0 to 5.0, default 0.6
#define OPTSAVE_CIN_BLUE_OUTGAMMA	IFINPUT(17)			// Value from 0.0 to 5.0, default 1.0
#define OPTSAVE_CIN_TABLE				IFINPUT(12)			

#define OPTSAVE_DPX_BYPASS				IFINPUT(14)			// 0 = do log->lin, 1 = bypass
#define OPTSAVE_DPX_LOCKRGB			IFINPUT(13)			// 0 = unlock, 1 = lock
#define OPTSAVE_DPX_RED_BLACK			IFINPUT(1)			// Value from 0 to 1023, default 95
#define OPTSAVE_DPX_RED_WHITE			IFINPUT(2)			// Value from 0 to 1024, default 685
#define OPTSAVE_DPX_RED_SOFTCLIP		IFINPUT(3)			// Value from 0 to 50, default 0
#define OPTSAVE_DPX_RED_FILMGAMMA	IFINPUT(0)			// Value from 0.0 to 5.0, default 0.6
#define OPTSAVE_DPX_RED_OUTGAMMA		IFINPUT(15)			// Value from 0.0 to 5.0, default 1.0
#define OPTSAVE_DPX_GREEN_BLACK		IFINPUT(5)			// Value from 0 to 1023, default 95
#define OPTSAVE_DPX_GREEN_WHITE		IFINPUT(6)			// Value from 0 to 1024, default 685
#define OPTSAVE_DPX_GREEN_SOFTCLIP	IFINPUT(7)			// Value from 0 to 50, default 0
#define OPTSAVE_DPX_GREEN_FILMGAMMA	IFINPUT(4)			// Value from 0.0 to 5.0, default 0.6
#define OPTSAVE_DPX_GREEN_OUTGAMMA	IFINPUT(16)			// Value from 0.0 to 5.0, default 1.0
#define OPTSAVE_DPX_BLUE_BLACK		IFINPUT(9)			// Value from 0 to 1023, default 95
#define OPTSAVE_DPX_BLUE_WHITE		IFINPUT(10)			// Value from 0 to 1024, default 685
#define OPTSAVE_DPX_BLUE_SOFTCLIP	IFINPUT(11)			// Value from 0 to 50, default 0
#define OPTSAVE_DPX_BLUE_FILMGAMMA	IFINPUT(8)			// Value from 0.0 to 5.0, default 0.6
#define OPTSAVE_DPX_BLUE_OUTGAMMA	IFINPUT(17)			// Value from 0.0 to 5.0, default 1.0
#define OPTSAVE_DPX_TABLE				IFINPUT(12)			

#define OPTSAVE_FITS_SAVEALPHA		IFINPUT(0)			// 0 = off, 1 = on

#define OPTSAVE_IFF_COMPRESS			IFINPUT(1)			// 0 = off, 1 = on

#define OPTSAVE_IPL_QUALITY			IFINPUT(0)			// Value from 0.0 to 100.0

#define OPTSAVE_JPG_QUALITY			IFINPUT(0)			// Value from 0.0 to 100.0

#define OPTSAVE_MAYAIFF_SAVEALPHA	IFINPUT(1)			// 0 = off, 1 = on
#define OPTSAVE_MAYAIFF_SAVEZ			IFINPUT(2)			// 0 = off, 1 = on
#define OPTSAVE_MAYAIFF_BITDEPTH		IFINPUT(3)			// 0 = 8 bit, 1 = 16 bit

#define OPTSAVE_PIC_SAVEALPHA			IFINPUT(0)			// 0 = off, 1 = on
#define OPTSAVE_PIC_COMPRESS			IFINPUT(1)			// 0 = off, 1 = on

#define OPTSAVE_PNG_SAVEALPHA			IFINPUT(0)			// 0 = off, 1 = on
#define OPTSAVE_PNG_BITDEPTH			IFINPUT(1)			// 0 = 8 bit, 1 = 16 bit
#define OPTSAVE_PNG_COMPRESS			IFINPUT(2)			// Value from 0 (min) to 9 (max)

#define OPTSAVE_RAW_COMPRESS			IFINPUT(0)			// 0 = none, 1 = zlib, 2 = RLE

#define OPTSAVE_RLA_SAVEALPHA			IFINPUT(0)			// 0 = off, 1 = on
#define OPTSAVE_RLA_COMPRESS			IFINPUT(1)			// 0 = off, 1 = on
#define OPTSAVE_RLA_BITDEPTH			IFINPUT(2)			// 0 = 8 bit, 1 = 16 bit

#define OPTSAVE_SGI_SAVEALPHA			IFINPUT(0)			// 0 = off, 1 = on
#define OPTSAVE_SGI_COMPRESS			IFINPUT(1)			// 0 = off, 1 = on
#define OPTSAVE_SGI_BITDEPTH			IFINPUT(2)			// 0 = 8 bit, 1 = 16 bit

#define OPTSAVE_SUN_SAVEALPHA			IFINPUT(0)			// 0 = off, 1 = on

#define OPTSAVE_TGA_SAVEALPHA			IFINPUT(0)			// 0 = off, 1 = on
#define OPTSAVE_TGA_COMPRESS			IFINPUT(1)			// 0 = off, 1 = on

#define OPTSAVE_TIF_SAVEALPHA			IFINPUT(0)			// 0 = off, 1 = on
#define OPTSAVE_TIF_BITDEPTH			IFINPUT(2)			// 0 = 8 bit, 1 = 16bit, 2 = float, 3 = SGI Log Luv
#define OPTSAVE_TIF_COMPRESS			IFINPUT(3)			// 0 = off, 1 = zip, 2 = lzw

#define OPTSAVE_YUV_STDSIZES			IFINPUT(0)			// 0 = off, 1 = on
#define OPTSAVE_YUV_COLORSPACE		IFINPUT(1)			// 0 = ITU-R BT.601, 1 = ITU-R BT.709

#define OPTSAVE_EXR_SAVEALPHA			IFINPUT(0)			// 0 = off, 1 = on
#define OPTSAVE_EXR_COMPRESS			IFINPUT(1)			// 0 = none, 1 = RLE, 2 = Zip (1 line), 3 = Zip (16 lines), 4 = Piz (wavelet)

#ifdef DFLIB
#ifndef ASSERT
#define	ASSERT(x)
#endif
#ifndef IFINPUT
#define IFINPUT(x)	STDINP('Fmt\0' | x)
#endif
#ifndef STDINP
#define STDINP(x)		(zMAKEID((x)|(1l<<31)))
#endif
#endif

class FuSYSTEM_API DFLibImageIO : public LockableObject
{
	FuDeclareClass(DFLibImageIO, LockableObject); // declare ClassID static member, etc.

private:
	uint32 type;

public:
	Operator *op;
	Clip *clip;
	ImageFormat *fmt;
	IOClass *ioc;

private:
	const Registry *fmtreg;
	bool donesetup;

	bool OpenClip(const char *filename, const Registry *fmtreg = NULL);
	bool SetImageFormat(ImageFormat *ifmt, const Registry *reg, const char *name = NULL);

	Request *GetSetupReq(void);

public:
	DFLibImageIO(TagList &tags);
	virtual ~DFLibImageIO();

	ImageFormat *GetFormat(const char *name);
	Image *GetFrame(uint32 framenum = 0);
	Image *GetFrame(uint32 framenum, Tag firsttag, ...);
	Image *GetFrame(uint32 framenum, TagList &tags);
	Image *ReadImage(const char *name, uint32 framenum = 0);
	Image *ReadImage(const char *name, uint32 framenum, Tag firsttag, ...);
	Image *ReadImage(const char *name, uint32 framenum, TagList &tags);

	bool SetFormat(const char *name, const char *ext);
	bool SetFormat(const char *name, const Registry *reg);
	void SetFormatOption(const char *id, double value);
	bool PutFrame(Image *img, uint32 framenum = 0);
	bool PutFrame(Image *img, uint32 framenum, Tag firsttag, ...);
	bool PutFrame(Image *img, uint32 framenum, TagList &tags);
	bool WriteImage(Image *img, const char *name, uint32 framenum = 0);
	bool WriteImage(Image *img, const char *name, uint32 framenum, Tag firsttag, ...);
	bool WriteImage(Image *img, const char *name, uint32 framenum, TagList &tags);

	int32 GetClipLength(void);
	int32 GetClipStart(void);
	float64 GetClipFrameRate(void);

	uint32 GetFormatAttr(Tag tag);
	uint32 GetFormatAttr(Tag tag, uint32 def);
	float64 GetFormatAttrD(Tag tag);
	float64 GetFormatAttrD(Tag tag, float64 def);

	static char *GetFormatName(const Registry *reg);
	static char *GetFormatName(ImageFormat *fmt);

	static int GetSeqNumber(const char *path);
	static int SetSeqNumber(const char *path, int seq, int digits = 0);
};

FuSYSTEM_API MemIOBuffer *CreateMemIOBuffer(uint32 size);
FuSYSTEM_API void DestroyMemIOBuffer(MemIOBuffer *buf);

#ifndef _FILE_H_
FuSYSTEM_API const char *GetExtension(const char *name, const char sep = '.');
FuSYSTEM_API void SetExtension(char *name, const char *ext, const char sep = '.');
#endif

FuSYSTEM_API HANDLE DF_CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, DWORD dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);

#endif
