#ifndef _SHADE_CONTEXT_H_
#define _SHADE_CONTEXT_H_


#include "Types.h"
#include "GLTools.h"
#include "VectorMath.h"
#include "3D_MtlBaseImplSW.h"
#include "3D_LightBaseImplSW.h"

const int MAX_SW_ATTRS = 32;

class Node3D;
class ParamBlockSW;
class RenderContextSW3D;




// * TransmittanceContext3D - Used with Transmit() callback
class TransmittanceContext3D
{
public:
	// In
	MtlImplSW3D *Mtl;

	// Out
	Color4f Transmittance;
};



// * ShadeContext3D - Used for passing information needed to evaluate a light/material and the results of such an evaluation.
// Each time a fragment is to be shaded by a material or illuminated by a light, a ShadeContext3D structure is passed into the Shade()/Illuminate()
//   callback.  If it is being passed to a light, the lightcontext 'lc' information is valid.  If it is being passed to a material, then the 
//   materialcontext 'mc' is valid.
// The shadecontext contains a pointer to the rendercontext which also has lots of useful information about the current render state, such as the 
//   camera transform.
// For each user attribute, the SW renderer generates 4 vectors of values.  First at each corner of the triangle it divides the attribute by 
//   W and then interpolates that across the triangle to find the value of the attribute at the center of the current pixel and stores that in
//   UserAttribW.  GradX/GradY contain the gradients of UserAttribW in the X & Y directions.  Finally, the value of UserAttribW is multiplied
//   by W to give UserAttrib.  Let (x, y, z, w) be a post-projection position and an attribute (a, b, c, d), then the members of shadecontext are:
//   - invW: during rendering, the value of 1/W is computed at each vertex and then interpolated across the triangle to center of the current pixel
//   - W: this is the inverse of invW at the center of the current pixel, ie. W = 1/(invW) = 1/(1/W)  
//   - dz_x, dz_y: the eyespace slopes dz/dx, dz/dy computed at the center of the pixel.  Note that for a perspective transform, these slopes are
//		             not constant over the surface of a pixel
//   - ScreenPos:  Stores (screenX, screenY, ndcZ, invW):
//      screenX, screenY: are the screen space position of the fragment being shaded.  Note that edges of pixels have integer valued 
//         coordinates in SW renderer and the values given here are for the center of the pixel so they'll always be of the form screenX = intX + 0.5f 
//         and screenY = intY + 0.5f, where (intX, intY) is the integer position of the pixel.  Pixels are numbered starting at the (0, 0)th pixel in 
//         the lower left corner to the (viewportWidth-1, viewportHeight-1)th pixel in the upper right corner.
//	     ndcZ:  this is the value of z in normalized device coordinates (-1 <= z <= 1, where -1 is near, and +1 is far) for the center of the pixel
//   - ScreenPosGradX:  Stores the change in (screenX, screenY, ndcZ, invW) over the width of a pixel going from left to right across a scanline.
//	                     Note that this will always be of the form (0.0f, 1.0f, delta(ndcZ), invW).
//   - ScreenPosGradY:  Stores the change in (screenX, screenY, ndcZ, invW) over the height of a pixel going from the bottom to the top of the
//	                     screen.  Note that this will always be of the form (1.0f, 0.0f, delta(ndcZ), invW).
// Notes: 
//  - Because the gradients are divided 1/w, they will be constant over the surface of a triangle, ie. independent of perspective
//  - all the variables in shadecontext should never be modified from within Shade() or Illuminate() with the exception of some of the
//    variables in lc, mc, tc.



/**
 * Render state made available in per-fragment Shade() operations.
 */
class ShadeContext3D
{
public:
	RenderContextSW3D *rc;

	int NumLights;
	LightImplSW3D **LightList;
	//uint8 *DoShadows;

	int NumProjectors;
	LightImplSW3D **ProjectorList;

	bool HiQ;
	bool DoLighting;
	bool DoShadows;

	bool DoAmbient;
	bool DoDiffuse;
	bool DoSpecular;

	// Illuminate() is responsible for writing these.  They will be valid inside Shade().  They will never be uninitialized garbage values such as
	// float specials (NaN, Inf).
	ALIGN16 Color4f LightColor;					// the diffuse/specular light contribution  (texture projectors also put their contribution here)
	ALIGN16 Color4f AmbientLightColor;			// the ambient light contribution at the point to be lit
	ALIGN16 Vector3f LightVector;					// a vector pointing from the point currently being shaded to the lights center
	float32 Pad;

	//Color4f UnShadowedLightColor;

	// Shadow() is responsible for writing these two variables.
	ALIGN16 Color4f Transmittance;
	ALIGN16 Color4f TransmittedLight;

	int ThreadNumber;							/**< The thread number (0, ... MaxThreads-1) that is currently executing.  This is usually accessed in MtlImplSW3D::Shade(). */
	int MaxThreads;							/**< The total number of threads executing. */

	MtlImplSW3D *Mtl;

	int X, Y;									// the center of the pixel that we are shading

	float32 dZ_dX, dZ_dY;					// gradients of eye space position

	float32 W, IW, dIW_dX, dIW_dY;		// w, 1/w, d(1/w)/dx, d(1/w)/dy
	float32 Z, ZIW, dZIW_dX, dZIW_dY;	// eyeZ, eyeZ/W, d(eyeZ/W)/dx, d(eyeZ/W)/dy

	ParamBlockSW **MaterialBlocks;
	ParamBlockSW **LightBlocks;
	ParamBlockSW **ProjectorBlocks;

public:
	ShadeContext3D();
	~ShadeContext3D();

	bool Init(RenderContextSW3D &rc);

	LightImplSW3D *GetLight(int32 i)									{ return LightList[i]; }
	ParamBlockSW &GetLightBlock(int32 i)							{ return *LightBlocks[i]; }
	ParamBlockSW &GetMaterialBlock(const MtlImplSW3D *mtl)	{ return *MaterialBlocks[mtl->NodeIndex]; }
	bool Illuminate(int32 lightIndex)								{ return LightList[lightIndex]->Illuminate(*this, LightBlocks[lightIndex]); }
	bool CastsShadows(int32 lightIndex)								{ return DoShadows && LightList[lightIndex]->CastsShadows; }
	void Shadow(int32 lightIndex)										{ return LightList[lightIndex]->Shadow(*this, LightBlocks[lightIndex]); }
	bool Project(int32 projIndex)										{ return ProjectorList[projIndex]->Illuminate(*this, ProjectorBlocks[projIndex]); }
};




#endif
