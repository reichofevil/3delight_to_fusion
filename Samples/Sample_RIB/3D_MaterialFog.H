#ifndef _3D_FOG_H_
#define _3D_FOG_H_

#include "3D_MaterialBase.h"
#include "3D_MtlBaseImplSW.h"
#include "3D_MtlBaseImplGL.h"
#include "3D_SceneOperator.h"





enum FT_FogType
{
	FT_Linear,
	FT_Exp,
	FT_Exp2,
	FT_NumFogTypes
};

const char *FogTypeStr[] = 
{ 
	"Linear",
	"Exp",
	"Exp2",
};






//
//
// MtlFogData3D
//
//
class MtlFogData3D : public MtlData3D
{
	FuDeclareClass(MtlFogData3D, MtlData3D);

public:
	enum ChildMtlSlot
	{
		CMS_Background,
		CMS_FogDensity,
		CMS_NumChildren
	};

	Color4f FogColor;					// alpha is meaningless

	bool Radial;
	FT_FogType FogType;

	float32 FogDensity;

	float32 NearDist;					// for linear fog
	float32 FarDist;

	bool ShowFogInView;

public:
	MtlFogData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlFogData3D(const MtlFogData3D &toCopy);
	virtual ~MtlFogData3D();

	virtual bool CreateManagedParams();

	virtual Data3D *CopyTagList(TagList &tags); // throws CMemoryException

	virtual bool IsOpaque();
	virtual bool IsTexture();
};







//
//
// MtlFogGL3D
//
//
class MtlFogGL3D : public MtlImplGL3D
{
	FuDeclareClass(MtlFogGL3D, MtlImplGL3D);

public:
	class MtlFogCg : public ShadeNodeCg
	{
	public:
		ParamCg Enabled;
		ParamCg FogColor;
		ParamCg Radial;
		ParamCg FogType;
		ParamCg FogDensity;

		ParamCg LinearCoef;

		ParamCg Material;
		ParamCg FogDensityTex;
	};

	MtlImplGL3D *Material;
	MtlImplGL3D *FogDensityTex;

public:
	MtlFogGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlFogGL3D(const MtlFogGL3D &toCopy);		// nyi
	virtual ~MtlFogGL3D();

	virtual bool Activate(RenderContextGL3D &rc);
	virtual void Deactivate(RenderContextGL3D &rc);

	virtual const char *GetShaderFilename(RenderContextGL3D &rc);
	virtual ShadeNodeCg *CreateShadeNode(RenderContextGL3D &rc);
	virtual const char *GetShadeNodeName(RenderContextGL3D &rc);
	virtual bool CreateChildMtls(RenderContextGL3D &rc);
	virtual bool ConnectParams(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct);
	virtual bool ConnectChildMtls(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct);
	virtual int GetState(RenderContextGL3D &rc, char buf[16]);
};








//
//
// MtlFogSW3D
//
//
class MtlFogSW3D : public MtlImplSW3D
{
	FuDeclareClass(MtlFogSW3D, MtlImplSW3D);

public:
	class FogBlock : public ParamBlockSW
	{
	public:
		Vector3f *EyePosition;
		Color4f  *Color;
	};

	MtlImplSW3D *Material;
	MtlImplSW3D *FogDensityTex;

	Color4f FogColor;
	
	bool Radial;
	FT_FogType FogType;
	
	float FogDensity;
	
	Vector2f LinearCoef;			// (FogStart, 1 / (FogEnd - FogStart))

public:
	MtlFogSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlFogSW3D(const MtlFogSW3D &toCopy);		// nyi
	virtual ~MtlFogSW3D();

	virtual ParamBlockSW *CreateParamBlock(RenderContextSW3D &rc);

	void ShadeFragment(ShadeContext3D &sc);
};






//
//
// FogOperator3D  (would like to call it Fog3D, but there already is such a tool)
//
//
class FogOperator3D: public SceneMaterialOperator3D
{
	FuDeclareClass(FogOperator3D, SceneMaterialOperator3D);

public:
	Input *InEnable;

	Input *InFogR, *InFogG, *InFogB;

	Input *InRadial;
	Input *InFogType;					// linear, exp, exp2

	Input *InFogDensity;
	Input *InFogDensityTex;	// a texture with 0..1 values

	Input *InNearDist;				// for linear fog
	Input *InFarDist;
	bool FarNearVisible;
	bool FogDensityVisible;

	Input *InShowFogInView;

	// variables used during Process()
	MtlFogData3D *FogData;

public:
	FogOperator3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	FogOperator3D(const FogOperator3D &rhs);		// nyi
	virtual ~FogOperator3D();

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);

	virtual bool PreProcessScene(Request *req);
	virtual void PostProcessScene(bool success);
	virtual MtlData3D *ProcessMaterial(MtlData3D *oldMaterial);
};





#endif