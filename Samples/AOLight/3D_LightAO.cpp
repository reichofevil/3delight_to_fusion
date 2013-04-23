//@ Project "Fu3D"

#include "stdafx.h"

#define MODULENAME "3DLightAO"

#define BaseClass LightInputs3D
#define ThisClass LightAOInputs3D

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
#include "3D_LightAO.h"

static const FuID LongID_LightAO = COMPANY_ID "LightAO";			// All of the classes associated with LightAO will add this REGID_LightLongID tag to their registry lists.

//const FuID CLSID_Light_AO_Operator;
//const FuID CLSID_Light_AO_Inputs;
//const FuID CLSID_Light_AO_Data;


//
//
// LightAOInputs3D
//
//
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_AO_Inputs, CT_LightInputs)
	REGS_Name,					COMPANY_ID "AO Light Inputs",
	REGID_LightLongID,	LongID_LightAO,
	TAG_DONE);

LightAOInputs3D::LightAOInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

LightAOInputs3D::~LightAOInputs3D()
{
}

bool LightAOInputs3D::AddInputsTagList(TagList &tags)
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
	InMulti = AddInput("max distance", "env_multi",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			TRUE,
		INP_MinAllowed,			1.0,
		INP_MinScale,			1.0,
		INP_MaxScale,			999999.0,
		INP_Default,			5.0,
		TAG_DONE);
	InEnvSamples = AddInput("Samples", "env_samples",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			TRUE,
		INP_MinAllowed,			0.0,
		INP_MinScale,			1.0,
		INP_MaxScale,			2048.0,
		INP_Default,			64.0,
		TAG_DONE);
	InEnvOcc = AddInput("occlusion strength", "env_occ",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			FALSE,
		INP_MinAllowed,			0.0,
		INP_MinScale,			1.0,
		INP_MaxScale,			10.0,
		INP_Default,			1.0,
		TAG_DONE);
	return true;
}

Data3D *LightAOInputs3D::ProcessTagList(Request *req, Data3D *data, TagList &tags)
{
	//Text *env_map = (Text *) InMap->GetValue(req);
	double env_samples = *InEnvSamples->GetValue(req);
	double env_multi = *InMulti->GetValue(req);
	//const char *envPathFile = "";
	int env_occ = 0;
	double env_in_occ = *InEnvOcc->GetValue(req);
	
	LightAOData3D *dirData = (LightAOData3D *) data;

	if (!dirData)
		dirData = new LightAOData3D(&LightAOData3D::Reg, nil, TagList());

	if (dirData)
		dirData->Direction = Vector3f(0.0f, 0.0f, -1.0f);
		dirData->env_samples = env_samples;
		dirData->env_multi = env_multi;
		//dirData->envPathFile = (const char *)*(env_map);
		dirData->env_occ = env_in_occ;
		
	return BaseClass::ProcessTagList(req, dirData, tags);
}






//
//
// LightAO3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass LightOperator3D
#define ThisClass LightAO3D

FuRegisterClass(COMPANY_ID ".AOLight", CT_Light3D)
	REGS_Name,							COMPANY_ID_SPACE "AO Light",
	REGS_AboutDescription,			"Generate a AO Light",
	REGS_Category,						COMPANY_ID_DBS "3D\\Light",
	REG_TileID,							IDB_TILE_3D_LIGHTDIRECTIONAL,
	REGS_OpIconString,				"3AOL",
	REGS_HelpTopic,					"Tools/3D/AOLight",
	REG_OpNoMask,						TRUE,
//	REG_NoAutoProxy,					TRUE,
	REG_NoMotionBlurCtrls,			TRUE,		// perhaps this can be used in some other way
	REGID_LightLongID,				LongID_LightAO,
	T3DREG_ScaleControls,			FALSE,
	TAG_DONE);


LightAO3D::LightAO3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	LightInputs = (LightAOInputs3D *) LightAOInputs3D::Reg.New(table, tags);

	if (LightInputs)
	{
		RegisterInputs(LightInputs);
		LightInputs->AddInputs();
	}
}

LightAO3D::~LightAO3D()
{
}







//
//
// LightAOData3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass LightData3D
#define ThisClass LightAOData3D
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_AO_Data, CT_LightData3D)
	REGS_Name,					COMPANY_ID "AO Light Data",
	REGID_LightLongID,		LongID_LightAO,
	TAG_DONE);

LightAOData3D::LightAOData3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

LightAOData3D::LightAOData3D(const LightAOData3D &toCopy) : LightData3D(toCopy)
{
}

LightAOData3D::~LightAOData3D()
{
}

Data3D *LightAOData3D::CopyTagList(TagList &tags)
{
	return new LightAOData3D(*this);
}

const Registry *LightAOData3D::GetImplRegistry(uint32 type)
{
	const Registry *ret = NULL;

	if (m_RegNode == &LightAOData3D::Reg)		// so subclasses aren't forced to also override this func
	{
		switch (type)
		{
			case CT_LightImplSW3D:
				ret = &LightAOSW3D::Reg;
				break;
			case CT_LightImplGL3D:
				ret = &LightAOGL3D::Reg;
				break;
		}
	}
	else
		ret = LightData3D::GetImplRegistry(type);

	return ret;
}










//
//
// LightAOGL3D class
//
//

#undef BaseClass
#undef ThisClass
#define BaseClass LightImplGL3D 
#define ThisClass LightAOGL3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Directional_Impl_GL, CT_LightImplGL3D)
	REGS_Name,					COMPANY_ID "LightDirectionalGLImpl",
	REGID_LightLongID,		LongID_LightAO,
	TAG_DONE);


LightAOGL3D::LightAOGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

LightAOGL3D::~LightAOGL3D()
{
}

bool LightAOGL3D::Activate(RenderContextGL3D &rc, TagList *tags)
{
	if (rc.ShadePath == SP_FixedFunction)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		rc.GetLightToEye().glLoadMatrix();

		// we need to set all the light parameters in case this light was already set to something else...
		GLenum light = GL_LIGHT0 + rc.LightNumberFf;

		LightAOData3D *data = (LightAOData3D *) LightData;

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
		LightAOData3D *data = (LightAOData3D *) LightData;

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

void LightAOGL3D::Deactivate(RenderContextGL3D &rc, TagList *tags)
{
	if (rc.ShadePath == SP_FixedFunction)
		glDisable(GL_LIGHT0 + rc.LightNumberFf);
}

const char *LightAOGL3D::GetShaderFilename(RenderContextGL3D &rc, TagList *tags)
{
	return "Shaders:Lights/Cg/FuLightDirectional_f.cg";
}

ShadeNodeCg *LightAOGL3D::CreateShadeNode(RenderContextGL3D &rc, ShaderCg *shader, TagList *tags)
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
// LightAOSW3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass LightImplSW3D 
#define ThisClass LightAOSW3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Directional_Impl_SW, CT_LightImplSW3D)
	REGS_Name,					COMPANY_ID "LightAOSWImpl",
	REGID_LightLongID,		LongID_LightAO,
	TAG_DONE);

LightAOSW3D::LightAOSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	LightAOData3D *lightData = (LightAOData3D *) tags.GetPtr(LGTP_LightData, NULL);
	Color = lightData->Color;
	Direction = lightData->Direction;
}

LightAOSW3D::~LightAOSW3D()
{
}

bool LightAOSW3D::PreRender(RenderContextSW3D &rc, TagList *tags)
{
	Direction = -rc.GetLightToEyeIT().TransformVector(Direction);
	Direction.Normalize();

	return true;
}

ParamBlockSW *LightAOSW3D::CreateParamBlock(RenderContextSW3D &rc, TagList *tags)
{
	LightAOBlock *block = new LightAOBlock;
	block->Size = sizeof(LightAOBlock);
	return block;
}

bool LightAOSW3D::Illuminate(ShadeContext3D &sc, ParamBlockSW *lightBlock)
{
	sc.AmbientLightColor = Color4f(0.0f, 0.0f, 0.0f, 0.0f);
	sc.LightColor = Color;
	sc.LightVector = Direction;
	return true;
}


	