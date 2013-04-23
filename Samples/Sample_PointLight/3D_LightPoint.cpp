//@ Project "Fu3D"

#include "stdafx.h"

#define MODULENAME "3DLightPoint"

#define BaseClass LightInputs3D
#define ThisClass LightPointInputs3D

#define ALLOW_GL_COLOR_FUNCTIONS
#define ALLOW_LIGHT_MODEL_COLOR_CONTROL

#ifdef FuPLUGIN
#include "FuPlugin.h"
#endif

#include "Resource.h"
#include "PipelineDoc.h"
#include "RegistryLists.h"

#include "3D_Scene.h"
#include "3D_Light.h"							// for the constants
#include "3D_LightPoint.h"
#include "3D_CameraBase.h"
#include "3D_ShadeContext.h"
#include "3D_MtlBaseImplSW.h"
#include "3D_RenderContextGL.h"
#include "3D_RenderContextSW.h"

const FuID LongID_Light_Point	= COMPANY_ID "LightPoint";			// All of the classes associated with LightPoint will add this REGID_LightLongID tag to their registry lists.




//
//
// LightPointInputs3D
//
//
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Point_Inputs, CT_LightInputs)
	REGS_Name,					"Point Light Inputs",
	TAG_DONE);

LightPointInputs3D::LightPointInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

LightPointInputs3D::~LightPointInputs3D()
{
}

bool LightPointInputs3D::AddInputsTagList(TagList &tags)
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

		owner->InTx->SetAttrs(LTC_Style, (uint32)LCT_Point, TAG_DONE);
	}

	AddStandardLightInputs();

	InDecayType = AddInput("Decay Type", "DecayType",
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		MULTIBUTTONCONTROL_ID,
		MBTNC_AddButton,			"No Decay",
		MBTNC_AddButton,			"Linear",
		MBTNC_AddButton,			"Quadratic",
		MBTNC_StretchToFit,		TRUE,
		INP_DoNotifyChanged,		TRUE,
		INP_Default,				0.0,
		TAG_DONE);

	InDecayRate = AddInput("Decay Rate", "DecayRate",
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		INP_MinAllowed,			0.001,
		INP_MaxScale,				1.0,
		INP_Default,				FuDefault_LightConstantDecay,
		TAG_DONE);

	CurrentDecayType = 0;
	InDecayRate->HideInputControls();

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

Data3D *LightPointInputs3D::ProcessTagList(Request *req, Data3D *data, TagList &tags)
{
	LightPointData3D *pointData = (LightPointData3D *) data;

	if (!pointData)
		pointData = new LightPointData3D(&LightPointData3D::Reg, nil, TagList());

	if (pointData)
	{
		pointData->Position = Vector3f(0.0f, 0.0f, 0.0f);
		float32 decayRate = *InDecayRate->GetValue(req);

		pointData->blur = *InBlur->GetValue(req);

		int decayType = int(*InDecayType->GetValue(req) + 0.5);
		switch (decayType)
		{
			case 0:
				pointData->ConstantDecay = 1.0f;
				pointData->LinearDecay = 0.0f;
				pointData->QuadraticDecay = 0.0f;
				break;
			case 1:
				pointData->ConstantDecay = 0.0f;
				pointData->LinearDecay = decayRate;
				pointData->QuadraticDecay = 0.0f;
				break;
			case 2:
				pointData->ConstantDecay = 0.0f;
				pointData->LinearDecay = 0.0f;
				pointData->QuadraticDecay = decayRate;
				break;
			default:
				uassert(0);
				break;
		}
	}
		
	return BaseClass::ProcessTagList(req, pointData, tags);
}

void LightPointInputs3D::NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags)
{
	if (param)
	{
		if (in == InDecayType)
		{
			int decayType = int(*(Number *)param + 0.5);
			if ((decayType == 0 && CurrentDecayType != 0) || (decayType != 0 && CurrentDecayType == 0))
			{
				if (decayType == 0)
					InDecayRate->HideInputControls();
				else
					InDecayRate->ShowInputControls();
			}

			CurrentDecayType = decayType;
		}
	}

	BaseClass::NotifyChanged(in, param, time, tags);
}







//
//
// LightPoint3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass LightOperator3D 
#define ThisClass LightPoint3D

FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Point_Operator, CT_Light3D)
	REGS_Name,							COMPANY_ID_SPACE "Point Light",
	REGS_Category,						COMPANY_ID_DBS "3D\\Light",
	REG_TileID,							IDB_TILE_3D_LIGHTPOINT,
	REGS_OpIconString,				"3PL",
	REGS_OpDescription,				"Generate a Point Light",
	REGS_HelpTopic,					"Tools/3D/PointLight",
	REG_OpNoMask,						TRUE,
//	REG_NoAutoProxy,					TRUE,
	REG_NoMotionBlurCtrls,			TRUE,		// perhaps this can be used in some other way
	REGID_LightLongID,				LongID_Light_Point,
	T3DREG_RotationControls,		FALSE,
	T3DREG_RotationPivotControls,	FALSE,
	T3DREG_ScaleControls,			FALSE,
	T3DREG_TargetControls,			FALSE,
	TAG_DONE);

LightPoint3D::LightPoint3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{	
	LightInputs = (LightPointInputs3D *) LightPointInputs3D::Reg.New(table, tags);

	if (LightInputs)
	{
		RegisterInputs(LightInputs);
		LightInputs->AddInputs();
	}
}

LightPoint3D::~LightPoint3D()
{
}







//
//
// LightPointData3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass LightData3D 
#define ThisClass LightPointData3D
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Point_Data, CT_LightData3D)
	REGS_Name,					COMPANY_ID_SPACE "Point Light Data",
	REGID_LightLongID,		LongID_Light_Point,
	TAG_DONE);

LightPointData3D::LightPointData3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

LightPointData3D::LightPointData3D(const LightPointData3D &toCopy) : LightData3D(toCopy)
{
}

LightPointData3D::~LightPointData3D()
{
}

Data3D *LightPointData3D::CopyTagList(TagList &tags)
{
	return new LightPointData3D(*this);
}

const Registry *LightPointData3D::GetImplRegistry(uint32 type)
{
	const Registry *ret = NULL;

	if (m_RegNode == &LightPointData3D::Reg)		// so subclasses aren't forced to also override this func
	{
		switch (type)
		{
			case CT_LightImplSW3D:
				ret = &LightPointSW3D::Reg;
				break;
			case CT_LightImplGL3D:
				ret = &LightPointGL3D::Reg;
				break;
		}
	}
	else
		ret = LightData3D::GetImplRegistry(type);

	return ret;
}









//
//
// LightPointGL3D class
//
//

#undef BaseClass
#undef ThisClass
#define BaseClass LightImplGL3D 
#define ThisClass LightPointGL3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Point_Impl_GL, CT_LightImplGL3D)
	REGS_Name,					COMPANY_ID "LightPointGLImpl",
	REGID_LightLongID,		LongID_Light_Point,
	TAG_DONE);


LightPointGL3D::LightPointGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

LightPointGL3D::~LightPointGL3D()
{
}

bool LightPointGL3D::Activate(RenderContextGL3D &rc, TagList *tags)
{
	if (rc.ShadePath == SP_FixedFunction)
	{
		LightPointData3D *data = (LightPointData3D *) LightData;

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		rc.GetLightToEye().glLoadMatrix();

		// we need to set all the light parameters in case this light was already set to something else...
		GLenum light = GL_LIGHT0 + rc.LightNumberFf;

		const float defaultAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightfv(light, GL_AMBIENT, defaultAmbient);
		glLightfv(light, GL_DIFFUSE, data->Color.V);
		glLightfv(light, GL_SPECULAR, data->Color.V);

		float pos[4] = { data->Position.X, data->Position.Y, data->Position.Z, 1.0f };
		glLightfv(light, GL_POSITION, pos);								// transformed by mv

		float dir[3] = { 0.0f, 0.0f, -1.0f };
		glLightf(light, GL_SPOT_CUTOFF, 180.0f);						// disable spotlights
		glLightfv(light, GL_SPOT_DIRECTION, dir);						// transformed by mv
		glLightf(light, GL_SPOT_EXPONENT, 0.0f);

		glLightf(light, GL_CONSTANT_ATTENUATION, data->ConstantDecay);
		glLightf(light, GL_LINEAR_ATTENUATION, data->LinearDecay);
		glLightf(light, GL_QUADRATIC_ATTENUATION, data->QuadraticDecay);

		glEnable(light);

		glPopMatrix();
	}
	else if (rc.ShadePath == SP_Cg)
	{
		LightPointCg *sn = (LightPointCg *) rc.CgLightShadeNode;
		LightPointData3D *data = (LightPointData3D *) LightData;

		// position
		Vector3f pos = rc.GetLightToEye().TransformPoint(data->Position);		// should this be in prerender?
		sn->InLightPos.Set(pos);

		// color
		Color4f lightColor = data->Color;
		lightColor.A = 1.0f;							// needed for projectors to work
		sn->InColor.Set(lightColor);

		// attenuation
		Vector4f atten(data->ConstantDecay, data->LinearDecay, data->QuadraticDecay);
		sn->InAttenuation.Set3fv(atten.V);
	}

	return true;
}

void LightPointGL3D::Deactivate(RenderContextGL3D &rc, TagList *tags)
{
	if (rc.ShadePath == SP_FixedFunction)
		glDisable(GL_LIGHT0 + rc.LightNumberFf);
}

const char *LightPointGL3D::GetShaderFilename(RenderContextGL3D &rc, TagList *tags)
{
	return "Shaders:Lights/Cg/FuLightPoint_f.cg";
}

ShadeNodeCg *LightPointGL3D::CreateShadeNode(RenderContextGL3D &rc, ShaderCg *shader, TagList *tags)
{
	LightPointCg *sn = new LightPointCg();

	if (sn)
	{
		ParamCg light = shader->CreateUserParameter("FuLightPoint");

		sn->m_Struct		= light;
		sn->InLightPos		= light.GetSubParameter("LightPos");
		sn->InColor			= light.GetSubParameter("Color");
		sn->InAttenuation	= light.GetSubParameter("Atten");
	}

	return sn;
}











//
//
// LightPointSW3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass LightImplSW3D 
#define ThisClass LightPointSW3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Point_Impl_SW, CT_LightImplSW3D)
	REGS_Name,					COMPANY_ID "LightPointSWImpl",
	REGID_LightLongID,		LongID_Light_Point,
	TAG_DONE);

LightPointSW3D::LightPointSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	LightPointData3D *lightData = (LightPointData3D *) tags.GetPtr(LGTP_LightData, NULL);

	Color = lightData->Color;

	ConstantDecay = lightData->ConstantDecay;
	LinearDecay = lightData->LinearDecay;
	QuadraticDecay = lightData->QuadraticDecay;
}

LightPointSW3D::~LightPointSW3D()
{
}

bool LightPointSW3D::PreRender(RenderContextSW3D &rc, TagList *tags)
{
	Position = rc.GetLightToEye().TransformPoint(((LightPointData3D *)LightData)->Position);			
	return true;
}

ParamBlockSW *LightPointSW3D::CreateParamBlock(RenderContextSW3D &rc, TagList *tags)
{
	LightPointBlock *block = new LightPointBlock;
	block->Size = sizeof(LightPointBlock);

	ParamSW *param;

	param = block->CreateParam(block->EyePosition, Stream_Position, CS_Eye);
	param->SetEnabled(true);
	param->SetRequired(true);

	return block;
}

bool LightPointSW3D::Illuminate(ShadeContext3D &sc, ParamBlockSW *lightBlock)
{
	LightPointBlock &block = (LightPointBlock &) *lightBlock;

	Vector3f lightVector = Position - *block.EyePosition;
	float32 dist = lightVector.Length();

	float32 atten = 1.0f / (ConstantDecay + LinearDecay * dist + QuadraticDecay * dist * dist);

	sc.AmbientLightColor = Color4f(0.0f, 0.0f, 0.0f, 0.0f);
	sc.LightColor = Color * atten;
	sc.LightVector = lightVector / dist;

	return true;				// just return true always for now - TODO - estimate a 0 radius
}
	
