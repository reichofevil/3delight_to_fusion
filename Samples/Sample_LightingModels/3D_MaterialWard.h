#ifndef _3D_MATERIAL_WARD_H_
#define _3D_MATERIAL_WARD_H_

#include "3D_MaterialBlinn.h"







class MtlWardData3D : public MtlBlinnData3D
{
	FuDeclareClass(MtlWardData3D, MtlBlinnData3D);

public:
	float32 SpreadX;
	float32 SpreadY;

public:
	MtlWardData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlWardData3D(const MtlWardData3D &toCopy);
	virtual ~MtlWardData3D();

	virtual const Registry *GetImplRegistry(uint32 type);
};





class MtlWardSW3D : public MtlBlinnSW3D
{
	FuDeclareClass(MtlWardSW3D, MtlBlinnSW3D);

public:
	class WardBlock : public ParamBlockSW
	{
	public:
		Vector4f *EyePosition;
		Vector4f *EyeNormal;
		Color4f  *VertexColor;
		Vector4f *TangentU;
		Vector4f *TangentV;
		Color4f  *Color;
	};

	MtlImplSW3D *SpreadXMtl;
	MtlImplSW3D *SpreadYMtl;

public:
	MtlWardSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlWardSW3D(const MtlWardSW3D &toCopy);		// ni
	virtual ~MtlWardSW3D();

	virtual ParamBlockSW *CreateParamBlock(RenderContextSW3D &rc);
	void ShadeFragment(ShadeContext3D &sc);
};





class MtlWardGL3D : public MtlBlinnGL3D
{
	FuDeclareClass(MtlWardGL3D, MtlBlinnGL3D);

public:
	class WardCg : public ShadeNodeCg
	{
	public:
		ParamCg Diffuse;						// non-texture inputs on this shader
		ParamCg Specular;
		ParamCg SpecularIntensity;
		ParamCg SpreadX;
		ParamCg SpreadY;

		ParamCg DiffuseMtl;					// Texture inputs on this shader
		ParamCg SpecularMtl;
		ParamCg SpecularIntensityMtl;
		ParamCg SpreadXMtl;
		ParamCg SpreadYMtl;
		ParamCg BumpmapMtl;
	};

	MtlImplGL3D *SpreadXMtl;
	MtlImplGL3D *SpreadYMtl;

public:
	MtlWardGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlWardGL3D(const MtlWardGL3D &toCopy);		// nyi
	virtual ~MtlWardGL3D();

	virtual bool Activate(RenderContextGL3D &rc);

	ShadeNodeCg *CreateShadeNode(RenderContextGL3D &rc);
	virtual const char *GetShaderFilename(RenderContextGL3D &rc);
	virtual const char *GetShadeNodeName(RenderContextGL3D &rc);
	virtual bool CreateChildMtls(RenderContextGL3D &rc);
	virtual bool ConnectParams(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct);
	virtual bool ConnectChildMtls(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct);
};







class MtlWardInputs3D: public MtlBlinnInputs3D
{
	FuDeclareClass(MtlWardInputs3D, MtlBlinnInputs3D);

public:
	Input *InSpreadX;
	Input *InSpreadY;

	Input *InSpreadXMtl;
	Input *InSpreadYMtl;

public:
	MtlWardInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~MtlWardInputs3D();

	virtual bool AddInputsTagList(TagList &tags);

	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);
	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);
};







class MtlWard3D: public MtlBlinn3D
{
	FuDeclareClass(MtlWard3D, MtlBlinn3D);

public:
	MtlWard3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~MtlWard3D();
};




#endif
