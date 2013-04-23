#ifndef _3D_TEXTUREIMAGE_H_
#define _3D_TEXTUREIMAGE_H_

#include "3D_ShadeContext.h"
#include "3D_MtlBaseImplSW.h"
#include "3D_MtlBaseImplGL.h"







class GLTexture3D : public GLTexture
{
   FuDeclareClass(GLTexture3D, GLTexture);

public:
   uint32 Width, Height, Depth;

public:
   GLTexture3D();
   GLTexture3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
   GLTexture3D(const GLTexture3D &rhs);												// deliberately not implemented
   virtual ~GLTexture3D();

   GLTexture3D &operator=(const GLTexture3D &rhs);									// deliberately not implemented
   
	bool BeginUpload(uint32 width, uint32 height, uint32 depth);
   void UploadSlice(int i, Image *img);
	void EndUpload();

   virtual void FreeData(TagList *tags = NULL);

protected:
	// these are temp variables used during the upload process and are meaningless outside of BeginUpload/EndUpload.
	Image *TempImage;
	uint32 TexMemoryKbytes;

protected:
	Image *ResizeImage(Image *img);
	void ConvertTo8Bit(Image *dst, Image *src);
};




class Texture3DData : public MtlData3D
{
   FuDeclareClass(Texture3DData, MtlData3D);

public:
   float32 StartTime;
   int Width, Height, Depth;
   WM_WrapMode WrapMode;

	Input *InImage;
	uint32 ImageFlags;

public:
   Texture3DData(const Registry *reg, const ScriptVal &table, const TagList &tags);
   Texture3DData(const Texture3DData &toCopy);
   virtual ~Texture3DData();

	virtual bool CreateManagedParams();

   virtual const Registry *GetImplRegistry(uint32 type);

   virtual Data3D *CopyTagList(TagList &tags); // throws CMemoryException

   virtual uint64 EstimateVideoMemoryUsage();

   virtual bool IsOpaque();
   virtual bool IsTexture();
};








class Texture3DGL : public MtlImplGL3D
{
   FuDeclareClass(Texture3DGL, MtlImplGL3D);

public:
   class Texture3DCg : public ShadeNodeCg
   {
   public:
      ParamCg Map;
      ParamCg Transform;
   };

	uint32 Width, Height, Depth;

   GLTexture3D *Texture;
   uint32 TexUnit;
	GLenum WrapMode;

public:
   Texture3DGL(const Registry *reg, const ScriptVal &table, const TagList &tags);
   Texture3DGL(const Texture3DGL &toCopy);		// purposefully left unimplemented
   ~Texture3DGL();

   virtual bool PreRender(RenderContextGL3D &rc);
   virtual bool Activate(RenderContextGL3D &rc);
   virtual void Deactivate(RenderContextGL3D &rc);
   virtual void PostRender(RenderContextGL3D &rc);

   virtual ShaderString3D CreateShaderString(RenderContextGL3D &rc);
   virtual ShadeNodeCg *CreateShadeNode(RenderContextGL3D &rc);
	virtual const char *GetShadeNodeName(RenderContextGL3D &rc);
	virtual bool ConnectParams(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct);
};







class Texture3DSW : public MtlImplSW3D
{
   FuDeclareClass(Texture3DSW, MtlImplSW3D);

public:
   class Texture3DBlock : public ParamBlockSW
   {
   public:
      Vector2f *TexCoord;
      Vector2f *TexCoordDx;
      Vector2f *TexCoordDy;
      Color4f  *Color;
   };

   Image *Img;
   WM_WrapMode WrapMode;

public:
   Texture3DSW(const Registry *reg, const ScriptVal &table, const TagList &tags);
   Texture3DSW(const Texture3DSW &toCopy);		// purposefully left unimplemented
   ~Texture3DSW();

   virtual ParamBlockSW *CreateParamBlock(RenderContextSW3D &rc);

   void ShadeFragment(ShadeContext3D &sc);
   //virtual void Transmit(ShadeContext3D &sc);

   virtual bool Activate(RenderContextSW3D &rc);
   virtual void Deactivate(RenderContextSW3D &rc);
};






class Texture3DInputs: public MtlInputs3D
{
   FuDeclareClass(Texture3DInputs, MtlInputs3D);

public:
   Input *InImage;
   Input *InStartTime;
   Input *InNumberOfFrames;
   Input *InWrapType;

public:
   Texture3DInputs(const Registry *reg, const ScriptVal &table, const TagList &tags);
   Texture3DInputs(const Texture3DInputs &toCopy);		// ni
   virtual ~Texture3DInputs();

   virtual bool AddInputsTagList(TagList &tags);
   virtual void ShowInputs(bool visible);
   virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);
	virtual void CheckRequest(Request *req);

   Image *GetFrame(Input *in, TimeStamp time, Request *req);
};







class Texture3DTool : public MtlOperator3D
{
   FuDeclareClass(Texture3DTool, MtlOperator3D);

public:
   Texture3DTool(const Registry *reg, const ScriptVal &table, const TagList &tags);
   Texture3DTool(const Texture3DTool &toCopy);		// ni
   virtual ~Texture3DTool();
};







#endif
