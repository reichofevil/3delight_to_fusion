#ifndef _GL_TEXTURE_BASE_H_
#define _GL_TEXTURE_BASE_H_

#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

#include "Object.h"
#include "Pixel.h"

#include "3D_TextureResource.h"






class FuSYSTEM_API GLTexture : public TexResource
{
	FuDeclareClass(GLTexture, TexResource);

public:
	// Images are either RGBA or LUMINANCE.  If its RGBA, the channel may only CHAN_COLOR.  If the image
	// is LUMINANCE, the channel may any of the channel types, eg. CHAN_RED, CHAN_ALPHA, CHAN_Z, CHAN_VECTORX.
	ChannelType Channel;

	GLuint GLTexName;							// 0 if not uploaded

	GLenum GLTexTarget;						// GL_TEXTURE_2D or GL_TEXTURE_RECTANGLE_NV
	GLenum GLTexFormat;						// the internal format GL specifier

	// The renderer will set these to whatever the current state for this texture is.  Note that only one GL renderer maybe rendering
	// a texture at a time (or if there are 2 video cards, there are 2 separate resource managers and textures are not shared between them,
	// so this is non-issue)
	GLenum GLMinFilter, GLMagFilter;

	#ifdef _DEBUG
		bool HasBeenBound;
	#endif

	int32 last_glerror;

	uint8 pad_GLTexture[60];

public:
	bool UploadTexSubImage(Image *img, ChooseTexFormat3D &ctf);
	bool UploadPBO(Image *img, ChooseTexFormat3D &ctf, bool allowThreads);

	bool PBOSubImage(GLenum texTarget, GLint x, GLint y, GLint w, GLint h, GLint format, GLint dataType, bool allowThreads, const void *data);

	void FillBuffer(const BufferInfo &bin);

	static bool ChooseTextureFormatMatch(ChooseTexFormat3D &ctf);
	static bool ChooseTextureFormatDefault(ChooseTexFormat3D &ctf, F3D_DataType reqestedTexDataType);

	static DWORD WINAPI TextureUploadThread(void *tdata);

	static void BGRAF32ToBGRAF32(const BufferInfo &bi);
	static void BGRAF16ToBGRAF16(const BufferInfo &bi);
	static void BGRAI16ToBGRAI16(const BufferInfo &bi);
	static void BGRAI8ToBGRAI8  (const BufferInfo &bi);

	static void BGRAF32ToBGRAF16(const BufferInfo &bi);
	static void BGRAF32ToBGRAI8 (const BufferInfo &bi);
	static void BGRAF16ToBGRAI8 (const BufferInfo &bi);
	static void BGRAF16ToBGRAF32(const BufferInfo &bi);
	static void BGRAI16ToBGRAI8 (const BufferInfo &bi);
	static void BGRAI16ToBGRAF16(const BufferInfo &bi);
	static void BGRAI16ToBGRAF32(const BufferInfo &bi);

	static void BGRAF32ToRGBAF16(const BufferInfo &bi);
	static void BGRAF32ToRGBAI8 (const BufferInfo &bi);
	static void BGRAF16ToRGBAI8 (const BufferInfo &bi);
	static void BGRAF16ToRGBAF32(const BufferInfo &bi);
	static void BGRAI16ToRGBAI8 (const BufferInfo &bi);
	static void BGRAI16ToRGBAF16(const BufferInfo &bi);
	static void BGRAI16ToRGBAF32(const BufferInfo &bi);

	static void AuxToF32(const BufferInfo &bi);
	static void AuxToF16(const BufferInfo &bi);
	static void AuxToI16(const BufferInfo &bi);
	static void AuxToI8 (const BufferInfo &bi);


	static void BGRAF32ToBGRAF32_SB(const BufferInfo &bi);
	static void BGRAF16ToBGRAF16_SB(const BufferInfo &bi);
	static void BGRAI16ToBGRAI16_SB(const BufferInfo &bi);
	static void BGRAI8ToBGRAI8_SB  (const BufferInfo &bi);

	static void BGRAF32ToBGRAF16_SB(const BufferInfo &bi);
	static void BGRAF32ToBGRAI8_SB (const BufferInfo &bi);
	static void BGRAF16ToBGRAI8_SB (const BufferInfo &bi);
	static void BGRAF16ToBGRAF32_SB(const BufferInfo &bi);
	static void BGRAI16ToBGRAI8_SB (const BufferInfo &bi);
	static void BGRAI16ToBGRAF16_SB(const BufferInfo &bi);
	static void BGRAI16ToBGRAF32_SB(const BufferInfo &bi);

	static void BGRAF32ToRGBAF16_SB(const BufferInfo &bi);
	static void BGRAF32ToRGBAI8_SB (const BufferInfo &bi);
	static void BGRAF16ToRGBAI8_SB (const BufferInfo &bi);
	static void BGRAF16ToRGBAF32_SB(const BufferInfo &bi);
	static void BGRAI16ToRGBAI8_SB (const BufferInfo &bi);
	static void BGRAI16ToRGBAF16_SB(const BufferInfo &bi);
	static void BGRAI16ToRGBAF32_SB(const BufferInfo &bi);

	static void AuxToF32_SB(const BufferInfo &bi);
	static void AuxToF16_SB(const BufferInfo &bi);
	static void AuxToI16_SB(const BufferInfo &bi);
	static void AuxToI8_SB (const BufferInfo &bi);


	static void BGRAF32ToBGRAF32_R(const BufferInfo &bi);
	static void BGRAF16ToBGRAF16_R(const BufferInfo &bi);
	static void BGRAI16ToBGRAI16_R(const BufferInfo &bi);
	static void BGRAI8ToBGRAI8_R  (const BufferInfo &bi);

	static void BGRAF32ToBGRAF16_R(const BufferInfo &bi);
	static void BGRAF32ToBGRAI8_R (const BufferInfo &bi);
	static void BGRAF16ToBGRAI8_R (const BufferInfo &bi);
	static void BGRAF16ToBGRAF32_R(const BufferInfo &bi);
	static void BGRAI16ToBGRAI8_R (const BufferInfo &bi);
	static void BGRAI16ToBGRAF16_R(const BufferInfo &bi);
	static void BGRAI16ToBGRAF32_R(const BufferInfo &bi);

	static void BGRAF32ToRGBAF16_R(const BufferInfo &bi);
	static void BGRAF32ToRGBAI8_R (const BufferInfo &bi);
	static void BGRAF16ToRGBAI8_R (const BufferInfo &bi);
	static void BGRAF16ToRGBAF32_R(const BufferInfo &bi);
	static void BGRAI16ToRGBAI8_R (const BufferInfo &bi);
	static void BGRAI16ToRGBAF16_R(const BufferInfo &bi);
	static void BGRAI16ToRGBAF32_R(const BufferInfo &bi);

	static void AuxToF32_R(const BufferInfo &bi);
	static void AuxToF16_R(const BufferInfo &bi);
	static void AuxToI16_R(const BufferInfo &bi);
	static void AuxToI8_R (const BufferInfo &bi);


	static void BGRAF32ToBGRAF32_SBR(const BufferInfo &bi);
	static void BGRAF16ToBGRAF16_SBR(const BufferInfo &bi);
	static void BGRAI16ToBGRAI16_SBR(const BufferInfo &bi);
	static void BGRAI8ToBGRAI8_SBR  (const BufferInfo &bi);

	static void BGRAF32ToBGRAF16_SBR(const BufferInfo &bi);
	static void BGRAF32ToBGRAI8_SBR (const BufferInfo &bi);
	static void BGRAF16ToBGRAI8_SBR (const BufferInfo &bi);
	static void BGRAF16ToBGRAF32_SBR(const BufferInfo &bi);
	static void BGRAI16ToBGRAI8_SBR (const BufferInfo &bi);
	static void BGRAI16ToBGRAF16_SBR(const BufferInfo &bi);
	static void BGRAI16ToBGRAF32_SBR(const BufferInfo &bi);

	static void BGRAF32ToRGBAF16_SBR(const BufferInfo &bi);
	static void BGRAF32ToRGBAI8_SBR (const BufferInfo &bi);
	static void BGRAF16ToRGBAI8_SBR (const BufferInfo &bi);
	static void BGRAF16ToRGBAF32_SBR(const BufferInfo &bi);
	static void BGRAI16ToRGBAI8_SBR (const BufferInfo &bi);
	static void BGRAI16ToRGBAF16_SBR(const BufferInfo &bi);
	static void BGRAI16ToRGBAF32_SBR(const BufferInfo &bi);

	static void AuxToF32_SBR(const BufferInfo &bi);
	static void AuxToF16_SBR(const BufferInfo &bi);
	static void AuxToI16_SBR(const BufferInfo &bi);
	static void AuxToI8_SBR (const BufferInfo &bi);

public:
	// Use TextureManager to create GLTexture2D objects *always*, never use new or ClassRegistry->New() or the texture manager will
	// not know about them
	GLTexture();
	GLTexture(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~GLTexture();

	GLTexture &operator=(const GLTexture &rhs);				// nyi
	GLTexture(const GLTexture &rhs);								// nyi

	// Binds this texture to the current texture target
	virtual void Bind(TagList *tags = NULL);

	// This binds 0 to the current texture target.  This can be handy because the driver will hold onto the current texture (even if you glDeleteTexture() it) 
	// until another texture is bound.  This method is mostly only used internally by Fusion, no need for plugins to call it usually.
	virtual void Unbind(TagList *tags = NULL);

	virtual void Enable(TagList *tags = NULL);
	virtual void Disable(TagList *tags = NULL);

	virtual bool DebugIsValid(TagList *tags = NULL);

	virtual void FreeData(TagList *tags = NULL);
};









#endif