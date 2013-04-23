#ifndef _3D_SURFACEBASE_H_
#define _3D_SURFACEBASE_H_

/** 
 * @file  3D_SurfaceBase.h 
 * @brief base classes for surfaces
 */

#include "Geometry3D.h"
#include "LockableObject.h"

#include "3D_Stream.h"
#include "3D_BaseData.h"
#include "3D_DataTypes.h"
#include "3D_BaseInputs.h"
#include "3D_MaterialBase.h"
#include "3D_RenderContext.h"
#include "3D_ShadowClasses.h"
#include "3D_BoundingVolume.h"

class Geometry3D;
class MtlInputs3D;
class SurfaceInputs3D;






/**
 * A surface data object is an abstract description of geometry that knows how to convert itself into renderable geometry.  For example, 
 * for a sphere it might include a center and radius value.  When the data object needs to be converted into actual renderable geometry 
 * GetGeometry() is called.  Typically, the data object is preserved down the pipeline and then converted into geometry at the renderer.  
 * However, some tools like bender/displace which require geometry ahead of time call GetGeometry() and then store the geometry in a 
 * SurfaceGeometry3D wrapper object.
 */
class FuSYSTEM_API SurfaceData3D : public Data3D
{
	FuDeclareClass(SurfaceData3D, Data3D);

public:
	GeometryCache3D GeometryCache;

	PCT_PolyClipType ClipType, QuickClipType;

	std::vector<int32> ObjectIDs;					/**< Contains objectIDs put here by SurfaceInputs::Process(), one for each (SBI_ObjectIDInputName, SBI_ObjectIDInputID) pair */

	bool m_IsParticles;								/**< (false) a hint for renderers who wish to treat particles in a special way */

	uint8 m_SurfaceDataPad[16];

public:
	SurfaceData3D();
	SurfaceData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	SurfaceData3D(const SurfaceData3D &toCopy);
	virtual ~SurfaceData3D();

	virtual void MapOut();
	virtual void MapIn();
	virtual void FreeTempData();

	virtual uint64 EstimateMemoryUsage();

	void Init();

	/** 
	 * All plugin surfaces must implement the Copy() method.  Typical implementation: return new MySurfaceClass(*this)
	 */
	virtual Data3D *CopyTagList(TagList &tags); // throws CMemoryException

	virtual void CopyCommonSettings(Data3D *toCopyFrom);

	/**
	 * Checks for a cached version of the geometry and if none is found, calls CreateGeometry().  If you intend to modify 
	 * the returned geometry you must set modifiable to true which cause new unshared Geometry3D objects to be created.
	 */
	virtual bool GetGeometryTagList(std::vector<Geometry3D*> &geometry, bool modifiable, TagList &tags);
	bool GetGeometry(std::vector<Geometry3D*> &geometry, bool modifiable = false, Tag firstTag = _TAG_DONE, ...);

	/**
	 * Override this to compute the bounding box.  This function should set the AABB and AABBSet members in the base class.  
	 * If you do not override this, the default implementation will be forced to generate the geometry for this surface and 
	 * loop over the vertices to compute the bbox by brute force.   The bounding box should be computed in the objects natural 
	 * coordinate system before any transformations have been applied.
	 */
	virtual bool BuildBoundingBox();

	virtual void Use();
	virtual void Recycle();

	/**
	 * This is called from with CreateGeometry() to create a new Geometry3D object.
	 */
	virtual Geometry3D *NewGeometry(); // throws CMemoryException
		
	/**
	 * Returns true by default.  Your return value should not take materials into account but rather this is for the situation where your
	 * surface has intrinsic transparency.  eg. particles have a vertex color stream with non-one alpha
	 */
	virtual bool IsOpaque();

	/**
	 * All subclasses must override CreateGeometryTagList() to create their geometry.  Always recreates the geometry even if 
	 * there is a cached version.  Never call this directly, use GetGeometry() since it can potentially avoid going through 
	 * the process of recreating the geometry.  
	 */
	virtual bool CreateGeometryTagList(std::vector<Geometry3D*> &geometry, TagList &tags); // throws CMemoryException
	bool CreateGeometry(std::vector<Geometry3D*> &geometry, Tag firstTag = _TAG_DONE, ...); // throws CMemoryException

	// SetupGeometry() should always called by CreateGeometry() as the last thing before returning.
	virtual bool SetupGeometry(std::vector<Geometry3D*> &geometry);

	virtual bool CacheGeometry(std::vector<Geometry3D*> &geometry);
	virtual void ClearGeometryCache();

	// Materials are stored in Node3D::Mtls.  Surfaces advertise how many materials they can take and the names of the slots but
	// have nothing to do with the actual materials.  eg. for Cube3D GetNumMtlSlots() would return 6 and GetMtlSlotName() would return 
	// "Front", "Right", "Left", "Bottom", "Top", "Back".  If you don't overide these functions GetNumMtlSlots() defaults to returning 1
	// and GetMtlSlotName() will return "Material1", "Material2", "Material3", ...  (note: not zero based as these values may appear in the UI
	// at some point in the future so we should use non-programmer normal person counting)
	virtual int GetNumMtlSlots();
	virtual const FuID GetMtlSlotName(int i);
};



#define SURFACE_INPUTS_BASE (INPUTS_BASE + TAGOFFSET)

/**
 * Pass these tags to SurfaceInputs3D::SetAttrs() or SurfaceInputs3D::AddInputs().  If they are passed to the ctor they will be ignored.
 */
enum SurfaceInputs3D_Tags ENUM_TAGS
{
	SI_IntBaseTag = SURFACE_INPUTS_BASE | TAG_INT,
	SI_AddVisibilityInputs,			/**< (true) Adds visibility inputs when true. */
	SI_ProcessVisibilityInputs,	/**< (true) Pulls values from visibility inputs into SurfaceData3D when true. */
	SI_AddLightingInputs,			/**< (true) Adds lighting inputs when true. */
	SI_ProcessLightingInputs,		/**< (true) Pulls values from lighting inputs into SurfaceData3D when true. */
	SI_AddMatteInputs,				/**< (true) Adds matte object inputs when true. */
	SI_ProcessMatteInputs,			/**< (true) Pulls values from matte object inputs into SurfaceData3D when true. */
	SI_AddWireframeInputs,			/**< (true) Adds wireframe inputs when true. */
	SI_ProcessWireframeInputs,		/**< (true) Pulls values from wireframe inputs into SurfaceData3D when true. */
	SI_AddObjectIDInputs,			/**< (true) Adds ObjectID inputs when true. */
	SI_ProcessObjectIDInputs,		/**< (true) Pulls values from ObjectID inputs into SurfaceData3D when true. */
	SI_AddBlendModeInputs,			/**< (true) Adds BlendMode inputs when true. */
	SI_ProcessBlendModeInputs,		/**< (true) Pulls values from BlendModes inputs into SurfaceData3D when true. */

	SI_PtrBaseTag = SURFACE_INPUTS_BASE | TAG_PTR,

	SI_MultiIDBaseTag = SURFACE_INPUTS_BASE | TAG_ID | TAG_MULTI,
	SI_ObjectIDInputName,			/**< Name appearing on the ObjectID input.  Must occur in pairs with SI_ObjectIDInputID. */
	SI_ObjectIDInputID,				/**< ID for the ObjectID input.  Must occur in pairs with SI_ObjectIDInputName. */
};



/**
 * Geometry creation tools may inherit this class and it will add the default inputs for visibility, lighting, shadows, wireframe, and objectID.
 */
class FuSYSTEM_API SurfaceInputs3D : public Inputs3D
{
	FuDeclareClass(SurfaceInputs3D, Inputs3D);

public:
	// Visibility Set
	Input *InVisibilityGroup;
	Input *InIsVisible;
	Input *InIsUnseenByCamera;
	Input *InCullFrontFace;
	Input *InCullBackFace;
	Input *InIgnoreTransparentPixels;

	// Lighting and shadows inputs set
	int32 OldIsAffectedByLights;
	Input *InLightingGroup;
	Input *InIsAffectedByLights;
	Input *InIsShadowCaster;
	Input *InIsShadowReceiver;

	// Wireframe inputs set
	Input *InIsWireframe;
	Input *InIsWireframeAntialiased;
	int32 WireframeAAShowCount;			// showing if >= 0

	// ObjectID inputs
	struct ObjInput
	{
		Input *In;
		int32 ObjectID;
		ObjInput(Input *in, int32 id) : In(in), ObjectID(id) {}
	};
	Input *InObjectIDGroup;
	std::vector<ObjInput> ObjectIDInputs;

	// Matte objects
	Input *InMatteGroup;
	Input *InIsMatteObject;
	Input *InIsMatteOpaqueAlpha;
	Input *InIsMatteInfiniteZ;	
	int MatteShowCount;

	// BlendMode inputs
	Input *InBlendModeGroup;

protected:
	/** Called when the inputs Owner has been added to the flow.  When overriding always call BaseClass::OnAddToFlow(). */
	virtual void OnAddToFlow(TagList *tags = NULL);

	/** Called when the inputs Owner has been removed from the flow.  When overriding always call BaseClass::OnRemoveFromFlow(). */
	virtual void OnRemoveFromFlow(TagList *tags = NULL);

public:
	SurfaceInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~SurfaceInputs3D();

	static bool RegInit(Registry *regnode);
	static void RegCleanup(Registry *regnode);

	virtual void Use();
	virtual void Recycle();

	/** 
	 * Sets the tags into Inputs3D::Attrs.  Instead of accessing Attrs directly, one should use this function because the class needs
	 * to be able to react to changes in its Attrs.
	 */
	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	/** Called when an inputs value has changed.  When overriding always call BaseClass::NotifyChanged(). */
	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);

	/** 
	 * Shows/hides the inputs.  Override this to show/hide the inputs in your subclass.  You should always call BaseClass::ShowInputs() 
	 * to allow the base class to show/hide any inputs it has.
	 */
	virtual void ShowInputs(bool visible);

	/**
	 * Adds the inputs to the Owner.  When overriding this method, you should call the base class version.  In particular, 
	 * SurfaceInputs3D::AddInputsTagList() will add the default set of visibility/lighting/wireframe/objectID inputs.
	 * @tags SetAttrsTagList() will be called on this tags to merge them into Inputs3D::Attrs
	 **/
	virtual bool AddInputsTagList(TagList &tags);

	virtual bool AddWireframeInputs(TagList *tags = NULL);
	virtual bool AddBlendModeInputs(TagList *tags = NULL);
	virtual bool AddVisibilityInputs(TagList *tags = NULL);
	virtual bool AddMatteInputs(TagList *tags = NULL);
	virtual bool AddLightingInputs(TagList *tags = NULL);
	virtual bool AddObjectIDInputs(TagList *tags = NULL);

	/** Adds a single objectID slider.  This is called from AddInputsTagList(). */
	virtual Input *AddObjectIDInputTagList(const FuID &name, const FuID &id, TagList &tags);
	Input *AddObjectIDInput(const FuID &name, const FuID &id, Tag firsttag = _TAG_DONE, ...);

	/** 
	 * Gets the inputs values from the request, stores them in a new SurfaceData3D object and returns that.  The returned SurfaceData3D pointer 
	 * should be recycled when you are done with it.  Subclasses should override this to do their own input specific processing.  It is important
	 * to call the baseclass version SurfaceInputs3D::ProcessTagList() which will read the values from the the visibility/lighting/wireframe/objectID
	 * inputs and set them into the data object.  A typical implementation will do this:
	 *    if (!data)
	 *			data = new MySurfaceClass;
	 *		BaseClass::ProcessTagList(req, data, tags);
	 *    ... now do processing for MySurfaceClass here ...
	 *    return data;
	 */
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);

	/** Reads the visibility inputs from the request and stores them in the data object.  This is called by ProcessTagList() if SI_ProcessVisibilityInputs is set. */
	virtual void ProcessSurfaceVisibilityInputs(Request *req, Data3D *data);

	/** Reads the lighting inputs from the request and stores them in the data object.  This is called by ProcessTagList() if SI_ProcessLightingInputs is set. */
	virtual void ProcessSurfaceLightingInputs(Request *req, Data3D *data);

	/** Reads the matte inputs from the request and stores them in the data object. */
	virtual void ProcessMatteInputs(Request *req, Data3D *data);

	/** Reads the wireframe inputs from the request and stores them in the data object.  This is called by ProcessTagList() if SI_ProcessWireframeInputs is set. */
	virtual void ProcessSurfaceWireframeInputs(Request *req, Data3D *data);

	/** Reads the objectID inputs from the request and stores them in the data object.  This is called by ProcessTagList() if SI_ProcessObjectIDInputs is set. */
	virtual void ProcessObjectIDInputs(Request *req, SurfaceData3D *data);
};

#endif
