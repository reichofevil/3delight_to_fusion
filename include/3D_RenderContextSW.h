#ifndef _RENDER_CONTEXT_SW_H_
#define _RENDER_CONTEXT_SW_H_

#include "Types.h"
#include "3D_RenderContext.h"

class RendererSW3D;
class RendererExp3D;
class LightImplSW3D;





/**
 * The software render context is passed to render callbacks to allow materials/lights access to render state.  It is also used by 
 * the renderer to keep track of render state settings per thread.
 */
class /*FuSYSTEM_API*/ RenderContextSW3D : public RenderContext3D
{
	FuDeclareClass(RenderContextSW3D, RenderContext3D);

public:
	LightImplSW3D **LightList;
	int NumLights;

	LightImplSW3D **ProjectorList;
	int NumProjectors;

	Image *OutImage;

	int ThreadNumber;			/**< The thread number that is currently executing.  This is usually accessed in MtlImplSW3D::Shade(). */
	int MaxThreads;			/**< The total number of threads executing. */

	bool MemoryLimited;		/**< true if the amount of mesh/texture memory needed for the render is too large (eg. exceeds available memory) */

	uint8 m_PadSWRC[512];

public:
	RenderContextSW3D();																									// doesn't call Init()
	RenderContextSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	RenderContextSW3D(const RenderContextSW3D &toCopy);
	virtual ~RenderContextSW3D();																						// doesn't call Free()

	virtual RenderContext3D *Copy() const;

	virtual const Matrix4f &GetModelToWorld();		/**< transforms points from model to world */
	virtual const Matrix4f &GetModelToWorldIT();		/**< transforms normals from model to world */

	virtual const Matrix4f &GetModelToEye();			/**< transforms points from model to eye */
	virtual const Matrix4f &GetModelToEyeIT();		/**< transforms normals from model to eye */

	virtual const Matrix4f &GetLightToWorld();		/**< transforms points from light to world */
	virtual const Matrix4f &GetLightToWorldIT();		/**< transforms normals from light to world */

	virtual const Matrix4f &GetLightToEye();			/**< transforms points from light to eye */
	virtual const Matrix4f &GetLightToEyeIT();		/**< transforms normals from light to eye */

	virtual const Matrix4f &GetWorldToEye();			/**< transforms points from world to eye */
	virtual const Matrix4f &GetWorldToEyeIT();		/**< transforms normals from world to eye */

	virtual const Matrix4f &GetEyeToWorld();			/**< transforms points from eye to world */
	virtual const Matrix4f &GetEyeToWorldIT();		/**< transforms normals from eye to world */

	virtual const Matrix4f &GetProjection();			/**< transforms points from eye to NDC */
	virtual const Matrix4f &GetProjectionI();			/**< transforms points from NDC to eye */

	virtual const Matrix4f &GetClipToScreen();		/**< transforms points from clip space to screen space */

	virtual int GetNumLights();							/**< gets the number of lights */
	virtual LightImplSW3D **GetLightList();			/**< gets an array of pointers to the lights */

	virtual bool DebugIsValid();

	virtual bool LogErrorMsg(int32 severity, const char *message = NULL);
	virtual void OutOfMemoryError();

protected:
	friend class RendererSM3D;
	friend class RendererSW3D;
	friend class RendererExp3D;

	RendererSW3D *Renderer;
};





#endif
