//@ Project "Fu3D"

#include "stdafx.h"

#define MODULENAME "3DLightEnv"

#define BaseClass LightInputs3D
#define ThisClass LightEnvInputs3D

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
#include "3D_LightEnv.h"

static const FuID LongID_LightEnv = COMPANY_ID "LightEnv";			// All of the classes associated with LightDirectional will add this REGID_LightLongID tag to their registry lists.

//const FuID CLSID_Light_Env_Operator;
//const FuID CLSID_Light_Env_Inputs;
//const FuID CLSID_Light_Env_Data;


//
//
// LightEnvInputs3D
//
//
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Env_Inputs, CT_LightInputs)
	REGS_Name,					COMPANY_ID "Env Light Inputs",
	REGID_LightLongID,	LongID_LightEnv,
	TAG_DONE);

LightEnvInputs3D::LightEnvInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

LightEnvInputs3D::~LightEnvInputs3D()
{
}

bool LightEnvInputs3D::AddInputsTagList(TagList &tags)
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

		owner->InTx->SetAttrs(LTC_Style, (uint32)LCT_Env, TAG_DONE);
	}

	//AddStandardLightInputs();
	/*InMap = AddInput("env map", "env_map", 
         LINKID_DataType,		CLSID_DataType_Text,
         INPID_InputControl,	FILECONTROL_ID,
         INP_Required,			FALSE,
         INP_External,			FALSE,
         INP_DoNotifyChanged,	TRUE,
         FCS_FilterString,		"hdr (*.hdr)|*.hdr",		
         TAG_DONE);*/
	InMulti = AddInput("Multiplier", "env_multi",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			FALSE,
		INP_MinAllowed,			0.0,
		INP_MinScale,			0.0,
		INP_MaxScale,			16.0,
		INP_Default,			1.0,
		TAG_DONE);
	InEnvSamples = AddInput("Samples", "env_samples",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			TRUE,
		INP_MinAllowed,			0.0,
		INP_MinScale,			1.0,
		INP_MaxScale,			256.0,
		INP_Default,			64.0,
		TAG_DONE);
	InEnvOcc = AddInput("occlusion strength", "env_occ",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			TRUE,
		INP_MinAllowed,			0.0,
		INP_MinScale,			1.0,
		INP_MaxScale,			10.0,
		INP_Default,			1.0,
		TAG_DONE);

	//InDiffuseColorNest = BeginGroup("Color", "Color", true, false);

	//		InDiffuseR = AddInput("Red", "Red",
	//			LINKID_DataType,			CLSID_DataType_Number,
	//			INPID_InputControl,		COLORCONTROL_ID,
	//			LINKID_LegacyID,			FuID("..DiffuseRed"),
	//			I3D_AutoFlags,				PF_AutoProcess,	
	//			I3D_ParamName,				FuID("Diffuse.R"),
	//			IC_ControlGroup,			1,
	//			IC_ControlID,				0,
	//			ICS_Name,					"Diffuse Color",
	//			INP_MinScale,				0.0,
	//			INP_MaxScale,				1.0,
	//			INP_Default,				1.0,
	//			INP_SubType,				IST_ColorR,
	//			TAG_DONE);

	//		InDiffuseG = AddInput("Green", "Green",
	//			LINKID_DataType,			CLSID_DataType_Number,
	//			INPID_InputControl,		COLORCONTROL_ID,
	//			LINKID_LegacyID,			FuID("..DiffuseGreen"),
	//			I3D_AutoFlags,				PF_AutoProcess,
	//			I3D_ParamName,				FuID("Diffuse.G"),
	//			IC_ControlGroup,			1,
	//			IC_ControlID,				1,
	//			INP_MinScale,				0.0,
	//			INP_MaxScale,				1.0,
	//			INP_Default,				1.0,
	//			INP_SubType,				IST_ColorG,
	//			TAG_DONE);

	//		InDiffuseB = AddInput("Blue", "Blue",
	//			LINKID_DataType,			CLSID_DataType_Number,
	//			INPID_InputControl,		COLORCONTROL_ID,
	//			LINKID_LegacyID,			FuID("..DiffuseBlue"),
	//			I3D_AutoFlags,				PF_AutoProcess,
	//			I3D_ParamName,				FuID("Diffuse.B"),
	//			IC_ControlGroup,			1,
	//			IC_ControlID,				2,
	//			INP_MinScale,				0.0,
	//			INP_MaxScale,				1.0,
	//			INP_Default,				1.0,
	//			INP_SubType,				IST_ColorB,
	//			TAG_DONE);
	//	EndGroup();
	return true;
}

Data3D *LightEnvInputs3D::ProcessTagList(Request *req, Data3D *data, TagList &tags)
{
	//Text *env_map = (Text *) InMap->GetValue(req);
	double env_samples = *InEnvSamples->GetValue(req);
	double env_multi = *InMulti->GetValue(req);
	//const char *envPathFile = "";
	int env_occ = 0;
	double env_in_occ = *InEnvOcc->GetValue(req);
	
	LightEnvData3D *dirData = (LightEnvData3D *) data;

	if (!dirData)
		dirData = new LightEnvData3D(&LightEnvData3D::Reg, nil, TagList());

	if (dirData)
		dirData->Direction = Vector3f(0.0f, 0.0f, -1.0f);
		dirData->env_samples = env_samples;
		dirData->env_multi = env_multi;
		//dirData->envPathFile = (const char *)*(env_map);
		dirData->env_occ = env_in_occ;
		//Color4f diffuse;
		//dirData->Diffuse.R = *InDiffuseR->GetValue(req);
		//dirData->Diffuse.G = *InDiffuseG->GetValue(req);
		//dirData->Diffuse.B = *InDiffuseB->GetValue(req);
		//dirData->Diffuse.A = 1;
		
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
#define ThisClass LightEnv3D

FuRegisterClass(COMPANY_ID ".EnvLight", CT_Light3D)
	REGS_Name,							COMPANY_ID_SPACE "Env Light",
	REGS_AboutDescription,			"Generate a Env Light",
	REGS_Category,						COMPANY_ID_DBS "3D\\Light",
	REG_TileID,							IDB_TILE_3D_LIGHTDIRECTIONAL,
	REGS_OpIconString,				"3EnvL",
	REGS_HelpTopic,					"Tools/3D/EnvLight",
	REG_OpNoMask,						TRUE,
//	REG_NoAutoProxy,					TRUE,
	REG_NoMotionBlurCtrls,			TRUE,		// perhaps this can be used in some other way
	REGID_LightLongID,				LongID_LightEnv,
	T3DREG_ScaleControls,			FALSE,
	TAG_DONE);


LightEnv3D::LightEnv3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	LightInputs = (LightEnvInputs3D *) LightEnvInputs3D::Reg.New(table, tags);

	if (LightInputs)
	{
		RegisterInputs(LightInputs);
		LightInputs->AddInputs();
	}
}

LightEnv3D::~LightEnv3D()
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
#define ThisClass LightEnvData3D
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Env_Data, CT_LightData3D)
	REGS_Name,					COMPANY_ID "Env Light Data",
	REGID_LightLongID,		LongID_LightEnv,
	TAG_DONE);

LightEnvData3D::LightEnvData3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

LightEnvData3D::LightEnvData3D(const LightEnvData3D &toCopy) : LightData3D(toCopy)
{
}

LightEnvData3D::~LightEnvData3D()
{
}

Data3D *LightEnvData3D::CopyTagList(TagList &tags)
{
	return new LightEnvData3D(*this);
}

const Registry *LightEnvData3D::GetImplRegistry(uint32 type)
{
	const Registry *ret = NULL;

	if (m_RegNode == &LightEnvData3D::Reg)		// so subclasses aren't forced to also override this func
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
	REGID_LightLongID,		LongID_LightEnv,
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

		LightEnvData3D *data = (LightEnvData3D *) LightData;

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
		LightEnvData3D *data = (LightEnvData3D *) LightData;

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
	REGS_Name,					COMPANY_ID "LightEnvSWImpl",
	REGID_LightLongID,		LongID_LightEnv,
	TAG_DONE);

LightDirectionalSW3D::LightDirectionalSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	LightEnvData3D *lightData = (LightEnvData3D *) tags.GetPtr(LGTP_LightData, NULL);
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


	