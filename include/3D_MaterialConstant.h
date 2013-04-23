#ifndef _3D_MATERIALCONSTANT_H_
#define _3D_MATERIALCONSTANT_H_


#include "3d_AABB.h"
#include "3D_MaterialBase.h"
#include "3D_MtlBaseImplSW.h"
#include "3D_MtlBaseImplGL.h"

class Operator;
class RenderContext3D;

const int NumStandardMatUniforms = 4;

const FuID LongID_MtlStd			= "MtlStd";				// All of the classes associated with MtlStd will add this MIREG_MaterialID tag to their registry lists.
const FuID CLSID_MtlStdData		= "MtlStdData";
const FuID CLSID_MtlStdInputs		= "MtlStdInputs";
const FuID CLSID_MtlStdGL			= "MtlStdGL";
const FuID CLSID_MtlStdSW			= "MtlStdSW";




class FuSYSTEM_API MtlStdData3D : public MtlData3D
{
	FuDeclareClass(MtlStdData3D, MtlData3D);

public:
	enum ChildMtlSlot
	{
		CMS_Diffuse,
		CMS_NumChildren
	};

	Color4f Diffuse;
	Color4f Specular;
	float32 Opacity;
	float32 SpecularExponent;
	float32 SpecularIntensity;
	Color4f Transmittance;
	float32 Saturation;
	float32 AlphaDetail;
	float32 ColorDetail;

	MtlData3D *DiffuseMtl;

public:
	MtlStdData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlStdData3D(const MtlStdData3D &toCopy);
	virtual ~MtlStdData3D();

	virtual bool CreateManagedParams();

	virtual Data3D *CopyTagList(TagList &tags); // throws CMemoryException

	virtual void SetToDefaults();
	virtual bool IsOpaque();
	virtual bool IsTexture() { return false; }

	// Returns the diffuse texture but only if the texture connected directly to the input with no intervening material tools.
	// Returns NULL otherwise.  The returned image will need to be mapped in.
	Image *GetDiffuseTexture();

	void SetDiffuseTexture(MtlData3D *texData);
};








class FuSYSTEM_API MtlStdSW3D : public MtlImplSW3D
{
	FuDeclareClass(MtlStdSW3D, MtlImplSW3D);

public:
	class StandardBlock : public ParamBlockSW
	{
	public:
		Vector4f *EyePosition;
		Vector4f *EyeNormal;
		Color4f  *VertexColor;
		Color4f  *Color;
	};

	MtlImplSW3D *DiffuseColorTex;	// store a copy here to make this faster to access

	Color4f PreDiffuse;				// value before it was multiplied by opacity
	Color4f Diffuse;					// value after multiplied by opacity

	Color4f PreSpecular;				// value before it was multiplied by opacity
	Color4f Specular;
	float32 SpecularExponent;

	Color4f ShadowColor;

	Color4f Transmittance;
	float32 Saturation;
	float32 AlphaDetail;
	float32 ColorDetail;

public:
	MtlStdSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlStdSW3D(const MtlStdSW3D &toCopy);	// ni
	virtual ~MtlStdSW3D();

	virtual ParamBlockSW *CreateParamBlock(RenderContextSW3D &rc);
	void ShadeFragment(ShadeContext3D &sc);
	virtual void Transmit(ShadeContext3D &sc);
};






// Implementation of the of the Standard Material for the fixed function GL renderer
class FuSYSTEM_API MtlStdGL3D : public MtlImplGL3D
{
	FuDeclareClass(MtlStdGL3D, MtlImplGL3D);

public:
	class MtlStdCg : public ShadeNodeCg
	{
	public:
		ParamCg Diffuse;
		ParamCg Specular;
		ParamCg SpecularExponent;
		ParamCg DiffuseMtl;
	};

	Color4f Diffuse;
	Color4f Specular;
	Color4f Transmittance;

	MtlImplGL3D *DiffuseColorTex;

public:
	MtlStdGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlStdGL3D(const MtlStdGL3D &toCopy);		// purposefully left unimplemented
	virtual ~MtlStdGL3D();

	virtual const char *GetShaderFilename(RenderContextGL3D &rc);
	virtual const char *GetShadeNodeName(RenderContextGL3D &rc);
	virtual ShadeNodeCg *CreateShadeNode(RenderContextGL3D &rc);
	virtual bool CreateChildMtls(RenderContextGL3D &rc);
	virtual bool ConnectParams(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct);
	virtual bool ConnectChildMtls(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct);
	virtual bool Activate(RenderContextGL3D &rc);
};






#define MTL_STD_INPUTS_BASE	(MATERIAL_INPUTS_BASE + TAGOFFSET)


enum MaterialStandardInputs_Tags ENUM_TAGS
{
	MSI_IntBaseTag = TAG_INT + MTL_STD_INPUTS_BASE,
	
	MSI_PtrBaseTag = TAG_PTR + MTL_STD_INPUTS_BASE,
	MSI_DiffuseTextureInput,	/**< (NULL) Use from within MtlStdInputs3D::AddInputsTagList to provide a pre-existing texture/image input to be used as the input for the diffuse texture input.  If none is provided one is created. */
};


class FuSYSTEM_API MtlStdInputs3D: public MtlInputs3D
{
	FuDeclareClass(MtlStdInputs3D, MtlInputs3D);

public:
	Input *InUsingExternalMaterial;

	Input *InDiffuseNest;
	Input *InDiffuseColorNest;
	Input *InDiffuseR;
	Input *InDiffuseG;
	Input *InDiffuseB;
	Input *InAlpha;
	Input *InOpacity;
	Input *InDiffuseTex;

	Input *InSpecularNest;
	Input *InSpecularColorNest;
	Input *InSpecularR;
	Input *InSpecularG;
	Input *InSpecularB;
	Input *InSpecularExponent;
	Input *InSpecularIntensity;

	Input *InTransmittanceNest;
	Input *InTransmittanceColorNest;
	Input *InTransmittanceR, *InTransmittanceG, *InTransmittanceB;
	Input *InTransmittanceSaturation;
	Input *InTransmittanceAlphaDetail;
	Input *InTransmittanceColorDetail;

	Input *InReceivesLighting, *InReceivesShadows;
	Input *InUseTwoSidedLighting;

	// These member variables are used if this material isn't attached to an owner, otherwise the
	// data is retrieved from the inputs
	Color4f DiffuseColor;				// alpha component not used
	Color4f SpecularColor;				// alpha component not used
	float32 SpecularExponent;
	float32 Alpha;
	float32 Opacity;

public:
	MtlStdInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~MtlStdInputs3D();

	// TODO - these are virtual so that they may be accessed outside of the 3D DLL
	// TODO - shouldn't these be in the base class?
	virtual void SetDiffuseColor(TimeStamp t, Color4f &color);				
	virtual void SetSpecularColor(TimeStamp t, Color4f &color);	
	virtual void SetAlpha(TimeStamp t, float32 alpha);
	virtual void SetOpacity(TimeStamp t, float32 opacity);
	virtual void SetSpecularExponent(TimeStamp t, float32 specularExponent);

	virtual void ShowOverrideMessage(const char *msg);
	virtual void UsingExternalMaterial(bool usingExternalMtl);

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);
	
	virtual bool AddInputsTagList(TagList &tags);
	virtual void ShowInputs(bool visible);
};



#endif
