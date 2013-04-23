#ifndef _3D_RENDER_CONTEXT_GL_H
#define _3D_RENDER_CONTEXT_GL_H

#include "3D_RenderContext.h"

class ShaderCg;
class ContextCg;
class MtlInfo;
class NodeInfo;
class ShadeNodeCg;
class RendererGL3D;
class LightImplGL3D;


enum SM_SortMethodGL3D
{
	SM_ZBuffer = 0,				// only use the ZBuffer to sort transparents
	SM_BSP,							// BSP (splittables -> splitters)
	SM_QuickBSP,					// BSP (splittables allowed)
	SM_DepthPeeling,				// use depth peeling to sort transparents (this path died in 5.0 beta)
	SM_Painters,					// nyi - use painters sort (this path never really got past design)
};






// A subclassed version of RenderContext3D with renderer states specific to the OpenGL based renderer.
class /*FuSYSTEM_API*/ RenderContextGL3D : public RenderContext3D
{
	FuDeclareClass(RenderContextGL3D, RenderContext3D);

public:
	LightImplGL3D **LightList;
	int NumLights;

	LightImplGL3D **ProjectorList;
	int NumProjectors;

	SM_SortMethodGL3D SortMethod;
	SP_ShadePathGL3D ShadePath;					// Chooses how materials/lights are to be used (eg. fixed function GL, Cg, GLSL)
	SM_ShadingModel ShadingModel;

	// Store some hardware limitations here so we don't have to constantly requery GL for them
	int MaxFixedTextureUnits;						// Maximum number of bindable textures in the fixed pipeline (note that this is not the same as the number of texcoord sets, or the number of bindable textures in the programmable pipe) 
	int MaxHardwareLights;							// Max hardware lights in the fixed GL pipeline

	int MaxPow2TextureSize;
	int MaxRectTextureSize;

	F3D_DataType TextureDepth;						/**< when uploading textures you should follow this option that comes from the GL renderer texture depth option (unless your tool has a specific reason to override it).  The value will be one of F3D_INT8, F3D_INT16, F3D_FLOAT16, F3D_FLOAT32, F3D_MATCH. */

	int LightNumberFf;								// used by LightImplGL3D::Activate(), tells it which GL light to use, since we may have multiple lights active at once


	// Cg Stuff
	std::vector<ShaderString3D> ShaderStrings;

	ContextCg *CgContext;
	ShaderCg *CgShader;
	ShadeNodeCg *CgLightShadeNode;

	uint8 m_PadGLRC[512];

public:
	RenderContextGL3D();	
	RenderContextGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	RenderContextGL3D(const RenderContextGL3D &toCopy);
	virtual ~RenderContextGL3D();

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
	virtual LightImplGL3D **GetLightList();			/**< gets an array of pointers to the lights */
	
	// Gets a string that uniquely identifies the state that the renderer currently is in, for use in material/lighting shaders.
	// eg.  If shadowing is currently on, the string might contain an 's', else it might contain a '.' instead of the 's'.  The
	// contents of the string is unimportant, it is used as part of identification keys for shaders.
	virtual const char *GetShaderStateString();

	virtual bool DebugIsValid();

	int GetTextureUnit() { return 0; }	// TODO

	virtual bool LogErrorMsg(int32 severity, const char *message = NULL);
	virtual void OutOfMemoryError();

protected:
	friend class RendererGL3D;
	friend class LightSpotGL3D;
	friend class PlaneShadowBuffer3D;

	RendererGL3D *Renderer;
};



#endif