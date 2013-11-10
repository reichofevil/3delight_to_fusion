//@ Project "Fu3D"

#include "stdafx.h"

#define MODULENAME "3DMaterialCook"

#define BaseClass MtlData3D 
#define ThisClass MtlCookData3D 

#define ALLOW_GL_COLOR_FUNCTIONS
#define ALLOW_LIGHT_MODEL_COLOR_CONTROL

#ifdef FuPLUGIN
#include "FuPlugin.h"
#endif

#include "Resource.h"
#include "GLTexture.h"
#include "PipelineDoc.h"
#include "GLExtensions.h"
#include "TextureManager.h"

#include "3D_Stream.h"
#include "3D_MtlGraph.h"
#include "3D_LightBase.h"
#include "3D_CameraBase.h"
#include "3D_ShadeContext.h"
#include "3D_TextureImage.h"
#include "3D_MaterialCook.h"
#include "3D_LightBaseImplSW.h"
#include "3D_RenderContextGL.h"
#include "3D_RenderContextSW.h"
#include "3D_MaterialConstant.h"


static const FuID LongID_MtlCook = COMPANY_ID "advMat";			// All of the classes associated with MtlBlinn will add this REGID_MaterialLongID tag to their registry lists.


//
//
// MtlCookData3D class
//
//
FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlCookData, CT_MtlData3D)
	REGS_Name,					COMPANY_ID_SPACE "AdvancedMaterial",
	REGID_MaterialLongID,	LongID_MtlCook,
	TAG_DONE);

MtlCookData3D::MtlCookData3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	// The most common case is that you will always add the same managed parameters in the same order, however, if that
	// is not the case you must tell Fusion that it cannot cache a input -> param lookup table.  This forces Fusion to do 
	// brute force string matching rather than an indexed lookup.
	// m_AccelInputToParam = false;
}

MtlCookData3D::MtlCookData3D(const MtlCookData3D &toCopy) : BaseClass(toCopy)
{
	//// Any managed parameters/children will get copied by the BaseClass copy ctor so no need to copy them here.

	DiffuseMtl				= toCopy.DiffuseMtl;
	SpecularMtl				= toCopy.SpecularMtl;
	SpecularIntensityMtl	= toCopy.SpecularIntensityMtl;
	SpecularExponentMtl	= toCopy.SpecularExponentMtl;
	BumpmapMtl				= toCopy.BumpmapMtl;

	Diffuse					= toCopy.Diffuse;
	Specular					= toCopy.Specular;
	SpecularIntensity		= toCopy.SpecularIntensity;
	Transmittance			= toCopy.Transmittance;
	SpecularExponent		= toCopy.SpecularExponent;
	Opacity					= toCopy.Opacity;
	Saturation				= toCopy.Saturation;	
	AlphaDetail				= toCopy.AlphaDetail;	
	ColorDetail				= toCopy.ColorDetail;
}

MtlCookData3D::~MtlCookData3D()
{
}

Data3D *MtlCookData3D::CopyTagList(TagList &tags)
{
	return new MtlCookData3D(*this);
}

bool MtlCookData3D::CreateManagedParams()
{
	ADD_MANAGED_PARAM(Diffuse);				
	ADD_MANAGED_PARAM(Specular);
	ADD_MANAGED_PARAM(Refl);
	ADD_MANAGED_PARAM(Opacity);
	ADD_MANAGED_PARAM(SpecularIntensity);
	ADD_MANAGED_PARAM(SpecularExponent);

	ADD_MANAGED_PARAM(Transmittance);
	ADD_MANAGED_PARAM(Saturation);
	ADD_MANAGED_PARAM(AlphaDetail);
	ADD_MANAGED_PARAM(ColorDetail);

	ALLOC_CHILD_SLOTS(CMS_NumChildren);
	ADD_MANAGED_CHILD(DiffuseMtl, CMS_Diffuse, 4);
	ADD_MANAGED_CHILD(SpecularMtl, CMS_Specular, 4);
	ADD_MANAGED_CHILD(SpecularIntensityMtl, CMS_SpecularIntensity, 1);
	ADD_MANAGED_CHILD(SpecularExponentMtl, CMS_SpecularExponent, 1);
	ADD_MANAGED_CHILD(BumpmapMtl, CMS_Bumpmap, 4);

	return BaseClass::CreateManagedParams();
}

bool MtlCookData3D::IsOpaque() 
{ 
	if (Opacity < 1.0f || Diffuse.A < 1.0f)
		return false; 

	MtlData3D *diffuseMtl = GetChildMtl(CMS_Diffuse);

	return diffuseMtl ? diffuseMtl->IsOpaque() : true;
}

const Registry *MtlCookData3D::GetImplRegistry(uint32 type)
{
	const Registry *ret = NULL;

	if (m_RegNode == &MtlCookData3D::Reg)		// so subclasses aren't forced to also override this func
	{
		switch (type)
		{
			case CT_MtlImplSW3D:
				ret = &MtlBlinn2SW3D::Reg;
				break;
			case CT_MtlImplGL3D:
				ret = &MtlBlinn2GL3D::Reg;
				break;
		}
	}
	else
		ret = MtlData3D::GetImplRegistry(type);

	return ret;
}

void MtlCookData3D::Use()
{
	BaseClass::Use();
}

void MtlCookData3D::Recycle()
{
	BaseClass::Recycle();
}






//
//
// MtlBlinn2GL3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlImplGL3D 
#define ThisClass MtlBlinn2GL3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlBlinnGL, CT_MtlImplGL3D)
	REGS_Name,					COMPANY_ID "MtlBlinn2GL3D",
	REGID_MaterialLongID,	LongID_MtlCook,
	TAG_DONE);

MtlBlinn2GL3D::MtlBlinn2GL3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

MtlBlinn2GL3D::~MtlBlinn2GL3D()
{
}

bool MtlBlinn2GL3D::CreateChildMtls(RenderContextGL3D &rc)
{
	if (rc.ShadePath == SP_FixedFunction)
	{
		MtlData3D *diffuseMtl = MtlData->GetChildMtl(MtlCookData3D::CMS_Diffuse);
		if (diffuseMtl && diffuseMtl->IsTexture())
			DiffuseMtl = CreateChildMtl(rc, MtlCookData3D::CMS_Diffuse);

		SpecularMtl				= NULL;
		SpecularIntensityMtl = NULL;
		SpecularExponentMtl	= NULL;
		BumpmapMtl				= NULL;
	}
	else
	{
		// OPT: change this based on if lighting is enabled?
		DiffuseMtl				= CreateChildMtl(rc, MtlCookData3D::CMS_Diffuse);
		SpecularMtl				= CreateChildMtl(rc, MtlCookData3D::CMS_Specular);
		SpecularIntensityMtl = CreateChildMtl(rc, MtlCookData3D::CMS_SpecularIntensity);
		SpecularExponentMtl  = CreateChildMtl(rc, MtlCookData3D::CMS_SpecularExponent);
		BumpmapMtl           = CreateChildMtl(rc, MtlCookData3D::CMS_Bumpmap);
	}

	return true;
}

bool MtlBlinn2GL3D::ConnectParams(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct)
{
	BlinnCg *sn = (BlinnCg *) node;

	sn->Diffuse						= mtlStruct.GetSubParameter("Diffuse");
	sn->Specular					= mtlStruct.GetSubParameter("Specular");
	sn->SpecularIntensity		= mtlStruct.GetSubParameter("SpecularIntensity");
	sn->SpecularExponent			= mtlStruct.GetSubParameter("SpecularExponent");

	ParamCg lightArray = shader->CreateLightArrayParameter(rc);
	if (!lightArray.IsValid())
		return false;
	lightArray.ConnectTo(mtlStruct.GetSubParameter("Lights"));

	return true;
}

bool MtlBlinn2GL3D::ConnectChildMtls(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct)
{
	BlinnCg *sn = (BlinnCg *) node;

	// diffuse texture
	sn->DiffuseMtl	= mtlStruct.GetSubParameter("DiffuseMtl");
	if (DiffuseMtl)
	{
		ShadeNodeCg *dsn = DiffuseMtl->DoCreateShadeNode(rc, shader);
		dsn->ConnectOutputTo(sn->DiffuseMtl);	
		shader->AddShadeNode(dsn);
	}
	else
		shader->SetInputToConst(sn->DiffuseMtl, WHITE4F);


	// specular color
	sn->SpecularMtl = mtlStruct.GetSubParameter("SpecularMtl");
	if (SpecularMtl)
	{											// DoCreateShadeNode
		ShadeNodeCg *ssn = SpecularMtl->DoCreateShadeNode(rc, shader);
		ssn->ConnectOutputTo(sn->SpecularMtl);	
		shader->AddShadeNode(ssn);
	}
	else
		shader->SetInputToConst(sn->SpecularMtl, WHITE4F);


	// specular intensity 
	sn->SpecularIntensityMtl = mtlStruct.GetSubParameter("SpecularIntensityMtl");
	if (SpecularIntensityMtl)
	{
		ShadeNodeCg *ssn = SpecularIntensityMtl->DoCreateShadeNode(rc, shader);
		shader->ConnectFloat4ToFloat1(ssn->m_Struct, sn->SpecularIntensityMtl);
		shader->AddShadeNode(ssn);
	}
	else
		shader->SetInputToConst(sn->SpecularIntensityMtl, 1.0f);


	// specular exponent 
	sn->SpecularExponentMtl = mtlStruct.GetSubParameter("SpecularExponentMtl");
	if (SpecularExponentMtl)
	{
		ShadeNodeCg *ssn = SpecularExponentMtl->DoCreateShadeNode(rc, shader);
		shader->ConnectFloat4ToFloat1(ssn->m_Struct, sn->SpecularExponentMtl);
		shader->AddShadeNode(ssn);
	}
	else
		shader->SetInputToConst(sn->SpecularExponentMtl, 1.0f);


	// bumpmap texture
	sn->BumpmapMtl = mtlStruct.GetSubParameter("BumpmapMtl");
	if (BumpmapMtl)
	{
		ShadeNodeCg *bsn = BumpmapMtl->DoCreateShadeNode(rc, shader);
		bsn->ConnectOutputTo(sn->BumpmapMtl);
		shader->AddShadeNode(bsn);
	}
	else
		shader->ConnectToFragmentNormal(sn->BumpmapMtl);

	return true;
}

ShadeNodeCg *MtlBlinn2GL3D::CreateShadeNode(RenderContextGL3D &rc)
{
	return new BlinnCg;
}

const char *MtlBlinn2GL3D::GetShadeNodeName(RenderContextGL3D &rc)
{
	return "FuMtlBlinn";
}

const char *MtlBlinn2GL3D::GetShaderFilename(RenderContextGL3D &rc)
{
	return "Shaders:Materials/Cg/FuMtlBlinn_f.cg";
}

bool MtlBlinn2GL3D::Activate(RenderContextGL3D &rc)
{
	if (rc.ShadePath == SP_FixedFunction)
	{
		MtlCookData3D *mpd = (MtlCookData3D *) MtlData;

		Vector4f diffuse = mpd->Opacity * mpd->Diffuse;
		glColor4fv(diffuse.V);

		if (rc.DoLighting)
		{
			Vector4f ambient = mpd->Opacity * mpd->Diffuse;
			Vector4f specular = mpd->Opacity * mpd->Specular;
			Vector4f emission = Color4f(0.0f, 0.0f, 0.0f, 0.0f);
			float32 specularExp = GLTools.LimitSpecularExponent(mpd->SpecularExponent);

			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient.V);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse.V);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular.V);
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission.V);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, specularExp);

			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, UseTwoSidedLighting);
		}
	}
	else if (rc.ShadePath == SP_Cg)
	{
		//ActivateManagedParams(rc);		// the MaterialGL base class will call this by default

		BlinnCg *sn = (BlinnCg *) rc.CgShader->GetShadeNode(this);
		MtlCookData3D *data = (MtlCookData3D *) MtlData;

		data->Specular.A = 1.0f;

		sn->Diffuse.Set(data->Opacity * data->Diffuse);
		sn->Specular.Set(data->Opacity * data->Specular);

		sn->SpecularIntensity.Set(data->SpecularIntensity);
		sn->SpecularExponent.Set(data->SpecularExponent);
	}

	return true;
}








//
//
// MtlBlinn2SW3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlImplSW3D 
#define ThisClass MtlBlinn2SW3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlBlinnSW, CT_MtlImplSW3D)
	REGS_Name,					COMPANY_ID "MtlBlinnSW",
	REGID_MaterialLongID,	LongID_MtlCook,
	TAG_DONE);

MtlBlinn2SW3D::MtlBlinn2SW3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	MtlCookData3D *mtlData = (MtlCookData3D *) tags.GetPtr(MTL_MaterialData, NULL);
	RenderContextSW3D &rc = *((RenderContextSW3D *) tags.GetPtr(MTL_RenderContext, NULL));

	DiffuseMtl = CreateChildMtl(rc, MtlCookData3D::CMS_Diffuse);

	if (rc.DoLighting)
	{
		SpecularMtl = CreateChildMtl(rc, MtlCookData3D::CMS_Specular);
		SpecularIntensityMtl = CreateChildMtl(rc, MtlCookData3D::CMS_SpecularIntensity);
		SpecularExponentMtl = CreateChildMtl(rc, MtlCookData3D::CMS_SpecularExponent);
		BumpmapMtl = CreateChildMtl(rc, MtlCookData3D::CMS_Bumpmap);

		SpecularIntensity = mtlData->SpecularIntensity;
		SpecularExponent = mtlData->SpecularExponent;

		PreSpecular = mtlData->Specular * mtlData->SpecularIntensity;
		Specular = mtlData->Specular * mtlData->Opacity/* * mtlData->SpecularIntensity*/;
	}
	else
	{
		SpecularMtl = NULL;
		SpecularIntensityMtl = NULL;
		SpecularExponentMtl = NULL;
		BumpmapMtl = NULL;
	}

	Diffuse = mtlData->Diffuse * mtlData->Opacity;

	TransmittanceMtl = NULL;

	PreDiffuse = mtlData->Diffuse;

	Transmittance = mtlData->Transmittance;
	Saturation = mtlData->Saturation;
	AlphaDetail = mtlData->AlphaDetail;
	ColorDetail = mtlData->ColorDetail;

	ShadeFunc = (ShadeFunc3D) &MtlBlinn2SW3D::ShadeFragment;
}

MtlBlinn2SW3D::~MtlBlinn2SW3D()
{
}

inline static Vector3f reflect(const Vector3f &p, const Vector3f &n)
{
	return p - 2.0f * Dot(n, p) * n;
}

inline static Vector3f reflect(const Vector4f &p, const Vector4f &n)
{
	return reflect(Vector3f(p), Vector3f(n));
}

inline static Vector3f refract(const Vector3f &p, const Vector3f &n, float etaRatio)
{
	float a = Dot(-p, n);
	float b = 1.0f - etaRatio * etaRatio * (1.0f - a * a);
	Vector3f c = etaRatio * p + ((etaRatio * a - sqrtf(fabs(b))) * n);
	return b > 0.0f ? c : Vector3f(0.0f);
}

inline static Vector3f refract(const Vector4f &p, const Vector4f &n, float etaRatio)
{
	return refract(Vector3f(p), Vector3f(n), etaRatio);
}

ParamBlockSW *MtlBlinn2SW3D::CreateParamBlock(RenderContextSW3D &rc)
{
	MtlCookData3D *mtlData = (MtlCookData3D *) MtlData;

	BlinnBlock *block = new BlinnBlock;
	block->Size = sizeof(BlinnBlock);

	ParamSW *param;

	param = block->CreateParam(block->VertexColor, Stream_VertexColor, CS_Unset);
	param->SetEnabled(true);
	param->SetRequired(false);

	param = block->CreateParam(block->EyePosition, Stream_Position, CS_Eye);
	param->SetEnabled(rc.DoLighting);
	param->SetRequired(true);

	param = block->CreateParam(block->EyeNormal, Stream_Normal, CS_Eye);
	param->SetEnabled(rc.DoLighting);
	param->SetRequired(true);

	param = block->CreateParam(block->Color, Stream_Color, CS_Unset);
	param->SetEnabled(true);
	param->SetRequired(false);

	return block;
}

void MtlBlinn2SW3D::ShadeFragment(ShadeContext3D &sc)
{
	BlinnBlock &block = (BlinnBlock &) sc.GetMaterialBlock(this);

	// diffuse color
	const Vector4f &vertexColor = IsSet(block.VertexColor) ? *block.VertexColor : WHITE4F;

	Color4f diffuseMtl;	

	if (DiffuseMtl)
	{
		DiffuseMtl->Shade(sc);
		diffuseMtl = *block.Color;
	}
	else
		diffuseMtl = WHITE4F;

	Color4f Kd = Mul(vertexColor, Mul(Diffuse, diffuseMtl));


	if (!sc.DoLighting || (!ReceivesLighting && !ReceivesShadows))
		*block.Color = Kd;
	else
	{
		Vector4f oldNormal;
		Color4f diffuseMtl;

		// position
		Vector4f p = Normalize3(*block.EyePosition);

		// normal
		if (BumpmapMtl)
		{
			oldNormal = *block.EyeNormal;							// save normal
			BumpmapMtl->Shade(sc);
		}

		Vector4f n = Normalize3(*block.EyeNormal);

		if (BumpmapMtl)
			*block.EyeNormal = oldNormal;							// restore normal

		if (UseTwoSidedLighting && Dot3(n, p) > 0.0f)
			n = -n;

		// specular
		Color4f specularMtl;
		float32 specularIntensityMtl, specularExponentMtl;

		if (SpecularMtl)
		{
			SpecularMtl->Shade(sc);
			specularMtl = *block.Color;
		}
		else
			specularMtl = WHITE4F;

		if (SpecularIntensityMtl)
		{
			SpecularIntensityMtl->Shade(sc);
			specularIntensityMtl = block.Color->A;
		}
		else
			specularIntensityMtl = 1.0f;

		if (SpecularExponentMtl)
		{
			SpecularExponentMtl->Shade(sc);
			specularExponentMtl = block.Color->A;
		}
		else
			specularExponentMtl = 1.0f;

		Color4f Ks = (SpecularIntensity * specularIntensityMtl) * Mul(vertexColor, Mul(Specular, specularMtl));

		float32 specularExponent = SpecularExponent * specularExponentMtl;

		// do blinn lighting
		Color4f ambientAccum = ZERO4F;
		Color4f diffuseAccum = ZERO4F;
		Color4f specularAccum = ZERO4F;

		//uassert(sc.X != 742 || sc.Y != 320);

		const int nLights = sc.NumLights;
		for (int i = 0; i < nLights; i++)
		{
			if (sc.Illuminate(i))
			{
				if (ReceivesLighting)
				{
					if (sc.CastsShadows(i) && ReceivesShadows)
					{
						// get the transmittance (incl shadowdensity) and transmitted light and use them to apply shadows
						sc.Shadow(i);
						sc.LightColor = Lerp(sc.LightColor, sc.TransmittedLight, sc.Transmittance.A);
					}

					const Vector4f &l = (const Vector4f &) sc.LightVector;

					ambientAccum += sc.AmbientLightColor;

					if (sc.DoDiffuse)
					{
						float32 diffuse = maxf(Dot3(n, l), 0.0f);
						diffuseAccum += diffuse * Mul(Kd, sc.LightColor);
					}

					if (sc.DoSpecular)
					{
						Vector4f h = Normalize3(l - p);		// half angle vector
						float32 specular = powf(maxf(Dot3(n, h), 0.0f), specularExponent);
						specularAccum += specular * sc.LightColor;
					}
				}
				else
				{
					if (sc.CastsShadows(i) && ReceivesShadows)
					{
						sc.Shadow(i);
 						diffuseAccum += Lerp(Kd, Mul(sc.Transmittance, Kd), sc.Transmittance.A);
					}
				}
			}
		}

		if (!sc.DoAmbient)
			ambientAccum = ZERO4F;

		// combine diffuse/specular/ambient
		Color4f outColor = Mul(ambientAccum, Kd) + diffuseAccum + Mul(specularAccum, Ks);
		outColor.A = Kd.A;

		FuASSERT(!outColor.IsNaN(), ("MtlBlinn2SW3D::NaN output"));

		*block.Color = outColor;
	}
}

bool MtlBlinn2SW3D::PreRender(RenderContextSW3D &rc)
{
	return true;
}

void MtlBlinn2SW3D::PostRender(RenderContextSW3D &rc)
{
}

bool MtlBlinn2SW3D::Activate(RenderContextSW3D &rc)
{
	return true;
}

void MtlBlinn2SW3D::Deactivate(RenderContextSW3D &rc)
{
}

void MtlBlinn2SW3D::Transmit(ShadeContext3D &sc)
{
	BlinnBlock &block = (BlinnBlock &) sc.GetMaterialBlock(this);
	const Vector4f &vertexColor = IsSet(block.VertexColor) ? *block.VertexColor : WHITE4F;

	Color4f diffuseMtl;

	if (DiffuseMtl)
	{
		DiffuseMtl->Shade(sc);
		diffuseMtl = *block.Color;
	}
	else
		diffuseMtl = WHITE4F;

	Color4f Kd = Mul(PreDiffuse, Mul(diffuseMtl, vertexColor));

	if (fabs(Kd.A) < 0.0001f)
		Kd = Color4f(1.0f, 1.0f, 1.0f, 0.0f);
	else
	{
		Kd.R /= Kd.A;
		Kd.G /= Kd.A;
		Kd.B /= Kd.A;
	}

	float32 y = Kd.Luminance();
	Color4f satColor = Lerp(Color4f(y, y, y, 1.0f), Kd, Saturation);
	satColor.A = 1.0f;

	Color4f textureDetail = Lerp(Color4f(0.0f, 0.0f, 0.0f, 1.0f), satColor, ColorDetail);
	textureDetail.A = 1.0f;

	float32 alphaDetail = Lerp(0.0f, 1.0f - Kd.A, AlphaDetail);
	Color4f outTrans = (1.0f - alphaDetail) * textureDetail + Color4f(alphaDetail);			// compute 1 - (1 - ad) * (1 - td)

	outTrans = Color4f(1.0f) - Mul(Color4f(1.0f) - outTrans, Color4f(1.0f) - Transmittance);

	sc.Transmittance.R = max(min(outTrans.R, 1.0f), 0.0f);
	sc.Transmittance.G = max(min(outTrans.G, 1.0f), 0.0f);
	sc.Transmittance.B = max(min(outTrans.B, 1.0f), 0.0f);
	sc.Transmittance.A = outTrans.A;

	FuASSERT(sc.Transmittance.A >= 0.0f && sc.Transmittance.A <= 1.002f, (""));
}








//
//
// MtlCookInputs3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlInputs3D 
#define ThisClass MtlCookInputs3D 

FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlCookInputs, CT_MtlInputs3D)
	REGS_Name,					COMPANY_ID "Adv Material Inputs",
	REGID_MaterialLongID,	LongID_MtlCook,
	TAG_DONE);

MtlCookInputs3D::MtlCookInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	InDiffuseNest = NULL;
	InDiffuseColorNest = NULL;
	InDiffuseR = NULL;
	InDiffuseG = NULL;
	InDiffuseB = NULL;
	InAlpha = NULL;
	InOpacity = NULL;

	InSpecularNest = NULL;
	InSpecularColorNest = NULL;
	InSpecularR = NULL;
	InSpecularG = NULL;
	InSpecularB = NULL;
	InSpecularExponent = NULL;
	InSpecularIntensity = NULL;

	InTransmittanceNest = NULL;
	InTransmittanceColorNest = NULL;
	InTransmittanceR = NULL;
	InTransmittanceG = NULL;
	InTransmittanceB = NULL;
	InTransmittanceSaturation = NULL;
	InTransmittanceAlphaDetail = NULL;
	InTransmittanceColorDetail = NULL;
	InShowTransmittanceMtlInput = NULL;

	InReceivesLighting = NULL;
	InReceivesShadows = NULL;
	InUseTwoSidedLighting = NULL;

	InputsAdded = false;
}

MtlCookInputs3D::~MtlCookInputs3D()
{
}

bool MtlCookInputs3D::RegInit(Registry *regnode)
{
	return BaseClass::RegInit(regnode);
}

void MtlCookInputs3D::RegCleanup(Registry *regnode)
{
	BaseClass::RegCleanup(regnode);
}

bool MtlCookInputs3D::SetAttrsTagList(const TagList &tags)
{
	return BaseClass::SetAttrsTagList(tags);
}

bool MtlCookInputs3D::CopyAttr(Tag tag, void *addr) const
{
	return BaseClass::CopyAttr(tag, addr);
}

void MtlCookInputs3D::OnAddToFlow(TagList *tags)
{
	BaseClass::OnAddToFlow(tags);
}

void MtlCookInputs3D::OnRemoveFromFlow(TagList *tags)
{
	BaseClass::OnRemoveFromFlow(tags);
}

bool MtlCookInputs3D::AddInputsTagList(TagList &tags)
{
	AddControlPage("Controls");
	AddAllCookInputs();
	return BaseClass::AddInputsTagList(tags);
}

void MtlCookInputs3D::ShowInputs(bool visible)
{
	BaseClass::ShowInputs(visible);
}

void MtlCookInputs3D::AddAllCookInputs()
{
	AddCoatingInputs();

	AddDiffuseCookInputs();

	BeginSpecularCookNest();
		AddSpecularCookInputs();
	EndSpecularCookNest();

	AddReflCookInputs();

	AddSSSCookInputs();

	AddBumpmapCookInputs();

	AddGICookInputs();

	AddTransmittanceCookInputs();

	AddLightingShadowsCookInputs();
}

void MtlCookInputs3D::AddCoatingInputs()
{
	InCoatingLayerNest = BeginGroup("CoatingLayer", "CoatingLayer",true,true);
	//InCoatingNest = BeginGroup("CoatingColor", "CoatingColor", true, true);
		InCoatingOn = AddInput("Coating On", "coating_on", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		CHECKBOXCONTROL_ID,
			LINKID_LegacyID,			FuID(".coating_on"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("coating_on"),
			INP_Default,				1.0,
			TAG_DONE);

		InCoatingColorNest = BeginGroup("Color", "Color", true, false);

			InCoatingR = AddInput("Red", "Red",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..CoatingRed"),
				I3D_AutoFlags,				PF_AutoProcess,	
				I3D_ParamName,				FuID("Coating.R"),
				IC_ControlGroup,			1,
				IC_ControlID,				0,
				ICS_Name,					"Coating Color",
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_Default,				1.0,
				INP_SubType,				IST_ColorR,
				TAG_DONE);

			InCoatingG = AddInput("Green", "Green",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..CoatingGreen"),
				I3D_AutoFlags,				PF_AutoProcess,
				I3D_ParamName,				FuID("Coating.G"),
				IC_ControlGroup,			1,
				IC_ControlID,				1,
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_Default,				1.0,
				INP_SubType,				IST_ColorG,
				TAG_DONE);

			InCoatingB = AddInput("Blue", "Blue",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..CoatingBlue"),
				I3D_AutoFlags,				PF_AutoProcess,
				I3D_ParamName,				FuID("Coating.B"),
				IC_ControlGroup,			1,
				IC_ControlID,				2,
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_Default,				1.0,
				INP_SubType,				IST_ColorB,
				TAG_DONE);

		EndGroup();

		InCoatingThick = AddInput("Thickness", "Thickness", 
			LINKID_DataType,			CLSID_DataType_Number, 
			INPID_InputControl,		SLIDERCONTROL_ID,
			LINKID_LegacyID,			FuID(".coatThickness"),
			I3D_AutoFlags,				PF_AutoProcess,
			INP_Default,				0.0,
			INP_MinAllowed,			0.0,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			TAG_DONE);

		InCoatingRough = AddInput("coating roughness", "coatingroughness", 
			LINKID_DataType,			CLSID_DataType_Number, 
			INPID_InputControl,		SLIDERCONTROL_ID,
			LINKID_LegacyID,			FuID(".CoatingRoughness"),
			I3D_AutoFlags,				PF_AutoProcess,
			INP_Default,				0,
			INP_MinAllowed,			0.0,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			TAG_DONE);

		InCoatIOR = AddInput("Coating IOR", "coat_IOR",
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		SLIDERCONTROL_ID,
			LINKID_LegacyID,			FuID("..coatIOR"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("CoatIOR"),
			INP_MinScale,				0.0,
			INP_MaxScale,				5.0,
			INP_Default,				1.3,
			TAG_DONE);
		
		InCoatSamples = AddInput("CoatSamples", "coat_samples",
			LINKID_DataType,		CLSID_DataType_Number,
			INPID_InputControl,	SLIDERCONTROL_ID,
			INP_Integer,			TRUE,
			INP_MinAllowed,			1.0,
			INP_MinScale,			1.0,
			INP_MaxScale,			256.0,
			INP_Default,			8.0,
			TAG_DONE);

		InCoatTransNest = BeginGroup("Coating tranmittance Color", "CoatTransColor", true, false);

			InCoatTransR = AddInput("Coat Transm Red", "CoatTransRed",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..CoatTransRed"),
				I3D_AutoFlags,				PF_AutoProcess,
				ICS_Name,					"Coat Transmittance",
				IC_ControlGroup,			1,
				IC_ControlID,				0,
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_Default,				1.0,
				INP_SubType,				IST_ColorR,
				TAG_DONE);

			InCoatTransG = AddInput("Coat Transm Green", "CoatTransGreen",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..CoatTransGreen"),
				I3D_AutoFlags,				PF_AutoProcess,
				IC_ControlGroup,			1,
				IC_ControlID,				1,
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_Default,				1.0,
				INP_SubType,				IST_ColorG,
				TAG_DONE);

			InCoatTransB = AddInput("Coat Transm Blue", "CoatTransBlue",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..CoatTransBlue"),
				I3D_AutoFlags,				PF_AutoProcess,
				IC_ControlGroup,			1,
				IC_ControlID,				2,
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_Default,				1.0,
				INP_SubType,				IST_ColorB,
				TAG_DONE);
			EndGroup();

		InCoatingRefl = AddInput("has reflection", "coating_refl", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		CHECKBOXCONTROL_ID,
			LINKID_LegacyID,			FuID(".coating_refl"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("coating_refl"),
			INP_Default,				1.0,
			TAG_DONE);

		InCoatingSpec = AddInput("has specualar", "coating_spec", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		CHECKBOXCONTROL_ID,
			LINKID_LegacyID,			FuID(".coating_spec"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("coating_spec"),
			INP_Default,				1.0,
			TAG_DONE);

		InCoatingEnv = AddInput("has enviroment", "coating_env", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		CHECKBOXCONTROL_ID,
			LINKID_LegacyID,			FuID(".coating_env"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("coating_env"),
			INP_Default,				1.0,
			TAG_DONE);
	
		
	//EndGroup();
	EndGroup();
}
void MtlCookInputs3D::AddDiffuseCookInputs()
{
	InBaseLayerNest = BeginGroup("BaseLayer", "BaseLayer",true,true);
	InDiffuseNest = BeginGroup("Diffuse", "Diffuse", true, false);

		InDiffuseColorNest = BeginGroup("Color", "Color", true, false);

			InDiffuseR = AddInput("Red", "Red",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..DiffuseRed"),
				I3D_AutoFlags,				PF_AutoProcess,	
				I3D_ParamName,				FuID("Diffuse.R"),
				IC_ControlGroup,			1,
				IC_ControlID,				0,
				ICS_Name,					"Diffuse Color",
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_Default,				1.0,
				INP_SubType,				IST_ColorR,
				TAG_DONE);

			InDiffuseG = AddInput("Green", "Green",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..DiffuseGreen"),
				I3D_AutoFlags,				PF_AutoProcess,
				I3D_ParamName,				FuID("Diffuse.G"),
				IC_ControlGroup,			1,
				IC_ControlID,				1,
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_Default,				1.0,
				INP_SubType,				IST_ColorG,
				TAG_DONE);

			InDiffuseB = AddInput("Blue", "Blue",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..DiffuseBlue"),
				I3D_AutoFlags,				PF_AutoProcess,
				I3D_ParamName,				FuID("Diffuse.B"),
				IC_ControlGroup,			1,
				IC_ControlID,				2,
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_Default,				1.0,
				INP_SubType,				IST_ColorB,
				TAG_DONE);

			InAlpha = AddInput("Alpha", "Alpha", 
				LINKID_DataType,			CLSID_DataType_Number, 
				INPID_InputControl,		SLIDERCONTROL_ID,
				LINKID_LegacyID,			FuID("..Alpha"),
				I3D_AutoFlags,				PF_AutoProcess,
				I3D_ParamName,				FuID("Diffuse.A"),
				INP_Default,				DefaultAlpha,
				INP_MinAllowed,			0.0,
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				IC_Visible,					FALSE,
				TAG_DONE);

			InDiffuseMtl = AddInput("Diffuse Color Material", "Material",
				LINKID_DataType,			CLSID_DataType_Image,//CLSID_DataType_MtlGraph3D,
				//LINKID_AllowedDataType,	CLSID_DataType_Image,
				LINKID_LegacyID,			FuID("..DiffuseTex"),
				I3D_AutoFlags,				PF_AutoProcess,
				I3D_ParamName,				FuID("DiffuseMtl"),
				I3D_ChildSlot,				MtlCookData3D::CMS_Diffuse,
				LINK_Main,					MMID_DiffuseColor,
				INP_Required,				FALSE,
				TAG_DONE);

		EndGroup();

		InOpacity = AddInput("Opacity", "Opacity", 
			LINKID_DataType,			CLSID_DataType_Number, 
			INPID_InputControl,		SLIDERCONTROL_ID,
			LINKID_LegacyID,			FuID(".Opacity"),
			I3D_AutoFlags,				PF_AutoProcess,
			INP_Default,				DefaultOpacity,
			INP_MinAllowed,			0.0,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			TAG_DONE);

		InRough = AddInput("roughness", "roughness", 
			LINKID_DataType,			CLSID_DataType_Number, 
			INPID_InputControl,		SLIDERCONTROL_ID,
			LINKID_LegacyID,			FuID(".Roughness"),
			I3D_AutoFlags,				PF_AutoProcess,
			INP_Default,				.1,
			INP_MinAllowed,			0.0,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			TAG_DONE);

		InIncanColorNest = BeginGroup("IncandescenceColor", "IncandescenceColor", true, false);

			InIncaR = AddInput("IncaRed", "IncaRed",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..IncaRed"),
				I3D_AutoFlags,				PF_AutoProcess,
				ICS_Name,					"Incandescence Color",
				IC_ControlGroup,			1,
				IC_ControlID,				0,
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_Default,				0,
				INP_SubType,				IST_ColorR,
				TAG_DONE);

			InIncaG = AddInput("IncaGreen", "IncaGreen",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..IncaGreen"),
				I3D_AutoFlags,				PF_AutoProcess,
				IC_ControlGroup,			1,
				IC_ControlID,				1,
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_Default,				0,
				INP_SubType,				IST_ColorG,
				TAG_DONE);

			InIncaB = AddInput("IncaBlue", "IncaBlue",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..IncaBlue"),
				I3D_AutoFlags,				PF_AutoProcess,
				IC_ControlGroup,			1,
				IC_ControlID,				2,
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_Default,				0,
				INP_SubType,				IST_ColorB,
				TAG_DONE);
			EndGroup();
		
	EndGroup();
	EndGroup();
}


void MtlCookInputs3D::BeginSpecularCookNest()
{
	InSpecularNest = BeginGroup("Reflection", "Reflection");
}

void MtlCookInputs3D::AddSpecularCookInputs(bool addSpecularExponent)
{
	InSpecularColorNest = BeginGroup("Color", "Color", true, false);

		InSpecularR = AddInput("Refl Red", "Red",
			LINKID_DataType,			CLSID_DataType_Number, 
			INPID_InputControl,		COLORCONTROL_ID,
			LINKID_LegacyID,			FuID("..ReflRed"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("Refl.R"),
			IC_ControlGroup,			2,
			IC_ControlID,				0,
			ICS_Name,					"Reflect Color",
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				DefaultSpecularColor.R,
			INP_SubType,				IST_ColorR,
			TAG_DONE);

		InSpecularG = AddInput("Green", "Green", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		COLORCONTROL_ID,
			LINKID_LegacyID,			FuID("..ReflGreen"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("Refl.G"),
			IC_ControlGroup,			2,
			IC_ControlID,				1,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				DefaultSpecularColor.G,
			INP_SubType,				IST_ColorG,
			TAG_DONE);

		InSpecularB = AddInput("Blue", "Blue", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		COLORCONTROL_ID,
			I3D_AutoFlags,				PF_AutoProcess,
			LINKID_LegacyID,			FuID("..ReflBlue"),
			I3D_ParamName,				FuID("Refl.B"),
			IC_ControlGroup,			2,
			IC_ControlID,				2,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				DefaultSpecularColor.B,
			INP_SubType,				IST_ColorB,
			TAG_DONE);

		InSpecularColorMtl = AddInput("Refl Color Material", "Material",
			LINKID_DataType,			CLSID_DataType_MtlGraph3D,
			LINKID_AllowedDataType,	CLSID_DataType_Image,
			LINKID_LegacyID,			FuID("..ReflColorTex"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("SpecularMtl"),
			I3D_ChildSlot,				MtlCookData3D::CMS_Specular,
			LINK_Main,					MMID_SpecularColor,
			INP_Required,				FALSE, 
			TAG_DONE);

	EndGroup();

	InSpecularIntensity = AddInput("Specular Intensity", "Intensity", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".SpecularIntensity"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("SpecularIntensity"),
		INP_MinAllowed,			0.0,
		INP_MinScale,				0.0,
		INP_MaxScale,				1.0,
		INP_Default,				1.0,
		IC_Visible,					FALSE,
		TAG_DONE);

	InSpecularIntensityMtl = AddInput("Specular Intensity Material", "Intensity.Material",
		LINKID_DataType,			CLSID_DataType_MtlGraph3D,
		LINKID_AllowedDataType,	CLSID_DataType_Image,
		LINKID_LegacyID,			FuID(".SpecularIntensityTex"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("SpecularIntensityMtl"),
		I3D_ChildSlot,				MtlCookData3D::CMS_SpecularIntensity,
		LINK_Main,					MMID_SpecularIntensity,
		INP_Required,				FALSE, 
		TAG_DONE);


	if (addSpecularExponent)
	{
		InSpecularExponent = AddInput("Specular Exponent", "Exponent", 
			LINKID_DataType,			CLSID_DataType_Number, 
			INPID_InputControl,		SLIDERCONTROL_ID,
			LINKID_LegacyID,			FuID(".SpecularExponent"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("SpecularExponent"),
			INP_Default,				DefaultSpecularExponent,
			INP_MinAllowed,			1.0,
			INP_MinScale,				1.0,
			INP_MaxScale,				100.0,
			ICD_Center,					25.0,
			IC_Visible,					FALSE,
			TAG_DONE);

		InSpecularExponentMtl = AddInput("Specular Roughness Material", "Roughness.Material",
			LINKID_DataType,			CLSID_DataType_MtlGraph3D,
			LINKID_AllowedDataType,	CLSID_DataType_Image,
			LINKID_LegacyID,			FuID(".SpecularExponentTex"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("SpecularExponentMtl"),
			I3D_ChildSlot,				MtlCookData3D::CMS_SpecularExponent,
			LINK_Main,					MMID_SpecularExponent,
			INP_Required,				FALSE,
			TAG_DONE);
	}
	InSpecularIOR = AddInput("Reflection IOR ", "refl_IOR", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".refl_IOR"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("refl_IOR"),
		INP_MinAllowed,			0.0,
		INP_MinScale,				0.0,
		INP_MaxScale,				5.0,
		INP_Default,				1.45,
		TAG_DONE);

	InSpecularRoughness = AddInput("Refl roughness", "refl_rough", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".refl_rough"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("refl_rough"),
		INP_MinAllowed,			0.0,
		INP_MinScale,				0.0,
		INP_MaxScale,				1.0,
		INP_Default,				0.1,
		TAG_DONE);

	InSpecularRefl = AddInput("reflect geo", "sepc_refl", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		CHECKBOXCONTROL_ID,
			LINKID_LegacyID,			FuID(".sepc_refl"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("sepc_refl"),
			INP_Default,				1.0,
			TAG_DONE);

	InSpecularSpec = AddInput("reflect lights", "sepc_spec", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		CHECKBOXCONTROL_ID,
			LINKID_LegacyID,			FuID(".sepc_spec"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("sepc_spec"),
			INP_Default,				1.0,
			TAG_DONE);

	InSpecularEnv = AddInput("reflect enviroment", "sepc_env", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		CHECKBOXCONTROL_ID,
			LINKID_LegacyID,			FuID(".sepc_env"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("sepc_env"),
			INP_Default,				1.0,
			TAG_DONE);

	InSpecSamples = AddInput("Reflection Samples", "ReflSamples",
			LINKID_DataType,		CLSID_DataType_Number,
			INPID_InputControl,	SLIDERCONTROL_ID,
			INP_Integer,			TRUE,
			INP_MinAllowed,		1.0,
			INP_MinScale,			1.0,
			INP_MaxScale,			128.0,
			INP_Default,			8.0,
			TAG_DONE);

	InSpecularAniso = AddInput("anisotropic", "sepc_aniso", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".refl_aniso"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("refl_aniso"),
		INP_MinScale,				-1.0,
		INP_MaxScale,				1.0,
		INP_Default,				0.0,
		TAG_DONE);
	
	InSpecularAnisoColorNest = BeginGroup("AnisoDirection", "AnisoDirection", true, false);

		InSpecularAnisoR = AddInput("Aniso Red", "AnisoRed",
			LINKID_DataType,			CLSID_DataType_Number, 
			INPID_InputControl,		COLORCONTROL_ID,
			LINKID_LegacyID,			FuID("..ReflAnisoRed"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("ReflAniso.R"),
			IC_ControlGroup,			2,
			IC_ControlID,				0,
			ICS_Name,					"ReflectAnisoDirection",
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				0.5,
			INP_SubType,				IST_ColorR,
			TAG_DONE);

		InSpecularAnisoG = AddInput("Aniso Green", "AnisoGreen", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		COLORCONTROL_ID,
			LINKID_LegacyID,			FuID("..ReflAnisoGreen"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("ReflAniso.G"),
			IC_ControlGroup,			2,
			IC_ControlID,				1,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				1.0,
			INP_SubType,				IST_ColorG,
			TAG_DONE);

		InSpecularAnisoB = AddInput("Aniso Blue", "AnisoBlue", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		COLORCONTROL_ID,
			I3D_AutoFlags,				PF_AutoProcess,
			LINKID_LegacyID,			FuID("..ReflAnisoBlue"),
			I3D_ParamName,				FuID("ReflAniso.B"),
			IC_ControlGroup,			2,
			IC_ControlID,				2,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				0.5,
			INP_SubType,				IST_ColorB,
			TAG_DONE);

	EndGroup();
}

void MtlCookInputs3D::EndSpecularCookNest()
{
	EndGroup();
}

void MtlCookInputs3D::AddReflCookInputs()
{
	InReflNest = BeginGroup("Refraction", "Refraction");

	  InReflColorNest = BeginGroup("Refraction Color", "Color", true, false);

		InReflR = AddInput("Refraction Red", "Red",
			LINKID_DataType,			CLSID_DataType_Number, 
			INPID_InputControl,		COLORCONTROL_ID,
			LINKID_LegacyID,			FuID("..RefractionRed"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("Refraction.R"),
			IC_ControlGroup,			2,
			IC_ControlID,				0,
			ICS_Name,					"Refraction Color",
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				0,
			INP_SubType,				IST_ColorR,
			TAG_DONE);

		InReflG = AddInput("Green", "Green", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		COLORCONTROL_ID,
			LINKID_LegacyID,			FuID("..RefractionGreen"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("Refraction.G"),
			IC_ControlGroup,			2,
			IC_ControlID,				1,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				0,
			INP_SubType,				IST_ColorG,
			TAG_DONE);

		InReflB = AddInput("Blue", "Blue", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		COLORCONTROL_ID,
			I3D_AutoFlags,				PF_AutoProcess,
			LINKID_LegacyID,			FuID("..RefractionBlue"),
			I3D_ParamName,				FuID("Refraction.B"),
			IC_ControlGroup,			2,
			IC_ControlID,				2,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				0,
			INP_SubType,				IST_ColorB,
			TAG_DONE);

		InReflColorMtl = AddInput("Refraction Color Material", "Material",
			LINKID_DataType,			CLSID_DataType_Image,//CLSID_DataType_MtlGraph3D,
			//LINKID_AllowedDataType,	CLSID_DataType_Image,
			LINKID_LegacyID,			FuID("..RefractionColorTex"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("SpecularMtl"),
			I3D_ChildSlot,				MtlCookData3D::CMS_Specular,
			LINK_Main,					MMID_SpecularColor,
			INP_Required,				FALSE, 
			TAG_DONE);

	EndGroup();

	InReflRoughness = AddInput("Refraction roughness", "refra_rough", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".refra_rough"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("refra_rough"),
		INP_MinAllowed,			0.0,
		INP_MinScale,				0.0,
		INP_MaxScale,				1.0,
		INP_Default,				0.1,
		TAG_DONE);

	InReflRoughMtl = AddInput("Refraction Rough Material", "RefraRoughMaterial",
			LINKID_DataType,			CLSID_DataType_MtlGraph3D,
			LINKID_AllowedDataType,	CLSID_DataType_Image,
			LINKID_LegacyID,			FuID(".RefraRoughTex"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("RefraRoughMtl"),
			I3D_ChildSlot,				MtlCookData3D::CMS_SpecularExponent,
			LINK_Main,					MMID_SpecularExponent,
			INP_Required,				FALSE,
			TAG_DONE);

	InRefrIOR = AddInput("Refraction IOR", "refra_IOR",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		SLIDERCONTROL_ID,
				LINKID_LegacyID,			FuID("..RefrIOR"),
				I3D_AutoFlags,				PF_AutoProcess,
				I3D_ParamName,				FuID("RefrIOR"),
				INP_MinScale,				0.0,
				INP_MaxScale,				5.0,
				INP_Default,				1.3,
				TAG_DONE);

	InRefraSamples = AddInput("Refraction Samples", "RefraSamples",
			LINKID_DataType,		CLSID_DataType_Number,
			INPID_InputControl,	SLIDERCONTROL_ID,
			INP_Integer,			TRUE,
			INP_MinAllowed,		1.0,
			INP_MinScale,			1.0,
			INP_MaxScale,			128.0,
			INP_Default,			4.0,
			TAG_DONE);

	InFogStrength = AddInput("Refraction Fog Strength", "fog_strength", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".fog_strength"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("fog_strength"),
		INP_MinAllowed,			0.0,
		INP_MinScale,				0.0,
		INP_MaxScale,				1.0,
		INP_Default,				0.0,
		TAG_DONE);

	InReflFogColorNest = BeginGroup("Refraction Fog Color", "FogColor", true, false);

		InReflFogR = AddInput("Refraction Fog Red", "FogRed",
			LINKID_DataType,			CLSID_DataType_Number, 
			INPID_InputControl,		COLORCONTROL_ID,
			LINKID_LegacyID,			FuID("..RefractionFogRed"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("RefractionFog.R"),
			IC_ControlGroup,			2,
			IC_ControlID,				0,
			ICS_Name,					"Refraction Fog Color",
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				0,
			INP_SubType,				IST_ColorR,
			TAG_DONE);

		InReflFogG = AddInput("Refraction Fog Green", "FogGreen", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		COLORCONTROL_ID,
			LINKID_LegacyID,			FuID("..RefractionFogGreen"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("RefractionFog.G"),
			IC_ControlGroup,			2,
			IC_ControlID,				1,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				0,
			INP_SubType,				IST_ColorG,
			TAG_DONE);

		InReflFogB = AddInput("Refraction Fog Blue", "FogBlue", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		COLORCONTROL_ID,
			I3D_AutoFlags,				PF_AutoProcess,
			LINKID_LegacyID,			FuID("..RefractionFogBlue"),
			I3D_ParamName,				FuID("RefractionFog.B"),
			IC_ControlGroup,			2,
			IC_ControlID,				2,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				0,
			INP_SubType,				IST_ColorB,
			TAG_DONE);
	EndGroup();
	
 EndGroup();

}
void MtlCookInputs3D::AddSSSCookInputs()
{
	InSSSNest = BeginGroup("SubSurfaceScattering", "SSS");

	InSSSOn = AddInput("SSS On", "SSS_on", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		CHECKBOXCONTROL_ID,
			LINKID_LegacyID,			FuID(".SSS_on"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("SSS_on"),
			INP_Default,				0.0,
			TAG_DONE);

	  InSSSColorNest = BeginGroup("SSS Color", "SSSColor", true, false);

		InSSSR = AddInput("SSS Red", "SSSRed",
			LINKID_DataType,			CLSID_DataType_Number, 
			INPID_InputControl,		COLORCONTROL_ID,
			LINKID_LegacyID,			FuID("..SSSRed"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("SSS.R"),
			IC_ControlGroup,			2,
			IC_ControlID,				0,
			ICS_Name,					"Refraction Color",
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				0,
			INP_SubType,				IST_ColorR,
			TAG_DONE);

		InSSSG = AddInput("SSS Green", "SSSGreen", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		COLORCONTROL_ID,
			LINKID_LegacyID,			FuID("..SSSGreen"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("SSS.G"),
			IC_ControlGroup,			2,
			IC_ControlID,				1,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				0,
			INP_SubType,				IST_ColorG,
			TAG_DONE);

		InSSSB = AddInput("SSS Blue", "SSSBlue", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		COLORCONTROL_ID,
			I3D_AutoFlags,				PF_AutoProcess,
			LINKID_LegacyID,			FuID("..SSSBlue"),
			I3D_ParamName,				FuID("SSS.B"),
			IC_ControlGroup,			2,
			IC_ControlID,				2,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				0,
			INP_SubType,				IST_ColorB,
			TAG_DONE);
	EndGroup();

	InSSSStrength = AddInput("SSS strength", "SSSstrength", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".SSSstrength"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("SSSstrength"),
		INP_MinAllowed,			0.0,
		INP_MinScale,				0.0,
		INP_MaxScale,				1.0,
		INP_Default,				0.0,
		TAG_DONE);

	InSSSIOR = AddInput("SSS IOR", "SSS_IOR",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		SLIDERCONTROL_ID,
				LINKID_LegacyID,			FuID("..SSSIOR"),
				I3D_AutoFlags,				PF_AutoProcess,
				I3D_ParamName,				FuID("SSSIOR"),
				INP_MinScale,				0.0,
				INP_MaxScale,				5.0,
				INP_Default,				1.3,
				TAG_DONE);

	InSSSScale = AddInput("SSS Scale", "SSSScale",
			LINKID_DataType,		CLSID_DataType_Number,
			INPID_InputControl,	SLIDERCONTROL_ID,
			INP_MinAllowed,		0.0,
			INP_MinScale,			0.0,
			INP_MaxScale,			5.0,
			INP_Default,			1.0,
			TAG_DONE);
 EndGroup();

}

void MtlCookInputs3D::AddBumpmapCookInputs()
{
	InBumpNest = BeginGroup("Displacement", "Displacement");

	InBumpmapMtl = AddInput("Bumpmap Material", "Bumpmap.Material",
		LINKID_DataType,			CLSID_DataType_Image,//CLSID_DataType_MtlGraph3D,
		//LINKID_AllowedDataType,	CLSID_DataType_Image,	// no! - the bumpmap isn't trivial, it has to transform normals to 'texture' basis
		LINKID_LegacyID,			FuID("BumpmapTex"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("BumpmapMtl"),
		I3D_ChildSlot,				MtlCookData3D::CMS_Bumpmap,
		LINK_Main,					MMID_Bumpmap,
		INP_Required,				FALSE, 
		LINKID_OverideDefLink,	LinkType_Eye_Normal,
		IC_Visible,					TRUE,
		TAG_DONE);

	InBumpStrength = AddInput("Displacement Strength", "bump_mult", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".bump_mult"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("bump_mult"),
		INP_MinAllowed,			0.0,
		INP_MinScale,				0.0,
		INP_MaxScale,				1.0,
		INP_Default,				1.0,
		TAG_DONE);

	InBumpBound = AddInput("Displace Bound", "BumpBound", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".BumpBound"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("BumpBound"),
		INP_MinAllowed,			0.0,
		INP_MinScale,				0.0,
		INP_MaxScale,				2.0,
		INP_Default,				0.5,
		TAG_DONE);

	EndGroup();
}


void MtlCookInputs3D::AddGICookInputs()
{
	InGINest = BeginGroup("ColorBleeding", "ColorBleeding");

			InDoGI = AddInput("Recievice ColorBleeding/GI/FG", "DoGI", 
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		CHECKBOXCONTROL_ID,
				LINKID_LegacyID,			FuID(".DoGI"),
				I3D_AutoFlags,				PF_AutoProcess,
				I3D_ParamName,				FuID("DoGI"),
				INP_Default,				0.0,
				TAG_DONE);

			InGIStrength = AddInput("Strength", "GIStrength",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		SLIDERCONTROL_ID,
				LINKID_LegacyID,			FuID("..GIStrength"),
				I3D_AutoFlags,				PF_AutoProcess,
				I3D_ParamName,				FuID("GIStrength"),
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_Default,				1.0,
				TAG_DONE);

	EndGroup();
}

void MtlCookInputs3D::AddTransmittanceCookInputs()
{
	InTransmittanceNest = BeginGroup("Transmittance", "Transmittance");

		InTransmittanceColorNest = BeginGroup("Color", "Color", true, false);

			InTransmittanceR = AddInput("Red", "Red",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..TransmittanceRed"),
				I3D_AutoFlags,				PF_AutoProcess,
				I3D_ParamName,				FuID("Transmittance.R"),
				IC_ControlGroup,			3,
				IC_ControlID,				0,
				ICS_Name,					"Attenuation",
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_MaxAllowed,			1.0,
				INP_Default,				0.0,
				INP_SubType,				IST_ColorR,
				IC_Visible,					FALSE,
				TAG_DONE);

			InTransmittanceG = AddInput("Green", "Green",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..TransmittanceGreen"),
				I3D_AutoFlags,				PF_AutoProcess,
				I3D_ParamName,				FuID("Transmittance.G"),
				IC_ControlGroup,			3,
				IC_ControlID,				1,
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_MaxAllowed,			1.0,
				INP_Default,				0.0,
				INP_SubType,				IST_ColorG,
				IC_Visible,					FALSE,
				TAG_DONE);

			InTransmittanceB = AddInput("Blue", "Blue",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..TransmittanceBlue"),
				I3D_AutoFlags,				PF_AutoProcess,
				I3D_ParamName,				FuID("Transmittance.B"),
				IC_ControlGroup,			3,
				IC_ControlID,				2,
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_MaxAllowed,			1.0,
				INP_Default,				0.0,
				INP_SubType,				IST_ColorB,
				IC_Visible,					FALSE,
				TAG_DONE);

			//InTransmittanceMtl.AddInput("Transmittance Material", "Material",
			//	LINKID_DataType,			CLSID_DataType_MtlGraph3D,
			//	LINKID_AllowedDataType,	CLSID_DataType_Image,
			// I3D_ParamName,				FuID("TransmittanceMtl"),
			// I3D_ShadeParam,			FALSE,
			//	LINK_Main,					5,
			//	INP_Required,				FALSE, 
			//	OBJP_Owner,					Owner,
			//	TAG_DONE);

		EndGroup();

		InTransmittanceAlphaDetail = AddInput("Alpha Detail", "AlphaDetail",
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		SLIDERCONTROL_ID,
			LINKID_LegacyID,			FuID(".TransmittanceAlphaDetail"),
			I3D_AutoFlags,				PF_AutoProcess,
			INP_MinAllowed,			0.0,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_MaxAllowed,			1.0,
			INP_Default,				1.0,
			IC_Visible,					FALSE,
			TAG_DONE);

		InTransmittanceColorDetail = AddInput("Color Detail", "ColorDetail",
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		SLIDERCONTROL_ID,
			LINKID_LegacyID,			FuID(".TransmittanceColorDetail"),
			I3D_AutoFlags,				PF_AutoProcess,
			INP_MinAllowed,			0.0,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_MaxAllowed,			1.0,
			INP_Default,				0.0,
			IC_Visible,					FALSE,
			TAG_DONE);

		InTransmittanceSaturation = AddInput("Saturation", "Saturation",
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		SLIDERCONTROL_ID,
			LINKID_LegacyID,			FuID(".TransmittanceSaturation"),
			I3D_AutoFlags,				PF_AutoProcess,
			INP_MinAllowed,			0.0,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				1.0,
			IC_Visible,					FALSE,
			TAG_DONE);

	EndGroup();
}

void MtlCookInputs3D::AddLightingShadowsCookInputs()
{
	InReceivesLighting = AddInput("Receives Lighting", "ReceivesLighting", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		CHECKBOXCONTROL_ID,
		I3D_AutoFlags,				PF_AutoProcess,
		INP_Default,				1.0,
		ICD_Width,					0.5,
		TAG_DONE);
		
	InReceivesShadows = AddInput("Receives Shadows", "ReceivesShadows", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		CHECKBOXCONTROL_ID,
		I3D_AutoFlags,				PF_AutoProcess,
		INP_Default,				1.0,
		ICD_Width,					0.5,
		TAG_DONE);

	InUseTwoSidedLighting = AddInput("Two Sided Lighting", "UseTwoSidedLighting", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		CHECKBOXCONTROL_ID,
		I3D_AutoFlags,				PF_AutoProcess,
		INP_Default,				0.0,
		ICD_Width,					0.5,
		TAG_DONE);
}

void MtlCookInputs3D::NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags)
{
	if (param && in)		// check in also as some of the inputs may be NULL for subclasses
	{
	}

	BaseClass::NotifyChanged(in, param, time, tags);
}

MtlCookData3D *MtlCookInputs3D::ProcessCookInputs(Request *req, MtlCookData3D *data)
{
	if (!data)
		return NULL;

	// diffuse
	Color4f diffuse;
	data->Diffuse.R = *InDiffuseR->GetValue(req);
	data->Diffuse.G = *InDiffuseG->GetValue(req);
	data->Diffuse.B = *InDiffuseB->GetValue(req);
	data->Diffuse.A = *InAlpha->GetValue(req);
	data->Opacity = *InOpacity->GetValue(req);
	data->roughness = *InRough->GetValue(req);
	data->SetChildMtl(MtlCookData3D::CMS_Diffuse, InDiffuseMtl->GetValue(req));
	
	Image *img = (Image *) InDiffuseMtl->GetValue(req);
	if (img){
		data->DiffuseImg = img;
	}
	else data->DiffuseImg = NULL;

	data->Incan.R = *InIncaR->GetValue(req);
	data->Incan.G = *InIncaG->GetValue(req);
	data->Incan.B = *InIncaB->GetValue(req);
	data->Incan.A = 1.0f;

	//coating
	data->CoatingColor.R = *InCoatingR->GetValue(req);
	data->CoatingColor.G = *InCoatingG->GetValue(req);
	data->CoatingColor.B = *InCoatingB->GetValue(req);
	data->CoatingColor.A = 1.0f;
	data->CoatingOn = *InCoatingOn->GetValue(req);
	data->CoatIOR = *InCoatIOR->GetValue(req);
	data->CoatSamples = *InCoatSamples->GetValue(req);
	data->CoatTrans.R = *InCoatTransR->GetValue(req);
	data->CoatTrans.G = *InCoatTransG->GetValue(req);
	data->CoatTrans.B = *InCoatTransB->GetValue(req);
	data->CoatTrans.A = 1.0f;
	data->CoatingThick = *InCoatingThick->GetValue(req);
	data->CoatingRough = *InCoatingRough->GetValue(req);
	data->CoatingEnv = *InCoatingEnv->GetValue(req);
	data->CoatingSpec = *InCoatingSpec->GetValue(req);
	data->CoatingRefl = *InCoatingRefl->GetValue(req);

	// specular color
	Color4f specular;
	data->Specular.R = *InSpecularR->GetValue(req);
	data->Specular.G = *InSpecularG->GetValue(req);
	data->Specular.B = *InSpecularB->GetValue(req);
	data->Specular.A = 1.0f;														// this value is ignored
	data->SetChildMtl(MtlCookData3D::CMS_Specular, InSpecularColorMtl->GetValue(req));
	data->SpecularEnv = *InSpecularEnv->GetValue(req);
	data->SpecularRefl = *InSpecularRefl->GetValue(req);
	data->SpecularSpec = *InSpecularSpec->GetValue(req);
	data->SpecSamples = *InSpecSamples->GetValue(req);
	data->ReflAniso.R = *InSpecularAnisoR->GetValue(req);
	data->ReflAniso.G = *InSpecularAnisoG->GetValue(req);
	data->ReflAniso.B = *InSpecularAnisoB->GetValue(req);
	data->ReflAniso.A = 1.0f;
	data->SpecularAniso = *InSpecularAniso->GetValue(req);
	
	Image *img_spec1 = (Image *) InSpecularColorMtl->GetValue(req);
	if (img_spec1){
		data->SpecColorImg = img_spec1;
	}
	else data->SpecColorImg = NULL;

	Image *img_spec2 = (Image *) InSpecularIntensityMtl->GetValue(req);
	if (img_spec2){
		data->SpecExpoImg = img_spec2;
	}
	else data->SpecExpoImg = NULL;

	Image *img_spec3 = (Image *) InSpecularExponentMtl->GetValue(req);
	if (img_spec3){
		data->SpecRoughImg = img_spec3;
	}
	else data->SpecRoughImg = NULL;

	// specular intensity
	data->SpecularIntensity = *InSpecularIntensity->GetValue(req);
	data->SetChildMtl(MtlCookData3D::CMS_SpecularIntensity, InSpecularIntensityMtl->GetValue(req));

	// specular exponent
	if (InSpecularExponent)
	{
		data->SpecularExponent = *InSpecularExponent->GetValue(req);
		data->SetChildMtl(MtlCookData3D::CMS_SpecularExponent, InSpecularExponentMtl->GetValue(req));
	}
	data->specIOR = *InSpecularIOR->GetValue(req);
	data->specRoughness = *InSpecularRoughness->GetValue(req);

	// reflection color
	Color4f reflection;
	data->Refl.R = *InReflR->GetValue(req);
	data->Refl.G = *InReflG->GetValue(req);
	data->Refl.B = *InReflB->GetValue(req);
	data->Refl.A = 1.0f;														// this value is ignored
	//data->SetChildMtl(MtlCookData3D::CMS_Specular, InSpecularColorMtl->GetValue(req));
	Image *img_refl1 = (Image *) InReflColorMtl->GetValue(req);
	if (img_refl1){
		data->ReflColorImg = img_refl1;
	}
	else data->ReflColorImg = NULL;

	Image *img_refl2 = (Image *) InReflRoughMtl->GetValue(req);
	if (img_refl2){
		data->ReflRoughImg = img_refl2;
	}
	else data->ReflRoughImg = NULL;

	data->ReflRoughness = *InReflRoughness->GetValue(req);

	data->FogStrength = *InFogStrength->GetValue(req);
	data->ReflFog.R = *InReflFogR->GetValue(req);
	data->ReflFog.G = *InReflFogG->GetValue(req);
	data->ReflFog.B = *InReflFogB->GetValue(req);
	data->ReflFog.A = 1.0f;
	data->RefraSamples = *InRefraSamples->GetValue(req);

	// refraction
	data->RefrIOR = *InRefrIOR->GetValue(req);

	///ColorBleeding
	data->DoGI = *InDoGI->GetValue(req) > 0.5;
	data->GIStrength = *InGIStrength->GetValue(req);

	// bumpmap
	data->SetChildMtl(MtlCookData3D::CMS_Bumpmap, InBumpmapMtl->GetValue(req));
	if (InBumpmapMtl->IsConnected()){
		Image *img_bump = (Image *) InBumpmapMtl->GetValue(req);
		if (img_bump){
			data->BumpImg = img_bump;
		}
		else data->BumpImg = NULL;
	}
	else data->BumpImg = NULL;

	data->BumpBound = *InBumpBound->GetValue(req);
	data->BumpStrength = *InBumpStrength->GetValue(req);

	data->SSS_color.R = *InSSSR->GetValue(req);
	data->SSS_color.G = *InSSSG->GetValue(req);
	data->SSS_color.B = *InSSSB->GetValue(req);
	data->SSS_color.A = 1.0f;
	data->SSS_on = *InSSSOn->GetValue(req);
	data->SSSIOR = *InSSSIOR->GetValue(req);
	data->SSSStrength = *InSSSStrength->GetValue(req);
	data->SSSScale = *InSSSScale->GetValue(req);


	// transmittance
	data->Transmittance.R = *InTransmittanceR->GetValue(req);
	data->Transmittance.G = *InTransmittanceG->GetValue(req);
	data->Transmittance.B = *InTransmittanceB->GetValue(req);

	data->Saturation = *InTransmittanceSaturation->GetValue(req);
	data->AlphaDetail = *InTransmittanceAlphaDetail->GetValue(req);
	data->ColorDetail = *InTransmittanceColorDetail->GetValue(req);

	// other
	data->ReceivesLighting = *InReceivesLighting->GetValue(req) > 0.5;
	data->ReceivesShadows = *InReceivesShadows->GetValue(req) > 0.5;
	data->UseTwoSidedLighting = *InUseTwoSidedLighting->GetValue(req) > 0.5;

	return data;
}

Data3D *MtlCookInputs3D::ProcessTagList(Request *req, Data3D *data, TagList &tags)
{
	// This is where we would in->GetValue(req) and stored the results in the MtlCookData3D object for any unmanaged inputs.

	if (!data)
		data = new MtlCookData3D(&MtlCookData3D::Reg, nil, TagList());

	if (data)
	{
		FuASSERT(data->IsTypeOf(CLSID_MtlCookData), (""));
		ProcessCookInputs(req, (MtlCookData3D *) data);
	}

	return BaseClass::ProcessTagList(req, data, tags);
}


//
//
// MtlCook3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlOperator3D 
#define ThisClass MtlCook3D 

FuRegisterClass(COMPANY_ID ".AdvMat", CT_Material3D)
	REGS_Name,					COMPANY_ID_SPACE "AdvMaterial",
	REGS_OpIconString,		"3AM",
	REGS_Category,				COMPANY_ID_DBS "3D\\Material",
	REGS_OpDescription,		"Advanced Material",
	REGS_HelpTopic,			"Tools/3D/Material/Phong",
	REGS_AboutDescription,	"Generate a Uber material",
	REGID_MaterialLongID,	LongID_MtlCook,
	REG_ToolbarDefault,		FALSE,
	REG_OpNoMask,				TRUE,
	REG_NoBlendCtrls,			TRUE,
	REG_NoObjMatCtrls,		TRUE,
	REG_NoMotionBlurCtrls,	TRUE,
	REG_TileID,					IDB_TILE_3D_BLINN,
	TAG_DONE);

MtlCook3D::MtlCook3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	if (m_RegNode == &MtlCook3D::Reg)	// other materials may derive this material so be careful not to set MtlInputs twice
	{
		MtlInputs = (MtlCookInputs3D *) MtlCookInputs3D::Reg.New(table, tags);
		RegisterInputs(MtlInputs);
		//MtlInputs->AddInputs();  -- no, Blinn has special handling to add inputs in OnAddToFlow()
	}
}

MtlCook3D::~MtlCook3D()
{
	SAFE_RECYCLE(MtlInputs);
}

bool MtlCook3D::RegInit(Registry *regnode)
{
	return BaseClass::RegInit(regnode);
}

void MtlCook3D::RegCleanup(Registry *regnode)
{
	BaseClass::RegCleanup(regnode);
}

bool MtlCook3D::SetAttrsTagList(const TagList &tags)
{
	return BaseClass::SetAttrsTagList(tags);
}

bool MtlCook3D::CopyAttr(Tag tag, void *addr) const
{
	return BaseClass::CopyAttr(tag, addr);
}

void MtlCook3D::Process(Request *req)
{
	BaseClass::Process(req);
}

void MtlCook3D::NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags)
{
	if (param && in)
	{
	}

	BaseClass::NotifyChanged(in, param, time, tags);
}

void MtlCook3D::OnAddToFlow(TagList *tags)
{
	// add the inputs - we do this here in OnAddToFlow because we can't do it in the ctor since AddInputs() calls virtual functions
	if (MtlInputs)
	{
		MtlInputs->BeginAddInputs();
		MtlInputs->AddInputs();
		MtlInputs->EndAddInputs();
	}

	BaseClass::OnAddToFlow(tags);
}

void MtlCook3D::OnRemoveFromFlow(TagList *tags)
{
	BaseClass::OnRemoveFromFlow(tags);
}





