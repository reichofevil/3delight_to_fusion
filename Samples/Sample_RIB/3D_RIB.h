#ifndef _RIB_EXPORTER_3D_H_
#define _RIB_EXPORTER_3D_H_

#include <stdlib.h>

#include "3D_DataTypes.h"
//#include "Ri.h"
#include "3D_RiDLL.h"
#include "3D_RendererBase.h"
#include "SourceOperator.h"
#include "imageformat.h"

class RmDLL;
class Node3D;
class Scene3D;
class Matrix4f;
class Geometry3D;

const FuID CLSID_Renderer_RIB	= "RendererRIB";
const FuID CLSID_Exporter_RIB	= "ExporterRIB";

#define RENDERER_RM_BASE		(RENDERER_BASE + TAGOFFSET)

// Uncomment this to enable the multi output code - it is an example of how a Fusion tool can output more than one image.  In this case
// we're using it to output an "imaginary" caustics channel (which just contains a copy of the RGBA data).  There is nothing unique about
// the way RendererRIB3D does this... any other tool in Fusion can also have multiple outputs that appear on the flow -- they just happen to not.
//#define MULTI_OUT 1

/**
 * RIB renderer specific tags that can passed to Process() or Render().  Also see RendererBase_tags for more general tags.
 * Plugin renderers don't need to use taglists if they don't want to... its this way because Fusion uses taglists a lot internally 
 * for parameter passing.
 */
enum RendererRM_Tags ENUM_TAGS
{
   RenRM_BaseIntTag = RENDERER_RM_BASE + TAG_INT,
   RenRM_FrameNumber,				// (0) 
	RenRM_PixelFilterIndex,			// (PF_Gaussian)

   RenRM_DblBaseTag = RENDERER_RM_BASE + TAG_DOUBLE,
	RenRM_PixelSamplesX,				// (2.0)
	RenRM_PixelSamplesY,				// (2.0)
	RenRM_ShadingRate,
	RenRM_PixelFilterWidth,			// (PF_DefaultWidth[PixelFilter])
	RenRM_PixelFilterHeight,
	RenRM_FStop,	// (8.0) the f-stop of the camera - determines amount of depth of field blur
	RenRM_DofSides,
	RenRM_DofAngle,
	RenRM_DofRoundness,
	RenRM_DofDensity,
	RenRM_DoAO,
	RenRM_AOSamples,
	RenRM_AODepth,
	RenRM_DoMoBlur,
	RenRM_DoDefMoBlur,
	RenRM_shutterSamples,
	RenRM_shutterOpen,
	RenRM_shutterClose,
	RenRM_shutterStart,
	RenRM_shutterEnd,
	RenRM_RaySamples,
	RenRM_EnvMap,
	RenRM_DoPho,
	RenRM_PhoSamples,
	RenRM_PhoStrength,
	RenRM_PhoDist,
	RenRM_PhoBounce,
	RenRM_PhoRays,
	RenRM_PhoFile,
	RenRM_RibFile,
	RenRM_DoRIB,
	RenRM_RaytraceAll,
	RenRM_ProgCons,
	RenRM_ROI,
	RenRM_ValidWindowleft,
	RenRM_ValidWindowright,
	RenRM_ValidWindowbottom,
	RenRM_ValidWindowtop,


   RenRM_PtrBaseTag = RENDERER_RM_BASE + TAG_PTR,
   RenRM_RenderFileName,			// (NULL) filename for render.dll
   RenRM_ShaderPathName,
   RenRM_ProcPathName,

   RenRM_ObjBaseTag = RENDERER_RM_BASE + TAG_OBJECT,

   RenRM_IDBaseTag = RENDERER_RM_BASE + TAG_ID,
};






class RendererRIB3D: public RendererBase3D
{
   FuDeclareClass(RendererRIB3D, RendererBase3D);

protected:
   Input *InExecutablesNest;
   Input *InRender;
   Input *InTexMake;
   Input *InShader;
   Input *InShaderPath;
   Input *InProcPath;

   Input *InLightingNest;
   //Input *InLightingEnabled;			-- use RendererBase3D input for this
   //Input *InShadowsEnabled;				-- use RendererBase3D input for this

	Input *InSamplingNest;
	Input *InLockXYPixelSamples;
	Input *InPixelSamplesX;
	Input *InPixelSamplesY;
	Input *InPixelFilter;
	Input *InLockXYFilterSamples;
	Input *InFilterSamplesX;
	Input *InFilterSamplesY;
	Input *InRaySamples ;

	Input *InOptionsNest;
	Input *InDofNest;
	Input *InEnableDepthOfField;
	Input *InFStop;
	Input *InDofSides;
	Input *InDofRoundness;
	Input *InDofDensity;
	Input *InDofAngle;

	Input *InShadeRateNest;
	Input *InShadRate;

	Input *InMoBlurNest;
	Input *InEnableMoBlur;
	Input *InEnableDefMoBlur;
	Input *InMoBlurSteps;
	Input *InMoBlurOpen;
	Input *InMoBlurClose;
	Input *InMoBlurStart;
	Input *InMoBlurEnd;

	Input *InAONest;
	Input *InEnableAO;
	Input *InAOSamples;
	Input *InAODepth;

	Input *InEnvNest;
	Input *InEnvMap;

	Input *InPhotonNest;
	Input *InEnablePhotons;
	Input *InPhotonSamples;
	Input *InPhotonStrength;
	Input *InPhotonBounce;
	Input *InPhotonDist;
	Input *InPhotonRays;
	Input *InPhotonMap;

	Input *InExportNest;
	Input *InEnableExport;
	Input *InExportFile;

	Input *SceneInput;

	Input *InRaytraceAll;
	Input *InProgCons;

	#ifdef MULTI_OUT
		Output *OutCaustics;
		Request *Req;
	#endif

	// Lights, materials, surfaces stored in the Scene3D as LightData3D, MtlData3D, SurfaceData3D are shared reference counted objects and as 
	// such we may not modify them in any way.  But we need to store extra information associated with these objects.  For example, for lights
	// we'd like to store the Renderman light handle.  To get around this restriction, we'll create a wrapper "Light" class which can be modified
	// and in this we'll store our extra information.

	class Light
	{
	public:
		Node3D *Node;							// the node in the Fusion Scene3D containing this light
		RtLightHandle RmLight;				// the corresponding renderman light
	};

	class Material
	{
	public:
		enum TexType							// when you supply a texture to Renderman you can either supply it in its native format or convert it to the Render .tx format
		{
			Tiff,									// we chose tiff since its a common format and supports alpha and various bit depths
			Tx										// the renderman texture format which includes preprocessed mip levels
		};

		bool IsSupported;						// does our simple RIB renderer support this material type?
		MtlData3D *FuMaterial;				// the fusion material
		TexType DiffuseType;					// is the diffuse texture stored on disk as a Tiff or a .tx file?
		char *DiffuseFile;					// the filename of the texture file on disk
		char *DiffusePathFile;				// path & filename of the texture file
		char *SpecColorFile;					// the filename of the texture file on disk
		char *SpecColorPathFile;				// path & filename of the texture file
		char *SpecExpoFile;					// the filename of the texture file on disk
		char *SpecExpoPathFile;				// path & filename of the texture file
		char *SpecRoughFile;					// the filename of the texture file on disk
		char *SpecRoughPathFile;				// path & filename of the texture file
		char *IncaFile;					// the filename of the texture file on disk
		char *IncaPathFile;				// path & filename of the texture file
		Color4f diff_color;
		Color4f Incan;
		Color4f spec_color;
		float32 spec_int;
		float32 spec_expo;
		float32 opacity;
		float32 mtlID;
		int shader_type;
		float32 diff_rough;
		float32 spec_rough;
		Color4f refl_color;
		float32 refl_rough;
		float32 refl_int;
		float32 FogStrength;
		Color4f ReflFog;
		float32 RefraSamples;
		char *ReflRoughFile;					// the filename of the texture file on disk
		char *ReflRoughPathFile;				// path & filename of the texture file
		char *ReflColorFile;					// the filename of the texture file on disk
		char *ReflColorPathFile;
		char *ReflIntFile;					// the filename of the texture file on disk
		char *ReflIntPathFile;

		char *BumpFile;					// the filename of the texture file on disk
		char *BumpPathFile;
		float32 BumpBound;
		float32 BumpStrength;

	 	Color4f CoatingColor;
		float32 CoatingOn;
		float32 CoatIOR;
		float32 CoatSamples;
		Color4f CoatTrans;
		float32 CoatingThick;
		float32 CoatingRough;
		float32 CoatingEnv;
		float32 CoatingSpec;
		float32 CoatingRefl;

		float32 spec_intensity;
		float32 spec_ior;
		int colorspace;
		int is_metal;
		int nrg;
		float32 refl_val;
		float32 refrior;
		float32 refrint;
		float32 GIStrength;
		int DoGI;
		float32 maxdist;
		Color4f tansmission;
		float32 xroughness;
		float32 yroughness;
		float32 fogmindist;
		float32 fogmaxdist;
		Color4f fogcolor;
		float32 SpecularEnv;
		float32 SpecularRefl;
		float32 SpecularSpec;
		float32 SpecSamples;
		Color4f ReflAniso;
		float32 SpecularAniso;

		float32 SSS_on;
		Color4f SSS_color;
		float32 SSSIOR;
		float32 SSSStrength;
		float32 SSSScale;

	};

	class Surface
	{
	public:
		Node3D *Node;							// the node in the Fusion Scene3D containing this surface
		std::vector<Light*> LightList;	// the list of lights affecting this surface
		std::vector<Material*> MaterialList;	// list of materials used by this surface
	};

	int *NodeLUT;								// NodeLUT[Node3D::NodeIndex] gives index into LightList/SurfaceList

	int NumLights;								// size of LightList[] array
	Light *LightList;							// array of all lights in the scene
	Light FullAmbient;						// full intensity ambient light to be used when lighting is turned off
	Light PhtotonIndirect;					//inderectlight with photons

	int NumSurfaces;							// size of SurfaceList[] array
	Surface *SurfaceList;					// array of all surfaces in the scene

	int NumMaterials;							// size of MaterialList[] array
	Material *MaterialList;					// array of all materials in the scene

   RmDLL *rm;

   const char *RenderFilename;
   int FrameNumber;

   Image **OutSlot;

   Scene3D *Scene;							// TODO - Current or Primary?
   Scene3D *second_scene;
   CameraData3D *PrimaryCamera;			// TODO - Current or Primary?
   Node3D *PrimaryCameraNode;
   
   Node3D *new_camera;
   Node3D *new_geo;


	bool DoDepthOfField;
	float FocalDistance;
	float32 FStop;								// determines amount of DoF blur
	float DoFSides;
	float DoFAngle;
	float DoFRoundness;
	float DoFDensity;
	bool DoMoBlur;
	bool DoDefMoBlur;
	float shutterOpen;
	float shutterClose;
	float shutterSamples;
	float shutterstart, shutterend;

	float MBTimes[6];


	bool DoAO;
	float AOSamples;
	float AODepth;

	int DoPhotons;
	float32 PhotonCount;
	float32 PhotonStrength;
	float32 PhotonDist;
	float32 PhotonBounce;
	float32 PhotonRays;
	const char* PhotonFile;


	int DoRib;
	const char* RibFile;
	const char* RibFile2;
	char* RibFile_edit;

	bool DoRaytrace, DoProgCons;




   FusionDoc *Document;
   bool HiQ;									// is this a quick or hiQ render?
   int Proxy;									// the current proxy setting
   TimeStamp PrimaryTime;					// the current time
	bool PreCalc;								// is this a PreCalc render?

   int OutWidth, OutHeight;
   DepthTags OutDepth;						// the out image depth as one of the IMDP enums
   F3D_DataType OutDataType;				// the out image depth as one of the F3D enums
   IFLoadDepthEnums IFDepth;
   float32 PixelAspectX, PixelAspectY;

	float PixelSamplesX;						// hider samples
	float PixelSamplesY;
	int PixelFilterIndex;
	float FilterSamplesX;						// hider samples
	float FilterSamplesY;
	float PixelFilterWidth;
	float PixelFilterHeight;
	float shadingRate;
	float RaySamples;

	FuRectInt *DataWindow;
	FuRectInt *ValidWindow;
	FuRectInt *ROI;
	bool DoROI;
	float RM_ROI_Left;
	float RM_ROI_Right;
	float RM_ROI_Top;
	float RM_ROI_Bottom;


	//AutoImageRoI reqRoI;
	//AutoImageDoD inpDoD;
	FuRectInt dataWindow;

	float32 m_radius;
	float32 m_radius2;
	int m_type;
	uint32 p_type;
	float32 p_radius, p_radius_scale;
	float32 m_theta;
	float32 m_zmin;
	float32 m_zmax;
	float32 m_Height;
	bool m_BottomCap;
	bool m_TopCap;
	const char* m_path;
	const char* p_type_string;

	const char* fit_type;
	float32 fit_out;

	bool InDoRIB;

	const char* envPathMap;
	const char* EnvLPath;
	char *txPathFile;
	char *pPath;
	char *txPathFile_pro;
	char *txFile_pro;

	

   bool LightingEnabled;					// global lighting toggle
   bool ShadowsEnabled;						// global shadows toggle

   static RendererRIB3D *Ren;
   LockableObject ErrorLock;

   HMODULE RenderDLL;						// handle to render.dll - the main renderman DLL
	bool IsPRMan;								// true if the render.dll is Pixar's libprman.dll

	char *RenderTiff;							// where the renderman dll will write the render to disk
	char *RenderTiff2;							// where the renderman dll will write the render to disk
	std::vector<char*> TempFiles;			// list of temporary files used to pass Fusion textures to renderman
	std::vector<char*> TempPathFiles;	// list of temporary pathfiles
	char TempPath[_MAX_PATH];				// the directory where temporary (texture) files will be stored
	const char *ShaderPath;
	const char *ProcPath;

public:
   RendererRIB3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
   ~RendererRIB3D();

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);
   virtual bool ProcessTagList(Request *req, const TagList &tags);
   virtual bool RenderTagList(TagList &tags);

   static RtVoid ReportToConsole(RtInt code, RtInt severity, char *message);
  static void ProgressToConsole(RtFloat i_progress);
  static void ProgressToConsole2(RtFloat i_progress);

protected:
   void CheckAbort();

	Surface *GetSurfaceInfo(const Node3D *n);
	Light *GetLightInfo(const Node3D *n);

   virtual void ParseRenderAttrs(TagList &tags);

	RmDLL *LoadRenderman();
   void ReleaseRenderman(RmDLL *rm);

	Image *ReadTiffFromDisk(const char *filename);
	void WriteTiffToDisk(Image *img, const char *filename);

	Image *ReadEXRFromDisk(const char *filename);
	void WriteEXRToDisk(Image *img, const char *filename);

	void CreateSurfaceList();
	void FreeSurfaceList();

	void CreateMaterialList();
	void CreateRmTextures();
	void FreeMaterialList();

	void CreateLightLists();
	void FreeLightLists();

	void SetTextureSearchPath();
	void SetShaderSearchPath();
	void SetProcSearchPath();
	void CreateTempFilename(char *&pathfile, char *&file, const char *prefix, const char *suffix);
	void FreeTempFiles();

   void ConcatTransform(const Matrix4f &m);

	void RenderScene();
   void RenderNode(Node3D *n);
   void RenderSurface(Node3D *n);
	void RenderMaterial(Material *mtl);
   void RenderGeometry(Geometry3D *g);
	void RenderSphere(float32 g_radius,float32 g_theta, float32 g_zmin, float32 g_zmax);
	void RenderTorus(float32 g_radius,float32 g_radius2,float32 g_theta, float32 g_zmin, float32 g_zmax);
	void RenderCylinder(float32 g_radius,float32 g_theta, float32 g_zmin, float32 g_zmax);
	void RenderCone(float32 g_radius,float32 g_theta, float32 g_height);
	void RenderRib(const char *g_path);
	void RenderPartio(const char *g_path, uint32 g_type, float32 g_radius, float32 g_radius_scale);
	void RenderParticles(Geometry3D *g);

	RtLightHandle CreateAmbientLight(Node3D *n);
	RtLightHandle CreateDirectionalLight(Node3D *n);
	RtLightHandle CreateDirectionalLight2(Node3D *n);
	RtLightHandle CreatePointLight(Node3D *n);
	RtLightHandle CreatePointLight2(Node3D *n);
	RtLightHandle CreateSpotLight(Node3D *n);
	RtLightHandle CreateEnvLight(Node3D *n);
	RtLightHandle CreateAOLight(Node3D *n);
	RtLightHandle CreateProjectorLight(Node3D *n);
	RtLightHandle CreateIndirectLight();


	void ActivateLights(Node3D *n);
	void DeactivateLights(Node3D *n);

   virtual int NumSupportedRenderChannels();
   virtual const FuID *GetChannelName(int chan);

   virtual bool AddInputsTagList(TagList &tags);
   virtual void ShowInputs(bool visible);

   virtual CameraData3D *GetCamera() { return NULL; }
   //virtual RenderContext3D &GetRC() { return rc; }

	Image *CreateOutImage();
};





#if 0
class ExporterRIB3D: public ThreadedOperator
{
   FuDeclareClass(ExporterRIB3D, ThreadedOperator);

public:
   Input *InScene;
   Input *InRIBFileNest, *InRIBFile, *InWriteHeader;
   Input *InFrameFormat, *InUseFrameFormatSettings, *InWidth, *InHeight, *InPixelAspect;

   Output *OutScene;

   FILE *fp;

   int FrameNumber;

   Request *Req;
   Scene3D *Scene;

   const char *RIBPathFile;		// D:\rib\myrib.rib
   const char *RIBFile;				// myrib.rib

   //compression checkbox
   bool DoHeader;

public:
   ExporterRIB3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
   virtual ~ExporterRIB3D();

   void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags);
   void Process(Request *req);

   void OutputHeader();
   void OutputFrameFormat();
   void OutputCamera(const Node3D *node);
   void OutputDefaultCamera();
   void OutputWorld();
   void OutputNode(const Node3D *node);
   void OutputTransform(const Matrix4f *m);
   void OutputGeometry(const Geometry3D *g);


   void Newline();
   void Comment(const char *str);

   void IntArray(RtInt len, RtInt *arr);
   void Float3Array(RtInt len, RtFloat *arr);		// len is number of 3 tuples

   void Line(const char *str);

   void Color(Color4f &v);
   virtual void Color(float32 r, float32 g, float32 b);
   void Color(float32 r, float32 g, float32 b, float32 a);
   
   void Translate(float32 x, float32 y, float32 z);
   void Rotate(float32 angle, float32 axisX, float32 axisY, float32 axisZ);

   void Sphere(float32 cx, float32 cy, float32 cz, float32 angle);

   //
   //void Begin();
   //void End();

   void FrameBegin(RtInt num);
   void FrameEnd();

   void WorldBegin();
   void WorldEnd();

   void TransformBegin();
   void TransformEnd();

   void Transform(RtMatrix &m);
   void ConcatTransform(RtMatrix &m);

   void AttributeBegin();
   void AttributeEnd();

   void ReverseOrientation();
   //void Orientation(int orientation);

   void PointsPolygons(RtInt nPolys, RtInt *vertsPerPoly, RtInt *indices, RtFloat *position, RtFloat *normal);
   //
};
#endif



#endif