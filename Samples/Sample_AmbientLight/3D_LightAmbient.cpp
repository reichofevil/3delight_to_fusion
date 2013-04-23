//@ Project "Fu3D"

#include "stdafx.h"

#define MODULENAME "3DLightAmbient"

#define BaseClass LightInputs3D 
#define ThisClass LightAmbientInputs3D 

#define ALLOW_GL_COLOR_FUNCTIONS
#define ALLOW_LIGHT_MODEL_COLOR_CONTROL

#ifdef FuPLUGIN
#include "FuPlugin.h"
#endif

#include <windows.h>
#include <GL/gl.h>

#include "Resource.h"
#include "RegistryLists.h"

#include "3D_Light.h"							// for defaults
#include "3D_LightAmbient.h"
#include "3D_ShadeContext.h"

static const FuID LongID_Light_Ambient	= COMPANY_ID "LightAmbient";				// All of the classes associated with LightAmbient will add this REGID_LightLongID tag to their registry lists.



FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Ambient_Inputs, CT_LightInputs)
	REGS_Name,					COMPANY_ID "Ambient Light Inputs",
	TAG_DONE);

LightAmbientInputs3D::LightAmbientInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	if (table.IsNil())
		SetIntensity(TIME_UNDEFINED, 0.2);			// default ambient intensity to a reasonable value
}

LightAmbientInputs3D::~LightAmbientInputs3D()
{
}

bool LightAmbientInputs3D::AddInputsTagList(TagList &tags)
{
	if (Owner->IsTypeOf(Transform3DOperator::ClassID))
	{
		Transform3DOperator *owner = (Transform3DOperator *) Owner;

		owner->InTx->SetAttrs(INPID_PreviewControl, LIGHTCONTROL_ID, TAG_DONE);
		owner->InTy->SetAttrs(INPID_PreviewControl, LIGHTCONTROL_ID, TAG_DONE);
		owner->InTz->SetAttrs(INPID_PreviewControl, LIGHTCONTROL_ID, TAG_DONE);
		owner->InRx->SetAttrs(INPID_PreviewControl, NOID, TAG_DONE);
		owner->InRy->SetAttrs(INPID_PreviewControl, NOID, TAG_DONE);
		owner->InRz->SetAttrs(INPID_PreviewControl, NOID, TAG_DONE);
		owner->InSx->SetAttrs(INPID_PreviewControl, NOID, TAG_DONE);
		owner->InSy->SetAttrs(INPID_PreviewControl, NOID, TAG_DONE);
		owner->InSz->SetAttrs(INPID_PreviewControl, NOID, TAG_DONE);

		owner->InTx->SetAttrs(LTC_Style, (uint32)LCT_Ambient, TAG_DONE);
	}

	AddStandardLightInputs();

	return true;
}

Data3D *LightAmbientInputs3D::ProcessTagList(Request *req, Data3D *data, TagList &tags)
{
	LightAmbientData3D *ambientData = (LightAmbientData3D *) data;

	if (!ambientData)
		ambientData = new LightAmbientData3D(&LightAmbientData3D::Reg, nil, TagList());
		
	return BaseClass::ProcessTagList(req, ambientData, tags);
}







//
//
// LightAmbient3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass LightOperator3D 
#define ThisClass LightAmbient3D 

FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Ambient_Operator, CT_Light3D)
	REGS_Name,							COMPANY_ID_SPACE "Ambient Light",
	REGS_AboutDescription,			"Generate a Ambient Light",
	REGS_Category,						COMPANY_ID_DBS "3D\\Light",
	REG_TileID,							IDB_TILE_3D_LIGHTAMBIENT,
	REGS_OpIconString,				"3AL",
	REGS_HelpTopic,					"Tools/3D/AmbientLight",
	REG_OpNoMask,						TRUE,
//	REG_NoAutoProxy,					TRUE,
	REG_NoMotionBlurCtrls,			TRUE,		// perhaps this can be used in some other way
	REGID_LightLongID,				LongID_Light_Ambient,
	T3DREG_RotationControls,		FALSE,
	T3DREG_RotationPivotControls,	FALSE,
	T3DREG_ScaleControls,			FALSE,
	T3DREG_TargetControls,			FALSE,
	TAG_DONE);

LightAmbient3D::LightAmbient3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	LightInputs = (LightAmbientInputs3D	*) LightAmbientInputs3D::Reg.New(table, tags);

	if (LightInputs)
	{
		RegisterInputs(LightInputs);
		LightInputs->AddInputs();
	}
}

LightAmbient3D::~LightAmbient3D()
{
}







//
//
// LightAmbientData3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass LightData3D 
#define ThisClass LightAmbientData3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Ambient_Data, CT_LightData3D)
	REGS_Name,					COMPANY_ID "Constant Light Data",
	REGID_LightLongID,		LongID_Light_Ambient,
	TAG_DONE);

LightAmbientData3D::LightAmbientData3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

LightAmbientData3D::LightAmbientData3D(const LightAmbientData3D &toCopy) : LightData3D(toCopy)
{
}

LightAmbientData3D::~LightAmbientData3D()
{
}

Data3D *LightAmbientData3D::CopyTagList(TagList &tags)
{
	return new LightAmbientData3D(*this);
}

bool LightAmbientData3D::GetAmbientContribution(Color4f &c)
{
	c = Color;
	return true;
}

const Registry *LightAmbientData3D::GetImplRegistry(uint32 type)
{
	const Registry *ret = NULL;

	if (m_RegNode == &LightAmbientData3D::Reg)		// so subclasses aren't forced to also override this func
	{
		switch (type)
		{
			case CT_LightImplSW3D:
				ret = &LightAmbientSW3D::Reg;
				break;
			case CT_LightImplGL3D:
				ret = &LightAmbientGL3D::Reg;
				break;
		}
	}
	else
		ret = LightData3D::GetImplRegistry(type);

	return ret;
}












//
//
// LightAmbientGL3D class
//
//

#undef BaseClass
#undef ThisClass
#define BaseClass LightImplGL3D 
#define ThisClass LightAmbientGL3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Ambient_Impl_GL, CT_LightImplGL3D)
	REGS_Name,					COMPANY_ID "LightAmbientGLImpl",
	REGID_LightLongID,		LongID_Light_Ambient,
	TAG_DONE);


LightAmbientGL3D::LightAmbientGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

LightAmbientGL3D::~LightAmbientGL3D()
{
}

bool LightAmbientGL3D::Activate(RenderContextGL3D &rc, TagList *tags)
{
	if (rc.ShadePath == SP_FixedFunction)
	{
		LightAmbientData3D *data = (LightAmbientData3D *) LightData;

		// we need to set all the light parameters in case this light was already set to something else...
		GLenum light = GL_LIGHT0 + rc.LightNumberFf;

		const float Black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightfv(light, GL_AMBIENT, data->Color.V);
		glLightfv(light, GL_DIFFUSE, Black);				// no diffuse
		glLightfv(light, GL_SPECULAR, Black);				// no specular

		glLightf(light, GL_SPOT_CUTOFF, 180.0f);			// disable spotlights

		glEnable(light);
	}
	else if (rc.ShadePath == SP_Cg)
	{
		LightAmbientCg *sn = (LightAmbientCg *) rc.CgLightShadeNode;
		LightAmbientData3D *data = (LightAmbientData3D *) LightData;

		Color4f lightColor = data->Color;
		lightColor.A = 1.0f;							// needed for projectors to work
		sn->InColor.Set(lightColor);
	}

	return true;
}

void LightAmbientGL3D::Deactivate(RenderContextGL3D &rc, TagList *tags)
{
	if (rc.ShadePath == SP_FixedFunction)
		glDisable(GL_LIGHT0 + rc.LightNumberFf);
}

const char *LightAmbientGL3D::GetShaderFilename(RenderContextGL3D &rc, TagList *tags)
{
	return "Shaders:Lights/Cg/FuLightAmbient_f.cg";
}

ShadeNodeCg *LightAmbientGL3D::CreateShadeNode(RenderContextGL3D &rc, ShaderCg *shader, TagList *tags)
{
	LightAmbientCg *sn = new LightAmbientCg;

	if (sn)
	{
		ParamCg light = shader->CreateUserParameter("FuLightAmbient");
		sn->m_Struct = light;
		sn->InColor	= light.GetSubParameter("Color");
	}

	return sn;
}








//
//
// LightAmbientSW3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass LightImplSW3D 
#define ThisClass LightAmbientSW3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Ambient_Impl_SW, CT_LightImplSW3D)
	REGS_Name,					COMPANY_ID "LightAmbientSWImpl",
	REGID_LightLongID,		LongID_Light_Ambient,
	TAG_DONE);

LightAmbientSW3D::LightAmbientSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	LightAmbientData3D *lightData = (LightAmbientData3D *) tags.GetPtr(LGTP_LightData, NULL);
	Color = lightData->Color;
}

LightAmbientSW3D::~LightAmbientSW3D()
{
}

ParamBlockSW *LightAmbientSW3D::CreateParamBlock(RenderContextSW3D &rc, TagList *tags)
{
	LightAmbientBlock *block = new LightAmbientBlock;
	block->Size = sizeof(LightAmbientBlock);
	return block;
}

bool LightAmbientSW3D::Illuminate(ShadeContext3D &sc, ParamBlockSW *lightBlock)
{
	sc.AmbientLightColor = Color;
	sc.LightColor = Color4f(0.0f, 0.0f, 0.0f, 0.0f);
	sc.LightVector = Vector3f(0.0f, 0.0f, -1.0f);		// was originally (0, 0, 0) but some code expects a normalized vector here and was creating #INFs

	return true;
}

