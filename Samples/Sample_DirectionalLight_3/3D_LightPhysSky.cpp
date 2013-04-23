//@ Project "Fu3D"

#include "stdafx.h"

#define MODULENAME "3DLightPhysSky"

#define BaseClass LightInputs3D
#define ThisClass LightPhysSkyInputs3D

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
#include "3D_LightPhysSky.h"

const FuID LongID_Light_PhysSky = COMPANY_ID "LightPhysSky";			// All of the classes associated with LightPhysSky will add this REGID_LightLongID tag to their registry lists.



//
//
// LightPhysSkyInputs3D
//
//
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_PhysSky_Inputs, CT_LightInputs)
	REGS_Name,					COMPANY_ID "Physical Sky Light Inputs",
	TAG_DONE);

LightPhysSkyInputs3D::LightPhysSkyInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

LightPhysSkyInputs3D::~LightPhysSkyInputs3D()
{
}

bool LightPhysSkyInputs3D::AddInputsTagList(TagList &tags)
{
	/*if (Owner->IsTypeOf(Transform3DOperator::ClassID))
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
	}*/

	//AddStandardLightInputs();
	InLatitude = AddInput("latitude", "latitude", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".latitude"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("latitude"),
		INP_MinAllowed,			-180.0,
		INP_MinScale,				-180.0,
		INP_MaxScale,				180.0,
		INP_Default,				0.0,
		IC_Visible,					TRUE,
		TAG_DONE);
	
	InLongitude = AddInput("longitude", "longitude", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".longitude"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("longitude"),
		INP_MinAllowed,			-90.0,
		INP_MinScale,				-90.0,
		INP_MaxScale,				90.0,
		INP_Default,				0.0,
		IC_Visible,					TRUE,
		TAG_DONE);

	InDay = AddInput("Day", "Day", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".day"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("day"),
		INP_MinAllowed,			0.0,
		INP_MinScale,				0.0,
		INP_MaxScale,				365.0,
		INP_Default,				120.0,
		IC_Visible,					TRUE,
		TAG_DONE);

	InTime = AddInput("time", "Time", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".time"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("time"),
		INP_MinAllowed,			0.0,
		INP_MinScale,				0.0,
		INP_MaxScale,				24.0,
		INP_Default,				12.0,
		IC_Visible,					TRUE,
		TAG_DONE);

	InTurbiditye = AddInput("turbidity", "turbidity", 
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		LINKID_LegacyID,			FuID(".turbidity"),
		I3D_AutoFlags,				PF_AutoProcess,
		I3D_ParamName,				FuID("turbidity"),
		INP_MinAllowed,			2.0,
		INP_MinScale,				2.0,
		INP_MaxScale,				10.0,
		INP_Default,				2.0,
		IC_Visible,					TRUE,
		TAG_DONE);

	


	return true;
}

Data3D *LightPhysSkyInputs3D::ProcessTagList(Request *req, Data3D *data, TagList &tags)
{
	LightPhysSkyData3D *dirData = (LightPhysSkyData3D *) data;

	if (!dirData)
		dirData = new LightPhysSkyData3D(&LightPhysSkyData3D::Reg, nil, TagList());

	if (dirData)
	{
		dirData->Direction = Vector3f(0.0f, 0.0f, -1.0f);
		dirData->latidue =  *InLatitude->GetValue(req);
		dirData->longitude =  *InLongitude->GetValue(req);
		dirData->time =  *InTime->GetValue(req);
		dirData->turbi = *InTurbiditye->GetValue(req);
		dirData->day = *InDay->GetValue(req);
	}
		
	return BaseClass::ProcessTagList(req, dirData, tags);
}






//
//
// LightPhysSky3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass LightOperator3D
#define ThisClass LightPhysSky3D

FuRegisterClass(COMPANY_ID ".PhysSkyLight", CT_Light3D)
	REGS_Name,							COMPANY_ID_SPACE "PhysicalSky Light",
	REGS_AboutDescription,			"Generate a PhysicalSky Light",
	REGS_Category,						COMPANY_ID_DBS "3D\\Light",
	REG_TileID,							IDB_TILE_3D_LIGHTDIRECTIONAL,
	REGS_OpIconString,				"3DL",
	REGS_HelpTopic,					"Tools/3D/PhysicalSky",
	REG_OpNoMask,						TRUE,
//	REG_NoAutoProxy,					TRUE,
	REG_NoMotionBlurCtrls,			TRUE,		// perhaps this can be used in some other way
	REGID_LightLongID,				LongID_Light_PhysSky,
	T3DREG_ScaleControls,			FALSE,
	TAG_DONE);


LightPhysSky3D::LightPhysSky3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	LightInputs = (LightPhysSkyInputs3D *) LightPhysSkyInputs3D::Reg.New(table, tags);

	if (LightInputs)
	{
		RegisterInputs(LightInputs);
		LightInputs->AddInputs();
	}
}

LightPhysSky3D::~LightPhysSky3D()
{
}







//
//
// LightPhysSkyData3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass LightData3D
#define ThisClass LightPhysSkyData3D
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_PhysSky_Data, CT_LightData3D)
	REGS_Name,					COMPANY_ID "Physical Sky Light Data",
	REGID_LightLongID,		LongID_Light_PhysSky,
	TAG_DONE);

LightPhysSkyData3D::LightPhysSkyData3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

LightPhysSkyData3D::LightPhysSkyData3D(const LightPhysSkyData3D &toCopy) : LightData3D(toCopy)
{
}

LightPhysSkyData3D::~LightPhysSkyData3D()
{
}

Data3D *LightPhysSkyData3D::CopyTagList(TagList &tags)
{
	return new LightPhysSkyData3D(*this);
}

const Registry *LightPhysSkyData3D::GetImplRegistry(uint32 type)
{
	const Registry *ret = NULL;

	if (m_RegNode == &LightPhysSkyData3D::Reg)		// so subclasses aren't forced to also override this func
	{
		switch (type)
		{
			case CT_LightImplSW3D:
				ret = &LightPhysSkySW3D::Reg;
				break;
			case CT_LightImplGL3D:
				ret = &LightPhysSkyGL3D::Reg;
				break;
		}
	}
	else
		ret = LightData3D::GetImplRegistry(type);

	return ret;
}










//
//
// LightPhysSkyGL3D class
//
//

#undef BaseClass
#undef ThisClass
#define BaseClass LightImplGL3D 
#define ThisClass LightPhysSkyGL3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Directional_Impl_GL, CT_LightImplGL3D)
	REGS_Name,					COMPANY_ID "LightDirectionalGLImpl",
	REGID_LightLongID,		LongID_Light_PhysSky,
	TAG_DONE);


LightPhysSkyGL3D::LightPhysSkyGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

LightPhysSkyGL3D::~LightPhysSkyGL3D()
{
}

bool LightPhysSkyGL3D::Activate(RenderContextGL3D &rc, TagList *tags)
{
	if (rc.ShadePath == SP_FixedFunction)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		rc.GetLightToEye().glLoadMatrix();

		// we need to set all the light parameters in case this light was already set to something else...
		GLenum light = GL_LIGHT0 + rc.LightNumberFf;

		LightPhysSkyData3D *data = (LightPhysSkyData3D *) LightData;

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
		LightPhysSkyCg *sn = (LightPhysSkyCg *) rc.CgLightShadeNode;
		LightPhysSkyData3D *data = (LightPhysSkyData3D *) LightData;

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

void LightPhysSkyGL3D::Deactivate(RenderContextGL3D &rc, TagList *tags)
{
	if (rc.ShadePath == SP_FixedFunction)
		glDisable(GL_LIGHT0 + rc.LightNumberFf);
}

const char *LightPhysSkyGL3D::GetShaderFilename(RenderContextGL3D &rc, TagList *tags)
{
	return "Shaders:Lights/Cg/FuLightDirectional_f.cg";
}

ShadeNodeCg *LightPhysSkyGL3D::CreateShadeNode(RenderContextGL3D &rc, ShaderCg *shader, TagList *tags)
{
	LightPhysSkyCg *sn = new LightPhysSkyCg();

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
// LightPhysSkySW3D class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass LightImplSW3D 
#define ThisClass LightPhysSkySW3D 
FuRegisterClass(COMPANY_ID_DOT + CLSID_Light_Directional_Impl_SW, CT_LightImplSW3D)
	REGS_Name,					COMPANY_ID "LightPhysSkySWImpl",
	REGID_LightLongID,		LongID_Light_PhysSky,
	TAG_DONE);

LightPhysSkySW3D::LightPhysSkySW3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	LightPhysSkyData3D *lightData = (LightPhysSkyData3D *) tags.GetPtr(LGTP_LightData, NULL);
	Color = lightData->Color;
	Direction = lightData->Direction;
}

LightPhysSkySW3D::~LightPhysSkySW3D()
{
}

bool LightPhysSkySW3D::PreRender(RenderContextSW3D &rc, TagList *tags)
{
	Direction = -rc.GetLightToEyeIT().TransformVector(Direction);
	Direction.Normalize();

	return true;
}

ParamBlockSW *LightPhysSkySW3D::CreateParamBlock(RenderContextSW3D &rc, TagList *tags)
{
	LightPhysSkyBlock *block = new LightPhysSkyBlock;
	block->Size = sizeof(LightPhysSkyBlock);
	return block;
}

bool LightPhysSkySW3D::Illuminate(ShadeContext3D &sc, ParamBlockSW *lightBlock)
{
	sc.AmbientLightColor = Color4f(0.0f, 0.0f, 0.0f, 0.0f);
	sc.LightColor = Color;
	sc.LightVector = Direction;
	return true;
}


	