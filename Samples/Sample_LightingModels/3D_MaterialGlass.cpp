//@ Project "Fu3D"

#include "stdafx.h"

#define MODULENAME "3DMaterialGlass"

#define BaseClass MtlData3D 
#define ThisClass MtlGlassData3D 

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
#include "3D_MaterialBlinn.h"
#include "3D_MaterialGlass.h"
#include "3D_LightBaseImplSW.h"
#include "3D_RenderContextGL.h"
#include "3D_RenderContextSW.h"
#include "3D_MaterialConstant.h"


static const FuID LongID_MtlGlass = COMPANY_ID "MtlGlass";			// All of the classes associated with MtlGlass will add this REGID_MaterialLongID tag to their registry lists.


//
//
// MtlGlassData3D class
//
//
FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlGlassData, CT_MtlData3D)
	REGS_Name,					COMPANY_ID "MtlGlassData",
	REGID_MaterialLongID,	LongID_MtlGlass,
	TAG_DONE);

MtlGlassData3D::MtlGlassData3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	// The most common case is that you will always add the same managed parameters in the same order, however, if that
	// is not the case you must tell Fusion that it cannot cache a input -> param lookup table.  This forces Fusion to do 
	// brute force string matching rather than an indexed lookup.
	// m_AccelInputToParam = false;
}

MtlGlassData3D::MtlGlassData3D(const MtlGlassData3D &toCopy) : BaseClass(toCopy)
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

MtlGlassData3D::~MtlGlassData3D()
{
}

Data3D *MtlGlassData3D::CopyTagList(TagList &tags)
{
	return new MtlGlassData3D(*this);
}

bool MtlGlassData3D::CreateManagedParams()
{
	ADD_MANAGED_PARAM(Diffuse);				
	ADD_MANAGED_PARAM(Specular);	
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

bool MtlGlassData3D::IsOpaque() 
{ 
	if (Opacity < 1.0f || Diffuse.A < 1.0f)
		return false; 

	MtlData3D *diffuseMtl = GetChildMtl(CMS_Diffuse);

	return diffuseMtl ? diffuseMtl->IsOpaque() : true;
}

const Registry *MtlGlassData3D::GetImplRegistry(uint32 type)
{
	const Registry *ret = NULL;

	if (m_RegNode == &MtlGlassData3D::Reg)		// so subclasses aren't forced to also override this func
	{
		switch (type)
		{
			case CT_MtlImplSW3D:
				ret = &MtlGlassSW3D::Reg;
				break;
			case CT_MtlImplGL3D:
				ret = &MtlGlassGL3D::Reg;
				break;
		}
	}
	else
		ret = MtlData3D::GetImplRegistry(type);

	return ret;
}

void MtlGlassData3D::Use()
{
	BaseClass::Use();
}

void MtlGlassData3D::Recycle()
{
	BaseClass::Recycle();
}






//
//
// MtlGlassGL3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlImplGL3D 
#define ThisClass MtlGlassGL3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlGlassGL, CT_MtlImplGL3D)
	REGS_Name,					COMPANY_ID "MtlGlassGL3D",
	REGID_MaterialLongID,	LongID_MtlGlass,
	TAG_DONE);

MtlGlassGL3D::MtlGlassGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

MtlGlassGL3D::~MtlGlassGL3D()
{
}

bool MtlGlassGL3D::CreateChildMtls(RenderContextGL3D &rc)
{
	if (rc.ShadePath == SP_FixedFunction)
	{
		MtlData3D *diffuseMtl = MtlData->GetChildMtl(MtlGlassData3D::CMS_Diffuse);
		if (diffuseMtl && diffuseMtl->IsTexture())
			DiffuseMtl = CreateChildMtl(rc, MtlGlassData3D::CMS_Diffuse);

		SpecularMtl				= NULL;
		SpecularIntensityMtl = NULL;
		SpecularExponentMtl	= NULL;
		BumpmapMtl				= NULL;
	}
	else
	{
		// OPT: change this based on if lighting is enabled?
		DiffuseMtl				= CreateChildMtl(rc, MtlGlassData3D::CMS_Diffuse);
		SpecularMtl				= CreateChildMtl(rc, MtlGlassData3D::CMS_Specular);
		SpecularIntensityMtl = CreateChildMtl(rc, MtlGlassData3D::CMS_SpecularIntensity);
		SpecularExponentMtl  = CreateChildMtl(rc, MtlGlassData3D::CMS_SpecularExponent);
		BumpmapMtl           = CreateChildMtl(rc, MtlGlassData3D::CMS_Bumpmap);
	}

	return true;
}

bool MtlGlassGL3D::ConnectParams(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct)
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

bool MtlGlassGL3D::ConnectChildMtls(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct)
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

ShadeNodeCg *MtlGlassGL3D::CreateShadeNode(RenderContextGL3D &rc)
{
	return new BlinnCg;
}

const char *MtlGlassGL3D::GetShadeNodeName(RenderContextGL3D &rc)
{
	return "FuMtlBlinn";
}

const char *MtlGlassGL3D::GetShaderFilename(RenderContextGL3D &rc)
{
	return "Shaders:Materials/Cg/FuMtlBlinn_f.cg";
}

bool MtlGlassGL3D::Activate(RenderContextGL3D &rc)
{
	if (rc.ShadePath == SP_FixedFunction)
	{
		MtlGlassData3D *mpd = (MtlGlassData3D *) MtlData;

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
		MtlGlassData3D *data = (MtlGlassData3D *) MtlData;

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
// MtlGlassSW3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlImplSW3D 
#define ThisClass MtlGlassSW3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlGlassSW, CT_MtlImplSW3D)
	REGS_Name,					COMPANY_ID "MtlGlassSW",
	REGID_MaterialLongID,	LongID_MtlGlass,
	TAG_DONE);

MtlGlassSW3D::MtlGlassSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	MtlGlassData3D *mtlData = (MtlGlassData3D *) tags.GetPtr(MTL_MaterialData, NULL);
	RenderContextSW3D &rc = *((RenderContextSW3D *) tags.GetPtr(MTL_RenderContext, NULL));

	DiffuseMtl = CreateChildMtl(rc, MtlGlassData3D::CMS_Diffuse);

	if (rc.DoLighting)
	{
		SpecularMtl = CreateChildMtl(rc, MtlGlassData3D::CMS_Specular);
		SpecularIntensityMtl = CreateChildMtl(rc, MtlGlassData3D::CMS_SpecularIntensity);
		SpecularExponentMtl = CreateChildMtl(rc, MtlGlassData3D::CMS_SpecularExponent);
		BumpmapMtl = CreateChildMtl(rc, MtlGlassData3D::CMS_Bumpmap);

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

	ShadeFunc = (ShadeFunc3D) &MtlGlassSW3D::ShadeFragment;
}

MtlGlassSW3D::~MtlGlassSW3D()
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

ParamBlockSW *MtlGlassSW3D::CreateParamBlock(RenderContextSW3D &rc)
{
	MtlGlassData3D *mtlData = (MtlGlassData3D *) MtlData;

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

void MtlGlassSW3D::ShadeFragment(ShadeContext3D &sc)
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

		FuASSERT(!outColor.IsNaN(), ("MtlGlassSW3D::NaN output"));

		*block.Color = outColor;
	}
}

bool MtlGlassSW3D::PreRender(RenderContextSW3D &rc)
{
	return true;
}

void MtlGlassSW3D::PostRender(RenderContextSW3D &rc)
{
}

bool MtlGlassSW3D::Activate(RenderContextSW3D &rc)
{
	return true;
}

void MtlGlassSW3D::Deactivate(RenderContextSW3D &rc)
{
}

void MtlGlassSW3D::Transmit(ShadeContext3D &sc)
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
// MtlGlassInputs3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlInputs3D 
#define ThisClass MtlGlassInputs3D 

FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlGlassInputs, CT_MtlInputs3D)
	REGS_Name,					COMPANY_ID "Glass Material Inputs",
	REGID_MaterialLongID,	LongID_MtlGlass,
	TAG_DONE);

MtlGlassInputs3D::MtlGlassInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
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

MtlGlassInputs3D::~MtlGlassInputs3D()
{
}

bool MtlGlassInputs3D::RegInit(Registry *regnode)
{
	return BaseClass::RegInit(regnode);
}

void MtlGlassInputs3D::RegCleanup(Registry *regnode)
{
	BaseClass::RegCleanup(regnode);
}

bool MtlGlassInputs3D::SetAttrsTagList(const TagList &tags)
{
	return BaseClass::SetAttrsTagList(tags);
}

bool MtlGlassInputs3D::CopyAttr(Tag tag, void *addr) const
{
	return BaseClass::CopyAttr(tag, addr);
}

void MtlGlassInputs3D::OnAddToFlow(TagList *tags)
{
	BaseClass::OnAddToFlow(tags);
}

void MtlGlassInputs3D::OnRemoveFromFlow(TagList *tags)
{
	BaseClass::OnRemoveFromFlow(tags);
}

bool MtlGlassInputs3D::AddInputsTagList(TagList &tags)
{
	AddControlPage("Controls");
	AddAllGlassInputs();
	return BaseClass::AddInputsTagList(tags);
}

void MtlGlassInputs3D::ShowInputs(bool visible)
{
	BaseClass::ShowInputs(visible);
}

void MtlGlassInputs3D::AddAllGlassInputs()
{
	AddDiffuseGlassInputs();

	BeginSpecularGlassNest();
		AddSpecularGlassInputs();
	EndSpecularGlassNest();

	AddBumpmapGlassInputs();

	AddTransmittanceGlassInputs();

	AddLightingShadowsGlassInputs();
}

void MtlGlassInputs3D::AddDiffuseGlassInputs()
{
	InDiffuseNest = BeginGroup("Diffuse", "Diffuse", true, true);

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
				TAG_DONE);

			InDiffuseMtl = AddInput("Diffuse Color Material", "Material",
				LINKID_DataType,			CLSID_DataType_MtlGraph3D,
				LINKID_AllowedDataType,	CLSID_DataType_Image,
				LINKID_LegacyID,			FuID("..DiffuseTex"),
				I3D_AutoFlags,				PF_AutoProcess,
				I3D_ParamName,				FuID("DiffuseMtl"),
				I3D_ChildSlot,				MtlGlassData3D::CMS_Diffuse,
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
		
	EndGroup();
}

void MtlGlassInputs3D::BeginSpecularGlassNest()
{
	InSpecularNest = BeginGroup("Specular", "Specular");
}

void MtlGlassInputs3D::AddSpecularGlassInputs(bool addSpecularExponent)
{
	InSpecularColorNest = BeginGroup("Color", "Color", true, false);

		InSpecularR = AddInput("Specular Red", "Red",
			LINKID_DataType,			CLSID_DataType_Number, 
			INPID_InputControl,		COLORCONTROL_ID,
			LINKID_LegacyID,			FuID("..SpecularRed"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("Specular.R"),
			IC_ControlGroup,			2,
			IC_ControlID,				0,
			ICS_Name,					"Specular Color",
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				DefaultSpecularColor.R,
			INP_SubType,				IST_ColorR,
			TAG_DONE);

		InSpecularG = AddInput("Green", "Green", 
			LINKID_DataType,			CLSID_DataType_Number,
			INPID_InputControl,		COLORCONTROL_ID,
			LINKID_LegacyID,			FuID("..SpecularGreen"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("Specular.G"),
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
			LINKID_LegacyID,			FuID("..SpecularBlue"),
			I3D_ParamName,				FuID("Specular.B"),
			IC_ControlGroup,			2,
			IC_ControlID,				2,
			INP_MinScale,				0.0,
			INP_MaxScale,				1.0,
			INP_Default,				DefaultSpecularColor.B,
			INP_SubType,				IST_ColorB,
			TAG_DONE);

		InSpecularColorMtl = AddInput("Specular Color Material", "Material",
			LINKID_DataType,			CLSID_DataType_MtlGraph3D,
			LINKID_AllowedDataType,	CLSID_DataType_Image,
			LINKID_LegacyID,			FuID("..SpecularColorTex"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("SpecularMtl"),
			I3D_ChildSlot,				MtlGlassData3D::CMS_Specular,
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
		TAG_DONE);

	InSpecularIntensityMtl = AddInput("Specular Intensity Material", "Intensity.Material",
		LINKID_DataType,			CLSID_DataType_MtlGraph3D,
		LINKID_AllowedDataType,	CLSID_DataType_Image,
		LINKID_LegacyID,			FuID(".SpecularIntensityTex"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("SpecularIntensityMtl"),
		I3D_ChildSlot,				MtlGlassData3D::CMS_SpecularIntensity,
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
			TAG_DONE);

		InSpecularExponentMtl = AddInput("Specular Exponent Material", "Exponent.Material",
			LINKID_DataType,			CLSID_DataType_MtlGraph3D,
			LINKID_AllowedDataType,	CLSID_DataType_Image,
			LINKID_LegacyID,			FuID(".SpecularExponentTex"),
			I3D_AutoFlags,				PF_AutoProcess,
			I3D_ParamName,				FuID("SpecularExponentMtl"),
			I3D_ChildSlot,				MtlGlassData3D::CMS_SpecularExponent,
			LINK_Main,					MMID_SpecularExponent,
			INP_Required,				FALSE, 
			TAG_DONE);
	}
}

void MtlGlassInputs3D::EndSpecularGlassNest()
{
	EndGroup();
}

void MtlGlassInputs3D::AddBumpmapGlassInputs()
{
	InBumpmapMtl = AddInput("Bumpmap Material", "Bumpmap.Material",
		LINKID_DataType,			CLSID_DataType_MtlGraph3D,
		//LINKID_AllowedDataType,	CLSID_DataType_Image,	// no! - the bumpmap isn't trivial, it has to transform normals to 'texture' basis
		LINKID_LegacyID,			FuID("BumpmapTex"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("BumpmapMtl"),
		I3D_ChildSlot,				MtlGlassData3D::CMS_Bumpmap,
		LINK_Main,					MMID_Bumpmap,
		INP_Required,				FALSE, 
		LINKID_OverideDefLink,	LinkType_Eye_Normal,
		TAG_DONE);
}

void MtlGlassInputs3D::AddTransmittanceGlassInputs()
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
			TAG_DONE);

	EndGroup();
}

void MtlGlassInputs3D::AddLightingShadowsGlassInputs()
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

void MtlGlassInputs3D::NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags)
{
	if (param && in)		// check in also as some of the inputs may be NULL for subclasses
	{
	}

	BaseClass::NotifyChanged(in, param, time, tags);
}

MtlGlassData3D *MtlGlassInputs3D::ProcessGlassInputs(Request *req, MtlGlassData3D *data)
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
	data->SetChildMtl(MtlGlassData3D::CMS_Diffuse, InDiffuseMtl->GetValue(req));

	// specular color
	Color4f specular;
	data->Specular.R = *InSpecularR->GetValue(req);
	data->Specular.G = *InSpecularG->GetValue(req);
	data->Specular.B = *InSpecularB->GetValue(req);
	data->Specular.A = 1.0f;														// this value is ignored
	data->SetChildMtl(MtlGlassData3D::CMS_Specular, InSpecularColorMtl->GetValue(req));

	// specular intensity
	data->SpecularIntensity = *InSpecularIntensity->GetValue(req);
	data->SetChildMtl(MtlGlassData3D::CMS_SpecularIntensity, InSpecularIntensityMtl->GetValue(req));

	// specular exponent
	if (InSpecularExponent)
	{
		data->SpecularExponent = *InSpecularExponent->GetValue(req);
		data->SetChildMtl(MtlGlassData3D::CMS_SpecularExponent, InSpecularExponentMtl->GetValue(req));
	}

	// bumpmap
	data->SetChildMtl(MtlGlassData3D::CMS_Bumpmap, InBumpmapMtl->GetValue(req));

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

Data3D *MtlGlassInputs3D::ProcessTagList(Request *req, Data3D *data, TagList &tags)
{
	// This is where we would in->GetValue(req) and stored the results in the MtlGlassData3D object for any unmanaged inputs.

	if (!data)
		data = new MtlGlassData3D(&MtlGlassData3D::Reg, nil, TagList());

	if (data)
	{
		FuASSERT(data->IsTypeOf(CLSID_MtlGlassData), (""));
		ProcessGlassInputs(req, (MtlGlassData3D *) data);
	}

	return BaseClass::ProcessTagList(req, data, tags);
}






//
//
// MtlGlass3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlOperator3D 
#define ThisClass MtlGlass3D 

FuRegisterClass(COMPANY_ID ".Glass", CT_Material3D)
	REGS_Name,					COMPANY_ID_SPACE "Glass",
	REGS_OpIconString,		"3Gl",
	REGS_Category,				COMPANY_ID_DBS "3D\\Material",
	REGS_OpDescription,		"Glass Material",
	REGS_HelpTopic,			"Tools/3D/Material/Glass",
	REGS_AboutDescription,	"Generate a Glass material",
	REGID_MaterialLongID,	LongID_MtlGlass,
	REG_ToolbarDefault,		FALSE,
	REG_OpNoMask,				TRUE,
	REG_NoBlendCtrls,			TRUE,
	REG_NoObjMatCtrls,		TRUE,
	REG_NoMotionBlurCtrls,	TRUE,
	REG_TileID,					IDB_TILE_3D_BLINN,
	TAG_DONE);

MtlGlass3D::MtlGlass3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	if (m_RegNode == &MtlGlass3D::Reg)	// other materials may derive this material so be careful not to set MtlInputs twice
	{
		MtlInputs = (MtlGlassInputs3D *) MtlGlassInputs3D::Reg.New(table, tags);
		RegisterInputs(MtlInputs);
		//MtlInputs->AddInputs();  -- no, Glass has special handling to add inputs in OnAddToFlow()
	}
}

MtlGlass3D::~MtlGlass3D()
{
	SAFE_RECYCLE(MtlInputs);
}

bool MtlGlass3D::RegInit(Registry *regnode)
{
	return BaseClass::RegInit(regnode);
}

void MtlGlass3D::RegCleanup(Registry *regnode)
{
	BaseClass::RegCleanup(regnode);
}

bool MtlGlass3D::SetAttrsTagList(const TagList &tags)
{
	return BaseClass::SetAttrsTagList(tags);
}

bool MtlGlass3D::CopyAttr(Tag tag, void *addr) const
{
	return BaseClass::CopyAttr(tag, addr);
}

void MtlGlass3D::Process(Request *req)
{
	BaseClass::Process(req);
}

void MtlGlass3D::NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags)
{
	if (param && in)
	{
	}

	BaseClass::NotifyChanged(in, param, time, tags);
}

void MtlGlass3D::OnAddToFlow(TagList *tags)
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

void MtlGlass3D::OnRemoveFromFlow(TagList *tags)
{
	BaseClass::OnRemoveFromFlow(tags);
}


