#include "stdafx.h"

#define BaseClass SurfaceInputs3D 
#define ThisClass SimpleCubeInputs

#ifdef FuPLUGIN
#include "FuPlugin.h"												// make sure you change the macro COMPANY_ID in FuPlugin.h which defaults to "samples"
#endif

#include "3D_Scene.h"
#include "Geometry3D.h"
#include "SimpleCube.h"
#include "RegistryLists.h"
#include "3D_MaterialConstant.h"

#include "Partio.h"


static const FuID CLSID_PartioViewerTool   = "PartioViewerTool";
static const FuID CLSID_PartioViewerInputs = "PartioViewerInputs";
static const FuID CLSID_PartioViewerData   = "PartioViewerData";

/*
 * This file consists of 3 classes.  I'm going to explain briefly here what they are and how they interact:
 *
 * SimpleCubeInputs:  These are the inputs for the cube.  They could be for things like size or subdivision level.
 *                    The SimpleCubeInputs object gets hosted by the SimpleCubeTool.  When SimpleCubeInputs::Process() is
 *                    called it produces and returns a SimpleCubeData object.
 *
 * SimpleCubeData:  This is a data object produced by the SimpleCubeInputs.  Its job is to produce Geometry3D for a cube
 *                  upon demand either when a Renderer3D requests it or when a downstream geometry modification tool like 
 *                  Displace3D/Bender3D requests it.  The SimpleCubeData is stored within a Node3D::Data where Node3D is a node 
 *                  within a Scene3D.  SimpleCubeData could contain the actual geometry for a cube or it could just contain an 
 *                  abstract description like { cubeWidth, cubeHeight, cubeDepth, subdivisions } ... it is up to the programmer 
 *                  to choose which but keep in mind there are memory usage advantages to delaying geometry creation until absolutely 
 *                  needed.
 *
 * SimpleCubeTool:  This class represents the actual tool on the flow.  When the SimpleCubeTool is asked to Process() it askes the 
 *                  SimpleCubeInputs to Process(), which in turn produce a SimpleCubeData object.  The SimpleCubeTool then takes
 *                  the data object and creates a new Scene3D containing the incoming scene and the data object.  The new Scene3D
 *                  is then output.
 *
 * It may seem a bit overly complicated to have 3 classes to generate a cube.  The SimpleCubeInputs and SimpleCubeTool classes could
 * be merged into one class if the plugin developer really wanted (but not recommeneded).  One of the advantages of inheriting the 
 * SimpleCubeInputs class is that SimpleCubeInputs contains a lot of base functionality (inputs for wireframe, visibility, lighting, 
 * matte options, and object ID).
 *
 * The tool/inputs/data paradigm is used all over the place in the 3D SDK.  It is used for lights, cameras, surfaces, and materials.
 * The reasons for its use actually goes much deeper.  This paradigm is used by Fusion to allow inputs to be injected from one plugin
 * into another plugin.  For example, the blend mode inputs inside of SimpleCubeTool are actually put there by the RendererGL3D and
 * RendererSW3D (eyeon internal) plugins.  It is also used to have one set of inputs/data hosted by several different tools: eg. 
 *  (1) the Cube3D and Shape3D tools share the SurfaceCubeInputs3D/SurfaceCubeData3D classes 
 *  (2) the ImagePlane3D, Shape3D and Camera3D share the SurfacePlaneInputs3D/SurfacePlaneData3D classes
 *  (3) all the geometry tools share the MtlStdInputs3D/MtlStdData3D classes which add the default material inputs
 */





//
//
// SimpleCubeInputs class 
//
//

// This declares the class registry node for our "SimpleCubeInputs" class.  The ClassRegistry is Fusions RTTI system and contains registry nodes
// for all the internal and plugin classes within Fusion.  The tool will be identified uniquely in the ClassRegistry by its class ID which
// in this case is "COMPANY_ID_DOT + CLSID_SimpleCubeInputs" = "samples.SimpleCubeInputs".
FuRegisterClass(COMPANY_ID_DOT + CLSID_PartioViewerInputs, CT_SurfaceInputs3D)
   TAG_DONE);

SimpleCubeInputs::SimpleCubeInputs(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{	
}

SimpleCubeInputs::~SimpleCubeInputs()
{
}

bool SimpleCubeInputs::AddInputsTagList(TagList &tags)
{
   // add our inputs
   InCubeWidth = AddInput("Width", "Width", 
      LINKID_DataType,		CLSID_DataType_Number,
      INPID_InputControl,	SLIDERCONTROL_ID,
      INP_MinScale,			0.0,
      INP_MaxScale,			10.0,
      INP_Default,			1.0,
      TAG_DONE);

   InCubeHeight = AddInput("Height", "Height", 
      LINKID_DataType,		CLSID_DataType_Number,
      INPID_InputControl,	SLIDERCONTROL_ID,
      INP_MinScale,			0.0,
      INP_MaxScale,			10.0,
      INP_Default,			1.0,
      TAG_DONE);

   InCubeDepth = AddInput("Depth", "Depth", 
      LINKID_DataType,		CLSID_DataType_Number,
      INPID_InputControl,	SLIDERCONTROL_ID,
      INP_MinScale,			0.0,
      INP_MaxScale,			10.0,
      INP_Default,			1.0,
      TAG_DONE);

   InFile = AddInput("partio File", "PFile", 
         LINKID_DataType,		CLSID_DataType_Text,
         INPID_InputControl,	FILECONTROL_ID,
         INP_Required,			FALSE,
         INP_External,			FALSE,
         INP_DoNotifyChanged,	TRUE,
         FCS_FilterString,		"partio Files (*.geo)|*.geo",		
         TAG_DONE);

   InPartDiv = AddInput("Read every Nth particle", "pProxy", 
      LINKID_DataType,		CLSID_DataType_Number,
      INPID_InputControl,	SLIDERCONTROL_ID,
	  INP_Integer,			TRUE,
      INP_MinScale,			1.0,
      INP_MaxScale,			100.0,
      INP_Default,			1.0,
      TAG_DONE);

   InPartColor = BeginGroup("Default Color", "Color", true, false);

			InPartR = AddInput("Red", "Red",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..PartRed"),
				I3D_AutoFlags,				PF_AutoProcess,	
				I3D_ParamName,				FuID("Part.R"),
				IC_ControlGroup,			1,
				IC_ControlID,				0,
				ICS_Name,					"Part Color",
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_Default,				1.0,
				INP_SubType,				IST_ColorR,
				TAG_DONE);

			InPartG = AddInput("Green", "Green",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..PartGreen"),
				I3D_AutoFlags,				PF_AutoProcess,
				I3D_ParamName,				FuID("Part.G"),
				IC_ControlGroup,			1,
				IC_ControlID,				1,
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_Default,				1.0,
				INP_SubType,				IST_ColorG,
				TAG_DONE);

			InPartB = AddInput("Blue", "Blue",
				LINKID_DataType,			CLSID_DataType_Number,
				INPID_InputControl,		COLORCONTROL_ID,
				LINKID_LegacyID,			FuID("..PartBlue"),
				I3D_AutoFlags,				PF_AutoProcess,
				I3D_ParamName,				FuID("Part.B"),
				IC_ControlGroup,			1,
				IC_ControlID,				2,
				INP_MinScale,				0.0,
				INP_MaxScale,				1.0,
				INP_Default,				1.0,
				INP_SubType,				IST_ColorB,
				TAG_DONE);

		EndGroup();


   // allow the BaseClass to add its wireframe/visibility/matte/objectID inputs and any other input injectors (eg. blendmodes)
   return BaseClass::AddInputsTagList(tags);
}

Data3D *SimpleCubeInputs::ProcessTagList(Request *req, Data3D *data, TagList &tags)
{
   // construct a cube data object using fusions RTTI class registry
   SimpleCubeData *cubeData = (SimpleCubeData *) ClassRegistry->New(COMPANY_ID_DOT + CLSID_PartioViewerData, CT_SurfaceData3D, DocPtr);
   
   // read the values from this tools inputs and store them into the data
   cubeData->Width  = *InCubeWidth->GetValue(req);
   cubeData->Height = *InCubeHeight->GetValue(req);
   cubeData->Depth  = *InCubeDepth->GetValue(req);
   Text *renderText = (Text *) InFile->GetValue(req);
   const char *renderPathFile = (const char *)*(renderText);
   cubeData->File = renderPathFile;
   cubeData->pDiv  = *InPartDiv->GetValue(req);
   cubeData->PartColor.R = *InPartR->GetValue(req);
   cubeData->PartColor.G = *InPartG->GetValue(req);
   cubeData->PartColor.B = *InPartB->GetValue(req);
   cubeData->PartColor.A = 1.0f;



   // allow the BaseClass and any injected inputs to read any inputs they need (eg. wireframe/visibility/matte/blend mode/object id)
   return BaseClass::ProcessTagList(req, cubeData, tags);
}

//
//
// SimpleCubeData class
//
//
#undef BaseClass
#undef ThisClass
#define BaseClass SurfaceData3D 
#define ThisClass SimpleCubeData
FuRegisterClass(COMPANY_ID_DOT + CLSID_PartioViewerData, CT_SurfaceData3D)
   TAG_DONE);

SimpleCubeData::SimpleCubeData(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
   // These are some hints provided for polygon sorting in the views.  Because the cube consists of 6 large quads rather
   // than a bunch of smaller polygons we can afford to give it the nicest sorting hint.  Typically the default settings
   // are just fine and you don't need to touch the ClipType/QuickClipType here.
   ClipType = PT_Point;
   QuickClipType = PT_Point;
}

Data3D *SimpleCubeData::CopyTagList(TagList &tags) // throws CMemoryException
{
   // just do this
   return new SimpleCubeData(*this);
}

SimpleCubeData::~SimpleCubeData()
{
}

SimpleCubeData::SimpleCubeData(const SimpleCubeData &toCopy) : BaseClass(toCopy)
{
   // It is important that you provide a copy constructor implementation for all Data3D subclasses as tools like Override3D rely
   // on the cctor to work.  The cctor need only do a shallow copy (it can ref count across any internal reference counted members).
   Width  = toCopy.Width;
   Height = toCopy.Height;
   Depth  = toCopy.Depth;
   File = toCopy.File;
   pDiv = toCopy.pDiv;
   PartColor = toCopy.PartColor;
}

bool SimpleCubeData::BuildBoundingBox()
{
   // Fusion uses Axis Aligned Bounding Boxes (AABBs) for its bounding volume heirarchy.  The bounding box should be provided
   // in the objects natural coordinate system.  The bounding box for the cube will be used for culling and other quick rejection 
   // tests.  It is recommeding you override this function always.  The default implementation will build the geometry for the cube 
   // and loop through all the vertices to brute force compute the bounding box.

   float32 hWidth  = 0.5f * Width;
   float32 hHeight = 0.5f * Height;
   float32 hDepth  = 0.5f * Depth;
	Vector3f bmin(-hWidth, -hHeight, -hDepth),bmax(hWidth, hHeight, hDepth);

   	Partio::ParticlesData* pdata;
	
	if (File)pdata=Partio::read(File);
	
	if (pdata){
		Partio::ParticleAttribute posAttr;
		uint32 pnum = pdata->numParticles();
		for (int i=0;i<pnum;i++)
                    {
                        const float* aapos=pdata->data<float>(posAttr,i);
                        bmin[0]=min(bmin[0],aapos[0]);bmin[1]=min(bmin[1],aapos[1]);bmin[2]=min(bmin[2],aapos[2]);
                        bmax[0]=max(bmax[0],aapos[0]);bmax[1]=max(bmax[1],aapos[1]);bmax[2]=max(bmax[2],aapos[2]);
                    }
		pdata->release();	
	}
	

   AABBSet = true;

   AABB.SetValue(bmin, bmax);

   return true;
}

bool SimpleCubeData::CreateGeometryTagList(std::vector<Geometry3D*> &geometry, TagList &tags) // throws CMemoryException
{
   // CreateGeometryTagList() must be overriden by all subclasses of SurfaceData3D.  You should generate all your Geometry3D
   // and push them onto the geometry vector.  In this SimpleCube example we'll only be generating one piece of geometry.
   //
   // Surfaces know nothing about materials.  Any material can be applied to any surface although it may not always make sense.
   // The SimpleCubeTool itself has a material input and it later attaches the material to the Node3D where the SimpleCubeData
   // we are generating here is stored.
   // 
   // Geometry3D consists of an array of Stream3Ds indexed by a single IndexArray3D.  Each stream is an array of per vertex
   // attributes like normals, texture coordinates, position, vertex color, or some arbitrary user defined quantity like
   // density or pressure.  These quantities are interpolated across polygons by the renderer and made available per-pixel
   // to materials.  Keep in mind that the IndexArray3D is shared by all the Stream3Ds - so even though a cube can be described
   // by 8 spatial vertices each of those 8 vertices belongs to 3 separate faces on which it will have different normals and
   // texture coordinates.  So there are really 24 vertices.
    int pnum = 1;
	Partio::ParticlesData* pdata;
	
	if (File)pdata=Partio::read(File);
	
	if (pdata){
		pnum = pdata->numParticles();
		
	};


   
   
   const int indicesPerFace = 4;			// we're drawing quads
   const int numFaces = 6;					// 6 faces on a cube

   const int numIndices = pnum;			// numVertices = numIndices in this SimpleCube example so its
   const int numVertices = pnum;			// not the greatest example in the world

   Geometry3D *g = NULL;
   IndexArray3D *ia = NULL;
   Stream3D *posStream = NULL;			// per vertex model space position
   Stream3D *normStream = NULL;			// per vertex normals
   Stream3D *tcStream = NULL;				// per vertex texture coordinates
   Stream3D *colStream = NULL;

   // Its good to be a little careful about memory allocation here typically because surfaces typically have user
   // defined subdivision levels (although our simple cube doesn't) which can lead to some large allocations.
   try
   {
      g = NewGeometry();					// a helper function to generate a new properly initialized Geometry3D

      g->PrimitiveType = PT_Points;		// will be generating quads
      g->NumPrimitives = pnum;		// ... and there will be one quad for each of the 6 faces

      ia = IndexArray3D::Alloc(numIndices);

      // When creating streams, we provide (1) a string name which it can be later found by in a material, (2) the coordinate space
      // we are providing the vertex data in, and (3) the number of components per tuple.
      posStream  = Stream3D::Alloc(Stream_Position,  CS_Model, numVertices, 3);		// (x, y, z) position
      normStream = Stream3D::Alloc(Stream_Normal,    CS_Model, numVertices, 3);		// (nx, ny, nz) normal
      tcStream   = Stream3D::Alloc(Stream_TexCoord0, CS_Unset, numVertices, 2);		// (u, v) texcoord
	  colStream = Stream3D::Alloc(Stream_Color, CS_Unset, numVertices, 4); //RGB color

      // Each primitive (quad) can belong to a smoothgroup.  A smoothgroup is represented by a number 0 <= uint32_max.  Specifying
      // a smoothgroup is optional here.  If you do not supply one, all the primitives in the geometry are assumed to belong to the
      // same smoothgroup.
      g->SmoothGroup = new uint32[numFaces];
   }
   catch (CMemoryException *e)
   {
      SAFE_RECYCLE(g);
      SAFE_RECYCLE(ia);
      SAFE_RECYCLE(posStream);
      SAFE_RECYCLE(normStream);
      SAFE_RECYCLE(tcStream);
	 SAFE_RECYCLE(colStream);
      throw e;
   }

   g->Indices = ia;

   g->Streams.Add(posStream);
   g->Streams.Add(normStream);
   g->Streams.Add(tcStream);
   //g->Streams.Add(colStream);


   


   geometry.push_back(g);

   //normStream->Fill(Vector3f(0.0f, 0.0f, 1.0f));


   // Now we're going to fill in the streams.  Remember that Fusion uses +y as the up axis and a right handed coordinate system.
   // Indices should be provided so that when facing the front of a primitive they are give in CCW (counter-clockwise) order.
   // The CCW order is used when culling primitives.  (Hint:  If you turn on back face culling and it hides a front face you 
   // probably got it wrong)
   Vector3f *pos  = (Vector3f *) posStream->Tuples;
   Vector3f *norm = (Vector3f *) normStream->Tuples;
   Vector2f *tc   = (Vector2f *) tcStream->Tuples;
   //Color4f *pcol = (Color4f *) colStream->Tuples;

   

   //const float32 x = 0.5f * Width;
   //const float32 y = 0.5f * Height;
   //const float32 z = 0.5f * Depth;
   pos[0] = Vector3f(0,0,0);
   



//for (int i=0;i<pnum;i++)
//	{
////		pcol[i] = PartColor;
//		norm[i] = Vector3f(1,0,0);
//	}
	
	if (pdata){
		Partio::ParticleAttribute posAttr;
		for(int i=0;i<pdata->numParticles();i++)
		{
			if(i%pDiv==0)
			{
				const float* posa=pdata->data<float>(posAttr,i);
				pos[i]=Vector3f(posa[0],posa[1],posa[2]);
			}
		/*if (pdata->attributeInfo("rgbPP", colorAttr) ||
                        pdata->attributeInfo("rgb", colorAttr) ||
                        pdata->attributeInfo("color", colorAttr) ||
                        pdata->attributeInfo("pointColor", colorAttr))
                {
                   for(int i=0;i<pdata->numParticles();i++)
					{
						const float* cola=pdata->data<float>(colorAttr,i);
						pcol[i]=Vector3f(cola[0],cola[1],cola[2]);
					};
                }*/	
	}
	 pdata->release();

	/*for (int i = 0; i < pnum; i++)
		pcol[i] = PartColor;*/
	 //pcol[i] = PartColor;
for (int i=0;i<pnum;i++)
	{
		//pcol[i] = PartColor;
		norm[i] = Vector3f(0.0f,0.0f,1.0f);
	}
	

   //const Vector3f p[8] =
   //{
   //   Vector3f(-x, -y, +z),		// the 8 vertices
   //   Vector3f(+x, -y, +z),
   //   Vector3f(+x, +y, +z),
   //   Vector3f(-x, +y, +z),
   //   Vector3f(-x, -y, -z),
   //   Vector3f(+x, -y, -z),
   //   Vector3f(+x, +y, -z),
   //   Vector3f(-x, +y, -z)
   //};
   
   //const Vector3f n[6] =
   //{
   //   Vector3f(+1.0f,  0.0f,  0.0f),		// +x face
   //   Vector3f(-1.0f,  0.0f,  0.0f),		// -x face
   //   Vector3f( 0.0f, +1.0f,  0.0f),		// +y face
   //   Vector3f( 0.0f, -1.0f,  0.0f),		// -y face
   //   Vector3f( 0.0f,  0.0f, +1.0f),		// +z face
   //   Vector3f( 0.0f,  0.0f, -1.0f),		// -z face
   //};

   const Vector2f t[4] =
   {
      Vector2f(0.0f, 0.0f),
      Vector2f(1.0f, 0.0f),
      Vector2f(1.0f, 1.0f),
      Vector2f(0.0f, 1.0f)
   };

   // +z face
   //pos[0]  = p[0];   
   //norm[0]  = n[4];
   tc[0]  = t[0];
   //pos[1]  = p[1];   
   //norm[1]  = n[4];
   tc[1]  = t[1];
   //pos[2]  = p[2];   
   //norm[2]  = n[4];
   tc[2]  = t[2];
   //pos[3]  = p[3];   
   //norm[3]  = n[4];
   tc[3]  = t[3];

   // -z face
   //pos[4]  = p[5];
   //norm[4]  = n[5];
   tc[4]  = t[0];
   //pos[5]  = p[4];
   //norm[5]  = n[5];
   tc[5]  = t[1];
   //pos[6]  = p[7];
   //norm[6]  = n[5];
   tc[6]  = t[2];
   //pos[7]  = p[6];
   //norm[7]  = n[5];
   tc[7]  = t[3];

   // +y face
   //pos[8]  = p[3];
   //norm[8]  = n[2];
   tc[8]  = t[0];
   //pos[9]  = p[2];
   //norm[9]  = n[2];
   tc[9]  = t[1];
   //pos[10] = p[6];
   //norm[10] = n[2];
   tc[10] = t[2];
   //pos[11] = p[7];
   //norm[11] = n[2];
   tc[11] = t[3];

   // -y face
   //pos[12] = p[4];
   //norm[12] = n[3];
   tc[12] = t[0];
   //pos[13] = p[5];
   //norm[13] = n[3];
   tc[13] = t[1];
   //pos[14] = p[1];
   //norm[14] = n[3];
   tc[14] = t[2];
   //pos[15] = p[0];
   //norm[15] = n[3];
   tc[15] = t[3];

   // +x face
   //pos[16] = p[1];
   //norm[16] = n[0];
   tc[16] = t[0];
   //pos[17] = p[5];
   //norm[17] = n[0];
   tc[17] = t[1];
   //pos[18] = p[6];
   //norm[18] = n[0];
   tc[18] = t[2];
   //pos[19] = p[2];
   //norm[19] = n[0];
   tc[19] = t[3];
                                        
   // -x face										 
   //pos[20] = p[4];
   //norm[20] = n[1]; 
   tc[20] = t[0];
   //pos[21] = p[0];
   //norm[21] = n[1];
   tc[21] = t[1];
   //pos[22] = p[3];
   //norm[22] = n[1];
   tc[22] = t[2];
   //pos[23] = p[7];
   //norm[23] = n[1];
   tc[23] = t[3];



   uint32 *ind = ia->Data;
   for (int i = 0; i < numIndices; i++)
      ind[i] = i;


   // each face will be in its own smoothgroup
   for (int i = 0; i < 6; i++)
      g->SmoothGroup[i] = i;

	}
   return true;
	
}

int SimpleCubeData::GetNumMtlSlots()
{
	// By default the base class version of this function returns 1.  If you had a cube that had a material per face then you would
	// want to return 6 here.
	return 1;
}

const FuID SimpleCubeData::GetMtlSlotName(int i)
{
	// By default the base class version of this function returns "Material1".  If you had a cube that had a material per face
	// you might want to make this return "Front", "Back", "Top", ... etc.
	return "Material1";
}







//
//
// SimpleCubeTool class
//
//

// By inheriting from Transform3DOperator we will automagically have a Transform tab added to our tool which contains 
// controls for translation, rotation, pivot, scale and targeting.  These controls are optional.  For example, if you 
// didn't want target controls you could use the T3DREG_TargetControls tag.
#undef BaseClass
#undef ThisClass
#define BaseClass Transform3DOperator
#define ThisClass SimpleCubeTool
FuRegisterClass(COMPANY_ID_DOT + CLSID_PartioViewerTool, CT_3DFilterSource)
   REGS_Name,					COMPANY_ID_SPACE "PartioViewer 3D",	// "samples Simple Cube 3D" - this name minus the spaces appears on the tools tile in the flow view
   REGS_Category,				COMPANY_ID_DBS "3D",						// "samples\\3D" - this determines where the tool appears within the menus
   REG_TileID,					IDB_TILE_3D_CUBE,							// an internal resource ID we're appropiating for our samples tile picture
   REGS_OpIconString,		"RoEPV",										// this text appears on the toolbar (Fusions Cube3D tool also uses this abbreviation which will cause some confusion so this really isn't a good choice)
   REGS_OpDescription,		"Loads Partio files and displays them as points",				// text that appears in the statusbar when hovering the cursor over the "3Cb" toolbar entry
   REG_OpNoMask,				TRUE,											// don't add the default mask input that most 2D filter tools have
   REG_NoBlendCtrls,			TRUE,											// don't add the blend controls that appear on the radioactive tab
   REG_NoObjMatCtrls,		TRUE,											// don't add the object/material ID controls that appear on the radioactive tab
   REG_NoMotionBlurCtrls,	TRUE,											// don't add the motionblur controls that appear on the radioactive tab
   T3DREG_TargetControls,	FALSE,										// we don't want the default targetting controls on the transform tab
   TAG_DONE);

SimpleCubeTool::SimpleCubeTool(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
   // add the cube's geometry inputs to the controls page
   AddControlPage("Controls");

   InCubeInputs = (SimpleCubeInputs *) ClassRegistry->New(COMPANY_ID_DOT + CLSID_PartioViewerInputs, CT_SurfaceInputs3D, table, tags);

   if (InCubeInputs)
   {
      // Register the inputs with this SimpleCubeTool.  This will cause the tool to forward NotifyChanged()/OnAddToFlow()/OnConnect() and
      // other GUI notifications onto the inputs.  It will not actually add the inputs to this tool though.
      RegisterInputs(InCubeInputs);

      // This is how you currently add object ID inputs but it needs to be changed...
      InCubeInputs->SetAttrs(
         SI_ObjectIDInputName,		"ObjectID",
         SI_ObjectIDInputID,			"ObjectID",
         TAG_DONE);

      // Now add the cube inputs to this tool.  Just to be clean, we're going to put the inputs in a group so that to scripting their IDs
      // will be things like SimpleCubeTool.SimpleCubeInputs.Width.  If the group wasn't here, the inputs would be like SimpleCubeTool.Width
      // and this could conflict with other injected inputs.
      BeginGroup(CLSID_PartioViewerInputs, CLSID_PartioViewerInputs, true, false);
      InCubeInputs->AddInputs();
      EndGroup();
   }

   // Add the material input
   InMaterial = AddInput("Material Input", "MaterialInput",
      LINKID_DataType,			CLSID_DataType_MtlGraph3D,
      LINKID_AllowedDataType,	CLSID_DataType_Image,		// LINKID_AllowedDataType is optional, but putting it here allows the material input to receive image connections also
      LINK_Main,					GetNextMainValue(),			// make sure our LINK_Main value doesn't conflict with Transform3DOperator::In3Ds LINK_Main 
      INP_Required,				FALSE,
      INP_ConnectRequired,		TRUE,
      TAG_DONE);
}

SimpleCubeTool::~SimpleCubeTool()
{
   SAFE_RECYCLE(InCubeInputs);									// what we create we should free
}


void SimpleCubeTool::CheckRequest(Request *req)
{
   // Tools in Fusion process in response to Requests issued by downstream tools/views.  Before a tool can Process() it needs to get the 
   // values (Parameters) for all its Inputs from upstream tools.  This in turn is done by issuing Requests to upstream tools.  Before 
   // the upstream Request gets issued, the tool gets a chance to modify it in CheckRequest().  In this particular case, SimpleCubeTool is 
   // modifying the time that the InMaterial input is requesting to deal correctly with motion blur.  When motion blur is enabled, 
   // Requests are issued for each of the motionblur subtimes in a spread about the motionblur "base time".  Basically what it is doing 
   // can be summarized thusly: "if there is motionblur then use the material evaluated at the motionblur base time rather than at the 
   // the motionblur subtimes".  

   BaseClass::CheckRequest(req);

   // for a typical request the base time is set to TIME_UNDEFINED unless motion blur is enabled
   if (req->GetPri() == 0 && req->GetBaseTime() != TIME_UNDEFINED)
      req->SetInputTime(InMaterial, req->GetBaseTime());
}

void SimpleCubeTool::Process(Request *req)
{
   // We really don't need to override Process() here as the default implementation which calls CreateScene() is ok.   We
   // are overriding it so you can get an idea of what the base implementation does if at some point the default implementation
   // is insufficient.  For this SimpleCube example you could replace all the below code with "BaseClass::Process(req);".

   // Fusion tools pass Parameters between their inputs and outputs.  Image, Number, Scene3D, and MtlGraph3D are examples
   // of subclasses of Parameter that plugin developers will typically see getting passed between inputs and outputs.  
   // Transform3DOperator automatically adds an input(In3D) and output(Out3D) declared to take a Scene3D parameters.  It is 
   // the job of the SimpleCubeTool to create a new Scene3D containing the incoming Scene3D and the cube geometry and then 
   // outputing the resulting scene.  If we didn't inherit from Transform3DOperator we would have to manually add the main 
   // input/output ourselves.
   
   // We're going to start off by setting the output Scene3D to be NULL.  This will cause Fusion to fail the render of this
   // tool.  Later on we'll set the output to something valid if we succeed.  This is just to make error handling easier.
   Out3D->Set(req, NULL);

   Scene3D *inScene = (Scene3D *) In3D->GetValue(req);	// get the incoming scene
   Node3D *root = CreateScene(req);								// get the surface node we'll be merging into the incoming scene

   if (root)
   {
      Scene3D *outScene = new Scene3D(Document, req);		// create the outgoing scene

      if (inScene && inScene->RootNode)
      {
         // Note that we have to take a copy of the incoming scene rather attaching it to root directly.  Any Parameter you get
         // from an input (ie. via GetValue(req) in Process()) must be treated as read only.  The reason is that Parameters are
         // reference counted shared objects.  This is kind of unfortunate, but its not so bad because we're doing a shallow copy
         // of the node structure and all the heavy data is refcounted across.
         Node3D *inRoot = inScene->RootNode->Copy();
         root->AddChild(inRoot);
      }

      ProcessTransform(req, root);								// copy the transform from the inputs in the transform tab to the root node

      outScene->SetRootNode(root);

      // output the new scene
      Out3D->Set(req, outScene);
   }
}

Node3D *SimpleCubeTool::CreateScene(Request *req, TagList *tags)
{
   // The job of CreateScene() is to output a new Node3D containing the geometry we wish merged into the scene

   Node3D *cubeNode = new Node3D(NDET_Surface);
   cubeNode->SetName(GetName());					// set the Nodes name to be this tools name
   cubeNode->SetID((uint_ptr) this);			// use the operator pointer as the ID - this must always be done - it used for things like picking
   cubeNode->SetSubID(0);							// if there's more than one Node3D generated per tool, each should have its own unique SubID - this is used (for example) in picking characters produced by a Text3D tool
   cubeNode->SetTransformMatrix(Matrix4f());	// Set the transform matrix to identity.

   // Call process on the inputs
   SimpleCubeData *cubeData = (SimpleCubeData *) InCubeInputs->Process(req, NULL);
   cubeNode->Data = cubeData;						// cubeNode->Data is a smart pointer
   cubeData->Recycle();								// ... so we need an extra recycle here

   // Get the material and attach it to the cubeNode.  Note that because because InMaterial had the INP_Required set to false we have
   // to handle the case where there is no material attached.
   MtlData3D *mtlData = ConvertToMtlData(InMaterial->GetValue(req));

   if (!mtlData)
   {
      // grab the default material that fusion uses.
      mtlData = (MtlStdData3D *) ClassRegistry->New(CLSID_MtlStdData, CT_MtlData3D, DocPtr);
      ((MtlStdData3D *) mtlData)->Diffuse = Vector4f(0.37f, 0.13f, 1.0f, 1.0f);		// and make it an interesting purplely color
   }

   // Surfaces advertise the amount of and names of material slots they have via GetNumMtlSlots() and GetMtlSlotName(i).  The materials 
	// themselves are stored within the nodes of the scene.  This keeps materials separated from surfaces in Fusion.
	int nMtls = cubeData->GetNumMtlSlots();
	for (int i = 0; i < nMtls; i++)
	{
		if (i > 0)
			mtlData->Use();
		cubeNode->AddMtl(mtlData);
	}

   return cubeNode;
}

