#ifndef _GL_TEXTURE_H_
#define _GL_TEXTURE_H_

// *** GLTexture is located in GLTextureBase.h, GLTexture2D is located here (don't want to loose CVS history by renaming files...)


#include "DFSafePtr.h"
#include "LockableObject.h"
#include "GLTextureBase.h"


class Image;







class FuSYSTEM_API GLTexture2D : public GLTexture
{
	FuDeclareClass(GLTexture2D, GLTexture);

public:
	friend class TextureManager;
	friend class Pbuffer;

	int16 TexWidth, TexHeight;
	int16 SrcWidth, SrcHeight;			// width and height of the source data

	// In general we want to apply a scale and offset to all texture coordinates.  There can be multiple reasons for this.  eg. a rectangular
	// texture is packed into a 2D texture at 1-1 the scale and offset make sure the texture coords pick out the correct subregion.   A texture 
	// might also be scaled to pow2 so that edge wrap modes will work, ... etc
	Vector4f TexScale;					// applied like this:  Mul(TexScale, (u, v, s, t)) + TexOffset
	Vector4f TexOffset;

	FuPointer<Image> simg;				// Pointer to scaled down version of the orginal (large) image

	uint8 pad_GLTexture2D[32];

	GLenum GLWrapMode[2];				// the wrap mode actually being used which may be different from...
	WM_WrapMode WrapMode[2];			// the wrap mode requested (eg. GL_TEXTURE_RECTANGLE_ARB doesn't support WM_Wrap)

protected:
	int32 DataUseCount;

	//Pbuffer *attachedPbuffer;
	
	bool UploadImageInternal(Image *img, TagList &tags);
	bool UploadBufferInternal(F3D_TexFormat texFormat, F3D_DataType texDataType, int32 bufWidth, int32 bufHeight, 
		int32 bufNumChannels, F3D_DataType bufDataType, void *buf, TagList &tags);

public:
	GLTexture2D();						
	GLTexture2D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~GLTexture2D();	

	GLTexture2D &operator=(const GLTexture2D &rhs);				// nyi
	GLTexture2D(const GLTexture2D &rhs);							// nyi

	void Init(TagList *tags = NULL);

	Matrix4f GetTextureMatrix()
	{
		return Matrix4f(
			TexScale[0],         0.0f,         0.0f,  TexOffset[0],
			       0.0f,  TexScale[1],         0.0f,  TexOffset[1],
					 0.0f,         0.0f,  TexScale[2],  TexOffset[2],
					 0.0f,         0.0f,         0.0f,  TexOffset[3]);
	}

	uint8 *GetImageData(Image *image, ChannelType chan = CHAN_COLOR, CRect *srect = NULL, TagList *tags = NULL);
	float32 *GetFltImageData(Image *image, ChannelType chan = CHAN_COLOR, CRect *srect = NULL, TagList *tags = NULL);

	// enables texturing, sets up texture; use glTexCoord2f(0,0),glTexCoord2f(width,height)
	bool GLPrepare(TagList *tags = NULL);

	// Draws image rect at coords (or subrect of image, if specified)
	bool GLDrawRect(float32 x1, float32 y1, float32 x2, float32 y2, CRect *subrect = NULL, TagList *tags = NULL);
	void DrawSubRect(float32 x1, float32 y1, float32 x2, float32 y2, Vector4f &ltrb, TagList *tags = NULL);

	//bool HasAttachedPbuffer() { return attachedPbuffer != 0; }

	void SetWrapMode(WM_WrapMode wm);								// same wrapmode for both s & t
	void SetWrapMode(WM_WrapMode wms, WM_WrapMode wmt);		// separate wrapmodes for s & t
	void SetFilterMode(GLenum minFilter, GLenum magFilter);

	virtual bool UploadImageTagList(Image *img, TagList &tags);

	virtual bool UploadBufferTagList(F3D_TexFormat texFormat, F3D_DataType texDataType, int32 bufWidth, int32 bufHeight, 
		int32 bufNumChannels, F3D_DataType bufDataType, void *buf, TagList &tags);

	// Uses the texture matrix to scale texcoords to 0..w and 0..h, accounting for non-rectangular textures
	void ScaleTextureCoords(int w, int h, TagList *tags = NULL);

	void BuildMipMaps(Image *img);
	Image *CreateSmallImage(Image *img);

	// This function is only for testing purposes - don't use - inefficient
	virtual void Fill(float r, float g, float b, float a, TagList *tags = NULL);

	virtual bool DebugIsValid(TagList *tags = NULL);

	// Frees all the GL/RAM data associated with this texture.  Doesn't detach it from pbuffer.
	virtual void FreeData(TagList *tags = NULL);

	virtual void Bind(TagList *tags = NULL);

	virtual bool IsSizeOK(GLenum target, uint32 width, uint32 height);

	uint32 GetMaxSize();
	bool IsSizeOK(GLenum target, Image *img) { return IsSizeOK(target, img->Width, img->Height); }
};



#endif