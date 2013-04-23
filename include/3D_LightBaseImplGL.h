#ifndef _LIGHTBASEIMPLGL3D_H_
#define _LIGHTBASEIMPLGL3D_H_

#include "3D_CgWrapper.h"
#include "3D_LightBase.h"




class RenderContextGL3D;
class ShadowBuffer3D;


// Instead of calling OpenGL commands directly in plugins to setup the OpenGL lights, we pass one of these structures around.  
// This keeps all the OpenGL calls inside of the engine and allows the renderer to handle multiple lights and resource limits.
struct LightStateFf3D
{
	// Lights must be one of the 4 hardware accelarated light types (or a user defined type)
	enum LightTypeFf
	{							// For each type you must supply the following parameters:
		Ambient,				//	 Ambient Color
		Point,				//  Colors, Position, decay rates
		Directional,		//  Colors, Direction
		Spot,					//  Colors, Position, Direction, decay rates, cones angles, falloff
		User
	} LightType;

	Color4f AmbientColor;			// OpenGL allows different colors for different components of the light, but Fusion's lighting
	Color4f DiffuseColor;			// model assumes that they are all the same color (materials have the individual ambient,
	Color4f SpecularColor;		// diffuse and specular settings).  Note:  alpha value is ignored

	Vector4f Position;				// position of light in local coords, usually (0, 0, 0) (the renderer will apply any necessary transforms)
	Vector4f Direction;				// light direction in local coords, usally (0, 0, -1) (the renderer will apply any necessary transforms)

	float32 ConstantDecay, LinearDecay, QuadraticDecay;

	float32 OuterConeAngle, InnerConeAngle, Falloff;				// outer/inner cone angles are in degrees

	TagList *Tags;						// future expandibility
};







// 
//
// LightImplGL3D - the base class to be used by all lights that work with the fixed function OpenGL renderer
//
// 

class FuSYSTEM_API LightImplGL3D : public LightImpl3D
{
	FuDeclareClass(LightImplGL3D, LightImpl3D);

public:
	ShadowBuffer3D *ShadowBuffer;				// used internally by the renderer

	// This will be accessed by the renderer inbetween Activate()/Deactivate() calls.
	LightStateFf3D LightState;

public:
	LightImplGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightImplGL3D(const LightImplGL3D &toCopy);		// purposefully left unimplemented
	virtual ~LightImplGL3D();

	static LightImplGL3D *CreateLight(RenderContextGL3D &rc, LightData3D *lightData);

	virtual bool DoPreRender(RenderContextGL3D &rc, TagList *tags = NULL);
	virtual void DoPostRender(RenderContextGL3D &rc, TagList *tags = NULL);
	virtual bool DoActivate(RenderContextGL3D &rc, TagList *tags = NULL);
	virtual void DoDeactivate(RenderContextGL3D &rc, TagList *tags = NULL);

	// Everytime the renderer needs to use a light, it will Active/Deactive it.  All the Activate/Deactivate calls in a particular render
	// pass will be bracketed by a PreRender/PostRender pair.  The GL renderer uses the 'LightInfo' data member to setup the light.
	// The 'LightInfo' data member must be set either in PreRender() or in the Activate() call.  Deactivate/PostRender give the light
	// a chance to do any needed cleanup.  
	// 
	// Note that if PreRender() fails PostRender() will not get called, so if PreRender() is going to fail, it should also do any necessary
	// cleanup.  The same holds with Activate()/Deactivate() -- if Activate() fails, Deactivate() will not get called.  If PreRender() fails
	// on a light, Activate() will never be called.  Note that if even if Activate() fails, the framework is still responsible for calling
	// PostRender().
	virtual bool PreRender(RenderContextGL3D &rc, TagList *tags = NULL);
	virtual bool Activate(RenderContextGL3D &rc,	TagList *tags = NULL);
	virtual void Deactivate(RenderContextGL3D &rc, TagList *tags = NULL);
	virtual void PostRender(RenderContextGL3D &rc, TagList *tags = NULL);

	virtual ShaderString3D CreateShaderString(RenderContextGL3D &rc, TagList *tags = NULL);
	virtual bool DoCreateShaderString(RenderContextGL3D &rc, TagList *tags = NULL);
	virtual const char *GetShaderFilename(RenderContextGL3D &rc, TagList *tags = NULL);
	virtual ShadeNodeCg *CreateShadeNode(RenderContextGL3D &rc, ShaderCg *shader, TagList *tags = NULL);
	virtual int GetState(RenderContextGL3D &rc, char buf[16]);

	// Utility function that loads a shader string from a file and returns it.  The file is assumed to be located within the
	// c:/Fusion/Shaders directory.
	virtual ShaderString3D LoadShaderString(RenderContextGL3D &rc, const char *file);

	// Overrides used internally by renderer.
	virtual void Init();
	virtual void Free();
};




#endif