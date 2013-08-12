#ifndef _3D_MATERIALBASE_H_
#define _3D_MATERIALBASE_H_

/** 
 * @file  3D_MaterialBase.h 
 * @brief base classes for materials
 */

#include "Types.h"
#include "Parameter.h"
#include "RegistryLists.h"

#include "3D_Material.h"					// for constants
#include "3D_BaseData.h"
#include "3D_Operator.h"
#include "3D_DataTypes.h"
#include "3D_BaseInputs.h"
#include "3D_RenderContextGL.h"

const FuID CLSID_MtlData3D			= "MtlData";
const FuID CLSID_MtlInputs3D		= "MtlInputs";
const FuID CLSID_MtlImpl3D			= "MtlImpl";					// base class for all material implementations
const FuID CLSID_MtlOperator3D	= "MtlOperator3D";

class MtlImplSW3D;
class ManagedParam;

#define MATERIAL_BASE	(OBJECT_BASE + TAGOFFSET)

enum Material_Tags ENUM_TAGS
{
	MTL_BaseIntTag = TAG_INT + MATERIAL_BASE,

	MTL_BasePtrTag = TAG_PTR + MATERIAL_BASE,
	MTL_RenderContext,			// pointer to a RenderContext3D
	MTL_MaterialData,				// pointer to a MtlData3D
};



enum MtlFlags
{
	MF_Enabled		= 0x00000001,	/**<  */
	MF_Required		= 0x00000002,	/**<  */
	MF_DiffX			= 0x00000004,	/**< x differential:  value at x + dx */
	MF_DiffY			= 0x00000008,	/**< y differential:  value at y + dy */

	MF_DiffXY      = MF_DiffX | MF_DiffY,
};



class ParamSW
{
public:
	FuID Name;
	CS_CoordSpace CoordSpace;
	void **Value;
	uint32 Flags;

public:
	ParamSW() : CoordSpace(CS_Unset), Value(NULL), Flags(0) {}
	~ParamSW() {}

	bool IsEnabled()  { return (Flags & MF_Enabled) != 0; }
	bool IsRequired() { return (Flags & MF_Required) != 0; }
	bool IsDiffX()		{ return (Flags & MF_DiffX) != 0; }
	bool IsDiffY()		{ return (Flags & MF_DiffY) != 0; }

	// Only enabled parameters will have their required flags set
	void SetRequired(bool req = true) 
	{
		if (req)
			Flags |= MF_Required;
		else
			Flags &= ~MF_Required;
	}

	void SetEnabled(bool enable = true) 
	{
		if (enable)
			Flags |= MF_Enabled;
		else
			Flags &= ~MF_Enabled;
	}
	
	void SetDiffX(bool isXDiff = true)
	{
		if (isXDiff)
			Flags |= MF_DiffX;
		else
			Flags &= ~MF_DiffX;
	}

	void SetDiffY(bool isYDiff = true)
	{
		if (isYDiff)
			Flags |= MF_DiffY;
		else
			Flags &= ~MF_DiffY;
	}
};



class ParamBlockSW
{
public:
	int32 Size;
	std::vector<ParamSW> Params;

public:
	ParamBlockSW() : Size(0) {}

	virtual ParamSW *CreateParam(Vector4f *&param, const char *name, CS_CoordSpace cs, uint32 flags = MF_Enabled)
	{
		int32 idx = Params.size();
		Params.push_back(ParamSW());

		ParamSW &p = Params.back();
		p.Name = name;
		p.CoordSpace = cs;
		p.Value = (void **) &param;
		p.Flags = flags;

		return &p;
	}

	virtual ParamSW *CreateParam(Vector3f *&param, const char *name, CS_CoordSpace cs, uint32 flags = MF_Enabled)
	{
		return CreateParam((Vector4f *&) param, name, cs, flags);
	}

	virtual ParamSW *CreateParam(Vector2f *&param, const char *name, CS_CoordSpace cs, uint32 flags = MF_Enabled)
	{
		return CreateParam((Vector4f *&) param, name, cs, flags);
	}
};






/**
 *  Renderer independent discription of the material.  MtlData3D are both data objects and also nodes in the material tree.  MtlData3D are 
 *  stored in the Scene3D object, but a renderer specific implementation of the material is created when it is time to render with it.  When 
 *  the renderer gets the MtlData3D object, it searches the registry for a matching implemenation and creates it.
 */
class FuSYSTEM_API MtlData3D : public Data3D
{
	FuDeclareClass(MtlData3D, Data3D);

public:
	class ChildSlot
	{
	public:
		ChildSlot()								{}
		~ChildSlot()							{ if (m_Material) m_Material->Recycle(); }

	public:
		FuID m_Name;							// eg. this will be "Diffuse" but not "Diffuse.R"
		MtlData3D *m_Material;				// can be NULL (eg. if Blinn.Diffuse.Material is not connected)
		uint32 m_NumComponents;				// eg. DiffuseColorMtl -> 4, SpecularIntensityMtl -> 1 
		ManagedParam *m_Param;				// if this child was created from a ManagedInput it will have an associated ManagedParam
	};

	ChildSlot *m_ChildSlots;				// child mtls will be stored here always and if they're created via ADD_MANAGED_CHILD() they'll also be stored in Data3D::m_ManagedParamList	
	int m_NumChildSlots;

	int32 MaterialID;							/**< User assigned Material ID (not guarenteed to be unique since the user can set it to anything).  If unset it defaults to -1.  Do not access this directly, use SetMaterialID() and GetMaterialID(). */

	FuID m_LongID;								/**< comes form REGID_MaterialLongID */
	uint16 m_ShortID;							/**< Each material plugin loaded is assigned a unique ID (1, 2, 3, ...) by fusion */

	bool ReceivesLighting;
	bool ReceivesShadows;
	bool UseTwoSidedLighting;				// flip the normals if they are facing away from the light

	//Operator3D *m_Operator;				// the operator this data was created by (if such exists)

	uint8 m_MtlDataPad[16];

public:
	MtlData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlData3D(const MtlData3D &toCopy);
	virtual ~MtlData3D();

	/**
	 * Returns a unique name used mostly for printing debug messages and asserts.  Default implementation returns m_RegNode->m_Name if its
	 * non-NULL and 'unknown material' otherwise (never returns NULL)
	 */
	const char *GetName();

	/**
	 * When a renderer is given a MtlData3D object and wants to construct its corresponding software or GL implemention it calls this
	 * function.  By default this function will search the registry for an implementation of the material.  This rather lengthy process
	 * can be avoided by providing an overide.  'type' is one of CT_MtlImplSW3D CT_MtlImplGL3D
	 */
	virtual const Registry *GetImplRegistry(uint32 type);

	/** 
	 * All plugin materials must implement the Copy() method.  Typical implementation: return new MyMaterialClass(*this)
	 */
	virtual Data3D *CopyTagList(TagList &tags); // throws CMemoryException

	/**
	 * Sets the material ID for this material.  The default implementation will set the value of MaterialID to id.
	 */
	virtual void SetMaterialID(int32 id, TagList *tags = NULL);

	/**
	 * Gets the material ID for this material.  -1 if its unset.  The default implementation will return MaterialID.
	 */
	virtual int32 GetMaterialID(TagList *tags = NULL);

	virtual bool CreateManagedParams();


	/**
	 * 
	 * @param slot Its illegal to specify a non-existant slot.   Slots may be created via AllocChildSlots().
	 */
	virtual ManagedParam *AddManagedChild(MtlData3D *&child, const char *name, int slot, int nComponents, uint32 flags);

	void AllocChildSlots(int nSlots);
	void FreeChildSlots();

	/**
	 * Utility function for converting the results of InMaterial->GetValue(req) which could return an Image/MtlGraph3D
	 * into a MtlData3D.  The returned MtlData3D must be recycled.
	 */
	virtual MtlData3D *ConvertToMtlData(Number *n);

	virtual void SetChildMtl(int slot, Number *n);
	virtual void SetChildMtl(int slot, MtlData3D *mtl);
	virtual MtlData3D *GetChildMtl(int slot);

	//virtual void AddManagedParam(void *param, const char *name, int nComponents, uint32 flags);

	/**
	 * Estimates the total amount of video memory in bytes that this materials textures would use (and any other video memory).
	 * This doesn't need to be overridden unless the material uploads a texture.  By default it calls EstimateChildVideoMemoryUsage().
	 */
	virtual uint64 EstimateVideoMemoryUsage();

	// typically called by EstimateVideoMemoryUsage()
	uint64 EstimateChildVideoMemoryUsage();

	/**
	 * This function is only used for ImagePlane3D to adaptively change its size based on the size of the image connected.
	 */
	virtual bool GetNaturalDims(float32 dims[4]);

	virtual void MapOut();
	virtual void MapIn();
	virtual void FreeTempData();

	// We might want to pass a rendercontext here so the material can decide based on renderer settings if its transparent (eg. StereoMix material
	// could examine left/right materials appropiately).
	virtual bool IsOpaque();

	// Something is either a material or a texture.  The general guideline is if it does lighting computations its a material.  All plugin materials
	// must override this so that the framework may distinguish between textures and materials.
	virtual bool IsTexture();

	virtual void Use();
	virtual void Recycle();
};






/** 
 *  Base class for all material implementations.  Material implementations are created by the renderer when it wants to render with a
 *  a specific MtlData3D.  Currently there are MtlImplSW3D and MtlImplGL3D that are implementations of the material that can be used with
 *  the software and OpenGL renderer respectively.
 *  @see MtlData3D
 *  @see MtlImplGL3D
 *  @see MtlImplSW3D
 */
class FuSYSTEM_API MtlImpl3D : public Object
{
	FuDeclareClass(MtlImpl3D, Object);

public:
	enum LightingFlags
	{
		LF_Ambient,
		LF_DiffuseAndSpecular,
		LF_Projector
	};			// lights that should be enabled

	MtlData3D *MtlData;						/**< the material that this object is an implementation of */

	MtlImpl3D *Base;							/**< the Base mtl of the MtlGraph that this mtl is a member of (setup by EnumerateNodes()) */
	MtlImpl3D *ParentMtl;					/**< pointer to parent material, NULL if this is the base material in the tree */
	std::vector<MtlImpl3D*> ChildMtls;	/**< refcounted array of child materials, pointers can be NULL (eg. for unconnected child mtls) */

	int32 NodeIndex;							/**< each node in the material tree has a unique index assigned to it immediately after construction */
	int32 NumMtlNodes;						/**< contains the number of nodes in the mtlgraph, valid only for base material after construction */

	int32 PreRenderCount;					/**< the number of times this material has been prerendered.  This is needed because multiple threads can be using this material at once.  DoPreRender() and DoPostRender() must inc/dec the count appropiately.  Note: only base material will have meaningful PreRenderCount. */
	int32 ActivateCount;						/**< the number of times this material has been activated.  This is needed because multiple threads can be using this material at once.  DoActivate() and DoDeactivate() must inc/dec the count appropiately.  Note: only base material will have meaningful ActivateCount. */
	LockableObject SetupLock;				/**< protects PreRenderCount and ActivateCount and TLS */

	bool ReceivesLighting;
	bool ReceivesShadows;
	bool UseTwoSidedLighting;				// nyi - TODO

	uint16 m_ShortID;
	uint32 MaterialID;

	uint8 m_MtlImplPad[16];

public:
	MtlImpl3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlImpl3D(const MtlImpl3D &toCopy);	// purposefully left unimplemented
	~MtlImpl3D();

	/**
	 * Returns a unique name used mostly for printing debug messages and asserts.  Default implementation returns m_RegNode->m_Name if its
	 * non-NULL and 'unknown material' otherwise (never returns NULL)
	 */
	virtual const char *GetName();

	virtual const char *GetToolName();

	/**
	 * Returns true if this is the base material in a material tree.
	 */
	bool IsBase() const { return ParentMtl == NULL; }

	/**
	 * Called on the base material immediately after the material tree has been constructed to setup (eg. NodeIndex, NumMtlNodes, etc).
	 */
	virtual void EnumerateNodes();

	/**
	 * Adds a child mtl at the given slot in the ChildMtls array.	Ownership of pointer is passed to MtlImpl3D
	 */
	virtual void SetChildMtl(int slot, MtlImpl3D *childMtl);

	virtual bool DebugIsValid(RenderContext3D &rc, TagList *tags = NULL);

	virtual uint32 GetLightingFlags(RenderContextSW3D &rc, TagList *tags);	// Not sure this is how we want things to work yet...

	virtual void Use();
	virtual void Recycle();
};






#define MATERIAL_INPUTS_BASE	(INPUTS_BASE + TAGOFFSET)

enum MtlInputs3D_Tags
{
	MI_IntBaseTag = MATERIAL_INPUTS_BASE | TAG_INT,
	MI_AddMaterialIDInputs,			/**< (true) Adds MaterialID inputs when true. */
	MI_ProcessMaterialIDInputs,	/**< (true) Puts values from object ID inputs into MtlData3D when true. */

	MI_PtrBaseTag = MATERIAL_INPUTS_BASE | TAG_PTR,
	//MI_OriginalDimensions,		// (NULL) (Process) - pass a float[4], stuffs originalwidth, originalheight, originalxscale, originalyscale into these

	MI_MultiIDBaseTag = MATERIAL_INPUTS_BASE | TAG_ID | TAG_MULTI,
};




/** 
 *  Base class for material inputs.  
 *  @see MtlOperator3D
 *  @see MtlData3D
 *  @see MtlImplGL3D
 *  @see MtlImplSW3D
 */
class FuSYSTEM_API MtlInputs3D : public Inputs3D
{
	FuDeclareClass(MtlInputs3D, Inputs3D);

public:
	Input *InMaterialIDNest;
	Input *InMaterialID;
	int32 MaterialID;					/**< the current material ID, -1 if unset */

	uint8 m_MtlInputsPad[16];

protected:
	virtual void OnAddToFlow(TagList *tags = NULL);
	virtual void OnRemoveFromFlow(TagList *tags = NULL);

public:
	/** 
	 * MtlInputs is a base class for inheriting material inputs from.  You do not construct one directly.
	 * @param reg
	 * @param table
	 * @param tags  must contain a (OBJP_Owner, Operator*) pair which determines which Operator the inputs are being added to
	 */
	MtlInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~MtlInputs3D();

	static bool RegInit(Registry *regnode);
	static void RegCleanup(Registry *regnode);

	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	/** Adds a single material ID slider.  This is called from AddInputsTagList(). */
	virtual Input *AddMaterialIDInputTagList(const FuID &name, const FuID &id, TagList &tags);
	Input *AddMaterialIDInput(const FuID &name, const FuID &id, Tag firsttag = _TAG_DONE, ...);

	virtual Data3D *CreateData(Request *req);

	/**
	 * Subclasses will override this to read the values from the material's inputs and output a MtlData object.
	 */
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);

	/** Reads the material ID inputs from the request and stores them in the MtlData3D object.  This is called by ProcessTagList(). */
	virtual void ProcessMaterialIDInputs(Request *req, MtlData3D *data);

	/**
	 * Adds the inputs to the Owner.  When overriding this method, you should call the base class version.  In particular, 
	 * MtlInputs3D::AddInputsTagList() will add the default set of object ID inputs.
	 **/
	virtual bool AddInputsTagList(TagList &tags);

	virtual void ShowInputs(bool visible);

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);

	virtual void Use();
	virtual void Recycle();

	/**
	 * Utility function for converting the results of InMaterial->GetValue(req) which could return an Image/MtlGraph3D 
	 * into a MtlData3D.  The returned MtlData3D must be recycled.
	 */
	virtual MtlData3D *ConvertToMtlData(Number *n);
};





/** 
 *  Base class for material tools.  
 *  @see MtlInputs3D
 *  @see MtlData3D
 *  @see MtlImplGL3D
 *  @see MtlImplSW3D
 */
class FuSYSTEM_API MtlOperator3D : public Operator3D
{
	FuDeclareClass(MtlOperator3D, Operator3D);

public:
	Output *OutMaterial;

	MtlInputs3D *MtlInputs;
	MtlInputs3D *MtlInputs22;

public:
	MtlOperator3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~MtlOperator3D();

	static bool RegInit(Registry *regnode);
	static void RegCleanup(Registry *regnode);

	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	virtual void Process(Request *req);
	virtual Data3D *CreateData(Request *req);
	virtual Data3D *ProcessData(Request *req, Data3D *data);
	virtual void PreCalcProcess(Request *req);

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);
	virtual void OnAddToFlow(TagList *tags = NULL);
	virtual void OnRemoveFromFlow(TagList *tags = NULL);
};









#endif
