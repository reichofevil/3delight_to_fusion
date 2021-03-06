//@ Project "Fu3D"

#include "stdafx.h"

#define MODULENAME "3DRIB"

#define BaseClass Object
#define ThisClass Ri3D

#ifdef FuPLUGIN
#include "FuPlugin.h"		// make sure you change the macro COMPANY_ID in FuPlugin.h which defaults to "samples"
#endif

#include <time.h>
#include <direct.h>
#include <stdlib.h>

#include "3D_RIB.h"
#include "3D_Node.h"
#include "3D_Scene.h"
#include "3D_Datatypes.h"
#include "3D_CameraBase.h"
#include "3D_SurfaceBase.h"
#include "3D_TextureImage.h"
#include "3D_MaterialConstant.h"



//#include "3D_MaterialBlinn.h"
#include "3D_MaterialPhong.h"
#include "3D_MaterialWard.h"
#include "3D_MaterialFog.h"

//include my plugins here
#include "3D_MaterialCook.h" 
#include "3D_MaterialGlass.h"
#include "3D_LightDirectional.h"
#include "3D_SurfacePlane.h"
// end of my plugins


#include "FileIO.h"
#include "ImageFormat.h"
#include "Geometry3D.h"
#include "PipelineDoc.h"
#include "PipelineDoc.h"





/**
 * This file contains two classes:
 *
 * RendererRIB3D:  This is a plugin renderer that will appear as "RIB Renderer" along with the GL/SW renderers in the 
 *						 Renderer3D.RendererType combobox.  It works by directly loading the renderman dll and issuing commands directly to 
 *						 the dll (be warned:  some renderers that implement the Renderman binary interface are kind of crashy when you use 
 *                 them this way).  The resulting image is written to disk and then read back into memory as a Fusion Image.  This can 
 *                 renderer handles cameras, lights, geometry, and some simple materials.
 *
 *                 This plugin renderer is intended as an SDK example and not for use in Fusion as a renderer.  In particular, it does 
 *                 not replicate all the functionality of the 3D system (eg. materials) and its needs a lot more testing/bugfixing.
 *
 * ExporterRIB3D:  (Not done) This is a plugin exporter tool.  You can use it to output cameras, lights, geometry, and simple materials 
 *                 into a RIB file.
 *
 * Gotchas:  
 *  - One thing to be aware of that can cause the RIB Renderer to fail is your system path.  RendererRIB3D will be able to find render.dll
 *    because you gave it the path explicitly (from InRender) but often the render.dll will depend upon other DLLS and files which need the 
 *    proper path set in Windows in order to work.   
 *  - For example, for Pixar's PRMan you would need to add "C:\Program Files (x86)\Pixar\RenderManProServer-15.1\bin" to your path.  Or
 *    use the RMANTREE environment variable that gets setup during PRMan install to accomplish the same thing.
 *  - The name of the render.dll varies based upon the renderer.  For example:
 *       PRMan    -- C:\Program Files (x86)\Pixar\RenderManProServer-15.1\lib\libprman.dll
 *       3Delight -- C:\Program Files (x86)\3Delight\bin\3Delight.dll
 *  - If the plugin still does not work, make sure you can render a simple RIB from the command line renderer.  If this fails it might indicate
 *    your license server is setup incorrectly.
 *  - Its probably better to write interfaces to individual renderers rather than trying to write a general one that interfaces all renderman
 *    compliant renderers unless you have a lot of time on your hand.  The problem is that each renderer seems to have its own gotchas/idiosyncrasies 
 *    when using the DLL directly rather than going through RIB files that you end up writing/testing a lot of code switching on the renderer type 
 *    (PRMan/3Delight/Aqsis/Pixie/BMRT/etc).  Plus there is the fact that each renderer will have its own extensions.
 *  - We have not done any testing with the RIB renderer under linux.  It may work "out of the box" but it may also require some tweaks for
 *    linux style paths.
 *
 *  TODO
 *    -move spotlight angles dropoff into base class
 *    -instancing geometry
 *    -images, 
 *    -curves
 *    -surface colors
 *    -render to RIB file
 */

enum PF_FilterType
{
	PF_Box,				// 0
	PF_Triangle,		// 1
	PF_CatmullRom,
	PF_Gaussian,
	PF_Sinc,
	PF_NumFilters
};

static const int PF_DefaultFilterIndex = PF_Gaussian;

static const char * const PF_Name[PF_NumFilters] =
{
	"Box",
	"Bi-Linear (triangle)",
	"Catmull-Rom",
	"Gaussian",
	"Sinc",
};

static const FuID PF_FuID[PF_NumFilters] = 
{ 
	"Box", 
	"Triangle", 
	"CatmullRom", 
	"Gaussian", 
	"Sinc", 
};

static double PF_DefaultWidth[PF_NumFilters] =
{
	1.0,
	2.0,		// TODO - are these right, compare with GL renderer
	4.0,
	1.0,
	8.0
};

enum RenRIB_SupportedChannels
{
   RenRIB_Chan_RGBA = 0,
   RenRIB_Chan_Z,							// TODO
   RenRIB_NumSupportedChannels
};

RendererChannel_t RenRIB_SupportedChannelEnum[] =
{
   F3D_Chan_RGBA,
   F3D_Chan_Z,
};

const FuID RenRIB_SupportedChannelFuID[] =
{
   RendererChannelFuID[F3D_Chan_RGBA],
   RendererChannelFuID[F3D_Chan_Z],
};

// Texture files may be supplied to Renderman directly (eg. as TIFFs) or you can call RiMakeTexture() to convert the TIFF to a 
// Renderman TX texture file which contains prefiltered mips. Note that when RiMakeTexture() is called on some renderers a command line 
// window is popped up and if we don't call RiMakeTexture() then the renderer complains about inefficiencies/quality in Fusions console. 
// PRMan will refuse to directly receive TIFF files and this variable must be set to true.  If set to false, PRMan gives errors like
// "*err* T02002 "Tex_00004c38_140708B8_00000002.tiff" is not a texture file."
static bool UseRiMakeTexture = true;


//static Node3D *FindAnyCamera(Scene3D *scene)
//{
//   SceneItor3D itor;
//   Node3D *node = itor.BeginTraversal(scene);
//
//   while (node)
//   {
//      if (node->NodeType == NDET_Camera && node->Data)
//         return node;
//      node = itor.NextNode();
//   }
//
//   return NULL;
//}

static const char *GetFilename(const char *pathfile)
{
   const char *ret = NULL;

   if (pathfile)
   {
      const char *f = pathfile;
      while (*f != NULL)
         f++;

      while (1)
      {
         char c = *f;
         if (c == '\\' || c == '/' || c == ':')
         {
            f++;
            break;
         }
         if (f == pathfile)
            break;
         f--;
      }

      if (*f != NULL)
         ret = f;
   }

   return ret;
}







//
//
// RendererRIB3D - a plugin RIB renderer for Fusion
//  When writing a renderer for Fusion there are two possible approaches you can take.  You can either embed your renderer into the Renderer3D
//  tool or you can you create your own tool.  This example shows how to create a renderer that embeds itself into the Renderer3D tool.  When
//  you start Fusion you will see a "RIB Renderer" entry in the Renderer3D.RendererType combobox.
//
//  Embedded plugin renderers must inherit RendererBase3D and must declare their registry type to be CT_Renderer3D.  When a Renderer3D tool is
//  created it searches Fusions class registry of any classes of type CT_Renderer3D and instantiates them.  The Renderer3D tool does not do any
//  rendering itself, it is the RendererSW3D and RendererGL3D subclasses of RendererBase3D which contain all the rendering logic for the Software
//  and OpenGL renderers.  The purpose of Renderer3D is to present a common set of base inputs and functionality to the user - its really nothing 
//  more than a shell with some inputs common to all renderers.
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass RendererBase3D
#define ThisClass RendererRIB3D
FuRegisterClass(COMPANY_ID_DOT + CLSID_Renderer_RIB, CT_Renderer3D)
   REGS_Name,					"3Delight",	// this is the name that appears in the Renderer3D.RendererType combobox	
   REGS_OpDescription,		"A RIB Renderer",
	REG_Hide,					FALSE,				// this can be set to true to prevent your renderer from showing up in Renderer3D.RendererType
   REG_SupportsDoD,			TRUE,		// Indicates that this tool supports DoD/RoI,
   REG_NoMotionBlurCtrls, TRUE,

	TAG_DONE);

RendererRIB3D::RendererRIB3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
   InExecutablesNest = InRender = InTexMake = InShader = NULL;

   rm = NULL;
   RenderFilename = NULL;
   OutSlot = NULL;
   Scene = NULL;
	Document = NULL;
   RenderDLL = 0;
	NodeLUT = NULL;
	LightList = NULL;
	SurfaceList = NULL;
	IsPRMan = false;

	RenderTiff = NULL;

	// "Temp:" is a pathmap defined by Fusion to point to the users temporary directory
	strcpy(TempPath, "Temp:");
	MapPath(TempPath);					// this will convert it in place to something like "C:\Users\stephen\AppData\Local\Temp\"
}

RendererRIB3D::~RendererRIB3D()
{
}

inline void RendererRIB3D::CheckAbort()
{
	// In order to maintain UI interactivity its good to check frequently to see if the user aborted rendering.  For example, if the 
	// user is dragging a slider on a material/geometry tool upstream of the renderer RendererRIB3D::ProcessTagList() will get called
	// a lot.
	// TODO - if your renderer is a "black box" it might be possible also to pass in a pointer to a variable to poll
   if (Document->IsAborting())
      throw FuSilentException3D("user abort");
}


bool RendererRIB3D::AddInputsTagList(TagList &tags)
{
   // Here we'll attach the inputs associated with our renderer.  Renderer3D will call AddInputs() on RendererRIB3D when it needs to 
	// show these inputs.

   AddControlPage("Controls");

	// Executables nest
   InExecutablesNest = BeginControlNest("Executables", "Executables", true);

      InRender = AddInput("3delight dll", "Render", 
         LINKID_DataType,		CLSID_DataType_Text,
         INPID_InputControl,	FILECONTROL_ID,
         INP_Required,			FALSE,
         INP_External,			FALSE,
         INP_DoNotifyChanged,	TRUE,
         FCS_FilterString,		"DLL Files (*.dll)|*.dll",		
         TAG_DONE);

      InTexMake = AddInput("tdlmake exe", "TexMake", 
         LINKID_DataType,		CLSID_DataType_Text,
         INPID_InputControl,	FILECONTROL_ID,
         INP_Required,			FALSE,
         INP_External,			FALSE,
         INP_DoNotifyChanged,	TRUE,
         FCS_FilterString,		"EXE Files (*.exe)|*.exe",		
         TAG_DONE);

      InShader = AddInput("shaderdl exe", "Shader",			// TODO - remove extra inputs here
         LINKID_DataType,		CLSID_DataType_Text,
         INPID_InputControl,	FILECONTROL_ID,
         INP_Required,			FALSE,
         INP_External,			FALSE,
         INP_DoNotifyChanged,	TRUE,
         FCS_FilterString,		"EXE Files (*.exe)|*.exe",		
         TAG_DONE);

	  InShaderPath = AddInput("shader search path", "ShaderPath",			// TODO - remove extra inputs here
         LINKID_DataType,		CLSID_DataType_Text,
         INPID_InputControl,	FILECONTROL_ID,
         INP_Required,			FALSE,
         INP_External,			FALSE,
         INP_DoNotifyChanged,	TRUE,
		 FC_PathBrowse,			TRUE,
         //FCS_FilterString,		"EXE Files (*.exe)|*.exe",		
         TAG_DONE);

	  InProcPath = AddInput("procedural search path", "ProcPath",			// TODO - remove extra inputs here
         LINKID_DataType,		CLSID_DataType_Text,
         INPID_InputControl,	FILECONTROL_ID,
         INP_Required,			FALSE,
         INP_External,			FALSE,
         INP_DoNotifyChanged,	TRUE,
		 FC_PathBrowse,			TRUE,
         //FCS_FilterString,		"EXE Files (*.exe)|*.exe",		
         TAG_DONE);

   EndControlNest();


	// Sampling nest
	InSamplingNest = BeginControlNest("Sampling", "Sampling", false);

	InLockXYPixelSamples = AddInput("Lock X/Y Pixel Samples", "LockXYPixelSamples",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	CHECKBOXCONTROL_ID,
		INP_MinAllowed,		0.0,
		INP_MaxAllowed,		1.0,
		INP_Default,			1.0,
		INP_External,			FALSE,			// this makes this input not animatable
		INP_DoNotifyChanged,	TRUE,
		TAG_DONE);

	InPixelSamplesX = AddInput("Pixel Samples X", "PixelSamplesX",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			TRUE,
		INP_MinAllowed,		1.0,
		INP_MinScale,			1.0,
		INP_MaxScale,			16.0,
		INP_Default,			2.0,
		TAG_DONE);

	InPixelSamplesY = AddInput("Pixel Samples Y", "PixelSamplesY",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			TRUE,
		INP_MinAllowed,		1.0,
		INP_MinScale,			1.0,
		INP_MaxScale,			16.0,
		INP_Default,			2.0,
		TAG_DONE);

	if (*(Number *)InLockXYPixelSamples->GetSource(TIME_UNDEFINED) < 0.5)	// this only works because InLockXYSamples is not animatable
		InPixelSamplesY->HideInputControls();

	TagList rfTags(
		LINKID_DataType,			CLSID_DataType_FuID,
		INPID_InputControl,		COMBOIDCONTROL_ID,
		INPID_DefaultID,			PF_FuID[PF_DefaultFilterIndex],
		INP_DoNotifyChanged,		TRUE,
		CC_LabelPosition,			CCLP_Vertical,
		ICD_Width,					0.5,
		TAG_DONE);

	for (int i = 0; i < PF_NumFilters; i++)
	{
		rfTags.Add(CCS_AddString, PF_Name[i]);
		rfTags.Add(CCID_AddID, PF_FuID[i]);
	}

	InPixelFilter = AddInputTagList("Pixel Filter", "PixelFilter", rfTags); 

	InLockXYFilterSamples = AddInput("Lock X/Y Filter Samples", "LockXYFilterSamples",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	CHECKBOXCONTROL_ID,
		INP_MinAllowed,		0.0,
		INP_MaxAllowed,		1.0,
		INP_Default,			1.0,
		INP_External,			FALSE,			// this makes this input not animatable
		INP_DoNotifyChanged,	TRUE,
		TAG_DONE);

	InFilterSamplesX = AddInput("Filter Samples X", "FilterSamplesX",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			TRUE,
		INP_MinAllowed,		1.0,
		INP_MinScale,			1.0,
		INP_MaxScale,			16.0,
		INP_Default,			2.0,
		TAG_DONE);

	InFilterSamplesY = AddInput("Filter Samples Y", "FilterSamplesY",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			TRUE,
		INP_MinAllowed,		1.0,
		INP_MinScale,			1.0,
		INP_MaxScale,			16.0,
		INP_Default,			2.0,
		TAG_DONE);

	if (*(Number *)InLockXYFilterSamples->GetSource(TIME_UNDEFINED) < 0.5)	// this only works because InLockXYSamples is not animatable
		InFilterSamplesY->HideInputControls();

	InRaySamples = AddInput("Raytrace Samples", "RaySamples",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			TRUE,
		INP_MinAllowed,		1.0,
		INP_MinScale,			1.0,
		INP_MaxScale,			64.0,
		INP_Default,			8.0,
		TAG_DONE);

	EndControlNest();


	// Options Nest
	InOptionsNest = BeginControlNest("Options", "Options", false);

	InRaytraceAll = AddInput("use Raytracing instead of REYES", "EnableRaytrace",
			LINKID_DataType,		CLSID_DataType_Number,
			INPID_InputControl,	CHECKBOXCONTROL_ID,
			INP_MinAllowed,		0.0,
			INP_MaxAllowed,		1.0,
			INP_Default,			1.0,
			TAG_DONE);

	InProgCons = AddInput("output progress to console", "EnableProgCons",
			LINKID_DataType,		CLSID_DataType_Number,
			INPID_InputControl,	CHECKBOXCONTROL_ID,
			INP_MinAllowed,		0.0,
			INP_MaxAllowed,		1.0,
			INP_Default,			0.0,
			TAG_DONE);

	EndControlNest();
	
	InDofNest = BeginControlNest("Depth of Field", "DoF", false);

	InEnableDepthOfField = AddInput("Depth of Field", "EnableDepthOfField",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	CHECKBOXCONTROL_ID,
		INP_MinAllowed,		0.0,
		INP_MaxAllowed,		1.0,
		INP_Default,			0.0,
		ICD_Width,				1.0,
		TAG_DONE);

	InFStop = AddInput("f-stop", "FStop",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_MinAllowed,		0.1,
		INP_MinScale,			1.0,
		INP_MaxScale,			128.0,
		INP_Default,			8.0,
		ICD_Width,				1.0,
		TAG_DONE);

	InDofSides = AddInput("aperture blades", "DoFSides",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_MinAllowed,		1.0,
		INP_MinScale,			2.0,
		INP_MaxScale,			8.0,
		INP_Default,			2.0,
		//ICD_Width,				1.0,
		TAG_DONE);

	InDofRoundness = AddInput("aperture roundness", "DoFRound",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_MinAllowed,		0.0,
		INP_MinScale,			0.0,
		INP_MaxScale,			1.0,
		INP_Default,			0.0,
		//ICD_Width,				1.0,
		TAG_DONE);
	
	InDofAngle = AddInput("aperture angle", "DoFAngle",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_MinAllowed,		0.0,
		INP_MinScale,			0.0,
		INP_MaxScale,			360.0,
		INP_Default,			0.0,
		//ICD_Width,				1.0,
		TAG_DONE);

	InDofDensity = AddInput("aperture density", "DoFDensity",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_MinAllowed,		-1.0,
		INP_MaxAllowed,		1.0,
		INP_MinScale,			-1.0,
		INP_MaxScale,			1.0,
		INP_Default,			0.0,
		//ICD_Width,				1.0,
		TAG_DONE);

	EndControlNest();


	// Lighting Nest
   InLightingNest = BeginControlNest("Lighting", "Lighting", false);

   // Note that we use InLightingEnabled/InShadowsEnabled where are declared in RendererBase3D.  If we reuse these existing inputs, it
	// allows Renderer3D to remember Lighting/Shadows enabled settings when the user switches between renderer types.
   InLightingEnabled = AddInput("Enable Lighting", "LightingEnabled",
      LINKID_DataType,		CLSID_DataType_Number,
      INPID_InputControl,	CHECKBOXCONTROL_ID,
      INP_Default,			0.0,
      ICD_Width,				0.5,
      TAG_DONE);

   InShadowsEnabled = AddInput("Enable Shadows", "ShadowsEnabled",
      LINKID_DataType,		CLSID_DataType_Number,
      INPID_InputControl,	CHECKBOXCONTROL_ID,
      INP_Default,			0.0,
      ICD_Width,				0.5,
      TAG_DONE);

   EndControlNest();

   // ShadingRate Nest
   InShadeRateNest = BeginControlNest("Shading Rate", "ShadeRate", false);
   InShadRate = AddInput("ShadingRate", "ShadingRate",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			FALSE,
		INP_MinAllowed,		0.001,
		INP_MinScale,			1.0,
		INP_MaxScale,			20.0,
		INP_Default,			2.0,
		TAG_DONE);

   EndControlNest();

      // MoBlur Nest
   InMoBlurNest = BeginControlNest("MotionBlur Settings", "MoBlur", false);
	InEnableMoBlur = AddInput("MotionBlur", "EnableMoBlur",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	CHECKBOXCONTROL_ID,
		INP_MinAllowed,		0.0,
		INP_MaxAllowed,		1.0,
		INP_Default,			0.0,
		ICD_Width,				1.0,
		TAG_DONE);

	InEnableDefMoBlur = AddInput("DefMotionBlur", "EnableDefMoBlur",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	CHECKBOXCONTROL_ID,
		INP_MinAllowed,		0.0,
		INP_MaxAllowed,		1.0,
		INP_Default,			0.0,
		ICD_Width,				1.0,
		TAG_DONE);


   InMoBlurSteps = AddInput("Time Samples", "MBSamples",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			TRUE,
		INP_MinAllowed,		1.0,
		INP_MinScale,			1.0,
		INP_MaxScale,			6.0,
		INP_Default,			4.0,
		TAG_DONE);

   InMoBlurOpen = AddInput("Shutter Open", "MBOpen",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			FALSE,
		INP_MinAllowed,		0.0,
		INP_MinScale,			0.0,
		INP_MaxScale,			1.0,
		INP_Default,			0.0,
		TAG_DONE);

   InMoBlurClose = AddInput("Shutter Close", "MBClose",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			FALSE,
		INP_MinAllowed,		0.0,
		INP_MinScale,			0.0,
		INP_MaxScale,			1.0,
		INP_Default,			0.5,
		TAG_DONE);
    
   InMoBlurStart = AddInput("Shutter Efficieny Start", "MBStart",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			FALSE,
		INP_MinAllowed,		0.0,
		INP_MinScale,			0.0,
		INP_MaxScale,			1.0,
		INP_Default,			0.7,
		TAG_DONE);

   InMoBlurEnd = AddInput("Shutter Efficieny End", "MBEnd",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			FALSE,
		INP_MinAllowed,		0.0,
		INP_MinScale,			0.0,
		INP_MaxScale,			1.0,
		INP_Default,			0.7,
		TAG_DONE);

   EndControlNest();

      // AO Nest
   InAONest = BeginControlNest("Ambient Occlusion Settings", "AO", false);
	InEnableAO = AddInput("enable Ambient Occlusion", "EnableAO",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	CHECKBOXCONTROL_ID,
		INP_MinAllowed,		0.0,
		INP_MaxAllowed,		1.0,
		INP_Default,			0.0,
		ICD_Width,				0.6,
		TAG_DONE);

   InAOSamples = AddInput("AO Samples", "AOSamples",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			TRUE,
		INP_MinAllowed,		1.0,
		INP_MinScale,			1.0,
		INP_MaxScale,			1024.0,
		INP_Default,			32.0,
		TAG_DONE);

   InAODepth = AddInput("max distance", "AODepth",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			TRUE,
		INP_MinAllowed,		1.0,
		INP_MinScale,			1.0,
		INP_MaxScale,			1000.0,
		INP_Default,			100.0,
		TAG_DONE);

   EndControlNest();

   	InEnvNest = BeginControlNest("Enviroment (LatLong Map)", "Env", false);
		InEnvMap = AddInput("env map", "env_map", 
			LINKID_DataType,		CLSID_DataType_Text,
			INPID_InputControl,	FILECONTROL_ID,
			INP_Required,			FALSE,
			INP_External,			FALSE,
			INP_DoNotifyChanged,	TRUE,
			FCS_FilterString,		"hdr (*.hdr)|*.hdr",		
			TAG_DONE);
	EndControlNest();

	/*InPhotonNest = BeginControlNest("Photons", "Pho", false);
		InEnablePhotons = AddInput("enable Photons(and do finalgather)", "EnablePho",
			LINKID_DataType,		CLSID_DataType_Number,
			INPID_InputControl,	CHECKBOXCONTROL_ID,
			INP_MinAllowed,		0.0,
			INP_MaxAllowed,		1.0,
			INP_Default,			0.0,
			ICD_Width,				1.0,
			TAG_DONE);
		InPhotonSamples = AddInput("Photon Count (total)", "PhoSamples",
			LINKID_DataType,		CLSID_DataType_Number,
			INPID_InputControl,	SLIDERCONTROL_ID,
			INP_Integer,			TRUE,
			INP_MinAllowed,		1.0,
			INP_MinScale,			1000.0,
			INP_MaxScale,			1000000.0,
			INP_Default,			10000.0,
			TAG_DONE);
		InPhotonStrength = AddInput("Finalgather Multiplier", "PhoStrength",
			LINKID_DataType,		CLSID_DataType_Number,
			INPID_InputControl,	SLIDERCONTROL_ID,
			INP_MinAllowed,		0.0,
			INP_MinScale,			0.0,
			INP_MaxScale,			2.0,
			INP_Default,			1.0,
			TAG_DONE);
		InPhotonDist = AddInput("max. distance (0 = unlimited)", "PhoDistance",
			LINKID_DataType,		CLSID_DataType_Number,
			INPID_InputControl,	SLIDERCONTROL_ID,
			INP_MinAllowed,		0.0,
			INP_MinScale,			0.0,
			INP_MaxScale,			100.0,
			INP_Default,			0.0,
			TAG_DONE);
		InPhotonBounce = AddInput("Photon Bounces", "PhoBounce",
			LINKID_DataType,		CLSID_DataType_Number,
			INPID_InputControl,	SLIDERCONTROL_ID,
			INP_MinAllowed,		1.0,
			INP_MinScale,			1.0,
			INP_MaxScale,			10.0,
			INP_Default,			5.0,
			TAG_DONE);
		InPhotonRays = AddInput("Photon Rays", "PhoRays",
			LINKID_DataType,		CLSID_DataType_Number,
			INPID_InputControl,	SLIDERCONTROL_ID,
			INP_MinAllowed,		1.0,
			INP_MinScale,			1.0,
			INP_MaxScale,			256.0,
			INP_Default,			64.0,
			TAG_DONE);
	EndControlNest();*/

	InExportNest = BeginControlNest("Export RIB (do not render)", "Export", false);
		InEnableExport = AddInput("do Export", "EnableExport",
			LINKID_DataType,		CLSID_DataType_Number,
			INPID_InputControl,	CHECKBOXCONTROL_ID,
			INP_MinAllowed,		0.0,
			INP_MaxAllowed,		1.0,
			INP_Default,			0.0,
			ICD_Width,				0.6,
			TAG_DONE);

		InExportFile = AddInput("RIB file", "rib_path", 
			LINKID_DataType,		CLSID_DataType_Text,
			INPID_InputControl,	FILECONTROL_ID,
			INP_Required,			FALSE,
			INP_External,			FALSE,
			INP_DoNotifyChanged,	TRUE,
			FCS_FilterString,		"rib (*.rib)|*.rib",
			FC_IsSaver,				TRUE,
			TAG_DONE);	
		
	EndControlNest();

	#ifdef MULTI_OUT
		OutCaustics = Owner->AddOutput("Caustics", "Caustics",
			LINKID_DataType,		CLSID_DataType_Image,
			LINK_Main,				2,			// outputs MUST not have conflicting LINK_Main values, Renderer3D adds the main color output as LINK_Main = 1
			TAG_DONE);
	#endif

   // TODO
   // for now we put the rib, the textures and shaders all into the same directory for all frames
   // but one could imagine that the user might want
      // * all the data associated with a particular time in its own folder
      // * all the shaders in their own folder, all the textures in their own folder, all the ribs in their own folder
      // * textures could be output as tiffs or compiled to a texture forma

   //InCamera = combobox

   BaseClass::AddInputsTagList(tags);

   return true;
}

void RendererRIB3D::ShowInputs(bool visible)
{
	// Show inputs gets called by the Renderer3D tool when switching between renderer types.
   if (visible)
   {
      InExecutablesNest->ShowInputControls();
      InRender->ShowInputControls();
      InTexMake->ShowInputControls();
      InShader->ShowInputControls();
	  InShaderPath->ShowInputControls();
	  InProcPath->ShowInputControls();

		InLightingNest->ShowInputControls();
		InLightingEnabled->ShowInputControls();
		InShadowsEnabled->ShowInputControls();

		InSamplingNest->ShowInputControls();
		InLockXYPixelSamples->ShowInputControls();
		InPixelSamplesX->ShowInputControls();			// TODO - do we need special handling for these???
		InPixelSamplesY->ShowInputControls();
		InPixelFilter->ShowInputControls();
		InLockXYFilterSamples->ShowInputControls();
		InFilterSamplesX->ShowInputControls();			// TODO - do we need special handling for these???
		InFilterSamplesY->ShowInputControls();
		InRaySamples->ShowInputControls();
		InShadeRateNest->ShowInputControls();
		InShadRate->ShowInputControls();
		InMoBlurNest->ShowInputControls();
		InEnableMoBlur->ShowInputControls();
		InEnableDefMoBlur->ShowInputControls();
		InMoBlurSteps->ShowInputControls();
		InMoBlurOpen->ShowInputControls();
		InMoBlurClose->ShowInputControls();
		InMoBlurStart->ShowInputControls();
		InMoBlurEnd->ShowInputControls();
		InAONest->ShowInputControls();
		InEnableAO->ShowInputControls();
		InAOSamples->ShowInputControls();
		InAODepth->ShowInputControls();
		InEnvNest->ShowInputControls();
		InEnvMap->ShowInputControls();
		/*InPhotonNest->ShowInputControls();
		InPhotonSamples->ShowInputControls();
		InPhotonStrength->ShowInputControls();
		InPhotonBounce->ShowInputControls();
		InPhotonDist->ShowInputControls();
		InPhotonRays->ShowInputControls();
		InEnablePhotons->ShowInputControls();*/
		InExportNest->ShowInputControls();
		InExportFile->ShowInputControls();
		InEnableExport->ShowInputControls();
		InRaytraceAll->ShowInputControls();
		InProgCons->ShowInputControls();
		InDofNest->ShowInputControls();
		InDofSides->ShowInputControls();
		InDofAngle->ShowInputControls();
		InDofDensity->ShowInputControls();
		InDofRoundness->ShowInputControls();


   }
   else
   {
      InExecutablesNest->HideInputControls();
      InRender->HideInputControls();
      InTexMake->HideInputControls();
      InShader->HideInputControls();
	  InShaderPath->HideInputControls();
	  InProcPath->HideInputControls();

		InLightingNest->HideInputControls();
		InLightingEnabled->HideInputControls();
		InShadowsEnabled->HideInputControls();

		InSamplingNest->HideInputControls();
		InLockXYPixelSamples->HideInputControls();
		InPixelSamplesX->HideInputControls();
		InPixelSamplesY->HideInputControls();
		InPixelFilter->HideInputControls();
		InLockXYFilterSamples->HideInputControls();
		InFilterSamplesX->HideInputControls();			// TODO - do we need special handling for these???
		InFilterSamplesY->HideInputControls();
		InRaySamples->HideInputControls();
		InShadeRateNest->HideInputControls();
		InShadRate->HideInputControls();
		InMoBlurNest->HideInputControls();
		InEnableMoBlur->HideInputControls();
		InEnableDefMoBlur->HideInputControls();
		InMoBlurSteps->HideInputControls();
		InMoBlurOpen->HideInputControls();
		InMoBlurClose->HideInputControls();
		InMoBlurStart->HideInputControls();
		InMoBlurEnd->HideInputControls();
		InAONest->HideInputControls();
		InEnableAO->HideInputControls();
		InAOSamples->HideInputControls();
		InAODepth->HideInputControls();
		InEnvNest->HideInputControls();
		InEnvMap->HideInputControls();
		/*InPhotonNest->HideInputControls();
		InPhotonSamples->HideInputControls();
		InPhotonStrength->HideInputControls();
		InPhotonBounce->HideInputControls();
		InPhotonDist->HideInputControls();
		InPhotonRays->HideInputControls();
		InEnablePhotons->HideInputControls();*/
		InExportNest->HideInputControls();
		InExportFile->HideInputControls();
		InEnableExport->HideInputControls();
		InRaytraceAll->HideInputControls();
		InProgCons->HideInputControls();
		InDofNest->HideInputControls();
		InDofSides->HideInputControls();
		InDofAngle->HideInputControls();
		InDofDensity->HideInputControls();
		InDofRoundness->HideInputControls();

   }

	#ifdef MULTI_OUT
		OutCaustics->SetAttrs(LINK_Visible, visible, TAG_DONE);
	#endif

   BaseClass::ShowInputs(visible);
}

void RendererRIB3D::NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags)
{
	if (param)
	{
		if (in == InLockXYPixelSamples)
		{
			if (*((Number *) param) > 0.5)
				InPixelSamplesY->HideInputControls();
			else
				InPixelSamplesY->ShowInputControls();
		}
	}
	if (param)
	{
		if (in == InLockXYFilterSamples)
		{
			if (*((Number *) param) > 0.5)
				InFilterSamplesY->HideInputControls();
			else
				InFilterSamplesY->ShowInputControls();
		}
	}


	BaseClass::NotifyChanged(in, param, time, tags);
}


bool RendererRIB3D::ProcessTagList(Request *req, const TagList &tags)
{
   if (!ErrorCallback)
   {
      // The Renderer3D tool or view who called this function will have provided an error callback for us to use.  There should always 
      // be one installed, even if its the 'ignore' callback that does nothing
      LogError(RS_FATAL_ERROR, "no error callback installed");
      return false;
   }

   // The job of ProcessTagList() is to read the values from our inputs and store them into rmTags.
   TagList rmTags;


   // framenumber
   int frameNumber = int(req->Time + 0.5);		// TODO - 000n file ext
   rmTags.Add(RenRM_FrameNumber, frameNumber);


   // filename for render.dll
   Text *renderText = (Text *) InRender->GetValue(req);
   if (!renderText)
	{
		LogError(RS_FATAL_ERROR, "render.dll unspecified\n");
      return false;
	}

   const char *renderPathFile = (const char *)*(renderText);
   if (*renderPathFile == 0)
	{
		LogError(RS_FATAL_ERROR, "render.dll unspecified\n");
      return false;
	}

   rmTags.Add(RenRM_RenderFileName, renderPathFile);

    // pathname for shader search
   Text *ShaderPathText = (Text *) InShaderPath->GetValue(req);
   const char *ShaderPathFile = (const char *)*(ShaderPathText);
   rmTags.Add(RenRM_ShaderPathName, ShaderPathFile);
  
   // pathname for procedural search
   Text *ProcPathText = (Text *) InProcPath->GetValue(req);
   const char *ProcPathFile = (const char *)*(ProcPathText);
   rmTags.Add(RenRM_ProcPathName, ProcPathFile);

	// shading rate
	double shadingRate = *InShadRate->GetValue(req);
	//sampling
	double pixelSamplesX = *InPixelSamplesX->GetValue(req);
	double pixelSamplesY = *InPixelSamplesY->GetValue(req);
	
	//raytracing samples
	double RaySamples = *InRaySamples->GetValue(req);
	rmTags.Add(RenRM_RaySamples, RaySamples);
	
	//sampling
	bool lockXYPixelSamples = *InLockXYPixelSamples->GetValue(req) > 0.5;
	if (lockXYPixelSamples)
		pixelSamplesY = pixelSamplesX;

	double FilterSamplesX = *InFilterSamplesX->GetValue(req);
	double FilterSamplesY = *InFilterSamplesY->GetValue(req);
	bool lockXYFilterSamples = *InLockXYFilterSamples->GetValue(req) > 0.5;
	if (lockXYFilterSamples)
		FilterSamplesY = FilterSamplesX;

	rmTags.Add(RenRM_PixelSamplesX, pixelSamplesX);
	rmTags.Add(RenRM_PixelSamplesY, pixelSamplesX);
	rmTags.Add(RenRM_ShadingRate, shadingRate);


	FuID filterID = *(FuIDParam *) InPixelFilter->GetValue(req);
	int filterIdx = GetDFIDIndex(filterID, PF_FuID, PF_NumFilters);
	if (filterIdx == -1)
		filterIdx = PF_Gaussian;

	rmTags.Add(RenRM_PixelFilterIndex, filterIdx);
	rmTags.Add(RenRM_PixelFilterWidth, FilterSamplesX);
	rmTags.Add(RenRM_PixelFilterHeight, FilterSamplesY);


	// depth of field
	bool enableDoF = *InEnableDepthOfField->GetValue(req) > 0.5;
	double fstop = *InFStop->GetValue(req);
	double dof_sides = *InDofSides->GetValue(req);
	double dof_roundness = *InDofRoundness->GetValue(req);
	double dof_angle = *InDofAngle->GetValue(req);
	double dof_density = *InDofDensity->GetValue(req);
	rmTags.Add(Ren_DoDepthOfField, enableDoF);
	rmTags.Add(RenRM_FStop, fstop);
	rmTags.Add(RenRM_DofSides, dof_sides);
	rmTags.Add(RenRM_DofRoundness, dof_roundness);
	rmTags.Add(RenRM_DofAngle, dof_angle);
	rmTags.Add(RenRM_DofDensity, dof_density);

	//raytrace hidder
	bool raytrace_hidder = *InRaytraceAll->GetValue(req) > 0.5;
	rmTags.Add(RenRM_RaytraceAll, raytrace_hidder);

	//progress to console
	bool prog_to_console = *InProgCons->GetValue(req) > 0.5;
	rmTags.Add(RenRM_ProgCons, prog_to_console);


	//MoBlur????
	//bool isMoBlur = DoOwnMotionBlur();
	//rmTags.Add(Ren_DoMotionBlur, isMoBlur);
	bool DoMoBlur = *InEnableMoBlur->GetValue(req) > 0.5;
	bool DoDefMoBlur = *InEnableDefMoBlur->GetValue(req) > 0.5;
	double shutterOpen = *InMoBlurOpen->GetValue(req);
	double shutterClose = *InMoBlurClose->GetValue(req);
	double shutterSamples = *InMoBlurSteps->GetValue(req);
	double shutterStart = *InMoBlurStart->GetValue(req);
	double shutterEnd = *InMoBlurEnd->GetValue(req);
	rmTags.Add(RenRM_DoMoBlur, DoMoBlur);
	rmTags.Add(RenRM_DoDefMoBlur, DoDefMoBlur);
	rmTags.Add(RenRM_shutterOpen, shutterOpen);
	rmTags.Add(RenRM_shutterClose, shutterClose);
	rmTags.Add(RenRM_shutterSamples, shutterSamples);
	rmTags.Add(RenRM_shutterStart, shutterStart);
	rmTags.Add(RenRM_shutterEnd, shutterEnd);
	//Ambient Occlussion
	bool DoAO = *InEnableAO->GetValue(req) > 0.5;
	float AOSamples = *InAOSamples->GetValue(req);
	float AODepth = *InAODepth->GetValue(req);
	rmTags.Add(RenRM_DoAO, DoAO);
	rmTags.Add(RenRM_AOSamples, AOSamples);
	rmTags.Add(RenRM_AODepth, AODepth);
	
	//Enviroment Map
	Text *env_map = (Text *) InEnvMap->GetValue(req);
	const char *envPathFile = (const char *) *(env_map);
	rmTags.Add(RenRM_EnvMap, envPathFile);
	//Photons
	/*bool DoPho = *InEnablePhotons->GetValue(req) > 0.5;
	float PhoSamples = *InPhotonSamples->GetValue(req);
	rmTags.Add(RenRM_DoPho, DoPho);
	rmTags.Add(RenRM_PhoSamples, PhoSamples);
	float PhoStrength = *InPhotonStrength->GetValue(req);
	rmTags.Add(RenRM_PhoStrength, PhoStrength);
	float PhoDist = *InPhotonDist->GetValue(req);
	if (PhoDist == 0) {
		PhoDist = 1.0e36;
	}
	float PhoRays = *InPhotonRays->GetValue(req);
	float PhoBounce = *InPhotonBounce->GetValue(req);
	rmTags.Add(RenRM_PhoDist, PhoDist);
	rmTags.Add(RenRM_PhoBounce, PhoBounce);
	rmTags.Add(RenRM_PhoRays, PhoRays);*/
	//RIB Export
	InDoRIB = *InEnableExport->GetValue(req) > 0.5;
	rmTags.Add(RenRM_DoRIB, InDoRIB);
	Text *rib_file = (Text *) InExportFile->GetValue(req);
	const char *RibFile = (const char *) *(rib_file);
	rmTags.Add(RenRM_RibFile, RibFile);

	//ImageDomain *inROI = req->GetRoI();
	//reqRoI->ValidWindow();
	// Obtain the Region of Interest requested for this render
	const AutoImageRoI reqRoI(req);

	// Obtain the Domain of Definition for the tool connected to InImage
	//const AutoImageDoD inpDoD(req, InImage);

	// The maximum possible amount of data we would need to process from the
	// input Image, is the intersection of the DoD from the upstream connected
	// tool, and the data contained in the actual Image received.
	//FuRectInt dataWindow(inpDoD->ValidWindow & in->DataWindow);

	// If this is a pre-calc render, then we should generate an Image that is the
	// largest possible, given the upstream DoD and the input Image, as the results
	// from this render will be used as the DoD for this tool. Because of this,
	// during the pre-calc, there will be no RoI. We need to check for that. If
	// there is a RoI, then we can restrict our output Image based on that.
	if (reqRoI)
		dataWindow &= reqRoI->ValidWindow;
	

	int32 ROI_left = dataWindow.left;
	int32 ROI_right = dataWindow.right;
	int32 ROI_top = dataWindow.top;
	int32 ROI_bottom = dataWindow.bottom;
	//throw FuException3D("ROI: %d %d %d %d", ROI_left,ROI_top,ROI_right,ROI_bottom);
		
	rmTags.Add(RenRM_ValidWindowleft, ROI_left);
	rmTags.Add(RenRM_ValidWindowright, ROI_right);
	rmTags.Add(RenRM_ValidWindowbottom, ROI_bottom);
	rmTags.Add(RenRM_ValidWindowtop, ROI_top);
	
	
	// merge tags supplied by Renderer3D into ours
   rmTags.SetTags(&tags);	


   
   // pass to RendererBase3D which will read more inputs and store their values in tags and then call RenderTagList()
   return BaseClass::ProcessTagList(req, rmTags);			
}

bool RendererRIB3D::RenderTagList(TagList &tags)
{
	// tags - these are the tags we created and passed to the BaseClass in ProcessTagList().  If you are writing a renderer that is directly
	//   renderable to Fusions views these tags could also have been created by a view.

   rm = NULL;												// assign some variables default values so that if an exception is thrown
   Image *out = NULL;									//   we'll know if we need to clean them up or not
	NodeLUT = NULL;
	LightList = NULL;
	SurfaceList = NULL;
   bool renderOK = true;
	RenderTiff = NULL;
	//bool doOwnMoBlur = DoOwnMotionBlur();
	//bool doOwnROI = SupportsRegionOfInterest();


   try
   {
      ParseRenderAttrs(tags);							// parse the tags into class member variables

		if (PreCalc)
		{
			// In order to support DoD/ROI Fusion issues what are called "PreCalc" renders.  During a PreCalc render you should skip all 
			// your normal processing and output an empty (no data) image but with its width/height/depth/channels set correctly.
			out = CreateOutImage();
		}
		else
		{
			//doOwnROI = true;
			//doOwnMoBlur = true;
			Scene->Setup();								// if you try to Setup() a PreCalc scene it might complain about not having a rootnode

			rm = LoadRenderman();						// load the renderman dll
			RenderScene();									// issue drawing commands to the dll and output a tiff file to disk
			ReleaseRenderman(rm);						// release the renderman dll
			rm = NULL;										// its invalid now, don't try using it after this

			CheckAbort();									// check for user aborts (eg. user dragging a slider on an upstream tool or pressing ESC)
			
			//check if we actual render a image or exporting a RIB file
			//then check for the Image bit depth to read a tiff or EXR file back
			if(InDoRIB==false)
			{
				if ((IFDepth == IFLD_8bitInt)||(IFDepth == IFLD_16bitInt))	
					out = ReadTiffFromDisk(RenderTiff);		// read the tiff from disk into the output image (TODO: use a Renderman display driver to read it directly from memory rather than this write/read process?)
				else 
				{	
					out = ReadEXRFromDisk(RenderTiff);
				}
			}
			else
				out = CreateOutImage();
		}

      *OutSlot = out;

		#ifdef MULTI_OUT
			if (out)
			{
				// We'll output a copy of the color on the caustics output.  If you wanted to output something else you would create
				// an new Image and fill it with your caustic data and set it into the output (without the extra Use()).
				//out->Use();									// because we're reusing the existing color image
				OutCaustics->Set(Req, out);
			}
		#endif
   }
   catch (FuException3D &e)
   {
      if (!e.IsSilent())
		LogError(RS_FATAL_ERROR, e.what());		// print the error message
		renderOK = false;
   }
   catch (CMemoryException *e)
   {
      e->Delete();
      OutOfMemoryError();
      renderOK = false;
   }

	if (!PreCalc)
	{
		if (!renderOK)
		{
			ReleaseRenderman(rm);
			SAFE_RECYCLE(out);
		}

		FreeTempFiles();
		RenderTiff = NULL;
	}

	// One thing you'll notice is that the RIB renderer isn't as responsive as the built in Fusion renderers.  This is because the
	// RIB renderer is not responding to requests for Fusion to abort.  So for example when you drag a slider on some 3D object in the
	// scene this will typically force a large number of rerender requests to be sent but because the RIB renderer cannot abort it has
	// to complete them all.
	// If you are writing a renderer plugin and your renderer does support aborting what you need to do is periodically check Fusions
	// aborting flag and if its true abort your render.
	// TODO - apparently there's a function RiSynchronize() that can be used to interrupt renders? (for autoproxy)

   return renderOK;
}

Image *RendererRIB3D::CreateOutImage()
{
	// This is how you would create a Fusion image.  When doing PreCalc processing its important that you specify all the channels
	// that would be there during normal processing.  For example, if you output a CHAN_Z during normal processing it should be
	// output during PreCalc renders also.
	
	Image *img = NewImage(
		IMG_Channel,		CHAN_RED,
		IMG_Channel,		CHAN_GREEN,
		IMG_Channel,		CHAN_BLUE,
		IMG_Channel,		CHAN_ALPHA,
		IMG_Channel,		CHAN_NORMALX,
		IMG_Channel,		CHAN_NORMALY,
		IMG_Channel,		CHAN_NORMALZ,
		IMG_Channel,		CHAN_Z,
		IMG_Channel,		CHAN_MATERIAL,
		IMG_Width,			OutWidth, 
		IMG_Height,			OutHeight, 
		IMG_Depth,			OutDepth, 
		IMG_XScale,			PixelAspectX, 
		IMG_YScale,			PixelAspectY, 
		IMG_Quality,		HiQ,
		IMG_Document,		Document,
		//IMG_DataWindow,	&dataWindow,
		//IMG_ValidWindow,	reqRoI ? &reqRoI->ValidWindow : NULL,
		//IMG_DataWindow,	&DataWindow,			// this is for more advanced region of interest support that renderers shouldn't have to worry about unless they override SupportsRegionOfInterest() to return true or want to 
		//IMG_ValidWindow,&ValidWindow,			//   ... output an image where they know only a specific subrectangle contains data (as a means of using less memory)
		IMG_NoData,			PreCalc,					// if PreCalc is true, the image is generated but with no scanline buffers allocated -- it is no data
		TAG_DONE);

	/*
	// When writing a plugin renderer you will need to copy the rendered image from your renderers framebuffer/tiles into a Fusion Image.  Images
	// are a type of Parameter that get passed around in the pipes between tools.  As an example what you see belows is how you might write data into 
	// a Fusion image.  Its commented out here because the RIB renderer uses Fusion's TIFF loader to write directly into the image.  For more information 
	// on working with Images you can refer to the 2D image processing examples in the SDK.
	if (!PreCalc)
	{
		// This process can be optimized a bit.
		//  1) Use SimplePixPtr instead of PixPtr if you are writing to an image that contains only RGBA data (and no Aux channels)
		//  2) Use Pixel instead of FltPixel if you are writing to an image with int8/int16 data.
		//
		// It is also possible to write/read data directly from Fusion images scanline buffers however this can be tricky because
		// you will have to handle all the float32/float16/int16/int8 datatype conversions manually and deal with DataWindow/RowModulo
		// issues.  I'm not going to explain it here.

		FltPixel p;					// use FltPixel if you want the images data to be converted to float32 and use Pixel if you want to see it as uint16
		PixPtr pp(img);			// PixPtr is an image iterator class
		 
		for (int32 y = 0; y < OutWidth; y++)
		{
			pp.GotoXY(0, y);
			for (int32 x = 0; x < OutHeight; x++)
			{
				p.R = 0.8f;
				p.G = 0.7f;
				p.B = 1.0f;
				p.A = 1.0f;

				// When writing aux channels here you probably want to create them in your image by adding tags for the appropiate
				// channels:  CHAN_Z, CHAN_U, CHAN_V, CHAN_COVERAGE, CHAN_OBJECT, CHAN_MATERIAL, CHAN_NORMALX, CHAN_NORMALY, CHAN_NORMALZ
				p.Z = FAR_PLANE_DEPTH;

				p.Coverage = 1.0f;

				p.MaterialID = 2;
				p.ObjectID = 3;

				p.U = 0.5f;
				p.V = 0.3f;

				p.NX = 0.0f;
				p.NY = 0.0f;
				p.NZ = 1.0f;

				pp >>= p;	// this will also do any type conversions - eg. convert the float32 data to float16 values and write them into a float16 image
			}
		}
	}
	*/

	return img;
}

int RendererRIB3D::NumSupportedRenderChannels()
{
   return RenRIB_NumSupportedChannels;
}

const FuID *RendererRIB3D::GetChannelName(int chan)
{
   if (chan >= 0 && chan < RenRIB_NumSupportedChannels)
      return &RenRIB_SupportedChannelFuID[chan];
   else
      return NULL;
}

// Renderman ErrorCode -> Text translation
static const char *GetErrorCodeString(RtInt code)
{
   const char *ret;

   switch (code)
   {
      case RIE_NOERROR:			ret = "no error";									break;
      case RIE_NOMEM:			ret = "out of memory";							break;
      case RIE_SYSTEM:			ret = "miscellaneous system error";			break;
      case RIE_NOFILE:			ret = "file nonexistent";						break;
      case RIE_BADFILE:			ret = "bad file format";						break;
      case RIE_VERSION:			ret = "file version mismatch";				break;
      case RIE_DISKFULL:		ret = "target disk is full";					break;
      case RIE_INCAPABLE:		ret = "optional RI feature";					break;
      case RIE_UNIMPLEMENT:	ret = "unimplemented feature";				break;
      case RIE_LIMIT:			ret = "arbitrary program limit";				break;
      case RIE_BUG:				ret = "probably a bug in renderer";			break;
      case RIE_NOTSTARTED:		ret = "RiBegin not called";					break;
      case RIE_NESTING:			ret = "bad begin-end nesting";				break;
      case RIE_NOTOPTIONS:		ret = "invalid state for options";			break;
      case RIE_NOTATTRIBS:		ret = "invalid state for attribs";			break;
      case RIE_NOTPRIMS:		ret = "invalid state for primitives";		break;
      case RIE_ILLSTATE:		ret = "other invalid state";					break;
      case RIE_BADMOTION:		ret = "badly formed motion block";			break;
      case RIE_BADSOLID:		ret = "badly formed solid block";			break;
      case RIE_BADTOKEN:		ret = "invalid token for request";			break;
      case RIE_RANGE:			ret = "parameter out of range";				break;
      case RIE_CONSISTENCY:	ret = "parameters inconsistent";				break;
      case RIE_BADHANDLE:		ret = "bad object/light handle";				break;
      case RIE_NOSHADER:		ret = "can't load requested shader";		break;
      case RIE_MISSINGDATA:	ret = "required parameters not provided";	break;
      case RIE_SYNTAX:			ret = "declare type syntax error";			break;
      case RIE_MATH:				ret = "math error";								break;  // zerodivide, noninvert matrix, etc
      default:						ret = "unknown err code";						break;
   }

   return ret;
}

static const char *GetSeverityString(RtInt severity)
{
   const char *ret;

   switch (severity)
   {
      case RIE_INFO:		ret = "*inf*";		break;		// Rendering stats and other info
      case RIE_WARNING:	ret = "*war*";		break;		// Something seems wrong, maybe okay
      case RIE_ERROR:	ret = "*err*";		break;		// Problem. Results may be wrong
      case RIE_SEVERE:	ret = "*sev*";		break;		// So bad you should probably abort
      default:				ret = "*unk*";		break;
   }

   return ret;
}

RendererRIB3D *RendererRIB3D::Ren = NULL;

// This function is the error handler we're going to supply to the Renderman DLL.  It prints messages to Fusions console.
RtVoid RendererRIB3D::ReportToConsole(RtInt code, RtInt severity, char *message)
{
   if (Ren)
   {
      EventConsoleSubID e = severity == RIE_INFO ? ECONID_Log : ECONID_Error;
      Ren->Owner->DocPtr->PrintF(e, "%s: %s %s (%s) \n", Ren->Owner->GetName(), GetSeverityString(severity), message, GetErrorCodeString(code));
   }
}

RtVoid RendererRIB3D::ProgressToConsole(RtFloat i_progress)
{
   if (Ren)
   {
      EventConsoleSubID e = ECONID_Log;
	  //Ren->Owner->DocPtr->PrintF(e, "%s : %f %% \n", Ren->Owner->GetName(), i_progress);
	  float progress_bar = i_progress/100;
	  Ren->Owner->SetProgress(progress_bar);
   }
}

RtVoid RendererRIB3D::ProgressToConsole2(RtFloat i_progress)
{
   if (Ren)
   {
      EventConsoleSubID e = ECONID_Log;
	  Ren->Owner->DocPtr->PrintF(e, "%s : %f %% \n", Ren->Owner->GetName(), i_progress);
	  float progress_bar = i_progress/100;
	  Ren->Owner->SetProgress(progress_bar);
   }
}


void RendererRIB3D::ParseRenderAttrs(TagList &tags)
{
	// Its the job of ParseRenderAttrs() to parse the supplied taglist into class member variables.  If we wanted, we could keep around
	// "tags" for the duration of the render if we wanted to and read the values directly from there but it would be slower.

	#ifdef MULTI_OUT
		Req = (Request *) tags.GetPtr(Ren_Request, NULL);
	#endif

   // OutSlot is where the rendered image will be place
   OutSlot = (Image **) tags.GetPtr(Ren_OutImageSlot, NULL);
   if (OutSlot)
      *OutSlot = NULL;		// set it to NULL here, later we'll set the OutImage into it, but only if we succeed


   // width/height/depth/aspect of out image
   OutWidth = tags.GetLong(Ren_OutWidth, -1);
   OutHeight = tags.GetLong(Ren_OutHeight, -1);

   if (OutWidth <= 0 || OutHeight <= 0)
   {
      LogError(RS_FATAL_ERROR, "output width or height is less than 1");
      throw FuException3D();
   }

   OutDepth = (DepthTags) tags.GetLong(Ren_OutDepth, IMDP_8bitInt);
   OutDataType = IMDPDepth_To_F3DDataType[OutDepth];

   if (OutDataType == F3D_INVALID_DATA_TYPE)
      throw FuException3D("invalid Ren_OutDepth");

   	if (OutDataType == F3D_UINT8) IFDepth = IFLD_8bitInt;
	else if (OutDataType == F3D_UINT16) IFDepth = IFLD_16bitInt;
	else if (OutDataType == F3D_FLOAT16) IFDepth = IFLD_16bitFloat;
	else if (OutDataType == F3D_FLOAT32) IFDepth = IFLD_32bitFloat;

   PixelAspectX = tags.GetDouble(Ren_PixelAspectX, 1.0);
   PixelAspectY = tags.GetDouble(Ren_PixelAspectY, 1.0);
	//setting AO 
   DoAO = tags.GetBool(RenRM_DoAO, false);
   AOSamples = tags.GetDouble(RenRM_AOSamples, 16.0);
	AODepth =  tags.GetDouble(RenRM_AODepth, 100.0);

	//raytracing samples
	RaySamples = tags.GetDouble(RenRM_RaySamples, 8);

	//enviroment map and convert slashes
	envPathMap = (const char *) tags.GetString(RenRM_EnvMap,NULL);
	if (*envPathMap > 0)
	{ 
		EnvLPath = envPathMap;

		char result[_MAX_PATH];   // array to hold the result.
	
		strcpy(result,EnvLPath); // copy string one into the result.
	
		// change all the '\' to '/'
		int len = strlen(result);
		for (int i = 0; i < len; i++)
			if (result[i] == '\\')
				result[i] = '/';

		pPath = (char *) &result;

		char *pPath2 = pPath;
		
		CreateTempFilename( txPathFile, pPath2, "Env", "tdl");
		}
	else
		{
			//char *txPathFile;
			txPathFile = "";
		}
	//path to shader files
	ShaderPath = (const char *) tags.GetPtr(RenRM_ShaderPathName, NULL);
	ProcPath = (const char *) tags.GetPtr(RenRM_ProcPathName, NULL);
	//photons
	/*DoPhotons = tags.GetBool(RenRM_DoPho,false);
	PhotonCount =tags.GetDouble(RenRM_PhoSamples,10000.0);
	PhotonStrength = tags.GetDouble(RenRM_PhoStrength,1.0);
	PhotonDist = tags.GetDouble(RenRM_PhoDist,1.0e36);
	PhotonBounce = tags.GetDouble(RenRM_PhoBounce, 5);
	PhotonRays = tags.GetDouble(RenRM_PhoRays, 64);*/
	//rib export
	DoRib = tags.GetBool(RenRM_DoRIB, false);
	RibFile = (const char *) tags.GetPtr(RenRM_RibFile,NULL);



   // document access for error reporting
   Document = (FusionDoc *) tags.GetPtr(Ren_Document, NULL);
   if (!Document)
      throw FuException3D("invalid Ren_Document");


   // some other info
   HiQ = tags.GetBool(Ren_HiQ, false);
   Proxy = tags.GetLong(Ren_Proxy, Document ? Document->ProxyScale : 1);
	PreCalc = tags.GetBool(Ren_PreCalc, false);


   // the scene
   Scene = (Scene3D *) tags.GetPtr(Ren_Scene, NULL);
   if (!Scene)
   {
      FuASSERT(0, ("Ren_Scene required tag"));
      LogError(RS_FATAL_ERROR, "no scene to render");
      throw FuException3D();
   }

   if (!Scene->RootNode && !PreCalc)		// if its a PreCalc (no data) render then the scene may not have a root node
   {
      FuASSERT(0, ("no rootnode in scene"));
      LogError(RS_FATAL_ERROR, "no rootnode in scene");
      throw FuException3D();
   }

   SceneInput = (Input *) tags.GetPtr(Ren_SceneInput, NULL);
   if (!SceneInput)
	throw FuException3D("invalid scene input");



   PrimaryTime = tags.GetDouble(Ren_Time, Scene->Time);


   // Camera
   PrimaryCamera = (CameraData3D *) tags.GetPtr(Ren_Camera, NULL);
   if (!PrimaryCamera)
      throw FuException3D("no camera selected or camera invalid");

   PrimaryCameraNode = (Node3D *) tags.GetPtr(Ren_CameraNode, NULL);
   if (!PrimaryCameraNode)
      throw FuException3D("invalid camera node");
//DoF
	DoDepthOfField = tags.GetBool(Ren_DoDepthOfField, false);
	FocalDistance = tags.GetDouble(Ren_PlaneOfFocus, 4.0);		// FocalLength is a lens property, FocalDistance sets the distance where objects will be in focus
	FStop = tags.GetDouble(RenRM_FStop, 8.0);
	DoFSides = tags.GetDouble(RenRM_DofSides, 0.0);
	if (DoFSides <3) DoFSides = 0;
	DoFAngle = tags.GetDouble(RenRM_DofAngle, 0.0);
	DoFRoundness = tags.GetDouble(RenRM_DofRoundness, 0.0);
	DoFDensity = tags.GetDouble(RenRM_DofDensity, 0.0);
//Raytrace Hider
	DoRaytrace = tags.GetBool(RenRM_RaytraceAll, false);

	//progress to console
    DoProgCons = tags.GetBool(RenRM_ProgCons, false);


   // lighting and shadows
   LightingEnabled = tags.GetBool(Ren_LightingEnabled, false);
   if (LightingEnabled)
      ShadowsEnabled = tags.GetBool(Ren_ShadowsEnabled, false);
   else
      ShadowsEnabled = false;


	// supersampling
	PixelSamplesX = tags.GetDouble(RenRM_PixelSamplesX, 2.0);
	shadingRate = tags.GetDouble(RenRM_ShadingRate, 2.0); //shading rate
	PixelSamplesY = tags.GetDouble(RenRM_PixelSamplesY, 2.0);
	PixelFilterIndex = tags.GetLong(RenRM_PixelFilterIndex, PF_Gaussian);
	PixelFilterWidth = tags.GetDouble(RenRM_PixelFilterWidth, PF_DefaultWidth[PixelFilterIndex]);
	PixelFilterHeight = tags.GetDouble(RenRM_PixelFilterHeight, PF_DefaultWidth[PixelFilterIndex]);
//MoBlur
	DoMoBlur = tags.GetBool(RenRM_DoMoBlur, false);
	DoDefMoBlur = tags.GetBool(RenRM_DoDefMoBlur, false);
	shutterOpen = tags.GetDouble(RenRM_shutterOpen, 0.0);
	shutterClose = tags.GetDouble(RenRM_shutterClose, 0.5);
	shutterSamples = tags.GetDouble(RenRM_shutterSamples, 4);
	shutterend = tags.GetDouble(RenRM_shutterEnd, 1.0);
	shutterstart = tags.GetDouble(RenRM_shutterStart,0.0);

	RM_ROI_Left = tags.GetDouble(RenRM_ValidWindowleft,0.0);
	RM_ROI_Right = tags.GetDouble(RenRM_ValidWindowright,0.0);
	RM_ROI_Top = tags.GetDouble(RenRM_ValidWindowtop,0.0);
	RM_ROI_Bottom = tags.GetDouble(RenRM_ValidWindowbottom,0.0);
	//throw FuException3D("ROI: %f %f %f %f", RM_ROI_Left,RM_ROI_Top,RM_ROI_Right,RM_ROI_Bottom);

	//tried ROI here, but somehow i failed
	//ROI->Set(0,0,0,0);
	//ROI = (FuRectInt *) tags.GetPtr(Ren_RegionOfInterest, NULL);
	//float roi_left = ROI->left;
	//if (!ROI)throw FuException3D("ROI empty");

   // Read out the tags we put there in ProcessTagList().
   RenderFilename = (const char *) tags.GetPtr(RenRM_RenderFileName, NULL);
   if (!RenderFilename)
   {
      //throw; TODO
   }

   FrameNumber = tags.GetLong(RenRM_FrameNumber, 0);
}

// This function loads the renderman dll and attaches a RmDLL object to it.
RmDLL *RendererRIB3D::LoadRenderman()
{
   RenderDLL = LoadLibrary(RenderFilename);

   if (!RenderDLL)
   {
      DWORD err = GetLastError();
      const int len = FuException3D::MSG_BUFFER_LEN;
      TCHAR buf[len];
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, len, NULL);
      throw FuException3D("failed to load \"%s\": %s", RenderFilename, buf);
   }


   // attach to dll
   RmDLL *dll = new RmDLL;
   dll->AttachTo(RenderDLL);

	IsPRMan = dll->PRManBegin != NULL;

	if (IsPRMan)
	{
		// When using Pixars Renderman renderer there is this extra function that needs to be called ahead of time
		// to initialize the renderer.
		int argc = 0;
		char *argv[] = { NULL, NULL, NULL };					// not sure what is supposed to be here?
		dll->PRManBegin(argc, argv);										// PRMan crashes sometimes here, no clue why.
	}

   return dll;
}

void RendererRIB3D::ReleaseRenderman(RmDLL *dll)
{
   // Deteach from the renderman dll and unload it.
   if (dll)
   {
		if (IsPRMan)
			dll->PRManEnd();

      dll->Detach();
      delete dll;
   }

   if (RenderDLL)
   {
      FreeLibrary(RenderDLL);
      RenderDLL = 0;
   }
  
   // Note as an optimization we could stay attached to the DLL and/or not call FreeLibrary() so during the next frame we don't need 
   // to reattach/reload.  A possible advantage of reloading each time is stability.
}

void RendererRIB3D::WriteEXRToDisk(Image *img, const char *filename)
//we write an OpenEXR file to disk
{
	FuPointer<Image> mapImageIn(img);	// map the images data into memory if it has been mapped out

   FileIO *io = (FileIO *) ClassRegistry->New("FileIO", CT_Protocol);
	ImageFormat *oexr = NULL;

   if (io && io->Open(filename, IOC_OPEN_WRITE))
   {
		ImageFormat *oexr = (ImageFormat *) ClassRegistry->New("OpenEXRFormat", CT_ImageFormat);

		if (oexr && oexr->SaveHeader(io, img))
		{
			oexr->SaveFrame(0, img);

			oexr->Close();
			io->Close();
		}

		SAFE_RECYCLE(oexr);
		SAFE_RECYCLE(io);
	}
   else
   {
      SAFE_RECYCLE(oexr);
		SAFE_RECYCLE(io);

      DWORD err = GetLastError();
      const int len = FuException3D::MSG_BUFFER_LEN;
      TCHAR buf[len];
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, len - 1, NULL);
      throw FuException3D("failed to write image \"%s\" (\"%s\")", filename, buf);
   }
}

Image *RendererRIB3D::ReadEXRFromDisk(const char *filename)
//we read an OpenEXR from disk and setting some default channels
{
   FileIO *io = (FileIO *) ClassRegistry->New("FileIO", CT_Protocol);

   Image *img = NULL;
	ImageFormat *oexr = NULL;
	
   if (io && io->Open(filename, IOC_OPEN_READ))
   {
      oexr = (ImageFormat *) ClassRegistry->New("OpenEXRFormat", CT_ImageFormat, Document);
	  if (oexr && oexr->ImageFormat::LoadHeader(io))
      {
		TagList tags(
            IMG_Channel,					CHAN_RED,
            IMG_Channel,					CHAN_GREEN,
            IMG_Channel,					CHAN_BLUE,
            IMG_Channel,					CHAN_ALPHA,
			IMG_Channel,					CHAN_NORMALX,
			IMG_Channel,					CHAN_NORMALY,
			IMG_Channel,					CHAN_NORMALZ,
			IMG_Channel,					CHAN_Z,
			IMG_Channel,					CHAN_MATERIAL,
            IMG_Width,						OutWidth,
            IMG_Height,						OutHeight,
            IMG_Depth,						OutDepth,
            IMG_XScale,						PixelAspectX,
            IMG_YScale,						PixelAspectY,
            IMG_Quality,					HiQ,
            IMG_Document,					Document,
			IF_LoadDepth,					IFDepth,
            //IMG_MotionBlurQuality,	// no need to add these tags, the Renderer3D tool will add them for you
            //IMAT_HasMotionBlur,		// and you don't have access to all the information to fill them out easily
            //IMG_ProxyScale,
            //IMAT_OriginalWidth,
            //IMAT_OriginalHeight,
            TAG_DONE);
	
         if (!oexr->LoadFrame(0, img, tags))
            throw FuException3D("failed to alloc/load output image \"%s\"", filename);

			oexr->Close();
      }

		io->Close();

      SAFE_RECYCLE(oexr);
		SAFE_RECYCLE(io);
	}
   else
   {
      SAFE_RECYCLE(oexr);
		SAFE_RECYCLE(io);

      DWORD err = GetLastError();
      const int len = FuException3D::MSG_BUFFER_LEN;
      TCHAR buf[len];
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, len - 1, NULL);
      throw FuException3D("failed to open output image \"%s\" (\"%s\")", filename, buf);
   }

   return img;
}
void RendererRIB3D::WriteTiffToDisk(Image *img, const char *filename)
{
	FuPointer<Image> mapImageIn(img);	// map the images data into memory if it has been mapped out

   FileIO *io = (FileIO *) ClassRegistry->New("FileIO", CT_Protocol);
	ImageFormat *tiff = NULL;

   if (io && io->Open(filename, IOC_OPEN_WRITE))
   {
		ImageFormat *tiff = (ImageFormat *) ClassRegistry->New("TiffFormat", CT_ImageFormat);

		if (tiff && tiff->SaveHeader(io, img))
		{
			tiff->SaveFrame(0, img);

			tiff->Close();
			io->Close();
		}

		SAFE_RECYCLE(tiff);
		SAFE_RECYCLE(io);
	}
   else
   {
      SAFE_RECYCLE(tiff);
		SAFE_RECYCLE(io);

      DWORD err = GetLastError();
      const int len = FuException3D::MSG_BUFFER_LEN;
      TCHAR buf[len];
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, len - 1, NULL);
      throw FuException3D("failed to write image \"%s\" (\"%s\")", filename, buf);
   }
}

Image *RendererRIB3D::ReadTiffFromDisk(const char *filename)
{
   FileIO *io = (FileIO *) ClassRegistry->New("FileIO", CT_Protocol);

   Image *img = NULL;
	ImageFormat *tiff = NULL;

   if (io && io->Open(filename, IOC_OPEN_READ))
   {
      tiff = (ImageFormat *) ClassRegistry->New("TiffFormat", CT_ImageFormat, Document);

      if (tiff && tiff->ImageFormat::LoadHeader(io))
      {
         TagList tags(
            IMG_Channel,					CHAN_RED,
            IMG_Channel,					CHAN_GREEN,
            IMG_Channel,					CHAN_BLUE,
            IMG_Channel,					CHAN_ALPHA,
            IMG_Width,						OutWidth,
            IMG_Height,						OutHeight,
            IMG_Depth,						OutDepth,
            IMG_XScale,						PixelAspectX,
            IMG_YScale,						PixelAspectY,
            IMG_Quality,					HiQ,
            IMG_Document,					Document,
			IF_LoadDepth,					IFDepth,
            //IMG_MotionBlurQuality,	// no need to add these tags, the Renderer3D tool will add them for you
            //IMAT_HasMotionBlur,		// and you don't have access to all the information to fill them out easily
            //IMG_ProxyScale,
            //IMAT_OriginalWidth,
            //IMAT_OriginalHeight,
            TAG_DONE);
			
		 if (!tiff->LoadFrame(0, img, tags))
            throw FuException3D("failed to alloc/load output image \"%s\"", filename);

			tiff->Close();
      }

		io->Close();

      SAFE_RECYCLE(tiff);
		SAFE_RECYCLE(io);
	}
   else
   {
      SAFE_RECYCLE(tiff);
		SAFE_RECYCLE(io);

      DWORD err = GetLastError();
      const int len = FuException3D::MSG_BUFFER_LEN;
      TCHAR buf[len];
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, len - 1, NULL);
      throw FuException3D("failed to open output image \"%s\" (\"%s\")", filename, buf);
   }

   return img;
}

void RendererRIB3D::CreateTempFilename(char *&pathfile, char *&file, const char *prefix, const char *suffix)
{
	file = new char[_MAX_PATH];
	pathfile = new char[_MAX_PATH];

	// remember it for later removal
	TempFiles.push_back(file);
	TempPathFiles.push_back(pathfile);

	// Create a temporary filename that will hopefully be unique.  The "Temp:" pathmap is a Fusion defined pathmap that will
	// defaults into expanding into the users temporary data directory as given by Windows (and is Windows version specific).
	// It is important this includes the process ID since multiple copies of Fusion can be running simulataneously and something
	// to identify the specific renderer since multiple RIB renderers can be rendering at the same time.
	static LONG ID = 0;
	LONG id = ::InterlockedIncrement(&ID);
	sprintf(file, "%s_%08x_%p_%08x.%s", prefix, GetCurrentProcessId(), this, id, suffix);

	sprintf(pathfile, "%s%s", TempPath, file);
}

void RendererRIB3D::SetTextureSearchPath()
{
	// Renderman has a specific format that it expects its searchpaths to be in.  We're going to convert TempPath into this format.
	// The format can vary depending on renderer.  All use ':' as pass separators, others also allow ';'.  There are various different
	// ways of supplying paths to windows drives.  If texture loading is failing, you probably need to check your renderer documentation
	// and see what is the proper way to specify the searchpath.

	// If TempPath is "C:\Users\stephen\AppData\Local\Temp\" the below code will convert it to "//C/Users/stephen/AppData/Local/Temp/:&".
	// This seems to work fine with PRMan 15.1 and 3Delight 9.0.

	char path[_MAX_PATH];
	sprintf(path, "//%c%s:&", TempPath[0], TempPath + 2);		// TODO - more sophisticed parsing required here, especially to handle UNC paths to network machines

	// change all the '\' to '/'
	int len = strlen(path);
	for (int i = 0; i < len; i++)
		if (path[i] == '\\')
			path[i] = '/';

	char **pPath = (char **) &path;

	const int nParams = 1;
	RtToken tokens[nParams] = { "texture" };
	RtPointer params[nParams] = { &pPath };		

	// NOTE:  If the below line crashed, the triple indirection to the path seems to work with PRMan/3Delight but some other renderers 
	// seem to expect only double indirection.
	rm->RiOptionV("searchpath", nParams, tokens, params);
}
void RendererRIB3D::SetShaderSearchPath()
{
	char path3[_MAX_PATH];
	strcpy(path3,ShaderPath); // copy string one into the result.
	const char *source = ";@;";
	strcat(path3, source);
	// change all the '\' to '/'
	int len = strlen(path3);
	for (int i = 0; i < len; i++)
		if (path3[i] == '\\')
			path3[i] = '/';

	char **pPath3 = (char **) &path3;

	const int nParams = 1;
	RtToken tokens[nParams] = { "shader" };
	RtPointer params[nParams] = { &pPath3 };		

	// NOTE:  If the below line crashed, the triple indirection to the path seems to work with PRMan/3Delight but some other renderers 
	// seem to expect only double indirection.
	rm->RiOptionV("searchpath", nParams, tokens, params);
}
void RendererRIB3D::SetProcSearchPath()
{
	char path3[_MAX_PATH];
	//const char ShaderPathProc[10] = "d://proc/";
	strcpy(path3,ProcPath); // copy string one into the result.
	const char *source = ";@;";
	strcat(path3, source);
	// change all the '\' to '/'
	int len = strlen(path3);
	for (int i = 0; i < len; i++)
		if (path3[i] == '\\')
			path3[i] = '/';

	char **pPath3 = (char **) &path3;
	const int ppParams = 1;
	RtToken pptokens[ppParams] = { "procedural" };
	RtPointer ppparams[ppParams] = { &pPath3 };		

		// NOTE:  If the below line crashed, the triple indirection to the path seems to work with PRMan/3Delight but some other renderers 
		// seem to expect only double indirection.
	rm->RiOptionV("searchpath", ppParams, pptokens, ppparams);
}


void RendererRIB3D::RenderScene()
{
	try
	{
		// we need this lock here only for the error handler - the error handling is a bit tricky because our error handler needs a 
		// FusionDoc pointer to print error messages to Fusion's console but there doesn't seem to be any provision for providing userdata
		// into the Renderman error handler... so instead we use a file scope thread-protected variable to store the FusionDoc ptr.
		AutoLock lock(ErrorLock);						
		Ren = this;

		// create and intialize the default rendering context - not all renderers behave the same way here it seems (for the renderers I've tested
		// RI_NULL seems to work fine but PRMan seems to like RI_RENDER -- the renderman specification only mentions RI_NULL as a possible arg, so
		// I'm not sure whats up.  It could be that all renderers should be getting RI_RENDER here, but I haven't gone back to test the non-Pixar 
		// renderers to see what they do)

		char *format = "ascii";
		RtToken format_tokens[] = { "format"};
		RtPointer format_params[] = { &format };
		rm->RiOptionV("rib",1,format_tokens,format_params);
		
		char *format2 = "on";
		RtToken format_tokens2[] = { "ident"};
		RtPointer format_params2[] = { &format2 };
		rm->RiOptionV("rib",1,format_tokens2,format_params2);

		char *format4 = "on";
		RtToken format_tokens4[] = { "header"};
		RtPointer format_params4[] = { &format4 };
		rm->RiOptionV("rib",1,format_tokens4,format_params4);

		if (DoRib)
		{
			//throw FuException3D("Rib Path: %s", RibFile);
			if (*RibFile > 0)
			{ 
				RibFile2 = RibFile;
				char result2[_MAX_PATH+10];   // array to hold the result.
				strcpy(result2,RibFile2); // copy string one into the result.
				// change all the '\' to '/'
				int len = strlen(result2);
				for (int i = 0; i < len; i++)
					if (result2[i] == '\\')
						result2[i] = '/';
				rm->RiBegin(result2);
			}
			else
			{
				throw FuException3D("No export file given");
			}
		}
		else
		{
			rm->RiBegin(IsPRMan ? "Ri:render" : RI_NULL);
		} 
		rm->RiArchiveRecord("comment","exported from eyeon fusion via the 3delight render plugin from reichofevil.wordpress.com");
		
		// install an error handler that will print errors to console
		rm->RiErrorHandler(ReportToConsole);
		
		//for ROI
		//bool ROI_NULL = ROI->IsNull();
		//rm->RiCropWindow(RM_ROI_Left,RM_ROI_Top,RM_ROI_Right,RM_ROI_Bottom);
		

		////enable raytrace for everyting
		if (DoRaytrace)
		{
			int progressive_number = 0;
			RtToken progressive_tokens[] = { "integer progressive"};
			RtPointer progressive_params[] = { &progressive_number };
			rm->RiHiderV("raytrace", 1, progressive_tokens , progressive_params);

			int moblur_number = 1;
			RtToken moblur_tokens[] = { "integer samplemotion"};
			RtPointer moblur_params[] = { &moblur_number };
			rm->RiAttributeV("trace", 1, moblur_tokens , moblur_params);
		}
		if (DoDepthOfField)
		{
			RtToken dof1_tokens[] = { "float[4] aperture"};
			RtPointer dof1_params[] = { &DoFSides, &DoFAngle, &DoFRoundness, &DoFDensity };
			rm->RiHiderV("hidden", 1, dof1_tokens , dof1_params);
		}

		// tell renderman that it needs to search for textures in the Temp: directory
		SetTextureSearchPath();

		// tell renderman that it needs to search for shaders in the given directory
		
		SetShaderSearchPath();

		SetProcSearchPath();

		// Create a list of all the surfaces in the scene.
		CreateSurfaceList();

		// Enumerate the textures in the scene and associate them with surfaces.
		CreateMaterialList();

		

		

		// This will create the .tx texture files on disk.  Some renderman renderer get upset if you call RiMakeTexture between WorldBegin()/WorldEnd()
		// while others do not.  So we do this prepass through and create all the textures beforehand.
		CreateRmTextures();
		char *edge = "clamp";
		const float width = 2.0f;		// this is not the sigma of the gaussian but rather the support of the filter kernal
		//if (strlen(txPathFile) > 0)
		if (*envPathMap > 0)
		{
			EnvLPath = envPathMap;

			char result[_MAX_PATH];   // array to hold the result.
	
			strcpy(result,EnvLPath); // copy string one into the result.
	
			// change all the '\' to '/'
			int len = strlen(result);
			for (int i = 0; i < len; i++)
				if (result[i] == '\\')
					result[i] = '/';

			pPath = (char *) &result;
			
			rm->RiMakeLatLongEnvironment( pPath, txPathFile, rm->RiGaussianFilter, width, width, RI_NULL);
					
			CheckAbort();
		}
		//setting all the image Bit depth and output channels
		if (OutDataType == F3D_FLOAT16)
		{
			rm->RiQuantize("rgba", 0, 0, 0, 0); //sets output to 16bit half float}
			rm->RiArchiveRecord("comment","halfloat");
		}
		else if (OutDataType == F3D_UINT16)
		{
			rm->RiQuantize("rgba", 65535, 0, 65535, 0.5); //sets output to 16bit int}
		}
		else if (OutDataType == F3D_FLOAT32)
		{
			rm->RiQuantize("rgba", 0, 0, 0, 0); //sets output to 32bit float}
		}
		else 
		{
			rm->RiQuantize("rgba", 255, 0, 255, 0.5); //sets output to 8bit int}
		}

		// set output filename and type
		char *file;
		//CreateTempFilename(RenderTiff, file, "RIB", "tiff");
		/*uint32 *img_data = NULL;

		rm->RiDisplay( "buffer", "memory", "rgba", &img_data, RI_NULL);*/
		
		if (OutDataType == F3D_FLOAT32){
			CreateTempFilename(RenderTiff, file, "RIB", "exr");
			char *AOV1 = "zips";
			char *AOV2 = "float";
			int AOV3 = 1;
			RtToken AOV_tokens2[] = { "string exrpixeltype",  "string compression", "int autocrop"};
			RtPointer AOV_params2[] = { &AOV2, &AOV1, &AOV3 };
			char file2[_MAX_PATH+10] = "+";
			strcat(file2,RenderTiff); // copy string one into the result.
			//rm->RiDisplayChannel("varying float materialID");
			rm->RiDeclare("materialID", "varying float");
			rm->RiDisplayV(RenderTiff,"exr","rgba",3,AOV_tokens2,AOV_params2);
			rm->RiDisplay(file2, "exr", "z", RI_NULL);
			rm->RiDisplay(file2, "exr", "materialID", RI_NULL);
		}
		else if (OutDataType == F3D_FLOAT16) {
			CreateTempFilename(RenderTiff, file, "RIB", "exr");
			char *AOV1 = "zips";
			char *AOV2 = "half";
			int AOV3 = 1;
			RtToken AOV_tokens2[] = { "string exrpixeltype",  "string compression", "int autocrop"};
			RtPointer AOV_params2[] = { &AOV2, &AOV1, &AOV3 };
			char file2[_MAX_PATH+10] = "+";
			strcat(file2,RenderTiff); // copy string one into the result.
			//rm->RiDisplayChannel("varying float materialID");
			rm->RiDeclare("materialID", "varying float");
			rm->RiDisplayV(RenderTiff,"exr","rgba",3,AOV_tokens2,AOV_params2);
			rm->RiDisplay(file2, "exr", "z", RI_NULL);
			rm->RiDisplay(file2, "exr", "materialID", RI_NULL);

		}
		else if ((OutDataType == F3D_UINT16)||(OutDataType == F3D_UINT8))
		{
			CreateTempFilename(RenderTiff, file, "RIB", "tif");
			
			rm->RiDisplay(RenderTiff,"tiff","rgba",RI_NULL);
		}


		
		// set output width/height/pixelAspect
		rm->RiFormat(OutWidth, OutHeight, PixelAspectX / PixelAspectY);

		// Setting the FrameAspectRatio is not necessary as Renderman will by default use the same frame aspect we are explicitly setting.
		float32 frameAspectRatio = (float(OutWidth) / OutHeight) * (PixelAspectX / PixelAspectY);
		//rm->RiFrameAspectRatio(frameAspectRatio);			// uncommenting this crashes 3Delight

		// The screen window determines how the aperture gate is mapped to the resolution gate.  The default screen window convention that 
		// renderman uses corresponds to FitMethod_Outside.  The resolution/aperture gate fitting overly complicates things a bit more than 
		// we'd want for an SDK sample.
		rm->RiScreenWindow(-frameAspectRatio, +frameAspectRatio, -1.0f, +1.0f);  //TODO - other fit methods

		// set the MSAA reconstruction filter
		rm->RiPixelFilter(rm->PixelFilterFunc[PixelFilterIndex], PixelFilterWidth, PixelFilterHeight);

		// set the number of geometry samples per pixel (MSAA rates)
		rm->RiPixelSamples(PixelSamplesX, PixelSamplesY);

		//ShadingRate
		if (Proxy>1)
			{
				shadingRate *= Proxy;		
			}	
		rm->RiShadingRate(shadingRate);
		rm->RiShadingInterpolation("smooth");

		// begin frame specific information
		rm->RiFrameBegin(FrameNumber);
						
		
		// add progress to commandline
		if (DoProgCons)
			{
			RtPointer callBack = &ProgressToConsole2;
			rm->RiOption( "statistics", "progresscallback", &callBack, RI_NULL );
			}
		else
			{
				RtPointer callBack = &ProgressToConsole;
				rm->RiOption( "statistics", "progresscallback", &callBack, RI_NULL );
			}
		

		int progress = 1;
		RtToken prog_tokens[] = { "integer progress" };
		RtPointer prog_params[] = { &progress };
		//rm->RiOptionV("statistics", 1, prog_tokens, prog_params);
		
		/*int progress2 = 2;
		RtToken prog_tokens2[] = { "integer endofframe" };
		RtPointer prog_params2[] = { &progress2 };
		rm->RiOptionV("statistics",1,prog_tokens2, prog_params2);

		char *progress3 = "d://3delight_render.html";
		RtToken prog_tokens3[] = { "string filename" };
		RtPointer prog_params3[] = { &progress3 };
		rm->RiOptionV("statistics",1,prog_tokens3, prog_params3);*/

		// In Renderman cameras are considered part of the global state, whereas in Fusion they are part of the scene graph.  In Renderman there
		// is only one camera in a scene, but in Fusion there can be many cameras especially once you take into consideration each time you branch
		// the flow downstream of a camera you are creating a copy of the camera to go down each branch.  To resolve this we allow the user to select 
		// the camera they want to use when we render and we have to be careful to transform that camera to world space.
		float32 fov = PrimaryCamera->GetFovY();  // TODO - write a version that takes a param for resolution gate or film gate
		RtToken tokens[] = { "fov" };
		RtPointer params[] = { &fov };
		rm->RiProjectionV("perspective", 1, tokens, params);		
		

		//raytracing depth
		int trace_number = 2;
		RtToken Stokens[] = { "integer maxdepth" };
		RtPointer Sparams[] = { &trace_number };
		rm->RiOptionV("trace", 1, Stokens , Sparams);

		//Option "trace" "int diffuseraycache" 1
		int trace_cache = 1;
		RtToken Ctokens[] = { "integer diffuseraycache" };
		RtPointer Cparams[] = { &trace_cache };
		rm->RiOptionV("trace", 1, Ctokens , Cparams);

		//photons
		/*if (DoPhotons){
			RtToken Ptokens[] = { "integer emit" };
			int pho_number = PhotonCount;
			RtPointer Pparams[] = { &pho_number };
			rm->RiOptionV("photon",1,Ptokens,Pparams);

			RtToken Ptokens2[] = { "maxdiffusedepth" };
			int pho_number2 = 5;
			RtPointer Pparams2[] = { &PhotonBounce };
			rm->RiOptionV("photon",1,Ptokens2,Pparams2);

			RtToken Ptokens5[] = { "maxspeculardepth" };
			int pho_number5 = 5;
			RtPointer Pparams5[] = { &PhotonBounce };
			rm->RiOptionV("photon",1,Ptokens5,Pparams5);

			const char *pho_map_name = "global";
			RtToken Ptokens3[] = { "string globalmap" };
			RtPointer Pparams3[] = { &pho_map_name };
			rm->RiAttributeV("photon", 1, Ptokens3, Pparams3);
			
			const char *pho_map_name_caus = "global_caus";
			RtToken Ptokens4[] = { "string causticmap" };
			RtPointer Pparams4[] = { &pho_map_name_caus };
			rm->RiAttributeV("photon", 1, Ptokens4, Pparams4);
		}*/
		

		// depth of field
		if (DoDepthOfField)
			rm->RiDepthOfField(FStop, PrimaryCamera->FLength, FocalDistance);

		// since renderman uses a left handed coordinate system and fusion is righthanded we need to correct for this
		rm->RiScale(1.0f, 1.0f, -1.0f);

		
		//motionblur settings

		// transform us into world space - ModelToWorld transforms geometry from camera space to world space, but we do not want to transform
		// geometry but rather the (coordiante) spaces so we need to invert
		//Matrix4f worldToCam = PrimaryCameraNode->ModelToWorld.Inverse();
		//ConcatTransform(worldToCam);
		//throw FuException3D("shutter samples: %s", &shutterSamples);
		if (DoMoBlur)
		{
			rm->RiShutter(shutterOpen, shutterClose);
			RtToken Ptokens3[] = { "float efficiency" };
			RtPointer Pparams3[] = { &shutterstart, &shutterend };
			rm->RiOptionV("shutter", 1, Ptokens3, Pparams3);
									
			//int moblur_number = 1;
			//RtToken moblur_tokens[] = { "integer samplemotion"};
			//RtPointer moblur_params[] = { &moblur_number };
			//rm->RiAttributeV("trace", 1, moblur_tokens , moblur_params);
			
			rm->RiGeometricApproximation("motionfactor", (RtFloat) 1.0);		
			RtInt MBSamples = (RtInt) shutterSamples; 
			for (int i = 0; i <= shutterSamples; i++)
			{
				MBTimes[i] = i*(1/shutterSamples);
			}
			rm->RiMotionBeginV(MBSamples,MBTimes);
			for (int i = 0; i < shutterSamples; i++)
			{ 
				MBTimes[i] = (Scene->Time) + i*(1/shutterSamples);
				float mb_offset = (shutterClose-shutterOpen)/2;
				second_scene = (Scene3D *) SceneInput->GetSource(MBTimes[i]-mb_offset);
				if (second_scene){

					second_scene->Setup();
					Matrix4f worldToNewCam = second_scene->GetNodeByID(new_camera,PrimaryCameraNode->ID,PrimaryCameraNode->SubID)->ModelToWorld.Inverse();
					ConcatTransform(worldToNewCam);
				}
				else{
					throw FuException3D("scene_error");
				}
				}
			rm->RiMotionEnd();
		}
		else{
			Matrix4f worldToCam = PrimaryCameraNode->ModelToWorld.Inverse();
			ConcatTransform(worldToCam);
			
		}
		// When WorldBegin() is called, the worldToCamera transform is set to the current transform and the current transform is 
		// set to identity.  Any objects declared between a WorldBegin/WorldEnd pair go out of scope when WorldEnd() is called
		// and are destructed.
		rm->RiWorldBegin();

		//enable raytrace shadows for all objects
		int trans_number = 1;
		RtToken Ttokens[] = { "integer transmission"};
		RtPointer Tparams[] = { &trans_number };
	    rm->RiAttributeV("visibility", 1, Ttokens , Tparams);
	    
		//if (DoAO){
		////enable AO for all objects
		//char *occ_string = "shader";
		//RtToken otokens[] = { "string transmission"};
		//RtPointer oparams[] = { &occ_string };
	 //   rm->RiAttributeV("visibility", 1, otokens , oparams);
		
		
	    //enable calculation of GI from shader color for all objects
		char* Hitmode = "shader";
		RtToken Htokens[] = { "string diffusehitmode"};
		RtPointer Hparams[] = { &Hitmode };
	    rm->RiAttributeV("shade", 1, Htokens , Hparams);
		//}

		 //enable secondary rays for all objects
		int d_number = 1;
		RtToken Dtokens[] = { "integer diffuse"};
		RtPointer Dparams[] = { &d_number };
	    rm->RiAttributeV("visibility", 1, Dtokens , Dparams);

		//enable raytrace reflections for all objects (needs special shader)
		int ray_spec = 1;
		RtToken Rtokens[] = { "integer specular" };
		RtPointer Rparams[] = { &ray_spec };
	    rm->RiAttributeV("visibility", 1, Rtokens , Rparams);

		////enable raytrace 
		//char* ray_trace = "shader";
		//RtToken RRtokens[] = { "string transmissionhitmode" };
		//RtPointer RRparams[] = { &ray_trace };
	 //   rm->RiAttributeV("shade", 1, RRtokens , RRparams);

		////enable raytrace reflections for all objects (needs special shader)
		//char* ray_SSS = "SSS";
		//RtToken RSSStokens[] = { "string subsurface" };
		//RtPointer RSSSparams[] = { &ray_spec };
	 //   rm->RiAttributeV("visibility", 1, RSSStokens , RSSSparams);
		CreateGeoLists();


		CreateLightLists();					// build up a list of lights affecting each surface node

		RenderNode(Scene->RootNode);		// recursively render the nodes inside of the scene
		FreeLightLists();						// free the light lists
		FreeSurfaceLists();

		// WorldEnd() causes the renderer to render the image.  If there is a crash in the RM renderer it typically happens here.
		rm->RiWorldEnd();

		rm->RiFrameEnd();

		rm->RiErrorHandler(NULL);				// remove our custom error handler

		// cleanup the rendering context and associated memory
		rm->RiEnd();										

		FreeMaterialList();
		FreeSurfaceList();

		Ren = NULL;
	}
	catch (...)
	{
		FreeLightLists();
		FreeMaterialList();
		FreeSurfaceList();
		throw;
	}
}

void RendererRIB3D::CreateSurfaceList()
{
	// create a mapping which allow us quickly to get a Light/Surface object from a Node3D
	NodeLUT = new int[Scene->NumNodes];
	memset(NodeLUT, 0xff, Scene->NumNodes * sizeof(int));


	// create the SurfaceList
	NumSurfaces = Scene->GeomList.size();
	SurfaceList = new Surface[NumSurfaces];

	for (int i = 0; i < NumSurfaces; i++)
	{
		int nodeIndex = Scene->GeomList[i];
		NodeLUT[nodeIndex] = i;

		SurfaceList[i].Node = Scene->NodeList[nodeIndex];
	}
}

void RendererRIB3D::FreeSurfaceList()
{
	SAFE_DELETE_ARRAY(NodeLUT);
	SAFE_DELETE_ARRAY(SurfaceList);
}

void RendererRIB3D::CreateMaterialList()
{
	// The purpose of this method is to enumerate all the textures appearing in the scene.  The reason we do this extra prepass and 
	// enumerate and create the textures before even beginning to render the scene is that some renderman renderers get upset if you 
	// call RiMakeTexture between WorldBegin()/WorldEnd().

	// TODO - If the same fusion image is piped into multiple geometry, this code will become inefficient.  We need some way of checking 
	//        if the same image is used twice

	NumMaterials = 0;

	for (int i = 0; i < NumSurfaces; i++)
		NumMaterials += SurfaceList[i].Node->GetNumMtls();

	int idx = 0;
	MaterialList = new Material[NumMaterials];

	for (int i = 0; i < NumSurfaces; i++)
	{
		Node3D *n = SurfaceList[i].Node;

		int nMtlsThisNode = n->GetNumMtls();
		for (int j = 0; j < nMtlsThisNode; j++)
		{
			Material &mtl = MaterialList[idx++];

			// set some defaults that we'll override in CreateRmTextures()
			mtl.IsSupported = false;
			mtl.DiffuseFile = NULL;
			mtl.DiffuseType = UseRiMakeTexture ? Material::Tx : Material::Tiff;
			mtl.FuMaterial = n->GetMtl(j);

			SurfaceList[i].MaterialList.push_back(&mtl);
		}
	}
}

void RendererRIB3D::FreeMaterialList()
{
	SAFE_DELETE_ARRAY(MaterialList);
}

void RendererRIB3D::FreeTempFiles()
{
	// remove any temporary files we created
	int nTempPathFiles = TempPathFiles.size();
	for (int i = 0; i < nTempPathFiles; i++)
	{
		remove(TempPathFiles[i]);
		delete [] TempPathFiles[i];
	}

	TempPathFiles.clear();


	int nTempFiles = TempFiles.size();
	for (int i = 0; i < nTempFiles; i++)
		delete [] TempFiles[i];

	TempFiles.clear();
}

void RendererRIB3D::CreateRmTextures()
{
	// Write out any fusion images feeding into the surface tools to disk so that renderman may read them back in.
	// If UseRiMakeTexture = true this method will write .tx files to disk, otherwise it will write .tiff files to disk.

	for (int i = 0; i < NumMaterials; i++)
	{
		Material &mtl = MaterialList[i];
		MtlData3D *mtlData = mtl.FuMaterial;
 
		// Covered most of the Fusion materials and added some by my own
		// think this part is very inefficent
		// needs optimisation
		mtl.IsSupported = (mtlData && mtlData->IsTypeOf(CLSID_MtlPhongData))||(mtlData && mtlData->IsTypeOf(CLSID_MtlStdData))||(mtlData && mtlData->IsTypeOf(COMPANY_ID_DOT + CLSID_MtlCookData))||(mtlData && mtlData->IsTypeOf(CLSID_MtlBlinnData))||(mtlData && mtlData->IsTypeOf(CLSID_MtlWardData))||(mtlData && mtlData->IsTypeOf(CLSID_MtlGlassData))||(mtlData && mtlData->IsTypeOf(COMPANY_ID_DOT + CLSID_MtlBlinnData));
		if (mtl.IsSupported)
		{
			if (mtlData->IsTypeOf(CLSID_MtlPhongData))
				{
				Image *img = NULL;
				MtlPhongData3D *mtlPhongData = (MtlPhongData3D *) mtlData;
				Color4f diff_color = mtlPhongData->Diffuse;
				Color4f spec_color = mtlPhongData->Specular;
				float32 spec_intensity = mtlPhongData->SpecularIntensity;
				//throw FuException3D("phong zeugs: %s ", spec_intensity);
				float32 spec_expo = mtlPhongData->SpecularExponent;
				float32 opacity = mtlPhongData->Opacity;
				//TagList *tags();
				int32 mtl_ID = mtlPhongData->GetMaterialID();
				
				//float mtl_ID = 0;
				
				if (img)
				{
					char *tmpTexTiff = NULL;		// we'll write images from Fusion into the .tiff format
					char *tmpTexTx = NULL;			// and then convert them into this Renderman preferred .tx format

					char *edge = "clamp";		
					//char *edge = "periodic";
	
					char *tiffPathFile, *tiffFile;
					CreateTempFilename(tiffPathFile, tiffFile, "Tex", "tiff");
						
					CheckAbort();
	
					WriteTiffToDisk(img, tiffPathFile);

					//throw FuException3D("texture: %s ", tiffPathFile);
	
					CheckAbort();
	
					if (mtl.DiffuseType == Material::Tx)
					{
						char *txPathFile, *txFile;
						CreateTempFilename(txPathFile, txFile, "Tex", "tx");

						mtl.DiffuseFile = txFile;
						mtl.DiffusePathFile = txPathFile;
						mtl.spec_color = spec_color;
						mtl.spec_int = spec_intensity;
						mtl.spec_expo = spec_expo;
						mtl.opacity = opacity;
						mtl.diff_color = diff_color;
						mtl.shader_type = 2;
						mtl.mtlID = mtl_ID;

						const float width = 2.0f;		// this is not the sigma of the gaussian but rather the support of the filter kernal
						rm->RiMakeTexture(tiffPathFile, txPathFile, edge, edge, rm->RiGaussianFilter, width, width, RI_NULL);
						CheckAbort();
					}
					else if (mtl.DiffuseType == Material::Tiff)
					{
						mtl.DiffuseFile = tiffFile;
						mtl.DiffusePathFile = tiffPathFile;
					}
				}
				else
				{
					mtl.spec_color = spec_color;
					mtl.spec_int = spec_intensity;
					mtl.spec_expo = spec_expo;
					mtl.opacity = opacity;
					mtl.diff_color = diff_color;
					mtl.shader_type = 2;
					mtl.mtlID = mtl_ID;
				}
			}

			if (mtlData->IsTypeOf(COMPANY_ID_DOT + CLSID_MtlCookData))
				{
					MtlCookData3D *mtlCookData = (MtlCookData3D *) mtlData;
					Image *img = NULL;
					img = mtlCookData->DiffuseImg;

					int32 mtl_ID = mtlCookData->GetMaterialID();

					Image *img_inca = NULL;
					img_inca = mtlCookData->IncaImg;

					Image *spec_img1 = NULL;
					spec_img1 = mtlCookData->SpecColorImg;
					
					Image *spec_img2 = NULL;
					spec_img2 = mtlCookData->SpecExpoImg;
					
					Image *spec_img3 = NULL;
					spec_img3 = mtlCookData->SpecRoughImg;

					Color4f diff_color = mtlCookData->Diffuse;
					float32 diff_rough = mtlCookData->roughness;

					Color4f i_Incan = mtlCookData->Incan;

					Color4f i_coat_color = mtlCookData->CoatingColor;
					float32 i_coat_rough = mtlCookData->CoatingRough;
					float32 i_CoatingOn = mtlCookData->CoatingOn;;
					float32 i_CoatIOR = mtlCookData->CoatIOR;
					float32 i_CoatSamples = mtlCookData->CoatSamples;
					Color4f i_CoatTrans = mtlCookData->CoatTrans;
					float32 i_CoatingThick = mtlCookData->CoatingThick;
					float32 i_CoatingEnv = mtlCookData->CoatingEnv;
					float32 i_CoatingSpec = mtlCookData->CoatingSpec;
					float32 i_CoatingRefl = mtlCookData->CoatingRefl;
					float32 i_FogStrength = mtlCookData->FogStrength;
					Color4f i_ReflFog = mtlCookData->ReflFog;
					float32 i_RefraSamples = mtlCookData->RefraSamples;
					
					Color4f spec_color = mtlCookData->Specular;
					float32 spec_intensity = mtlCookData->SpecularIntensity;
					float32 spec_expo = mtlCookData->SpecularExponent;
					float32 specIOR = mtlCookData->specIOR;
					float32 spec_rough = mtlCookData->specRoughness;
					float32 opacity = mtlCookData->Opacity;
					float32 i_SpecularEnv = mtlCookData->SpecularEnv;
					float32 i_SpecularRefl = mtlCookData->SpecularRefl;
					float32 i_SpecularSpec = mtlCookData->SpecularSpec;
					float32 i_SpecSamples = mtlCookData->SpecSamples;
					Color4f i_ReflAniso = mtlCookData->ReflAniso;
					float32 i_SpecularAniso = mtlCookData->SpecularAniso;
					

					Image *bump_img = NULL;
					bump_img = mtlCookData->BumpImg;

					float32 bumpy = mtlCookData->BumpBound;
					float32 bump_int = mtlCookData->BumpStrength;

					Color4f refl_color = mtlCookData->Refl;
					float32 refl_rough = mtlCookData->ReflRoughness;
									
					float32 refrior = mtlCookData->RefrIOR;

					int DoGI = mtlCookData->DoGI;
					float32 GIStrength = mtlCookData->GIStrength;
					
					Color4f transmission = mtlCookData->Transmittance;

					Color4f i_SSS_Color = mtlCookData->SSS_color;
					float32 i_SSS_on = mtlCookData->SSS_on;
					float32 i_SSSIOR = mtlCookData->SSSIOR;
					float32 i_SSSStrength = mtlCookData->SSSStrength;
					float32 i_SSSScale = mtlCookData->SSSScale;
							
					mtl.diff_color = diff_color;
					mtl.diff_rough = diff_rough;
					mtl.opacity = opacity;
					mtl.Incan = i_Incan;

					mtl.spec_color = spec_color;
					mtl.spec_expo = spec_expo;
					mtl.spec_rough = spec_rough;
					mtl.spec_ior = specIOR;
					mtl.spec_int = spec_intensity;
					mtl.SpecularEnv = i_SpecularEnv;
					mtl.SpecularRefl = i_SpecularRefl;
					mtl.SpecularSpec = i_SpecularSpec;
					mtl.SpecSamples = i_SpecSamples;
					mtl.ReflAniso = i_ReflAniso;
					mtl.SpecularAniso = i_SpecularAniso;
					
					mtl.refl_color = refl_color;
					mtl.refl_rough = refl_rough;
					mtl.tansmission = transmission;

					mtl.FogStrength = i_FogStrength;
					mtl.ReflFog = i_ReflFog;
					mtl.RefraSamples = i_RefraSamples;
		
					mtl.refrior = refrior;
		
					mtl.DoGI = DoGI;
					mtl.GIStrength = GIStrength;
		
					mtl.shader_type = 3;

					mtl.BumpBound = bumpy;
					mtl.BumpStrength = bump_int;

					mtl.CoatingColor = i_coat_color;
					mtl.CoatingOn = i_CoatingOn;
					mtl.CoatIOR = i_CoatIOR;
					mtl.CoatSamples = i_CoatSamples;
					mtl.CoatTrans = i_CoatTrans;
					mtl.CoatingThick = i_CoatingThick;
					mtl.CoatingRough = i_coat_rough;
					mtl.CoatingEnv = i_CoatingEnv;
					mtl.CoatingSpec = i_CoatingSpec;
					mtl.CoatingRefl = i_CoatingRefl;

					mtl.SSS_color = i_SSS_Color;
					mtl.SSS_on = i_SSS_on;
					mtl.SSSIOR = i_SSSIOR;
					mtl.SSSScale = i_SSSScale;
					mtl.SSSStrength = i_SSSStrength;

					mtl.mtlID = mtl_ID;




					if (img)
					{
						char *tmpTexTiff = NULL;		// we'll write images from Fusion into the .tiff format
						char *tmpTexTx = NULL;			// and then convert them into this Renderman preferred .tx format

						char *edge = "clamp";		
						//char *edge = "periodic";
		
						char *tiffPathFile, *tiffFile;
						CreateTempFilename(tiffPathFile, tiffFile, "Tex", "tiff");
						
						CheckAbort();
		
						WriteTiffToDisk(img, tiffPathFile);
							
						CheckAbort();
						
				
						if (mtl.DiffuseType == Material::Tx)
						{
							char *txPathFile, *txFile;
							CreateTempFilename(txPathFile, txFile, "Tex", "tx");
							mtl.DiffuseFile = txFile;
							mtl.DiffusePathFile = txPathFile;
							mtl.opacity = opacity;
							
							const float width = 2.0f;		// this is not the sigma of the gaussian but rather the support of the filter kernal
							rm->RiMakeTexture(tiffPathFile, txPathFile, edge, edge, rm->RiGaussianFilter, width, width, RI_NULL);
							
							CheckAbort();
						}
						else if (mtl.DiffuseType == Material::Tiff)
						{
							mtl.DiffuseFile = tiffFile;
							mtl.DiffusePathFile = tiffPathFile;
						}
					}
					if (spec_img1)
					{
						char *tmpTexTiff2 = NULL;		// we'll write images from Fusion into the .tiff format
						char *tmpTexTx2 = NULL;			// and then convert them into this Renderman preferred .tx format

						char *edge = "clamp";		
						//char *edge = "periodic";
		
						char *tiffPathFile2, *tiffFile2;
						CreateTempFilename(tiffPathFile2, tiffFile2, "Tex", "tiff");
						
						CheckAbort();
		
						WriteTiffToDisk(spec_img1, tiffPathFile2);
							
						CheckAbort();
						
				
						if (mtl.DiffuseType == Material::Tx)
						{
							char *txPathFile2, *txFile2;
							CreateTempFilename(txPathFile2, txFile2, "Tex", "tx");
							mtl.SpecColorFile = txFile2;
							mtl.SpecColorPathFile = txPathFile2;
							
							const float width = 2.0f;		// this is not the sigma of the gaussian but rather the support of the filter kernal
							rm->RiMakeTexture(tiffPathFile2, txPathFile2, edge, edge, rm->RiGaussianFilter, width, width, RI_NULL);
							
							CheckAbort();
						}
						else if (mtl.DiffuseType == Material::Tiff)
						{
							mtl.SpecColorFile = tiffFile2;
							mtl.SpecColorPathFile = tiffPathFile2;
						}
					}
					else
					{
						mtl.SpecColorFile = "";
						mtl.SpecColorPathFile = "";
					}
					if (img_inca)
					{
						char *tmpTexTiffinca = NULL;		// we'll write images from Fusion into the .tiff format
						char *tmpTexTxinca = NULL;			// and then convert them into this Renderman preferred .tx format

						char *edge = "clamp";		
						//char *edge = "periodic";
		
						char *tiffPathFileinca, *tiffFileinca;
						CreateTempFilename(tiffPathFileinca, tiffFileinca, "Tex", "tiff");
						
						CheckAbort();
		
						WriteTiffToDisk(img_inca, tiffPathFileinca);
							
						CheckAbort();
						
				
						if (mtl.DiffuseType == Material::Tx)
						{
							char *txPathFileinca, *txFileinca;
							CreateTempFilename(txPathFileinca, txFileinca, "Tex", "tx");
							mtl.IncaFile = txFileinca;
							mtl.IncaPathFile = txPathFileinca;
							
							const float width = 2.0f;		// this is not the sigma of the gaussian but rather the support of the filter kernal
							rm->RiMakeTexture(tiffPathFileinca, txPathFileinca, edge, edge, rm->RiGaussianFilter, width, width, RI_NULL);
							
							CheckAbort();
						}
						else if (mtl.DiffuseType == Material::Tiff)
						{
							mtl.IncaFile = tiffFileinca;
							mtl.IncaPathFile = tiffPathFileinca;
						}
					}
					else
					{
						mtl.IncaFile = "";
						mtl.IncaPathFile = "";
					}
					if (spec_img2)
					{
						char *tmpTexTiff3 = NULL;		// we'll write images from Fusion into the .tiff format
						char *tmpTexTx3 = NULL;			// and then convert them into this Renderman preferred .tx format

						char *edge = "clamp";		
						//char *edge = "periodic";
		
						char *tiffPathFile3, *tiffFile3;
						CreateTempFilename(tiffPathFile3, tiffFile3, "Tex", "tiff");
						
						CheckAbort();
		
						WriteTiffToDisk(spec_img2, tiffPathFile3);
							
						CheckAbort();
						
				
						if (mtl.DiffuseType == Material::Tx)
						{
							char *txPathFile3, *txFile3;
							CreateTempFilename(txPathFile3, txFile3, "Tex", "tx");
							mtl.SpecExpoFile = txFile3;
							mtl.SpecExpoPathFile = txPathFile3;
							
							const float width = 2.0f;		// this is not the sigma of the gaussian but rather the support of the filter kernal
							rm->RiMakeTexture(tiffPathFile3, txPathFile3, edge, edge, rm->RiGaussianFilter, width, width, RI_NULL);
							
							CheckAbort();
						}
						else if (mtl.DiffuseType == Material::Tiff)
						{
							mtl.SpecExpoFile = tiffFile3;
							mtl.SpecExpoPathFile = tiffPathFile3;
						}
					}
					else
					{
						mtl.SpecExpoFile = "";
						mtl.SpecExpoPathFile = "";
					}
					if (spec_img3)
					{
						char *tmpTexTiff4 = NULL;		// we'll write images from Fusion into the .tiff format
						char *tmpTexTx4 = NULL;			// and then convert them into this Renderman preferred .tx format

						char *edge = "clamp";		
						//char *edge = "periodic";
		
						char *tiffPathFile4, *tiffFile4;
						CreateTempFilename(tiffPathFile4, tiffFile4, "Tex", "tiff");
						
						CheckAbort();
		
						WriteTiffToDisk(spec_img3, tiffPathFile4);
							
						CheckAbort();
						
				
						if (mtl.DiffuseType == Material::Tx)
						{
							char *txPathFile4, *txFile4;
							CreateTempFilename(txPathFile4, txFile4, "Tex", "tx");
							mtl.SpecRoughFile = txFile4;
							mtl.SpecRoughPathFile = txPathFile4;
							
							const float width = 2.0f;		// this is not the sigma of the gaussian but rather the support of the filter kernal
							rm->RiMakeTexture(tiffPathFile4, txPathFile4, edge, edge, rm->RiGaussianFilter, width, width, RI_NULL);
							
							CheckAbort();
						}
						else if (mtl.DiffuseType == Material::Tiff)
						{
							mtl.SpecRoughFile = tiffFile4;
							mtl.SpecRoughPathFile = tiffPathFile4;
						}
					}
					else
					{
						mtl.SpecRoughFile = "";
						mtl.SpecRoughPathFile = "";
					}
				
					if (bump_img!=NULL)
					{
						char *tmpTexTiff8 = NULL;		// we'll write images from Fusion into the .tiff format
						char *tmpTexTx8 = NULL;			// and then convert them into this Renderman preferred .tx format

						char *edge = "clamp";		
						//char *edge = "periodic";
		
						char *tiffPathFile8, *tiffFile8;
						CreateTempFilename(tiffPathFile8, tiffFile8, "Tex", "tiff");
						
						CheckAbort();
		
						WriteTiffToDisk(bump_img, tiffPathFile8);
							
						CheckAbort();
						
				
						if (mtl.DiffuseType == Material::Tx)
						{
							char *txPathFile8, *txFile8;
							CreateTempFilename(txPathFile8, txFile8, "Tex", "tx");
							mtl.BumpFile = txFile8;
							mtl.BumpPathFile = txPathFile8;
							
							const float width = 2.0f;		// this is not the sigma of the gaussian but rather the support of the filter kernal
							rm->RiMakeTexture(tiffPathFile8, txPathFile8, edge, edge, rm->RiGaussianFilter, width, width, RI_NULL);
							
							CheckAbort();
						}
						else if (mtl.DiffuseType == Material::Tiff)
						{
							mtl.BumpFile = tiffFile8;
							mtl.BumpPathFile = tiffPathFile8;
						}
					}
					else
					{
						mtl.BumpFile = NULL;
						mtl.BumpPathFile = NULL;
					}
				}

				
			if (mtlData->IsTypeOf(COMPANY_ID_DOT + CLSID_MtlBlinnData))
			//if (mtlData->IsTypeOf(CLSID_MtlBlinnData))
				{
				MtlBlinnData3D *mtlBlinnData = (MtlBlinnData3D *) mtlData;
				//if (mtlBlinnData->DiffuseMtl->IsTexture())
				//{
				//	Image *img = mtlBlinnData->DiffuseMtl;
				//}
				//else{
				//	Image *img = NULL;
				//}
				Image *img = NULL;
				Color4f diff_color = mtlBlinnData->Diffuse;
				Color4f spec_color = mtlBlinnData->Specular;
				float32 spec_intensity = mtlBlinnData->SpecularIntensity;
				//throw FuException3D("Blinn zeugs: %s ", spec_intensity);
				float32 spec_expo = mtlBlinnData->SpecularExponent;
				float32 opacity = mtlBlinnData->Opacity;
				int32 mtl_ID = mtlBlinnData->GetMaterialID();
				//float mtl_ID = 0;

				
				if (img)
				{
					char *tmpTexTiff = NULL;		// we'll write images from Fusion into the .tiff format
					char *tmpTexTx = NULL;			// and then convert them into this Renderman preferred .tx format

					char *edge = "clamp";		
					//char *edge = "periodic";
	
					char *tiffPathFile, *tiffFile;
					CreateTempFilename(tiffPathFile, tiffFile, "Tex", "tiff");
	
					CheckAbort();
	
					WriteTiffToDisk(img, tiffPathFile);
	
					CheckAbort();
	
					if (mtl.DiffuseType == Material::Tx)
					{
						char *txPathFile, *txFile;
						CreateTempFilename(txPathFile, txFile, "Tex", "tx");

						mtl.DiffuseFile = txFile;
						mtl.DiffusePathFile = txPathFile;
						mtl.spec_color = spec_color;
						mtl.spec_int = spec_intensity;
						mtl.spec_expo = spec_expo;
						mtl.opacity = opacity;
						mtl.diff_color = diff_color;
						mtl.shader_type = 4;
						mtl.mtlID = mtl_ID;

						const float width = 2.0f;		// this is not the sigma of the gaussian but rather the support of the filter kernal
						rm->RiMakeTexture(tiffPathFile, txPathFile, edge, edge, rm->RiGaussianFilter, width, width, RI_NULL);
						CheckAbort();
					}
					else if (mtl.DiffuseType == Material::Tiff)
					{
						mtl.DiffuseFile = tiffFile;
						mtl.DiffusePathFile = tiffPathFile;
					}
				}
				else
				{
					mtl.spec_color = spec_color;
					mtl.spec_int = spec_intensity;
					mtl.spec_expo = spec_expo;
					mtl.opacity = opacity;
					mtl.diff_color = diff_color;
					mtl.shader_type = 4;
					mtl.mtlID = mtl_ID;
				}
			}

				
			
			if (mtlData->IsTypeOf(CLSID_MtlBlinnData))
				{
				MtlBlinnData3D *mtlBlinnData = (MtlBlinnData3D *) mtlData;
				//if (mtlBlinnData->DiffuseMtl->IsTexture())
				//{
				//	Image *img = mtlBlinnData->DiffuseMtl;
				//}
				//else{
				//	Image *img = NULL;
				//}
				Image *img = NULL;
				Color4f diff_color = mtlBlinnData->Diffuse;
				Color4f spec_color = mtlBlinnData->Specular;
				float32 spec_intensity = mtlBlinnData->SpecularIntensity;
				//throw FuException3D("Blinn zeugs: %s ", spec_intensity);
				float32 spec_expo = mtlBlinnData->SpecularExponent;
				float32 opacity = mtlBlinnData->Opacity;
				int32 mtl_ID = mtlBlinnData->GetMaterialID();
				//float mtl_ID = 0;
				
				if (img)
				{
					char *tmpTexTiff = NULL;		// we'll write images from Fusion into the .tiff format
					char *tmpTexTx = NULL;			// and then convert them into this Renderman preferred .tx format

					char *edge = "clamp";		
					//char *edge = "periodic";
	
					char *tiffPathFile, *tiffFile;
					CreateTempFilename(tiffPathFile, tiffFile, "Tex", "tiff");
	
					CheckAbort();
	
					WriteTiffToDisk(img, tiffPathFile);
	
					CheckAbort();
	
					if (mtl.DiffuseType == Material::Tx)
					{
						char *txPathFile, *txFile;
						CreateTempFilename(txPathFile, txFile, "Tex", "tx");

						mtl.DiffuseFile = txFile;
						mtl.DiffusePathFile = txPathFile;
						mtl.spec_color = spec_color;
						mtl.spec_int = spec_intensity;
						mtl.spec_expo = spec_expo;
						mtl.opacity = opacity;
						mtl.diff_color = diff_color;
						mtl.shader_type = 4;
						mtl.mtlID = mtl_ID;

						const float width = 2.0f;		// this is not the sigma of the gaussian but rather the support of the filter kernal
						rm->RiMakeTexture(tiffPathFile, txPathFile, edge, edge, rm->RiGaussianFilter, width, width, RI_NULL);
						CheckAbort();
					}
					else if (mtl.DiffuseType == Material::Tiff)
					{
						mtl.DiffuseFile = tiffFile;
						mtl.DiffusePathFile = tiffPathFile;
					}
				}
				else
				{
					mtl.spec_color = spec_color;
					mtl.spec_int = spec_intensity;
					mtl.spec_expo = spec_expo;
					mtl.opacity = opacity;
					mtl.diff_color = diff_color;
					mtl.shader_type = 4;
					mtl.mtlID = mtl_ID;
				}
			}

			if (mtlData->IsTypeOf(CLSID_MtlWardData))
				{
				
				Image *img = NULL;
				MtlWardData3D *mtlWardData = (MtlWardData3D *) mtlData;
				Color4f diff_color = mtlWardData->Diffuse;
				Color4f spec_color = mtlWardData->Specular;
				float32 spec_intensity = mtlWardData->SpecularIntensity;
				//throw FuException3D("phong zeugs: %s ", spec_intensity);
				float32 spec_expo = mtlWardData->SpecularExponent;
				float32 opacity = mtlWardData->Opacity;
				float32 spreadX = mtlWardData->SpreadX;
				float32 spreadY = mtlWardData->SpreadY;
				int32 mtl_ID = mtlWardData->GetMaterialID();
				//float mtl_ID = 0;
				
					mtl.spec_color = spec_color;
					mtl.spec_int = spec_intensity;
					mtl.spec_expo = spec_expo;
					mtl.opacity = opacity;
					mtl.diff_color = diff_color;
					mtl.xroughness = spreadX;
					mtl.yroughness = spreadY;
					mtl.shader_type = 5;
					mtl.mtlID = mtl_ID;
					//throw FuException3D("ward");
			}

			if (mtlData->IsTypeOf(CLSID_MtlStdData))
				{
					MtlStdData3D *mtlStdData = (MtlStdData3D *) mtlData;
					Image *img = mtlStdData->GetDiffuseTexture();
					Color4f diff_color = mtlStdData->Diffuse;
					Color4f spec_color = mtlStdData->Specular;
					float32 spec_intensity = mtlStdData->SpecularIntensity;
					float32 spec_expo = mtlStdData->SpecularExponent;
					float32 opacity = mtlStdData->Opacity;
					int32 mtl_ID = mtlStdData->GetMaterialID();
					//float mtl_ID = 0;
					//throw FuException3D("mat_id_call_1: %d", mtl_ID);

					if (img)
					{
						char *tmpTexTiff = NULL;		// we'll write images from Fusion into the .tiff format
						char *tmpTexTx = NULL;			// and then convert them into this Renderman preferred .tx format
	
						char *edge = "clamp";		
						//char *edge = "periodic";

						char *tiffPathFile, *tiffFile;
						CreateTempFilename(tiffPathFile, tiffFile, "Tex", "tiff");
						CheckAbort();

						WriteTiffToDisk(img, tiffPathFile);
						CheckAbort();

						if (mtl.DiffuseType == Material::Tx)
							{
								char *txPathFile, *txFile;
								CreateTempFilename(txPathFile, txFile, "Tex", "tx");

								mtl.DiffuseFile = txFile;
								mtl.DiffusePathFile = txPathFile;
								mtl.spec_color = spec_color;
								mtl.spec_int = spec_intensity;
								mtl.spec_expo = spec_expo;
								mtl.opacity = opacity;
								mtl.diff_color = diff_color;
								mtl.shader_type = 1;
								mtl.mtlID = mtl_ID;

								const float width = 2.0f;		// this is not the sigma of the gaussian but rather the support of the filter kernal
								rm->RiMakeTexture(tiffPathFile, txPathFile, edge, edge, rm->RiGaussianFilter, width, width, RI_NULL);
								CheckAbort();
							}
						else if (mtl.DiffuseType == Material::Tiff)
							{
								mtl.DiffuseFile = tiffFile;
								mtl.DiffusePathFile = tiffPathFile;
							}
					}
				else
				{
					mtl.spec_color = spec_color;
					mtl.spec_int = spec_intensity;
					mtl.spec_expo = spec_expo;
					mtl.opacity = opacity;
					mtl.diff_color = diff_color;
					mtl.shader_type = 1;
					mtl.mtlID = mtl_ID;
				}
				/*if (mtlData->IsTypeOf(CLSID_MtlFog3D))
				{
					MtlFogData3D *mtlFogData = (MtlFogData3D *) mtlData;
					Color4f fog_color = mtlFogData->FogColor;
					float32 fog_far_dist = mtlFogData->FarDist;
					float32 fog_near_dist = mtlFogData->NearDist;
					mtl.fogcolor = fog_color;
					mtl.fogmaxdist = fog_far_dist;
					mtl.fogmindist = fog_near_dist;
					mtl.shader_type = 6;
				}*/
			}
		}
	}
	
}

void RendererRIB3D::CreateLightLists()
{
	// This is going to be trickiest bit of the FusionScene -> RendermanScene conversion process.  The problem is caused by the difference
	// in how Fusion and Renderman control which lights affect which surfaces.  If you're not worried about maintaining which lights affect 
	// which surfaces you can skip a lot of this stuff.
	// * In Renderman the renderer maintains an active light list and we turn lights on/off as we render each surface.  This means we need
	//   to maintain a lightlist for each geometry node in the scene containing the lights to be used while rendering that surface.
	// * In Fusion there are no lightlists, but rather the lights are part of the Scene3D and illuminate any surface nodes connecting to
	//   the first Merge3D downstream of the light node.  If the first downstream Merge3D has "Pass Through Lights" enabled the lighting
	//   will proceed down the second downstream Merge3D and so forth. 
	// What we're going to do in this function is analyze Fusions Scene3D and we're going to build up the list of lights that affects each 
	// surface.  Since we can't modify the Scene3D to store this information (as its a refcounted shared object) we'll need to build up a 
	// separate "database" of surface & light information.  We'll construct a Light/Surface wrapper object for each light/surface in the 
	// Scene3D and store our information in those.
		/*if (DoPhotons)
		{
			rm->RiTransformBegin();	
			rm->RiIdentity();
			PhtotonIndirect.Node = NULL;
			float32 samples = PhotonRays;
			if (Proxy>1) samples /= Proxy;
			float32 fg = 1.0;
			float32 strength = PhotonStrength;
			float32 maxdist = PhotonDist;
			const int nParams = 4;
			RtToken tokens[nParams] = { 
				"float samples",
				"float finalgather",
				"float intensity",
				"float maxdist"};
			RtPointer params[nParams] = { 
				&samples, 
				&fg, 
				&strength,
				&maxdist,	
			};
			PhtotonIndirect.RmLight = rm->RiLightSourceV("roe_indirectlight", nParams, tokens, params);
			rm->RiTransformEnd();
			rm->RiIlluminate(PhtotonIndirect.RmLight,RI_TRUE);
		}*/

   // Create a full intensity ambient light.  If global lighting is off we'll toggle the fullAmbient and disable any other lights in the scene.
	rm->RiTransformBegin();	
	rm->RiIdentity();

   RtFloat intensity = 1.0;
   Vector3f lightColor(1.0f, 1.0f, 1.0f);
   RtToken tokens[] = { "intensity", "lightcolor" };
   RtPointer params[] = { &intensity, lightColor.V };
	FullAmbient.Node = NULL;
   FullAmbient.RmLight = rm->RiLightSourceV("ambientlight", 2, tokens, params);

	rm->RiTransformEnd();
   rm->RiIlluminate(FullAmbient.RmLight, RI_FALSE);


	// create the LightList
	if (LightingEnabled)
	{
		NumLights = Scene->LightList.size();
		LightList = new Light[NumLights];

		SceneItor3D itor;

				
		// create the renderman lights
		for (int i = 0; i < NumLights; i++)
		{
			int nodeIndex = Scene->LightList[i];
			NodeLUT[nodeIndex] = i;

			Light &light = LightList[i];
			light.Node = Scene->NodeList[nodeIndex];

			LightData3D *fuLight = (LightData3D *) (Data3D *) light.Node->Data;

			if (!fuLight->IsLightProjector())					// going to handle light projectors
			{
				rm->RiAttributeBegin();
				FuID light_name = light.Node->GetName();
				
				// Renderman specification has four standard light types which happen to correspond to Fusions standard lights.  
				// By examining the lights registry node we can determine which type it is.
				const FuID &lightID = light.Node->Data->m_RegNode->m_ID;
				
				RtToken name_token[] = { "string name"  };
				RtPointer name_params[] = { &light_name };
				rm->RiAttributeV("identifier",1,name_token, name_params);
				//checking if light is coming from an actual light or from an env map. only "real" lights should emitt photons
				/*if ((lightID == CLSID_Light_Point_Data)||(lightID == CLSID_Light_Directional_Data)||(lightID == CLSID_Light_Spot_Data)||(COMPANY_ID_DOT + CLSID_Light_Directional_Data)||(COMPANY_ID_DOT + CLSID_Light_Point_Data)){
					if (DoPhotons){
						char *pho_number1 = "on";
						RtToken Ptokens4[] = { "string emitphotons" };
						RtPointer Pparams4[] = { &pho_number1 };
						rm->RiAttributeV("light", 1, Ptokens4, Pparams4);
					}
				}*/
				rm->RiTransformBegin();								// push the current transform
				rm->RiIdentity();										// make sure we are specifying the lights transform wrt world space
				ConcatTransform(light.Node->ModelToWorld);	// setup the lights transform

			
				if (lightID == CLSID_Light_Ambient_Data)
					light.RmLight = CreateAmbientLight(light.Node);			// remember their handles so we can enable/disable them per surface node
				else if (lightID == CLSID_Light_Directional_Data)
				{
					light.RmLight = CreateDirectionalLight(light.Node);
				}
				else if (lightID == COMPANY_ID_DOT + CLSID_Light_Directional_Data)
				{
					light.RmLight = CreateDirectionalLight2(light.Node);
				}
				else if (lightID == CLSID_Light_Point_Data)
				{
					light.RmLight = CreatePointLight(light.Node);
				}
				else if (lightID == COMPANY_ID_DOT + CLSID_Light_Point_Data)
				{
					light.RmLight = CreatePointLight2(light.Node);
				}
				else if (lightID == CLSID_Light_Spot_Data)
				{
					light.RmLight = CreateSpotLight(light.Node);
				}
				else if (lightID == COMPANY_ID_DOT + CLSID_Light_Env_Data)
				{
					light.RmLight = CreateEnvLight(light.Node);
				}
								
				rm->RiTransformEnd();								// restore the transform
				
				rm->RiAttributeEnd();
				
				rm->RiIlluminate(light.RmLight, RI_FALSE);	// default the light off - we'll turn it on per surface when its needed

				// The "top" node for a light is the first parent merge node of the light that doesn't have "pass through lights" enabled.
				// All nodes below the top node are affected by the light.
				Node3D *top = light.Node->GetLightTopNode();

				// Traverse all the nodes below the top node and add this light to their lightlists.
				Node3D *n = itor.BeginTraversal(top);

				while (n)
				{
					if (n->NodeType == NDET_Surface || n->NodeType == NDET_Curve)
					{
						Data3D *data = (Data3D *) (Object *) n->Data;
						Surface *surface = GetSurfaceInfo(n);

						if (data && surface)
						{
							if (data->IsAffectedByLights)
								surface->LightList.push_back(&light);				// push this light onto the surfaces lightlist
							else if (surface->LightList.size() == 0)				// only put the FullAmbient there once
								surface->LightList.push_back(&FullAmbient);
						}
					}

					n = itor.NextNode();
				}
				itor.EndTraversal();
			}
			if (fuLight->IsLightProjector())					// going to handle light projectors in this exporter
			{
				
				rm->RiAttributeBegin();
				FuID light_name = light.Node->GetName();
				RtToken name_token[] = { "string name"  };
				RtPointer name_params[] = { &light_name };
				rm->RiAttributeV("identifier",1,name_token, name_params);
				
				rm->RiTransformBegin();								// push the current transform
				
				rm->RiIdentity();// make sure we are specifying the lights transform wrt world space

				ConcatTransform(light.Node->ModelToWorld);	// setup the lights transform
								
				light.RmLight = CreateProjectorLight(light.Node);			// remember their handles so we can enable/disable them per surface node
				
				rm->RiTransformEnd();								// restore the transform

				rm->RiAttributeEnd();
				
				rm->RiIlluminate(light.RmLight, RI_FALSE);	// default the light off - we'll turn it on per surface when its needed

				// The "top" node for a light is the first parent merge node of the light that doesn't have "pass through lights" enabled.
				// All nodes below the top node are affected by the light.
				Node3D *top = light.Node->GetLightTopNode();

				// Traverse all the nodes below the top node and add this light to their lightlists.
				Node3D *n = itor.BeginTraversal(top);

				while (n)
				{
					if (n->NodeType == NDET_Surface || n->NodeType == NDET_Curve)
					{
						Data3D *data = (Data3D *) (Object *) n->Data;
						Surface *surface = GetSurfaceInfo(n);

						if (data && surface)
						{
							if (data->IsAffectedByLights)
								surface->LightList.push_back(&light);				// push this light onto the surfaces lightlist
							else if (surface->LightList.size() == 0)				// only put the FullAmbient there once
								surface->LightList.push_back(&FullAmbient);
						}
					}

					n = itor.NextNode();
				}
				itor.EndTraversal();
			}


		}
	}
	else
	{
		// lighting is disabled in the renderer - render all objects with a full intensity ambient light
		NumLights = 0;
		LightList = NULL;

		for (int i = 0; i < NumSurfaces; i++)
			SurfaceList[i].LightList.push_back(&FullAmbient);
	}

	//TODO - texture projectors - what do we do?
}

void RendererRIB3D::FreeLightLists()
{
	SAFE_DELETE_ARRAY(LightList);
}


void RendererRIB3D::CreateGeoLists()
{
	// create the GEoList
		NumGeo = Scene->GeomList.size();
		SurfaceList = new Surface[NumGeo];

		SceneItor3D itor;

		std::vector<Geometry3D*> geometry;	
		// create the renderman reference Geo
		for (int i = 0; i < NumGeo; i++)
		{
			int nodeIndex = Scene->GeomList[i];
			NodeLUT[nodeIndex] = i;

			Surface &geo_node = SurfaceList[i];
			geo_node.Node = Scene->NodeList[nodeIndex];
			
			//LightData3D *fuLight = (LightData3D *) (Data3D *) light.Node->Data;

				
			//FuID node_name = geo_node.Node->GetName();
				const char *short_name = geo_node.Node->GetName();
				//if (i>0) {throw FuException3D("current node %s", short_name);}
				RtToken OBJname_token[] = { "string __handleid" };
				RtPointer OBJname_params[] = { (RtPointer ) &short_name };
				rm->RiObjectBeginV(1,OBJname_token, OBJname_params);
				
				Data3D *data = (Data3D *) (Object *) geo_node.Node->Data;
				
			   if (data && data->IsVisible && !data->IsUnseenByCamera)
			   {
					// handle CullFrontFace/CullBackFace
					/*int nSides = 2;
					rm->RiArchiveRecord("comment",data->m_RegNode->m_Name );
					rm->RiOrientation("outside");

					if (data->CullBackFace)
						nSides--;

					if (data->CullFrontFace)
					{
						rm->RiOrientation("inside");
						nSides--;
					}

					if (nSides == 0)
						return;

					rm->RiSides(nSides);*/

				
				//rm->RiTransformBegin();								// push the current transform
				//rm->RiIdentity();										// make sure we are specifying the lights transform wrt world space
				//ConcatTransform(geo_node.Node->ModelToWorld);	// setup the lights transform						
				
				 RenderGeometry( geo_node.Node->GetGeometry(0,false));	
				//RenderGeometry(geometry[i]);	
				//RenderGeometry(geo_node.Node->GetGeometry(i, false));
				//rm->RiTransformEnd();								// restore the transform
				rm->RiObjectEnd();
			   }
		}
}

void RendererRIB3D::FreeSurfaceLists()
{
	SAFE_DELETE_ARRAY(SurfaceList);
}		


RtLightHandle RendererRIB3D::CreateAmbientLight(Node3D *n)
{
	// Here we'll create a renderman ambient light from a Fusion one.
	LightData3D *fuLight = (LightData3D *) (Object *) n->Data;

	const int nParams = 2;

   RtFloat intensity = 1.0;

	RtToken tokens[nParams] = 
	{ 
		"float intensity", 
		"color lightcolor", 
	};

   RtPointer params[nParams] = 
	{ 
		&intensity, 
		fuLight->Color.V, 
	};

	return rm->RiLightSourceV("ambientlight", nParams, tokens, params);
}

RtLightHandle RendererRIB3D::CreateEnvLight(Node3D *n)
{
	// Here we'll create a renderman directional light from a Fusion one.
	LightData3D *fuLight = (LightData3D *) (Object *) n->Data;

	const int nParams = 6;

	//////////////////should be in upper class
	float EnvLSamples = fuLight->env_samples;
	if (Proxy>1) EnvLSamples /= Proxy;
	float kenv = fuLight->env_multi;
	float kocc = fuLight->env_occ;
	Vector3f from(0.0f, 0.0f, 0.0f);				// directional lights in Fusion always point down their negative z axis in local coordinates
	Vector3f to(0.0f, 0.0f, -1.0f);
	//Color4f light_color = fuLight->Diffuse.V;



   RtToken tokens[nParams] = 
	{  
		"float samples", 
		"string envmap", 
		"float Kenv",
		"float Kocc",
		"point to",
		"point from",
		//"light_color",
	};

   RtPointer params[nParams] = 
	{  
		&EnvLSamples,
		&txPathFile,
		&kenv,
		&kocc,
		to.V,
		from.V,
		//&light_color,
	};

	return rm->RiLightSourceV("envlight3", nParams, tokens, params);
}



RtLightHandle RendererRIB3D::CreateDirectionalLight2(Node3D *n)
{
	// Here we'll create a renderman directional light from a Fusion one.
	LightData3D *fuLight = (LightData3D *) (Object *) n->Data;

	const int nParams = 7;

   RtFloat intensity = 1.0;
   Vector3f lightColor(1.0f, 1.0f, 1.0f);
	Vector3f from(0.0f, 0.0f, 0.0f);				// directional lights in Fusion always point down their negative z axis in local coordinates
	Vector3f to(0.0f, 0.0f, -1.0f);
	float32 samples = RaySamples;
	if (Proxy>1) samples /= Proxy;
	char *shadowname = "raytrace";

	Input *p_blur_in = fuLight->m_Operator->FindInput("Blur");
	float blur=0.5;
	if ((p_blur_in))
	{
		Parameter *p = p_blur_in->GetSource(Document->GetCurrentTime());
		blur = *(Number *)p;
		p->Recycle();
	}

   RtToken tokens[nParams] = 
	{ 
		"float intensity", 
		"color lightcolor", 
		"point from", 
		"point to",
		"string shadowname",
		"float samples",
		"float blur"
	};

   RtPointer params[nParams] = 
	{ 
		&intensity, 
		fuLight->Color.V, 
		from.V, 
		to.V,
		&shadowname,
		&samples,
		&blur
	};

	return rm->RiLightSourceV("roe_distantshadow", nParams, tokens, params);
}

RtLightHandle RendererRIB3D::CreateDirectionalLight(Node3D *n)
{
	// Here we'll create a renderman directional light from a Fusion one.
	LightData3D *fuLight = (LightData3D *) (Object *) n->Data;

	const int nParams = 6;

   RtFloat intensity = 1.0;
   Vector3f lightColor(1.0f, 1.0f, 1.0f);
	Vector3f from(0.0f, 0.0f, 0.0f);				// directional lights in Fusion always point down their negative z axis in local coordinates
	Vector3f to(0.0f, 0.0f, -1.0f);
	float32 samples = RaySamples;
	if (Proxy>1) samples /= Proxy;
	char *rayshadows = "raytrace";

   RtToken tokens[nParams] = 
	{ 
		"float intensity", 
		"color lightcolor", 
		"point from", 
		"point to",
		"string shadowname",
		"float samples",
	};

   RtPointer params[nParams] = 
	{ 
		&intensity, 
		fuLight->Color.V, 
		from.V, 
		to.V,
		&rayshadows,
		&samples,
	};

	return rm->RiLightSourceV("h_distantshadow", nParams, tokens, params);
}


RtLightHandle RendererRIB3D::CreatePointLight(Node3D *n)
{
	// Here we will create a renderman point light from a Fusion one - note that we cannot reproduce the const/linear/quadratic 
	// falloff terms of Fusions point light without a custom shader.  The default renderman "pointlight" assumes c = 0 and q = 0
	// and l = 1 in Fusions light falloff equation:  falloff = 1.0f / (c + l * dist + q * dist^2)
	LightData3D *fuLight = (LightData3D *) (Object *) n->Data;

	// TODO - remove position from LightPointData3D and direction for directional lights

	const int nParams = 4;

   RtFloat intensity = 1.0;
   Vector3f lightPosition(0.0f, 0.0f, 0.0f);		// point lights always sit at the origin in their own local coordinate system 	
	float decay_type = 2.0;
	if (fuLight->QuadraticDecay)
		{
			decay_type = 2.0;
		}
	else if (fuLight->LinearDecay)
		{
			decay_type = 1.0;
		}
	else if (fuLight->ConstantDecay)
		{
			decay_type = 0.0;
		}

   RtToken tokens[nParams] = 
	{ 
		"float intensity", 
		"color lightcolor", 
		"point from" ,
		"float decay"
	};

   RtPointer params[nParams] = 
	{ 
		&intensity, 
		fuLight->Color.V, 
		lightPosition.V ,
		&decay_type
	};

	return rm->RiLightSourceV("pointlight2", nParams, tokens, params);
}
RtLightHandle RendererRIB3D::CreatePointLight2(Node3D *n)
{
	// Here we will create a renderman point light from a Fusion one - note that we cannot reproduce the const/linear/quadratic 
	// falloff terms of Fusions point light without a custom shader.  The default renderman "pointlight" assumes c = 0 and q = 0
	// and l = 1 in Fusions light falloff equation:  falloff = 1.0f / (c + l * dist + q * dist^2)
	LightData3D *fuLight = (LightData3D *) (Object *) n->Data;

	// TODO - remove position from LightPointData3D and direction for directional lights

	const int nParams = 7;

   RtFloat intensity = 1.0;
   Vector3f lightPosition(0.0f, 0.0f, 0.0f);		// point lights always sit at the origin in their own local coordinate system 	
	float decay_type = 2.0;
	if (fuLight->QuadraticDecay)
		{
			decay_type = 2.0;
		}
	else if (fuLight->LinearDecay)
		{
			decay_type = 1.0;
		}
	else if (fuLight->ConstantDecay)
		{
			decay_type = 0.0;
		}
	float32 samples = RaySamples;
	if (Proxy>1) samples /= Proxy;
	float shadow_on = 0.0;
	if (ShadowsEnabled==true)
		{
			shadow_on = 1.0;
		}
	Input *p_blur_in = fuLight->m_Operator->FindInput("Blur");
	float blur=0.5;
	if ((p_blur_in))
	{
		Parameter *p = p_blur_in->GetSource(Document->GetCurrentTime());
		blur = *(Number *)p;
		p->Recycle();
	}
   RtToken tokens[nParams] = 
	{ 
		"float intensity", 
		"color lightcolor", 
		"point from" ,
		"float decay",
		"float samples",
		"float shadow_on",
		"float blur"
   };

   RtPointer params[nParams] = 
	{ 
		&intensity, 
		fuLight->Color.V, 
		lightPosition.V ,
		&decay_type,
		&samples,
		&shadow_on,
		&blur,
	};

	return rm->RiLightSourceV("pointlight3", nParams, tokens, params);
}


RtLightHandle RendererRIB3D::CreateSpotLight(Node3D *n)
{
   // Note that the renderman spotlight is different from fusionlands.  The RM spotlight doesn't have an attenuation factor (it is hardcoded
	// to be 1/dist) so exported fusion spotlights will look a lot dimmer unless you bump up their intensity.  Also the dropoff between
	// the inner/outer cones may be computed differently.

   // Also we're going to ignore the shadow settings on the spotlight. ????

	LightData3D *fuLight = (LightData3D *) (Object *) n->Data;
	int shadow_on;
	Input *p_shad_in = fuLight->m_Operator->FindInput("ShadowLightInputs3D.ShadowsEnabled");
	if ((p_shad_in))
	{
		Parameter *p = p_shad_in->GetSource(Document->GetCurrentTime());
		shadow_on = *(Number *)p;
		p->Recycle();
	}

	RtFloat intensity = 1.0;
	Vector3f from(0.0f, 0.0f, 0.0f);
	Vector3f to(0.0f, 0.0f, -1.0f);
	float32 coneAngle = DegToRad(fuLight->ConeAngle);
	float32 coneDeltaAngle = DegToRad(fuLight->PenumbraAngle);
	float32 beamDistribution = fuLight->Dropoff;
	float32 samples = RaySamples;
	if (Proxy>1) samples /= Proxy;
	char *rayshadows = "raytrace"; 
	//float32 Sblur = 0.05;
	float decay_type = 2.0;
	if (fuLight->QuadraticDecay)
	{
		decay_type = 2.0;
	}
	else if (fuLight->LinearDecay)
	{
		decay_type = 1.0;
	}
	else if (fuLight->ConstantDecay)
	{
		decay_type = 0.0;
	}
	if ((shadow_on == 1)&&(ShadowsEnabled == true))
	{
	const int nParams = 9;

	RtToken tokens[nParams] = 
		{ 
			"float intensity", 
			"color lightcolor", 
			"point from",
			"point to",
			"float coneangle",
			"float conedeltaangle",
			"float beamdistribution",
			"string shadowmap",
			//"float samples",
			"float decay",
		//	"blur",
		};

	   RtPointer params[nParams] = 
		{ 
			&intensity, 
			fuLight->Color.V,
			from.V,
			to.V,
			&coneAngle,
			&coneDeltaAngle,
			&beamDistribution,
			&rayshadows,
			//&samples,
			&decay_type,
		//	&Sblur,
		};
	   return rm->RiLightSourceV("spotlight", nParams, tokens, params);
	}
	else
	{
		const int nParams = 8;
		RtToken tokens[nParams] = 
		{ 
			"float intensity", 
			"color lightcolor", 
			"point from",
			"point to",
			"float coneangle",
			"float conedeltaangle",
			"float beamdistribution",
			"float decay",
		//	"shadowmap",
		//	"samples",
		//	"blur",
		};

	   RtPointer params[nParams] = 
		{ 
			&intensity, 
			fuLight->Color.V, 
			from.V,
			to.V,
			&coneAngle,
			&coneDeltaAngle,
			&beamDistribution,
			&decay_type,
		//	&rayshadows,
		//	&samples,
		//	&Sblur,
		};
   return rm->RiLightSourceV("spotlight", nParams, tokens, params);
	}
};

RtLightHandle RendererRIB3D::CreateProjectorLight(Node3D *n)
{
   // Note that the renderman spotlight is different from fusionlands.  The RM spotlight doesn't have an attenuation factor (it is hardcoded
	// to be 1/dist) so exported fusion spotlights will look a lot dimmer unless you bump up their intensity.  Also the dropoff between
	// the inner/outer cones may be computed differently.

   // Also we're going to ignore the shadow settings on the spotlight. ????

	LightData3D *fuLight = (LightData3D *) (Object *) n->Data;
	
	int shadow_on;
	Input *p_shad_in = fuLight->m_Operator->FindInput("ShadowLightInputs3D.ShadowsEnabled");
	if ((p_shad_in))
	{
		Parameter *p = p_shad_in->GetSource(Document->GetCurrentTime());
		shadow_on = *(Number *)p;
		p->Recycle();
	}	
	RtFloat intensity = 1.0;
	Input *fit_inp =  fuLight->m_Operator->FindInput("Fit");
	if (fit_inp)
	{
		Parameter *p = fit_inp->GetSource(Document->GetCurrentTime());
		if (p)
		{
			fit_type = *(Text *) p;	
			p->Recycle();
		}
	}
	
	float decay_type = 2.0;
	if (fuLight->QuadraticDecay)
	{
		decay_type = 2.0;
	}
	else if (fuLight->LinearDecay)
	{
		decay_type = 1.0;
	}
	else if (fuLight->ConstantDecay)
	{
		decay_type = 0.0;
	}
	
	float32 beamDistribution = fuLight->Dropoff;

	Vector3f from(0.0f, 0.0f, 0.0f);
	Vector3f to(0.0f, 0.0f, -1.0f);
	float32 angle;
	Input *p_angle_in = fuLight->m_Operator->FindInput("Angle");
	if (p_angle_in)
	{
		Parameter *p = p_angle_in->GetSource(Document->GetCurrentTime());
		angle = *(Number *)p;
		p->Recycle();
	}

	float32 proj_intensity;
	Input *p_intensity_in = fuLight->m_Operator->FindInput("Intensity");
	if (p_intensity_in)
	{
		Parameter *p = p_intensity_in->GetSource(Document->GetCurrentTime());
		proj_intensity = *(Number *)p;
		p->Recycle();
	}
	intensity = proj_intensity;
	Image *pim = NULL;
	Input *inp = fuLight->m_Operator->FindInput("ProjectiveImage");
	if (inp)
	{
		Parameter *p = inp->GetSource(Document->GetCurrentTime());
		if (p)
		{
			pim = (Image *)p;
			p->Recycle();
		}
	}
	if (pim)
	{
		char *tmpTexTiff = NULL;		// we'll write images from Fusion into the .tiff format
		char *tmpTexTx = NULL;			// and then convert them into this Renderman preferred .tx form
		char *edge = "clamp";		
		char *repeat = "periodic";
		char *black = "black";
	
		char *tiffPathFile_pro, *tiffFile_pro;
		CreateTempFilename(tiffPathFile_pro, tiffFile_pro, "Proj", "tiff");

		CheckAbort();
	
		WriteTiffToDisk(pim, tiffPathFile_pro);
	
		CheckAbort();

		CreateTempFilename(txPathFile_pro, txFile_pro, "Proj", "tx");

		const float width = 2.0f;		// this is not the sigma of the gaussian but rather the support of the filter kernal
		rm->RiMakeTexture(tiffPathFile_pro, txPathFile_pro, black, black, rm->RiGaussianFilter, width, width, RI_NULL);
		CheckAbort();
	}
		
	char *rayshadows = "raytrace"; 
	float32 samples = RaySamples;
	if (Proxy>1) samples /= Proxy;
	float32 img_height = (float32) pim->Height;
	float32 img_width = (float32) pim->Width;
	float32 aspect = img_width/img_height;
		
	Vector3f up(0,1,0);
	const char *filename = txPathFile_pro;
	if ((shadow_on == 1)&&(ShadowsEnabled == true))
	{
		const int nParams = 9;
		RtToken tokens[nParams] = 
		{ 
			"fov",
			//"up",
			"aspect",
			"intensity",
			"fit_type",
			"slidename",
			//"from",
			//"to",
			"float beamdistribution",
			"float decay",
			"string shadowmap",
			"float samples",
		};

	   RtPointer params[nParams] = 
		{ 
			&angle,
			//&up.V,
			&aspect,
			&intensity,
			(void *) &fit_type,
			(void *) &filename,
			//from.V,
			//to.V,
			&beamDistribution,
			&decay_type,
			&rayshadows,
			&samples,
		};
	   return rm->RiLightSourceV("roe_slideprojector_2", nParams, tokens, params);
	}
	else
	{
		const int nParams = 7;
		RtToken tokens[nParams] = 
		{ 
			"float fov",
			//"up",
			"float aspect",
			"float intensity",
			"string fit_type",
			"string slidename",
			//"from",
			//"to",
			"float beamdistribution",
			"float decay",
		};

	   RtPointer params[nParams] = 
		{ 
			&angle,
			//&up.V,
			&aspect,
			&intensity,
			(void *) &fit_type,
			(void *) &filename,
			//from.V,
			//to.V,
			&beamDistribution,
			&decay_type,
		};
	   return rm->RiLightSourceV("roe_slideprojector_2", nParams, tokens, params);
	}
  
};


RendererRIB3D::Surface *RendererRIB3D::GetSurfaceInfo(const Node3D *n)
{
	FuASSERT(n->NodeType == NDET_Surface || n->NodeType == NDET_Curve, (""));

	int surfaceIndex = NodeLUT[n->NodeIndex];
	return surfaceIndex != -1 ? SurfaceList + surfaceIndex : NULL;
}

RendererRIB3D::Light *RendererRIB3D::GetLightInfo(const Node3D *n)
{
	FuASSERT(n->NodeType == NDET_Light, (""));

	int lightIndex = NodeLUT[n->NodeIndex];
	return lightIndex != -1 ? LightList + lightIndex : NULL;
}

void RendererRIB3D::ActivateLights(Node3D *n)
{
	if (n->NodeType == NDET_Surface || n->NodeType == NDET_Curve)
	{
		Surface *surface = GetSurfaceInfo(n);
	
		if (surface)
		{
			std::vector<Light*>::iterator i    = surface->LightList.begin();
			std::vector<Light*>::iterator iEnd = surface->LightList.end();

			while (i != iEnd)
				rm->RiIlluminate((*i++)->RmLight, RI_TRUE);
		}
	}
}

void RendererRIB3D::DeactivateLights(Node3D *n)
{
	if (n->NodeType == NDET_Surface || n->NodeType == NDET_Curve)
	{
		Surface *surface = GetSurfaceInfo(n);
	
		if (surface)
		{
			std::vector<Light*>::iterator i    = surface->LightList.begin();
			std::vector<Light*>::iterator iEnd = surface->LightList.end();

			while (i != iEnd)
				rm->RiIlluminate((*i++)->RmLight, RI_FALSE);
		}
	}
}

void RendererRIB3D::RenderNode(Node3D *n)
{
   if (n)
   {
	  rm->RiAttributeBegin();
	  //if (n->IsRenderable)
	  //{
		FuID node_name = n->GetName();
		const char *nodenamestring = n->GetName();
		int name_number = 1;
		RtToken name_tokens2[] = { "string name" };
		RtPointer name_params2[] = { &node_name };
		rm->RiAttributeV("identifier",1, name_tokens2, name_params2);
		//}

		if (DoMoBlur)
		{
			RtInt MBSamples = (RtInt) shutterSamples;
			//RtFloat MBTimes[6];
			for (int i = 0; i <= shutterSamples; i++)
			{
				MBTimes[i] = i*(1/shutterSamples);
			}
			rm->RiMotionBeginV(MBSamples,MBTimes);
			for (int i = 0; i < shutterSamples; i++)
			{ 
				MBTimes[i] = (Scene->Time) + i*(1/shutterSamples);
				second_scene = (Scene3D *) SceneInput->GetSource(MBTimes[i]);
				if (second_scene){

					second_scene->Setup();
					Matrix4f new_transform = second_scene->GetNodeByID(new_geo,n->ID,n->SubID)->M;
					ConcatTransform(new_transform);
				}
				else{
					throw FuException3D("scene_error");
				}
			}
			rm->RiMotionEnd();
		}
		else
		{
			rm->RiTransformBegin();				// push the current transform
			ConcatTransform(n->M);				// M transforms vertices in this node into its parent's coordinate system
		}
      if (n->NodeType == NDET_Surface || n->NodeType == NDET_Curve)
         RenderSurface(n, nodenamestring);
	  

      RenderNode(n->Child);
	  //if(DoMoBlur) rm->RiMotionEnd;
	  //else rm->RiTransformEnd();				// restore the pushed transform
	  rm->RiTransformEnd();
	  rm->RiAttributeEnd();
      RenderNode(n->Next);
   }
}

/*
bool SceneInfo::GetCurveGeometry(std::vector<Geometry3D*> &geometry, NodeInfo *ln)
{
	CurveData3D *curveData = (CurveData3D *)(Data3D *) ln->m_Node->Data;

	Curve3D *curve;
	if (curveData->CachedCurve)
	{
		curve = curveData->CachedCurve;
		curve->Use();	// because we're going to recycle it below
	}
	else
	{
		RenderContextGL3D rc;
		curve = curveData->Render(rc);	// don't render it under the lock

		curveData->CachedCurveLock.ObtainLock();

		if (curveData->CachedCurve == (Curve3D *) NULL)
			curveData->CachedCurve = curve;
		else
		{
			curve->Recycle();
			curve = curveData->CachedCurve;
			curve->Use();
		}
			
		curveData->CachedCurveLock.ReleaseLock();
	}

	if (!curve)
		return false;

	if (curve->CurveFillMode == PFM_Filled)
	{
		curve->Tesselate();	// TODO - handle failure
		
		curveData->TesselationCacheLock.ObtainLock();
		if (curveData->TesselationCache == (TessArray *) NULL)
		{
			curveData->TesselationCache = curve->TesselationCache;			// share the tesselation cache with the original data object so it may get reused later
			curveData->TesselationCache->Use();
		}
		curveData->TesselationCacheLock.ReleaseLock();

		Geometry3D *geom;
		if (geom = curve->GenerateGeometry())
		{
			geom->Setup();
			geometry.push_back(geom);
		}
		else
			return false;
	}
	else
	{
		// TODO
		//if (rc.GlobalPolyMode == PR_Wireframe && curve->CurveRenderMode == PR_Filled)
		//	localNode->PolygonRenderMode = PR_Wireframe;
		//else if (rc.GlobalPolyMode == PR_Points && curve->CurveRenderMode != PR_Points)		// TODO - does glPolygonMode() affect lines or only polgyons? If so, this is ignored.
		//	localNode->PolygonRenderMode = PR_Points;
		//else
		//	localNode->PolygonRenderMode = curve->CurveRenderMode;

		//Renderables.push_back(localNode);
	}

	curve->Recycle();
	curve = NULL;

	return true;
}
*/

void RendererRIB3D::RenderSurface(Node3D *n, const char *nodenamestring)
{
   Data3D *data = (Data3D *) (Object *) n->Data;
   if (data && data->IsVisible && !data->IsUnseenByCamera)
   {
		// handle CullFrontFace/CullBackFace
		int nSides = 2;
		rm->RiArchiveRecord("comment",data->m_RegNode->m_Name );
		rm->RiOrientation("outside");

		if (data->CullBackFace)
			nSides--;

		if (data->CullFrontFace)
		{
			rm->RiOrientation("inside");
			nSides--;
		}

		if (nSides == 0)
			return;

		rm->RiSides(nSides);
		

      // deactivate lights affecting this node
      ActivateLights(n);

		int surfaceIdx = NodeLUT[n->NodeIndex];
		if (surfaceIdx == -1)
			return;

		Surface &surface = SurfaceList[surfaceIdx];

      // output geometry & material
      std::vector<Geometry3D*> geometry;

		bool success = true;

		if (n->NodeType == NDET_Curve)
			success = false;//GetCurveGeometry(geometry, n);
		else if (n->NodeType == NDET_Surface)
		{
			SurfaceData3D *surfaceData = (SurfaceData3D *) data;
			success = surfaceData->GetGeometry(geometry);
		}
      if (success)
      {
		   //rm->RiBound(); 			// output bounding box TODO

			int nMtls = surface.MaterialList.size();
			
         int nGeom = geometry.size();
		
         for (int i = 0; i < nGeom; i++)
			{
				if (i < nMtls)
					RenderMaterial(surface.MaterialList[i]);
				RtObjectHandle OBJname_params = (RtObjectHandle *) nodenamestring;
				rm->RiObjectInstance(OBJname_params);
			}
      }

      // deactivate lights affecting this node
      DeactivateLights(n);

      // the returned geometry must be recycled after we're done with it
      SAFE_RECYCLE_VECTOR(geometry);
   
}
}

void RendererRIB3D::RenderMaterial(Material *mtl)
{
	if ((mtl->IsSupported)&&(mtl->shader_type==2))
	{
		float Ka = 1.0f;
		float Kd = 1.0f;
		float Ks = mtl->spec_int;
		float roughness = 1/mtl->spec_expo;
		Color4f specularcolor = mtl->spec_color;
		const char *filename = mtl->DiffuseFile;

		
		
		float occ_samples = AOSamples;
		if (Proxy>1) occ_samples /= Proxy;
		float occ_dist = AODepth;
		float Do_AO = DoAO;

		Color4f Cs = mtl->diff_color;
		Color4f Os (mtl->opacity);
		float32 samples = RaySamples;
		if (Proxy>1) samples /= Proxy;

		float mtl_ID = mtl->mtlID;
		//float mtl_ID = 0;
		
		
		const int n = 10;
		
		RtToken tokens[n] =
		{
			"float Kd",
			"float Ks",
			"float Ka",
			"float roughness",
			"color specularcolor",
			"float occ_samples",
			"float maxdist",
			//"samples",
			//"envname",
			"float DoAO",
			"float materialID",
			"string diff_mapname",
			
		};
		
		RtPointer params[n] =
		{
			&Kd,
			&Ks,
			&Ka,
			&roughness,
			specularcolor.V,
			&occ_samples,
			&occ_dist,
			//&samples,
			//&txPathFile,
			&Do_AO,
			&mtl_ID,
			(void *) &filename,	
			
		};
		rm->RiDisplacement("",RI_NULL);
		rm->RiColor(Cs.V);
		rm->RiOpacity(Os.V);
		rm->RiSurfaceV("fu_3de_phong2", filename ? n : n - 1, tokens, params );
		
	}
	else if ((mtl->IsSupported)&&(mtl->shader_type==1)) 
	{
		float Ka = 1.0f;
		float Kd = 1.0f;
		float Ks = mtl->spec_int;
		float roughness = 1/mtl->spec_expo;
		Color4f specularcolor = mtl->spec_color;
		const char *filename = mtl->DiffuseFile;
		
		float occ_samples = AOSamples;
		if (Proxy>1) occ_samples /= Proxy;
		float occ_dist = AODepth;
		float Do_AO = DoAO;

		Color4f Cs = mtl->diff_color;
		Color4f Os (mtl->opacity);
		
		float mtl_ID = mtl->mtlID;
		//float mtl_ID = 0;
		

		const int n = 10;
		
		RtToken tokens[n] =
		{
			"float Kd",
			"float Ks",
			"float Ka",
			"float roughness",
			"color specularcolor",
			"float occ_samples",
			"float maxdist",
			"float DoAO",
			"float materialID",
			"string diff_mapname",
			
		};
		
		RtPointer params[n] =
		{
			&Kd,
			&Ks,
			&Ka,
			&roughness,
			specularcolor.V,
			&occ_samples,
			&occ_dist,
			&Do_AO,
			&mtl_ID,
			(void *) &filename,
			
		};
		rm->RiDisplacement("",RI_NULL);
		rm->RiColor(Cs.V);
		rm->RiOpacity(Os.V);
		rm->RiSurfaceV("fu_3de_phong2", filename ? n : n - 1, tokens, params );
	}
	else if ((mtl->IsSupported)&&(mtl->shader_type==3))
	{
		Color4f Cs = mtl->diff_color;
		Color4f Os (mtl->opacity);
		Color4f transpa (1);
		transpa.R = 1-Os.R;
		transpa.G = 1-Os.G;
		transpa.B = 1-Os.B;
		transpa.A = 1-Os.A;
		float Tin = Os.A;

		Color4f diffColor = mtl->diff_color;
		const char *filename = "";
		if (mtl->DiffuseFile)
		{
		filename = mtl->DiffuseFile;
		//throw FuException3D("diff path map: %s", filename);
		}

		const char *spec_color_tex = "";
		if (mtl->SpecColorFile )
		{
		spec_color_tex = mtl->SpecColorFile;
		}

		const char *refl_rough_tex = "";
		if (mtl->SpecRoughFile)
		{
		refl_rough_tex = mtl->SpecRoughFile;
		}

		const char *aniso_tex = "";
		if (mtl->SpecExpoFile )
		{
			aniso_tex = mtl->SpecExpoFile;
		}

		const char *inca_tex = "";
		if (mtl->IncaFile )
		{
			inca_tex = mtl->IncaFile;
		}

		float diff_roughness = mtl->diff_rough;
		//throw FuException3D("diff rough: %d", &diff_roughness);

		Color4f specColor = mtl->spec_color;
		float spec_roughness = mtl->spec_rough;
		float fresnel_ior = mtl->spec_ior;

		Color4f reflColor = mtl->refl_color;
		float refl_rough = mtl->refl_rough;
		float raysamples = RaySamples;
		if (Proxy>1) raysamples /= Proxy;
		
		Color4f transm = mtl->tansmission;

		float refr_ior = mtl->refrior;

		float enableGI = mtl->DoGI;
		float GI_Val = mtl->GIStrength;
		if (Proxy>1) GI_Val /= Proxy;

		float Kr = mtl->refl_int;
		float Ks = mtl->spec_int;
		//throw FuException3D("ks: %s", &Ks);

		float occ_samples = AOSamples;
		if (Proxy>1) occ_samples /= Proxy;
		float occ_dist = AODepth;
		float Do_AO = DoAO;
		float DoPho = 0;
		//if ((DoPhotons)||(enableGI==1)) DoPho = 1.0;

		float ia_coating_on = mtl->CoatingOn;
		Color4f ia_coating_color = mtl->CoatingColor;
		float ia_coating_roughness = mtl->CoatingRough;
		float ia_coating_ior = mtl->CoatIOR;
		float ia_coating_samples = mtl->CoatSamples;
		if (Proxy>1) ia_coating_samples /= Proxy;
		Color4f ia_coating_transmittance = mtl->CoatTrans;
		float ia_coating_thickness = mtl->CoatingThick;

		float32 ia_SpecularEnv = mtl->SpecularEnv;
		float32 ia_SpecularRefl = mtl->SpecularRefl;
		float32 ia_SpecularSpec = mtl->SpecularSpec;
		float32 ia_SpecSamples = mtl->SpecSamples;
		if (Proxy>1) ia_SpecSamples /= Proxy;
		Color4f ia_ReflAniso = mtl->ReflAniso;
		float32 ia_SpecularAniso = mtl->SpecularAniso;

		float32 ia_refract_fog_strength = mtl->FogStrength;
		Color4f ia_refract_fog_color = mtl->ReflFog;
		float32 ia_RefraSamples = mtl->RefraSamples;
		if (Proxy>1) ia_RefraSamples /= Proxy;

		float32 ia_CoatingEnv = mtl->CoatingEnv;
		float32 ia_CoatingSpec = mtl->CoatingSpec;
		float32 ia_CoatingRefl = mtl->CoatingRefl;

		Color4f ia_SSS_Color = mtl->SSS_color;
		float32 ia_SSS_on = mtl->SSS_on;
		float32 ia_SSSIOR = mtl->SSSIOR;
		float32 ia_SSSStrength = mtl->SSSStrength;
		float32 ia_SSSScale = mtl->SSSScale;
		
		float mtl_ID = mtl->mtlID;


		Color4f ia_incandescence = mtl->Incan;
	
		const int n = 43;
		//throw FuException3D("env path map 3: %s", txPathFile);

		
		RtToken tokens[n] =
		{
			"color ia_diffuse_color",
			"float ia_diffuse_roughness",
			"float ia_reflect_ior",
			"color ia_reflect_color",
			"float ia_reflect_roughness",
			"color ia_transparency",
			"float ia_coating_on",
			"color ia_coating_color",
			"float ia_coating_roughness",
			"float ia_coating_ior",
			"float ia_coating_samples",
			"color ia_coating_transmittance",
			"float ia_coating_thickness",
			"color ia_refract_color",
			"float ia_refract_ior",
			"float ia_refract_roughness",
			"color ia_refract_fog_color",
			"float ia_refract_fog_strength",
			"float ia_reflect_anisotropy",
			"color ia_reflect_anisotropy_direction",
			"float ia_reflect_samples",
			"float ia_reflect_has_reflection",
			"float ia_reflect_has_specular",
			"float ia_reflect_has_environment",
			"float ia_coating_has_reflection",
			"float ia_coating_has_specular",
			"float ia_coating_has_environment",
			"color ia_incandescence",
			"float ia_refract_samples",
			"float ia_sss_on",
			"color ia_sss_scattering",
			"float ia_sss_scattering_scale",
			"float ia_sss_ior",
			"float ia_sss_scale",
			"float ia_do_gi",
			"float ia_gi_samples",
			"float materialID",
			"uniform string ia_envmap",
			"uniform string diffTex",
			"uniform string reflRoughTex",
			"uniform string i_aniso_tex",
			"uniform string i_inca_tex",
			"uniform string reflColorTex",
			

		};
		
		RtPointer params[n] =
		{
			diffColor.V,
			&diff_roughness,
			&fresnel_ior,
			specColor.V,
			&spec_roughness,
			&transpa,
			&ia_coating_on,
			ia_coating_color.V,
			&ia_coating_roughness,
			&ia_coating_ior,
			&ia_coating_samples,
			&ia_coating_transmittance,
			&ia_coating_thickness,
			reflColor.V,
			&refr_ior,
			&refl_rough,
			ia_refract_fog_color.V,
			&ia_refract_fog_strength,
			&ia_SpecularAniso,
			&ia_ReflAniso,
			&ia_SpecSamples,
			&ia_SpecularRefl,
			&ia_SpecularSpec,
			&ia_SpecularEnv,
			&ia_CoatingRefl,
			&ia_CoatingSpec,
			&ia_CoatingEnv,
			ia_incandescence.V,
			&ia_RefraSamples,
			&ia_SSS_on,
			ia_SSS_Color.V,
			&ia_SSSStrength,
			&ia_SSSIOR,
			&ia_SSSScale,
			&enableGI,
			&GI_Val,
			&mtl_ID,
			&txPathFile,
			&filename,
			&refl_rough_tex,
			&aniso_tex,
			&inca_tex,
			&spec_color_tex,
			

		};
		const char *bump_tex = "";
		float bump_int = mtl->BumpStrength;
		const char *bump_file = mtl->BumpFile;
		//throw FuException3D("bump: %s", bump_file);
		if (bump_file != NULL)
		{
			bump_tex = bump_file;
			
			const int bn = 2;
			RtToken btokens[bn] =
			{
				"string texturename",
				"float Km",
			};
			RtPointer bparams[bn] =
			{
				(void *) &bump_tex,
				&bump_int,
			};
			const int bnn = 1;
			float maxbound = mtl->BumpBound;
			
			RtToken bntokens[bnn] =
			{
				"sphere",
				//"space",
			};
			RtPointer bnparams[bnn] =
			{
				&maxbound,
				//"object",
			};
			//rm->RiAttributeBegin;
			rm->RiAttributeV("displacementbound", bnn, bntokens, bnparams);
			
			int trace_number = 1;
			RtToken disp_tokens[] = { "displacements" };
			RtPointer disp_params[] = { &trace_number };
			rm->RiAttributeV("trace", 1, disp_tokens , disp_params);
			//rm->RiAttributeEnd;
			if (bump_int>0) rm->RiDisplacementV("Roe_bumpy", bn , btokens, bparams);
			else rm->RiDisplacement("",RI_NULL);
		}
		else{
			rm->RiDisplacement("",RI_NULL);
		}

		rm->RiColor(diffColor.V);
		rm->RiOpacity(Os.V);
		rm->RiSurfaceV("RoEShader3", n , tokens, params );
	}
	else if ((mtl->IsSupported)&&(mtl->shader_type==4))
	{
		float Ka = 1.0f;
		float Kd = 1.0f;
		float Ks = mtl->spec_int;
		float roughness = 1/mtl->spec_expo;
		Color4f specularcolor = mtl->spec_color;
		const char *filename = mtl->DiffuseFile;
		
		float occ_samples = AOSamples;
		if (Proxy>1) occ_samples /= Proxy;
		float occ_dist = AODepth;
		float Do_AO = DoAO;

		Color4f Cs = mtl->diff_color;
		Color4f Os (mtl->opacity);
		float32 samples = RaySamples;
		if (Proxy>1) samples /= Proxy;
		
		const int n = 9;
		
		RtToken tokens[n] =
		{
			"float Kd",
			"float Ks",
			"float Ka",
			"float roughness",
			"color specularcolor",
			"float occ_samples",
			"float maxdist",
			//"samples",
			//"envname",
			"float DoAO",
			"string diff_mapname",	
		};
		
		RtPointer params[n] =
		{
			&Kd,
			&Ks,
			&Ka,
			&roughness,
			specularcolor.V,
			&occ_samples,
			&occ_dist,
			//&samples,
			//&txPathFile,
			&Do_AO,
			(void *) &filename,	
		};
		rm->RiDisplacement("",RI_NULL);
		rm->RiColor(Cs.V);
		rm->RiOpacity(Os.V);
		rm->RiSurfaceV("RoEBlinn", filename ? n : n - 1, tokens, params );
		
	}
	else if ((mtl->IsSupported)&&(mtl->shader_type==5))
	{
		float Ka = 1.0f;
		float Kd = 1.0f;
		float Ks = mtl->spec_int;
		float xroughness = mtl->xroughness;
		float yroughness = mtl->yroughness;
		//throw FuException3D("x: %s y: %s", xroughness, yroughness);
		Color4f specularcolor = mtl->spec_color;
		const char *filename = mtl->DiffuseFile;
		
		float occ_samples = AOSamples;
		if (Proxy>1) occ_samples /= Proxy;
		float occ_dist = AODepth;
		float Do_AO = DoAO;

		Color4f Cs = mtl->diff_color;
		Color4f Os (mtl->opacity);
		float32 samples = RaySamples;
		if (Proxy>1) samples /= Proxy;
		
		const int n = 10;
		
		RtToken tokens[n] =
		{
			"float Kd",
			"float Ks",
			"float Ka",
			"float uroughness",
			"float vroughness",
			"color specularcolor",
			"float occ_samples",
			"float maxdist",
			//"samples",
			//"envname",
			"float DoAO",
			"string diff_mapname",	
		};
		
		RtPointer params[n] =
		{
			&Kd,
			&Ks,
			&Ka,
			&xroughness,
			&yroughness,
			specularcolor.V,
			&occ_samples,
			&occ_dist,
			//&samples,
			//&txPathFile,
			&Do_AO,
			(void *) &filename,	
		};
		rm->RiDisplacement("",RI_NULL);
		rm->RiColor(Cs.V);
		rm->RiOpacity(Os.V);
		rm->RiSurfaceV("RoEWard", filename ? n : n - 1, tokens, params );
		
		}

	else
	{
		// assign it a default material
		Color4f Cs (0.0f);
		Color4f Os (1);
		rm->RiDisplacement("",RI_NULL);
		rm->RiSurface("matte", 0, RI_NULL, RI_NULL);
	}
}

void RendererRIB3D::RenderGeometry(Geometry3D *g)
{
   RtInt *vertsPerPoly = NULL;

	Vector2f *flippedTexCoord = NULL;
	   try
	   { 
		  RtInt nVertsPerPrimitive = g->NumVerticesPerPrimitive();
		  vertsPerPoly = new RtInt[g->NumPrimitives];		// throws CMemoryException
	      
		  for (int i = 0; i < g->NumPrimitives; i++)
			 vertsPerPoly[i] = nVertsPerPrimitive;

			std::vector<RtToken> tokens;
			std::vector<RtPointer> params;

			tokens.push_back("P");
			params.push_back(g->GetPosition()->Tuples);

			if (g->GetNormal())
			{
				tokens.push_back("N");
				params.push_back(g->GetNormal()->Tuples);
			}

			if (g->GetTexCoord())
			{
				// We need to be careful here since rendermans texture coordinate system is flipped compared to Fusions.  Fusion uses a coordinate system
				// where (0, 0) is the lower left corner and (1, 1) is the upper right corner.  In rendermanland these are the upperleft and bottomright
				// corners respectively.
				Stream3D *tcStream = g->GetTexCoord();
				flippedTexCoord = new Vector2f[tcStream->NumTuples];

				float32 *src = tcStream->Tuples;
				float32 *dst = (float *) flippedTexCoord;

				for (int i = 0; i < tcStream->NumTuples; i++)
				{
					dst[0] = src[0];
					dst[1] = 1.0f - src[1];

					src += tcStream->TupleSize;			// texcoords can have 3 or 4 components in Fusion sometimes
					dst += 2;
				}

				tokens.push_back("st");
				params.push_back(flippedTexCoord);
			}

			if (g->GetColor()) 
			{
				// TODO - particles use this   g->GetColor()->Tuples
				tokens.push_back("Cs");
				params.push_back(g->GetColor()->Tuples);
			}

			//rm->RiSphere (0.5, 0.0, 0.5, 360.0, RI_NULL);
			//rm->RiPointsPolygons(g->NumPrimitives, vertsPerPoly, (RtInt *) g->Indices->Data, "P", g->GetPosition()->Tuples, RI_NULL);

		  rm->RiPointsPolygonsV(g->NumPrimitives, vertsPerPoly, (RtInt *) g->Indices->Data, tokens.size(), (RtToken *) &tokens[0], (RtPointer *) &params[0]);

			delete [] flippedTexCoord;
		  delete [] vertsPerPoly;
	   }
	   catch (...)
	   {
			delete [] flippedTexCoord;
		  delete [] vertsPerPoly;
		  throw;
	   }
}
void RendererRIB3D::RenderSphere(float32 g_radius,float32 g_theta, float32 g_zmin, float32 g_zmax)
{	
	
	rm->RiSphereV (g_radius, g_zmin, g_zmax, g_theta, 0, NULL, NULL);

} 

void RendererRIB3D::RenderTorus(float32 g_radius,float32 g_radius2,float32 g_theta, float32 g_zmin, float32 g_zmax)
{
	rm->RiTorusV (g_radius, g_radius2, g_zmin, g_zmax, g_theta,0, NULL, NULL);
}

void RendererRIB3D::RenderCylinder(float32 g_radius,float32 g_theta, float32 g_zmin, float32 g_zmax)
{
	rm->RiCylinderV (g_radius, g_zmin, g_zmax, g_theta,0, NULL, NULL);
}
void RendererRIB3D::RenderCone(float32 g_radius,float32 g_theta, float32 g_height)
{
	rm->RiConeV (g_height, g_radius, g_theta,0, NULL, NULL);
}

void RendererRIB3D::RenderRib(const char* g_path)
{
	//rm->RiRotate(-90,1,0, 0);
	
	char result2[_MAX_PATH];   // array to hold the result.
		  //		sprintf(result2, "%s", RibFile2);
	
	strcpy(result2,g_path); // copy string one into the result.
	// change all the '\' to '/'
	int len = strlen(result2);
	for (int i = 0; i < len; i++)
		if (result2[i] == '\\')
			result2[i] = '/';
	//char start_term[_MAX_PATH+10] = "\"";
	//const char* end_term = "\"";
	//const char* new_term = result2;
	//strcat(start_term, result2);
	//strcat(start_term, end_term);
	//throw FuException3D("rib path : %s", result2);
	const char* archive_path = result2;
	RtToken rib_tokens = (RtToken) result2;
	//throw FuException3D("rib path : %s", rib_tokens);
	rm->RiReadArchive(rib_tokens,NULL,RI_NULL);
}

void RendererRIB3D::RenderPartio(const char* g_path,uint32 g_type, float32 g_radius, float32 g_radius_scale)
{
	char result2[_MAX_PATH+15];   // array to hold the result.
	const char fFile[10] = " --file ";	 	
	strcpy(result2,fFile); // copy string one into the result.
	
	strcat(result2,g_path); // copy string one into the result.
	
	 //change all the '\' to '/'
	int len = strlen(result2);
	for (int i = 0; i < len; i++)
		if (result2[i] == '\\')
			result2[i] = '/';

	const char* archive_path = result2;

	//throw FuException3D("partio type : %s", archive_path);
	
	char result3[_MAX_PATH+35];
	const char pptype[10] = " --type ";
	strcpy(result3,pptype);
	if (g_type == 1) p_type_string = "blobby";
	else if (g_type == 2) p_type_string = "patch";
	else if (g_type == 3) p_type_string = "sphere";
	else if (g_type == 4) p_type_string = "disk";
	else p_type_string = "particle";

	strcat(result3,p_type_string);
	strcat(result3,archive_path);
	const char* type_p = result3;

	char result4[_MAX_PATH+50];
	const char ppptype[13] = " --radius ";
	strcpy(result4,ppptype);
	char ppradius[10];
	sprintf(ppradius, "%f", g_radius);
	strcat(result4,ppradius);
	strcat(result4,type_p);

	const char* type_pp = result4;
		
	
	char result5[_MAX_PATH+50];
	const char pppptype[20] = " --radius_scale ";
	strcpy(result5,pppptype);
	char pppradius[10];
	sprintf(ppradius, "%f", g_radius_scale);
	
	strcat(result5,pppradius);
	strcat(result5,type_pp);

	const char* type_ppp = result5;
	//throw FuException3D("partio type : %s", type_p);

	RtPointer params[2];
	params[0] = "partio43delight_procedural";
	params[1] = (RtPointer) type_ppp;
	//params[1] = (RtPointer) archive_path;
	//RtToken rib_tokens = (RtToken) result2;
	rm->RiProcDynamicLoad( &params , NULL);
	//rm->RiReadArchive(rib_tokens,NULL,RI_NULL);
}

void RendererRIB3D::RenderParticles(Geometry3D *g)
{
	try
	{
		//std::vector<RtToken> tokens;
		std::vector<RtPointer> params;
		//tokens.push_back("P");
		
		params.push_back(g->GetPosition()->Tuples);
		float32 a_out = NULL;
		if (g->GetColor()) 
			{
				Stream3D *tcStream = g->GetColor();
				float32 *part_colors = tcStream->Tuples;	
				float const_width[1];
				const_width[0] = 0.01f;
				Color4f *out_rgba = new Color4f[tcStream->NumTuples];
				Vector3f *Part_color = new Vector3f[tcStream->NumTuples];
				Vector3f *alpha_out = new Vector3f[tcStream->NumTuples];
				for (int i=0; i<tcStream->NumTuples; i++)
				{
					Part_color[i].R=tcStream->Col4(i).R;
					Part_color[i].G=tcStream->Col4(i).G;
					Part_color[i].B=tcStream->Col4(i).B;
					alpha_out[i].X = tcStream->Col4(i).A;
					alpha_out[i].Y = tcStream->Col4(i).A;
					alpha_out[i].Z = tcStream->Col4(i).A;
				}
				//tokens.push_back("Cs"); 
				//params.push_back(g->GetColor()->Tuples);
				
				params.push_back(&Part_color[0]);
				params.push_back(&alpha_out[0]);
				params.push_back(&const_width[0]);
				delete [] out_rgba;
				delete [] Part_color;
				delete [] alpha_out;
			}
		
		
		
		//int part_size_idx = g->Streams.GetIndex(Stream_Scale);
		//Stream3D *pcStreams = g->Streams.GetStream(part_size_idx);
		//float32 *part_size = new float32[pcStreams->NumTuples];
		//Vector4f irgendwas(0,0,0,0);
		//for (int a= 0; a<pcStreams->NumTuples; a++){
		//	pcStreams->GetTuple(a,irgendwas) ;
		//}
		//params.push_back(&irgendwas);
		//delete [] part_size;
		////
		


		RtToken tokens[4] = {"P", "Cs","Os","constantwidth"};
		
		//rm->RiDisplacement("",RI_NULL);
		//RtColor White = {1,1,1};
		//rm->RiColor(White);
		//rm->RiSurface("particle", 0, RI_NULL, RI_NULL);
		rm->RiPointsV(g->NumPrimitives,4, tokens, (RtPointer *) &params[0]);
		
	}
	catch (...)
	{
		throw;
	}
}

void RendererRIB3D::ConcatTransform(const Matrix4f &m)
{
   // One troublesome thing to be aware of when writing exporters/importers is that the computer graphics world doesn't always
   // use the standardized math/physics matrix notation.  Sometimes matrices are stored transposed, or column vectors are treated as
   // row vectors or points to be transformed sit on the left of the matrix rather than the right.
   // In the Renderman universe, points are stored as row vectors and matrices are stored transposed from the standard mathematics
   // notation.  For example to translate the point (x, y, z) by (2, 3, 4):
   //   [ x y z 1] | 1 0 0 0 |  
   //              | 0 1 0 0 |
   //              | 0 0 1 0 |
   //              | 2 3 4 1 |
   // where the matrix would be stored in memory as a C array:  { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 2, 3, 4, 1 } }
   // In the Fusion universe, points are stored as column vectors (row vectors are the DualSpace(column vectors)) and the aforementioned
   // multiplication would look like:
   //   | 1 0 0 2 | | x | 
   //   | 0 1 0 3 | | y |
   //   | 0 0 1 4 | | z |
   //   | 0 0 0 1 | | 1 |

   Matrix4f n = m.Transpose();
   rm->RiConcatTransform(n.V);
}







// Exporter not working yet
#if 0

//
//
// RibExporter3D
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass ThreadedOperator
#define ThisClass ExporterRIB3D
FuRegisterClass(CLSID_Exporter_RIB, CT_FilterSink)
   REGS_Name,					"RIB Exporter",
   REGS_Category,				"3D",
   REG_TileID,					IDB_TILE_3D_FBXEXPORT,
   REGS_OpIconString,		"RIB",
   REGS_OpDescription,		"Export to .RIB file",
   REGS_HelpTopic,			"Tools/3D/RIB Exporter",
   REG_OpNoMask,				TRUE,
   REG_NoBlendCtrls,			TRUE,
   REG_NoObjMatCtrls,		TRUE,
   REG_NoMotionBlurCtrls,	TRUE,		// perhaps this can be used in some other way
   REG_Unpredictable,		TRUE,		// don't cache - if it gets cached the downstream tool (RenderThread) will always be satisfied by the cache and the RIBExporter will never Process()
#ifdef DFXPLUS
   REGP_RequiresFeature,	"DFXPlusMod4",
#endif
   TAG_DONE);


ExporterRIB3D::ExporterRIB3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
   InScene = AddInput("SceneInput", "SceneInput",
      LINKID_DataType,			CLSID_DataType_3D,
      LINK_Main,					1,
      TAG_DONE);

   OutScene = AddOutput("3D Data", "Output",
      LINKID_DataType,			CLSID_DataType_3D,
      LINK_Main,					1,
      TAG_DONE);

   AddControlPage("Controls");

   // RIB File
   InRIBFileNest = BeginControlNest("RIB File", "RIBFileNest", true);

      InRIBFile = AddInput("RIB Filename", "RIBFilename", 
         LINKID_DataType,		CLSID_DataType_Text,
         INPID_InputControl,	FILECONTROL_ID,
         INP_Required,			FALSE,
         INP_External,			FALSE,
         INP_DoNotifyChanged,	TRUE,
         IC_IsSaver,				TRUE,
         FCS_FilterString,		"RIB Files (*.rib)|*.rib",		
         TAG_DONE);

      InWriteHeader = AddInput("Write Header", "WriteHeader",
         LINKID_DataType,			CLSID_DataType_Number,
         INPID_InputControl,		CHECKBOXCONTROL_ID,
         INP_Default,				1.0,
         TAG_DONE);

   EndControlNest();

   // Frame Format
   InFrameFormat = BeginControlNest("Frame Format", "FrameFormatNest", false);

      InUseFrameFormatSettings = AddInput("Use Frame Format Settings", "UseFrameFormatSettings",
         LINKID_DataType,			CLSID_DataType_Number,
         INPID_InputControl,		CHECKBOXCONTROL_ID,
         INP_Default,				0.0,
         INP_External,				FALSE,
         INP_DoNotifyChanged,		TRUE,
         TAG_DONE);

      InWidth = AddInput("Width", "Width",
         LINKID_DataType,			CLSID_DataType_Number,
         INPID_InputControl,		SLIDERCONTROL_ID,
         INP_MinAllowed,			1.0,
         INP_MaxScale,				1024.0,
         INP_MaxAllowed,			32767.0,
         INP_Integer,				TRUE,
         IC_Steps,					1024,
         INP_Default,				320.0,
         INP_External,				FALSE,
         INP_DoNotifyChanged,		TRUE,
         INP_OpMenu,					TRUE,
         TAG_DONE);
      
      if (!IsBeingLoaded)
         InWidth->SetSource(AutoNumber(double(Document->GetPrefLong(PREF_FFWidth))), TIME_UNDEFINED);

      InHeight = AddInput("Height", "Height",
         LINKID_DataType,			CLSID_DataType_Number,
         INPID_InputControl,		SLIDERCONTROL_ID,
         INP_MinAllowed,			2.0,
         INP_MaxScale,				1024.0,
         INP_MaxAllowed,			32767.0,
         IC_Steps,					1023,
         INP_Integer,				TRUE,
         INP_Default,				240.0,
         INP_External,				FALSE,
         INP_DoNotifyChanged,		TRUE,
         INP_OpMenu,					TRUE,
         TAG_DONE);

      if (!IsBeingLoaded)
         InHeight->SetSource(AutoNumber(Document->GetPrefLong(PREF_FFHeight)), TIME_UNDEFINED);

      InPixelAspect = AddInput("Pixel Aspect", "PixelAspect",
         LINKID_DataType,			CLSID_DataType_Point,
         INPID_InputControl,		OFFSETCONTROL_ID,
         INP_DefaultX,				1.0,
         INP_DefaultY,				1.0,
         INP_External,				FALSE,
         INP_OpMenu,					TRUE,
         INP_DoNotifyChanged,		TRUE,
         IC_DisplayedPrecision,	6,
         TAG_DONE);

      if (!IsBeingLoaded)
         InPixelAspect->SetSource(AutoPoint(Document->GetPrefDouble(PREF_FFAspectX), Document->GetPrefDouble(PREF_FFAspectY)), TIME_UNDEFINED);

   EndControlNest();


   // for now we put the rib, the textures and shaders all into the same directory for all frames
   // but one could imagine that the user might want
      // * all the data associated with a particular time in its own folder
      // * all the shaders in their own folder, all the textures in their own folder, all the ribs in their own folder
      // * textures could be output as tiffs or compiled to a texture forma

   //InCamera = combobox



   
   DoHeader = true;
}

ExporterRIB3D::~ExporterRIB3D()
{
}

void ExporterRIB3D::NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags)
{	
   if (param)
   {
      if (in == InWidth || in == InHeight || in == InPixelAspect || in == InUseFrameFormatSettings)
      {
         AutoNumber use(InUseFrameFormatSettings, time);
         if (*(Number *)use)
         {
            InWidth ->SetSource(AutoNumber(double(Document->GetPrefLong(PREF_FFWidth))), TIME_UNDEFINED, FALSE);
            InHeight->SetSource(AutoNumber(double(Document->GetPrefLong(PREF_FFHeight))), TIME_UNDEFINED, FALSE);
            InPixelAspect->SetSource(AutoPoint(Document->GetPrefDouble(PREF_FFAspectX), Document->GetPrefDouble(PREF_FFAspectY)), TIME_UNDEFINED, FALSE);
         }
      }
   }

   BaseClass::NotifyChanged(in, param, time, tags);
}

void ExporterRIB3D::Process(Request *req)
{
   Req = req;

   Scene = (Scene3D *) InScene->GetValue(req);
   if (!Scene)
   {
      OutScene->Set(req, NULL);
      return;
   }

   Scene->Use();							// until we can figure out to make this behave like saver just do this (TODO)
   OutScene->Set(req, Scene);

   Text *filenameText = (Text *) InRIBFile->GetValue(req);
   if (!filenameText)
      return;

   RIBPathFile = (const char *)*(filenameText);
   if (*RIBPathFile == 0)
      return;

   RIBFile = GetFilename(RIBPathFile);
   if (RIBFile == 0)
      return;

   FrameNumber = int(req->Time + 0.5);		// TODO - 000n file ext

   fp = fopen(RIBPathFile, "wt");
   if (!fp)
      return;			// TODO - notify user

   if (Scene)
   {
      DoHeader = *InWriteHeader->GetValue(req) > 0.5;
      //DoZip = true;
      //DoComments = true;
      //DoIndent = true;

      if (DoHeader)
         OutputHeader();

      FrameBegin(FrameNumber);

      OutputFrameFormat();

      //Node3D *camNode = FindAnyCamera(Scene);		// TODO
      //if (camNode)
      //   OutputCamera(camNode);
      //else
      //   OutputDefaultCamera();
      
      OutputWorld();

      FrameEnd();
   }

   fclose(fp);

   Req = NULL;
   Scene = NULL;
   RIBPathFile = NULL;
   RIBFile = NULL;
}

void ExporterRIB3D::OutputHeader()
{
   Comment(RIBFile);

   Comment("Exported from " PRODUCT_BASE_NAME " " PRODUCT_VERSION_STRING " build " PRODUCT_BUILD_STRING);
   Comment(CString("Composition: ") + DocPtr->GetTitle() + ", Tool: " + GetName());

   time_t timeLong;
   time(&timeLong);

   tm timeStruct;
   gmtime_s(&timeStruct, &timeLong);

   char timeString[32];
   asctime_s(timeString, 32, &timeStruct);		// result is always 26 chars
   strcpy(timeString + 24, " GMT");

   Comment(timeString);

   Newline();
}

void ExporterRIB3D::OutputFrameFormat()
{
   Line("Display \"myrib.tiff\" \"file\" \"rgba\"");
   Newline();

   int scale = 1;

   if (Req->IsStampOnly())
   {
      Document->ProxyLock.ObtainReadLock();
      scale = Document->ProxyScale;
      Document->ProxyLock.ReleaseReadLock();
   }

   int width = *InWidth->GetValue(Req) / scale;
   int height = *InHeight->GetValue(Req) / scale;
   
   float32 pixelAspect = *InPixelAspect->GetValue(Req);

   fprintf(fp, "Format %i %i %f\n", width, height, pixelAspect);
}

void ExporterRIB3D::OutputCamera(const Node3D *node)
{
   FuASSERT(node && node->Data && node->NodeType == NDET_Camera, (""));

   const CameraData3D *data = (const CameraData3D *) (Object *) ((Node3D *) node)->Data;

   // once we call Projection all transforms apply to the camera, until WorldBegin is called
   fprintf(fp, "Projection \"perspective\" \"fov\" [%f]\n", data->GetFovX());
   fprintf(fp, "Clipping [%f] [%f]\n", data->Near, data->Far);
   Newline();

   Line("Scale 1 1 -1");
}

void ExporterRIB3D::OutputDefaultCamera()
{
   Line("Projection \"perspective\" \"fov\" 45");		// once we call Projection all transforms apply to the camera, until WorldBegin is called
   Newline();

   Line("Scale 1 1 -1");
}

void ExporterRIB3D::OutputWorld()
{
   Line("ShadingInterpolation \"smooth\"");
   Newline();

   // world
   WorldBegin();

   //Orientation();	// I don't get why the bloody FUCK this needs to be LH when all fusion polys are RH!!!! (according to the spec the Scale 1 1 -1 should flip the default orientation from LH to RH)
   //Line("Orientation \"lh\"\n");

   Color(1.0f, 1.0f, 1.0f);
   Newline();

   Line("LightSource \"distantlight\" 1 \"intensity\" [1.0] \"lightcolor\" [.5 .5 .9] \"from\" [0 0 0] \"to\" [0 0 -1]");
   Line("Surface \"matte\"");
   Newline();

   //RI->Translate(0.0f, 0.0f, 14.0f);
   //rib.Sphere(0.5f, -0.5f, 0.5f, 360.0f);
   Newline();

   Node3D *node = Scene->RootNode;

   if (node && node->Data && node->NodeType == NDET_Surface)
   {
      // if (node->ID)
      //   Operator *op = (Operator *) op;
      //   Comment("Node: %i, %s", node->NodeIndex, op->Name);
      // else
      //   Comment("Node: %i", node->NodeIndex);

      SurfaceData3D *data = (SurfaceData3D *) (Object *) node->Data;

      TransformBegin();

      OutputTransform(&node->M);

      std::vector<Geometry3D*> geometry;
      if (data->GetGeometry(geometry))
      {
         int nGeom = geometry.size();
         for (int i = 0; i < nGeom; i++)
         {
            Geometry3D *geom = geometry[i];
            if (geom)
            {
               OutputGeometry(geom);
               geom->Recycle();
            }
         }
      }

      TransformEnd();
   }

   Newline();

   WorldEnd();
}

void ExporterRIB3D::OutputTransform(const Matrix4f *m)
{
   FuASSERT(m, (""));	// caller is responsible for not passing NULL

   Matrix4f n = m->Transpose();
   ConcatTransform(n.V);
}

void ExporterRIB3D::OutputNode(const Node3D *n)
{
   FuASSERT(n, (""));	// caller is responsible for not passing NULL
}

void ExporterRIB3D::OutputGeometry(const Geometry3D *g)
{
   FuASSERT(g, (""));	// caller is responsible for not passing NULL

   RtInt nVertsPerPrimitive;
   switch (g->PrimitiveType)			// TODO - make this a func
   {
      case PT_Points:
         nVertsPerPrimitive = 1;		// TODO - renderman knows what points/lines are?
         break;
      case PT_LineSegments:
         nVertsPerPrimitive = 2;
         break;
      case PT_Triangles:
         nVertsPerPrimitive = 3;
         break;
      case PT_Quads:
         nVertsPerPrimitive = 4;
         break;
   }

   RtInt *vertsPerPoly = new RtInt[g->NumPrimitives];
   if (!vertsPerPoly)
   {
      //Error = true;						// TODO
      return;
   }

   for (int i = 0; i < g->NumPrimitives; i++)
      vertsPerPoly[i] = nVertsPerPrimitive;

   PointsPolygons(g->NumPrimitives, vertsPerPoly, (RtInt *) g->Indices->Data, ((Geometry3D *)g)->GetPosition()->Tuples, ((Geometry3D *)g)->GetNormal()->Tuples);

   delete [] vertsPerPoly;
}


void ExporterRIB3D::Newline() 
{ 
   fprintf(fp, "\n"); 
}

void ExporterRIB3D::Line(const char *str) 
{ 
   fprintf(fp, "%s\n", str); 
}

void ExporterRIB3D::Comment(const char *str)			
{ 
   fprintf(fp, "# %s\n", str); 
}

void ExporterRIB3D::Color(Color4f &v)												
{ 
   fprintf(fp, "Color %f %f %f %f\n", v.R, v.G, v.B, v.A); 
}

void ExporterRIB3D::Color(float32 r, float32 g, float32 b)					
{ 
   fprintf(fp, "Color %f %f %f\n", r, g, b); 
}

void ExporterRIB3D::Color(float32 r, float32 g, float32 b, float32 a)	
{ 
   fprintf(fp, "Color %f %f %f %f\n", r, g, b, a); 
}

void ExporterRIB3D::Translate(float32 x, float32 y, float32 z)									
{ 
   fprintf(fp, "Translate %f %f %f\n", x, y, z); 
}

void ExporterRIB3D::Rotate(float32 angle, float32 axisX, float32 axisY, float32 axisZ) 
{ 
   fprintf(fp, "Rotate %f %f %f %f\n", angle, axisX, axisY, axisZ); 
}

void ExporterRIB3D::Sphere(float32 cx, float32 cy, float32 cz, float32 angle)	
{ 
   fprintf(fp, "Sphere %f %f %f %f\n", cx, cy, cz, angle); 
}

void ExporterRIB3D::FrameBegin(int num) 
{ 
   fprintf(fp, "FrameBegin %i\n", num); 
}

void ExporterRIB3D::FrameEnd()			 
{ 
   fprintf(fp, "FrameEnd\n"); 
}

void ExporterRIB3D::WorldBegin()		
{ 
   fprintf(fp, "WorldBegin\n"); 
}

void ExporterRIB3D::WorldEnd()			
{ 
   fprintf(fp, "WorldEnd\n");   
}

void ExporterRIB3D::TransformBegin() 
{ 
   fprintf(fp, "TransformBegin\n"); 
}

void ExporterRIB3D::TransformEnd()   
{ 
   fprintf(fp, "TransformEnd\n"); 
}

void ExporterRIB3D::ReverseOrientation()
{
   fprintf(fp, "ReverseOrientation\n"); 
}

//void ExporterRIB3D::Orientation(FuEnum orientation)
//{
//	FuASSERT(orientation == FU_LH || orientation == FU_RH, (""));
//	fprintf(fp, "Orientation \"%s\"\n", RiStrings[orientation]); 
//}

void ExporterRIB3D::Transform(RtMatrix &m)
{
   fprintf(fp, "Transform\n[\n");
   for (int i = 0; i < 4; i++)
      fprintf(fp, "%f %f %f %f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
   fprintf(fp, "]\n"); 
}

void ExporterRIB3D::ConcatTransform(RtMatrix &m)
{
   fprintf(fp, "ConcatTransform\n[\n");
   for (int i = 0; i < 4; i++)
      fprintf(fp, "%f %f %f %f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
   fprintf(fp, "]\n"); 
}

void ExporterRIB3D::AttributeBegin() 
{ 
   fprintf(fp, "AttributeBegin\n"); 
}

void ExporterRIB3D::AttributeEnd()   
{ 
   fprintf(fp, "AttributeEnd\n"); 
}

void ExporterRIB3D::IntArray(RtInt len, RtInt *arr)
{
   fprintf(fp, "[\n"); 

   for (int i = 0; i < len; i++)
      fprintf(fp, "%i ", arr[i]);

   fprintf(fp, "\n]"); 
}

void ExporterRIB3D::Float3Array(RtInt len, RtFloat *arr)
{
   uassert(len % 3 == 0);

   fprintf(fp, "[\n"); 

   for (int i = 0; i < len; i += 3)
      fprintf(fp, "%f %f %f\n", arr[i + 0], arr[i + 1], arr[i + 2]);

   fprintf(fp, "\n]"); 
}

void ExporterRIB3D::PointsPolygons(RtInt nPolys, RtInt *vertsPerPoly, RtInt *indices, RtFloat *position, RtFloat *normal)
{
   fprintf(fp, "PointsPolygons\n");

   IntArray(nPolys, vertsPerPoly);

   int nIndices = 0, nAttr = 0;
   for (int i = 0; i < nPolys; i++)
   {
      for (int j = 0; j < vertsPerPoly[i]; j++)
         nAttr = max(nAttr, indices[nIndices + j]);
      nIndices += vertsPerPoly[i];
   }
   nAttr++;

   IntArray(nIndices, indices);

   fprintf(fp, "\"P\" ");
   Float3Array(3 * nAttr, position);

   fprintf(fp, "\"N\" ");
   Float3Array(3 * nAttr, normal);
}


#endif