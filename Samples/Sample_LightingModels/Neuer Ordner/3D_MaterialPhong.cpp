//@ Project "Fu3D"

#include "stdafx.h"

#define MODULENAME "3DMaterialPhong"

#define BaseClass MtlBlinnData3D
#define ThisClass MtlPhongData3D 

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
#include "3D_ShadeContext.h"
#include "3D_TextureImage.h"
#include "3D_MaterialPhong.h"
#include "3D_LightBaseImplSW.h"
#include "3D_RenderContextGL.h"

static const FuID LongID_MtlPhong = COMPANY_ID "MtlCookTorrance";		



//
//
// MtlPhongData3D class
//
//
FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlPhongData, CT_MtlData3D)
	REGS_Name,					COMPANY_ID "MtlPhongData",
	REGID_MaterialLongID,	LongID_MtlPhong,
	TAG_DONE);

MtlPhongData3D::MtlPhongData3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

MtlPhongData3D::MtlPhongData3D(const MtlPhongData3D &toCopy) : BaseClass(toCopy)
{
}

MtlPhongData3D::~MtlPhongData3D()
{
}

const Registry *MtlPhongData3D::GetImplRegistry(uint32 type)
{
	const Registry *ret = NULL;

	if (m_RegNode == &MtlPhongData3D::Reg)		// so subclasses aren't forced to also override this func
	{
		switch (type)
		{
			case CT_MtlImplSW3D:
				ret = &MtlPhongSW3D::Reg;
				break;
			case CT_MtlImplGL3D:
				ret = &MtlPhongGL3D::Reg;
				break;
		}
	}
	else
		ret = MtlData3D::GetImplRegistry(type);

	return ret;
}




//
//
// MtlPhongSW3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlBlinnSW3D 
#define ThisClass MtlPhongSW3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlPhongSW, CT_MtlImplSW3D)
	REGS_Name,					COMPANY_ID "MtlPhongSW",
	REGID_MaterialLongID,	LongID_MtlPhong,
	TAG_DONE);

MtlPhongSW3D::MtlPhongSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	ShadeFunc = (ShadeFunc3D) &MtlPhongSW3D::ShadeFragment;
}

MtlPhongSW3D::~MtlPhongSW3D()
{
}

void MtlPhongSW3D::ShadeFragment(ShadeContext3D &sc)
{
	BlinnBlock &block = (BlinnBlock &) sc.GetMaterialBlock(this);

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

		// lighting
		Color4f ambientAccum = ZERO4F;
		Color4f diffuseAccum = ZERO4F;
		Color4f specularAccum = ZERO4F;

		//uassert(sc.X != 700 || sc.Y != 700);

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

					const Vector4f &l = (const Vector4f &) sc.LightVector;						// points from the point being shaded to the light
					float32 nDotL = Dot3(n, l);

					if (sc.DoDiffuse)
					{
						float32 diffuse = maxf(nDotL, 0.0f);
						diffuseAccum += diffuse * Mul(Kd, sc.LightColor);
					}

					if (sc.DoSpecular)
					{
						Vector4f r = Normalize3(2.0f * nDotL * n - l);		// reflection vec
						const Vector4f v = -p;										// points from the point being shaded to the camera

						float32 specular = powf(maxf(Dot3(r, v), 0.0f), specularExponent);
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

		FuASSERT(!out.IsNaN(), ("MtlBlinnSW3D::NaN output"));

		*block.Color = out;
	}
}






//
//
// MtlPhongGL3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlBlinnGL3D 
#define ThisClass MtlPhongGL3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlPhongGL, CT_MtlImplGL3D)
	REGS_Name,					COMPANY_ID "MtlPhongGL3D",
	REGID_MaterialLongID,	LongID_MtlPhong,
	TAG_DONE);


MtlPhongGL3D::MtlPhongGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

MtlPhongGL3D::~MtlPhongGL3D()
{
}

ShadeNodeCg *MtlPhongGL3D::CreateShadeNode(RenderContextGL3D &rc)
{
	return new BlinnCg;
}

const char *MtlPhongGL3D::GetShaderFilename(RenderContextGL3D &rc)
{
	return "Shaders:Materials/Cg/FuMtlPhong_f.cg";
}

const char *MtlPhongGL3D::GetShadeNodeName(RenderContextGL3D &rc)
{
	return "FuMtlPhong";
}






//
//
// MtlPhongInputs3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlBlinnInputs3D 
#define ThisClass MtlPhongInputs3D 

FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlPhongInputs, CT_MtlInputs3D)
	REGS_Name,					COMPANY_ID_SPACE "Phong Material Inputs",
	REGID_MaterialLongID,	LongID_MtlPhong,
	TAG_DONE);

MtlPhongInputs3D::MtlPhongInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

MtlPhongInputs3D::~MtlPhongInputs3D()
{
}

bool MtlPhongInputs3D::AddInputsTagList(TagList &tags)
{
	AddControlPage("Controls");
	AddAllBlinnInputs();
	
	return MtlInputs3D::AddInputsTagList(tags);		// note we're not calling MtlBlinn3D::AddInputsTagList()
}

Data3D *MtlPhongInputs3D::ProcessTagList(Request *req, Data3D *data, TagList &tags)
{
	return BaseClass::ProcessTagList(req, data, tags);
}








//
//
// MtlPhong3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlBlinn3D 
#define ThisClass MtlPhong3D 

FuRegisterClass(COMPANY_ID_DOT + CLSID_MtlPhong, CT_Material3D)
	REGS_Name,					COMPANY_ID_SPACE "CookTorrance",
	REGS_OpIconString,		"3CT",
	REGS_Category,				COMPANY_ID_DBS "3D\\Material",
	REGS_OpDescription,		"CookTorrance Material",
	REGS_HelpTopic,			"Tools/3D/Material/Phong",
	REGS_AboutDescription,	"Generate a CookTorrance material",
	REGID_MaterialLongID,	LongID_MtlPhong,
	REG_ToolbarDefault,		FALSE,
	REG_OpNoMask,				TRUE,
	REG_NoBlendCtrls,			TRUE,
	REG_NoObjMatCtrls,		TRUE,
	REG_NoMotionBlurCtrls,	TRUE,
	REG_TileID,					IDB_TILE_3D_PHONG,
	TAG_DONE);

MtlPhong3D::MtlPhong3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	if (m_RegNode == &MtlPhong3D::Reg)	// other materials may derive this material so be careful not to set MtlInputs twice
	{
		MtlInputs = (MtlPhongInputs3D *) MtlPhongInputs3D::Reg.New(table, tags);
		RegisterInputs(MtlInputs);
		//MtlInputs->AddInputs();  -- no, Blinn has special handling to add inputs in OnAddToFlow()
	}
}

MtlPhong3D::~MtlPhong3D()
{
}



