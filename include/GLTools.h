#ifndef _GL_TOOLS_H_
#define _GL_TOOLS_H_

#include "GLExtensions.h"
#include "LockableObject.h"

#include "3D_GLContext.h"
#include "3D_DataTypes.h"





class ShaderStream;


// The GLTools3D class encapsulates and controls OpenGL access for all the tools
// that use it.  There must be only one instance of GLTools3D.

class ToolsException {};





// This is simply parsed from the vendor string
enum CV_CardVendor
{
	CV_Nvidia		= 0,						// update CV_NumVendors when adding new vendors
	CV_ATI			= 1,
	CV_Matrox		= 2,
	CV_Intel			= 3,
	CV_S3				= 4,
	CV_3DLabs		= 5,
	CV_Microsoft	= 6,
	CV_Unknown		= 9999,
	CV_NumVendors	= 7
};

// These functions are in the 3D DLL because they need to be accessed outside of the 3D DLL, so they got to go here
FuSYSTEM_API extern bool IsDepthPeelingSupported();
FuSYSTEM_API extern bool IsShadowingSupported();
FuSYSTEM_API extern bool HardwareSupports(SP_ShadePathGL3D path);


class FuSYSTEM_API FramebufferMode3D
{
public:
	FramebufferMode3D();
	FramebufferMode3D(int nSamples, F3D_DataType dataType);
	bool operator==(FramebufferMode3D &m) const;	
	char *AssignName();
	void Free();

public:
	char *Name;
	F3D_DataType DataType;
	uint16 NumSamples;
};


class FuSYSTEM_API GLTools3D : public Object
{
	FuDeclareClass(GLTools3D, Object);

public:
	// All tools/operators that use OpenGL to do computations should do all their work on this one context.
	GLContext3D *Context;

	// This is just an inactive context that sits around that every other context in Fusion share's itself with.
	// It is here because it is a workaround for a problem I was having with wglShareLists and current contexts.
	// Originally, I had all contexts sharing themselves with the tools 'Context' above, but this wasn't working,
	// as wglShareLists() failed if Context was current to any thread.  HubContext is never current to any thread
	// so that wglShareLists() can never fail.  
	GLContext3D *HubContext;

	static bool IsInitialized;

	CV_CardVendor Vendor;
	
	GLint MaxTextureUnits;
	//GLint MaxTextureSize;
	GLint MaxPow2TextureSize;
	GLint MaxRectTextureSize;
	GLint MaxCubemapTextureSize;

	GLint MaxAttribStackDepth;
	GLint MaxClientAttribStackDepth;
	GLint MaxNameStackDepth;

	GLint MaxModelViewMatrixStackDepth;
	GLint MaxProjectionMatrixStackDepth;
	GLint MaxTextureMatrixStackDepth;
	GLint MaxColorMatrixStackDepth;
	
	union
	{
		struct
		{
			GLint MaxViewportWidth;
			GLint MaxViewportHeight;
		};

		GLint MaxViewportDimensions[2];
	};

	GLint MaxDrawBuffers;
	GLint MaxClipPlanes;
	GLint MaxLights;

	GLuint FontListBase;

	struct 
	{
      GLint VP_MaxVertexAttribs;
		GLint VP_MaxMatrices;
		GLint VP_MaxMatrixStackDepth;

		GLint VP_MaxInstructions;
		GLint VP_MaxNativeInstructions;
		GLint VP_MaxTemporaries;
		GLint VP_MaxNativeTemporaries;
		GLint VP_MaxParameters;
		GLint VP_MaxNativeParameters;
		GLint VP_MaxAttribs;
		GLint VP_MaxNativeAttribs;
		GLint VP_MaxAddressRegisters;
		GLint VP_MaxNativeAddressRegisters;
		GLint VP_MaxLocalParameters;
		GLint VP_MaxEnvParameters;
	};

	struct 
	{
      GLint FP_MaxTextureCoords;
		GLint FP_MaxTextureImageUnits;

		GLint FP_MaxALUInstructions;
		GLint FP_MaxNativeALUInstructions;
		GLint FP_MaxTexInstructions;
		GLint FP_MaxNativeTexInstructions;
		GLint FP_MaxInstructions;
		GLint FP_MaxTextureIndirections;
		GLint FP_MaxNativeInstructions;
		GLint FP_MaxTemporaries;
		GLint FP_MaxNativeTemporaries;
		GLint FP_MaxParameters;
		GLint FP_MaxNativeParameters;
		GLint FP_MaxAttribs;
		GLint FP_MaxNativeAttribs;
		GLint FP_MaxLocalParameters;
		GLint FP_MaxEnvParameters;
	};

	// a block of memory that anyone can use for any purpose, current used for texture uploading
	void *TempBuffer;
	LockableObject TempBufferLock;
	uint32 TempBufferSize;

	GLuint *TempPBO;								// if PBOs are allowable...
	LockableObject TempPBOLock;
	uint32 TempPBOSize;
	uint32 PaddedTempPBOSize;
	GLenum TempPBOUsage;

	std::vector<FramebufferMode3D> FBModes, FBPbufferModes;				// framebuffer modes

	bool AllowPBOs;								// if this is false PBOs will not be used (it will normally be true if PBOs are supported and FUSION_USE_PBO is true)

public:
	bool Init(FusionDoc *doc);	
	void Shutdown();

	/// Use the global instance 'GLTools', there must only be one copy of GLTools3D in existance.
	GLTools3D();
	GLTools3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	~GLTools3D();

	GLTools3D &operator=(const GLTools3D &rhs);					// ni
	GLTools3D(const GLTools3D &rhs);									// ni

	bool ObtainContext() { return Context ? Context->Obtain() : false; }
	void ReleaseContext() { Context->Release(); }

	bool GLToolsSupported() { return GL_Accelerated_Tools_Supported; }		// TODO

	// Shares lists/textures/programs in the ShareContext with the supplied context.
	bool ShareLists(HGLRC rc_to_share_with);

	void UseFontLists()		{	glListBase(FontListBase);	}

	CV_CardVendor DetermineCardVendor();

	bool CreateTempBuffers();
	void FreeTempBuffers();
	void *ObtainTempBuffer();			// there is only one tempbuffer that everyone shares
	void ReleaseTempBuffer();

	GLuint ObtainTempPBO();
	void ReleaseTempPBO();
	void *MapTempPBO(uint32 &offset);
	void UnmapTempPBO();

	bool DumpGLHardwareLimitations(std::ostream &buffer);

	// Framebuffer modes enumeration
	bool IsFBModeSupported(F3D_DataType dataType, int nSamples, bool pbuffer);		
	bool EnumFBModes(GLContext3D *context);												// called once at startup and puts values in a list
	void FreeFBModes();

	// Fixed-function OpenGL isn't capable of displaying any specular exponent.  This limits it to valid extents (which are card dependent).
	float32 LimitSpecularExponent(float32 exp);

	bool Is_10_10_10_2_32_Supported();

protected:
	friend class GLContext3D;

	bool GL_Accelerated_Tools_Supported;

	void AddContext(GLContext3D *ctx);
	void RemoveContext(GLContext3D *ctx);

	LockableObject m_ContextListLock;
	std::vector<GLContext3D*> m_ContextList;

protected:
	bool InitExts();			// initialize GL extensions
	void ShutdownExts();		// zero out GL extension func ptrs so no one uses them accidentally

	bool CreateGlobalFontLists();
	void FreeGlobalFontLists();

	void DetermineHardwareLimitations();
};


extern FuSYSTEM_API GLTools3D GLTools;				// The (one and only) global instance of GLtools



#include <GL/gl.h>
#include <GL/glu.h>

struct DebugGLState
{
	// shaders
	GLboolean FragmentProgramEnabled;
	GLboolean VertexProgramEnabled;
	GLint CurrentVertexProgram;
	GLint CurrentFragmentProgram;

	// texturing
	GLboolean EnabledTex1D, EnabledTex2D, EnabledTex3D, EnabledTexRect;
	GLint Bound1D, Bound2D, Bound3D, BoundRect;
	GLint Viewport[4];		// x, y, w, h

	// FBOs
	GLint BoundFBO, BoundRBO;
	GLint FBOComplete;
	GLint FBOAttachedColorObj[16];
	GLint FBOAttachedDepthObj, FBOAttachedStencilObj;

	// VBOs/IBOs
	GLint ArrayBufferBinding;					// eg. 7 -- this will have the value of the GL buffer object
	GLint IndexArrayBufferBinding;			// ????? -- don't know what this is used for - use element array   
	GLint VertexArrayBufferBinding;			// eg. 7 -- these would be all 7 if the normals, colors, texcoord, vertex were all stored in the same VBO like for BSP vertices
	GLint NormalArrayBufferBinding;			// eg. 7
	GLint ColorArrayBufferBinding;			// eg. 7
	GLint TexCoordArrayBufferBinding;		// eg. 7
	GLint ElementArrayBufferBinding;			// eg. 1 -- this is the one that is used to store indices

	// attribute/matrix stacks 
	GLint MVDepth, ProjDepth, TextureDepth, AttribDepth, NameDepth;
	Matrix4f ProjMatrix, ModelViewMatrix, TextureMatrix, ColorMatrix;

	// blendfunc
	GLint BlendSrc, BlendDst;
	GLint BlendEquation;
	GLint BlendSepSrcRGB;
	GLint BlendSepSrcA;
	GLint BlendSepDstRGB;
	GLint BlendSepDstA;
	GLfloat BlendColor[4];

	// lines
	GLfloat LineWidth;

	// rasterization
	GLfloat CurrentRasterColor[4];
	GLint PolyMode[2];						// GL_POINT = 0x1B00, GL_LINE = 0x1B01, GL_FILL = 0x1B02

	// framebuffer
	GLint DepthBits, AlphaBits;
	GLint StencilBits;
	GLboolean StencilEnabled;
	GLboolean ScissorEnabled;
	GLboolean DepthTestEnabled;
	GLboolean AlphaTestEnabled;
	GLint AlphaTestFunc;						// GL_GREATER = 0x0204, GL_GEQUAL 0x0206
	GLfloat AlphaTestRef;
	GLboolean DitheringEnabled;
	GLboolean BlendingEnabled;
	GLint DepthFunc;							// GL_LESS = 0x0201, GL_LEQUAL = 0x0203, GL_EQUAL = 0x0202, GL_NOTEQUAL = 0x0205
	GLboolean DepthWriteMask;				// true means its enabled for writing

	// vertex
	GLboolean RescaleNormalEnabled;
	GLboolean NormalizeEnabled;
	GLboolean CullFaceEnabled;
	GLint CullFaceMode;					// GL_FRONT = 0x0404, GL_BACK = 0x0405, GL_FRONT_AND_BACK = 0x0408
	GLint FrontFace;						// GL_CW = 0x0900, GL_CCW = 0x0901
	GLboolean ClipPlaneEnabled[6];
	GLfloat CurrentColor[4];
	GLfloat CurrentNormal[4];

	// pixel
	GLboolean FogEnabled;
	GLfloat FogColor[4];
	GLboolean SmoothShadeModel;

	// lighting
	GLboolean LightingEnabled;
	GLboolean LocalViewer;
	GLboolean TwoSidedLighting;
	GLfloat LightModelAmbientColor[4];

	struct 
	{
		GLboolean Enabled;
		GLfloat Ambient[4];
		GLfloat Diffuse[4];
		GLfloat Specular[4];
		GLfloat Position[4];
		GLfloat SpotDirection[3];
		GLfloat SpotExponent;
		GLfloat SpotCutoff;
		GLfloat ConstAttenuation;
		GLfloat LinearAttenuation;
		GLfloat QuadraticAttentuation;
	} Light[8];

	// materials
	GLfloat AmbientFront[4];
	GLfloat AmbientBack[4];
	GLfloat DiffuseFront[4];
	GLfloat DiffuseBack[4];
	GLfloat SpecularFront[4];
	GLfloat SpecularBack[4];
	GLfloat EmissionFront[4];
	GLfloat EmissionBack[4];
	GLfloat ShininessFront;		// specular exponent
	GLfloat ShininessBack;

	GLboolean ColorMaterialEnabled;

	// pixel storage
	GLboolean PackSwapBytes;
	GLboolean PackLSBFirst;
	GLint PackRowLength;
	GLint PackImageHeight;
	GLint PackSkipRows;
	GLint PackSkipPixels;
	GLint PackSkipImages;
	GLint PackAlignment;

	GLboolean UnpackSwapBytes;
	GLboolean UnpackLSBFirst;
	GLint UnpackRowLength;
	GLint UnpackImageHeight;
	GLint UnpackSkipRows;
	GLint UnpackSkipPixels;
	GLint UnpackSkipImages;
	GLint UnpackAlignment;
};


#ifdef _DEBUG

inline FuSYSTEM_API void DUMP_GL_STATE(DebugGLState &state)
{
	int i;

	// texturing
	glGetBooleanv(GL_TEXTURE_1D, &state.EnabledTex1D);
	glGetBooleanv(GL_TEXTURE_2D, &state.EnabledTex2D);
	glGetBooleanv(GL_TEXTURE_3D, &state.EnabledTex3D);

	glGetIntegerv(GL_TEXTURE_BINDING_1D, &state.Bound1D);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &state.Bound2D);
	glGetIntegerv(GL_TEXTURE_BINDING_3D, &state.Bound3D);

	if (ExtSupported(GL_NV_texture_rectangle_df) || ExtSupported(GL_EXT_texture_rectangle_df) || ExtSupported(GL_ARB_texture_rectangle_df))
	{
		glGetBooleanv(GL_TEXTURE_RECTANGLE_NV, &state.EnabledTexRect);
		glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE_NV, &state.BoundRect);	
	}

	// FBOs
	if (ExtSupported(GL_EXT_framebuffer_object_df))
	{
		glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &state.BoundFBO);
		glGetIntegerv(GL_RENDERBUFFER_BINDING_EXT, &state.BoundRBO);
		if (state.BoundFBO)
		{
			state.FBOComplete = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

			GLint maxattach;
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxattach);
			for (int i = 0; i < maxattach; i++)
				glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT, &state.FBOAttachedColorObj[i]);
			for (int i = maxattach; i < 16; i++)
				state.FBOAttachedColorObj[i] = -1;
			glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,   GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT, &state.FBOAttachedDepthObj);
//			glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT, &state.FBOAttachedStencilObj);		// throws error on older drivers
		}
	}

	// VBOs and IBOs
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING_ARB, &state.ArrayBufferBinding);
	glGetIntegerv(GL_INDEX_ARRAY_BUFFER_BINDING_ARB, &state.IndexArrayBufferBinding);
	glGetIntegerv(GL_VERTEX_ARRAY_BUFFER_BINDING_ARB, &state.VertexArrayBufferBinding);
	glGetIntegerv(GL_NORMAL_ARRAY_BUFFER_BINDING_ARB, &state.NormalArrayBufferBinding);
	glGetIntegerv(GL_COLOR_ARRAY_BUFFER_BINDING_ARB, &state.ColorArrayBufferBinding);
	glGetIntegerv(GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING_ARB, &state.TexCoordArrayBufferBinding);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB, &state.ElementArrayBufferBinding);

	// blendfunc
	glGetIntegerv(GL_BLEND_EQUATION, &state.BlendEquation);
	glGetIntegerv(GL_BLEND_SRC, &state.BlendSrc);
	glGetIntegerv(GL_BLEND_DST, &state.BlendDst);
	glGetFloatv(GL_BLEND_COLOR, state.BlendColor);

	if (ExtSupported(GL_EXT_blend_func_separate_df))
	{
		glGetIntegerv(GL_BLEND_SRC_RGB_EXT,   &state.BlendSepSrcRGB);
		glGetIntegerv(GL_BLEND_SRC_ALPHA_EXT, &state.BlendSepSrcA);
		glGetIntegerv(GL_BLEND_DST_RGB_EXT,   &state.BlendSepDstRGB);
		glGetIntegerv(GL_BLEND_DST_ALPHA_EXT, &state.BlendSepDstA);
	}
	else
		state.BlendSepSrcRGB = state.BlendSepSrcA = state.BlendSepDstRGB = state.BlendSepDstA = 0;

	// lines
	glGetFloatv(GL_LINE_WIDTH, &state.LineWidth);

	// rasterization
	glGetFloatv(GL_CURRENT_RASTER_COLOR, state.CurrentRasterColor);
	glGetIntegerv(GL_POLYGON_MODE, state.PolyMode);

	// matrix/attrib stacks
	glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &state.MVDepth);
	glGetIntegerv(GL_PROJECTION_STACK_DEPTH, &state.ProjDepth);
	glGetIntegerv(GL_TEXTURE_STACK_DEPTH, &state.TextureDepth);
	glGetIntegerv(GL_ATTRIB_STACK_DEPTH, &state.AttribDepth);
	glGetIntegerv(GL_NAME_STACK_DEPTH, &state.NameDepth);

	glGetFloatv(GL_PROJECTION_MATRIX, state.ProjMatrix.M);
	state.ProjMatrix = state.ProjMatrix.Transpose();

	glGetFloatv(GL_MODELVIEW_MATRIX, state.ModelViewMatrix.M);
	state.ModelViewMatrix = state.ModelViewMatrix.Transpose();

	glGetFloatv(GL_TEXTURE_MATRIX, state.TextureMatrix.M);
	state.TextureMatrix = state.TextureMatrix.Transpose();

	glGetIntegerv(GL_VIEWPORT, state.Viewport);

	//glGetFloatv(GL_COLOR_MATRIX, state.ColorMatrix.M);			// calling this causes random crashes on certain ATI cards + drivers during Fusion use
	//state.ColorMatrix = state.ColorMatrix.Transpose();

	// framebuffer
	glGetIntegerv(GL_DEPTH_BITS, &state.DepthBits);
	glGetIntegerv(GL_ALPHA_BITS, &state.AlphaBits);
	glGetIntegerv(GL_STENCIL_BITS, &state.StencilBits);
	glGetBooleanv(GL_STENCIL_TEST, &state.StencilEnabled);
	glGetBooleanv(GL_SCISSOR_TEST, &state.ScissorEnabled);
	glGetBooleanv(GL_DEPTH_TEST, &state.DepthTestEnabled);
	glGetBooleanv(GL_ALPHA_TEST, &state.AlphaTestEnabled);
	glGetIntegerv(GL_ALPHA_TEST_FUNC, &state.AlphaTestFunc);
	glGetFloatv(GL_ALPHA_TEST_REF, &state.AlphaTestRef);
	glGetBooleanv(GL_DITHER, &state.DitheringEnabled);
	glGetBooleanv(GL_BLEND, &state.BlendingEnabled);
	glGetIntegerv(GL_DEPTH_FUNC, &state.DepthFunc);
	glGetBooleanv(GL_DEPTH_WRITEMASK, &state.DepthWriteMask);

	// vertex
	glGetBooleanv(GL_RESCALE_NORMAL, &state.RescaleNormalEnabled);
	glGetBooleanv(GL_NORMALIZE, &state.NormalizeEnabled);
	glGetBooleanv(GL_CULL_FACE, &state.CullFaceEnabled);
	glGetIntegerv(GL_CULL_FACE_MODE, &state.CullFaceMode);
	glGetIntegerv(GL_FRONT_FACE, &state.FrontFace);
	glGetFloatv(GL_CURRENT_COLOR, state.CurrentColor);
	glGetFloatv(GL_CURRENT_NORMAL, state.CurrentNormal);

	for (i = 0; i < 6; i++)
		glGetBooleanv(GL_CLIP_PLANE0 + i, &state.ClipPlaneEnabled[i]);

	// pixel
	glGetBooleanv(GL_FOG, &state.FogEnabled);
	glGetFloatv(GL_FOG_COLOR, state.FogColor);

	int shadeModel; 
	glGetIntegerv(GL_SHADE_MODEL, &shadeModel);
	state.SmoothShadeModel = (shadeModel == GL_SMOOTH);

	// lighting
	glGetBooleanv(GL_LIGHTING, &state.LightingEnabled);
	glGetBooleanv(GL_LIGHT_MODEL_LOCAL_VIEWER, &state.LocalViewer);
	glGetBooleanv(GL_LIGHT_MODEL_TWO_SIDE, &state.TwoSidedLighting);
	glGetFloatv(GL_LIGHT_MODEL_AMBIENT, state.LightModelAmbientColor);

	for (i = 0; i < 8; i++)
	{
		glGetBooleanv(GL_LIGHT0 + i, &state.Light[i].Enabled);
		glGetLightfv(GL_LIGHT0 + i, GL_AMBIENT, state.Light[i].Ambient);
		glGetLightfv(GL_LIGHT0 + i, GL_DIFFUSE, state.Light[i].Diffuse);
		glGetLightfv(GL_LIGHT0 + i, GL_SPECULAR, state.Light[i].Specular);
		glGetLightfv(GL_LIGHT0 + i, GL_POSITION, state.Light[i].Position);
		glGetLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, state.Light[i].SpotDirection);
		glGetLightfv(GL_LIGHT0 + i, GL_AMBIENT, state.Light[i].Ambient);
		glGetLightfv(GL_LIGHT0 + i, GL_SPOT_EXPONENT, &state.Light[i].SpotExponent);
		glGetLightfv(GL_LIGHT0 + i, GL_SPOT_CUTOFF, &state.Light[i].SpotCutoff);
		glGetLightfv(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, &state.Light[i].ConstAttenuation);
		glGetLightfv(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, &state.Light[i].LinearAttenuation);
		glGetLightfv(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, &state.Light[i].QuadraticAttentuation);
	}

	// materials
	glGetMaterialfv(GL_FRONT, GL_AMBIENT, state.AmbientFront);
	glGetMaterialfv(GL_FRONT, GL_DIFFUSE, state.DiffuseFront);
	glGetMaterialfv(GL_FRONT, GL_SPECULAR, state.SpecularFront);
	glGetMaterialfv(GL_FRONT, GL_EMISSION, state.EmissionFront);
	glGetMaterialfv(GL_FRONT, GL_SHININESS, &state.ShininessFront);

	glGetMaterialfv(GL_BACK, GL_AMBIENT, state.AmbientBack);
	glGetMaterialfv(GL_BACK, GL_DIFFUSE, state.DiffuseBack);
	glGetMaterialfv(GL_BACK, GL_SPECULAR, state.SpecularBack);
	glGetMaterialfv(GL_BACK, GL_EMISSION, state.EmissionBack);
	glGetMaterialfv(GL_BACK, GL_SHININESS, &state.ShininessBack);

	state.ColorMaterialEnabled = glIsEnabled(GL_COLOR_MATERIAL);

	// pixel storage
	glGetBooleanv(GL_PACK_SWAP_BYTES, &state.PackSwapBytes);
	glGetBooleanv(GL_PACK_LSB_FIRST, &state.PackLSBFirst);
	glGetIntegerv(GL_PACK_ROW_LENGTH, &state.PackRowLength);
	glGetIntegerv(GL_PACK_IMAGE_HEIGHT, &state.PackImageHeight);
	glGetIntegerv(GL_PACK_SKIP_ROWS, &state.PackSkipRows);
	glGetIntegerv(GL_PACK_SKIP_PIXELS, &state.PackSkipPixels);
	glGetIntegerv(GL_PACK_SKIP_IMAGES, &state.PackSkipImages);
	glGetIntegerv(GL_PACK_ALIGNMENT, &state.PackAlignment);

	glGetBooleanv(GL_UNPACK_SWAP_BYTES, &state.UnpackSwapBytes);
	glGetBooleanv(GL_UNPACK_LSB_FIRST, &state.UnpackLSBFirst);
	glGetIntegerv(GL_UNPACK_ROW_LENGTH, &state.UnpackRowLength);
	glGetIntegerv(GL_UNPACK_IMAGE_HEIGHT, &state.UnpackImageHeight);
	glGetIntegerv(GL_UNPACK_SKIP_ROWS, &state.UnpackSkipRows);
	glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &state.UnpackSkipPixels);
	glGetIntegerv(GL_UNPACK_SKIP_IMAGES, &state.UnpackSkipImages);
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &state.UnpackAlignment);

	// shaders
	if (ExtSupported(GL_ARB_vertex_program_df))
	{
		state.VertexProgramEnabled = glIsEnabled(GL_VERTEX_PROGRAM_ARB);
		glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_BINDING_ARB, &state.CurrentVertexProgram);
	}

	if (ExtSupported(GL_ARB_fragment_program_df))
	{
		state.FragmentProgramEnabled = glIsEnabled(GL_FRAGMENT_PROGRAM_ARB);
		glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_BINDING_ARB, &state.CurrentFragmentProgram);
	}
}

// insert this function into your code to get a quick debug readback of GL state
inline FuSYSTEM_API void CHECK_GL_STATE() 
{
	if (wglGetCurrentContext())
	{
		DebugGLState state;
		DUMP_GL_STATE(state);
		int breakpoint = 10;
	}
}


// BEGIN_TEXTURE_WATCH_GL/END_TEXTURE_WATCH_GL - for detecting non-thread safe drivers
#define BEGIN_TEXTURE_WATCH_GL()																		\
   std::vector<GLuint> _fun;																			\
	for (int i = 0; i < 50; i++)																		\
		if (glIsTexture(i))																				\
			_fun.push_back(i);

#define END_TEXTURE_WATCH_GL()																		\
	for (int i = 0; i < 50; i++)																		\
		if (glIsTexture(i))																				\
			FuASSERT(std::find(_fun.begin(), _fun.end(), i) != _fun.end(), (""));


// BEGIN_WATCH_GL/END_WATCH_GL - for detecting stack mismatches
#define BEGIN_WATCH_GL()																				\
	GLint _MVDepth0, _ProjDepth0, _TextureDepth0, _AttribDepth0, _NameDepth0;			\
	glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &_MVDepth0);										\
	glGetIntegerv(GL_PROJECTION_STACK_DEPTH, &_ProjDepth0);									\
	glGetIntegerv(GL_TEXTURE_STACK_DEPTH, &_TextureDepth0);									\
	glGetIntegerv(GL_ATTRIB_STACK_DEPTH, &_AttribDepth0);										\
	glGetIntegerv(GL_NAME_STACK_DEPTH, &_NameDepth0);

#define END_WATCH_GL()																					\
	GLint _MVDepth1, _ProjDepth1, _TextureDepth1, _AttribDepth1, _NameDepth1;			\
	glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &_MVDepth1);										\
	glGetIntegerv(GL_PROJECTION_STACK_DEPTH, &_ProjDepth1);									\
	glGetIntegerv(GL_TEXTURE_STACK_DEPTH, &_TextureDepth1);									\
	glGetIntegerv(GL_ATTRIB_STACK_DEPTH, &_AttribDepth1);										\
	glGetIntegerv(GL_NAME_STACK_DEPTH, &_NameDepth1);											\
	FuASSERT(_MVDepth0 == _MVDepth1, ("modelview stack depth mistmatch"));				\
	FuASSERT(_ProjDepth0 == _ProjDepth1, ("projection stack depth mistmatch"));		\
	FuASSERT(_TextureDepth0 == _TextureDepth1, ("texture stack depth mistmatch"));	\
	FuASSERT(_AttribDepth0 == _AttribDepth1, ("attrib stack depth mistmatch"));		\
	FuASSERT(_NameDepth0 == _NameDepth1, ("name stack depth mistmatch"));			


// It is illegal to put this between a glBegin()/glEnd() pair, it will give "invalid operation" errors
inline FuSYSTEM_API bool CHECK_GL_ERRORS()
{
#ifndef RELEASE_VERSION
	char *str = getenv("FUSION_NO_CHECK_GL");
	if ((!str || (str && strcmpi(str, "true"))) && wglGetCurrentContext())
	{
		GLenum e = glGetError();		// slow

		if (e)
		{
			const char *errstring = (const char*)gluErrorString(e); 
			
			DebugGLState state;
			DUMP_GL_STATE(state);		// slow

			FuASSERT(false, ("GLError: %s", errstring));
		}
		return e != GL_NO_ERROR;
	}
#endif
	return false;
}

#else
	inline FuSYSTEM_API bool CHECK_GL_ERRORS() { return false; }
	inline FuSYSTEM_API void DUMP_GL_STATE(DebugGLState &state) {}
	inline FuSYSTEM_API void CHECK_GL_STATE() {}
	#define BEGIN_WATCH_GL() NULL
	#define END_WATCH_GL() NULL
	#define BEGIN_TEXTURE_WATCH_GL() NULL
	#define END_TEXTURE_WATCH_GL() NULL
#endif


inline FuSYSTEM_API void RenderTestQuad(float32 size = 10.0f, float32 z = 0.2f)
{
	//glColor4f(0.5f, 0.5f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
	glVertex3f(-size, -size, z);
	glVertex3f(+size, -size, z);
	glVertex3f(+size, +size, z);
	glVertex3f(-size, +size, z);
	glEnd();
}


#endif