//@ Project "Fu3D"

#include "stdafx.h"

#define MODULENAME "3DLightDirectional"

#define BaseClass LightInputs3D
#define ThisClass LightDirectionalInputs3D

#define ALLOW_GL_COLOR_FUNCTIONS
#define ALLOW_LIGHT_MODEL_COLOR_CONTROL

#ifdef FuPLUGIN
#include "FuPlugin.h"
#endif

#include "Resource.h"
#include "RegistryLists.h"

#include "3D_Scene.h"
#include "3D_Light.h"										// for the constants
#include "3D_CameraBase.h"
#include "3D_ShadeContext.h"
#include "3D_RenderContextGL.h"
#include "3D_RenderContextSW.h"
#include "3D_LightDirectional.h"

const FuID LongID_Light_Directional = COMPANY_ID "LightDirectional";			// All of the classes associated with LightDirectional will add this REGID_LightLongID tag to their registry lists.



//
//
// LightDirectionalInputs3D
//
//
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Directional_Inputs, CT_LightInputs)
	REGS_Name,					COMPANY_ID "Directional Light Inputs",
	TAG_DONE);

LightDirectionalInputs3D::LightDirectionalInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

LightDirectionalInputs3D::~LightDirectionalInputs3D()
{
}

bool LightDirectionalInputs3D::AddInputsTagList(TagList &tags)
{
	if (Owner->IsTypeOf(Transform3DOperator::ClassID))
	{
		Transform3DOperator *owner = (Transform3DOperator *) Owner;

		owner->InTx->SetAttrs(INPID_PreviewControl, LIGHTCONTROL_ID, TAG_DONE);
		owner->InTy->SetAttrs(INPID_PreviewControl, LIGHTCONTROL_ID, TAG_DONE);
		owner->InTz->SetAttrs(INPID_PreviewControl, LIGHTCONTROL_ID, TAG_DONE);
		owner->InRx->SetAttrs(INPID_PreviewControl, LIGHTCONTROL_ID, TAG_DONE);
		owner->InRy->SetAttrs(INPID_PreviewControl, LIGHTCONTROL_ID, TAG_DONE);
		owner->InRz->SetAttrs(INPID_PreviewControl, LIGHTCONTROL_ID, TAG_DONE);
		owner->InSx->SetAttrs(INPID_PreviewControl, NOID, TAG_DONE);
		owner->InSy->SetAttrs(INPID_PreviewControl, NOID, TAG_DONE);
		owner->InSz->SetAttrs(INPID_PreviewControl, NOID, TAG_DONE);

		owner->InTx->SetAttrs(LTC_Style, (uint32)LCT_Directional, TAG_DONE);
	}

	AddStandardLightInputs();

	InBlur = AddInput("Blur", "Blur", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".Blur"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("Blur"),
		INP_MinAllowed,			0.0,
		INP_MinScale,				0.0,
		INP_MaxScale,				1.0,
		INP_Default,				0.1,
		IC_Visible,					TRUE,
		TAG_DONE);

	return true;
}

Data3D *LightDirectionalInputs3D::ProcessTagList(Request *req, Data3D *data, TagList &tags)
{
	LightDirectionalData3D *dirData = (LightDirectionalData3D *) data;

	if (!dirData)
		dirData = new LightDirectionalData3D(&LightDirectionalData3D::Reg, nil, TagList());

	if (dirData)
	{
		dirData->Direction = Vector3f(0.0f, 0.0f, -1.0f);
		dirData->blur = *InBlur->GetValue(req);
	}
		
	return BaseClass::ProcessTagList(req, dirData, tags);
}






//
//
// LightDirectional3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass LightOperator3D
#define ThisClass LightDirectional3D

FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Directional_Operator, CT_Light3D)
	REGS_Name,							COMPANY_ID_SPACE "Directional Light",
	REGS_AboutDescription,			"Generate a Directional Light",
	REGS_Category,						COMPANY_ID_DBS "3D\\Light",
	REG_TileID,							IDB_TILE_3D_LIGHTDIRECTIONAL,
	REGS_OpIconString,				"3DL",
	REGS_HelpTopic,					"Tools/3D/DirectionalLight",
	REG_OpNoMask,						TRUE,
//	REG_NoAutoProxy,					TRUE,
	REG_NoMotionBlurCtrls,			TRUE,		// perhaps this can be used in some other way
	REGID_LightLongID,				LongID_Light_Directional,
	T3DREG_ScaleControls,			FALSE,
	TAG_DONE);


LightDirectional3D::LightDirectional3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	LightInputs = (LightDirectionalInputs3D *) LightDirectionalInputs3D::Reg.New(table, tags);

	if (LightInputs)
	{
		RegisterInputs(LightInputs);
		LightInputs->AddInputs();
	}
}

LightDirectional3D::~LightDirectional3D()
{
}







//
//
// LightDirectionalData3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass LightData3D
#define ThisClass LightDirectionalData3D
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Directional_Data, CT_LightData3D)
	REGS_Name,					COMPANY_ID "Directional Light Data",
	REGID_LightLongID,		LongID_Light_Directional,
	TAG_DONE);

LightDirectionalData3D::LightDirectionalData3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

LightDirectionalData3D::LightDirectionalData3D(const LightDirectionalData3D &toCopy) : LightData3D(toCopy)
{
}

LightDirectionalData3D::~LightDirectionalData3D()
{
}

Data3D *LightDirectionalData3D::CopyTagList(TagList &tags)
{
	return new LightDirectionalData3D(*this);
}

const Registry *LightDirectionalData3D::GetImplRegistry(uint32 type)
{
	const Registry *ret = NULL;

	if (m_RegNode == &LightDirectionalData3D::Reg)		// so subclasses aren't forced to also override this func
	{
		switch (type)
		{
			case CT_LightImplSW3D:
				ret = &LightDirectionalSW3D::Reg;
				break;
			case CT_LightImplGL3D:
				ret = &LightDirectionalGL3D::Reg;
				break;
		}
	}
	else
		ret = LightData3D::GetImplRegistry(type);

	return ret;
}










//
//
// LightDirectionalGL3D class
//
//

#undef BaseClass
#undef ThisClass
#define BaseClass LightImplGL3D 
#define ThisClass LightDirectionalGL3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Directional_Impl_GL, CT_LightImplGL3D)
	REGS_Name,					COMPANY_ID "LightDirectionalGLImpl",
	REGID_LightLongID,		LongID_Light_Directional,
	TAG_DONE);


LightDirectionalGL3D::LightDirectionalGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

LightDirectionalGL3D::~LightDirectionalGL3D()
{
}

bool LightDirectionalGL3D::Activate(RenderContextGL3D &rc, TagList *tags)
{
	if (rc.ShadePath == SP_FixedFunction)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		rc.GetLightToEye().glLoadMatrix();

		// we need to set all the light parameters in case this light was already set to something else...
		GLenum light = GL_LIGHT0 + rc.LightNumberFf;

		LightDirectionalData3D *data = (LightDirectionalData3D *) LightData;

		const float defaultAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightfv(light, GL_AMBIENT, defaultAmbient);
		glLightfv(light, GL_DIFFUSE, data->Color.V);
		glLightfv(light, GL_SPECULAR, data->Color.V);

		//Matrix4f normMatrix = model.InverseTranspose();		// transform to world space, and then ogl transforms it by the modelview matrix to eyespace

		float32 dir[4] = { -data->Direction.X, -data->Direction.Y, -data->Direction.Z, 0.0f };
		glLightfv(light, GL_POSITION, dir);										// transformed by mv

		float direction[3] = { 0.0f, 1.0f, -1.0f };
		glLightf(light, GL_SPOT_CUTOFF, 180.0f);								// disable spotlights
		glLightfv(light, GL_SPOT_DIRECTION, direction);						// transformed by mv
		glLightf(light, GL_SPOT_EXPONENT, 0.0f);

		glLightf(light, GL_CONSTANT_ATTENUATION, 1.0f);
		glLightf(light, GL_LINEAR_ATTENUATION, 0.0f);
		glLightf(light, GL_QUADRATIC_ATTENUATION, 0.0f);

		glEnable(light);

		glPopMatrix();
	}
	else if (rc.ShadePath == SP_Cg)
	{
		LightDirectionalCg *sn = (LightDirectionalCg *) rc.CgLightShadeNode;
		LightDirectionalData3D *data = (LightDirectionalData3D *) LightData;

		// direction
		const Matrix4f &lightToEyeIT = rc.GetLightToEyeIT();
		Vector3f direction = -Normalize(lightToEyeIT.TransformVector(data->Direction));		// should this be in prerender?
		sn->InLightDir.Set(direction);

		// color
		Color4f lightColor = data->Color;
		lightColor.A = 1.0f;							// needed for projectors to work
		sn->InColor.Set(lightColor);
	}

	return true;
}

void LightDirectionalGL3D::Deactivate(RenderContextGL3D &rc, TagList *tags)
{
	if (rc.ShadePath == SP_FixedFunction)
		glDisable(GL_LIGHT0 + rc.LightNumberFf);
}

const char *LightDirectionalGL3D::GetShaderFilename(RenderContextGL3D &rc, TagList *tags)
{
	return "Shaders:Lights/Cg/FuLightDirectional_f.cg";
}

ShadeNodeCg *LightDirectionalGL3D::CreateShadeNode(RenderContextGL3D &rc, ShaderCg *shader, TagList *tags)
{
	LightDirectionalCg *sn = new LightDirectionalCg();

	if (sn)
	{
		ParamCg light = shader->CreateUserParameter("FuLightDirectional");

		sn->m_Struct		= light;
		sn->InLightDir		= light.GetSubParameter("Direction");
		sn->InColor			= light.GetSubParameter("Color");
	}

	return sn;
}










//
//
// LightDirectionalSW3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass LightImplSW3D 
#define ThisClass LightDirectionalSW3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Directional_Impl_SW, CT_LightImplSW3D)
	REGS_Name,					COMPANY_ID "LightDirectionalSWImpl",
	REGID_LightLongID,		LongID_Light_Directional,
	TAG_DONE);

LightDirectionalSW3D::LightDirectionalSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	LightDirectionalData3D *lightData = (LightDirectionalData3D *) tags.GetPtr(LGTP_LightData, NULL);
	Color = lightData->Color;
	Direction = lightData->Direction;
}

LightDirectionalSW3D::~LightDirectionalSW3D()
{
}

bool LightDirectionalSW3D::PreRender(RenderContextSW3D &rc, TagList *tags)
{
	Direction = -rc.GetLightToEyeIT().TransformVector(Direction);
	Direction.Normalize();

	return true;
}

ParamBlockSW *LightDirectionalSW3D::CreateParamBlock(RenderContextSW3D &rc, TagList *tags)
{
	LightDirectionalBlock *block = new LightDirectionalBlock;
	block->Size = sizeof(LightDirectionalBlock);
	return block;
}

bool LightDirectionalSW3D::Illuminate(ShadeContext3D &sc, ParamBlockSW *lightBlock)
{
	sc.AmbientLightColor = Color4f(0.0f, 0.0f, 0.0f, 0.0f);
	sc.LightColor = Color;
	sc.LightVector = Direction;
	return true;
}


	