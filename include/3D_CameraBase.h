#ifndef _3D_CAMERABASE_H_
#define _3D_CAMERABASE_H_


#include "Transform3DOperator.h"

#include "3D_BaseData.h"

class Node3D;
class Frustum3D;



// Camera plugins consist of two classes:
//  1) CameraOperator3D - this is the tool that appears on the flow.  In its Process() call it is responsible for building
//	  a scene containing a camera node containing a CameraData3D object
//  2) CameraData3D - an instance of this class gets attached to the scene graph and is responsible for generating
//     various viewing/transform matrices when the renderer so requires.






/**
 * When a camera is setup it is responsible for initializing one of these structures for the renderer.  All fields of this structure must
 * be initialized within CameraData3D::Setup().
 */
class CameraInfo3D
{
public:
	PRJ_ProjectionType Type;				/**< Must be orthographic or perspective */
	float32 Near;								/**< The distance to the near clip plane from the camera.  This will usually be a positive number. */
	float32 Far;								/**< The distance to the far clip plane from the camera.  Far > Near > 0.0  */
	float32 FrameAspect;						/**< viewportWidth / viewportHeight, doesn't take into account pixel aspect */
	float32 FovY;								/**< field of view in the Y direction in degrees (used only for perspective cameras) */
	float32 FLength;							/**< focal length (mm) (used only for perspective cameras) */
	float32 ApertureW;						/**< camera aperture width (inches) (used only for perspective cameras) */
	float32 ApertureH;						/**< camera aperture height (inches) (used only for perspective cameras) */
	float32 OrthoScale;						/**< A scale that gets applied between the WorldToEye and Projection transforms.  It is used currently to scale the orthographic viewing volume.  For perspective projections you probably want to set this to 1.0. */

	Matrix4f WorldToEye;						/**< Transform vertices from world space to the eye space */
	Matrix4f Projection;						/**< Transforms vertices from eye space to normalized device coords (ie. onto a cube [-1,+1]x[-1,+1]x[-1,+1] after the homogenous divide).  This matrix has the PostProjection/PreProjection matrices already multiplied in. */

	Matrix4f PreProjection;					/**< (Identity) Projection = PostProjection * (BaseProjection * PreProjection).  If you are not using "Projection" and rather are reconstructing the Projection matrix from (Near, Far, FovY, Aperture, ...) these matrices need to be multiplied in. */
	Matrix4f BaseProjection;				/**< (Identity) see PreProjection.  This matrix is the bare perspective projection matrix. */
	Matrix4f PostProjection;				/**< (Identity) see PreProjection.  For example, "fit to resolution gate" is implemented as a post projection scale. */
	float32 GlobalScale;

	bool IsSceneCamera;						/**< if true this is one of the "hidden" cameras generated by a view, if false it is an actual camera created on the flow (eg. via Camera3D tool).  For plugin cameras set this to false.  */
};






#define CAMERA_BASE	(DATA_BASE + TAGOFFSET)

/**
 * These tags are passed to CameraData3D::Setup() typically by the renderer or to CameraData3D::FitToView() by the view.
 * If a tag is not specified within the taglist supplied to Setup() the camera should assume it has its (default) value as indicated in
 * the comments below.
 */
enum CameraData3D_Tags ENUM_TAGS
{
	CAM_BaseIntTag = CAMERA_BASE + TAG_INT,
	CAM_ViewportWidth,				/**< (R) the viewport width in pixels */
	CAM_ViewportHeight,				/**< (R) the viewport height in pixels */
	CAM_DoStereoEye,					/**< (0) 0 = normal, 1 = left eye, 2 = right eye */
	CAM_AllowAdaptiveNearFar,		/**< (true) determines whether the camera is allowed to do adapative near/far computations */
	CAM_RescaleNearToUnity,			/**< (false) if true, the camera will alter its projection transform to remap near -> 1 by scaling up the scene and the camera's frustum by 1/zNear.  This is useful for GL renders (esp. shadowbuffers) to reduce z-fighting precision artifacts due to zNear being less than 1.0. */

	CAM_DblBaseTag = CAMERA_BASE + TAG_DOUBLE,
	CAM_PixelAspectX,					/**< (1.0) the x pixel aspect.  The totalAspect is FrameAspect * (pixelAspectX / pixelAspectY) where FrameAspect = viewportWidth / viewportHeight. */
	CAM_PixelAspectY,					/**< (1.0) the y pixel aspect */
	CAM_LensShiftX,					/**< (0.0) how much the lens should be shifted along the x eyespace axis. */
	CAM_LensShiftY,					/**< (0.0) how much the lens should be shifted along the y eyespace axis. */
	CAM_DoFShiftX,						/**< (0.0) lens shift for depth of field accumulation.  This is different than CAM_LensShiftX in that the shifting is applied to frustum to keep PlaneOfFocus in focus, where CAM_LensShiftX is applied at the near plane. */
	CAM_DoFShiftY,						/**< (0.0) */
	CAM_PlaneOfFocus,					/**< (1.0) the distance from the camera's location to the plane of focus (the plane upon which objects are in focus).  This is used for depth of field. */
	CAM_Overfit,						/**< (1.0) the amount of overfitting done when fitting to view (<1 means we zoom out extra, >1 means we zoom in extra), used with the FitToView() function */

	CAM_PtrBaseTag = CAMERA_BASE + TAG_PTR,
	CAM_EyeToWorldMatrix,			/**< (NULL) this matrix transforms objects in camera space into world space. Either a valid CAM_EyeToWorldMatrix or a valid CAM_CameraNode must be supplied, but not both. */
	CAM_CameraNode,					/**< (NULL) pointer to the Node3D in the Scene3D from whence this camera came.  Either a valid CAM_EyeToWorldMatrix or a valid CAM_CameraNode must be supplied, but not both. */
	CAM_WorldAABB,						/**< (NULL) pointer to the entire scene's AABB3D given in worldspace.  This should be passed if adaptive near/far planes are turned on.  If NULL is passed the default near/far planes will be used. */
	CAM_CenterViewPoint,				/**< (NULL) use this to pass a Vector3f* in world space to the CenterView() function */

	CAM_IDBaseTag = CAMERA_BASE + TAG_ID,
};





/**
 * Base class for camera data.  The Camera data object is stored in the Scene3D.  When the renderer needs to render with the camera it calls
 * CameraData3D::Setup() to fill out of a CameraInfo3D structure which it uses to setup its own internal camera. 
 */
class FuSYSTEM_API CameraData3D : public Data3D
{
	FuDeclareClass(CameraData3D, Data3D);

public:
	/**
	 * You would think FoV would be simple but its not.  The problem is that the field of view can specified wrt the aperture gate or wrt
	 * the resolution gate and the aperture/resolution gates may not be the same.  ResolutionGateFit determines how the aperture gate is fit
	 * within the resolution gate.  Whenever you write down a FoV you should be thinking whether its relative to the resolution or aperture gate.
	 */
	enum FoV_Type
	{
		FT_ApertureGate,
		FT_ResolutionGate,
	};

	PRJ_ProjectionType Type;					/**< must be orthographic or perspective */
	float32 Near;									/**< The distance to the near clip plane from the camera.  This will usually be a positive number. */
	float32 Far;									/**< The distance to the far clip plane from the camera.  Far > Near > 0.0  */
	float32 FovY;									/**< field of view in the Y direction in degrees (used only for perspective cameras) */
	float32 FLength;								/**< focal length (mm) (used only for perspective cameras) */
	float32 PlaneOfFocus;						/**< plane of focus measured in scene units - this is used for depth of field */
	float32 ApertureW;							/**< camera aperture width (inches) (used only for perspective cameras) */
	float32 ApertureH;							/**< camera aperture height (inches) (used only for perspective cameras) */
	float32 LensShift[2];						/**< (x, y) shifts at the near plane */

	SM_StereoMethod StereoMethod;				/**< (SM_Rotate) method to use for constructing stereo cameras */
	float32 EyeSeparation;
	float32 ConvergenceDistance;
	FuID RightEyeName;							/**< If not NOID, use this named camera for stereo right-eye projection */

	FITM_FitMethod ResolutionGateFit;		/**< (FitMethod_Height) how the aperture gate is fit within the resolution gate */

	float32 OrthoScale;							/**< A scale that gets applied between the WorldToEye and Projection transforms.  It is used currently to scale the orthographic viewing volume.  For perspective projections this is usually 1.0. */

	bool AdaptiveNear;							/**< (true) if true CameraData3D::Setup() will move the near clip plane to bound the scene as closely as possible.  The idea is to avoid the scene being cut off and to improve depth precision. Setup() must receive a CAM_WorldAABB tag for this to work. */
	bool AdaptiveFar;								/**< (true) if true CameraData3D::Setup() will move the far clip plane to bound the scene as closely as possible.  The idea is to make the whole scene is visible and to improve depth precision. Setup() must receive a CAM_WorldAABB tag for this to work. */

	bool IsSceneCamera;							/**< if false this is one of the "hidden" cameras generated by a view, if true it is an actual camera created on the flow (eg. via Camera3D tool) */

	uint8 m_CameraDataPad[16];

public:
	CameraData3D();
	CameraData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	CameraData3D(const CameraData3D &toCopy);		/**< subclasses are expected to override the copy ctor */
	virtual ~CameraData3D();

	void Init();

	/**
	 *	Subclasses must override this method.  Typical implementation:  return new MyCameraData(*this)
	 */ 
	virtual Data3D *CopyTagList(TagList &tags);

	/** 
	 * Uses information supplied in the taglist to fill out the CameraInfo3D struct.  Subclasses may override this method but it is not
	 * not required if the default implementation is sufficient.  When someone wants to use the camera (eg. the renderer or the view for 
	 * drawing controls) it will call Setup().  The taglist must include these CAM_ViewportWidth and CAM_ViewportHeight tags.  It should
	 * also include either CAM_CameraNode or CAM_EyeToWorldMatrix.  Also CAM_WorldAABB should be specified.  Setup() is declared 'const' 
	 * because the camera data object is shared (among multiple scenes/renderers/threads) and may not be modified.
	 */
	virtual bool SetupTagList(CameraInfo3D &ci, TagList &tags) const;
	bool Setup(CameraInfo3D &ci, Tag firstTag = _TAG_DONE, ...) const;

	/**
	 * Adjusts this cameras internal settings so that the supplied AABB fits in the viewport.  The taglist must minimally contain the
	 * world AABB of the scene and the cameras viewportWidth and viewportHeight.  If this camera does not support fit to view then
	 * false can be returned (we expect most cameras except for the built in CameraView3D will not support fit to view).
	 */
	virtual bool FitToViewTagList(TagList &tags);
	bool FitToView(Tag firstTag = _TAG_DONE, ...);

	/**
	 * Rotates this camera so it points at the provided CAM_CenterViewPoint.
	 */
	virtual bool CenterViewTagList(TagList &tags);
	bool CenterView(Tag firstTag = _TAG_DONE, ...);
	
	/**
	 * Gets the horizontal field of view.
	 */
	virtual float32 GetFovX(FoV_Type type = FT_ResolutionGate) const;
	virtual float32 GetFovY(FoV_Type type = FT_ResolutionGate) const;
};







class FuSYSTEM_API CameraOperator3D : public Transform3DOperator
{
	FuDeclareClass(CameraOperator3D, Transform3DOperator);

public:
	CameraOperator3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	CameraOperator3D(const CameraOperator3D &toCopy);		// ni
	virtual ~CameraOperator3D();

	static bool RegInit(Registry *regnode);
	static void RegCleanup(Registry *regnode);
	
	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);
	virtual Vector4 GetDirection();

	virtual void OnAddToFlow(TagList *tags = NULL);
	virtual void OnRemoveFromFlow(TagList *tags = NULL);
};



void FuSYSTEM_API SetToDefaultView(CameraData3D *camData, Node3D *camNode, int32 outWidth, int32 outHeight, PRJ_ProjectionType projType);



#endif
