#ifndef _3D_RENDERERBASE_H_
#define _3D_RENDERERBASE_H_

#include "Operator.h"

#include "3D_Resource.h"				// for F3D_DataType
#include "3D_DataTypes.h"
#include "3D_BaseInputs.h"
#include "3D_RenderContext.h"

#define PICK_LIGHT					(1l << 0)
#define PICK_CAMERA					(1l << 1)
#define PICK_CURVE					(1l << 2)
#define PICK_SURFACE					(1l << 3)
#define PICK_ALL						0xffffffff
#define PICK_NONE						0x0

#define IMAGE_ARRAY_CHUNK 128

class RendererBase3D;
class CameraData3D;

enum RS_SEVERITY
{
	RS_INFO,
	RS_WARNING,
	RS_RECOVERABLE_ERROR,
	RS_FATAL_ERROR
};

// severity:
//  0 - information
//  1 - warning
//  2 - recoverable error
//  3 - fatal/unrecoverable error
// subcode:  this is completely renderer dependent and a renderer can assign any error values it wishes (subcodes 0 to 999 are reserved)
// message:  this string is only valid during the duration of the callback
// return: false to not continue, true to try to continue
typedef bool (*RendererErrorCallback)(int32 severity, const char *message, void *userData);

#define RENDERER_BASE			(INPUTS_BASE   + TAGOFFSET)
#define RENDERER_GL_BASE		(RENDERER_BASE + TAGOFFSET)
#define RENDERER_SW_BASE		(RENDERER_BASE + TAGOFFSET)
#define RENDERER_MESA_BASE		(RENDERER_BASE + TAGOFFSET)
#define RENDERER_CONSOLE_BASE	(RENDERER_BASE + TAGOFFSET)
#define RENDERER_SM_BASE		(RENDERER_BASE + TAGOFFSET)
#define RENDERER_SL_BASE		(RENDERER_BASE + TAGOFFSET)
#define RENDERER_RT_BASE		(RENDERER_BASE + TAGOFFSET)
#define RENDERER_EXP_BASE		(RENDERER_BASE + TAGOFFSET)


enum RendererBase_Tags ENUM_TAGS
{
	Ren_IntBaseTag = RENDERER_BASE + TAG_INT,
	Ren_OutWidth,						// (R) the width of the output image
	Ren_OutHeight,						// (R) the height of the output image
	Ren_OutDepth,						// (IMDP_8bitInt) the depth of the output image
	Ren_HiQ,								// (false) is this a quick or HiQ render?
	Ren_ConsoleID,						// (ECONID_Error) in case the renderer or any 3D plugin wants to write something to the console
	Ren_Proxy,							// (Document ? Document->ProxyScale : 1) an integer indicating the current proxy setting
	Ren_ForcePointAA,					// (true) global point antialiasing override
	Ren_ForceWireframeAA,			// (true) global wireframe antialiasing override
	Ren_LightingEnabled,				// (false) global lighting enable (if lighting is not enabled, everything should be renderer in full (1, 1, 1) ambient)
	Ren_ShadowsEnabled,				//	(false) global shadow enable
	Ren_DoPicking,						// (false) set to true if in picking mode
	Ren_GlobalPolygonFillMode,		// (PFM_Filled) global polygon fill mode override
	Ren_UseDefaultLight,				// (false) set to enable default light (a light that gets used if there are no other lights in the scene and global lighting is enabled)
	Ren_DoProgress,					// (false) set to true to enable progress reporting (you must supply a valid operator ptr if this option is on)
	Ren_DoMotionBlur,					// (false) a renderer must override DoOwnMotionBlur() to return true if wants to receive this tag (otherwise Fusion will render multiple passes and accumulate them)
	Ren_MBQuality,						// (2) a number indicating the quality of motion blur (Fusion will do 2*q+1 accumulation passes)
	Ren_DoStereoEye,					// (0) for stereo renders, use this to indicate which eye to render (0=mono, 1=left, 2=right)
	Ren_DoMaterialRender,			// (false) if a renderer specifies the REG_SupportsMaterialRenders in its registry, Fusion's material viewer will issue renders to it with that tag set to true
	Ren_MaterialRenderHint,			// (false) if true the object to be rendered is convex (eg. sphere, cube)
	Ren_IsViewRender,					// (false) set to true if we are rendering a fusion view (most software renderers will be too slow to render to fusions views in a sufficiently interactive manner)
	Ren_AllowAdaptiveNearFar,		// (true) determines whether the camera is allowed to do adapative near/far computations.  You only need to worry about this tag if your renderer is going to be used to render Fusion views.  It is used by views to prevent the grid/controls from being clipped.  This tags value should be passed to CameraData3D::Setup() call using the CAM_AllowAdaptiveNearFar tag 
	Ren_DoDepthOfField,				// (false) set to true if depth of field should be enabled
	Ren_PreCalc,						// (false) set to true if a IMG_NoData image should be produced, but with the correct depth and all relevant channels. No rendering need take place, since the Image has no data.

	Ren_DblBaseTag = RENDERER_BASE + TAG_DOUBLE,
	Ren_PixelAspectX,					// (1.0) the pixel aspect of the output image
	Ren_PixelAspectY,					// (1.0)
	Ren_Time,							// (scene->Time) the time we're rendering at (the default is okay)
	Ren_LineThickness,				// (1.0) the default global line thickness used when drawing lines
	Ren_PointDiameter,				// (1.0) the default diameter of points
	Ren_MBShutterAngle,				// (180.0) the range of angles in degrees (0 - angle) that the camera shutter is open for motion blur.  (Note: there are 360 degrees between one frame and another, this is the same as motionblur shutter angle on tools)
	Ren_MBCenterBias,					// (0.0) biases the motion blur window off from the center.  (Note: CenterBias / 2 = shift of motion blur window, this is the same as motionblur center bias on tools)
	Ren_PlaneOfFocus,					// (?) distance to the focal plane (for depth of field)

	Ren_PtrBaseTag = RENDERER_BASE + TAG_PTR,
	Ren_OutImageSlot,					// (NULL) an Image** pointer into which the output image will be placed (usually required)
	Ren_SceneInput,					// (NULL) an Input* from which the renderer may request more scenes (eg. at different times for motion blur)
	Ren_Document,						// (R) a Document* indication which document the renderer tool/preview belongs to
	Ren_Scene,							// (R) a Scene* to be rendered
	Ren_Camera,							// (R) a CameraData3D* through which the scene is to be renderer
	Ren_CameraNode,					// (NULL) the node the camera belongs to (set to NULL if the camera is external the scene and sits in world space)
	Ren_GlobalAmbient,				// (0, 0, 0) a Color4f* indicating a global ambient value (alpha is ignored)
	Ren_PickMatrix,					// (NULL) a Matrix4f* supplying a pick matrix (proj -> pick * proj)
	Ren_Operator,						// the tool that initiated this render (may be NULL, eg. if rendered by a view, but must be non-NULL if you set Ren_DoProgress)
	Ren_DefaultLightDiffuseColor,	// (1, 1, 1) a Color4f* 
	Ren_DefaultLightSpecularColor,// (1, 1, 1) a Color4f* 
	Ren_DefaultLightDirection,		// (?)
	Ren_DefaultLightData,			// (?) LightData3D *
	Ren_SceneTimeExtent,				// ([Ren_Time, Ren_Time]) a TimeExtent* for which the scene supplied via Ren_Scene is valid (used for reducing work when doing motion blur)
	Ren_Request,						// (NULL) a pointer to a Request object (required if you are doing renderering motionblur so the scene can be obtained at a secondary time)
	Ren_RegionOfInterest,			// (NULL) a FuRectInt* determining which region is to be rendered in image coordinates (this region will be contained within the ImageWindow all the time (unless you overrided PreCalc to create such "out of bounds" regions))

	Ren_MultiIDBaseTag = RENDERER_BASE + (TAG_ID | TAG_MULTI),
	Ren_OutputChannelID,				// a FuID representing an output channel (one of RendererChannelFuID), this tag can appear multiple times and usually F3D_Chan_RGBA appears
};





/**
 * If you wish to write a plugin renderer that will embed itself into the Renderer3D tool you must inherit RendererBase3D and declare your 
 * renderer as class type CT_Renderer3D.  RendererBase3D inherits Inputs3D so that renderers can be treated as a set of inputs that gets 
 * embedded into the Renderer3D tool.  Inheriting RendererBase3D gets your renderer into the Renderer3D.Renderers combobox and adds certain 
 * useful UI functionality.
 */
class FuSYSTEM_API RendererBase3D : public Inputs3D
{
	FuDeclareClass(RendererBase3D, Inputs3D);

public:
	class FuSYSTEM_API ChannelRec
	{
	public:
		Input *In;
		RendererChannel_t Enum;		// set to F3D_Chan_User for custom channels
		FuID ID;							// from RendererChannelFuID[] or set to a custom channel name
		const char *Name;
		bool IsLocked;
		uint32 Pad;

	public:
		ChannelRec();
		ChannelRec(RendererChannel_t chanEnum, FuID &id, bool isLocked = false);
	};

	int _PickMode;				// bitwise or'ing for different object picking -- TODO - goes into base class, use DoPicking

	RendererErrorCallback ErrorCallback;
	void *ErrorCallbackUserData;

	// These inputs default to NULL and subclasses do not have to use them.  However, if subclasses do use them,
	// then certain functionality in RenderBase3D will work:
	//   1) the channels nest/inputs will be created and managed for you
	//   2) the lighting/shadows/channels/wireframe input settings will be preserved when switching between renderers
	Input *InLightingEnabled;			
	Input *InShadowsEnabled;			
	Input *InWireframe;			
	Input *InWireframeAA;

	Input *InChannelsNest;
	std::vector<ChannelRec> Channels;

public:
	// The ctor taglist must always contain a (OBJP_Owner, Operator*) pair so that we can add our inputs to it
	RendererBase3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~RendererBase3D();

	static bool RegInit(Registry *regnode);
	static void RegCleanup(Registry *regnode);

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);

	virtual void ShowInputs(bool visible);
	virtual bool AddInputsTagList(TagList &tags);

	// This function is static - it doesn't access any class members
	Image *CreateOutImage(std::vector<FuID> &outChannels, Tag firstTag = _TAG_DONE, ...);
	virtual Image *CreateOutImageTagList(std::vector<FuID> &outChannels, const TagList &tags);

	virtual void SetPickMode(int pickMode);

	virtual void InstallErrorCallback(RendererErrorCallback callback, void *userData);

	virtual bool DoOwnMotionBlur();												// (false) return true if the renderer is going to do its own motion blur
	virtual bool SupportsRegionOfInterest();									// (false) return true if the renderer can handle the Ren_RegionOfInterest tag appropiately

	/**
	 * Typically one overrides Process() so that it reads out values from your inputs and stores them in a tagList and call 
	 * RendererBase3D::Process(tagList) which in turn adds more tags and calls Render(tagList).  One may call Render() directly 
	 * but you have to setup the tagList correctly.  The purpose of having Process() and Render() functions is to neatly separate 
	 * Fusion stuff like reading inputs from requests from the actual 3D rendering code.
	 */
	bool Process(Request *req, Tag firstTag = _TAG_DONE, ...);			
	virtual bool ProcessTagList(Request *req, const TagList &tags);

	bool Render(Tag firstTag = _TAG_DONE, ...);
	virtual bool RenderTagList(TagList &tags) = 0;

	// LogError returns false to not continue, true to try to continue (the renderer is also free to ignore this if it wishes)
	virtual bool LogError(int32 severity, const char *message = NULL);
	virtual void OutOfMemoryError();

	// SetupCamera() is called by the viewer so that it can get a camera with the tweaks the GL renderer does.  In particular, this
	// makes sure controls sit at the same depth as objects in the scene.
	virtual bool SetupCameraTagList(CameraInfo3D &ci, CameraData3D *camera, TagList &tags) const;
	bool SetupCamera(CameraInfo3D &ci, CameraData3D *camera, Tag firstTag = _TAG_DONE, ...) const;

	/**
	 * OnRendererChanged() is called by the Renderer3D tool when it switches renderers.  This allows renderers to preserve certain UI 
	 * settings from the previous renderer such as lighting/shadows/channels/wireframe enables.  The convention is that the new renderer
	 * is responsible for updating its inputs by examining the old renderer.
	 */
	virtual void OnRendererChanged(RendererBase3D *newRenderer, RendererBase3D *oldRenderer, TagList *tags = NULL);

	// Methods to query which channels are supported by the renderer.  Renderers may override these to get certain useful base UI functionality.
	virtual int GetNumChannels();									// returns the number of channels that this renderer can output
	virtual int FindChannel(const FuID &chan);				// returns -1 if not found
	virtual int FindChannel(RendererChannel_t chan);		// returns -1 if not found

protected:
	virtual void CopyInputState(Input *newInput, Input *oldInput);
	virtual void ShowChannelsNest(bool visible);

	virtual void BeginChannelsNest();
	virtual void AddChannelInput(int chan);
	virtual void AddChannelsInputs(); 
	virtual void EndChannelsNest();
};

#endif
