#ifndef _3D_CG_WRAPPER_H_
#define _3D_CG_WRAPPER_H_

#include "VectorMath.h"
#include "LockableObject.h"

#include "3D_Resource.h"

// The purpose of this file is act as a wrapper around Cg stuff so it (along with GLTools.cpp) is
//  the only one who needs to include Cg headers and libraries.

// Note:  I use the term 'user parameter' to indicate a special kind of parameter.  A CGparameter can be many things:
//  -a handle to a vector with 2, 3, or 4 components
//  -a handle to a matrix
//  -a handle to an array of parameters or a handle to a component of that array
//  -a handle to a structure or a handle to one of that struct's data members
//  -a handle to an arg of a function
//  -etc.
// For the case when the parameter is a handle to a user defined struct we will call it a 'user' parameter.  In the Cg documentation these are
//   called 'named user' or 'user' parameters.  A user parameter is always a 'shared' parameter.
// For the case when the parameter is a handle to an arg of a function we will call it a 'program' parameter

class Plane3f;
class ParamCg;
class ShaderCg;
class ContextCg;
class FusionApp;
class ShadeNodeCg;
class MtlImplGL3D;
class LightImplGL3D;
class RenderContextGL3D;

struct _CGcontext;				// these are from Cg.h
struct _CGprogram;
struct _CGparameter;

typedef struct _CGcontext *CGcontext;
typedef struct _CGprogram *CGprogram;
typedef struct _CGparameter *CGparameter;

typedef void (__cdecl * CgErrorCallback)(void);

typedef void (__cdecl * CgErrorHandlerFunc)(CGcontext context, uint32 error, void *appdata);





//
//
// class ParamCg - A parameter is a handle to an object in the Cg code
//
//
class FuSYSTEM_API ParamCg : public Object
{
	FuDeclareClass(ParamCg, Object);

public:
	CGparameter Param;

public:
	ParamCg();
	ParamCg(CGparameter param);

	bool IsValid() { return Param != NULL; }
	
	void Set1fv(const float *v);
	void Set2fv(const float *v);
	void Set3fv(const float *v);
	void Set4fv(const float *v);

	void Set(const Matrix4f &m);
	void Set3x3(const Matrix4f &m);
	void Set3x4(const Matrix4f &m);					// takes top 3 rows
	void Set4x4fv(const float32 *m);

	void Set(bool b);										// sets the param to the float value (b ? 1.0f : 0.0f)
	void Set(int i);										// sets the param to the float value float32(i)
	void Set(float x);
	void Set(float x, float y);
	void Set(float x, float y, float z);
	void Set(float x, float y, float z, float w);

	void Set(const Vector2f &v);
	void Set(const Vector3f &v);
	void Set(const Vector4f &v);

	void Set2f(const Vector3f &v);					// (x, y)
	void Set2f(const Vector4f &v);					// (x, y)
	void Set3f(const Vector4f &v);					// (x, y, z)

	void Set(const Plane3f &p);

	GLuint GetTexture();
	void SetTexture(GLuint texObj);					// only works if this is a texture parameter, pass id of the OpenGL texture object

	void SetLiteral();

	void ConnectTo(ParamCg dest);						// Param is treated as the source and connected to dest
	void Disconnect();									// only works if Param is dest, not source
	void DisconnectAllSubParameters();				// only works if this is a struct param; disconnects any sub parameters

	ParamCg GetSubParameter(const char *name);	// only works if this is a user param, 'name' is the text string appearing in the .cg struct

	ParamCg GetArrayElement(int index);				// only works if this is an array param, gets a handle to the element array[index]

	void Destroy();										// all compound/struct params must be destroyed, calling this twice has no effect

	// for vertex array parameters
	void GLSetParameterPointer(int tupleSize, GLenum type, int stride, void *data);
	void GLEnableClientState();
	void GLDisableClientState();

	#ifndef USE_NOSCRIPTING
		DECLARE_SIMPLE_LUATYPE(ParamCg);
	#endif
};


#define ADD_MANAGED_PARAMCG(name)				AddManagedParam(name, #name)

class ManagedParamCg
{
public:
	ParamCg m_Param;
	const char *m_Name;
};



//
// class ShadeNodeCg:  Cg shaders are built up from collections of ShadeNodeCg objects.  Cg materials should subclass this and add data 
//   members for all their CGparameters.  For example, there might be a ShadeNodes for 'Blinn', 'Blend', 'SpotLight', 'Place2DTexture', etc.  
//   This class encapsulates the shared Cg parameter that does the shading for a material or light.  A ShadeNodeCg is always attached to 
//   a particular shader, you cannot share them between 2 shaders.
//
class FuSYSTEM_API ShadeNodeCg : public Object
{
	FuDeclareClass(ShadeNodeCg, Object);

public:
	ParamCg m_Struct;								// this is the Cg handle for the material/light structure
	uint32 ID;										// a unique ID representing the mtl type
	uint32 NodeIndex;
	const char *StateString;
	std::vector<ManagedParamCg> m_ManagedParamList;

public:
	ShadeNodeCg();
	ShadeNodeCg(const ShadeNodeCg &toCopy);// nyi
	virtual ~ShadeNodeCg();						// calls Free()

	void SetLiteral();
	virtual void SetMaterial(MtlImplGL3D *mtl);	// sets the material this shadenode is associated with

	virtual void AddManagedParam(ParamCg &param, const char *name);
	
	virtual void Free();							// subclasses must override this to clean up any parameters they allocate (minimally free the NodeParam)

	void ConnectOutputTo(ParamCg dest);		// connects Param to dest

	#ifndef USE_NOSCRIPTING
		DECLARE_SIMPLE_LUATYPE(ShadeNodeCg);
	#endif
};







//
//
// ShaderKeyCg
//
//
class FuSYSTEM_API ShaderKeyCg
{
public:
	uint32 Hash;				// combined hash for mtls and lights (for quick rejection)
	uint32 LightHash;			// hash just for lights (when you know that the materials are the same already)
	uint16 MtlKeyLen;
	uint16 LightKeyLen;
	uint64 *MtlKey;			// material only state
	uint64 *LightKey;			// light and projector state

public:
	ShaderKeyCg() {}
	~ShaderKeyCg();

	void BuildHash();
	bool operator==(const ShaderKeyCg &rhs);
};










//
//
// class ContextCg
//
//
class FuSYSTEM_API ContextCg : public LockableObject
{
	FuDeclareClass(ContextCg, LockableObject);

public:
	char Name[32];																// the name of this context, mostly for debugging purposes

	CGcontext Context;
	FusionApp *App;
	bool ErrorOccurred;
	ShaderCg *CurrentShader;
	char **Options;

	int32 CurrentVertexProfile;											// these are CGprofile
	int32 DefaultVertexProfile;

	int32 CurrentGeometryProfile;
	int32 DefaultGeometryProfile;

	int32 CurrentFragmentProfile;
	int32 DefaultFragmentProfile;

	bool DumpCgErrorsToConsole;

public:
	ContextCg(const char *name);
	ContextCg(const Registry *reg, const ScriptVal &table, const TagList &tags);
	ContextCg(const ContextCg &toCopy);									// nyi
	virtual ~ContextCg();

	bool Init();																// this creates the context
	void Free();																// and this frees it

	bool IsContext();															// cgIsContext(Context)
	void SetAutoCompileToManual();
	void SetManageTextureParameters(bool manage);

	CGprogram GetFirstProgram();
	CGprogram GetNextProgram(CGprogram prog);

	void EnableVertexProfile(int32 profile = 0);						// chooses best profile if 0
	void DisableVertexProfile();
	bool IsVertexProfileEnabled(int32 profile = 0);					// checks if any profile is enabled if profile = 0
	int ChooseVertexProfile();

	void EnableFragmentProfile(int32 profile = 0);					// chooses best profile if 0
	void DisableFragmentProfile();
	bool IsFragmentProfileEnabled(int32 profile = 0);				// checks if any profile is enabled if profile = 0
	int ChooseFragmentProfile();

	CGparameter CreateParameter(uint32 type);	
	CGparameter CreateParameterArray(uint32 type, int length);
	CGparameter GetArrayParameter(CGparameter aparam, int index);
	void DestroyParameter(CGparameter param);							// its OK if param is NULL

	CGparameter GetNamedStructParameter(CGparameter param, const char *name);	

	void ConnectParameter(CGparameter from, CGparameter to);
	void DisconnectParameter(CGparameter to);							// can only disconnect destinations, not sources

	void SetParameterVariabilityToLiteral(CGparameter param);	// cgSetParameterVariability(param, CG_LITERAL)

	void SetParameter1fv(CGparameter param, const float *v);
	void SetParameter2fv(CGparameter param, const float *v);
	void SetParameter3fv(CGparameter param, const float *v);
	void SetParameter4fv(CGparameter param, const float *v);

	void SetParameter1f(CGparameter param, float x);
	void SetParameter2f(CGparameter param, float x, float y);
	void SetParameter3f(CGparameter param, float x, float y, float z);
	void SetParameter4f(CGparameter param, float x, float y, float z, float w);

	void SetMatrixParameterfr(CGparameter param, const float32 *m);
	void SetMatrixParameterfr(CGparameter param, const Matrix4f &m);

	void SetTextureParameter(CGparameter param, GLuint texobj);	// cgGLSetTextureParameter()

	const char *GetLastListing();

	uint32 GetError();														// CGerror cgGetError()
	const char *GetErrorString(uint32 errType);						// const char *cgGetErrorString(CGerror error)
	void SetErrorCallback(CgErrorCallback callback);				// void cgSetErrorCallback(CGerrorCallbackFunc func);
	void SetErrorHandler(CgErrorHandlerFunc func, void *appdata);

	const char *GetLatestErrorString();									// cgGetErrorString(cgGetError())

	void DumpProgram(FILE *fp, CGprogram prog);						// dumps the program
	void DumpContext(FILE *fp);											// dumps the entire context

protected:
	void DumpContextThrows(FILE *fp);
};








//
//
// class ShaderCg - This class represents a Cg fragment program.  It may contain a heirarchy of connected ShadeNodeCg's.
//
//
class FuSYSTEM_API ShaderCg : public Resource3D
{
	FuDeclareClass(ShaderCg, Resource3D);

public:
	enum ShaderType
	{
		ST_Vertex,
		ST_Geometry,
		ST_Fragment,
		ST_NumTypes
	};

	ShaderKeyCg *Key;

	ShaderType Type;
	uint32 Profile;
	CGprogram Program;
	ContextCg *Context;
	LockableObject Lock;

	ParamCg DefaultLightArray;
	ParamCg DefaultProjectorArray;

	uint16 NumMaterials, NumLights, NumProjectors;
	ShadeNodeCg **MtlList;
	ShadeNodeCg **LightList;
	ShadeNodeCg **ProjectorList;

	std::vector<ParamCg> Adapters;

public:
	ShaderCg();
	ShaderCg(const Registry *reg, const ScriptVal &table, const TagList &tags);	// do not use
	ShaderCg(const ShaderCg &toCopy);															// nyi
	virtual ~ShaderCg();

	// All functions within this class are protected by ObtainLock()/ReleaseLock()
	bool ObtainLock(ContextCg *context);
	void ReleaseLock();
	bool IsLocked();

	bool Init(int nMtls, int nLights, int nProjectors);

	void AddShadeNode(ShadeNodeCg *sn);
	void AddLightNode(ShadeNodeCg *sn);
	void AddProjectorNode(ShadeNodeCg *sn);

	ShadeNodeCg *GetShadeNode(MtlImplGL3D *mtl);

	bool CreateProgram(const char *program, const char *entry, ShaderType type = ST_Fragment, const char **options = NULL);
	void DestroyProgram();

	bool Compile();
	bool IsCompiled();

	bool Load();
	bool IsLoaded();

	virtual void OnCompile();												// called after shader is compiled and loaded

	virtual void Bind();
	void Unbind();

	void EnableProfile();
	void DisableProfile();

	uint32 GetUserType(const char *name);								// gets the type of a user parameter given the name of its struct
	
	ParamCg CreateUserParameter(const char *name);					// use this to create a user parameter given its struct 'name' it appears by in the program text.
	ParamCg CreateUserParameter(uint32 type);

	ParamCg CreateLightArrayParameter(RenderContextGL3D &rc);	// can return a cached one
	ParamCg CreateProjectorArrayParameter(RenderContextGL3D &rc);
	ParamCg CreateProjectorArrayParameter(RenderContextGL3D &rc, int projectorID);

	ParamCg CreateParameterArray(uint32 type, int length);
	ParamCg CreateParameterArray(const char *name, int length);

	ParamCg GetProgramParameter(const char *name);					// gets a parameter to the args of main()
	ParamCg GetGlobalParameter(const char *name);					// gets a global parameter

	void SetInputToConst(ParamCg input, float32 val);				// input must be declared as 'FuFloat1'
	void SetInputToConst(ParamCg input, const Vector2f &val);	// input must be declared as 'FuFloat2'
	void SetInputToConst(ParamCg input, const Vector3f &val);	// input must be declared as 'FuFloat3'
	void SetInputToConst(ParamCg input, const Vector4f &val);	// input must be declared as 'FuFloat4'

	void ConnectToFragmentNormal(ParamCg input);
	//void SetInputToFragmentTexCoord(ParamCg input);

	void ConnectFloat4ToFloat1(ParamCg float4Src, ParamCg float1Dest);

	void Free();

	#ifndef USE_NOSCRIPTING
		DECLARE_SIMPLE_LUATYPE(ShaderCg);
	#endif
};


#endif