#ifndef _3D_MATERIALPhong_H_
#define _3D_MATERIALPhong_H_

#include "3D_MaterialBase.h"
#include "3D_MtlBaseImplSW.h"
#include "3D_MtlBaseImplGL.h"

class TexImageGL3D;
class TexImageData3D;
class RenderContext3D;





class MtlPhongData3D : public MtlData3D
{
	FuDeclareClass(MtlPhongData3D, MtlData3D);

public:
	enum ChildMtlSlot
	{
		CMS_Diffuse,
		CMS_Opacity,
		CMS_Bumpmap,
		CMS_Specular,
		CMS_SpecularIntensity,
		CMS_SpecularExponent,
		CMS_Roughness,						// cook-torrance
		CMS_RefractiveIndex,
		CMS_SpreadX,						// ward
		CMS_SpreadY,
		CMS_NumChildren
	};

	Color4f Diffuse;
	Color4f Specular;	
	float32 Opacity;
	float32 roughness;
	float32 SpecularIntensity;
	float32 SpecularExponent;
	float32 specRoughness;
	float32 specIOR;

	Color4f Refl;
	float32 ReflIntensity;
	float32 ReflRoughness;
	float32 ReflMaxDist;
	float32 ReflMetal;
	float32 Refl_val;

	float32 RefrIOR;
	float32 RefrInt;

	Color4f Transmittance;
	float32 Saturation;
	float32 AlphaDetail;
	float32 ColorDetail;

	MtlData3D *DiffuseMtl;
	MtlData3D *SpecularMtl;
	MtlData3D *SpecularIntensityMtl;
	MtlData3D *SpecularExponentMtl;
	MtlData3D *BumpmapMtl;

public:
	MtlPhongData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlPhongData3D(const MtlPhongData3D &toCopy);
	virtual ~MtlPhongData3D();

	virtual bool IsOpaque();
	//virtual void GetTextureTransform(float32 &texOffsetX, float32 &texOffsetY, float32 &texScaleX, float32 &texScaleY);	//hack

	virtual bool CreateManagedParams();

	virtual Data3D *CopyTagList(TagList &tags); // throws CMemoryException

	virtual bool IsTexture() { return true; }

	virtual const Registry *GetImplRegistry(uint32 type);

	virtual void Use();
	virtual void Recycle();
};



class MtlPhongGL3D : public MtlImplGL3D
{
	FuDeclareClass(MtlPhongGL3D, MtlImplGL3D);

public:
	class PhongCg : public ShadeNodeCg
	{
	public:
		ParamCg Diffuse;							// scalar inputs on this shader
		ParamCg Specular;
		ParamCg SpecularIntensity;
		ParamCg SpecularExponent;

		ParamCg DiffuseMtl;						// texture inputs on this shader
		ParamCg SpecularMtl;
		ParamCg SpecularIntensityMtl;
		ParamCg SpecularExponentMtl;
		ParamCg BumpmapMtl;
	};

	MtlImplGL3D *DiffuseMtl;
	MtlImplGL3D *SpecularMtl;
	MtlImplGL3D *SpecularIntensityMtl;
	MtlImplGL3D *SpecularExponentMtl;
	MtlImplGL3D *BumpmapMtl;

public:
	MtlPhongGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlPhongGL3D(const MtlPhongGL3D &toCopy);		// purposefully left unimplemented
	virtual ~MtlPhongGL3D();

	virtual bool Activate(RenderContextGL3D &rc);

	virtual const char *GetShaderFilename(RenderContextGL3D &rc);
	virtual ShadeNodeCg *CreateShadeNode(RenderContextGL3D &rc);
	virtual const char *GetShadeNodeName(RenderContextGL3D &rc);
	virtual bool CreateChildMtls(RenderContextGL3D &rc);
	virtual bool ConnectParams(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct);
	virtual bool ConnectChildMtls(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct);
};





class MtlPhongSW3D : public MtlImplSW3D
{
	FuDeclareClass(MtlPhongSW3D, MtlImplSW3D);

public:
	class PhongBlock : public ParamBlockSW
	{
	public:
		Vector4f *EyePosition;
		Vector4f *EyeNormal;
		Color4f  *VertexColor;
		Color4f  *Color;
	};

	MtlImplSW3D *DiffuseMtl;
	MtlImplSW3D *SpecularMtl;
	MtlImplSW3D *SpecularIntensityMtl;
	MtlImplSW3D *SpecularExponentMtl;
	MtlImplSW3D *BumpmapMtl;

	MtlImplSW3D *TransmittanceMtl;

	Color4f Diffuse;					// value from the sliders multiplied by opacity

	Color4f PreDiffuse;				// value before it was multiplied by opacity
	Color4f PreSpecular;				// value before it was multiplied by opacity

	Color4f Specular;

	float32 SpecularIntensity;
	float32 SpecularExponent;

	Color4f ShadowColor;

	Color4f Transmittance;
	float32 Saturation;
	float32 AlphaDetail;
	float32 ColorDetail;

public:
	MtlPhongSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlPhongSW3D(const MtlPhongSW3D &toCopy);		// purposefully left unimplemented
	virtual ~MtlPhongSW3D();

	virtual bool PreRender(RenderContextSW3D &rc);
	virtual bool Activate(RenderContextSW3D &rc);
	virtual void Deactivate(RenderContextSW3D &rc);
	virtual void PostRender(RenderContextSW3D &rc);

	virtual ParamBlockSW *CreateParamBlock(RenderContextSW3D &rc);

	void ShadeFragment(ShadeContext3D &sc);
	virtual void Transmit(ShadeContext3D &sc);
};









class MtlPhongInputs3D: public MtlInputs3D
{
	FuDeclareClass(MtlPhongInputs3D, MtlInputs3D);

public:
	Input *InDiffuseNest;
	Input *InDiffuseColorNest;
	Input *InDiffuseR;
	Input *InDiffuseG;
	Input *InDiffuseB;
	Input *InAlpha;
	Input *InOpacity;
	Input *InRough;
	Input *InDiffuseMtl;

	Input *InSpecularNest;
	Input *InSpecularColorNest;
	Input *InSpecularR;
	Input *InSpecularG;
	Input *InSpecularB;
	Input *InSpecularExponent;
	Input *InSpecularIntensity;
	Input *InSpecularRoughness;
	Input *InSpecularIOR;
	Input *InSpecularColorMtl;
	Input *InSpecularIntensityMtl;
	Input *InSpecularExponentMtl;

	Input *InReflNest;
	Input *InReflColorNest;
	Input *InReflR;
	Input *InReflG;
	Input *InReflB;
	Input *InReflMetal;
	Input *InReflDist;
	Input *InReflColorMtl;
	Input *InReflIntensity;
	Input *InReflRoughness;
	Input *InReflStrength;

	Input *InBumpmapMtl;

	Input *InTransmittanceNest;
	Input *InTransmittanceColorNest;
	Input *InTransmittanceR, *InTransmittanceG, *InTransmittanceB;
	Input *InTransmittanceSaturation;
	Input *InTransmittanceAlphaDetail;
	Input *InTransmittanceColorDetail;
	Input *InShowTransmittanceMtlInput;
	//Input *InTransmittanceMtl;

	Input *InRefractionNest;
	Input *InRefrIOR;
	Input *InRefrInt;

	Input *InReceivesLighting, *InReceivesShadows;
	Input *InUseTwoSidedLighting;

	bool InputsAdded;

protected:
	virtual MtlPhongData3D *ProcessPhongInputs(Request *req, MtlPhongData3D *data);			// separate out common functionality between this and other subclasses

	// Some functions to add inputs in different sets because some of the inherited materials like to interweave their inputs within 
	// the Phong inputs
	virtual void AddAllPhongInputs();
	virtual void AddDiffusePhongInputs();
	virtual void BeginSpecularPhongNest();
	virtual void AddSpecularPhongInputs(bool addSpecularExponent = true);
	virtual void EndSpecularPhongNest();
	virtual void AddReflPhongInputs();
	virtual void AddBumpmapPhongInputs();
	virtual void AddRefractionPhongInputs();
	virtual void AddTransmittancePhongInputs();
	virtual void AddLightingShadowsPhongInputs();

public:
	MtlPhongInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~MtlPhongInputs3D();

	static bool RegInit(Registry *regnode);
	static void RegCleanup(Registry *regnode);

	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	virtual bool AddInputsTagList(TagList &tags);
	virtual void ShowInputs(bool visible);

	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);
	virtual void OnAddToFlow(TagList *tags = NULL);
	virtual void OnRemoveFromFlow(TagList *tags = NULL);
};






class MtlPhong3D : public MtlOperator3D
{
	FuDeclareClass(MtlPhong3D, MtlOperator3D);

public:

public:
	MtlPhong3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~MtlPhong3D();

	static bool RegInit(Registry *regnode);
	static void RegCleanup(Registry *regnode);

	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	virtual void Process(Request *req);

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);
	virtual void OnAddToFlow(TagList *tags = NULL);
	virtual void OnRemoveFromFlow(TagList *tags = NULL);
};





#endif
