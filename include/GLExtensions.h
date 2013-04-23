#ifndef _GL_EXTENSIONS_H_
#define _GL_EXTENSIONS_H_


#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <GL/wglext.h>

#include "Types.h"
#include "GLExtTypes.h"		// must come after GL/ includes

class LockableObject;



// some handy arrays of float texture formats indexed by (numChannels - 1)
const GLenum NV_FLOAT32_FORMAT[4]			= { GL_FLOAT_R32_NV, 			GL_FLOAT_RG32_NV, 					GL_FLOAT_RGB32_NV, 	GL_FLOAT_RGBA32_NV	};
const GLenum ATI_FLOAT32_FORMAT[4]			= { GL_LUMINANCE_FLOAT32_ATI, GL_LUMINANCE_ALPHA_FLOAT32_ATI, 	GL_RGB_FLOAT32_ATI, 	GL_RGBA_FLOAT32_ATI	};
const GLenum ARB_FLOAT32_FORMAT[4]			= { GL_LUMINANCE32F_ARB,		GL_LUMINANCE_ALPHA32F_ARB,			GL_RGB32F_ARB,			GL_RGBA32F_ARB			};
const GLenum NV_FLOAT16_FORMAT[4]			= { GL_FLOAT_R16_NV,				GL_FLOAT_RG16_NV,						GL_FLOAT_RGB16_NV,	GL_FLOAT_RGBA16_NV	};
const GLenum ATI_FLOAT16_FORMAT[4]			= { GL_LUMINANCE_FLOAT16_ATI,	GL_LUMINANCE_ALPHA_FLOAT16_ATI,	GL_RGB_FLOAT16_ATI,	GL_RGBA_FLOAT16_ATI	};
const GLenum ARB_FLOAT16_FORMAT[4]			= { GL_LUMINANCE16F_ARB,		GL_LUMINANCE_ALPHA16F_ARB,			GL_RGB16F_ARB,			GL_RGBA16F_ARB			};
const GLenum OGL_UINT8_FORMAT[4]				= { GL_LUMINANCE8,				GL_LUMINANCE8_ALPHA8,				GL_RGB8,					GL_RGBA8					};
const GLenum OGL_UINT16_FORMAT[4]			= { GL_LUMINANCE16,				GL_LUMINANCE16_ALPHA16,				GL_RGB16,				GL_RGBA16				};
const GLenum OGL_EXTERNAL_FORMAT_BGRA[4]	= { GL_LUMINANCE,					GL_LUMINANCE_ALPHA,					GL_RGB,					GL_BGRA_EXT				};
const GLenum OGL_EXTERNAL_FORMAT_RGBA[4]	= { GL_LUMINANCE,					GL_LUMINANCE_ALPHA,					GL_RGB,					GL_RGBA					};

const char * const NV_FLOAT32_FORMAT_STRINGS[4]				= { "GL_FLOAT_R32_NV", 				"GL_FLOAT_RG32_NV", 						"GL_FLOAT_RGB32_NV", 	"GL_FLOAT_RGBA32_NV"		};
const char * const ATI_FLOAT32_FORMAT_STRINGS[4]			= { "GL_LUMINANCE_FLOAT32_ATI",	"GL_LUMINANCE_ALPHA_FLOAT32_ATI", 	"GL_RGB_FLOAT32_ATI", 	"GL_RGBA_FLOAT32_ATI"	};
const char * const ARB_FLOAT32_FORMAT_STRINGS[4]			= { "GL_LUMINANCE32F_ARB",			"GL_LUMINANCE_ALPHA32F_ARB",			"GL_RGB32F_ARB",			"GL_RGBA32F_ARB"			};
const char * const NV_FLOAT16_FORMAT_STRINGS[4]				= { "GL_FLOAT_R16_NV",				"GL_FLOAT_RG16_NV",						"GL_FLOAT_RGB16_NV",		"GL_FLOAT_RGBA16_NV"		};
const char * const ATI_FLOAT16_FORMAT_STRINGS[4]			= { "GL_LUMINANCE_FLOAT16_ATI",	"GL_LUMINANCE_ALPHA_FLOAT16_ATI",	"GL_RGB_FLOAT16_ATI",	"GL_RGBA_FLOAT16_ATI"	};
const char * const ARB_FLOAT16_FORMAT_STRINGS[4]			= { "GL_LUMINANCE16F_ARB",			"GL_LUMINANCE_ALPHA16F_ARB",			"GL_RGB16F_ARB",			"GL_RGBA16F_ARB"			};
const char * const OGL_UINT8_FORMAT_STRINGS[4]				= { "GL_LUMINANCE8",					"GL_LUMINANCE8_ALPHA8",					"GL_RGB8",					"GL_RGBA8"					};
const char * const OGL_UINT16_FORMAT_STRINGS[4]				= { "GL_LUMINANCE16",				"GL_LUMINANCE16_ALPHA16",				"GL_RGB16",					"GL_RGBA16"					};
const char * const OGL_EXTERNAL_FORMAT_BGRA_STRINGS[4]	= { "GL_LUMINANCE",					"GL_LUMINANCE_ALPHA",					"GL_RGB",					"GL_BGRA_EXT"				};
const char * const OGL_EXTERNAL_FORMAT_RGBA_STRINGS[4]	= { "GL_LUMINANCE",					"GL_LUMINANCE_ALPHA",					"GL_RGB",					"GL_RGBA"					};
const char * const UNSUPPORTED_FORMAT_STRINGS[4]			= { "",									"",											"",							""								};




// The enumeration name for an extension is always the extension string with "_df" concatenated.
enum OpenGLExtEnum
{
	// ARB extensions
	GL_ARB_transpose_matrix_df				= 0,				// Note:  When adding a new extension after Fusion5 launch, 
	GL_ARB_vertex_program_df				= 1,				//        do not change the numbers of existing extensions
	GL_ARB_fragment_program_df				= 2,
	GL_ARB_vertex_buffer_object_df		= 3,
	GL_ARB_depth_texture_df					= 4,
	GL_ARB_shadow_df							= 5,
	GL_ARB_occlusion_query_df				= 6,
	GL_ARB_shader_objects_df				= 7,
	GL_ARB_vertex_shader_df					= 8,
	GL_ARB_fragment_shader_df				= 9,
	GL_ARB_shading_language_100_df		= 10,
	GL_ARB_texture_non_power_of_two_df	= 11,
	GL_ARB_point_sprite_df					= 12,
	GL_ARB_fragment_program_shadow_df	= 13, 
	GL_ARB_draw_buffers_df					= 14,
	GL_ARB_texture_rectangle_df			= 15,
	GL_ARB_pixel_buffer_object_df			= 16,				// there's also an EXT version
	GL_ARB_multisample_df					= 38,
	GL_ARB_texture_cube_map_df				= 45,
	GL_ARB_texture_float_df					= 47,
	GL_ARB_half_float_pixel_df				= 49,
	GL_ARB_framebuffer_object_df			= 51,

	// EXT extensions
	GL_EXT_pixel_buffer_object_df			= 17,
	GL_EXT_blend_equation_separate_df	= 18,
	GL_EXT_depth_bounds_test_df			= 19,
	GL_EXT_shadow_funcs_df					= 20,
	GL_EXT_texture_rectangle_df			= 21,
	GL_EXT_framebuffer_object_df			= 42,
	GL_EXT_framebuffer_blit_df				= 43,
	GL_EXT_framebuffer_multisample_df	= 44,
	GL_EXT_texture3D_df	               = 46,
	GL_EXT_packed_float_df              = 48,
	GL_EXT_blend_func_separate_df		   = 57,

	// WGL extensions
	WGL_ARB_pbuffer_df						= 22,
	WGL_ARB_render_texture_df				= 23,
	WGL_ARB_pixel_format_df					= 24,
	WGL_ARB_buffer_region_df				= 25,
	WGL_ARB_multisample_df					= 39,
	WGL_EXT_swap_control_df					= 40,
	WGL_NV_float_buffer_df					= 26,
	WGL_NV_render_texture_rectangle_df	= 27,
	WGL_NV_render_depth_texture_df		= 28, 
	WGL_ATI_pixel_format_float_df			= 37,
	WGL_ATI_render_texture_rectangle_df	= 41,

	// GLX extensions

	// NV extensions
	GL_NV_fragment_program_df				= 29,
	GL_NV_texture_rectangle_df				= 30,
	GL_NV_float_buffer_df					= 31,
	GL_NV_half_float_df						= 32,
	GL_NV_pixel_data_range_df				= 33,
	GL_NV_primitive_restart_df				= 34, 
	GL_NV_register_combiners_df			= 35,
	GL_NV_gpu_program4_df					= 52,
	GL_NV_fragment_program3_df				= 53,
	GL_NV_vertex_program3_df				= 54,
	GL_NV_fragment_program2_df				= 55,
	GL_NV_vertex_program2_df				= 56,
	GL_NV_present_video_df					= 58,

	// ATI extensions
	GL_ATI_texture_float_df					= 36,
	GL_ATI_draw_buffers_df					= 50,


	// Update this when a new extension is added
	NumOpenGLExtEnumEntries             = 59,
	NullOpenGLExt								= (OpenGLExtEnum) -1,
};



// NOTE:  Why do we have a fuGenTextures?  Originally there were two reasons:
// 1) glGenTextures() is not thread safe.  Even if you wrap it by synchronization locks it is still not thread safe.  It is possible for one thread
//    to call glGenTextures() and then another call it and get exactly the same values back.  The name only becomes reserved in GL when it becomes
//    a texture object, ie when you call glBindTexture() on it, up until that it is just an identifier.  Perhaps this was driver specific problem
//    or just in general the way GL is supposed to be.
// 2) The original fuGenTextures() also had debugging code to help detect double texture deletion and deletion of non-textures.

// Thread safe texture/buffer/list/framebuffer name generation:
FuSYSTEM_API extern void fuGenTextures(int n, GLuint *textures);								// replaces glGenTextures()
FuSYSTEM_API extern void fuDeleteTextures(int n, const GLuint *textures);					// replaces glDeleteTextures()

FuSYSTEM_API extern void fuGenBuffersARB(int n, GLuint *buffers);								// replaces glGenBuffersARB()
FuSYSTEM_API extern void fuDeleteBuffersARB(int n, const GLuint *buffers);					// replaces glDeleteBuffersARB()

FuSYSTEM_API extern GLuint fuGenList();																// kind of replaces glGenLists()
FuSYSTEM_API extern void fuDeleteList(GLuint list);												// kind of replaces glDeleteLists()

FuSYSTEM_API extern void fuGenFramebuffersEXT(int n, GLuint *framebuffers);				// replaces glGenFramebuffersEXT()
FuSYSTEM_API extern void fuDeleteFramebuffersEXT(int n, const GLuint *framebuffers);	// replaces glDeleteFramebuffersEXT()

FuSYSTEM_API extern void fuGenRenderbuffersEXT(int n, GLuint *renderbuffers);				// replaces glGenRenderbuffersEXT()
FuSYSTEM_API extern void fuDeleteRenderbuffersEXT(int n, const GLuint *renderbuffers);	// replaces glDeleteRenderbuffersEXT()

FuSYSTEM_API extern void fuGenProgramsARB(int n, GLuint *programs);							// replaces glGenProgramsARB()
FuSYSTEM_API extern void fuDeleteProgramsARB(int n, const GLuint *programs);				// replaces glDeleteProgramsARB()

FuSYSTEM_API extern LockableObject &DebugGetTextureNameLock();

#ifdef _DEBUG
FuSYSTEM_API extern BOOL fuMakeCurrent(HDC dc, HGLRC rc, const char *debugString = NULL);
#else
inline FuSYSTEM_API BOOL fuMakeCurrent(HDC dc, HGLRC rc, const char *debugString = NULL)
{
	return wglMakeCurrent(dc, rc);
}
#endif




// Tests support of a particular OpenGL extension.  This function is fast (just a table lookup), all the extension 
// string searching is done at startup.
FuSYSTEM_API bool ExtSupported(OpenGLExtEnum ext);

// Gets the GL extension string associated with this extension (eg. GetExtensionString(GL_ARB_multisample_df) = "GL_ARB_multisample")
FuSYSTEM_API const char *GetExtString(OpenGLExtEnum ext);

// Tests support for any of GL_NV_texture_rectangle, GL_EXT_texture_rectangle, or GL_ARB_texture_rectangle since the texture target
// enums are the same:  GL_TEXTURE_RECTANGLE_NV = GL_TEXTURE_RECTANGLE_EXT = GL_TEXTURE_RECTANGLE_ARB
FuSYSTEM_API extern bool TextureRectangleSupported;

// Checks support for all 4 GLSL extensions.
FuSYSTEM_API bool GLSLSupported();

// WGL_ARB_extensions_string
FuSYSTEM_API extern PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;

// WGL_ARB_pbuffer
FuSYSTEM_API extern PFNWGLCREATEPBUFFERARBPROC    wglCreatePbufferARB;
FuSYSTEM_API extern PFNWGLGETPBUFFERDCARBPROC     wglGetPbufferDCARB;	 
FuSYSTEM_API extern PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;
FuSYSTEM_API extern PFNWGLDESTROYPBUFFERARBPROC   wglDestroyPbufferARB;
FuSYSTEM_API extern PFNWGLQUERYPBUFFERARBPROC     wglQueryPbufferARB; 

// WGL_ARB_pixel_format
FuSYSTEM_API extern PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
FuSYSTEM_API extern PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB;
FuSYSTEM_API extern PFNWGLCHOOSEPIXELFORMATARBPROC      wglChoosePixelFormatARB;

// WGL_EXT_swap_control
FuSYSTEM_API extern PFNWGLSWAPINTERVALEXTPROC		wglSwapIntervalEXT;
FuSYSTEM_API extern PFNWGLGETSWAPINTERVALEXTPROC	wglGetSwapIntervalEXT;

// ARB_render_texture
FuSYSTEM_API extern PFNWGLBINDTEXIMAGEARBPROC		wglBindTexImageARB; 
FuSYSTEM_API extern PFNWGLRELEASETEXIMAGEARBPROC	wglReleaseTexImageARB;
FuSYSTEM_API extern PFNWGLSETPBUFFERATTRIBARBPROC	wglSetPbufferAttribARB;

// ARB_vertex_buffer_object (ARB_pixel_buffer_object uses these same entry points)
FuSYSTEM_API extern PFNGLBINDBUFFERARBPROC				glBindBufferARB;
FuSYSTEM_API extern PFNGLDELETEBUFFERSARBPROC			glDeleteBuffersARB;
FuSYSTEM_API extern PFNGLGENBUFFERSARBPROC				glGenBuffersARB;
FuSYSTEM_API extern PFNGLISBUFFERARBPROC					glIsBufferARB;
FuSYSTEM_API extern PFNGLBUFFERDATAARBPROC				glBufferDataARB;
FuSYSTEM_API extern PFNGLBUFFERSUBDATAARBPROC			glBufferSubDataARB;
FuSYSTEM_API extern PFNGLGETBUFFERSUBDATAARBPROC		glGetBufferSubDataARB;
FuSYSTEM_API extern PFNGLMAPBUFFERARBPROC					glMapBufferARB;
FuSYSTEM_API extern PFNGLUNMAPBUFFERARBPROC				glUnmapBufferARB;
FuSYSTEM_API extern PFNGLGETBUFFERPARAMETERIVARBPROC	glGetBufferParameterivARB;
FuSYSTEM_API extern PFNGLGETBUFFERPOINTERVARBPROC		glGetBufferPointervARB;

// ARB_vertex_program (ARB_fragment_program uses these same entry points)
FuSYSTEM_API extern PFNGLVERTEXATTRIB1DARBPROC	glVertexAttrib1dARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB1DVARBPROC	glVertexAttrib1dvARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB1FARBPROC	glVertexAttrib1fARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB1FVARBPROC	glVertexAttrib1fvARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB1SARBPROC	glVertexAttrib1sARB; 
FuSYSTEM_API extern PFNGLVERTEXATTRIB1SVARBPROC	glVertexAttrib1svARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB2DARBPROC	glVertexAttrib2dARB; 
FuSYSTEM_API extern PFNGLVERTEXATTRIB2DVARBPROC	glVertexAttrib2dvARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB2FARBPROC	glVertexAttrib2fARB; 
FuSYSTEM_API extern PFNGLVERTEXATTRIB2FVARBPROC	glVertexAttrib2fvARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB2SARBPROC	glVertexAttrib2sARB; 
FuSYSTEM_API extern PFNGLVERTEXATTRIB2SVARBPROC	glVertexAttrib2svARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB3DARBPROC	glVertexAttrib3dARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB3DVARBPROC	glVertexAttrib3dvARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB3FARBPROC	glVertexAttrib3fARB; 
FuSYSTEM_API extern PFNGLVERTEXATTRIB3FVARBPROC	glVertexAttrib3fvARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB3SARBPROC	glVertexAttrib3sARB; 
FuSYSTEM_API extern PFNGLVERTEXATTRIB3SVARBPROC	glVertexAttrib3svARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4NBVARBPROC	glVertexAttrib4NbvARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4NIVARBPROC	glVertexAttrib4NivARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4NSVARBPROC	glVertexAttrib4NsvARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4NUBARBPROC	glVertexAttrib4NubARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4NUBVARBPROC glVertexAttrib4NubvARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4NUIVARBPROC glVertexAttrib4NuivARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4NUSVARBPROC glVertexAttrib4NusvARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4BVARBPROC	glVertexAttrib4bvARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4DARBPROC	glVertexAttrib4dARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4DVARBPROC	glVertexAttrib4dvARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4FARBPROC	glVertexAttrib4fARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4FVARBPROC	glVertexAttrib4fvARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4IVARBPROC	glVertexAttrib4ivARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4SARBPROC	glVertexAttrib4sARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4SVARBPROC	glVertexAttrib4svARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4UBVARBPROC	glVertexAttrib4ubvARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4UIVARBPROC	glVertexAttrib4uivARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIB4USVARBPROC	glVertexAttrib4usvARB;
FuSYSTEM_API extern PFNGLVERTEXATTRIBPOINTERARBPROC		glVertexAttribPointerARB;
FuSYSTEM_API extern PFNGLENABLEVERTEXATTRIBARRAYARBPROC	glEnableVertexAttribArrayARB;
FuSYSTEM_API extern PFNGLDISABLEVERTEXATTRIBARRAYARBPROC	glDisableVertexAttribArrayARB;
FuSYSTEM_API extern PFNGLPROGRAMSTRINGARBPROC				glProgramStringARB;
FuSYSTEM_API extern PFNGLBINDPROGRAMARBPROC					glBindProgramARB;
FuSYSTEM_API extern PFNGLDELETEPROGRAMSARBPROC				glDeleteProgramsARB;
FuSYSTEM_API extern PFNGLGENPROGRAMSARBPROC					glGenProgramsARB;
FuSYSTEM_API extern PFNGLPROGRAMENVPARAMETER4DARBPROC			glProgramEnvParameter4dARB;
FuSYSTEM_API extern PFNGLPROGRAMENVPARAMETER4DVARBPROC		glProgramEnvParameter4dvARB;
FuSYSTEM_API extern PFNGLPROGRAMENVPARAMETER4FARBPROC			glProgramEnvParameter4fARB;
FuSYSTEM_API extern PFNGLPROGRAMENVPARAMETER4FVARBPROC		glProgramEnvParameter4fvARB;
FuSYSTEM_API extern PFNGLPROGRAMLOCALPARAMETER4DARBPROC		glProgramLocalParameter4dARB;
FuSYSTEM_API extern PFNGLPROGRAMLOCALPARAMETER4DVARBPROC		glProgramLocalParameter4dvARB;
FuSYSTEM_API extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC		glProgramLocalParameter4fARB;
FuSYSTEM_API extern PFNGLPROGRAMLOCALPARAMETER4FVARBPROC		glProgramLocalParameter4fvARB;
FuSYSTEM_API extern PFNGLGETPROGRAMENVPARAMETERDVARBPROC		glGetProgramEnvParameterdvARB;
FuSYSTEM_API extern PFNGLGETPROGRAMENVPARAMETERFVARBPROC		glGetProgramEnvParameterfvARB;
FuSYSTEM_API extern PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC	glGetProgramLocalParameterdvARB;
FuSYSTEM_API extern PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC	glGetProgramLocalParameterfvARB;
FuSYSTEM_API extern PFNGLGETPROGRAMIVARBPROC						glGetProgramivARB;
FuSYSTEM_API extern PFNGLGETPROGRAMSTRINGARBPROC				glGetProgramStringARB;
FuSYSTEM_API extern PFNGLGETVERTEXATTRIBDVARBPROC				glGetVertexAttribdvARB;
FuSYSTEM_API extern PFNGLGETVERTEXATTRIBFVARBPROC				glGetVertexAttribfvARB;
FuSYSTEM_API extern PFNGLGETVERTEXATTRIBIVARBPROC				glGetVertexAttribivARB;
FuSYSTEM_API extern PFNGLGETVERTEXATTRIBPOINTERVARBPROC		glGetVertexAttribPointervARB;
FuSYSTEM_API extern PFNGLISPROGRAMARBPROC							glIsProgramARB;

// multitexture
FuSYSTEM_API extern PFNGLACTIVETEXTUREPROC				glActiveTextureARB;
FuSYSTEM_API extern PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB;
FuSYSTEM_API extern PFNGLMULTITEXCOORD2IARBPROC			glMultiTexCoord2iARB;
FuSYSTEM_API extern PFNGLMULTITEXCOORD3FVARBPROC		glMultiTexCoord3fvARB;
FuSYSTEM_API extern PFNGLMULTITEXCOORD4FVARBPROC		glMultiTexCoord4fvARB;
FuSYSTEM_API extern PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB;

// GL_ARB_transpose_matrix
FuSYSTEM_API extern PFNGLLOADTRANSPOSEMATRIXFPROC glLoadTransposeMatrixfARB;
FuSYSTEM_API extern PFNGLLOADTRANSPOSEMATRIXDPROC glLoadTransposeMatrixdARB;
FuSYSTEM_API extern PFNGLMULTTRANSPOSEMATRIXFPROC glMultTransposeMatrixfARB;
FuSYSTEM_API extern PFNGLMULTTRANSPOSEMATRIXDPROC glMultTransposeMatrixdARB;

// GL_ARB_multisample
FuSYSTEM_API extern PFNGLSAMPLECOVERAGEARBPROC glSampleCoverageARB;

// WGL_ARB_buffer_region
FuSYSTEM_API extern PFNWGLCREATEBUFFERREGIONARBPROC	wglCreateBufferRegionARB;
FuSYSTEM_API extern PFNWGLDELETEBUFFERREGIONARBPROC	wglDeleteBufferRegionARB;
FuSYSTEM_API extern PFNWGLSAVEBUFFERREGIONARBPROC		wglSaveBufferRegionARB;
FuSYSTEM_API extern PFNWGLRESTOREBUFFERREGIONARBPROC	wglRestoreBufferRegionARB;

// GL_ARB_shader_objects
FuSYSTEM_API extern PFNGLDELETEOBJECTARBPROC			glDeleteObjectARB;
FuSYSTEM_API extern PFNGLGETHANDLEARBPROC				glGetHandleARB;
FuSYSTEM_API extern PFNGLDETACHOBJECTARBPROC			glDetachObjectARB;
FuSYSTEM_API extern PFNGLCREATESHADEROBJECTARBPROC	glCreateShaderObjectARB;
FuSYSTEM_API extern PFNGLSHADERSOURCEARBPROC			glShaderSourceARB;
FuSYSTEM_API extern PFNGLCOMPILESHADERARBPROC		glCompileShaderARB;
FuSYSTEM_API extern PFNGLCREATEPROGRAMOBJECTARBPROC	glCreateProgramObjectARB;
FuSYSTEM_API extern PFNGLATTACHOBJECTARBPROC			glAttachObjectARB;
FuSYSTEM_API extern PFNGLLINKPROGRAMARBPROC			glLinkProgramARB;
FuSYSTEM_API extern PFNGLUSEPROGRAMOBJECTARBPROC	glUseProgramObjectARB;
FuSYSTEM_API extern PFNGLVALIDATEPROGRAMARBPROC		glValidateProgramARB;
FuSYSTEM_API extern PFNGLUNIFORM1FARBPROC				glUniform1fARB;
FuSYSTEM_API extern PFNGLUNIFORM2FARBPROC				glUniform2fARB;
FuSYSTEM_API extern PFNGLUNIFORM3FARBPROC				glUniform3fARB;
FuSYSTEM_API extern PFNGLUNIFORM4FARBPROC				glUniform4fARB;
FuSYSTEM_API extern PFNGLUNIFORM1IARBPROC				glUniform1iARB;
FuSYSTEM_API extern PFNGLUNIFORM2IARBPROC				glUniform2iARB;
FuSYSTEM_API extern PFNGLUNIFORM3IARBPROC				glUniform3iARB;
FuSYSTEM_API extern PFNGLUNIFORM4IARBPROC				glUniform4iARB;
FuSYSTEM_API extern PFNGLUNIFORM1FVARBPROC			glUniform1fvARB;
FuSYSTEM_API extern PFNGLUNIFORM2FVARBPROC			glUniform2fvARB;
FuSYSTEM_API extern PFNGLUNIFORM3FVARBPROC			glUniform3fvARB;
FuSYSTEM_API extern PFNGLUNIFORM4FVARBPROC			glUniform4fvARB;
FuSYSTEM_API extern PFNGLUNIFORM1IVARBPROC			glUniform1ivARB;
FuSYSTEM_API extern PFNGLUNIFORM2IVARBPROC			glUniform2ivARB;
FuSYSTEM_API extern PFNGLUNIFORM3IVARBPROC			glUniform3ivARB;
FuSYSTEM_API extern PFNGLUNIFORM4IVARBPROC			glUniform4ivARB;
FuSYSTEM_API extern PFNGLUNIFORMMATRIX2FVARBPROC	glUniformMatrix2fvARB;
FuSYSTEM_API extern PFNGLUNIFORMMATRIX3FVARBPROC	glUniformMatrix3fvARB;
FuSYSTEM_API extern PFNGLUNIFORMMATRIX4FVARBPROC	glUniformMatrix4fvARB;
FuSYSTEM_API extern PFNGLGETOBJECTPARAMETERFVARBPROC glGetObjectParameterfvARB;
FuSYSTEM_API extern PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
FuSYSTEM_API extern PFNGLGETINFOLOGARBPROC			glGetInfoLogARB;
FuSYSTEM_API extern PFNGLGETATTACHEDOBJECTSARBPROC	glGetAttachedObjectsARB;
FuSYSTEM_API extern PFNGLGETUNIFORMLOCATIONARBPROC	glGetUniformLocationARB;
FuSYSTEM_API extern PFNGLGETACTIVEUNIFORMARBPROC	glGetActiveUniformARB;
FuSYSTEM_API extern PFNGLGETUNIFORMFVARBPROC			glGetUniformfvARB;
FuSYSTEM_API extern PFNGLGETUNIFORMIVARBPROC			glGetUniformivARB;
FuSYSTEM_API extern PFNGLGETSHADERSOURCEARBPROC		glGetShaderSourceARB;

// GL_ARB_vertex_shader
FuSYSTEM_API extern PFNGLBINDATTRIBLOCATIONARBPROC	glBindAttribLocationARB;
FuSYSTEM_API extern PFNGLGETACTIVEATTRIBARBPROC		glGetActiveAttribARB;
FuSYSTEM_API extern PFNGLGETATTRIBLOCATIONARBPROC	glGetAttribLocationARB;

// GL_ARB_draw_buffers
FuSYSTEM_API extern PFNGLDRAWBUFFERSARBPROC glDrawBuffersARB;

// GL_ARB_framebuffer_object
FuSYSTEM_API extern PFNGLISRENDERBUFFERPROC								glIsRenderbuffer;
FuSYSTEM_API extern PFNGLBINDRENDERBUFFERPROC							glBindRenderbuffer;
FuSYSTEM_API extern PFNGLDELETERENDERBUFFERSPROC						glDeleteRenderbuffers;
FuSYSTEM_API extern PFNGLGENRENDERBUFFERSPROC							glGenRenderbuffers;
FuSYSTEM_API extern PFNGLRENDERBUFFERSTORAGEPROC						glRenderbufferStorage;
FuSYSTEM_API extern PFNGLGETRENDERBUFFERPARAMETERIVPROC				glGetRenderbufferParameteriv;
FuSYSTEM_API extern PFNGLISFRAMEBUFFERPROC								glIsFramebuffer;
FuSYSTEM_API extern PFNGLBINDFRAMEBUFFERPROC								glBindFramebuffer;
FuSYSTEM_API extern PFNGLDELETEFRAMEBUFFERSPROC							glDeleteFramebuffers;
FuSYSTEM_API extern PFNGLGENFRAMEBUFFERSPROC								glGenFramebuffers;
FuSYSTEM_API extern PFNGLCHECKFRAMEBUFFERSTATUSPROC					glCheckFramebufferStatus;
FuSYSTEM_API extern PFNGLFRAMEBUFFERTEXTURE1DPROC						glFramebufferTexture1D;
FuSYSTEM_API extern PFNGLFRAMEBUFFERTEXTURE2DPROC						glFramebufferTexture2D;
FuSYSTEM_API extern PFNGLFRAMEBUFFERTEXTURE3DPROC						glFramebufferTexture3D;
FuSYSTEM_API extern PFNGLFRAMEBUFFERRENDERBUFFERPROC					glFramebufferRenderbuffer;
FuSYSTEM_API extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC	glGetFramebufferAttachmentParameteriv;
FuSYSTEM_API extern PFNGLGENERATEMIPMAPPROC								glGenerateMipmap;
FuSYSTEM_API extern PFNGLBLITFRAMEBUFFERPROC								glBlitFramebuffer;
FuSYSTEM_API extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC			glRenderbufferStorageMultisample;
FuSYSTEM_API extern PFNGLFRAMEBUFFERTEXTURELAYERPROC					glFramebufferTextureLayer;

// GL_EXT_framebuffer_object
FuSYSTEM_API extern PFNGLISRENDERBUFFEREXTPROC					glIsRenderbufferEXT;
FuSYSTEM_API extern PFNGLBINDRENDERBUFFEREXTPROC				glBindRenderbufferEXT;
FuSYSTEM_API extern PFNGLDELETERENDERBUFFERSEXTPROC			glDeleteRenderbuffersEXT;
FuSYSTEM_API extern PFNGLGENRENDERBUFFERSEXTPROC				glGenRenderbuffersEXT;
FuSYSTEM_API extern PFNGLRENDERBUFFERSTORAGEEXTPROC			glRenderbufferStorageEXT;
FuSYSTEM_API extern PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC	glGetRenderbufferParameterivEXT;
FuSYSTEM_API extern PFNGLISFRAMEBUFFEREXTPROC					glIsFramebufferEXT; 
FuSYSTEM_API extern PFNGLBINDFRAMEBUFFEREXTPROC					glBindFramebufferEXT;
FuSYSTEM_API extern PFNGLDELETEFRAMEBUFFERSEXTPROC				glDeleteFramebuffersEXT;
FuSYSTEM_API extern PFNGLGENFRAMEBUFFERSEXTPROC					glGenFramebuffersEXT;
FuSYSTEM_API extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC		glCheckFramebufferStatusEXT;
FuSYSTEM_API extern PFNGLFRAMEBUFFERTEXTURE1DEXTPROC			glFramebufferTexture1DEXT;
FuSYSTEM_API extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC			glFramebufferTexture2DEXT;
FuSYSTEM_API extern PFNGLFRAMEBUFFERTEXTURE3DEXTPROC			glFramebufferTexture3DEXT;
FuSYSTEM_API extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC		glFramebufferRenderbufferEXT;
FuSYSTEM_API extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT;
FuSYSTEM_API extern PFNGLGENERATEMIPMAPEXTPROC					glGenerateMipmapEXT;

// GL_EXT_framebuffer_blit
FuSYSTEM_API extern PFNGLBLITFRAMEBUFFEREXTPROC glBlitFramebufferEXT;

// GL_EXT_framebuffer_multisample
FuSYSTEM_API extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glRenderbufferStorageMultisampleEXT;

// GL_NV_register_combiners
FuSYSTEM_API extern PFNGLCOMBINERPARAMETERFVNVPROC					glCombinerParameterfvNV;
FuSYSTEM_API extern PFNGLCOMBINERPARAMETERFNVPROC					glCombinerParameterfNV;
FuSYSTEM_API extern PFNGLCOMBINERPARAMETERIVNVPROC					glCombinerParameterivNV;
FuSYSTEM_API extern PFNGLCOMBINERPARAMETERINVPROC					glCombinerParameteriNV;
FuSYSTEM_API extern PFNGLCOMBINERINPUTNVPROC							glCombinerInputNV;
FuSYSTEM_API extern PFNGLCOMBINEROUTPUTNVPROC						glCombinerOutputNV;
FuSYSTEM_API extern PFNGLFINALCOMBINERINPUTNVPROC					glFinalCombinerInputNV;
FuSYSTEM_API extern PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC		glGetCombinerInputParameterfvNV;
FuSYSTEM_API extern PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC		glGetCombinerInputParameterivNV;
FuSYSTEM_API extern PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC		glGetCombinerOutputParameterfvNV;
FuSYSTEM_API extern PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC		glGetCombinerOutputParameterivNV;
FuSYSTEM_API extern PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC	glGetFinalCombinerInputParameterfvNV;
FuSYSTEM_API extern PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC	glGetFinalCombinerInputParameterivNV;

// GL_NV_present_video
FuSYSTEM_API extern PFNGLPRESENTFRAMEKEYEDNVPROC		glPresentFrameKeyedNV;
FuSYSTEM_API extern PFNGLPRESENTFRAMEDUALFILLNVPROC	glPresentFrameDualFillNV;
FuSYSTEM_API extern PFNGLGETVIDEOIVNVPROC					glGetVideoivNV;
FuSYSTEM_API extern PFNGLGETVIDEOUIVNVPROC				glGetVideouivNV;
FuSYSTEM_API extern PFNGLGETVIDEOI64VNVPROC				glGetVideoi64vNV;
FuSYSTEM_API extern PFNGLGETVIDEOUI64VNVPROC				glGetVideoui64vNV;
FuSYSTEM_API extern PFNWGLENUMERATEVIDEODEVICESNVPROC wglEnumerateVideoDevicesNV;
FuSYSTEM_API extern PFNWGLBINDVIDEODEVICENVPROC			wglBindVideoDeviceNV;
FuSYSTEM_API extern PFNWGLQUERYCURRENTCONTEXTNVPROC	wglQueryCurrentContextNV;

// GL_EXT TexImage3D
FuSYSTEM_API extern PFNGLTEXIMAGE3DEXTPROC			glTexImage3DEXT;
FuSYSTEM_API extern PFNGLTEXSUBIMAGE3DEXTPROC		glTexSubImage3DEXT;
FuSYSTEM_API extern PFNGLCOPYTEXSUBIMAGE3DEXTPROC	glCopyTexSubImage3DEXT;

// TexImage3D - Core 1.2 GL I think - no extension to check?
#ifndef PFNGLTEXIMAGE3DPROC
typedef void (APIENTRYP PFNGLTEXIMAGE3DPROC) (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (APIENTRYP PFNGLTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (APIENTRYP PFNGLCOPYTEXSUBIMAGE3DPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
#endif

FuSYSTEM_API extern PFNGLTEXIMAGE3DPROC			glTexImage3D;
FuSYSTEM_API extern PFNGLTEXSUBIMAGE3DPROC		glTexSubImage3D;
FuSYSTEM_API extern PFNGLCOPYTEXSUBIMAGE3DPROC	glCopyTexSubImage3D;

// GL_EXT_blend_func_separate
FuSYSTEM_API extern PFNGLBLENDFUNCSEPARATEEXTPROC glBlendFuncSeparateEXT;




#endif