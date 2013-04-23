#ifndef _3D_TEXTURE_RESOURCE_H_
#define _3D_TEXTURE_RESOURCE_H_

#include "VectorMath.h"
#include "3D_Resource.h"
#include "3D_DataTypes.h"






struct BufferInfo;
struct ChooseTexFormat3D;


enum F3D_TexTarget
{
	F3D_INVALID_TEX_TARGET = -1,
	F3D_1D = 0,
	F3D_2D,
	F3D_3D,
	F3D_RECT,
	F3D_CUBE,
	F3D_NUM_TEXTURE_TARGETS
};

enum F3D_CubeFace
{
	F3D_TEXTURE_CUBE_MAP_POSITIVE_X,
	F3D_TEXTURE_CUBE_MAP_NEGATIVE_X,
	F3D_TEXTURE_CUBE_MAP_POSITIVE_Y,
	F3D_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	F3D_TEXTURE_CUBE_MAP_POSITIVE_Z,
	F3D_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

enum F3D_TexFormat
{
	F3D_INVALID_TEX_FORMAT = -1,
	F3D_LUMINANCE = 0,
	F3D_LUMINANCE_ALPHA,
	F3D_RGB,
	F3D_RGBA,
	F3D_DEPTH,
	F3D_NUM_TEX_FORMATS
};

enum F3D_TextureUploadMethod
{
	F3D_TexSubImage			= 1,
	F3D_PBO						= 2,
	F3D_ThreadedPBO			= 3,			// default and desired method (this needs to be tested more)
};


// These are tags that may be added to TexResourceGL::UploadImage() or TexResourceGL::UploadBuffer()
enum TextureResourceUpload_Tags ENUM_TAGS
{
	TR_BaseIntTag = TAG_INT + TEXTURE_RESOURCE_BASE,

	//TR_F3DTexFormat,		// how the texture is treated by GL, defaults to F3D_LUMINANCE, F3D_LUMINANCE_ALPHA, F3D_RGB, F3D_RGBA based on num channels
	TR_GLTexTarget,			// (ResourceMgr.RectTarget) may also be GL_TEXTURE_2D
	TR_F3DTexDataType,		// (F3D_UINT8) determines bpc and datatype of the texture
	TR_TexWidth,				// if TR_SrcRectangle isn't specified, this defaults to image/buffer width/width for F3D_RECT targets and NextPowerOf2(width/height)  
	TR_TexHeight,				// for F3D_2D targets.  If a src rect is specified, the width/height of the subrect is used instead of the image width/height.
	TR_TexCompress,			// NYI - defaults to FALSE (what about various types?)
	//TR_TexHasBorder,		// NYI - optional - 1 pixel wide border - defaults to false

	TR_AutoScale,				// (false) If the image is beyond half texture size limit, the image is resized before uploading
	TR_CacheInImg,				// (false) Caches the uploaded texture into the Image so that it will be available if its needed again.

	TR_GLMinFilter,			// (GL_LINEAR) other value: GL_MEAREST (mips not available for RECT textures)
	TR_GLMagFilter,			// (GL_LINEAR) other value: GL_NEAREST
	TR_WrapModeS,				// (WM_Clamp) warning: not all border modes are valid with GL_TEXTURE_RECTANGLE_ARB
	TR_WrapModeT,				// (WM_Clamp)

	TR_ReuseTexture,			// defaults to false, if true, the contents of the existing texture will get replaced by the upload, but the existing texture object won't be recreated - currently there is no checking done to see if the tag settings match up with the existing texture - so be careful when using this

	TR_SrcRowAlignment,		// the byte alignment that each row of pixel data starts on - defaults to 4 - all fusion images rows are 4 byte aligned

	TR_UploadMethod,

	TR_ScaleToExtents,		// NYI - default FALSE.  If TRUE data will be scaled to fit, otherwise it will be packed against lower left corner.

	TR_AutoGenerateMips,		// NYI - automatically generate the mipmap levels - defaults to false
	TR_MipLevel,				// NYI - the following flags supply mipmap level specific data

	TR_CubeFace,				// NYI

	TR_ResizeType,				// (0)
	TR_ResizeWidth,			// (0)
	TR_ResizeHeight,			// (0)
	//TR_PrefetchNumPixels,

	TR_Clear,					// Clears within the supplied SRect
	TR_ClearAll,				// Clears everything

	TR_NoUpload,				// Create the texture, clear it (optionally) but don't upload data.


	TR_BaseDoubleTag	= TAG_DOUBLE + TEXTURE_RESOURCE_BASE,


	TR_BasePtrTag = TAG_PTR + TEXTURE_RESOURCE_BASE,
	TR_SrcRectangle,				// (NULL) RECT* - measured in pixels (NOTE - (0, 0) is bottom left corner, so the CRect methods don't work since they assume origin is top left)
	//TR_DstRectangle,			// (NULL) RECT* - measured in pixels (must specify srcRectangle, and must be same size as the srcRectangle)
	TR_GLChooseFormatFunc,		// (NULL) bool (*)(ChooseTexFormatInfo3D &ctf) - callback function which chooses the texture format/datatype
	TR_GLChooseFormatFuncUD,	// (NULL) void* - user data supplied back to the ChooseTexFormat callback
	TR_DstOffset,					// (NULL) POINT* - in pixels, img or SrcRect must be <= size of tex
	TR_Scale,						// Vector4f(1.0) - result = mul(scale, pixel) + bias, note that the biasing is done afterwards in imitation to how GL 
	TR_Bias,							// Vector4f(0.0)   does it in its pixel transfer settings
	TR_Channels,					// (isMask ? TexResource::AA : TexResource::RGBA) - an array ChannelType[4] that specifies which channels to upload into the texture (can be any of the image channels in any order)
};




const FuID CLSID_TexResource = "TexResource";


// TexResource objects can be created directly using new, but that is bad because they are then not associated with any resource manager.
// It is better to use the resource manager to create them.
class FuSYSTEM_API TexResource : public Resource3D
{
	FuDeclareClass(TexResource, Resource3D);

public:
	enum TexResource_Flags
	{
		// Common flags used by various subclasses that can be used to make searching through a list of textures faster.
		NULL_FLAG					= 0x00000000,
		IS_UINT8						= 0x00000001,			// 8-bit uint8 components (internal format on the graphics card)
		IS_UINT16					= 0x00000002,
		IS_FLOAT16					= 0x00000004,			// 16-bit float components (internal format on the graphics card)
		IS_FLOAT32					= 0x00000008,			// 32-bit float components (internal format on the graphics card)
		FROM_BUFFER					= 0x00000010,			// is this a GLTextureResource object? (or a subclass)
		FROM_IMG						= 0x00000020,			// is this a GLTextureImageResource object? (or a subclass)
		IS_1D_TEXTURE				= 0x00000040,		
		IS_2D_TEXTURE				= 0x00000080,			// set if this is any type of 2D texture - pow2 or non
		IS_3D_TEXTURE				= 0x00000100,
		IS_CUBE_TEXTURE			= 0x00000200,
		//IS_POW2_TEXTURE			= 0x00000400,			// uses pow2 GPU texture - eg. GL_TEXTURE_2D
		IS_RECTANGLE_TEXTURE		= 0x00000800,			// uses rectangular GPU texture - eg. GL_ARB_texture_rectangle
		SCALED_INTO_POW2			= 0x00001000,			// image was scaled into a pow2 texture
		//PACKED_INTO_POW2		= 0x00002000,			// image was left unscaled and placed in the lower left corner of a pow2 texture
		SCALED_INTO_RECT			= 0x00004000,			// image was scaled into a rectangular texture
		NUM_CHAN_1					= 0x00008000,
		NUM_CHAN_2					= 0x00010000,	
		NUM_CHAN_3					= 0x00020000,
		NUM_CHAN_4					= 0x00040000,
		HAS_AUX_CHANNELS			= 0x00080000,
		IS_RGBA						= 0x00100000,
		IS_LUMINANCE				= 0x00200000,
		IS_GL_TEXTURE				= 0x00400000,
		IS_NORMALIZED				= 0x00800000,
	};

	static ChannelType RGBA[4];			// this is { CHAN_RED,   CHAN_GREEN, CHAN_BLUE, CHAN_ALPHA } -- for images
	static ChannelType A[4];				// this is { CHAN_ALPHA, CHAN_NONE,  CHAN_NONE, CHAN_NONE  } -- for masks

	uint32 TexResourceFlags;
	uint8 BitsPerChannel;					// for the texture on the GPU

	ChannelType Chan[4];						// the channels contained in this texture (CHAN_NONE for empty channels)
	uint8 NumMainChannels;					// number of main (RGBA) channels
	uint8 NumAuxChannels;					// number of aux channels 
	uint8 NumTotChannels;					// total # of channels

	F3D_DataType TexDataType;				// the data type for the texture - not used anywhere in GL, but useful instead of testing all the bazillion GL formats to figure this out (note this type that we requested from GL, but GL is free to return a different type)

	uint8 pad_TextureResource[32];

public:
	TexResource();
	TexResource(const TexResource &toCopy);					// nyi
	TexResource &operator=(const TexResource &rhs);			// nyi
	virtual ~TexResource();

	bool IsUInt8()		{ return (TexResourceFlags & IS_UINT8) != 0; }
	bool IsUInt16()	{ return (TexResourceFlags & IS_UINT16) != 0; }
	bool IsFloat()		{ return (TexResourceFlags & (IS_FLOAT16 | IS_FLOAT32)) != 0; }
	bool IsFloat16()	{ return (TexResourceFlags & IS_FLOAT16) != 0; }
	bool IsFloat32()	{ return (TexResourceFlags & IS_FLOAT32) != 0; }

	bool IsNormalized() { return (TexResourceFlags & IS_NORMALIZED) != 0; }

	virtual void Bind(TagList *tags = NULL);
	virtual void Unbind(TagList *tags = NULL);

	// Free's all GPU data associated with this texture
	virtual void FreeData(TagList *tags = NULL);

	// Enables the texture mode associated with this texture
	virtual void Enable(TagList *tags = NULL);
	virtual void Disable(TagList *tags = NULL);

	// UploadBuffer() - creates a 2D texture from a 2D buffer of data
	//  texType - the type of texture (l/la/rgb/rgba/etc) of the texture to be created
	//  texFormat - the datatype of the texture to be created (may be one of the choose types)
	//  bufWidth/bufHeight - the width and height of the whole buffer
	//  bufNumChannels - the number of channels in the buffer, must be 1 or 4
	//  bufFormat - the datatype of the buffer (cannot be one of the choose types)
	//  buf - the pixel data
	bool UploadBuffer(F3D_TexFormat texFormat, F3D_DataType texDataType, int32 bufWidth, int32 bufHeight, 
		int32 bufNumChannels, F3D_DataType bufDataType, void *buf, Tag firstTag = _TAG_DONE, ...);
	virtual bool UploadBufferTagList(F3D_TexFormat texFormat, F3D_DataType texDataType, int32 bufWidth, int32 bufHeight, 
		int32 bufNumChannels, F3D_DataType bufDataType, void *buf, TagList &tags);

	// UploadImage() - creates a 2D texture with data from an image
	bool UploadImage(Image *img, Tag firstTag = _TAG_DONE, ...);
	virtual bool UploadImageTagList(Image *img, TagList &tags);

	virtual bool IsSizeOK(GLenum target, uint32 width, uint32 height);							// returns false if this is not a 2D or RECT texture

	//// Downloads texture data from the card, leaves the texture on the card as it is.
	bool Download(Tag firstTag = _TAG_DONE, ...);				// nyi
	virtual bool DownloadTagList(TagList &tags);					// nyi

	// This function is only for testing purposes - don't use - inefficient
	virtual void Fill(float r, float g, float b, float a, TagList *tags = NULL) {}

	virtual bool DebugIsValid(TagList *tags = NULL);
};


enum CTF_ConversionOp
{
	CTF_BGRA_F32_To_BGRA_F32,		// if there is no scale/bias or resize, these are just passthrough
	CTF_BGRA_F16_To_BGRA_F16,
	CTF_BGRA_I16_To_BGRA_I16,
	CTF_BGRA_I8_To_BGRA_I8,

	CTF_BGRA_F32_To_BGRA_F16,
	CTF_BGRA_F32_To_BGRA_I8,
	CTF_BGRA_F16_To_BGRA_I8,
	CTF_BGRA_F16_To_BGRA_F32,
	CTF_BGRA_I16_To_BGRA_I8,
	CTF_BGRA_I16_To_BGRA_F16,
	CTF_BGRA_I16_To_BGRA_F32,

	CTF_BGRA_F32_To_RGBA_F16,		// swizzle versions
	CTF_BGRA_F32_To_RGBA_I8,		
	CTF_BGRA_F16_To_RGBA_I8,
	CTF_BGRA_F16_To_RGBA_F32,		// nyi
	CTF_BGRA_I16_To_RGBA_I8,
	CTF_BGRA_I16_To_RGBA_F16,		// nyi
	CTF_BGRA_I16_To_RGBA_F32,		// nyi

	CTF_Aux_To_F32,					// arbitrary source data which maybe interleaved or of difference datatypes
	CTF_Aux_To_F16,					//		(eg. aux channels tend to be interleaved and of different datatypes, or 
	CTF_Aux_To_I16,					//		uploading RGBA data as AGBB or something odd.)
	CTF_Aux_To_I8,

	CTF_NumConversionOps,

	CTF_ConversionOpBaseSB  = 1 * CTF_NumConversionOps,		// scale-bias
	CTF_ConversionOpBaseR   = 2 * CTF_NumConversionOps,		// resize
	CTF_ConversionOpBaseSBR = 3 * CTF_NumConversionOps,		// scale-bias-resize

	CTF_NumTotConversionOps = 4 * CTF_NumConversionOps,		// counting the hidden SB/R/SBR ops
};




// For uploading you can specify a conversion operation for Fusion to do.  Fusion will convert the data and copy it to a temporary buffer 
// and then glTexSubImaging that buffer.  If you don't specify a conversion operation none will be done.
// The details of source rectangles and img continguity are handled by code.
struct ChooseTexFormat3D
{
	// In
	Image *Img;		
	CRect *SrcRect;									// Really just a RECT
	POINT *DstOffset;									// Pixel offset within tex to upload subrect to
	ChannelType ReqChan[4];							// the channels requested to be uploaded
	bool IsRGBA;										// the requested channels are RGBA		
	bool IsMask;										// the requested channel is A
	F3D_DataType F3DSrcChanDataType[4];			// the datatype for each of the source channels
	int32 NumMainChannels;
	int32 NumAuxChannels;
	int32 NumTotChannels;
	int32 NumSrcRGBAChannels;						// number of RGBA channels in the source image (should be either 1 or 4)
	int32 NumChanByDataType[6];					// number of channels of type F3D_UINT8, F3D_UINT16, F3D_FLOAT16, F3D_FLOAT32
	GLenum TexTarget;
	void *UserData;									// user data passed into the Upload() function

	enum ConvertModes
	{ 
		Fusion,											// when there is an option, this decides who converts the data.  This only makes sense if you
		Driver											//   can use one of the color (not aux) conversion operations
	} WhoConverts;										// this is both an In/Out variable, the chooser can change the initial value if it so desires


	// Out - variables that must be set (UploadMethod has a default that doesn't have to be changed)
	GLenum GLExtDataType;							// what datatype GL will read this image in as (one of a bazillon values)
	F3D_DataType F3DExtDataType;					// the F3D datatype for the external source data
	GLenum GLExtFormat;								// what format the external data has
	F3D_DataType F3DTexDataType;
	GLenum GLTexFormat;
	ChannelType TexChan[4];							// the actual channels stored in the texture (can be different from ReqChan[], eg. RGBA may be requested, but we choose to pass in BGRA and swizzle it in a shader for performance reasons)
	CTF_ConversionOp ConversionOp;
	F3D_TextureUploadMethod UploadMethod;		// both In/Out - defaults to multithreaded PBOs (or whatever selection is passed in the Upload() taglist)

	// Other
	bool DoResize;
	bool DoScaleBias;
	int ResizeWidth;
	int ResizeHeight;
	__declspec(align(16)) Vector4f Scale;
	__declspec(align(16)) Vector4f Bias;
};








/*
	// Valid types are float16, float32, and uint8
	void SetType(bool isFloat, uint8 bpc)
	{
		uassert((isFloat && (bpc == 16 || bpc == 32)) || (!isFloat && (bpc == 8 || bpc == 16)));

		const uint32 mask = ~(IS_UINT8 | IS_UINT16 | IS_FLOAT16 | IS_FLOAT32);
		Flags &= mask;

		if (!isFloat)
		{
			if (bpc == 8)
				Flags |= IS_UINT8;
			else if (bpc == 16)
				Flags |= IS_UINT16;
		}
		else
		{
			if (bpc == 32)
				Flags |= IS_FLOAT32;
			else if (bpc == 16)
				Flags |= IS_FLOAT16;
		}
	}
*/


//const F3D_DataType ConversionOp_To_External_F3DDataType[] =
//{
//	F3D_FLOAT32, F3D_FLOAT16, F3D_UINT16,  F3D_UINT8,
//	F3D_FLOAT32, F3D_FLOAT16, F3D_FLOAT16, F3D_UINT16, F3D_UINT16, F3D_UINT16, 
//	F3D_FLOAT32, F3D_FLOAT16, F3D_FLOAT16, F3D_UINT16, F3D_UINT16, F3D_UINT16, 
//	F3D_FLOAT32, F3D_FLOAT16, F3D_UINT16,  F3D_UINT8,
//};
//
//const GLenum ConversionOp_To_External_GLDataType[] = { 
//	GL_FLOAT, GL_HALF_FLOAT_NV, GL_UNSIGNED_SHORT, GL_UNSIGNED_BYTE,
//	GL_FLOAT, GL_HALF_FLOAT_NV, GL_HALF_FLOAT_NV,  GL_UNSIGNED_SHORT, GL_UNSIGNED_SHORT, GL_UNSIGNED_SHORT,
//	GL_FLOAT, GL_HALF_FLOAT_NV, GL_HALF_FLOAT_NV,  GL_UNSIGNED_SHORT, GL_UNSIGNED_SHORT, GL_UNSIGNED_SHORT,
//	GL_FLOAT, GL_HALF_FLOAT_NV, GL_UNSIGNED_SHORT, GL_UNSIGNED_BYTE,
//};
//
//const GLenum ConversionOp_To_Internal_F3DDataType[] = 
//{
//};


#endif