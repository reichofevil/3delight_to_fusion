#ifndef _REGISTRYLISTS_H_
#define _REGISTRYLISTS_H_

#ifndef DF_TAGS_ONLY
//#include "debug.h"
#include "Registry.h"
#include "HashList.h"
#include "DFScript.h"
#endif

// These are used to start Op-specific tags
// USER2 is for second-generation subclasses, etc

#define REG_USER		100
#define REG_USER2		200
#define REG_USER3		300
#define REG_USER4		400
#define REG_USER5		500

#define END_CLASS MAKEID('EndC')
#define ROOT_CLASS MAKEID('Root')

enum RegTags ENUM_TAGS
{
	REG_ClassType = TAG_INT,
	REG_HelpID,
	REG_OpIcon,
	REG_OpNoMask,
	REG_TileID,

	REG_CreateStaticPreview,				// Create one of these Previews at startup, & add to the app's GlobalPreviewList
	REG_Preview_CanDisplayImage,			// Various Preview modes. All default to FALSE.
	REG_Preview_CanCreateAnim,
	REG_Preview_CanPlayAnim,
	REG_Preview_CanSaveImage,
	REG_Preview_CanSaveAnim,
	REG_Preview_CanCopyImage,
	REG_Preview_CanCopyAnim,

	REG_Secret,									// value is the normal class type (NULL is default)
	REG_Version,
	REG_SeedCode,
	REG_OperatorControl,
	REG_Preview_CanRecord,
	REG_Preview_UsesFilenames,
	REG_Source_GlobalCtrls,
	REG_Source_SizeCtrls,
	REG_Source_AspectCtrls,
	REG_NoAutoProxy,

	REG_Logo,

	REG_Priority,

	REG_NoBlendCtrls,
	REG_NoObjMatCtrls,
	REG_NoMotionBlurCtrls,

	REG_NoAuxChannels,
	REG_EightBitOnly,

	REG_HelpTopicID,
	REG_CreateFramePreview,             // Create this Preview for every ChildFrame, & add to the frame's PreviewList

	REG_MainView,
	REG_ControlView,

	REG_Preview_CanNetRender,           // (default FALSE)

	REG_Particle_ProbabilityCtrls,      // (default TRUE)
	REG_Particle_SetCtrls,              // (default TRUE)
	REG_Particle_AgeRangeCtrls,         // (default TRUE)
	REG_Particle_RegionCtrls,           // (default TRUE)
	REG_Particle_RegionModeCtrls,       // (default TRUE)
	REG_Particle_StyleCtrls,            // (default FALSE)
	REG_Particle_EmitterCtrls,          // (default FALSE)
	REG_Particle_RandomSeedCtrls,       // (default TRUE)

	REG_Prefs_GlobalOnly,					// (default TRUE)

	REG_Source_DepthCtrls,					// (default FALSE)

	REG_ForceCommonCtrls,					// Use it to force Blend, Obj/Mat and Motion Blur control when CT_Modifier
	REG_NoCommonCtrls,						// (default FALSE) Can be overridden by REG_ForceCommonCtrls
	REG_NoScriptCtrls,						// (default FALSE)
	REG_ForceCommentCtrls,					// (default FALSE)
	REG_NoChannelCtrls,						// No RGBA checkboxes to pass through channels

	REG_Unpredictable,						// Now designates a tool that must be re-rendered, even if the inputs or current time haven't changed
	REG_TimeVariant,							// (default FALSE) This tool will produce different results when time changes, even if the inputs don't

	REG_ToolbarDefault,						// (default FALSE)
	REG_Hide,									// ClassRegistry->New() works, but it shouldn't be shown in any UI (default FALSE)

	REG_IsFeatureAvailable,

	REG_Param_NoSplineAnimation,        // Specify this if you *don't* want to be animatable by a BezierSpline
	REG_Param_CreateDefault,            // (default TRUE) Specifying this causes the param to be cached inside outputs/inputs/operators.  Other objects like images are cached in the cache manager, so they set this to false.
	REG_Param_Save,                     // default TRUE
	REG_Param_LocalCache,               // default TRUE
	REG_Param_UseCacheManager,          // default FALSE

	REG_SupportsDoD,							// default FALSE. Indicates that does not want automatic full image bounds DOD behaviour.
	REG_ShowInTimeline,						// Default to TRUE, unless REG_Hide is specified.

	REG_NoCtrlsToolbar,
	REG_NoSettingsButtons,					// Still gets the ControlWnd toolbar, just no settings buttons

	REG_Fuse_NoEdit,
	REG_Fuse_NoReload,

	REG_Source_ClippingCtrls,
	REG_NoPreCalcProcess,					// PreCalcProcess() just calls Process()
	
	REG_SupportsMultiLayer,					// default FALSE
	REG_NoLayerCtrls,

	REG_MaterialShortID,						// Each material plugin loaded is assigned a unique uint16 ID (1, 2, 3, ...) by fusion.  This value can also be found in MtlData3D::ShortID and MtlImpl3D::ShortID.
	REG_LightShortID,							// Each light plugin loaded is assigned a unique uint16 ID (1, 2, 3, ...) by fusion.  This value can also be found in LightData3D::ShortID and LightImpl3D::ShortID.
	REG_SupportsMaterialRenders,			// (false) Renderers set this tag to true to indicate they support rendering of materials in the material viewer.

	REG_Region_Supports_Contains,
	REG_Region_Supports_Intersect,
	REG_Region_Supports_RandPoint,
	REG_Region_Supports_ClosestPoint,
	REG_Region_Supports_GetCenter,
	REG_Region_Supports_GetVelocity,

	REGD_RequiresSubscription = TAG_DOUBLE | TAG_MULTI,

	REGS_FileName = TAG_STRING,         // full filename of dll/plguin containing this regnode
	REGS_ScriptName,
	REGS_Name,
	REGS_Category,
	REGS_OpIconString,
	REGS_OpDescription,
	REGS_OpToolTip,
	REGS_AboutDescription,					// This and the next 4 tags are optional, but can be
	REGS_VersionString,						// used to provide more specific information for the
	REGS_Company,								// automatic plugin about system
	REGS_URL,									// Without these, DF will use whatever default info it can find
	REGS_HelpFile,
	REGS_HelpTopic,					      // HTML Help File and Topic
	REGS_UIName,								// Name used in menus, etc. A tool will still get a name from REGS_Name

	REGP_New = TAG_PTR,
	REGP_Init,									// Pass a function pointer
	REGP_Cleanup,								// Pass a function pointer

	REGP_ShowHelp,								// Pass a function pointer

	REGP_ParentReg,
	REGP_ScriptInfo,

	REGP_TileBitmap,
	REGP_TileDIBCache,

	REGP_Resource_Handle,

	REGP_DataTypeArray,
	REGP_InputDataTypeArray,

	REGP_PrefsObj,

	REGP_ObjectSave,

	REGP_TileColor,							// Contains a ptr to a ResColor. Fg color is for tool, bg is for text.
	REGP_TileDetailColor,					// Contains a ptr to a ResColor. Fg color is for hilights, bg for shadows.
	REGP_BugReportFunc,						// Pass a BUGREPORTPTR

	REGP_RuntimeClass,						// ptr to CRuntimeClass object

	REGP_ParentID,								// ptr to ID of parent class. Ho Hum.
	
	REGP_LuaRegisterFunc,
	REGP_LuaMapTagFunc,
	REGP_ScriptRegisterFunc,
	REGP_ScriptMapTagFunc,

	REGP_InjectInputsFunc,					// ptr to an InjectInputsFunc for injecting inputs into 3D tools

	REGP_RequiresFeature = TAG_PTR | TAG_MULTI,
	REGP_Particle_RequiresRegionSupport,

	// ID tags
	REGID_ClassID = TAG_ID,
	REGID_DataType,
	REGID_MergeDataType,						// Indicates what DataType the merge tool merges. If not specified then CLSID_DataType_Image is assumed
	REGID_Parent,								// Set from REGPTR_ParentID at runtime
	REGID_InputDataType,

	REGID_Particle_DefaultRegion,
	REGID_Particle_DefaultStyle,

	REGID_LegacyClassID,

	REGID_MaterialLongID,					// An ID shared among all the classes in a material plugin assigned by the plugin developer that uniquely identifies the material.  A material plugin will consist of several unconnected classes: the inputs, the data, and the renderer specific implementations.  All these material classes must declare the same REGID_MaterialLongID in their registry, so that (for example) given a data object one can search the registry for its software renderer implementation
	REGID_LightLongID,						// An ID shared among all the classes in a light plugin assigned by the plugin developer that uniquely identifies the light.  A light plugin will consist of several unconnected classes: the inputs, the data, and the renderer specific implementations.  All these light classes must declare the same REGID_LightLongID in their registry, so that (for example) given a data object one can search the registry for its software renderer implementation


	// Object tags
	REGO_InputInjectorsList = TAG_OBJECT,			// a List<Registry> of regnodes of Inputs3D that want to inject inputs into this object
};

#define CT_Any                  0x00000000

#define CT_Operator             (1<<0)
#define CT_Filter               ((1<<1) | CT_Operator)
#define CT_Tool                 CT_Filter						// Synonym
#define CT_Modifier             ((1<<2) | CT_Operator)
#define CT_Source               (1<<3)
#define CT_FilterSource         (CT_Source | CT_Filter)
#define CT_Sink                 (1<<4)
#define CT_FilterSink           (CT_Sink | CT_Filter)
#define CT_Mask                 ((1<<5) | CT_Filter)
#define CT_Spline               ((1<<6) | CT_Modifier)
#define CT_Merge                (1<<7)
#define CT_MultiMerge           (1<<8)
#define CT_MergeTool            (CT_Merge | CT_Tool)
#define CT_MultiMergeTool       (CT_MultiMerge | CT_Tool)
#define CT_Converter            ((1<<9) | CT_Operator)
#define CT_3D                   (1<<10)
#define CT_3DFilter             (CT_3D | CT_Filter)
#define CT_3DFilterSource       (CT_3D | CT_FilterSource)
#define CT_3DInputs             ((1<<11) | CT_3D)

// Bits 16:31 are used for more types

// MX (mutually exclusive) types
#define MXTypeOffset            (16)							// Bit offset for extra mutally exclusive types
#define MXClassType(t)          ((t) << MXTypeOffset)		// Macro for MX types
#define MXTypeMask              (MXClassType(0xffff))		// 0x1FE00000 - Mask for MX types
#define GetMXClassType(ct)      ((ct) & MXTypeMask)
#define GetRegMXClassType(reg)  (GetMXClassType((reg)->m_ClassType))

#define CT_PaintTool           (MXClassType(1) | CT_Modifier)
#define CT_BrushShape           MXClassType(2)
#define CT_BrushMode            MXClassType(3)
#define CT_ApplyMode            MXClassType(4)

#define CT_AnimSegment          MXClassType(10)
#define CT_FlowType             MXClassType(11)
#define CT_Locale               MXClassType(12)
#define CT_PreviewMedia         MXClassType(13)
#define CT_LayoutItem           MXClassType(14)
#define CT_Transition           MXClassType(15)
#define CT_ToolViewInfo         MXClassType(16)

#define CT_ParticleStyle        MXClassType(17)
#define CT_ParticleTool        (MXClassType(18) | CT_Tool)
#define CT_ParticleMergeTool   (CT_Merge | CT_ParticleTool)
#define CT_ParticleSource      (CT_Source | CT_ParticleTool)

#define CT_Region3D             MXClassType(20)

#define CT_ScriptLib            MXClassType(21)

#define CT_Parameter            MXClassType(23)
#define CT_ImageFormat          MXClassType(24)
#define CT_View                 MXClassType(25)
#define CT_InputControl         MXClassType(26)
#define CT_PreviewControl       MXClassType(27)
#define CT_Preview              MXClassType(28)
#define CT_BinItem              MXClassType(29)
#define CT_ExternalControl      MXClassType(30)
#define CT_Event                MXClassType(31)
#define CT_EventControl         MXClassType(32)
#define CT_Protocol             MXClassType(33)
#define CT_Utility              MXClassType(34)
#define CT_Prefs                MXClassType(35)

#define CT_GLViewer             MXClassType(36)

#define CT_LightData3D         (MXClassType(37) | CT_3D)
#define CT_Light3D             (MXClassType(38) | CT_3DFilterSource)		// the operator, not the inputs
#define CT_MtlData3D           (MXClassType(39) | CT_3D)
#define CT_MtlParticle3D       (MXClassType(40) | CT_3D)
#define CT_MtlInputs3D         (MXClassType(41) | CT_3DInputs)
#define CT_BlendMode3D         (MXClassType(42) | CT_3D)
#define CT_CameraData3D        (MXClassType(44) | CT_3D)
#define CT_Camera3D            (MXClassType(45) | CT_3DFilterSource)		// the operator, not the inputs
#define CT_CurveData3D         (MXClassType(46) | CT_3D)
#define CT_Curve3D             (MXClassType(47) | CT_3D)
#define CT_SurfaceData3D       (MXClassType(48) | CT_3D)
#define CT_SurfaceInputs3D     (MXClassType(49) | CT_3DInputs)
#define CT_Renderer3D          (MXClassType(50) | CT_3D)
#define CT_RendererInputs3D    (MXClassType(51) | CT_3DInputs)
#define CT_RenderContext3D     (MXClassType(52) | CT_3D)
#define CT_Shader3D            (MXClassType(53) | CT_3D)
#define CT_FileFormat3D         MXClassType(54)
#define CT_ShadowClass3D       (MXClassType(55) | CT_3D)
#define CT_GLTexture            MXClassType(56)
#define CT_MtlImplSW3D         (MXClassType(57)	| CT_3D)
#define CT_MtlImplGL3D         (MXClassType(58) | CT_3D)
#define CT_LightImplSW3D       (MXClassType(60) | CT_3D)
#define CT_LightImplGL3D       (MXClassType(61) | CT_3D)
#define CT_Menu                 MXClassType(66)
#define CT_ConsoleUtility       MXClassType(67)
#define CT_ViewLUTPlugin        MXClassType(68)
#define CT_UserControl          MXClassType(69)
#define CT_LUTFormat            MXClassType(70)
#define CT_Material3D          (MXClassType(71) | CT_3DFilter)
#define CT_Texture3D           (MXClassType(72) | CT_3DFilter)
#define CT_CameraInputs	       (MXClassType(73) | CT_3DInputs)
#define CT_LightInputs	       (MXClassType(74) | CT_3DInputs)

#ifndef DF_TAGS_ONLY

#ifndef USE_NOSCRIPTING
typedef void (*LWREGFUNC)(lua_State *L);
typedef void (*LUAMAPTAGFUNC)(lua_State *L, int index, TagList &tags, FuID whatfor, bool totags);
#endif

#ifdef _STDAFX_H_
typedef void (*BUGREPORTPTR)(Registry *reg, CString &str);
#endif

struct DataTypeArray
{
#if defined(__INTEL_COMPILER) || _MSC_VER >= 1310
	FuID OutputID;
	FuID DataType;
#else
	const char *OutputID;
	const char *DataType;
#endif
	const char *Name;
};

// NOTE: This must be used in a file where there also exists a Registry
// object called "Reg".

#include "intel_cpu_dispatch.h"

#define _EXPAND(x) x
#define _JOIN(x,y) _EXPAND(x) ## _EXPAND(y)

//	SCRIPTCLASS(id, "", "");\

#define FuRegisterAbstractClassX(id, type, thisclass, baseclass, name)\
	const FuID thisclass::name##ClassID = id;\
	Registry thisclass::name##Reg(\
		&ClassRegistry, thisclass::name##ClassID,\
		REGP_Init, thisclass::RegInit,\
		REGP_Cleanup, thisclass::RegCleanup,\
		REG_ClassType, type,\
		REGS_VersionString, "Built: " __DATE__, \
		REGP_ParentID, &baseclass::ClassID,\
		REGP_ScriptInfo, &__ScriptInfo_##thisclass,

#define FuRegisterClassX(id, type, thisclass, baseclass, name)\
	static Object *name##_newfunc(const Registry *reg, const ScriptVal &table, const TagList &tags) { return (Object *)(new thisclass(reg, table, tags)); }\
	FuRegisterAbstractClassX(id, type, thisclass, baseclass, name)\
		REGP_New, name##_newfunc,

// #define FuRegisterAbstractClass(id, type) FuRegisterAbstractClassX(id, type, ThisClass, BaseClass,)
#define FuRegisterAbstractClass(id, type)\
	SCRIPTCLASS(id, "", "");\
	const FuID ThisClass::ClassID = id;\
	Registry ThisClass::Reg(\
		&ClassRegistry, ThisClass::ClassID,\
		REGP_Init, ThisClass::RegInit,\
		REGP_Cleanup, ThisClass::RegCleanup,\
		REG_ClassType, type,\
		REGS_VersionString, "Built: " __DATE__, \
		REGP_ParentID, &BaseClass::ClassID,\
		REGP_ScriptInfo, &_JOIN(__ScriptInfo_, ThisClass),

// #define FuRegisterClass(id, type) FuRegisterClassX(id, type, ThisClass, BaseClass,)
#define FuRegisterClass(id, type)\
	static Object *_JOIN(ThisClass, _newfunc)(const Registry *reg, const ScriptVal &table, const TagList &tags) { return (Object *)(new ThisClass(reg, table, tags)); }\
	FuRegisterAbstractClass(id, type)\
		REGP_New, _JOIN(ThisClass, _newfunc),



class FuSYSTEM_API RegistryList : public HashList
{
	FuDeclareClass(RegistryList, HashList); // declare ClassID static member, etc.

public:
	virtual Node *NextNode(Node *node) { return List::NextNode(node); }
	virtual Node *NextNode(Node *node, uint32 type);

	Registry *Find(const FuID &id, uint32 type = CT_Any);
	Node *FindNode(const FuID &id, uint32 type = CT_Any);

	void Init();
	void Cleanup();
	Object *New(const FuID &id, uint32 type, FusionDoc *doc = NULL);
	Object *New(const FuID &id, uint32 type, const ScriptVal &table, const TagList &tags);
	Object *New(const FuID &id, uint32 type, FusionDoc *doc, const ScriptVal &table, const TagList &tags);

	virtual Node *Add(Object *obj, const FuID &id);
};

FuSYSTEM_API extern RegistryList *ClassRegistry;

////////////////////////////////////////////////////////////////////////
// The Global Operator Array. Provides indexed translation from a system
// Command ID to Registry node pointers (TODO: Settings files later?)

extern FuSYSTEM_API const FuID CLSID_OpArray_Registry;
extern FuSYSTEM_API const FuID CLSID_OpArray_Script;
extern FuSYSTEM_API const FuID CLSID_OpArray_Setting;


class FuSYSTEM_API OperatorArrayEntry : public Object
{
public:
	FuID m_Type;
	FuID m_Name, m_ShortName;
	void *m_Ptr;

public:
	OperatorArrayEntry(const FuID &type, const char *name) { m_Type = type; m_Name = name; }
	OperatorArrayEntry(const char *entry);

	virtual bool Save(ScriptVal &entry);

	virtual void *GetPtr() { return m_Ptr; }
	virtual const char *GetName() { return m_Name; }
	virtual bool IsType(const FuID &type) { return m_Type == type; }
	virtual bool Match(const FuID &type, void *ptr, TagList *tags) { return (IsType(type) && m_Ptr == ptr);	}
};

class FuSYSTEM_API RegistryOperatorArrayEntry : public OperatorArrayEntry
{
public:
	RegistryOperatorArrayEntry(Registry *reg) : OperatorArrayEntry(CLSID_OpArray_Registry, reg->GetString(REGS_UIName, reg->m_Name))
	{
		m_ShortName = reg->GetString(REGS_OpIconString, NULL);
		m_Ptr = reg;
	}

	virtual bool Save(ScriptVal &entry)	
	{
#ifndef USE_NOSCRIPTING
		entry = ((Registry *)m_Ptr)->m_ID;
#endif
		return true;
	}
};

class FuSYSTEM_API OperatorArray : public LockableObject
{
	FuDeclareClass(OperatorArray, LockableObject); // declare ClassID static member, etc.

protected:
	int Size;
	int Used;

	OperatorArrayEntry **Array;

public:
	OperatorArray();
	~OperatorArray();

	void Cleanup();

	void Remove(uint16 id);
	uint16 Add(OperatorArrayEntry *oae);

	OperatorArrayEntry *FindEntry(uint16 id);
	OperatorArrayEntry *FindEntry(const FuID &type, const char *name, TagList *tags = NULL);
	OperatorArrayEntry *FindEntry(const FuID &type, void *ptr, TagList *tags = NULL);

	void *Find(const FuID &type, uint16 id);
	uint16 Find(OperatorArrayEntry *oae);
	uint16 Find(const FuID &type, void *ptr, TagList *tags = NULL);


	// Registry specific.  ToDo: Remove?
	/*
	uint16 Add(Registry *reg);
	Registry *Find(uint16 id);
	uint16 Find(Registry *reg);
	*/
};

FuSYSTEM_API extern OperatorArray FilterArray;

FuSYSTEM_API extern HashList *ImageFormatList;
#endif


// Various types that things seem to want to know explicitly :(
//REGTODO: Convert to "type.name" format, and update regnodes...
//REGTODO: Move to appropriate headers
extern FuSYSTEM_API const FuID CLSID_Format_Targa;
extern FuSYSTEM_API const FuID CLSID_Format_TAR;
extern FuSYSTEM_API const FuID CLSID_Format_Perception;
extern FuSYSTEM_API const FuID CLSID_Format_Hollywood;
extern FuSYSTEM_API const FuID CLSID_Format_PAR;
extern FuSYSTEM_API const FuID CLSID_Format_AVI;
extern FuSYSTEM_API const FuID CLSID_Format_DirectShow;
extern FuSYSTEM_API const FuID CLSID_Format_Raw;

extern FuSYSTEM_API const FuID CLSID_List;

extern FuSYSTEM_API const FuID CLSID_Input;
extern FuSYSTEM_API const FuID CLSID_Output;

extern FuSYSTEM_API const FuID CLSID_Operator_Macro;
extern FuSYSTEM_API const FuID CLSID_Operator_Group;
extern FuSYSTEM_API const FuID CLSID_Operator_LUT;
extern FuSYSTEM_API const FuID CLSID_Operator_Merge;
extern FuSYSTEM_API const FuID CLSID_Operator_PreviewSaver;
extern FuSYSTEM_API const FuID CLSID_Operator_RunCommand;
extern FuSYSTEM_API const FuID CLSID_Operator_Loader;
extern FuSYSTEM_API const FuID CLSID_Operator_Saver;
extern FuSYSTEM_API const FuID CLSID_Operator_Paint;
extern FuSYSTEM_API const FuID CLSID_Operator_TextPlus;
extern FuSYSTEM_API const FuID CLSID_Operator_Text3D;
extern FuSYSTEM_API const FuID CLSID_Operator_pRender;
extern FuSYSTEM_API const FuID CLSID_Operator_CustomDialog;
extern FuSYSTEM_API const FuID CLSID_Operator_AudioDisplay;
extern FuSYSTEM_API const FuID CLSID_Operator_UltraKeyer;

extern FuSYSTEM_API const FuID CLSID_Modifier_Path;
extern FuSYSTEM_API const FuID CLSID_Modifier_SineWave;

extern FuSYSTEM_API const FuID CLSID_Publish_Number;
extern FuSYSTEM_API const FuID CLSID_Publish_Point;
extern FuSYSTEM_API const FuID CLSID_Publish_Text;
extern FuSYSTEM_API const FuID CLSID_Publish_Gradient;
extern FuSYSTEM_API const FuID CLSID_Publish_Polyline;

extern FuSYSTEM_API const FuID CLSID_Mask_Triangle;
extern FuSYSTEM_API const FuID CLSID_Mask_Rectangle;
extern FuSYSTEM_API const FuID CLSID_Mask_Ellipse;
extern FuSYSTEM_API const FuID CLSID_Mask_Paint;
extern FuSYSTEM_API const FuID CLSID_Mask_Wand;
extern FuSYSTEM_API const FuID CLSID_Mask_Polyline;
extern FuSYSTEM_API const FuID CLSID_Mask_BSpline;
extern FuSYSTEM_API const FuID CLSID_Mask_Bitmap;

extern FuSYSTEM_API const FuID CLSID_PaintTool_Group;
extern FuSYSTEM_API const FuID CLSID_PaintTool_Airbrush;
extern FuSYSTEM_API const FuID CLSID_PaintTool_CopyPolyline;

extern FuSYSTEM_API const FuID CLSID_PaintApply_Stamp;
extern FuSYSTEM_API const FuID CLSID_PaintApply_RubThrough;
extern FuSYSTEM_API const FuID CLSID_PaintApply_Smear;
extern FuSYSTEM_API const FuID CLSID_PaintApply_Wire;
extern FuSYSTEM_API const FuID CLSID_PaintApply_Erase;
extern FuSYSTEM_API const FuID CLSID_PaintApply_Emboss;
extern FuSYSTEM_API const FuID CLSID_PaintApply_Color;
extern FuSYSTEM_API const FuID CLSID_PaintApply_Merge;

extern FuSYSTEM_API const FuID CLSID_LUT_Bezier;
extern FuSYSTEM_API const FuID CLSID_LUT_Cubic;

extern FuSYSTEM_API const FuID CLSID_Spline_Bezier;
extern FuSYSTEM_API const FuID CLSID_Spline_Cubic;
extern FuSYSTEM_API const FuID CLSID_Spline_NaturalCubic;
extern FuSYSTEM_API const FuID CLSID_Spline_NURB;
extern FuSYSTEM_API const FuID CLSID_Spline_B;

extern FuSYSTEM_API const FuID CLSID_IO_File;
extern FuSYSTEM_API const FuID CLSID_IO_Tape;
extern FuSYSTEM_API const FuID CLSID_IO_Boxx;
extern FuSYSTEM_API const FuID CLSID_IO_Memory;

extern FuSYSTEM_API const FuID CLSID_Preview_Window;
extern FuSYSTEM_API const FuID CLSID_Preview_SingleFile;
extern FuSYSTEM_API const FuID CLSID_Preview_Audio;
extern FuSYSTEM_API const FuID CLSID_Preview_QuickTime;
extern FuSYSTEM_API const FuID CLSID_Preview_GL;
extern FuSYSTEM_API const FuID CLSID_Preview_Tabasco;
extern FuSYSTEM_API const FuID CLSID_Preview_Quattrus;
extern FuSYSTEM_API const FuID CLSID_Preview_Altitude;
extern FuSYSTEM_API const FuID CLSID_Preview_Toaster;
extern FuSYSTEM_API const FuID CLSID_Preview_DPS;

extern FuSYSTEM_API const FuID CLSID_PreviewControl_Operator;
extern FuSYSTEM_API const FuID CLSID_PreviewControl_Polyline;
extern FuSYSTEM_API const FuID CLSID_PreviewControl_Mesh;
extern FuSYSTEM_API const FuID CLSID_PreviewControl_Angle;
extern FuSYSTEM_API const FuID CLSID_PreviewControl_Crop;
extern FuSYSTEM_API const FuID CLSID_PreviewControl_Crosshair;
extern FuSYSTEM_API const FuID CLSID_PreviewControl_Rectangle;
extern FuSYSTEM_API const FuID CLSID_PreviewControl_ImgOverlay;
extern FuSYSTEM_API const FuID CLSID_PreviewControl_Tracker;
extern FuSYSTEM_API const FuID CLSID_PreviewControl_Positioner;
extern FuSYSTEM_API const FuID CLSID_PreviewControl_Point;
extern FuSYSTEM_API const FuID CLSID_PreviewControl_Ellipse;
extern FuSYSTEM_API const FuID CLSID_PreviewControl_XYOffset;
extern FuSYSTEM_API const FuID CLSID_PreviewControl_Transform;
extern FuSYSTEM_API const FuID CLSID_PreviewControl_AutoRoto;
extern FuSYSTEM_API const FuID CLSID_PreviewControl_Light;
extern FuSYSTEM_API const FuID CLSID_PreviewControl_Camera;

extern FuSYSTEM_API const FuID CLSID_InputControl_Slider;
extern FuSYSTEM_API const FuID CLSID_InputControl_Screw;
extern FuSYSTEM_API const FuID CLSID_InputControl_Range;
extern FuSYSTEM_API const FuID CLSID_InputControl_Color;
extern FuSYSTEM_API const FuID CLSID_InputControl_Gradient;
extern FuSYSTEM_API const FuID CLSID_InputControl_Levels;
extern FuSYSTEM_API const FuID CLSID_InputControl_LevelsOutput;
extern FuSYSTEM_API const FuID CLSID_InputControl_ColorWheel;
extern FuSYSTEM_API const FuID CLSID_InputControl_ColorSuppression;
extern FuSYSTEM_API const FuID CLSID_InputControl_Histogram;
extern FuSYSTEM_API const FuID CLSID_InputControl_ColorRanges;
extern FuSYSTEM_API const FuID CLSID_InputControl_Offset;
extern FuSYSTEM_API const FuID CLSID_InputControl_Font;
extern FuSYSTEM_API const FuID CLSID_InputControl_Clip;
extern FuSYSTEM_API const FuID CLSID_InputControl_Checkbox;
extern FuSYSTEM_API const FuID CLSID_InputControl_Button;
extern FuSYSTEM_API const FuID CLSID_InputControl_MultiButton;
extern FuSYSTEM_API const FuID CLSID_InputControl_MultiButtonID;
extern FuSYSTEM_API const FuID CLSID_InputControl_Combo;
extern FuSYSTEM_API const FuID CLSID_InputControl_ComboID;
extern FuSYSTEM_API const FuID CLSID_InputControl_Image;
extern FuSYSTEM_API const FuID CLSID_InputControl_Pattern;
extern FuSYSTEM_API const FuID CLSID_InputControl_File;
extern FuSYSTEM_API const FuID CLSID_InputControl_FontFile;
extern FuSYSTEM_API const FuID CLSID_InputControl_CustomFilter;
extern FuSYSTEM_API const FuID CLSID_InputControl_TextEdit;
extern FuSYSTEM_API const FuID CLSID_InputControl_List;
extern FuSYSTEM_API const FuID CLSID_InputControl_ListID;
extern FuSYSTEM_API const FuID CLSID_InputControl_CheckList;
extern FuSYSTEM_API const FuID CLSID_InputControl_Library;
extern FuSYSTEM_API const FuID CLSID_InputControl_ClipList;
extern FuSYSTEM_API const FuID CLSID_InputControl_Spline;
extern FuSYSTEM_API const FuID CLSID_InputControl_Label;


extern FuSYSTEM_API const FuID CLSID_DataType_Image;
extern FuSYSTEM_API const FuID CLSID_DataType_Mask;
extern FuSYSTEM_API const FuID CLSID_DataType_Number;
extern FuSYSTEM_API const FuID CLSID_DataType_Point;
extern FuSYSTEM_API const FuID CLSID_DataType_Text;
extern FuSYSTEM_API const FuID CLSID_DataType_Polyline;
extern FuSYSTEM_API const FuID CLSID_DataType_BSplinePolyline;
extern FuSYSTEM_API const FuID CLSID_DataType_CardinalSplinePolyline;
extern FuSYSTEM_API const FuID CLSID_DataType_NURBSPolyline;
extern FuSYSTEM_API const FuID CLSID_DataType_UniformBSplinePolyline;
extern FuSYSTEM_API const FuID CLSID_DataType_TransformMatrix;
extern FuSYSTEM_API const FuID CLSID_DataType_Mesh;
extern FuSYSTEM_API const FuID CLSID_DataType_LookUpTable;
extern FuSYSTEM_API const FuID CLSID_DataType_Clip;
extern FuSYSTEM_API const FuID CLSID_DataType_MultiClip;
extern FuSYSTEM_API const FuID CLSID_DataType_FlowClip;
extern FuSYSTEM_API const FuID CLSID_DataType_Gradient;
extern FuSYSTEM_API const FuID CLSID_DataType_Particles;
extern FuSYSTEM_API const FuID CLSID_DataType_DisplayList;
extern FuSYSTEM_API const FuID CLSID_DataType_3D;
extern FuSYSTEM_API const FuID CLSID_DataType_MtlGraph3D;
extern FuSYSTEM_API const FuID CLSID_DataType_Audio;
extern FuSYSTEM_API const FuID CLSID_DataType_Histogram;
extern FuSYSTEM_API const FuID CLSID_DataType_StyledText;
extern FuSYSTEM_API const FuID CLSID_DataType_OldStyledText;
extern FuSYSTEM_API const FuID CLSID_DataType_ColorCurves;
extern FuSYSTEM_API const FuID CLSID_DataType_FuID;
extern FuSYSTEM_API const FuID CLSID_DataType_ScriptVal;

extern FuSYSTEM_API const FuID CLSID_RenderJob_Script;

extern FuSYSTEM_API const FuID CLSID_RenderQueue;

extern FuSYSTEM_API const FuID CLSID_BinItem_Clip;

extern FuSYSTEM_API const FuID CLSID_Message_Request;

extern FuSYSTEM_API const FuID CLSID_View_GL;
extern FuSYSTEM_API const FuID CLSID_View_Controls;
extern FuSYSTEM_API const FuID CLSID_View_Modifiers;
extern FuSYSTEM_API const FuID CLSID_View_Flow;
extern FuSYSTEM_API const FuID CLSID_View_SplineEditor;
extern FuSYSTEM_API const FuID CLSID_View_Timeline;
extern FuSYSTEM_API const FuID CLSID_View_Console;
extern FuSYSTEM_API const FuID CLSID_View_Transport;

extern FuSYSTEM_API const FuID CLSID_EventControl_Hotkey;

extern FuSYSTEM_API const FuID CLSID_OperatorInfo;

extern FuSYSTEM_API const FuID CLSID_ParticleStyle_NGon;
extern FuSYSTEM_API const FuID CLSID_ParticleStyle_Brush;
extern FuSYSTEM_API const FuID CLSID_ParticleStyle_Bitmap;
extern FuSYSTEM_API const FuID CLSID_ParticleStyle_Blob;
extern FuSYSTEM_API const FuID CLSID_ParticleStyle_Point;
extern FuSYSTEM_API const FuID CLSID_ParticleStyle_Line;
extern FuSYSTEM_API const FuID CLSID_ParticleStyle_Coopers;
extern FuSYSTEM_API const FuID CLSID_ParticleStyle_PointCluster;
extern FuSYSTEM_API const FuID CLSID_ParticleStyle_Geometry;


#endif
