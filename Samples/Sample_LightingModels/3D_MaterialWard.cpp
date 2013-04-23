//@ Project "Fu3D"

#include "stdafx.h"

#define MODULENAME "3DMaterialWard"

#define BaseClass MtlBlinnData3D 
#define ThisClass MtlWardData3D 

#define ALLOW_GL_COLOR_FUNCTIONS
#define ALLOW_LIGHT_MODEL_COLOR_CONTROL

#ifdef FuPLUGIN
#include "FuPlugin.h"
#endif

#include "GLTexture.h"
#include "PipelineDoc.h"
#include "TextureManager.h"

#include "3D_Stream.h"
#include "3D_MtlGraph.h"
#include "3D_LightBase.h"
#include "3D_CameraBase.h"
#include "3D_MaterialBase.h"
#include "3D_ShadeContext.h"
#include "3D_TextureImage.h"
#include "3D_MaterialWard.h"
#include "3D_LightBaseImplSW.h"
#include "3D_RenderContextGL.h"
#include "3D_RenderContextSW.h"

const FuID LongID_MtlWard = COMPANY_ID_DOT "MtlWard";		




//
//
// MtlWardData3D class
//
//
FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlWardData, CT_MtlData3D)
	REGS_Name,					COMPANY_ID "MtlWardData",
	REGID_MaterialLongID,	LongID_MtlWard,
	TAG_DONE);

MtlWardData3D::MtlWardData3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	SpreadX = 0.1f;
	SpreadY = 0.1f;
}

MtlWardData3D::MtlWardData3D(const MtlWardData3D &toCopy) : BaseClass(toCopy)
{
	SpreadX = toCopy.SpreadX;
	SpreadY = toCopy.SpreadY;
}

MtlWardData3D::~MtlWardData3D()
{
}

const Registry *MtlWardData3D::GetImplRegistry(uint32 type)
{
	const Registry *ret = NULL;

	if (m_RegNode == &MtlWardData3D::Reg)		// so subclasses aren't forced to also override this func
	{
		switch (type)
		{
			case CT_MtlImplSW3D:
				ret = &MtlWardSW3D::Reg;
				break;
			case CT_MtlImplGL3D:
				ret = &MtlWardGL3D::Reg;
				break;
		}
	}
	else
		ret = MtlData3D::GetImplRegistry(type);

	return ret;
}




//
//
// MtlWardSW3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlBlinnSW3D 
#define ThisClass MtlWardSW3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlWardSW, CT_MtlImplSW3D)
	REGS_Name,					COMPANY_ID "MtlWardSW",
	REGID_MaterialLongID,	LongID_MtlWard,
	TAG_DONE);

MtlWardSW3D::MtlWardSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	MtlWardData3D *mtlData = (MtlWardData3D *) tags.GetPtr(MTL_MaterialData, NULL);
	RenderContextSW3D &rc = *((RenderContextSW3D *) tags.GetPtr(MTL_RenderContext, NULL));

	if (rc.DoLighting)
	{
		SpreadXMtl = CreateChildMtl(rc, MtlBlinnData3D::CMS_SpreadX);
		SpreadYMtl = CreateChildMtl(rc, MtlBlinnData3D::CMS_SpreadY);
	}
	else
	{
		SpreadXMtl = NULL;
		SpreadYMtl = NULL;
	}

	ShadeFunc = (ShadeFunc3D) &MtlWardSW3D::ShadeFragment;
}

MtlWardSW3D::~MtlWardSW3D()
{
	SpreadXMtl = NULL;
	SpreadYMtl = NULL;
}

ParamBlockSW *MtlWardSW3D::CreateParamBlock(RenderContextSW3D &rc)
{
	MtlWardData3D *mtlData = (MtlWardData3D *) MtlData;

	WardBlock *block = new WardBlock;
	block->Size = sizeof(WardBlock);

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

	param = block->CreateParam(block->TangentU, Stream_TangentU, CS_Eye);
	param->SetEnabled(rc.DoLighting);
	param->SetRequired(true);

	param = block->CreateParam(block->TangentV, Stream_TangentV, CS_Eye);
	param->SetEnabled(rc.DoLighting);
	param->SetRequired(true);

	param = block->CreateParam(block->Color, Stream_Color, CS_Unset);
	param->SetEnabled(true);
	param->SetRequired(false);

	return block;
}

void MtlWardSW3D::ShadeFragment(ShadeContext3D &sc)
{
	WardBlock &block = (WardBlock &) sc.GetMaterialBlock(this);

	// diffuse
	Color4f diffuseMtl;

	if (DiffuseMtl)
	{
		DiffuseMtl->Shade(sc);
		diffuseMtl = *block.Color;
	}
	else
		diffuseMtl = WHITE4F;

	const Vector4f &vertexColor = IsSet(block.VertexColor) ? *block.VertexColor : WHITE4F;

	Color4f Kd = Mul(vertexColor, Mul(Diffuse, diffuseMtl));

	if (!sc.DoLighting || (!ReceivesLighting && !ReceivesShadows))		// precompute?
		*block.Color = Kd;
	else
	{
		// normal
		Vector4f oldNormal;

		if (BumpmapMtl)
		{
			oldNormal = *block.EyeNormal;							// save normal
			BumpmapMtl->Shade(sc);
		}

		Vector4f n = Normalize3(*block.EyeNormal);

		if (BumpmapMtl)
			*block.EyeNormal = oldNormal;							// restore normal

		Vector4f p = Normalize3(*block.EyePosition);

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

		// ward aniso parameters
		Vector4f x = Normalize3(*block.TangentU);
		Vector4f y = Normalize3(*block.TangentV);

		MtlWardData3D *mtlData = (MtlWardData3D *) MtlData;

		float32 spreadX = mtlData->SpreadX;
		float32 spreadY = mtlData->SpreadY;

		if (SpreadXMtl)
		{
			SpreadXMtl->Shade(sc);
			spreadX *= block.Color->A;
		}

		if (SpreadYMtl)
		{
			SpreadYMtl->Shade(sc);
			spreadY *= block.Color->A;
		}

		float32 invSXSX = 2.0f / (spreadX * spreadX);		// OPT: could be precomputed if no spread texture
		float32 invSYSY = 2.0f / (spreadY * spreadY);
		float32 invSXSY = 1.0f / (4.0f * 3.141592654f * spreadX * spreadY);


		// lighting
		Color4f ambientAccum = ZERO4F;
		Color4f diffuseAccum = ZERO4F;
		Color4f specularAccum = ZERO4F;

		Vector4f v = -p;									// points from point being shaded to the camera

		float32 nDotV = max(Dot3(n, v), 0.001f);		// eg. prevent blowup on outline of a shaded sphere

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

					ambientAccum += sc.AmbientLightColor;

					//uassert(sc.X != 1360 || sc.Y != 530);

					const Vector4f &l = (const Vector4f &) sc.LightVector;	// points from point being shaded to the light
					float32 nDotL = max(Dot3(n, l), 0.0f);

					if (sc.DoDiffuse)
						diffuseAccum += nDotL * Mul(Kd, sc.LightColor);

					if (sc.DoSpecular)
					{
						Vector4f h = Normalize3(v + l);

						float nDotH = max(Dot3(n, h), 0.0f);
						float xDotH = Dot3(x, h);
						float yDotH = Dot3(y, h);
						
						float exponent = (invSXSX * xDotH * xDotH + invSYSY * yDotH * yDotH) / (1.0f + nDotH);
						float specular = invSXSY * sqrt(nDotL / nDotV) * exp(-2.0f * exponent);			// spec = lightColor * nDotL * BRDF

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

		Color4f out = Mul(ambientAccum, Kd) + diffuseAccum + Mul(specularAccum, Ks);
		out.A = Kd.A;

		*block.Color = out;
	}
}









//
//
// MtlWardGL3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlBlinnGL3D 
#define ThisClass MtlWardGL3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlWardGL, CT_MtlImplGL3D)
	REGS_Name,					COMPANY_ID "MtlWardGL3D",
	REGID_MaterialLongID,	LongID_MtlWard,
	TAG_DONE);


MtlWardGL3D::MtlWardGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

MtlWardGL3D::~MtlWardGL3D()
{
}

bool MtlWardGL3D::CreateChildMtls(RenderContextGL3D &rc)
{
	if (rc.ShadePath == SP_FixedFunction)
	{
		DiffuseMtl				= NULL;
		SpecularMtl				= NULL;
		SpecularIntensityMtl = NULL;
		SpecularExponentMtl	= NULL;
		BumpmapMtl				= NULL;
		SpreadXMtl				= NULL;
		SpreadYMtl				= NULL;
	}
	else
	{
		DiffuseMtl				= CreateChildMtl(rc, MtlBlinnData3D::CMS_Diffuse);
		SpecularMtl				= CreateChildMtl(rc, MtlBlinnData3D::CMS_Specular);
		SpecularIntensityMtl = CreateChildMtl(rc, MtlBlinnData3D::CMS_SpecularIntensity);
		SpecularExponentMtl  = CreateChildMtl(rc, MtlBlinnData3D::CMS_SpecularExponent);
		BumpmapMtl           = CreateChildMtl(rc, MtlBlinnData3D::CMS_Bumpmap);
		SpreadXMtl				= CreateChildMtl(rc, MtlBlinnData3D::CMS_SpreadX);
		SpreadYMtl				= CreateChildMtl(rc, MtlBlinnData3D::CMS_SpreadY);
	}

	return true;
}

bool MtlWardGL3D::ConnectParams(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct)
{
	WardCg *sn = (WardCg *) node;

	sn->Diffuse					= mtlStruct.GetSubParameter("Diffuse");
	sn->Specular				= mtlStruct.GetSubParameter("Specular");
	sn->SpecularIntensity	= mtlStruct.GetSubParameter("SpecularIntensity");
	sn->SpreadX					= mtlStruct.GetSubParameter("SpreadX");
	sn->SpreadY					= mtlStruct.GetSubParameter("SpreadY");

	ParamCg lightArray = shader->CreateLightArrayParameter(rc);
	if (!lightArray.IsValid())
		return false;
	lightArray.ConnectTo(mtlStruct.GetSubParameter("Lights"));

	return true;
}

bool MtlWardGL3D::ConnectChildMtls(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct)
{
	WardCg *sn = (WardCg *) node;
	MtlWardData3D *data = (MtlWardData3D *) MtlData;


	// diffuse texture
	sn->DiffuseMtl = mtlStruct.GetSubParameter("DiffuseMtl");
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
	{
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


	// spread X 
	sn->SpreadXMtl	= mtlStruct.GetSubParameter("SpreadXMtl");
	if (SpreadXMtl)
	{
		ShadeNodeCg *ssn = SpreadXMtl->DoCreateShadeNode(rc, shader);
		shader->ConnectFloat4ToFloat1(ssn->m_Struct, sn->SpreadXMtl);
		shader->AddShadeNode(ssn);
	}
	else
		shader->SetInputToConst(sn->SpreadXMtl, 1.0f);


	// spread Y
	sn->SpreadYMtl	= mtlStruct.GetSubParameter("SpreadYMtl");
	if (SpreadYMtl)
	{
		ShadeNodeCg *ssn = SpreadYMtl->DoCreateShadeNode(rc, shader);
		shader->ConnectFloat4ToFloat1(ssn->m_Struct, sn->SpreadYMtl);
		shader->AddShadeNode(ssn);
	}
	else
		shader->SetInputToConst(sn->SpreadYMtl, 1.0f);


	// bumpmap texture
	sn->BumpmapMtl	= mtlStruct.GetSubParameter("BumpmapMtl");
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

ShadeNodeCg *MtlWardGL3D::CreateShadeNode(RenderContextGL3D &rc)
{
	return new WardCg;
}

bool MtlWardGL3D::Activate(RenderContextGL3D &rc)
{
	bool ret;

	if (rc.ShadePath == SP_FixedFunction)
		ret = MtlBlinnGL3D::Activate(rc);
	else if (rc.ShadePath == SP_Cg)
	{
		CHECK_GL_ERRORS();

		WardCg *sn = (WardCg *) rc.CgShader->GetShadeNode(this);
		MtlWardData3D *data = (MtlWardData3D *) MtlData;

		Vector4f diffuse = data->Diffuse * data->Opacity;
		sn->Diffuse.Set(diffuse);

		Vector4f specular = data->Specular * data->Opacity;
		sn->Specular.Set(specular);

		sn->SpecularIntensity.Set(data->SpecularIntensity);

		sn->SpreadX.Set(data->SpreadX);
		sn->SpreadY.Set(data->SpreadY);

		ret = true;

		CHECK_GL_ERRORS();
	}

	return ret;
}

const char *MtlWardGL3D::GetShaderFilename(RenderContextGL3D &rc)
{
	return "Shaders:Materials/Cg/FuMtlWard_f.cg";
}

const char *MtlWardGL3D::GetShadeNodeName(RenderContextGL3D &rc)
{
	return "FuMtlWard";
}






//
//
// MtlWardInputs3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlBlinnInputs3D 
#define ThisClass MtlWardInputs3D 

FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlWardInputs, CT_MtlInputs3D)
	REGS_Name,					COMPANY_ID "Ward Material Inputs",
	REGID_MaterialLongID,	LongID_MtlWard,
	TAG_DONE);

MtlWardInputs3D::MtlWardInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	InSpreadX = NULL;
	InSpreadY = NULL;
}

MtlWardInputs3D::~MtlWardInputs3D()
{
}

Data3D *MtlWardInputs3D::ProcessTagList(Request *req, Data3D *data, TagList &tags)
{
	MtlWardData3D *wardData = (MtlWardData3D *) data;
	wardData->SpecularExponent = DefaultSpecularExponent;		// set some value or OpenGL Ff raises errors

	wardData->SpreadX = *InSpreadX->GetValue(req);
	wardData->SpreadY = *InSpreadY->GetValue(req);

	wardData->SetChildMtl(MtlBlinnData3D::CMS_SpreadX, InSpreadXMtl->GetValue(req));
	wardData->SetChildMtl(MtlBlinnData3D::CMS_SpreadY, InSpreadYMtl->GetValue(req));

	return BaseClass::ProcessTagList(req, data, tags);
}

bool MtlWardInputs3D::AddInputsTagList(TagList &tags)
{
	AddControlPage("Controls");

	AddDiffuseBlinnInputs();

	BeginSpecularBlinnNest();

	AddSpecularBlinnInputs(false);

	InSpreadX = AddInput("Spread U", "SpreadU",
		LINKID_DataType,			CLSID_DataType_Number, 
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".SpreadU"),
		INP_Default,				0.05,
		INP_MinAllowed,			0.0001,
		INP_MinScale,				0.01,
		INP_MaxScale,				0.2,
		TAG_DONE);

	InSpreadY = AddInput("Spread V", "SpreadV",
		LINKID_DataType,			CLSID_DataType_Number, 
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".SpreadV"),
		INP_Default,				0.16,
		INP_MinAllowed,			0.0001,
		INP_MinScale,				0.01,
		INP_MaxScale,				0.2,
		TAG_DONE);

	InSpreadXMtl = AddInput("Spread U Texture", "SpreadU.Material",
		LINKID_DataType,			CLSID_DataType_MtlGraph3D,
		LINKID_AllowedDataType,	CLSID_DataType_Image,
		LINKID_LegacyID,			FuID(".SpreadUTex"),
		LINK_Main,					MMID_SpreadU,
		INP_Required,				FALSE, 
		TAG_DONE);

	InSpreadYMtl = AddInput("Spread V Texture", "SpreadV.Material",
		LINKID_DataType,			CLSID_DataType_MtlGraph3D,
		LINKID_AllowedDataType,	CLSID_DataType_Image,
		LINKID_LegacyID,			FuID(".SpreadVTex"),
		LINK_Main,					MMID_SpreadV,
		INP_Required,				FALSE, 
		TAG_DONE);

	EndSpecularBlinnNest();

	AddBumpmapBlinnInputs();

	AddTransmittanceBlinnInputs();

	AddLightingShadowsBlinnInputs();

	return MtlInputs3D::AddInputsTagList(tags);		// note we're not calling MtlBlinn3D::AddInputsTagList()
}

void MtlWardInputs3D::NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags)
{
	if (param && in)
	{
	}

	BaseClass::NotifyChanged(in, param, time, tags);
}







//
//
// MtlWard3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlBlinn3D 
#define ThisClass MtlWard3D 

FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlWard, CT_Material3D)
	REGS_Name,					COMPANY_ID_SPACE "Ward",
	REGS_OpIconString,		"3Wd",
	REGS_Category,				COMPANY_ID_DBS "3D\\Material",
	REGS_OpDescription,		"Ward Material",
	REGS_HelpTopic,			"Tools/3D/Material/Ward",
	REGS_AboutDescription,	"Generates a Ward Anistotropic Material",
	REGID_MaterialLongID,	LongID_MtlWard,
	REG_ToolbarDefault,		FALSE,
	REG_OpNoMask,				TRUE,
	REG_NoBlendCtrls,			TRUE,
	REG_NoObjMatCtrls,		TRUE,
	REG_NoMotionBlurCtrls,	TRUE,
	REG_TileID,					IDB_TILE_3D_WARD,
	TAG_DONE);

MtlWard3D::MtlWard3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	if (m_RegNode == &MtlWard3D::Reg)	// other materials may derive this material so be careful not to set MtlInputs twice
	{
		MtlInputs = (MtlWardInputs3D *) MtlWardInputs3D::Reg.New(table, tags);
		RegisterInputs(MtlInputs);
		//MtlInputs->AddInputs();  -- no, Blinn has special handling to add inputs in OnAddToFlow()
	}
}

MtlWard3D::~MtlWard3D()
{
}




