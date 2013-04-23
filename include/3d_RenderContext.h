#ifndef _RENDER_CONTEXT_H_
#define _RENDER_CONTEXT_H_

#include "3D_Frustum.h"
#include "3D_Allocator.h"
#include "3D_Datatypes.h"
#include "3D_LightBase.h"
#include "3D_CameraBase.h"
#include "StandardEvents.h"


enum 
{
	PR_Undefined_Face = 0,
	PR_Front_Face,
	PR_Back_Face,
	PR_Front_And_Back_Faces
};






class Node3D;
class Scene3D;
class FusionApp;
class CameraData3D;



const int MAX_DEPTH_PEELS = 16;

const Color4f DEFAULT_GLOBAL_AMBIENT(0.0f, 0.0f, 0.0f, 1.0f);




/**
 * The render context is passed to render callbacks to allow materials/lights access to render state.  It is also used by 
 * the renderer to keep track of render state settings per thread.
 */
class FuSYSTEM_API RenderContext3D : public Object
{
	FuDeclareClass(RenderContext3D, Object);

public:
	FusionDoc *Document;					// document access
	EventConsoleSubID ConsoleID;		// consoleID to use in case the renderer or any 3D plugin wants to write something to the console
	Operator *Op;							// the tool that owns the renderer, useful for setting progress indicator (may be NULL if created by a view)
	FusionApp *App;						// the application - useful for querying things like number of procs

	Scene3D *Scene;						// pointer to the entire scene currently being rendered

	int OutWidth;							// width of the output image
	int OutHeight;							// height of the output image
	//FuRectInt DataWindow;				// DataWindow of output image

	float32 PixelAspectX;				// Pixel Aspect Ratio X
	float32 PixelAspectY;				// Pixel Aspect Ratio Y

	int Proxy;								// the current proxy setting
	bool HiQ;								// is this a quick or hiQ render?

	PFM_PolygonFillMode PolygonFillMode, GlobalPolygonFillMode;

	enum RendererType3D
	{
		RT_SoftwareRenderer,
		RT_OpenGLRenderer,
		//RT_ConsoleRenderer,
		//RT_MesaRenderer,
		RT_Unknown
	} RendererType;						// if the renderer is one of the 4 renderers that come with Fusion5, this will be set appropiately, and RT_Unknown otherwise

	// Camera information - we purposefully do not expose the CameraStdData3D object here as the renderer will typically modify the camera matrices for the
	//  purposes of supersampling, DoF, etc.
	Node3D *CameraNode;
	PRJ_ProjectionType CameraType;
	bool IsSceneCamera;					// is the current camera a scene camera?
	float32 zNear, zFar;

	FuPointer<LightData3D> DefaultLightData;				// Used as the default light if lighting is off

	bool IsInitialized;					// has this rendercontext been successfully setup yet?

	bool DoFrustumCulling;				// defaults to true (only GL Renderer obeys this atm, not SW)
	Frustum3D *Frustum;

	TagList Attrs;	// TODO - add a taglist here

	NodeInfo *SurfaceNode;
	NodeInfo *LightNode;

	bool DoLighting;						// mtls can test this to avoid doing lighting computations
	bool DoShadows;						// 

	int DoStereoEye;						// 0 = normal, 1 = left eye, 2 = right eye

	M4Pool *m_Matrix4fPool;				// TLS for Matrix4f

	TagList *m_ExtTags;					// instead of having extra extensibility arg "TagList *tag = NULL" on every single func we'll pass it inside of the rc

	uint8 m_PadRC[512];

protected:
	void CtorInit();

public:
	RenderContext3D();											// doesn't call Init() -- the renderer will do that
	RenderContext3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	RenderContext3D(const RenderContext3D &toCopy);
	virtual ~RenderContext3D();								// doesn't call Free() -- the renderer will do that

	virtual bool Init() { return true; }
	virtual void Free();

	virtual RenderContext3D *Copy() const/* = 0*/;

	virtual bool DebugIsValid();

	virtual const Matrix4f &GetModelToWorld()/* = 0*/;		/**< transforms points from model to world */
	virtual const Matrix4f &GetModelToWorldIT()/* = 0*/;	/**< transforms normals from model to world */

	virtual const Matrix4f &GetModelToEye()/* = 0*/;		/**< transforms points from model to eye */
	virtual const Matrix4f &GetModelToEyeIT()/* = 0*/;		/**< transforms normals from model to eye */

	virtual const Matrix4f &GetLightToWorld()/* = 0*/;		/**< transforms points from light to world */
	virtual const Matrix4f &GetLightToWorldIT()/* = 0*/;	/**< transforms normals from light to world */

	virtual const Matrix4f &GetLightToEye()/* = 0*/;		/**< transforms points from light to eye */
	virtual const Matrix4f &GetLightToEyeIT()/* = 0*/;		/**< transforms normals from light to eye */

	virtual const Matrix4f &GetWorldToEye()/* = 0*/;		/**< transforms points from world to eye */
	virtual const Matrix4f &GetWorldToEyeIT()/* = 0*/;		/**< transforms normals from world to eye */

	virtual const Matrix4f &GetEyeToWorld()/* = 0*/;		/**< transforms points from eye to world */
	virtual const Matrix4f &GetEyeToWorldIT()/* = 0*/;		/**< transforms normals from eye to world */

	virtual const Matrix4f &GetProjection()/* = 0*/;		/**< GetEyeToClip() - transforms points from eye to clip */
	virtual const Matrix4f &GetProjectionI()/* = 0*/;		/**< GetEyeToClipI() - transforms points from clip to eye */

	virtual const Matrix4f &GetClipToScreen()/* = 0*/;		/**< transforms points from clip space to screen space */

	// LogError returns false to not continue, true to try to continue (the renderer is also free to ignore this if it wishes).
	// LogError will internally call LogErrorMsg().
	bool LogError(int32 severity, const char *msg, ...);

	virtual bool LogErrorMsg(int32 severity, const char *message = NULL)/* = 0*/;
	virtual void OutOfMemoryError()/* = 0*/;
};





#endif