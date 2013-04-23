#ifndef _IMAGEFORMAT_H_
#define _IMAGEFORMAT_H_

#ifndef DFMANAGER

#ifndef DF_TAGS_ONLY

#include "MediaFormat.h"
#include "Image.h"
#include <windows.h>				// For RGB Quad.

class CChildFrame;

class FuSYSTEM_API ImageFormat : public MediaFormat
{
	FuDeclareClass(ImageFormat, MediaFormat); // declare ClassID static member, etc.

protected:
	uint32 Width, Height;
	float64 XScale, YScale;
	int Field;
	int Scale;
	int Depth;

public:
	ImageFormat(const Registry *reg, const ScriptVal &table, const TagList &tags);
// virtual ~ImageFormat();

	virtual bool CheckSizeOptions();

	bool LoadHeader(IOClass *ioc, Tag firsttag = _TAG_DONE, ...);
	virtual bool LoadHeader(IOClass *ioc, TagList &tags);
	bool LoadFrame(uint32 frame, Image *&img, Tag firsttag = _TAG_DONE, ...);
	virtual bool LoadFrame(uint32 frame, Image *&img, TagList &tags);

	bool SaveHeader(IOClass *ioc, Image *img, Tag firsttag = _TAG_DONE, ...);
	virtual bool SaveHeader(IOClass *ioc, Image *img, TagList &tags);
	bool SaveFrame(uint32 frame, Image *img, Tag firsttag = _TAG_DONE, ...);
	virtual bool SaveFrame(uint32 frame, Image *img, TagList &tags);

	// MediaFormat overrides
	virtual bool LoadHeader(const FuID &dtype, IOClass *ioc, Tag firsttag = _TAG_DONE, ...);
	virtual bool SaveHeader(const FuID &dtype, IOClass *ioc, Parameter *data, Tag firsttag = _TAG_DONE, ...);
	virtual Parameter *LoadData(const FuID &dtype, TimeStamp time, TimeStamp duration = 1.0, Tag firsttag = _TAG_DONE, ...);
	virtual bool SaveData(const FuID &dtype, Parameter *data, TimeStamp time, Tag firsttag = _TAG_DONE, ...);

	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	virtual void Convert8Bit(RGBQUAD *Pal, uint8 *Raw, Image *img);
	virtual void Convert8BitLine(int32 y, RGBQUAD *Pal, uint8 *Raw, Image *img);

	virtual int32 GetNumStreams(const FuID &dtype);
	virtual int32 PredictFileSize(Image *img);	// returns probable written file size, or 0
};

typedef void (* IFHookPtr)(Operator *op);

#ifndef IFINPUT
#define IFINPUT(x) STDINP('Fmt\0' | x)
#endif

#endif // DF_TAGS_ONLY

/** Attribute tags that an ImageFormat can be queried on, or set.
 ** attributes common to all instances, and capabilities of the format.
 **
 ** Note: Some of these are duplicates of MediaFormat tags, but they
 ** should have the same effect. One day, we'll change all ImageFormats
 ** over to using common MediaFormat tags etc...
 **/


enum ImageFormat_Tags
{
	IF_Width = TAG_INT + 200,
	IF_Height,
	IF_StartFrame,
	IF_NumFrames,
	IF_Field,				// 0 = field 0, 1 = field 1, -1 or not present = frame
	IF_Scale,				// 1 or not present = full size, 2 = half size, etc
	IF_IsPreviewSaver,
	IF_Depth,
	IF_LoadDepth,

	IF_FrameRate = TAG_DOUBLE + 200,		// Expected frame rate, in fps
	IF_SoundOffset,		// In time/frames (deprecated, use IF_Audio)
	IF_XScale,
	IF_YScale,

	IF_WAVSoundFilename = TAG_PTR + 200,	// ptr to filename of WAV file (deprecated, use IF_Audio)
	IF_RoI,
	IF_Audio,				// ptr to an Audio object containing the corresponding frame's worth of (uncompressed) audio data
};

/** Tags to use in an ImageFormat's Registry Node. These specify
 ** attributes common to all instances, and capabilities of the format.
 **/
enum ImageFormatReg_Tags
{
	IFREG_CanLoadFields = TAG_INT + REG_USER2,	// Default: FALSE
	IFREG_CanSaveFields,								// Default: FALSE
	IFREG_CanScale,									// Default: FALSE
	IFREG_CanSave8bit,								// Default: FALSE	(currently ignored)
	IFREG_CanSave24bit,								// Default: FALSE	(currently ignored)
	IFREG_CanSave32bit,								// Default: TRUE	(currently ignored)

	IFREG_Extension = TAG_PTR | TAG_MULTI + REG_USER2,
};

enum IFLoadDepthEnums
{
	IFLD_Format = 0,				// Whatever the format decides is the best depth
	IFLD_Default,					// Whatever the flow's depth currently is
	IFLD_8bitInt,					// 8 bit int per primary
	IFLD_16bitInt,					// 16 bit int per primary
	IFLD_16bitFloat,				// 16 bit float per primary
	IFLD_32bitFloat,				// 32 bit float per primary
};

#endif
#endif
