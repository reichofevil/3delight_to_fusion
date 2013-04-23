#ifndef _3D_TEXTUREIMAGE_H_
#define _3D_TEXTUREIMAGE_H_

#include "3D_ShadeContext.h"
#include "3D_MtlBaseImplSW.h"
#include "3D_MtlBaseImplGL.h"





class CreateTextureData3D : public MtlData3D
{
	FuDeclareClass(CreateTextureData3D, MtlData3D);

public:
	enum ChildMtlSlot
	{
		CMS_NumChildMtls					// no children
	};

	FuUnmappedPointer<Image> Img;

	WM_WrapMode WrapMode;
	Vector2f Scale;					// is appied thus:  (u, v) -> scale * (u, v) + offset, not (u, v) -> scale * ((u, v) + offset)
	Vector2f Offset;

	bool LimitToHalfMaxTextureSize;

public:
	CreateTextureData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	CreateTextureData3D(const CreateTextureData3D &toCopy);
	virtual ~CreateTextureData3D();

	virtual bool CreateManagedParams();

	virtual const Registry *GetImplRegistry(uint32 type);

	virtual Data3D *CopyTagList(TagList &tags); // throws CMemoryException

	virtual bool GetNaturalDims(float32 dims[4]);

	virtual uint64 EstimateVideoMemoryUsage();

	virtual bool IsOpaque();
	virtual bool IsTexture();
};








class CreateTextureGL3D : public MtlImplGL3D
{
	FuDeclareClass(CreateTextureGL3D, MtlImplGL3D);

public:
	class CreateTextureCg : public ShadeNodeCg
	{
	public:
		ParamCg Scale;
		ParamCg Offset;
		ParamCg DoScaleOffset;

		ParamCg Map;
		ParamCg TexScale;
		ParamCg TexOffset;
		ParamCg TexObject;
		ParamCg IsMask;
	};

	//FuUnmappedPointer<Image> Img;
	GLTexture2D *Texture;
	uint32 TexUnit;
	WM_WrapMode WrapMode;
	GLenum TexTarget;

	bool DoScaleOffset;
	BOOL LimitToHalfMaxTextureSize;

public:
	CreateTextureGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	CreateTextureGL3D(const CreateTextureGL3D &toCopy);		// purposefully left unimplemented
	~CreateTextureGL3D();

	virtual bool PreRender(RenderContextGL3D &rc);
	virtual bool Activate(RenderContextGL3D &rc);
	virtual void Deactivate(RenderContextGL3D &rc);
	virtual void PostRender(RenderContextGL3D &rc);

	virtual const char *GetShaderFilename(RenderContextGL3D &rc);
	virtual ShadeNodeCg *CreateShadeNode(RenderContextGL3D &rc);
	virtual const char *GetShadeNodeName(RenderContextGL3D &rc);
	virtual bool ConnectParams(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct);
	virtual int GetState(RenderContextGL3D &rc, char buf[16]);
};







class CreateTextureSW3D : public MtlImplSW3D
{
	FuDeclareClass(CreateTextureSW3D, MtlImplSW3D);

public:
	class CreateTextureBlock : public ParamBlockSW
	{
	public:
		Vector2f *TexCoord;
		Vector2f *TexCoordDx;
		Vector2f *TexCoordDy;
		Color4f  *Color;
	};

	Image *Img;
	WM_WrapMode WrapMode;

	Vector2f Scale;
	Vector2f Offset;
	bool DoScaleOffset;

public:
	CreateTextureSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	CreateTextureSW3D(const CreateTextureSW3D &toCopy);		// purposefully left unimplemented
	~CreateTextureSW3D();

	virtual ParamBlockSW *CreateParamBlock(RenderContextSW3D &rc);
	virtual bool IsComplete(RenderContextSW3D &rc);

	void ShadeFragment(ShadeContext3D &sc);
	//virtual void Transmit(ShadeContext3D &sc);

	virtual bool Activate(RenderContextSW3D &rc);
	virtual void Deactivate(RenderContextSW3D &rc);
};





class CreateTextureInputs3D : public MtlInputs3D
{
	FuDeclareClass(CreateTextureInputs3D, MtlInputs3D);

public:
	Input *InImage;					
	Input *InWrapType;
	Input *InUOffset, *InVOffset, *InUScale, *InVScale;
	Input *InLimitToHalfMaxTextureSize;

public:
	CreateTextureInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	CreateTextureInputs3D(const CreateTextureInputs3D &toCopy);			// ni
	virtual ~CreateTextureInputs3D();

	virtual bool AddInputsTagList(TagList &tags);
	virtual void ShowInputs(bool visible);
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);
	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);
};





class CreateTextureOperator3D : public MtlOperator3D
{
	FuDeclareClass(CreateTextureOperator3D, MtlOperator3D);

public:
	CreateTextureOperator3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	CreateTextureOperator3D(const CreateTextureOperator3D &toCopy);		// ni
	virtual ~CreateTextureOperator3D();

	//virtual bool ProcessImage(Request *req, Input *inImage)
	//{
	//	ImageInput = inImage;
	//	ImageInputTime = req->GetInputTime(inImage);
	//	ImageInputFlags = (req->GetInputFlags(inImage) & ~REQF_PreCalc);		// when we request it for real later, we don't want a precalc request
	//	return true;
	//}
};


#endif
