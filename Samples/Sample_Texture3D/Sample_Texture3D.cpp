//@ Project "Fu3D"

#include "stdafx.h"

#define MODULENAME "3DSampleTexture3D"			// internal eyeon use - not needed for plugins

#define BaseClass MtlOperator3D 
#define ThisClass Texture3DTool 

#define ALLOW_GL_COLOR_FUNCTIONS
#define ALLOW_LIGHT_MODEL_COLOR_CONTROL

#ifdef FuPLUGIN
#include "FuPlugin.h"
#endif

#include "RegistryLists.h"

#include "Resource.h"
#include "GLTexture.h"
#include "PipelineDoc.h"
#include "TextureManager.h"
#include "Sample_Texture3D.h"

#include "3D_Stream.h"
#include "3D_MtlGraph.h"
#include "3D_RenderContextGL.h"
#include "3D_RenderContextSW.h"

/*
 * Texture3D:
 *
 *
 * This file consists of 5 classes.  I'm going to explain briefly here what they are and how they interact:
 *
 * Texture3DTool:  This class represents the actual tool on the flow.  You'll notice its nothing more than a skeleton class whose
 *                 purpose is to host a set of Texture3DInputs.  When the Texture3DTool is asked to Process() it askes the 
 *                 Texture3DInputs to Process(), which in turn produce a Texture3DData object.  The Texture3DTool then takes
 *                 the Texture3DData object and does some stuff with it an outputs it.
 *
 * Texture3DInputs:  These are the inputs for the 3D texture.  The Texture3DInputs object gets hosted by the Texture3DTool.  When 
 *                   Texture3DInputs::Process() is called it produces and returns a Texture3DData object.
 *
 * Texture3DData:  This is a data object produced by the Texture3DInputs.  Its job is to encapsulate a renderer agnostic description
 *                 of the material.  The Texture3DData will flow downstream and eventually get attached to a Node3D in a Scene3D.
 *                 The Scene3D containing the Texture3DData flows down until eventually its hits a renderer at which point the renderer 
 *                 will need to create either a Texture3DSW or Texture3DGL implementation. 
 *
 * Texture3DSW:  This is a software renderer implementation of the Texture3D material.  The renderer will call CreateParamBlock() to
 *               allow the material to request any interpolants it needs to do its shading.  In this case the requested interpolants
 *               are UVWs.  When a triangle is rendered, the requested UVWs are interpolated across the triangle and supplied to the
 *               material inside of ShadeFragment().
 *
 * Texture3DGL:  This is an OpenGL renderer implementation of the Texture3D material.  The renderer will call CreateShadeNode() in 
 *               which a Cg shade node is created and parameters in the shader code are connected to ParamCgs in the C++ code.
 *
 * It may seem a bit overly complicated to have 5 classes for a material.  The Texture3DInputs and Texture3DTool classes could
 * be merged into one class if the plugin developer really wanted (but not recommeneded).  There are two advantages of doing it this way:
 *   - Texture3DInputs receives functionality from its base class MtlInputs3D like the material ID inputs
 *   - inheriting from MtlInputs3D allows plugin renderers to inject inputs into Texture3DInputs
 *
 * The tool/inputs/data paradigm is used all over the place in the 3D SDK.  It is used for lights, cameras, surfaces, and materials.
 * The reasons for its use actually goes much deeper.  This paradigm is used by Fusion to allow inputs to be injected from one plugin
 * into another plugin.  For example, the blend mode inputs on ImagePlane3D are actually put there by the RendererGL3D and
 * RendererSW3D plugins.
 */

// Each class belonging to the Texture3D material must set a REGID_MaterialLongID tag into its class registry node.  This tag is used
// to match up a Texture3DData object with a renderer specific implementation.  For example, the GL Renderer receives a Texture3DData
// object in the scene and it needs to construct a Texture3DGL object for rendering.  It will search the class registry for CT_MtlImplGL3D
// classes with the same REGID_MaterialLongID as the Texture3DData object.
static const FuID LongID_Texture3D = COMPANY_ID "Texture3D";

static const FuID CLSID_Texture3D_Data    = "Texture3DData";
static const FuID CLSID_Texture3D_GL      = "Texture3DGL";
static const FuID CLSID_Texture3D_SW      = "Texture3DSW";
static const FuID CLSID_Texture3D_Inputs  = "Texture3DInputs";
static const FuID CLSID_Texture3D_Tool    = "Texture3DTool";
static const FuID CLSID_GLSampleTexture3D = "GLSampleTexture3D";	// put a "Sample" here so this doesn't conflict with eyeon version of GLTexture3D



//
//
// Texture3DTool class
//
//
FuRegisterClass(COMPANY_ID_DOT + CLSID_Texture3D_Tool, CT_Texture3D)
	REGS_Name,					COMPANY_ID_SPACE "Texture 3D",	// "samples Texture 3D" - this name minus the spaces appears on the tools tile in the flow view
	REGS_Category,				COMPANY_ID_DBS "3D\\Texture",		// "samples\\Texture" - this determines where the tool appears within the menus
	REGS_OpIconString,		"3T3",									// this text appears on the toolbar - not really sure what to call it in this case
	REGS_OpDescription,		"Create a 3D Texture",				// text that appears in the statusbar when hovering the cursor over the "3T3" toolbar entry
	REGS_AboutDescription,	"Generate a 3D texture from a sequence of images",
	REGID_MaterialLongID,	LongID_Texture3D,						// all classes that make up the Texture3D material must add this to their registry nodes
	REG_ToolbarDefault,		FALSE,									// don't add the default mask input that most 2D filter tools have
	REG_OpNoMask,				TRUE,										// don't add the blend controls that appear on the radioactive tab
	REG_NoBlendCtrls,			TRUE,										// don't add the object/material ID controls that appear on the radioactive tab
	REG_NoObjMatCtrls,		TRUE,										// don't add the motionblur controls that appear on the radioactive tab
	REG_NoMotionBlurCtrls,	TRUE,										// we don't want the default targetting controls on the transform tab
	TAG_DONE);

Texture3DTool::Texture3DTool(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	// Create the inputs.  We could have instead called ClassRegistry->New(CLSID_Texture3D_Inputs, CT_MtlInputs3D, table, tags) to create
	// them but this would have involved a slower search through the registry.
	MtlInputs = (Texture3DInputs *) Texture3DInputs::Reg.New(table, tags);

	// This registers the inputs for receiving input notifications like NotifyChanged(), OnAddToFlow(), and Process().
	RegisterInputs(MtlInputs);

	// This does the actual adding of inputs to this tool.
	MtlInputs->AddInputs();
}

Texture3DTool::~Texture3DTool()
{
	// What we created in the constructor we need to destroy here.  Note that for classes that inherit RefObject you should always 
	// call Recycle() rather than calling delete directly or you can cause crashes.
	SAFE_RECYCLE(MtlInputs);
}










//
//
// Texture3DInputs class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlInputs3D 
#define ThisClass Texture3DInputs 

FuRegisterClass(COMPANY_ID_DOT + CLSID_Texture3D_Inputs, CT_MtlInputs3D)
	REGID_MaterialLongID,	LongID_Texture3D,
	TAG_DONE);

Texture3DInputs::Texture3DInputs(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
}

Texture3DInputs::~Texture3DInputs()
{
}

bool Texture3DInputs::AddInputsTagList(TagList &tags)
{
	AddControlPage("Controls");

	InImage = AddInput("Input", "Input",
		LINKID_DataType,			CLSID_DataType_Image,
		LINK_Main,					1,
		INP_Required,				TRUE,
		TAG_DONE);

	InStartTime = AddInput("Start Time", "StartTime",
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		INP_Default,				0.0,
		INP_MinScale,				0.0,
		INP_MaxScale,				100.0,
		TAG_DONE);

	InNumberOfFrames = AddInput("Number of Frames", "NumberOfFrames",
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		SLIDERCONTROL_ID,
		INP_Default,				64.0,
		INP_MinScale,				1.0,
		INP_MaxScale,				256.0,
		TAG_DONE);

	InWrapType = AddInput("Wrap Type", "WrapType",
		LINKID_DataType,			CLSID_DataType_FuID,
		ICS_Name,					"Wrap Type",
		INPID_InputControl,		COMBOIDCONTROL_ID,
		CCS_AddString,				WrapModeNames[WM_Wrap],
		CCID_AddID,					WrapModeIDs[WM_Wrap],
		CCS_AddString,				WrapModeNames[WM_Clamp],
		CCID_AddID,					WrapModeIDs[WM_Clamp],
		CCS_AddString,				WrapModeNames[WM_Black],
		CCID_AddID,					WrapModeIDs[WM_Black],
		INPID_DefaultID,			WrapModeIDs[WM_Clamp],
		TAG_DONE);

	return BaseClass::AddInputsTagList(tags);
}

	//virtual bool ProcessImage(Request *req, Input *inImage)
	//{
	//	ImageInput = inImage;
	//	ImageInputTime = req->GetInputTime(inImage);
	//	ImageInputFlags = (req->GetInputFlags(inImage) & ~REQF_PreCalc);		// when we request it for real later, we don't want a precalc request
	//	return true;
	//}

Data3D *Texture3DInputs::ProcessTagList(Request *req, Data3D *data, TagList &tags)
{
	Texture3DData *texData = (Texture3DData *) data;

	// We're going to remember the input and its request flags so we can later GetSource() it.
	texData->InImage = InImage;
	texData->ImageFlags = req->GetInputFlags(InImage);

	texData->StartTime = *InStartTime->GetValue(req);

	Image *img = (Image *) InImage->GetValue(req);
	texData->Width = img->Width;
	texData->Height = img->Height;
	texData->Depth = *InNumberOfFrames->GetValue(req);

	FuIDParam *wrapModeID = (FuIDParam *) InWrapType->GetValue(req);
	texData->WrapMode = (WM_WrapMode) (int32) GetDFIDIndex(*wrapModeID, WrapModeIDs);

	// pass the data to the base class to give it an opportunity to initialize its portion of the Data3D object
	return BaseClass::ProcessTagList(req, data, tags);
}

void Texture3DInputs::ShowInputs(bool visible)
{
	// There's no need to override ShowInputs() unless you think another tool will be planning to show/hide your inputs.
	// In our case the only tool that will be hosting our inputs is Texture3DTool and it will always leave them in their
	// default showing state so overriding ShowInputs() is unnecessary.  I've implemented this function here to show you 
	// how you would do it if you need to.

	if (visible)
	{
		InStartTime->ShowInputControls();
		InNumberOfFrames->ShowInputControls();
		InWrapType->ShowInputControls();
	}
	else
	{
		InStartTime->HideInputControls();
		InNumberOfFrames->HideInputControls();
		InWrapType->HideInputControls();
	}

	BaseClass::ShowInputs(visible);
}

void Texture3DInputs::CheckRequest(Request *req)
{
	BaseClass::CheckRequest(req);

	// CheckRequest() gets called when Texture3DInputs is about to issue its upstream.  Here we are telling Fusion to treat InImage as a PreCalc
	// request - this means inside of Process() we'll get a no-data image - its scanline pointers will be NULL but its width/height will be correct.
	if (req->GetPri() == InImage->Priority && !req->IsFailed()) 
		req->SetInputFlags(InImage, REQF_PreCalc);
}






//
//
// Texture3DData
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlData3D 
#define ThisClass Texture3DData
FuRegisterClass(COMPANY_ID_DOT + CLSID_Texture3D_Data, CT_MtlData3D)
	REGS_Name,					COMPANY_ID "Texture3DData",
	REGID_MaterialLongID,	LongID_Texture3D,
	TAG_DONE);

Texture3DData::Texture3DData(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	WrapMode = WM_Clamp;
}

Texture3DData::Texture3DData(const Texture3DData &toCopy) : BaseClass(toCopy)
{
	StartTime	= toCopy.StartTime;
	Width			= toCopy.Width;
	Height		= toCopy.Height;
	Depth			= toCopy.Depth;
	WrapMode		= toCopy.WrapMode;
	InImage		= toCopy.InImage;
	ImageFlags	= toCopy.ImageFlags;
}

Texture3DData::~Texture3DData()
{
}

bool Texture3DData::CreateManagedParams()
{
	return _funner;
}

Data3D *Texture3DData::CopyTagList(TagList &tags)
{
	return new Texture3DData(*this);
}

const Registry *Texture3DData::GetImplRegistry(uint32 type)
{
	// The job of this function is to find a matching GL or SW implementation in the ClassRegistry.  By default it will
	// search through the entire ClassRegistry for a match.  Purely as an optimization here we provide an implementation
	// that avoids this extra searching.  You could remove this function and everything would still work ok.
	const Registry *ret = NULL;

	if (m_RegNode == &Texture3DData::Reg)		// so subclasses aren't forced to also override this func
	{
		switch (type)
		{
			case CT_MtlImplSW3D:
				ret = &Texture3DSW::Reg;
				break;
			case CT_MtlImplGL3D:
				ret = &Texture3DGL::Reg;
				break;
		}
	}
	else
		ret = MtlData3D::GetImplRegistry(type);

	return ret;
}

bool Texture3DData::IsOpaque() 
{ 
	// Materials which are opaque can be substantially faster than transparents.  You don't have to clip & sort their polygons in GL
	// and you know you only have to shade the front most surfaces pixels. 
	return false; 
}

bool Texture3DData::IsTexture() 
{ 
	return true;
}

uint64 Texture3DData::EstimateVideoMemoryUsage()
{
	return sizeof(uint8) * Width * Height * Depth + EstimateChildVideoMemoryUsage();
}










//
//
// Texture3DGL
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlImplGL3D 
#define ThisClass Texture3DGL
FuRegisterClass(COMPANY_ID_DOT + CLSID_Texture3D_GL, CT_MtlImplGL3D)
	REGS_Name,					COMPANY_ID "CreateTextureGL3D",
	REGID_MaterialLongID,	LongID_Texture3D,
	TAG_DONE);


Texture3DGL::Texture3DGL(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	Texture3DData *mtlData = (Texture3DData *) tags.GetPtr(MTL_MaterialData, NULL);

	Width = mtlData->Width;
	Height = mtlData->Height;
	Depth = mtlData->Depth;

	WrapMode = GL_WrapMode[mtlData->WrapMode];

	Texture = NULL;
}

Texture3DGL::~Texture3DGL()
{
}

bool Texture3DGL::PreRender(RenderContextGL3D &rc)
{
	// All Activate()/Deactivate() calls will be bracketed by a PreRender()/PostRender() pair.  PreRender()/PostRender() will usually only get
	// called once per render.
	if (rc.ShadePath == SP_FixedFunction)
	{
	}
	else if (rc.ShadePath == SP_Cg)
	{
	}

	return true;
}

bool Texture3DGL::Activate(RenderContextGL3D &rc)
{
	// In Activate() you should do whatever is necessary to make your material active for rendering.  At a very simple level the 
	// renderer will do something like:
	//   mtl->Activate();
	//   RenderMeshes(mtl, meshesUsingMtl);
	//   mtl->Deactivate();

	if (!ExtSupported(GL_ARB_texture_non_power_of_two_df))
	{
		if (!IsPowerOf2(Width) || !IsPowerOf2(Height) || !IsPowerOf2(Depth))
		{
			// TODO - error report in rc.
			//DocPtr->PrintF(ECONID_Error, "%s: faces are not the same size\n", toolName);
			return false;
		}
	}

	Texture3DData *data = (Texture3DData *) MtlData;

	if (!Texture)
	{
		// Check for a cached texture before creating a new one.
		data->ResourceListLock.ObtainLock();

		if (data->ResourceList.size() > 0)
		{
			Resource3D *res = data->ResourceList[0];

			if (res->IsTypeOf(CLSID_GLSampleTexture3D))
			{
				Texture = (GLTexture3D *) res;
				res->Use();
			}
		}

		data->ResourceListLock.ReleaseLock();

		if (!Texture)
			Texture = new GLTexture3D;

		// Any resources such as textures and vertex arrays must be prerendered and activated in a similar fashion to materials.
		// This is used as a hint to the resource manager.  If a texture is activated then the manager knows it is in use and it shouldn't
		// try to free it up.  If a texture is prerendered the manager knows that the texture will potentially be in use at some point
		// in the future during the current render.
		Texture->PreRender();
	}

	if (Texture->BeginUpload(Width, Height, Depth))			// BeginUpload() will return true to indicate that we need to upload the texture data
	{
		uint32 flags = (data->Flags & ~REQF_PreCalc);		// remove the pre calc flag - we want an image with data
		flags |= REQF_SecondaryTime;								// we don't want the inputs on tools updating when we issue our GetSourceAttrs() request

		for (int i = 0; i < Depth; i++)
		{
			// Instead getting all the images inside of Texture3DInputs::Process() we delay the requests for the images until we are ready to
			// render with the material.  This is an advantage because the images don't use up memory until render time but a disadvantage because
			// GetSourceAttrs() occurs on the current thread (and if this is a view render it will lock up the GUI thread)
			Image *img = (Image *) data->InImage->GetSourceAttrs(data->StartTime + i, NULL, flags);
			Texture->UploadSlice(i, img);
			img->Recycle();
		}

		Texture->EndUpload();

		// Cache the texture back into the resource list so next time we activate this material we reuse it
		data->CacheResource(Texture);
	}

	Matrix4f texMatrix;
	texMatrix.Identity();

	// Although it is not used very much anymore Fusion has options to render using per-vertex and per-pixel lighting.  The per vertex path will
	// only get used on really old cards or by advanced users who are trying to speed up interactivity in the views for shader intensive scenes.
	if (rc.ShadePath == SP_FixedFunction)
	{
		TexUnit = rc.GetTextureUnit();
		if (TexUnit == -1)
			return false;

		glActiveTextureARB(GL_TEXTURE0_ARB + TexUnit);
		
		Texture->Enable();

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, WrapMode);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, WrapMode);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, WrapMode);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);				
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

		Texture->Bind();

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glMatrixMode(GL_TEXTURE);
		texMatrix.glLoadMatrix();
		glActiveTextureARB(GL_TEXTURE0_ARB + 0);
	}
	else if (rc.ShadePath == SP_Cg)
	{
		// Here we must initialize the parameters in our shade node.
		Texture3DCg *sn = (Texture3DCg *) rc.CgShader->GetShadeNode(this);

		sn->Transform.Set3x3(texMatrix);

		Texture->Bind();

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, WrapMode);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, WrapMode);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, WrapMode);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Texture->Unbind();

		sn->Map.SetTexture(Texture->GLTexName);
	}

	return true;
}

void Texture3DGL::Deactivate(RenderContextGL3D &rc)
{
	if (Texture)
	{
		if (rc.ShadePath == SP_FixedFunction)
		{
			glActiveTextureARB(GL_TEXTURE0_ARB + TexUnit);
			
			Texture->Disable();
			Texture->Unbind();			// this allows GL to free up the texture if it needs to do so

			glActiveTextureARB(GL_TEXTURE0_ARB + 0);
		}
		else if (rc.ShadePath == SP_Cg)
		{
		}

		Texture->Deactivate();
	}
}

void Texture3DGL::PostRender(RenderContextGL3D &rc)
{
	if (Texture)
	{
		Texture->PostRender();
		Texture->Recycle();					// its not really going away since we cached it
		Texture = NULL;
	}
}

ShaderString3D Texture3DGL::CreateShaderString(RenderContextGL3D &rc)
{
	// To get shader strings into Fusion you can either override GetShaderFilename() or override CreateShaderString().
	// By default CreateShaderString() will call GetShaderFilename(rc, tags) and then load the shader string and return it.

	/* 
	   All materials must inherit FuFloat4.  You can find these class definitions inside of Shaders/Generic/FuInterfaces.cg.

		struct FuFragment
		{
			float4 VertexColor  : COLOR;  
			float3 TexCoord0    : TEXCOORD0;
			float3 TexCoord1    : TEXCOORD1;
			float3 TexCoord2    : TEXCOORD2;
			float3 EnvCoord     : TEXCOORD3;
			float3 Position     : TEXCOORD4;	// eye space
			float3 Normal       : TEXCOORD5;	// eye space
			float3 TangentU     : TEXCOORD6;	// eye space
			float3 TangentV     : TEXCOORD7;	// eye space
		};

		interface FuFloat4
		{
			float4 Shade(inout FuFragment f);
		};
	*/

	static const char *shader = 
		"#ifndef _SAMPLE_TEXTURE3D_CG_ \n"			// always use inclusion guards
		"#define _SAMPLE_TEXTURE3D_CG_ \n"

		"struct SampleTexture3D : FuFloat4 \n"		// plugins should not begin their class names with "Fu" or they could conflict with eyeon classes
		"{"
			"sampler3D Map; \n"
			"float3x3 Transform; \n"

			"float4 Shade(inout FuFragment f) \n"
			"{ \n"
				"float3 uvw = mul(Transform, f.TexCoord0); \n"
				"return tex3D(Map, uvw); \n"
			"} \n"
		"}; \n"

		"#endif";

	return ShaderString3D(shader);
}

const char *Texture3DGL::GetShadeNodeName(RenderContextGL3D &rc)
{
	return "SampleTexture3D";
}

bool Texture3DGL::ConnectParams(RenderContextGL3D &rc, ShaderCg *shader, ShadeNodeCg *node, ParamCg mtlStruct)
{
	// The Cg material shader consists of a bunch connected shade nodes.  In this function we build our Cg shade node and associate
	// it with our C++ shade node Texture3DCg.  We need to connect up the parameters - in this case the parameters we are connecting 
	// to are the "sampler3D Map" and the "float3x3 Transform".  Note that you should not try initializing any non-literal parameters 
	// like "Transform" here - parameters should be initialized in Activate().  Because of shader caching CreateShadeNode() will only
	// be called when a shade tree is created the first time.

	Texture3DCg *sn = (Texture3DCg *) node;
	Texture3DData *mtlData = (Texture3DData *) MtlData;					// this is how we'd access the MtlData if we needed to

	sn->Transform = mtlStruct.GetSubParameter("Transform");
	sn->Map       = mtlStruct.GetSubParameter("Map");	

	return true;
}

ShadeNodeCg *Texture3DGL::CreateShadeNode(RenderContextGL3D &rc)
{
	return new Texture3DCg;
}








//
//
// Texture3DSW
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass MtlImplSW3D 
#define ThisClass Texture3DSW
FuRegisterClass(COMPANY_ID_DOT + CLSID_Texture3D_SW, CT_MtlImplSW3D)
	REGS_Name,					COMPANY_ID "CreateTextureSW3D",
	REGID_MaterialLongID,	LongID_Texture3D,
	TAG_DONE);

Texture3DSW::Texture3DSW(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	Texture3DData *mtlData = (Texture3DData *) tags.GetPtr(MTL_MaterialData, NULL);
	WrapMode = mtlData->WrapMode;

	Img = NULL;
	ShadeFunc = (ShadeFunc3D) &Texture3DSW::ShadeFragment;
}

Texture3DSW::~Texture3DSW()
{
	Img = NULL;
	uassert(Img == FuUnmappedPointer<Image>(NULL));
}

ParamBlockSW *Texture3DSW::CreateParamBlock(RenderContextSW3D &rc)
{
	Texture3DData *mtlData = (Texture3DData *) MtlData;

	Texture3DBlock *block = new Texture3DBlock;
	block->Size = sizeof(Texture3DBlock);

	ParamSW *param;

	param = block->CreateParam(block->TexCoord, Stream_TexCoord0, CS_Unset);
	param->SetEnabled(true);
	param->SetRequired(true);

	param = block->CreateParam(block->TexCoordDx, Stream_TexCoord0, CS_Unset);
	param->SetDiffX(true);
	param->SetEnabled(rc.HiQ);
	param->SetRequired(true);

	param = block->CreateParam(block->TexCoordDy, Stream_TexCoord0, CS_Unset);
	param->SetDiffY(true);
	param->SetEnabled(rc.HiQ);
	param->SetRequired(true);

	param = block->CreateParam(block->Color, Stream_Color, CS_Unset);
	param->SetEnabled(true);
	param->SetRequired(false);

	return block;
}

bool Texture3DSW::Activate(RenderContextSW3D &rc)
{
	Texture3DData *texImgData = (Texture3DData *) MtlData;

	uassert(!Img);

	//Img = texImgData->Img;
	//FuASSERT(Img, ("no image!"));		// someone didn't check IsComplete() for before connecting to this mtls output?
	//
	//if (Img)
	//{
	//	Img->Use();
	//	if (rc.HiQ)
	//		Img->UseSAT();
	//}

	//return Img != NULL;
	return true;
}

void Texture3DSW::Deactivate(RenderContextSW3D &rc)
{
	//if (rc.HiQ)
	//	Img->RecycleSAT();

	//Img->Recycle();
	//Img = NULL;
}

void Texture3DSW::ShadeFragment(ShadeContext3D &sc)
{
	Texture3DBlock &block = (Texture3DBlock &) sc.GetMaterialBlock(this);

	Vector4f out;

	if (sc.HiQ)
	{
		const Vector2f &tC = *block.TexCoord;			// center
		const Vector2f &tX = *block.TexCoordDx;		// center + (0.5, 0)
		const Vector2f &tY = *block.TexCoordDy;		// center + (0, 0.5)

		Vector2f dtX = tX - tC;
		Vector2f dtY = tY - tC;

		Vector2f fLL = tC - dtX - dtY;
		Vector2f fLR = tC + dtX - dtY;
		Vector2f fUL = tC - dtX + dtY;
		Vector2f fUR = tC + dtX + dtY;

		float32 width  = 65536.0f * Img->Width;
		float32 height = 65536.0f * Img->Height;

		FltPixel p;

		float64 xLL = fLL.X * width - 32768.0;
		float64 yLL = fLL.Y * height - 32768.0;

		float64 xLR = fLR.X * width - 32768.0;
		float64 yLR = fLR.Y * height - 32768.0;

		float64 xUL = fUL.X * width - 32768.0;
		float64 yUL = fUL.Y * height - 32768.0;

		float32 xUR = fUR.X * width - 32768.0;
		float32 yUR = fUR.Y * height - 32768.0;

		//if (WrapMode == WM_Clamp)
		//	Img->SampleAreaD(xLL, yLL, xLR, yLR, xUL, yUL, xUR, yUR, p);
		//else if (WrapMode == WM_Wrap)
		//	Img->SampleAreaW(xLL, yLL, xLR, yLR, xUL, yUL, xUR, yUR, p);
		//else if (WrapMode == WM_Black)
		//	Img->SampleAreaB(xLL, yLL, xLR, yLR, xUL, yUL, xUR, yUR, p);

		out.R = p.R;		// OPT: we can do this faster
		out.G = p.G;
		out.B = p.B;
		out.A = p.A;
	}
	else
	{
		const Vector2f &tc = *block.TexCoord;

		FltPixel p;

		// Fusions Image sampling functions assign a pixels coordinates to the center of a pixel.  OpenGL assigns pixel coordinates to the
		// lower left corner.  So to convert we need to add/subtract a half pixel.  (eg. in Fusion, the center of the (0, 0) pixel has coordinates
		// (0, 0), the lower left has coordinates (-32k, -32k), and the top right corner has coordinates (+32k, +32k), and pixels have a size of
		// 64k by 64k.
		// (TODO - optimize(?) this by caching width, height, Image ptr instead of using FuPointer?)
		float64 x = tc.X * Img->Width * 65536.0 - 32768.0;
		float64 y = tc.Y * Img->Height * 65536.0 - 32768.0;

		//if (WrapMode == WM_Clamp)
		//	Img->SamplePixelD(x, y, p);
		//else if (WrapMode == WM_Wrap)
		//	Img->SamplePixelW(x, y, p);
		//else if (WrapMode == WM_Black)
		//	Img->SamplePixelB(x, y, p);

		out.R = p.R;	// OPT: we can do this faster
		out.G = p.G;
		out.B = p.B;
		out.A = p.A;
	}

	*block.Color = out;
}






//
//
// GLTexture3D
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass GLTexture
#define ThisClass GLTexture3D

FuRegisterClass(CLSID_GLSampleTexture3D, CT_GLTexture)
	REGS_Name,					"GLSampleTexture3D",
	TAG_DONE);

GLTexture3D::GLTexture3D()
{
	m_RegNode = &Reg;
	GLTexTarget = GL_TEXTURE_3D;
	TempImage = NULL;

	// Once this resource is registered the resource manager is free to call FreeData() any time that this is resource
	// is not activated.
	ResourceMgr.RegisterResource(this);
}

GLTexture3D::GLTexture3D(const Registry *reg, const ScriptVal &table, const TagList &tags)// : BaseClass(reg, table, tags)
{
	GLTexTarget = GL_TEXTURE_3D;
	TempImage = NULL;
	ResourceMgr.RegisterResource(this);
}

GLTexture3D::~GLTexture3D()
{
	FreeData();

	if (MyManager)
		MyManager->UnregisterResource(this);
}

bool GLTexture3D::BeginUpload(uint32 width, uint32 height, uint32 depth)
{
	// Multiple threads could be accessing this texture at once so we need to surround our uploading activity by a lock.
	// Without the lock, more than one thread could be attempting to upload the same texture simultaneously.
	UploadLock.ObtainLock();

	// If the texture is already uploaded then we don't need to upload it again.
	if (vIsUploaded)
	{
		Activate();			// once this texture is activated we can be guarenteed the ResourceMgr won't free it up on us
		UploadLock.ReleaseLock();
		return false;
	}

	Width = width;
	Height = height;
	Depth = depth;

	// do we want to do something here like checking we're not creating a 3D texture which is too large to fit into memory?
	//uint64 mem = ResourceMgr.TotalCardMemory;		// in kbytes

	NumTotChannels = 4;
	TexDataType = F3D_UINT8;

	BitsPerChannel = 8 * F3D_DataType_Size[TexDataType];
	TexMemoryKbytes = (F3D_DataType_Size[TexDataType] * NumTotChannels * width * height * depth) >> 10;

	// give the resource manager a chance to free up memory
	if (NotifyOnPreAlloc())
		MyManager->ResourcePreAlloc(this, TexMemoryKbytes);

	// note we do not use glGenTextures() since it is not thread safe
	fuGenTextures(1, &GLTexName);

	// create the gl texture object here - we'll glTexSubImage the slices into it one at a time
	glBindTexture(GLTexTarget, GLTexName);
	glTexImage3D(GLTexTarget, 0, GL_RGBA8, width, height, depth, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, 0);

	// set up our source data alignment
	glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

	// create an 8bit temporary image into which we'll be copying/converting the source image data
	TempImage = NewImage(
		IMG_Width,			width,
		IMG_Height,			height,
		IMG_Channel,		CHAN_BLUE,
		IMG_Channel,		CHAN_GREEN,
		IMG_Channel,		CHAN_RED,
		IMG_Channel,		CHAN_ALPHA,
		IMG_Depth,			IMDP_8bitInt,
		TAG_DONE);

	return true;
}

Image *GLTexture3D::ResizeImage(Image *img)
{
	Image *ret = NULL;

	if (img->Width != Width || img->Height != Height)
	{
		ret = img->ResizeOfImage(
			RSZ_Width,		Width,
			RSZ_Height,		Height,
			RSZ_Filter,		RSZ_Gaussian,
			RSZ_EdgeMode,	EM_Duplicate,
			TAG_DONE);
	}
	else
	{
		img->Use();
		ret = img;
	}

	return ret;
}

void GLTexture3D::ConvertTo8Bit(Image *dst, Image *src)
{
	SimplePixPtr pSrc(src);
	SimplePixPtr pDst(dst);

	Pixel p;

	for (int y = 0; y < dst->Height; y++)
	{
		pSrc.GotoXY(0, y);
		pDst.GotoXY(0, y);

		for (int x = 0; x < dst->Width; x++)
		{
			p >>= pSrc;
			pDst >>= p;
		}
	}
}

void GLTexture3D::UploadSlice(int i, Image *img)
{
	CHECK_GL_ERRORS();

	if (img)
	{
		// All the images are likely to be the same size but the user could be doing something unusual like animating the width/height
		// so we need to be sure.
		Image *resizedImg = ResizeImage(img);

		// Convert the image data to 8bit.
		ConvertTo8Bit(TempImage, resizedImg);
		resizedImg->Recycle();
	}
	else
	{
		// upload an empty slice - we do this by clearing an image to 0s and uploading it (it could probably be done more efficiently
		// by using RTT with a FBO and glClear())
		TempImage->Clear();
	}

	// Note that we're not taking into account img->DataWindow here.  The reason is that Texture3DTool is not flagged as DoD aware so
	// any images passed into it will get padded up by the canvas color.  Also note that the image data may be stored non-contiguously in
	// memory.  I've seen several plugin authors who have made the mistake of assuming Fusions images are always contiguous.  When Fusion
	// is started, memory is non-fragmented and images will be contiguous however after working with Fusion for a while address space
	// will become fragmented.
	if (TempImage->IsContiguous())
	{
		glTexSubImage3D(GLTexTarget, 0, 0, 0, i, TempImage->Width, TempImage->Height, 1, 
			GL_BGRA_EXT, GL_UNSIGNED_BYTE, TempImage->ScanLine[0]);
	}
	else
	{
		int a = 0, b = 0;

		while (b != TempImage->Height)
		{
			b++;
			while (b != TempImage->Height && int_ptr(TempImage->ScanLine[b]) - int_ptr(TempImage->ScanLine[b - 1]) == TempImage->RowModulo)
				b++;

			glTexSubImage3D(GLTexTarget, 0, 0, a, i, TempImage->Width, b - a, 1,
				GL_BGRA_EXT, GL_UNSIGNED_BYTE, TempImage->ScanLine[a]);					// upload scanlines [a, b)

			a = b;
		}			
	}

	CHECK_GL_ERRORS();
}

void GLTexture3D::EndUpload()
{
	glPopClientAttrib();

	SAFE_RECYCLE(TempImage);

	// inform the resource manager that we are done allocating
	if (NotifyOnPostAlloc())
		MyManager->ResourcePostAlloc(this, TexMemoryKbytes);

	// if we didn't activate the resource here, when the UploadLock is released the ResourceMgr could immediately free up the 
	// texture we just uploaded if it felt like it
	Activate();

	// flag this resource as uploaded
	vIsUploaded = true;

	UploadLock.ReleaseLock();
}

void GLTexture3D::FreeData(TagList *tags)
{
	if (GLTexName != 0)
	{
		// Use the current context if there is one, otherwise use the ResourceMgr context
		HGLRC hCurrentRC = wglGetCurrentContext();
		bool restoreContext = false;
		if (hCurrentRC == 0)
		{
			ResourceMgr.ObtainContext();		// what do we do if this fails?
			restoreContext = true;
		}

		#ifdef _DEBUG
			uassert(glIsTexture(GLTexName));
			dprintf(("Deleted texture %i, context %i, target %i", GLTexName, wglGetCurrentContext(), GLTexTarget));
			HasBeenBound = false;
		#endif

		fuDeleteTextures(1, &GLTexName);

		//if (NotifyOnDestruction())
			MyManager->ResourceDestroyed(this);

		CHECK_GL_ERRORS();	// deleting a non-texture doesn't cause any errors interestingly

		GLTexName = 0;

		if (restoreContext)
			ResourceMgr.ReleaseContext();

		ASSERT(hCurrentRC == wglGetCurrentContext());
	}
}
