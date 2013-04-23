#ifndef _3D_DATATYPES_H_
#define _3D_DATATYPES_H_

#include "Image.h"
#include "3D_AABB.h"
#include "VectorMath.h"
#include "GLExtensions.h"
#include "RegistryLists.h"
#include "LockableObject.h"
#include "StandardControls.h"		// for IC_TAG

#ifdef _DEBUG
#undef new						// MFC-STL-MS memory debugging conflicts
#endif

#include <stack>
#include <deque>
#include <vector>
#include <fstream>
#include <algorithm>
#include <functional>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define _DO_BETTER_SHADOWS 1  

#ifdef _DEBUG		// TODO - this should go into a header common to all the scanline renderer files
	//#define F3D_DEBUG_SCANLINE_RENDERER								// this must be defined for any of the following defines to work
	#ifdef F3D_DEBUG_SCANLINE_RENDERER
		#define F3D_DEBUG_SCANLINE_SHOW_DEPTH_COMPLEXITY
		#define F3D_DEBUG_SCANLINE_SHOW_MERGED_FRAGMENTS
		#define F3D_DEBUG_SCANLINE_SHOW_INTERSECTING_FRAGMENTS
	#endif
#endif


//#define F3D_CREATE_BSP_STATISTICS 1
#ifdef _DEBUG
	#define F3D_CREATE_BSP_STATISTICS 1			// create statistics on balance, depth, splits, etc, but slows things down
#else
	#define F3D_CREATE_BSP_STATISTICS 0
#endif


#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#define DEF_TX 0.0
#define DEF_TY 0.0
#define DEF_TZ 0.0
#define DEF_RX 0.0
#define DEF_RY 0.0
#define DEF_RZ 0.0
#define DEF_SX 1.0
#define DEF_SY 1.0
#define DEF_SZ 1.0
#define DEF_CX 0.0
#define DEF_CY 0.0
#define DEF_CZ 0.0
#define DEF_ROTATION_ORDER double(RO_XYZ)

#define ALIGN16 _declspec(align(16))

class Data3D;
class Inputs3D;
class Stream3D;
class MtlData3D;
class MtlImpl3D;
class Resource3D;
class LightData3D;
class LightImpl3D;
class IndexArray3D;
class RenderContext3D;


// Z values vary between 0.0f and -infinity.  Uninitialized depth values have the value FAR_PLANE_DEPTH (eg. when outputing Z values from the renderer).
const float32 MAX_FLOAT_DEPTH = 0.0f;						// this is the near plane
const float32 FAR_PLANE_DEPTH = -1.0e30f;					// this is the far plane, despite it being named the "minimum" depth

// We don't want to let the near clip go too close to 0.0 in the perspective proj because of huge z-loss precision for zNear < 1.0f 
// for OpenGL renderer, and the software renderer starts to have precision problems becoming noticable around 0.05 - 0.01 on a scene 
// with a simple cube in it.
const float64 MIN_PERSP_NEAR = 0.05;

// The minimum and maximum field of view for cameras.  Don't go all the way to 180... at around 177 degrees really weird things start 
// to happen in the view, probably due to elements of the perspective matrix blowing up or going to zero.
const float64 MAX_FOV_Y = 175.0;
const float64 MIN_FOV_Y = 0.001;

// Default settings for cameras
const float64 DEFAULT_PERSP_FOV_Y = 45.0;	
const float64 DEFAULT_PERSP_NEAR = 0.05;				// its not good to set zNear < 1.0, because z-buffer is equally spaced in 1/z and lots of bits of zbuffer resolution get used up rapidly for distances < 1.0 (but this doesn't matter since the GL renderer will rescale the scene so that zNear = 1)
const float64 DEFAULT_PERSP_FAR = 100000.0;
const float64 DEFAULT_ORTHO_NEAR = -DEFAULT_PERSP_FAR;
const float64 DEFAULT_ORTHO_FAR = DEFAULT_PERSP_FAR;	

// The smallest value that the ortho scale can be set to (smaller gets clamped)
const float64 MIN_ORTHO_SCALE = 0.0001;

// Default material values for materials so we have some degree of uniformity.  Note from a 3D viewpoint it would make more sense to set the
// specular and diffuse colors to be 0.5 so that total illumination <= 1.0f.  From a 2D compositor viewpoint, the diffuse color is also used
// when compositing without lighting, so people would prefer it always to be 1.0f.
extern FuSYSTEM_API const Color4f DefaultDiffuseColor;
extern FuSYSTEM_API const Color4f DefaultSpecularColor;
extern FuSYSTEM_API const Color4f DefaultEmissiveColor;
const float32 DefaultSpecularExponent = 50.0f;			// certain ATI cards can do weird things in fixed function pipeline if this is > 127.0
const float32 DefaultAlpha = 1.0f;
const float32 DefaultOpacity = 1.0f;

// The IDs are limited because the material/object channels are uint16.
const uint32 MAX_MATERIAL_ID = 65535;
const uint32 MAX_OBJECT_ID = 65535;

/**
 * Predefine some Main IDs to be used as LINK_Main for materials.  Since inputs are colored by LINK_Main numbers using them can provide 
 * some measure of consistency to the UI.  Keep in mind that LINK_Main = 1 is the passthrough input.
 *   1  = orange
 *   2  = green
 *   3  = pink
 *   4  = purple
 *   5+ = white
 */
enum MMID_MaterialMainID
{
	MMID_Background = 1,			// material merge, chan bool, reflect
	MMID_Foreground,

	MMID_CrossImage = 1,			// cubemap/spheremap - must remain as 1 for auto connect to 3Sp and 3Cu to work
	MMID_PositiveX,				// left
	MMID_NegativeX,				// right
	MMID_PositiveY,				// up
	MMID_NegativeY,				// down
	MMID_PositiveZ,				// front
	MMID_NegativeZ,				// back

	MMID_DiffuseColor = 1,
	MMID_SpecularColor,
	MMID_SpecularIntensity,
	MMID_SpecularExponent,		// blinn/phong
	MMID_Bumpmap,					// EyeNormal
	MMID_TransmissiveColor,
	MMID_EmissiveColor,
	MMID_Roughness,				// cook-torrance
	MMID_RefractiveIndex,		// cook-torrance
	MMID_SpreadU,					// ward
	MMID_SpreadV,					// ward
	MMID_RefractionColor,
	MMID_ReflectionColor,
	MMID_RefractionIntensity,
	MMID_ReflectionIntensity,
};

extern FuSYSTEM_API const FuID LinkType_Eye_Normal;

const uint8 F3D_DataType_Size[] = { 1, 2, 4, 2, 4, 8 };		// in bytes

const char * const F3D_DataType_Str[] = { "int8", "int16", "int32", "float16", "float32", "float64" };	// appearing in the gui

const F3D_DataType IMDPDepth_To_F3DDataType[9] =
{	
	F3D_INVALID_DATA_TYPE,
	F3D_UINT8,		F3D_UINT16,		F3D_FLOAT16,		F3D_FLOAT32,
	F3D_UINT8,		F3D_UINT16,		F3D_FLOAT16,		F3D_FLOAT32,
};

const uint8 IMDP_Size[] = { 0, 1, 2, 2, 4, 1, 2, 2, 4 };

const GLenum IMDPDepth_To_GLenum[9] = 
{
	GL_ZERO,
	GL_UNSIGNED_BYTE,		GL_UNSIGNED_SHORT,		GL_HALF_FLOAT_ARB,		GL_FLOAT,
	GL_UNSIGNED_BYTE,		GL_UNSIGNED_SHORT,		GL_HALF_FLOAT_ARB,		GL_FLOAT,
};


const uint32 F3DDataType_To_IMDP[] =
{
	IMDP_32bitInt, 
	IMDP_64bitInt,
	IMDP_64bitInt,
	IMDP_64bitFloat,
	IMDP_128bitFloat,
	IMDP_128bitFloat,
};


// for exception safe memory handling
template<typename T>
class AutoDelete
{
public:
	T *Pointer;
	AutoDelete(T *t) : Pointer(t) {}
	~AutoDelete() { delete Pointer; }
};

template<typename T>
class AutoDeleteArray
{
public:
	T *Pointer;
	AutoDeleteArray(T *t) : Pointer(t) {}
	~AutoDeleteArray() { delete [] Pointer; }
};

template<typename T>
class AutoFree
{
public:
	T *Pointer;
	AutoFree(T *t) : Pointer(t) {}
	~AutoFree() { free(Pointer); }
};

template<typename T>
class AutoMMFree
{
public:
	T *Pointer;
	AutoMMFree(T *t) : Pointer(t) {}
	~AutoMMFree() { _mm_free(Pointer); }
};

#define auto_delete(ptr)       AutoDelete __autodelete##__COUNTER__(ptr)
#define auto_array_delete(ptr) AutoArrayDelete __autoarraydelete##__COUNTER__(ptr)
#define auto_free(ptr)         AutoFree __autofree##__COUNTER__(ptr)
#define auto_mm_free(ptr)      AutoMMFree __autommfree##__COUNTER__(ptr)







// Prefer throw by value and catch by reference.  eg. try { throw FuException3D("error!") } catch (FuException &e) { printf("%s\n", e.what()); }
class FuSYSTEM_API FuException3D : public std::exception		
{
public:
	FuException3D();													// not silent
	FuException3D(const char *msg, ...);						// takes printf format specifiers
	//FuException3D(uint16 silent, const char *msg, ...);	// message won't be displayed to console
	FuException3D(const FuException3D &toCopy);	
	virtual ~FuException3D();

	virtual const char *what() const;
	virtual bool IsSilent() const;

	static const int MSG_BUFFER_LEN = 1024;

protected:
	char Msg[MSG_BUFFER_LEN];
	bool Silent;
};

// This is a separate class instead of parameter to FuException's ctor because the only way I can figure out to overload the varargs ctor 
// without ambiguities is ugly.
class FuSYSTEM_API FuSilentException3D : public FuException3D	
{
public:
	FuSilentException3D();
	FuSilentException3D(const char *msg, ...);
	FuSilentException3D(const FuSilentException3D &toCopy);	
	virtual ~FuSilentException3D();
};


#define INPUTS3D_BASE  (ICTAG + 2 * TAGOFFSET)		// = INPUT_BASE + 4 * TAGOFFSET  -- Region3D.h already has INPUT_BASE + 3 * TAGOFFSET

/**
 * Extra tags for use when calling Inputs3D::AddInputs() or Operator3D::AddInputs().  These tags are used to control various automatic behaviour
 * that Fusion does for shader parameters and child materials.
 */
enum Input3D_Tags ENUM_TAGS
{
	I3D_BaseIntTag = INPUTS3D_BASE + TAG_INT,
	I3D_AutoFlags,					// (0) set to any combination of auto flags (eg. PF_AutoProcess | PF_AutoActivate)
	I3D_AutoProcess,				// (false) determines if you'll have to write glue code in Process() to read the inputs value into the data object 
	I3D_AutoConnect,				// (false) determines if you'll have to write glue code in ConnectParams()/ConnectChildMtls().
	I3D_AutoActivate,				// (false) determines if you'll have to write glue code in Activate() to set the value into the Cg parameter
	I3D_Literal,					// (true) flags this input as a literal - a constant that will get folded into shaders
	I3D_ChildSlot,					// (-1) specifies which ChildSlot the material from this input will be placed into.  This flag is required if you want Fusion to automatically handle your child materials.  If left as -1 it  will be the same as setting I3D_AutoProcess = I3D_AutoCreateParam = I3D_AutoCreateChild = I3D_AutoActivate.
	//I3D_ShadeParam,				// (true) nyi - indicates the parameters value should be available in Shade()
	//I3D_TransmitParam,			// (true) nyi - indicates the parameters value should be available in Transmit()

	I3D_DblBaseTag = INPUTS3D_BASE + TAG_DOUBLE,
	I3D_StringBaseTag = INPUTS3D_BASE + TAG_STRING,
	I3D_PtrBaseTag = INPUTS3D_BASE + TAG_PTR,
	I3D_ObjBaseTag = INPUTS3D_BASE + TAG_OBJECT,

	I3D_IDBaseTag = INPUTS3D_BASE + TAG_ID,
	I3D_ParamName,					// (Input.m_ID) the name the parameter will be known by in the SW/GL materials and in the Cg shader.  This string is used to match up inputs with shader parameters.  If unspecified it will default to the inputs ID.  It can be optionally postfixed with a ".R", ".G", ".B" or ".A" to indicate a particular component of a parameter (XYZW also work).
};


enum ParamFlags
{
	PF_AcceptsMaterials	= 0x00000001,		// the src input accepts Materials
	PF_AcceptsImages		= 0x00000002,		// the src input accepts Images
	PF_AcceptsNumbers		= 0x00000004,		// the src input accepts Numbers
	PF_Anonymous			= 0x00000008,		// if the parameter is not linked to Data3D class member it is called "anonymous"
	PF_AutoProcess			= 0x00010000,		// lower 4 bytes reserved for size ??? - separate member var?
	PF_AutoConnect			= 0x00020000,
	PF_AutoActivate		= 0x00040000,
	PF_Literal				= 0x00080000,

	PF_Material				= 0x02000000,
	PF_Bool					= 0x04000000,
	PF_Int					= 0x08000000,		// 32 bit int
	PF_Float					= 0x10000000,
	PF_Double				= 0x20000000,
	PF_TypeMask				= 0x3e000000,
	//PF_String
};

// By default managed parameters are assigned private storage locations however you can choose to store the parameters value in a
// location of your choice.  This handy macro causes the parameter with the specified name to be stored in a variable with the
// identical name.  eg. SET_PARAM_STORAGE(Diffuse) would cause the parameter with name "Diffuse" to be stored in a variable 
// named Diffuse.
//SET_PARAM_STORAGE()

#define ADD_MANAGED_PARAM(name)								AddManagedParam(name, #name)
#define ALLOC_CHILD_SLOTS(nSlots)							AllocChildSlots(nSlots)
#define ADD_MANAGED_CHILD(name, nComponents, slot)		AddManagedChild(name, #name, nComponents, slot, PF_Material)

const bool _funner = true;

// TODO - there's like two types here, there's also the PR_ guys: PR_Points | PR_Wireframe;
enum PT_PrimitiveType
{
	PT_Points			= 0x01,	// each vertex is a point
	PT_LineSegments	= 0x02,	// each pair of vertices is is a line segment
	PT_Triangles		= 0x04,	// triples of vertices defining triangles in ccw order
	PT_Quads				= 0x08,	// quadruples of vertices defining quadrilaterals in ccw order
	PT_Particles		= 0x11,	// particles are flagged as (PT_Points | PT_Particles)

	PT_NumPrimitiveTypes = 4
};

enum PFM_PolygonFillMode
{
	PFM_Points		= 0x01,				// Render as points
	PFM_Wireframe	= 0x02,				// Render as wireframe
	PFM_Filled		= 0x04,				// Render as solid filled polygons
};

inline int NumVerticesPerPrimitive(PT_PrimitiveType type)
{
	switch (type)
	{
		case PT_Quads:
			return 4;
		case PT_Triangles:
			return 3;
		case PT_LineSegments:
			return 2;
		case PT_Points:
			return 1;
	}

	return -1;
}


/*
const char* const PrimitiveTypeStrings[PT_NumPrimitiveTypes] =
{
	"undefined",
	"points",
	"line segments",
	"triangles",
	"quads",
};
*/

/**
 * Given a point or a vector in model space, this is how they would transform into the given space:
 * CS_Unset		invariant																	invariant
 * CS_Model		point -> Identity * point,												vec -> Identity.InverseTranspose() * vec
 * CS_World		point -> ModelToWorld * point,										vec -> ModelToWorld.InverseTranspose() * vec
 * CS_Eye		point -> WorldToEye * ModelToWorld * point,						vec -> (WorldToEye * ModelToWorld).InverseTranpose() * vec
 * CS_Clip		point -> Projection * WorldToEye * ModelToWorld * point,		vec -> (Projection * WorldToEye * ModelToWorld).InverseTranspose() * vec
 */
enum CS_CoordSpace						
{			
	CS_Unset		= 0x00000000,	/**< does not have a natural coordinate system (eg. texture coordinates or vertex colors) or its unset */
	CS_Model		= 0x00000001,	/**< model space coordinate system (this is always with respect to some object/model) */
	CS_World		= 0x00000002,	/**< world space coordinates */
	CS_Eye		= 0x00000004,	/**< eye/camera coordinate system (this is always respect to some object we are viewing through, typically a camera, but it could be a light or something else) */
	CS_Clip		= 0x00000008,	/**< coordinates after the projection transform but before the homogenous divide has occured */
	CS_Screen	= 0x00000010,	/**< (x, y, z, 1/w) where x and y are pixel positions in the final image, z is the eye space distance to the near clip plane, and 1/w is the inverse the w value produced by the projection transform */
	CS_Tangent	= 0x00000020,	/**< tangent space, aka texture space, aka TBN space (Tangent, Binormal, Normal), useful for bumpmapping */
	CS_Any		= 0xffffffff
};

extern FuSYSTEM_API const FuID CSID_Model;
extern FuSYSTEM_API const FuID CSID_World;
extern FuSYSTEM_API const FuID CSID_Eye;
extern FuSYSTEM_API const FuID CSID_Clip;
extern FuSYSTEM_API const FuID CSID_Screen;
extern FuSYSTEM_API const FuID CSID_Tangent;


enum TT_TransformType
{
	TT_Unset = -1,
	TT_Scalar = 0,					/**< transforms as a scalar (ie. is invariant) */
	TT_Point = 1,					/**< transforms as a point */
	TT_DirectionVector,			/**< transforms as a direction vector */
	TT_Normal,						/**< transforms as a normal */
	TT_NumTransformTypes
};


/** 
 * Streams are uniquely identified by a name and a coordinate system.  A coordinate system does not make sense for all streams.  For example,
 * a normal might be specified in model, world, eye, or tangent space.  While a vertex color stream would always be CS_Unset since it is not
 * affected by transforms.  Lighting is Fusion is performed in eye space typically.  2D texture coordinates are 
 */
extern FuSYSTEM_API const FuID Stream_Position;			/**< per vertex position */
extern FuSYSTEM_API const FuID Stream_Normal;			/**< per vertex normal */
extern FuSYSTEM_API const FuID Stream_TangentU;			/**< per vertex tangent vector along the direction of the Texcoord0.U axis */
extern FuSYSTEM_API const FuID Stream_TangentV;			/**< per vertex tangent vector along the direction of the Texcoord0.V axis */
extern FuSYSTEM_API const FuID Stream_TexCoord0;		/**< per vertex texture coordinate */
extern FuSYSTEM_API const FuID Stream_TexCoord1;
extern FuSYSTEM_API const FuID Stream_TexCoord2;
extern FuSYSTEM_API const FuID Stream_TexCoord3;
extern FuSYSTEM_API const FuID Stream_Fog;				/**< per vertex fog factor */
extern FuSYSTEM_API const FuID Stream_VertexColor;		/**< per vertex color */
extern FuSYSTEM_API const FuID Stream_VertexColor2;	/**< when drawing line particles, the color of the second point */
extern FuSYSTEM_API const FuID Stream_PointSize;
extern FuSYSTEM_API const FuID Stream_LineWidth;
extern FuSYSTEM_API const FuID Stream_EnvCoord;			/**< per vertex environment map coordinates */

extern FuSYSTEM_API const FuID Stream_Color;				// output color

extern FuSYSTEM_API const FuID Stream_Velocity;			// these are currently used by particles (but in the future may not be specific to particles)
extern FuSYSTEM_API const FuID Stream_Rotation;
extern FuSYSTEM_API const FuID Stream_Scale;
extern FuSYSTEM_API const FuID Stream_Flags8;
extern FuSYSTEM_API const FuID Stream_Flags16;
extern FuSYSTEM_API const FuID Stream_Flags32;

extern FuSYSTEM_API const FuID Stream_ParticleNGonData;
extern FuSYSTEM_API const FuID Stream_ParticleBlobData;
extern FuSYSTEM_API const FuID Stream_ParticleCenterSize;




/**
 * Renderer output channels
 */
enum RendererChannel_t
{
	F3D_Chan_Custom = -2,		// its not one of below predefined channels
	F3D_Chan_Unknown = -1,
	F3D_Chan_RGBA = 0,			// the final color of the fragment							
	F3D_Chan_Z,						// eye space z value, it will be usually negative an in the range (MAX_FLOAT_DEPTH, FAR_PLANE_DEPTH)
	F3D_Chan_BgColor,				// accumulated RGBA color of all fragments behind the frontmost
	F3D_Chan_Coverage,			// coverage of frontmost fragment	
	F3D_Chan_Normal,				// eye space geometric normal computed by interpolating per vertex normals across triangles
	F3D_Chan_ShadingNormal,		// eye space shading normal output from shade tree (eg. it could be affected by bumpmaps)
	F3D_Chan_BentNormal,
	F3D_Chan_TangentU,			// eye space geometric tangent vector oriented along the u texture cooordinate
	F3D_Chan_TangentV,			// eye space geometric tangent vector oriented along the v texture cooordinate
	F3D_Chan_TexCoord,			// texture coords before Shade()
	F3D_Chan_ObjectID,			// this is the object ID which can be changed by the user in the UI
	F3D_Chan_MaterialID,			// this is the material ID which can be changed by the user in the UI
	F3D_Chan_Displacement,
	F3D_Chan_Velocity,			// nyi - will this be eye or screen space?
	F3D_Chan_Lighting,			// total lighting, unmultiplied by the material color
	F3D_Chan_Ambient,				// ambient lighting contribution, unmultiplied by ambient material coefficient
	F3D_Chan_Diffuse,				// diffuse lighting contribution unmultiplied by diffuse material coefficient (typically max(dot(n, l), 0))
	F3D_Chan_Specular,			// specular lighting contribution unmultiplied by specular material coefficient (eg. for blinn pow(max(dot(n, l), 0), e))
	F3D_Chan_Shadow,				// nyi - will this be white or black where shadows are?					
	F3D_Chan_Caustic,
	F3D_Chan_Refraction,
	F3D_Chan_Reflection,
	F3D_Chan_Transmission,
	F3D_NumRenChannels,
};

/**
 * FuIDs identifying renderer output channels.
 */
extern FuSYSTEM_API const FuID RCID_RGBA;
extern FuSYSTEM_API const FuID RCID_Z;
extern FuSYSTEM_API const FuID RCID_BgColor;
extern FuSYSTEM_API const FuID RCID_Coverage;
extern FuSYSTEM_API const FuID RCID_Normal;
extern FuSYSTEM_API const FuID RCID_ShadingNormal;
extern FuSYSTEM_API const FuID RCID_BentNormal;
extern FuSYSTEM_API const FuID RCID_TangentU;
extern FuSYSTEM_API const FuID RCID_TangentV;
extern FuSYSTEM_API const FuID RCID_TexCoord0;
extern FuSYSTEM_API const FuID RCID_ObjectID;
extern FuSYSTEM_API const FuID RCID_MaterialID;
extern FuSYSTEM_API const FuID RCID_Displacement;
extern FuSYSTEM_API const FuID RCID_Velocity;
extern FuSYSTEM_API const FuID RCID_Lighting;
extern FuSYSTEM_API const FuID RCID_Ambient;
extern FuSYSTEM_API const FuID RCID_Diffuse;
extern FuSYSTEM_API const FuID RCID_Specular;
extern FuSYSTEM_API const FuID RCID_Shadow;
extern FuSYSTEM_API const FuID RCID_Caustic;
extern FuSYSTEM_API const FuID RCID_Refraction;
extern FuSYSTEM_API const FuID RCID_Reflection;
extern FuSYSTEM_API const FuID RCID_Transmission;

extern FuSYSTEM_API const FuID RendererChannelFuID[];

/**
 * Converts one of the RendererChannelFuIDs into a RendererChannel_t.  Returns F3D_Chan_Custom if the chanID is not in the list of
 * predefined IDs above.
 */
extern FuSYSTEM_API RendererChannel_t GetChannelEnum(FuID chanID);

const int NumCustomStreams = 1;

//// "abc" -> {0, c, b, a}
////#define MAKE_STREAM_ID(str) ((str)[0] | ((str)[1] << 8) | ((str[2]) << 16))
//#define MAKE_STREAM_NAME(a, b, c) (a | (b << 8) | (c << 16))
//
//enum SN_StreamName
//{
//	Stream_EyePosition		= MAKE_STREAM_NAME('p', 'o', 's'),		// pos
//	Stream_EyeNormal			= MAKE_STREAM_NAME('n', 'o', 'r'),		// nor
//	Stream_EyeTangentU		= MAKE_STREAM_NAME('t', 'n', 'U'),		// tnU
//	Stream_EyeTangentV		= MAKE_STREAM_NAME('t', 'n', 'V'),		// tnV
//	Stream_TexCoord		= MAKE_STREAM_NAME('t', 'c', '0'),		// tc0
//	Stream_Fog				= MAKE_STREAM_NAME('f', 'o', 'g'),		// fog
//	Stream_VertexColor	= MAKE_STREAM_NAME('v', 'c', '0'),		// vx0
//	Stream_VertexColor2	= MAKE_STREAM_NAME('v', 'c', '2'),		// vx2
//	Stream_PointSize		= MAKE_STREAM_NAME('p', 't', 's'),		// pts
//	Stream_LineWidth		= MAKE_STREAM_NAME('l', 'n', 'w'),		// lnw
//	Stream_Color			= MAKE_STREAM_NAME('c', 'o', 'l'),		// col
//
//	// particles
//	Stream_Velocity		= MAKE_STREAM_NAME('v', 'e', 'l'),		// vel
//	Stream_Rotation		= MAKE_STREAM_NAME('r', 'o', 't'),		// rot
//	Stream_Scale			= MAKE_STREAM_NAME('s', 'c', 'a'),		// sca
//	Stream_Flags8			= MAKE_STREAM_NAME('f', '0', '8'),		// f08
//	Stream_Flags16			= MAKE_STREAM_NAME('f', '1', '6'),		// f16
//	Stream_Flags32			= MAKE_STREAM_NAME('f', '3', '2'),		// f32
//	Stream_NGonData		= MAKE_STREAM_NAME('p', 'n', 'd'),		// pnd
//	Stream_BlobData		= MAKE_STREAM_NAME('b', 'l', 'd'),		// bld
//
//	//Stream_ParticleSize			= MAKE_STREAM_NAME('p', 's', 'z'),	// psz
//	//Stream_ParticleCenter		= MAKE_STREAM_NAME('p', 'c', 'r'),	// pcr
//	Stream_ParticleCenterSize	= MAKE_STREAM_NAME('p', 'c', 's'),	// pcs
//};


// Predefined streams used for hardware/GL rendering
enum HW_Stream
{							// -GL array-  -type-  -attrIdx-   -notes-
	HW_Position,		//  POSITION   float3      0			model space, w defaults to 1, must be first
	HW_Normal,			//  NORMAL     float3      2			model space, w = ?  
	HW_VertexColor,	//  COLOR      float4      3
	HW_TexCoord0,		//  TEXCOORD0  float3      8
	HW_TexCoord1,		//  TEXCOORD1  float3      9
	HW_TexCoord2,		//  TEXCOORD2  float3     10
	HW_EnvCoord,		//  TEXCOORD3  float3     11			environment mapping coordinates
	HW_TangentU,		//  TEXCOORD6  float3 	  14			model space
	HW_TangentV,		//  TEXCOORD7  float3 	  15			model space
	HW_NumStreams,
	HW_Unset = 999999,
};

const int HW_GLAttribIndex[HW_NumStreams] =
{
	0,
	2,
	3,
	8,
	9,
	10,
	11,
	14,
	15
};

const int HW_StreamSize[HW_NumStreams] =
{
	3, // Position  
	3, // Normal  
	4, // VertexColor
	2,	// TexCoord0
	2, // TexCoord1
	2, // TexCoord2
	2, // TexCoord3
	3, // TangentU 
	3  // TangentV 
};

const char * const HW_CgStreamName[HW_NumStreams] = 
{
	"v.Position",   
	"v.Normal",     
	"v.VertexColor",
	"v.TexCoord0",  
	"v.TexCoord1",  
	"v.TexCoord2",  
	"v.TexCoord3",  
	"v.TangentU",   
	"v.TangentV"
};



enum SM_StereoMethod
{
	SM_ToeIn,								// rotate left and right eye cameras slightly towards each other so they convergence at the correct distance
	SM_Parallel,							// use parallel cameras and a perspective shift lens to achieve the stereo effect
	SM_NumStereoMethods,
};

const char * const StereoMethodName[SM_NumStereoMethods] =
{
	"Toe In",
	"Parallel",
};

extern FuSYSTEM_API const FuID SMID_ToeIn;
extern FuSYSTEM_API const FuID SMID_Parallel;

extern FuSYSTEM_API const FuID StereoMethodID[SM_NumStereoMethods + 1];



/**
 * Cameras associated with the software and GL renderers can be either orthographic or perspective cameras.  Although both
 * projection types simplify down to a matrix in the end, it is important to make the distinction as there are various bits 
 * of the renderer which can depend on the projection type.
 */
enum PRJ_ProjectionType
{
	PRJ_Orthographic,
	PRJ_Perspective,
	PRJ_NumProjectionTypes
};

extern FuSYSTEM_API const FuID PRJID_Perspective;
extern FuSYSTEM_API const FuID PRJID_Orthographic;

extern FuSYSTEM_API const FuID ProjectionTypeID[PRJ_NumProjectionTypes + 1];



enum WM_WrapMode							// do not reorder these, used for multibutton indices 
{
	WM_Wrap,									// repeat                - not supported by GL_TEXTURE_RECTANGLE_ARB				
	WM_Clamp,								// duplicate edge values
	WM_Black,								// transparent black
	WM_Mirror,								// reflection about edge - not supported by GL_TEXTURE_RECTANGLE_ARB
	WM_NumWrapModes,
	WM_Default = WM_Clamp,				// By default all textures are clamp.  Most 3D apps will assume WM_Wrap here so be careful!  Why clamp? because most common 3D usage in fusion is ImagePlane tools and WM_Wrap would cause filtering artefacts the edges.
	WM_Unset	= 0x80000000,
};

const char * const WrapModeNames[WM_NumWrapModes] =
{
	"Wrap",
	"Clamp",
	"Black",
	"Mirror",
};

const GLenum GL_WrapMode[WM_NumWrapModes] =
{
	GL_REPEAT,
	GL_CLAMP_TO_EDGE,
	GL_CLAMP_TO_BORDER,
	GL_MIRRORED_REPEAT,
};

extern FuSYSTEM_API const FuID WMID_Wrap;
extern FuSYSTEM_API const FuID WMID_Clamp;
extern FuSYSTEM_API const FuID WMID_Black;
extern FuSYSTEM_API const FuID WMID_Mirror;

extern FuSYSTEM_API const FuID WrapModeIDs[];

enum IT_IntersectionType
{
	IT_Inside = 0,					// the object is fully contained within the test volume
	IT_Intersecting,				// the object is partially contained within the test volume
	IT_Outside,						// the object is located completely outside the test volume
	IT_NumTypes,

	IT_Front = IT_Outside,		// for planes
	IT_Back = IT_Inside,
	IT_Coinciding = IT_Intersecting,
};

enum FrontBack
{
	FB_Front,							
	FB_Back,
	FB_Coinciding,						// A point coincides with a poly if its in the poly's plane.  Two polys coincide, if it they lie in the same plane
	FB_Spanning							// A poly spans another poly, if it divides the poly
};

enum SM_ShadingModel
{
	SM_Flat,
	SM_Smooth
};

enum SP_ShadePathGL3D
{
	SP_FixedFunction = 0,	// fixed function GL				-- basic fallback path used for per vertex lighting
	SP_Cg,						// Cg									-- currently used for per pixel lighting
	SP_FragmentProgram,		// ARB_fragment_program			-- this worked in 5.1 branch, not sure we're ever going to merge into the main branch code
	SP_GLSL,						//										-- this path was added when working on 5.0 but no longer works
};

// Node3D::DataFlags and Data3D::DataFlags use these flags
enum
{
	// data type flags
	F3D_IS_SURFACE						= 0x00000001,
	F3D_IS_CURVE						= 0x00000002,
	F3D_IS_SURFACE_OR_CURVE			= 0x00000004,
	F3D_IS_LIGHT						= 0x00000008,
	F3D_IS_CAMERA						= 0x00000010,

	// surface flags
	F3D_IS_VISIBLE						= 0x00000020,	// if false the object doesn't cast or receive shadows and never renders.  If true, other flags apply.
	F3D_IS_SEEN_BY_CAMERA			= 0x00000040,	// if false the object doesn't appear when rendered through a camera, but can still cast shadows if IS_CASTER is set
	F3D_IS_CASTER						= 0x00000080,	// is it a shadow caster?
	F3D_IS_RECEIVER					= 0x00000100,	// is it a shadow receiver?
	F3D_AFFECTED_BY_LIGHTS			= 0x00000200,	// is lighting turned on/off on this object? (if its off, it can still be a caster, but not a receiver)
	F3D_IS_NOT_MATTE_OBJECT			= 0x00000400,
	F3D_HAS_DATA						= 0x00000800,	// true if Data pointer is non-NULL
	F3D_CULL_BACK_FACE				= 0x00001000,
	F3D_CULL_FRONT_FACE				= 0x00002000,
	F3D_IGNORE_TRANSPARENT_PIXELS = 0x00004000,	// if true transparent pixels don't contribute to aux channels
	F3D_IS_CACHABLE					= 0x00008000,	// if false the geometry needs to be recreated each time the view is rerendered (eg. align to camera particles)

	// reserved for use by renderers which define their own internal flags
	F3D_RESERVED0						= 0x01000000,
	F3D_RESERVED1						= 0x02000000,
	F3D_RESERVED2						= 0x04000000,
	F3D_RESERVED3						= 0x08000000,

	F3D_RESERVED4						= 0x10000000,
	F3D_RESERVED5						= 0x20000000,
	F3D_RESERVED6						= 0x40000000,
	F3D_RESERVED7						= 0x80000000,
};


// This is a hint for improving interactive viewing of the polygons in the 3D view.  It tells the viewer how much work it should go through to
//  to sort/clip the polygon for correct transparency.  During transparency sorting, the polygon will optionally be split.
enum PCT_PolyClipType
{
	PT_Splitter,				// polygon splits other splitters/splittables and can be split by other splitters  (eg. image plane)
	PT_Splittable,				// polygon cannot split other polgyons, but can be split by splitters (eg. high density meshes)
	PT_Point,					// polygon cannot split other polygons nor be split (eg. particles)
	PT_NumPolyClipTypes
};




/**
 * A subclass of Inputs3D that wants to inject its inputs into a victim would have a REGP_InjectInputsFunc tag in its registry pointing
 * to a InjectInputsFunc to do the injection.  Typically this function would:
 *  1) decide if it wants to inject inputs into the victim  (eg. by examining victim->m_RegNode)
 *  2) create the subinputs to inject (eg. subInputs = new MyInputs3D)
 *  3) add the subinputs to the victim (eg. call victim->AddSubInputs(subInputs)
 *  4) if the return value is true the InjectInputsFunc will be called again on future victims of the same class.  If the return value
 *     is false, it won't be called.
 */
typedef bool (* InjectInputsFunc)(Inputs3D *victim, TagList *tags);

/**
 * According to MSDN, reads/writes are 32 variables are guarenteed to be atomic on 32 bit platforms and 32/64 atomic on 64 bit platforms.
 * To document when a variable is being used this way we'll use our own types.
 */
typedef int32 aint32;
typedef int64 aint64;

// TODO - should there be a way to take any input and replace it with a texture?
// TODO - there also needs to be a classification of materials based on what they use as texture coordinates
//        eg. 1d, 2d, 3d, 4d
//			 and where those should come from (eg. position, normal, texcoord set #)
extern FuSYSTEM_API const FuID TT_Ambient;					// ambient material color
extern FuSYSTEM_API const FuID TT_Diffuse;					// diffuse material color
extern FuSYSTEM_API const FuID TT_Specular;					// specular material color
extern FuSYSTEM_API const FuID TT_Emmisive;					// emmisive material color
extern FuSYSTEM_API const FuID TT_SpecularExponent;		// specular exponent
extern FuSYSTEM_API const FuID TT_SpecularIntensity;		// specular intensity
extern FuSYSTEM_API const FuID TT_Bumpmap;
//.... tons and tons more, plus user defined

const char * const CubeFaceName[] = { "Front", "Right", "Left", "Bottom", "Top", "Back" };

extern FuSYSTEM_API const FuID FITM_Inside;
extern FuSYSTEM_API const FuID FITM_Width;
extern FuSYSTEM_API const FuID FITM_Height;
extern FuSYSTEM_API const FuID FITM_Outside;
extern FuSYSTEM_API const FuID FITM_Stretch;

enum FITM_FitMethod
{
	FitMethod_Inside,
	FitMethod_Width,
	FitMethod_Height,
	FitMethod_Outside,
	FitMethod_Stretch,
	FitMethod_NumMethods,
};

extern FuSYSTEM_API const FuID FitMethod_IDs[];




// TODO - we should split this file's enums out into generic Fusion defines & string names and GL specific defines

enum TEX_TexApplyModes
{
	TEX_DECAL = 0,
	TEX_MODULATE,
	TEX_BLEND,
	TEX_REPLACE,
	TEX_NUM_TEX_APPLY_MODES = 4
};

/**< For conversion from NumIndicesPerPrimitive to the GLenum without branching ifs. */
const GLenum GL_PrimitiveType[] = 
{ 
	0,
	GL_POINTS,		// 1
	GL_LINES,		// 2
	GL_TRIANGLES,	// 3
	GL_QUADS			// 4
};

const GLenum GL_TexApplyModes[TEX_NUM_TEX_APPLY_MODES] =
{
	GL_DECAL,
	GL_MODULATE,
	GL_BLEND,
	GL_REPLACE,
};


enum TEX_TexFilterModes
{
	TEX_NEAREST = 0,
	TEX_BILINEAR,						// same idea as GL_LINEAR
	TEX_TRILINEAR,						// same idea as GL_LINEAR_MIPMAP_LINEAR
	TEX_NUM_TEX_FILTER_MODES = 4
};

const GLenum GL_TexFilterModes[TEX_NUM_TEX_FILTER_MODES] =
{
	GL_NEAREST,
	GL_LINEAR,		
	GL_LINEAR_MIPMAP_LINEAR
};

enum TEX_TexBorderModes
{
	TEX_CLAMP_TO_EDGE,					// same idea as GL_CLAMP_TO_EDGE
	TEX_REPEAT,
	TEX_NUM_TEX_BORDER_MODES = 2
};

const GLenum GL_TexBorderModes[TEX_NUM_TEX_BORDER_MODES] =
{
	GL_CLAMP_TO_EDGE,
	GL_REPEAT,
};






//enum VertexResource_Tags ENUM_TAGS
//{
//	VR_BaseIntTag = TAG_INT + VERTEX_RESOURCE_BASE,
//};
//
//
//enum ShaderResource_Tags ENUM_TAGS
//{
//	SR_BaseIntTag = TAG_INT + SHADER_RESOURCE_BASE,
//};


// We are assuming that the faces of the cubemap are generated by an observer that is always inside the cubemap looking out.  For example
// the +Z face of the cubemap is generated by a camera sitting at the origin looking up the +Z axis.  The entire texture has s increasing 
// to the right and t increasing going up, but within each cell OpenGL/DX/RM have conventions/specifications for which way s/t increase 
// and how the s/t axes need to be flipped.  

enum CMF_Face
{
	CMF_PositiveX = 0,			// chosen to agree with the GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB enumerations
	CMF_NegativeX,
	CMF_PositiveY,
	CMF_NegativeY,
	CMF_PositiveZ,
	CMF_NegativeZ,
	CMF_NumFaces,

	CMF_Left		= 0,
	CMF_Right	= 1,
	CMF_Up		= 2,
	CMF_Down		= 3,
	CMF_Front	= 4,
	CMF_Back		= 5,
};

// When reading from the cross formats, the faces are extracted as follows for vertical/horizontal cross formats:
//    +Y          +Y
// +X +Z -X    +X +Z -X -Z
//    -Y          -Y
//    -Z
enum CMO_Orientation
{
	CMO_SeparateImages,
	CMO_VerticalCross,
	CMO_HorizontalCross
};

// camera at origin looking in dir:   left         right         up         down         front         back
const char * const CM_FaceID[]   = { "Left",		  "Right",		 "Up",		"Down",	    "Front",      "Back" };
const char * const CM_FaceName[] = { "Left (+X)", "Right (-X)", "Up (+Y)", "Down (-Y)", "Front (+Z)", "Back (-Z)" };

const CPoint CM_VerticalCrossOffset[CMF_NumFaces]   = { CPoint(0, 2), CPoint(2, 2), CPoint(1, 3), CPoint(1, 1), CPoint(1, 2), CPoint(1, 0) };
const CPoint CM_HorizontalCrossOffset[CMF_NumFaces] = { CPoint(0, 1), CPoint(2, 1), CPoint(1, 2), CPoint(1, 0), CPoint(1, 1), CPoint(3, 1) };

const CPoint CM_VerticalCrossInvert[CMF_NumFaces]   = { CPoint(-1, -1), CPoint(-1, -1), CPoint(-1, -1), CPoint(-1, -1), CPoint(-1, -1), CPoint(+1, +1) };
const CPoint CM_HorizontalCrossInvert[CMF_NumFaces] = { CPoint(-1, -1), CPoint(-1, -1), CPoint(-1, -1), CPoint(-1, -1), CPoint(-1, -1), CPoint(-1, -1) };
const CPoint CM_SeparateImagesInvert[CMF_NumFaces]  = { CPoint(-1, -1), CPoint(-1, -1), CPoint(-1, -1), CPoint(-1, -1), CPoint(-1, -1), CPoint(-1, -1) };






class FuSYSTEM_API ShaderString3D
{
public:
	char *Data;

public:
	ShaderString3D()					{ Data = NULL; }	
	ShaderString3D(const char *s);			// copys "s" into internal buffer
	ShaderString3D(int len);					// creates uninitialized string
	~ShaderString3D() {}							// have to explicitly call Free()

	void Free();

	char *GetString()					{ return Data; }
	int GetLength()					{ return *((uint32 *) (Data - 4)); }	// not counting terminating NULL
	void SetLength(int len)			{ *((uint32 *) (Data - 4)) = len; }
	operator const char *() const { return Data; }
	bool IsNull()						{ return Data == NULL; }
	bool IsValid()						{ return Data != NULL; }
};




// some macros that make memory handling & error handling neater
template<typename T> void SAFE_USE(T *p)									{ if (p) { p->Use(); } }
template<typename T> void SAFE_RECYCLE(T *&p)							{ if (p) { p->Recycle(); p = NULL; } }
template<typename T> void SAFE_DELETE(T *&p)								{ if (p) { delete p; p = NULL; } }
template<typename T> void SAFE_RELEASE(T *&p)							{ if (p) { p->Release(); p = NULL; } }
template<typename T> void SAFE_FREE(T *&p)								{ if (p) { free(p); p = NULL; } }
template<typename T> void SAFE_DELETE_ARRAY(T *&p)						{ if (p) { delete [] p; p = NULL; } }
template<typename T> void SAFE_MM_FREE(T *&p)						   { if (p) { _mm_free(p); p = NULL; } }
template<typename T> void SAFE_RECYCLE_VECTOR(std::vector<T> &v)  { int n = v.size(); for (int i = 0; i < n; i++) { if (v[i]) { v[i]->Recycle(); } } v.clear(); } // recycles each non-zero element and clears out the vector
template<typename T> void SAFE_DELETE_VECTOR(std::vector<T> &v)   { int n = v.size(); for (int i = 0; i < n; i++) { if (v[i]) { delete v[i];     } } v.clear(); } // deletes each non-zero element and clears out the vector
#define SAFE_ASSIGN(fuPtr, p)													{ if (p) { (fuPtr) = (p); (p)->Recycle(); (p) = NULL; } }


const uint32 prev3[3]		= { 2, 0, 1 };
const uint32 next3[3]		= { 1, 2, 0 };			// a look up table to avoid doing modulo base 3 computations
const uint32 nextnext3[3]	= { 2, 0, 1 };			// same as prev3
const uint32 prev4[4]		= { 3, 0, 1, 2 };
const uint32 next4[4]		= { 1, 2, 3, 0 };
const uint32 nextnext4[4]	= { 2, 3, 0, 1 };

inline uint32 Prev3(uint32 i)
{
	return prev3[i];
}

inline uint32 Next3(uint32 i) 
{
	return next3[i];			// return i % 3;
}

inline uint32 Prev4(uint32 i)
{
	return prev4[i];
}

inline uint32 Next4(uint32 i) 
{
	return next4[i];			// return i % 4;
}

// If x is odd, return x.  If x is even return x + 1
inline uint32 NextOdd(uint32 x) { return x + 1 - (x & 0x1); }
inline uint32 NextEven(uint32 x) { return (x + 1) & ~0x1; }

inline bool IsOdd(uint32 x) { return (x & 0x1) != 0; }
inline bool IsEven(uint32 x) { return (x & 0x1) == 0; }

inline void swap(int32 &a, int32 &b)
{
	int32 temp = a;
	a = b;
	b = temp;
}

static void swapf(float32 &a, float32 &b)
{
	float32 temp = b;
	b = a;
	a = temp;
}

inline void SwapChannels(ChannelType &a, ChannelType &b)
{
	ChannelType temp = a;
	a = b;
	b = temp;
}

inline float32 minf(float32 a, float32 b)
{
	return (a < b ? a : b);
}

inline float32 maxf(float32 a, float32 b)
{
	return (a < b ? b : a);
}

inline float32 Saturate(float32 v)
{
	if (v > 1.0f)
		v = 1.0f;
	else if (v < 0.0f)
		v = 0.0f;

	return v;
}

inline uint32 Clamp(int32 x, int32 xMin, int32 xMax)
{
	if (x < xMin)
		return xMin;
	else if (x > xMax)
		return xMax;
	else
		return x;
}

inline float32 Clamp(float32 x, float32 xMin, float32 xMax)
{
	if (x < xMin)
		return xMin;
	else if (x > xMax)
		return xMax;
	else
		return x;
}

inline float32 Smoothstep(float32 xMin, float32 xMax, float32 x)
{
	if (x < xMin)
		return 0.0f;
	else if (x > xMax)
		return 1.0f;
	else
	{
		float32 tmp = (x - xMin) / (xMax - xMin);		// could precompute this denom
		float32 tmp2 = tmp * tmp;

		return 3 * tmp2 - 2 * tmp2 * tmp;				// hermite interpolation
	}
}

inline uint32 NextMultipleOf4 (uint32 n) { return (n + 3 ) & 0xfffffffc; }
inline uint32 NextMultipleOf16(uint32 n) { return (n + 15) & 0xfffffff0; }

inline uint32 NextPowerOf2(uint32 i)
{
	i -= 1;

	i |= i >> 16;
	i |= i >> 8;
	i |= i >> 4;
	i |= i >> 2;
	i |= i >> 1;

	return i + 1;
}

inline bool IsPowerOf2(uint32 i)
{
	return i > 0 && (i & (i - 1)) == 0;
}

inline bool IsPowerOf2(int32 i)
{
	return i > 0 && (i & (i - 1)) == 0;
}

enum NodeType3D		// this is also used as a Data3D type, perhaps it should be called something else?
{
	NDET_Undefined = 0,
	NDET_Merge,
	NDET_Transform,
	NDET_Light,
	NDET_Camera,
	NDET_Projector,			// a texture projector (light projectors are classified as NDET_Light)
	NDET_Curve,
	NDET_Surface,
	NDET_Material,				// this is an invalid type for Node3Ds but valid type for Data3D
};

// Searches the registry for an implementation of the given material and creates it.  Type will typically be CT_MtlImplGL3D, 
// CT_MtlImplSW3D, or CT_MtlImplGLSL3D.  Returns a new object of the same type as the first match if one is found and NULL 
// otherwise.  It is the callers responsiblity to make sure the passed in material pointer is non-NULL.
extern FuSYSTEM_API MtlImpl3D *CreateMtlImpl(RenderContext3D &rc, uint32 type, MtlData3D *mtl);

// Searches the registry for an implementation of the given light and creates it.  Type will typically be CT_LightImplGL3D, 
// CT_LightImplSW3D, or CT_LightImplGLSL3D.  Returns a new object of the same type as the first match if one is found and NULL 
// otherwise.  It is the callers responsiblity to make sure the passed in light pointer is non-NULL.
extern FuSYSTEM_API LightImpl3D *CreateLightImpl(RenderContext3D &rc, uint32 type, LightData3D *light);



//enum MtlSelectComboBox_Tags ENUM_TAGS
//{
//	MSCBX_BaseIntTag = TAG_INT + TEXTURE_RESOURCE_BASE,
//	MSCBX_MaterialInputsIndex,					// defaults to 0
//};




/**
 * When constructing normals for a mesh and you have multiple triangle normals contributing to the normal at a given vertex, then you create
 * the vertex normal by doing a weighted sum of the triangle normals.  There are various ways you can choose the weights.
 */
enum NWM_WeightMethod
{
	NWM_Unity,				/**< normal weight is always 1.0f */
	NWN_ByArea,				/**< normal weight is proportional to the area the triangle subtends from the vertex */
	NWM_ByAngle				/**< normal weight is proportional to the angle the triangle subtends from the vertex */
};


extern FuSYSTEM_API bool ComputeNormals(Stream3D *&norm, IndexArray3D *&normInd, PT_PrimitiveType pType, const Stream3D *pos, IndexArray3D *posInd, 
						  NWM_WeightMethod weightMethod = NWM_ByAngle, const uint32 *smthGrp = NULL);

extern bool FuSYSTEM_API ComputeTangents(Stream3D *&tanU, Stream3D *&tanV, /*IndexArray3D *tanInd,*/ PT_PrimitiveType pType, IndexArray3D *posInd,
	const Stream3D *pos, const Stream3D *texcoord, NWM_WeightMethod weightMethod = NWM_ByAngle, const uint32 *smthGrp = NULL);

struct FuSYSTEM_API NormalParam3D
{
	// In
	PT_PrimitiveType PrimitiveType;		// must be PT_Triangles or PT_Quads

	const Vector3f *Position;
	int NumPosition;							// size of the Position[] array
	const uint32 *PositionIndices;		// groups of 3/4 indices specifying triangles/quads
	int NumPositionIndices;		

	const uint32 *SmoothGrp;				// smooth group per primitive if there is one

	NWM_WeightMethod WeightMethod;		// this applies to the tangent vectors also


	// Out
	Vector3f *Normals;						// use _mm_free() to delete
	int NumNormals;

	uint32 *NormalIndices;					// if SmoothGrp is not NULL, this will be NULL, use PositionIndices
	int NumNormalIndices;					// if SmoothGrp is not NULL, this will be 0, use NumPositionIndices
};

struct FuSYSTEM_API TangentParam3D
{
	// In
	PT_PrimitiveType PrimitiveType;		// must be PT_Triangles or PT_Quads

	const Vector3f *Position;
	int NumPosition;							// size of the Position[] array
	const uint32 *PositionIndices;		// groups of 3/4 indices specifying triangles/quads
	int NumPositionIndices;		

	const Vector2f *TexCoord;	
	int NumTexCoord;
	const uint32 *UVIndices;
	int NumUVIndices;							// always the same as NumPositionIndices

	const uint32 *SmoothGrp;				// smooth group per primitive if there is one

	NWM_WeightMethod WeightMethod;		// this applies to the tangent vectors also


	// Out
	Vector3f *TangentU;						// use _mm_free() to delete
	Vector3f *TangentV;
	uint32 NumTangentUV;

	uint32 *TangentIndices;					// indices for normal and tangent vector arrays, use _mm_free() to delete
	int NumTangentIndices;
};

extern FuSYSTEM_API bool ComputeNormals(NormalParam3D &param);
extern FuSYSTEM_API bool ComputeTangents(TangentParam3D &param);


enum HS_HeightSource
{
	HS_Luminance,
	HS_Average,			// of RGB
	HS_Max,				// of RGB
	HS_Min,				// of RGB
	HS_Red,
	HS_Green,
	HS_Blue,
	HS_Alpha,
	HS_NumHeightSources,
};

const char * const HeightSourceNames[HS_NumHeightSources] =
{
	"Luminance",
	"Average",
	"Max",
	"Min",
	"Red",
	"Green",
	"Blue",
	"Alpha",
};

extern FuSYSTEM_API const FuID HSID_Luminance;
extern FuSYSTEM_API const FuID HSID_Average;
extern FuSYSTEM_API const FuID HSID_Max;
extern FuSYSTEM_API const FuID HSID_Min;
extern FuSYSTEM_API const FuID HSID_Red;
extern FuSYSTEM_API const FuID HSID_Green;
extern FuSYSTEM_API const FuID HSID_Blue;
extern FuSYSTEM_API const FuID HSID_Alpha;

extern FuSYSTEM_API const FuID HeightSourceIDs[];


const int CREATE_NORMAL_MAP_BASE = OBJECT_BASE + TAGOFFSET;

enum CreateNormalMap_Tags ENUM_TAGS
{
	CNM_IntBaseTag = CREATE_NORMAL_MAP_BASE + TAG_INT,
	CNM_HeightSource,			// (HS_Luminance)
	CNM_FilterWrapMode,		// (WM_Clamp)
	CNM_FilterWidth,			// (3) must be 1, 3, or 5
	CNM_DataType,				// (F3D_UINT8) one of { F3D_UINT8, F3D_UINT16, F3D_FLOAT16, F3D_FLOAT32 }

	CNM_DblBaseTag = CREATE_NORMAL_MAP_BASE + TAG_DOUBLE,
	CNM_ClampZ,					// (0.0) adjust normals so that:  newNormal = normalize(norm.X, norm.Y, max(norm.Z, clampZ))
	CNM_Scale,					// (1.0) scales the heightfield before computation of normals occurs

	CNM_PtrBaseTag = CREATE_NORMAL_MAP_BASE + TAG_PTR,
	CNM_ProgressOp,			// (NULL) an operator to which progress notifications will be given
	CNM_FusionDoc,				// (NULL) if you provide a FusionDoc pointer, during processing CreateNormalMap() will poll it to for flow processing aborts
};

inline const char *FuYesNo(bool yesIsTrue) { return yesIsTrue == true ? "yes" : "no"; }


extern FuSYSTEM_API Image *CreateNormalMap(Image *src, Tag firstTag = _TAG_DONE, ...);
extern FuSYSTEM_API Image *CreateNormalMapTagList(Image *img, TagList &tags);
extern FuSYSTEM_API void CreateNormalMap(Image *src, Image *dst, Tag firstTag = _TAG_DONE, ...);
extern FuSYSTEM_API void CreateNormalMapTagList(Image *img, Image *dst, TagList &tags);

extern FuSYSTEM_API MtlData3D *ConvertToMtlData(Number *data);


const int SortFloats_Radix8Threshold = 150;
const int SortFloats_Radix11Threshold = 150;	

struct FltSortItem
{
	union
	{
		float32 f;
		uint32 u;
	};

	void *d;			// userdata
};


// Sorts floats in increasing order in mostly linear time.   The sort is destructive in 2 ways:  the values of dataToSort are not 
//  preserved during the sort and sortedData.f are not valid.
extern FuSYSTEM_API void SortFloats(FltSortItem *sortedData, FltSortItem *dataToSort, int nItems);

extern FuSYSTEM_API const FuID DefaultMtlName;		// "Material1", "Material2", "Material3", ...

extern FuSYSTEM_API const FuID CLSID_Inputs3D;
extern FuSYSTEM_API const FuID CLSID_Data3D;
extern FuSYSTEM_API const FuID CLSID_Surface_Data;
extern FuSYSTEM_API const FuID CLSID_Surface_Inputs;
extern FuSYSTEM_API const FuID CLSID_Surface_Plane_Data;
extern FuSYSTEM_API const FuID CLSID_Surface_Plane_Inputs;
extern FuSYSTEM_API const FuID CLSID_Surface_Cube_Data;
extern FuSYSTEM_API const FuID CLSID_Surface_Cube_Inputs;
extern FuSYSTEM_API const FuID CLSID_Surface_Sphere_Data;
extern FuSYSTEM_API const FuID CLSID_Surface_Sphere_Inputs;
extern FuSYSTEM_API const FuID CLSID_Surface_Cylinder_Data;
extern FuSYSTEM_API const FuID CLSID_Surface_Cylinder_Inputs;
extern FuSYSTEM_API const FuID CLSID_Surface_Cone_Data;
extern FuSYSTEM_API const FuID CLSID_Surface_Cone_Inputs;
extern FuSYSTEM_API const FuID CLSID_Surface_Torus_Data;
extern FuSYSTEM_API const FuID CLSID_Surface_Torus_Inputs;
extern FuSYSTEM_API const FuID CLSID_Surface_Ico_Data;
extern FuSYSTEM_API const FuID CLSID_Surface_Ico_Inputs;
extern FuSYSTEM_API const FuID CLSID_Surface_Particle_Data;
extern FuSYSTEM_API const FuID CLSID_Surface_Particle_Inputs;
extern FuSYSTEM_API const FuID CLSID_Surface_Geometry_Data;
extern FuSYSTEM_API const FuID CLSID_Surface_Geometry_Inputs;

extern FuSYSTEM_API const FuID CLSID_ParamCg;
extern FuSYSTEM_API const FuID CLSID_ShadeNodeCg;
extern FuSYSTEM_API const FuID CLSID_ShaderCg;
extern FuSYSTEM_API const FuID CLSID_ContextCg;
extern FuSYSTEM_API const FuID CLSID_ShaderKeyCg;

extern FuSYSTEM_API const FuID CLSID_Geometry;
extern FuSYSTEM_API const FuID CLSID_ParticleGeometry;

extern FuSYSTEM_API const FuID CLSID_Surface_PointCloud_Inputs;
extern FuSYSTEM_API const FuID CLSID_Surface_PointCloud_Data;

extern FuSYSTEM_API const FuID CLSID_Surface_FBXMesh_Operator;
extern FuSYSTEM_API const FuID CLSID_Surface_FBXMesh_Data;
extern FuSYSTEM_API const FuID CLSID_Surface_FBXMesh_Inputs;

extern FuSYSTEM_API const FuID CLSID_Texture_Base_Inputs;
extern FuSYSTEM_API const FuID CLSID_Texture_Base_Data;
extern FuSYSTEM_API const FuID CLSID_Texture_Base_Impl_GL;
extern FuSYSTEM_API const FuID CLSID_Texture_Base_Impl_SW;

extern FuSYSTEM_API const FuID CLSID_Create_Texture_Inputs;
extern FuSYSTEM_API const FuID CLSID_Create_Texture_Data;
extern FuSYSTEM_API const FuID CLSID_Create_Texture_GL;
extern FuSYSTEM_API const FuID CLSID_Create_Texture_SW;
extern FuSYSTEM_API const FuID CLSID_Create_Texture_Operator;

extern FuSYSTEM_API const FuID CLSID_TexCatcherInputs;
extern FuSYSTEM_API const FuID CLSID_TexCatcherData;
extern FuSYSTEM_API const FuID CLSID_TexCatcherGL;
extern FuSYSTEM_API const FuID CLSID_TexCatcherSW;
extern FuSYSTEM_API const FuID CLSID_TexCatcher;	

extern FuSYSTEM_API const FuID CLSID_Light_Point_Operator;
extern FuSYSTEM_API const FuID CLSID_Light_Point_Inputs;
extern FuSYSTEM_API const FuID CLSID_Light_Point_Data;
extern FuSYSTEM_API const FuID CLSID_Light_Point_Impl_GL;
extern FuSYSTEM_API const FuID CLSID_Light_Point_Impl_SW;

extern FuSYSTEM_API const FuID CLSID_Light_Directional_Operator;
extern FuSYSTEM_API const FuID CLSID_Light_Directional_Inputs;
extern FuSYSTEM_API const FuID CLSID_Light_Directional_Data;
extern FuSYSTEM_API const FuID CLSID_Light_Directional_Impl_GL;
extern FuSYSTEM_API const FuID CLSID_Light_Directional_Impl_SW;

extern FuSYSTEM_API const FuID CLSID_Light_Ambient_Operator;
extern FuSYSTEM_API const FuID CLSID_Light_Ambient_Inputs;
extern FuSYSTEM_API const FuID CLSID_Light_Ambient_Data;
extern FuSYSTEM_API const FuID CLSID_Light_Ambient_Impl_GL;
extern FuSYSTEM_API const FuID CLSID_Light_Ambient_Impl_SW;

extern FuSYSTEM_API const FuID CLSID_Light_Spot_Operator;
extern FuSYSTEM_API const FuID CLSID_Light_Spot_Inputs;
extern FuSYSTEM_API const FuID CLSID_Light_Spot_Data;
extern FuSYSTEM_API const FuID CLSID_Light_Spot_Impl_GL;
extern FuSYSTEM_API const FuID CLSID_Light_Spot_Impl_SW;
extern FuSYSTEM_API const FuID CLSID_Light_Projector_Operator;
extern FuSYSTEM_API const FuID CLSID_Light_Projector_Inputs;

extern FuSYSTEM_API const FuID CLSID_MtlImpl_GL;						// base class for all fixed path OpenGL material implementations
extern FuSYSTEM_API const FuID CLSID_MtlImpl_SW;						// base class for all software renderer material implementations
extern FuSYSTEM_API const FuID CLSID_MtlImpl_GLSL;						// base class for all GLSL material implementations

extern FuSYSTEM_API const FuID CLSID_Operator3D;
extern FuSYSTEM_API const FuID CLSID_Transform3DOperator;

extern FuSYSTEM_API const FuID CLSID_Light_Operator;
extern FuSYSTEM_API const FuID CLSID_Light_Inputs;						// base class for all light inputs
extern FuSYSTEM_API const FuID CLSID_Light_Data;						// base class for all light data
extern FuSYSTEM_API const FuID CLSID_Light_Impl;						// base class for all light implementations
extern FuSYSTEM_API const FuID CLSID_Light_Impl_GL;					// base class for all fixed path OpenGL light implementations
extern FuSYSTEM_API const FuID CLSID_Light_Impl_SW;					// base class for all software renderer light implementations

extern FuSYSTEM_API const FuID CLSID_Shadow_Light_Data;
extern FuSYSTEM_API const FuID CLSID_Shadow_Light_Inputs;

extern FuSYSTEM_API const FuID CLSID_Merge3D_Operator;
extern FuSYSTEM_API const FuID CLSID_Transform3D;						// the tool, not the Transform3DOperator base class

extern FuSYSTEM_API const FuID CLSID_Locator3D;
extern FuSYSTEM_API const FuID CLSID_Locator_Inputs;
extern FuSYSTEM_API const FuID CLSID_Locator_Data;
extern FuSYSTEM_API const FuID CLSID_Surface_Locator_Inputs;
extern FuSYSTEM_API const FuID CLSID_Surface_Locator_Data;

extern FuSYSTEM_API const FuID CLSID_ImagePlane3D;
extern FuSYSTEM_API const FuID CLSID_Displace3D;
extern FuSYSTEM_API const FuID CLSID_Duplicate3D;
extern FuSYSTEM_API const FuID CLSID_Override;
extern FuSYSTEM_API const FuID CLSID_Shape3D;	
extern FuSYSTEM_API const FuID CLSID_Cube3D;

extern FuSYSTEM_API const FuID CLSID_Camera_Operator;
extern FuSYSTEM_API const FuID CLSID_Camera_Inputs;
extern FuSYSTEM_API const FuID CLSID_Camera_Data;

extern FuSYSTEM_API const FuID CLSID_MtlBlinnInputs;
extern FuSYSTEM_API const FuID CLSID_MtlBlinnData;
extern FuSYSTEM_API const FuID CLSID_MtlBlinnGL;
extern FuSYSTEM_API const FuID CLSID_MtlBlinnSW;
extern FuSYSTEM_API const FuID CLSID_MtlBlinn;

extern FuSYSTEM_API const FuID CLSID_MtlWardInputs;
extern FuSYSTEM_API const FuID CLSID_MtlWardData;
extern FuSYSTEM_API const FuID CLSID_MtlWardGL;
extern FuSYSTEM_API const FuID CLSID_MtlWardSW;
extern FuSYSTEM_API const FuID CLSID_MtlWard;				// the operator

extern FuSYSTEM_API const FuID CLSID_MtlPhongInputs;
extern FuSYSTEM_API const FuID CLSID_MtlPhongData;
extern FuSYSTEM_API const FuID CLSID_MtlPhongGL;
extern FuSYSTEM_API const FuID CLSID_MtlPhongSW;
extern FuSYSTEM_API const FuID CLSID_MtlPhong;				// the operator

extern FuSYSTEM_API const FuID CLSID_MtlMerge3D;
extern FuSYSTEM_API const FuID CLSID_MtlMerge_Inputs;
extern FuSYSTEM_API const FuID CLSID_MtlMerge_Data;
extern FuSYSTEM_API const FuID CLSID_MtlMerge_SW;
extern FuSYSTEM_API const FuID CLSID_MtlMerge_GL;

extern FuSYSTEM_API const FuID CLSID_Texture_Transform_Operator;
extern FuSYSTEM_API const FuID CLSID_Texture_Transform_Inputs;
extern FuSYSTEM_API const FuID CLSID_Texture_Transform_Data;
extern FuSYSTEM_API const FuID CLSID_Texture_Transform_Impl_GL;
extern FuSYSTEM_API const FuID CLSID_Texture_Transform_Impl_SW;

extern FuSYSTEM_API const FuID CLSID_TexGradientData;
extern FuSYSTEM_API const FuID CLSID_TexGradientInputs;
extern FuSYSTEM_API const FuID CLSID_TexGradientGL;
extern FuSYSTEM_API const FuID CLSID_TexGradientSW;
extern FuSYSTEM_API const FuID CLSID_TexGradient;

extern FuSYSTEM_API const FuID CLSID_Falloff_Operator;
extern FuSYSTEM_API const FuID CLSID_Falloff_Inputs;
extern FuSYSTEM_API const FuID CLSID_Falloff_Data;
extern FuSYSTEM_API const FuID CLSID_Falloff_Impl_GL;
extern FuSYSTEM_API const FuID CLSID_Falloff_Impl_SW;

extern FuSYSTEM_API const FuID CLSID_MtlFogData;
extern FuSYSTEM_API const FuID CLSID_MtlFogGL;
extern FuSYSTEM_API const FuID CLSID_MtlFogSW;
extern FuSYSTEM_API const FuID CLSID_MtlFog3D;

extern FuSYSTEM_API const FuID CLSID_SphereMapInputs;
extern FuSYSTEM_API const FuID CLSID_SphereMapData;
extern FuSYSTEM_API const FuID CLSID_SphereMapGL;
extern FuSYSTEM_API const FuID CLSID_SphereMapSW;
extern FuSYSTEM_API const FuID CLSID_SphereMap;

extern FuSYSTEM_API const FuID CLSID_MtlStereoMix3D;
extern FuSYSTEM_API const FuID CLSID_MtlStereoMix_Inputs;
extern FuSYSTEM_API const FuID CLSID_MtlStereoMix_Data;
extern FuSYSTEM_API const FuID CLSID_MtlStereoMix_SW;
extern FuSYSTEM_API const FuID CLSID_MtlStereoMix_GL;

extern FuSYSTEM_API const FuID CLSID_CameraStdOperator;				// this is the camera3D tool (want to change this to CameraStdOperator but the ID is already in use by 3D party plugins)
extern FuSYSTEM_API const FuID CLSID_CameraStdData;					// this is the data object associated with the Camera3D tool that gets stored in the scene

extern FuSYSTEM_API const FuID CLSID_Camera_Maya;
extern FuSYSTEM_API const FuID CLSID_Camera_Maya_Inputs;

extern FuSYSTEM_API const FuID CLSID_Curve_Data;
extern FuSYSTEM_API const FuID CLSID_Curve_Base_Inputs;
extern FuSYSTEM_API const FuID CLSID_Curve_Polyline;
extern FuSYSTEM_API const FuID CLSID_Curve_Shape;
extern FuSYSTEM_API const FuID CLSID_Curve_Shape_Data;

extern FuSYSTEM_API const FuID CLSID_Renderer;							// the tool (the individual renderers adds themselves as inputs)

extern FuSYSTEM_API const FuID CLSID_Renderer_OpenGL;
extern FuSYSTEM_API const FuID CLSID_Renderer_Mesa;
extern FuSYSTEM_API const FuID CLSID_Renderer_Fusion;
extern FuSYSTEM_API const FuID CLSID_Renderer_Software;
extern FuSYSTEM_API const FuID CLSID_Renderer_Advanced;
extern FuSYSTEM_API const FuID CLSID_Renderer_Opt_Software;
extern FuSYSTEM_API const FuID CLSID_Renderer_ShadowMap;

extern FuSYSTEM_API const FuID CLSID_GLTexture1D;
extern FuSYSTEM_API const FuID CLSID_GLTexture2D;
extern FuSYSTEM_API const FuID CLSID_GLTexture3D;
extern FuSYSTEM_API const FuID CLSID_GLTextureCube;

extern FuSYSTEM_API const FuID CLSID_RenderContext_Base;
extern FuSYSTEM_API const FuID CLSID_RenderContext_Software;
extern FuSYSTEM_API const FuID CLSID_RenderContext_OpenGL;

extern FuSYSTEM_API const FuID CLSID_ShaderARB;
extern FuSYSTEM_API const FuID CLSID_ShaderGLSL;
extern FuSYSTEM_API const FuID CLSID_GLTools3D;

extern FuSYSTEM_API const FuID CLSID_Local_Scene;
extern FuSYSTEM_API const FuID CLSID_Local_Scene_GL;
extern FuSYSTEM_API const FuID CLSID_Local_Scene_SW;

extern FuSYSTEM_API const FuID CLSID_SceneInfo;
extern FuSYSTEM_API const FuID CLSID_SceneInfoGL;
extern FuSYSTEM_API const FuID CLSID_SceneInfoSW;

extern FuSYSTEM_API const FuID CLSID_KdTree;


#endif

