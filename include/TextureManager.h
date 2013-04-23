#ifndef _TEXTURE_MANAGER_H_
#define _TEXTURE_MANAGER_H_

#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include "GLTools.h"

#include "3D_DataTypes.h"
#include "3D_GLContext.h"
#include "3D_CgWrapper.h"
#include "3D_ShaderARB.h"

class CRect;
class Pbuffer;
class Hashmap;
class Resource3D;
class DxDiagInfo;
class GLTexture2D;
class TexResource;
class ShaderKeyCg;
class ShaderCacheCg3D;

const int SHADER_PATHMAP_LEN = 8;
const char * const SHADER_PATHMAP = "Shaders:";
const char * const DEFAULT_SHADER_PATHMAP = "Fusion:Shaders";  /* "Fusion:Shaders\\" */
const char * const SHADER_DLL_FILENAME = "Fusion:eyeonShaders.dll";






//
//
// class ResourceManager3D:
//   manages OpenGL objects that inherit from Resource3D including textures, vertex buffers, shaders and pbuffers.
//
//

class FuSYSTEM_API ResourceManager3D : public LockableObject
{
	FuDeclareClass(ResourceManager3D, LockableObject);

public:
	// The texture manager maintains a context that various internals of Fusion grab a hold of now and then
	// to do texture uploading/downloading and stuff.
	GLContext3D *Context;
	DxDiagInfo *DDI;

	bool IsInitialized;

	//uint32 MaxTextureSize;							// for rectangles if rectangles are supported, for 2D otherwise
	uint32 MaxTextureSize2D;							// for standard pow2 and GL_ARB_texture_non_power_of_two
	uint32 MaxTextureSizeRECT;							// for GL_ARB_texture_rectangle

	bool AllowVBOs;										// determines whether VBOs should be allowed while rendering (if VBOs are supported and FUSION_USE_VBO is true)

	//ShaderResource *ShaderList;
	//Resource *GenericList;

	LONG TotalCardMemory;								// the total amount of memory on the graphics card GPU in kilobytes - eg. 128MB, 256MB cards

	LONG DetectedCardMemory;							// use TotalCardMemory, this is the amount of memory detected by Fusion in kilobytes (this could be wrong, eg. some cards like to report RAM as part of their memory on Vista)
	LONG DetectedCardMemoryDX7;						// amount returned by DX7 (-1 if not set)
	LONG DetectedCardMemoryDxDiag;					// amount returned by the DxDiag API (-1 if not set)
	LONG DetectedCardMemoryNvidia;					// amount returned by Nvidia's API (-1 if not set)
	LONG DetectedCardMemoryATI;						// amount returned by ATI's API (-1 if not set)

	GLuint RectTarget;									// default texture target for rectangular textures (could be GL_TEXTURE_RECTANGLE_ARB or GL_TEXTURE_2D for GL_ARB_texture_non_power_of_two)  (Note: this must never be changed mid-runtime unless the shader cache has been flushed first)
	OpenGLExtEnum TextureRectExt;
	OpenGLExtEnum ForceTextureRectExt;				// -1 if unset

	bool TextureSupport[F3D_NUM_DATATYPES];		// RGBA texturing support - test BilinearSupport separately to see if float filtering supported
	bool BilinearSupport[F3D_NUM_DATATYPES];		// RGBA bilinear texture filtering support - note RGBA float filtering doesn't guarentee RGB/LA/L support
	bool BlendSupport[F3D_NUM_DATATYPES];			// RGBA framebuffer blending support - be aware that this was determined using either FBO or pbuffers depending what OffscreenBufferExt was set to

	const GLuint *Int8Format;							// internal texture formats
	const GLuint *Int16Format;							//   * formats are for 3D views and renderer - 2D views do their own thing and don't use these
	const GLuint *Flt16Format;							//   * formats can be NULL if not supported
	const GLuint *Flt32Format;							//   * array of 4 elements for A, LA, RGB, RGBA 

	const GLuint *TexFormat[F3D_NUM_DATATYPES];	// eg. TexFormat[F3D_UINT8] = Int8Format

	const char **Int8FormatString;
	const char **Int16FormatString;
	const char **Flt16FormatString;					// will be "" if not supported
	const char **Flt32FormatString;

	OpenGLExtEnum OffscreenBufferExt;
	OpenGLExtEnum ForceOffscreenBufferExt;			// -1 if unset

	int MaxOffscreenBufferSize;						// maximum width/height of offscreen buffers regardless of datatype
	int MaxOffscreenBufferPixels[F3D_NUM_DATATYPES];	// limit maximum number of pixels in a framebuffer
	int TileSize[F3D_NUM_DATATYPES];					// tile size used when rendering with GL renderer

	int IsCgSetup;											// can be queried anytime without a lock (reads of int are atomic)

	CGprogram FuInterface;

	ShaderCg *FuPassthroughF;
	ShaderCg *FuMainV;
	ShaderCg *FuZOnlyV;
	ShaderCg *FuWhiteF;
	ShaderCg *FuBlackF;
	ShaderCg *FuShowMatteObjectsF;

	ParamCg FuMainV_ModelToEye;
	ParamCg FuMainV_ModelToEyeIT;
	ParamCg FuMainV_ModelToNdc;
	ParamCg FuMainV_TwoSidedLighting;

	ParamCg FuZOnlyV_ModelToNdc;

	ParamCg FuMainV_Stream[HW_NumStreams];			// do not use anymore, just pass directly using gl vertex array funcs

	ShaderString3D FuInterfacesString;
	ShaderString3D FuMainString;
	ShaderString3D FuVertexMainString;

public:
	ResourceManager3D();
	ResourceManager3D &operator=(const ResourceManager3D &rhs);					// nyi
	ResourceManager3D(const ResourceManager3D &rhs);								// nyi
	~ResourceManager3D();

	bool Init();
	bool Shutdown();

	// The returned Cg context may only be used between a ObtainCgContext / ReleaseCgContext pair.  Right now there is only
	// one CgContext that is shared by everyone.  That means only one thread may be doing Cg stuff at a time.  If one GL renderer
	// calls GetSource() on an input which leads to an upstream GL render, then we need to pass the context somehow upstream in the request.
	ContextCg *ObtainCgContext();
	void ReleaseCgContext(ContextCg *context);

	bool ObtainContext()  { return Context->Obtain(); }
	void ReleaseContext() { Context->Release(); }

	int GetMaxFramebufferSize(F3D_DataType framebufferDataType);

	//uint16 DeclareStreamType(const char *name, coordspace, transformtype, user, flags = );

	// helpful functions
	static bool GetDataType(Image *img, GLenum &dataType, GLenum &format);
	static uint8 *GetImageDataBGRA8(Image *img, ChannelType chan = CHAN_COLOR, CRect *srect = NULL);



	//-------------------
	// Memory Management
	//-------------------
	uint32 FreeZombies();								// must have a GL context to call this, returns the amount of memory freed in kbytes

	void DetectCardMemory();
	
	void SetTotalCardMemory(uint32 kbytes);		// Use to change memory after initialization
	bool AttemptToFree(uint32 kilobytes);			// Attempts to free the requested amount of gpu memory

	// If there is no current context these funcs delay the freeing of the object until a context is made current.  
	// This avoids expensive context activations.
	void FreeVBO(GLuint vbo, int32 kbytes);		//  otherwise it should call this and not call ResourceDestroyed()
	void FreeList(GLuint list);						// safe to call when you don't have a context



	//-----------
	// Callbacks
	//-----------
	virtual bool RegisterResourceTagList(Resource3D *res, uint32 kbytes, TagList &tags);
	virtual void UnregisterResourceTagList(Resource3D *res, TagList &tags);

	// Most resources automatically register/unregister themselves with resource manager.  If you manually unregister a resource, then you are 
	// responsible for calling FreeData() on it.  It is assumed that no other threads will be modifying the resource for the duration
	// of the RegisterResource/UnregisterResource() call.
	bool RegisterResource(Resource3D *res, uint32 kbytes = 0, Tag firstTag = _TAG_DONE, ...);
	void UnregisterResource(Resource3D *res, Tag firstTag = _TAG_DONE, ...);

	// This are callbacks used by the resources to inform the manager when they've changed state.
	void ResourcePreRendered(Resource3D *res);	// PreRender()/PostRender() bracket all the Activate()/Deactivate() calls during a render
	void ResourceActivated(Resource3D *res);		// Activate()/Deactivate() bracket times where the resource *must* be valid
	void ResourceDeactivated(Resource3D *res);
	void ResourcePostRendered(Resource3D *res);

	// The memSize passes into PreAlloc() should match the size in PostAlloc(), unless the allocation failed, in which case the second memSize should be 0.
	void ResourcePreAlloc(Resource3D *res, uint32 kbytes);	// Call before allocing a GPU resource to allow the manager to free up enough memory.
	void ResourcePostAlloc(Resource3D *res, uint32 kbytes);	// Call after allocing a GPU resource to let the manager know the resource is now using memory

	void ResourceDestroyed(Resource3D *res);		// If a resource frees its own GPU data, it should call this



	//----------------------------------------
	// Shader strings - caching and loading
	//  if key is NULL it defaults tofilename
	//----------------------------------------
	ShaderString3D LoadShaderString(const char *filename, const char *key = NULL, TagList *tags = NULL);

	void CacheShaderString(ShaderString3D s, const char *key, TagList *tags = NULL);
	ShaderString3D GetCachedShaderString(const char *key, TagList *tags = NULL);		// prefer LoadShaderString() to this

	void EnableShaderStringCaching();
	void DisableShaderStringCaching();



	//------------------------------------------------------------------------------------------
	// Shaders - caching and loading
	//  @param filename 
	//  @param main     the name of the main() function in the shader and its also the default value for RSH_ShaderClassID
	//  @param key      if NULL it defaults to filename (if the loading func doesn't take a filename then the shader will be uncached)
	//  @return         the returned shader should be recycled when you are done with it
	//------------------------------------------------------------------------------------------
	ShaderARB *LoadShaderARB(const char *filename, ShaderARB::ShaderType shaderType, const char *key = NULL, TagList *tags = NULL);
	ShaderARB *LoadShaderARBvp(const char *filename, const char *key = NULL, TagList *tags = NULL);
	ShaderARB *LoadShaderARBfp(const char *filename, const char *key = NULL, TagList *tags = NULL);
	ShaderCg *LoadShaderCgV(const char *filename, const char *main, const char *key = NULL, TagList *tags = NULL);
	ShaderCg *LoadShaderCgF(const char *filename, const char *main, const char *key = NULL, TagList *tags = NULL);
	ShaderCg *LoadShaderCg(const char *filename, const char *main, ShaderCg::ShaderType type, const char *key = NULL, TagList *tags = NULL);

	ShaderARB *CreateShaderARBvp(const char *shaderString, const char *key = NULL, TagList *tags = NULL);
	ShaderARB *CreateShaderARBfp(const char *shaderString, const char *key = NULL, TagList *tags = NULL);
	ShaderARB *CreateShaderARBvp(std::string &shaderString, const char *key = NULL, TagList *tags = NULL);
	ShaderARB *CreateShaderARBfp(std::string &shaderString, const char *key = NULL, TagList *tags = NULL);
	ShaderARB *CreateShaderARBvp(ShaderStream &shaderString, const char *key = NULL, TagList *tags = NULL);
	ShaderARB *CreateShaderARBfp(ShaderStream &shaderString, const char *key = NULL, TagList *tags = NULL);
	ShaderARB *CreateShaderARB(const char *shaderString, ShaderARB::ShaderType shaderType, const char *key = NULL, TagList *tags = NULL);

	void EnableShaderCaching();
	void DisableShaderCaching();

	void FreeAllShaders(TagList *tags = NULL);

	void CacheMaterialShader(ShaderCg *shader);				// CacheMaterialShader() will place a UseCount on shader.
	ShaderCg *GetCachedMaterialShader(ShaderKeyCg *key);	// When you are done with returned shader you must call ReleaseLock() and Recycle() on it

	void CacheShaderCg(ShaderCg *shader, const char *key);
	ShaderCg *GetCachedShaderCg(const char *key);

	void CacheShaderARB(ShaderARB *shader, const char *key);
	ShaderARB *GetCachedShaderARB(const char *key);

	// Pre-create some common shaders shared by different parts of the renderer
	bool BuildSharedARBShaders();
	void FreeSharedARBShaders();

	void SetupShaderPaths();							// called during initialization

	const char *GetShadersDirectory();				// path/filename for "Shaders:" pathmap, NULL if it doesn't exist
	const char *GetShaderDllFilename();				// path/filename of eyeonShaders.dll, NULL if it doesn't exist


	
	//--------------------
	// Log to file/stream
	//--------------------
	bool DumpOpenGLObjects(const char *pathfile, bool dumpTextures = true, bool dumpCgPrograms = true, bool dumpVBOs = true, bool dumpFBOs = true);
	float32 DumpTextures(FILE *fp);
	float32 DumpCgShaders(FILE *fp);
	float32 DumpVBOs(FILE *fp);
	float32 DumpFBOs(FILE *fp);
	

	bool DumpCgObjects(const char *pathfile);
	bool DumpDxDiagInfo(std::ostream &buffer);
	bool DumpTextureAndOffscreenBufferSettings(std::ostream &buffer);

protected:
	std::vector<TexResource*> TextureList;			// list of all allocated textures (they could be destroyed to reclaim memory, but not unregistered)
	std::vector<uint32> EmptyTextureSlots;

	std::vector<Pbuffer*> PbufferList;

	std::vector<Stream3D*> StreamList;				// all registered streams, not necessarily have valid gpu buffers
	std::vector<uint32> EmptyStreamSlots;			// empty slots in StreamList (to save a search through what could be a large array)

	std::vector<IndexArray3D*> IndexList;			// all registered index arrays, not necessarily have valid gpu buffers
	std::vector<uint32> EmptyIndexSlots;

	int32 ZombieMemory;									// in kilobytes, locked by StreamListLock
	std::vector<GLuint> ZombieVBO;					// gpu buffers that need to be freed next time we get a context (both index and stream)

	std::vector<GLuint> ZombieLists;					// display lsits that need to be freed next time we get a context

	int NumMaterialTypes;								// TODO - these are not accurate if fuse 3D material/light plugins are present
	int NumLightTypes;

	//std::vector<StreamType3D*> StreamTypes;		// all declared stream types

	GLuint NextList;
	std::vector<GLuint> FreeLists;

	float64 MaxMemPercentage;							// as a percentage of the TotalCardMemory - this number can be often set greater than 1.0 (=100%) because OpenGL virtualizes its memory.
	uint32 MaxMemUsage;

	volatile LONG TotalUsedMemory;					// TotalUsedMemory - the total amount of memory used by all objects in kilobytes

	uint32 TextureReleaseThreshhold;					// (kilobytes) small resources below a certain threshhold in size are treated preferentially
	uint32 ReleaseThreshhold;							// when picking which resources to free

	// Cg stuff
	ContextCg *CgContext;
	LockableObject CgContextLock;

	// These are some common preloaded shaders that are used at various points in renderer
	ShaderARB *fpFixed0T;								// a surface with no textures
	ShaderARB *fpFixed1T;								// a surface with one 2D texture
	ShaderARB *fpPeel;									// a shader used for depth peeling
	ShaderARB *fpPassThrough;							// the simplest of all possible shaders, it simply passes through the color

	bool ShaderCachingEnabled;
	Hashmap *ShaderARBCache;
	Hashmap *ShaderCgCache;								// these are just plain Cg programs
	ShaderCacheCg3D *MtlShaderCache;					// these are shaders composed of user parameters connected together

	bool ShaderStringCachingEnabled;
	Hashmap *ShaderStringCache;						// hashmap of all loaded shader strings

	bool ShadersDirectoryExists;						// if true, ShaderDirectory is valid
	char ShadersDirectory[_MAX_PATH];				// this is what Shaders: maps to

	bool ShaderDllExists;								// if true, ShaderDllFilename is valid
	char ShaderDllFilename[_MAX_PATH];				// path and filename for eyeonShaders.dll

	float64 TimerPeriod;

protected:
	bool InitCg();
	void ShutdownCg();

	float64 GetTime();

	uint32 FreeGPUMemory(Resource3D *res);
	bool CleanupOrphanedResources();

	void CalcFramebufferLimits();
	void CalcFramebufferLimits(F3D_DataType bufferDataType);

	uint32 DetectCardMemoryDX7();						// using GetAvailableVidMem()
	uint32 DetectCardMemoryDxDiag();					// using DxDiag COM interface
	uint32 DetectCardMemoryNvidia();					// kbytes
	uint32 DetectCardMemoryATI();						// returns 0 on failure

	void EnumerateLights();
	void EnumerateMaterials();

	void ChooseTextureRectExt();
	void ChooseOffscreenBufferExt();

	void DetermineTextureSupport();
	void DetermineBilinearSupport();
	void DetermineBlendSupport();

	bool DoBilinearTest(F3D_DataType dataType, int size);
	bool DoBlendingTest(F3D_DataType dataType);

	ShaderString3D LoadShaderStringFromShaderDirectory(const char *filename);	// from "Shaders:" = "c:\Projects\Shaders\" - for use on dev machines
	ShaderString3D LoadShaderStringFromDll(const char *filename);					// from eyeonShaders.dll - for use by end users
	ShaderString3D LoadShaderStringFromFile(const char *filename);					// from arbitrary disk file - for use by plugin developers

	bool LoadEyeonCgInterfaces();
	void FreeEyeonCgInterfaces();

	void FreeAllShaderStrings();
};



extern FuSYSTEM_API ResourceManager3D ResourceMgr;				// for now there is only one of these, but in the future we may have one per graphics card

typedef void (*FbxCacheFunc)();
extern FuSYSTEM_API FbxCacheFunc g_ReloadFbxMeshes;	// I tried storing a g_FbxCache pointer here but that required FuSystem to link against FBX libs
extern FuSYSTEM_API FbxCacheFunc g_PurgeFbxCache;

class FuSYSTEM_API AutoCgContext
{
public:
	ContextCg *Context;

public:
	AutoCgContext() 			{ Context = ResourceMgr.ObtainCgContext(); }
	~AutoCgContext()			{ if (Context) ResourceMgr.ReleaseCgContext(Context); }
	bool IsLocked()			{ return Context != NULL; }
	operator ContextCg *()	{ return Context; }
};









#endif
