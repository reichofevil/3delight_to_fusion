#ifndef _LIGHTBASEIMPLSW3D_H_
#define _LIGHTBASEIMPLSW3D_H_

#include "3D_LightBase.h"
#include "3D_MaterialBase.h"

class StreamSet3D;
class ShadeContext3D;
class RenderContextSW3D;







// 
//
// LightImplSW3D - the base class to be used by all lights that work with the fixed function OpenSW renderer
//
// 

class FuSYSTEM_API LightImplSW3D : public LightImpl3D
{
	FuDeclareClass(LightImplSW3D, LightImpl3D);

public:

public:
	LightImplSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightImplSW3D(const LightImplSW3D &toCopy);	// purposefully left unimplemented
	virtual ~LightImplSW3D();

	/**
	 * Lights only have access to the original unmodified streams.  They cannot output modified streams or output other quantities.
	 * They cannot have outputs.  When Illuminate() is called, the light writes its output into the shade context.
	 */
	virtual ParamBlockSW *CreateParamBlock(RenderContextSW3D &rc, TagList *tags = NULL);

	virtual bool DoPreRender(RenderContextSW3D &rc, TagList *tags = NULL);
	virtual void DoPostRender(RenderContextSW3D &rc, TagList *tags = NULL);
	virtual bool DoActivate(RenderContextSW3D &rc, TagList *tags = NULL);
	virtual void DoDeactivate(RenderContextSW3D &rc, TagList *tags = NULL);

	// Everytime the renderer needs to use a light, it will Active/Deactive it.  All the Activate/Deactivate calls in a particular render
	// pass will be bracketed by a PreRender/PostRender pair.  Deactivate/PostRender give the light a chance to do any needed cleanup.  
	// If PreRender() returns true PostRender() will always be called.  If PreRender() fails then PostRender() will not be called.
	// If Activate() returns true Deactivate() will always be called.  If Activate() fails then Deactivate() will not be called.
	virtual bool PreRender(RenderContextSW3D &rc, TagList *tags = NULL);
	virtual bool Activate(RenderContextSW3D &rc,	TagList *tags = NULL);
	virtual void Deactivate(RenderContextSW3D &rc, TagList *tags = NULL);
	virtual void PostRender(RenderContextSW3D &rc, TagList *tags = NULL);


	// Illuminate is called per-fragment to compute the lighting contribution for the pixel currently being shaded pixel.  
	// If Illuminate() is going to return 'true' it must write the following variables (even if to only set them to zero):
	//   sc.AmbientLightColor - ambient light contribution to current fragment
	//   sc.LightColor - light color contribution to current pixel
	///  sc.LightVector = a normalized vector pointing from the pixel to the light
	// Returns:
	//   true to indicate the pixel was affected by the light, false to indicate the pixel was not affected by the light (if false is returned
	//   a material use this as an 'early out' to stop further processing of the light contribution at that point)
	// The color values will get used by the material of the surface, which is responsible for determining how it will respond to the light.
	virtual bool Illuminate(ShadeContext3D &sc, ParamBlockSW *lightBlock);
	virtual void Shadow(ShadeContext3D &sc, ParamBlockSW *lightBlock);

	static LightImplSW3D *CreateLight(RenderContextSW3D &rc, LightData3D *lightData); // throws CMemoryException
};




#endif
