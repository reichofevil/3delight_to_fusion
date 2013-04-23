#ifndef _BASE_DATA_3D_H_
#define _BASE_DATA_3D_H_

#include "3D_DataTypes.h"

class Operator3D;
class ManagedParam;
class ManagedParamBlock;

#define DATA_BASE	(OBJECT_BASE + TAGOFFSET)


/**
 * Tags for Data3D (future expandability).
 */
enum Data3D_Tags ENUM_TAGS
{
	D_IntBaseTag = TAG_INT | DATA_BASE,
};




/**
 * The base class for all the 'data' classes, like SurfaceData3D, CameraData3D, LightData3D, CurveData3D, MtlData3D.
 * Plugins which inject inputs into other Fusion tools will output a subclass of a Data3D object when they Process().
 */
class FuSYSTEM_API Data3D : public LockableObject
{
	FuDeclareClass(Data3D, LockableObject);

public:
	LockableObject ResourceListLock;
	std::vector<Resource3D*> ResourceList;	// for storing objects other than VBOs/textures/shaders that use GPU memory that need to persist from frame to frame

	NodeType3D DataType;							// subclasses set this to their nodetype on construction

	bool HasAABB;									// (false) surfaces/curves always have an AABB, lights/cameras may not have one
	bool AABBSet;									// (false)
	AABB3D AABB;

	Inputs3D *m_Inputs;							// The inputs that created this data.  May be NULL if this data was built programatically and not from the UI.
	Operator3D *m_Operator;						// the operator that create this data, may be NULL
	
	// TODO - put these into flags to save memory
	// These are the options as set in the GUI, the renderer will later decide if it actually need to render the object.
	bool IsVisible;								// (true) if its visible then the below flags apply, otherwise its completely invisible
	bool CullFrontFace;							// (false)
	bool CullBackFace;							// (false)
	bool IsUnseenByCamera;						// (false) still casts shadows, but not renderable
	bool IgnoreTransparentPixels;				// (true) transparent pixels (R=G=B=A=0) don't set aux channels

	bool IsAffectedByLights;					// (true) allows you to exclude individual nodes from the global lighting enable option
	bool UseTwoSidedLighting;					// (false)
	bool IsShadowCaster;							// (true)
	bool IsShadowReceiver;						// (true)

	bool IsCachable;								// (true) set to false if the geometry requires to be re-rendered each time the scene is viewed (eg. face camera particles)

	PFM_PolygonFillMode PolygonFillMode;	// (PFM_Filled) an override to force polys/quads/tris to be as points/lines/filled (valid for DataType = NDET_Surface/NDET_Curve)
	bool DoAntialiasing;							// (false) specified whether polygons should be rendered antialiased or not  (note: probably only want to use this for lines/points as OpenGL AA of polygons doesn't work very well)
	float32 LineThickness;						// (1.0f) specifies thickness of lines and diameter of points (valid for DataType = NDET_Surface/NDET_Curve)

	//bool IsOpaque;								// (true) only set this false if you have a vertex color stream with non-one alpha (this is for the geometry only, materials have a separate 'IsOpaque')

	bool IsMatteObject;							/**< (false) any objects whose pixels fall behind the matte objects pixels in Z do not get rendered */
	bool IsMatteOpaqueAlpha;					/**< (false) if true matte object pixels are (0,0,0,1) else (0,0,0,0) */
	bool IsMatteInfiniteZ;						/**< (true) if true the matte object pixels are located at z = Infinite */

	uint32 Flags;									// set by BuildFlags()

	ManagedParamBlock *m_ParamBlock;

	std::vector<Data3D*> SubData;				/**< the data generated by the subinputs */

	bool m_AccelInputToParam;					// (true)

	TagList Attrs;									/**< List of attributes for this data object.  Anything you want can be stored in this list.  Use SetAttrsTagList() to set in values. */	

	uint8 m_DataPad[16];

public:
	Data3D();
	Data3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	Data3D(const Data3D &toCopy);				/**< all subclasses must override the copy ctor. */
	virtual ~Data3D();

	/**
	 * Returns a copy of this object, NULL on failure.  All subclasses must override this method.  A typical implementation will 
	 * call the copy ctor to do the copy (eg. return new MyDataClass(*this) )
	 */
	virtual Data3D *CopyTagList(TagList &tags);
	Data3D *Copy(Tag firstTag = _TAG_DONE, ...);

	virtual void CopyCommonSettings(Data3D *toCopyFrom);

	// Builds a set flags used by the renderers to do faster chained-if testing
	virtual void BuildFlags();

	virtual void MapOut();
	virtual void MapIn();
	virtual void FreeTempData();

	/**
	 * Estimates the total amount of memory in bytes that this object is *currently* using.  Typically this is the size of this data object plus 
	 * memory used by the GeometryCache.  Overriding this method if the Data3D object is large can help the cache manager make better decisions.
	 */
	virtual uint64 EstimateMemoryUsage();

	/**
	 * Estimates the total amount of video memory in bytes that this object would use if it were loaded into video memory.  Some common 
	 * approximations:
	 *   meshes:         mem = 64 * numVertices + 4 * numIndices
	 *   material tree:  mem = sum of image memory usage over all materials in the material tree
	 */
	virtual uint64 EstimateVideoMemoryUsage();

	virtual void Use();
	virtual void Recycle();


	/**
	 * Gets the bounding box for this surface.  Calls BuildBoundingBox() internally.
	 */
	virtual bool GetBoundingBox(AABB3D &aabb);

	/**
	 * Sets the bounding box for this surface.  Subclasses may override this method to do their own bbox computations.  This method called
	 * internally by framework, use GetBoundingBox() instead.
	 */
	virtual bool BuildBoundingBox();

	/**
	 * Adds a resource to ResourceList.  If the resource is already in the list, nothing is done.
	 */
	virtual void CacheResource(Resource3D *res);

	/**
	 * Clears out the resources in ResourceList
	 */ 
	virtual void ClearResources();

	/**
	 * By default surfaces will return true and materials will return false.  Depending on the renderer type opaque geometry can be rendered 
	 * many times faster than transparent geometry and often renderers have special code paths for handling opaques separately.  For materials,
	 * you should always override this if you material has a chance of being opaque.  For surfaces you should return false if you have a per
	 * vertex color stream with non-one alpha.  TODO:  We might want to pass a rendercontext here so the material can decide based on renderer 
	 * settings if its transparent (eg. StereoMix material could examine left/right materials appropiately).
	 */
	virtual bool IsOpaque();

	/**
	 * Gets a certain subdata by its class ID.
	 */
	Data3D *GetSubData(const FuID &clsid);

	void Init();

	virtual bool DebugIsValid();

	ManagedParam *AddManagedParam(bool &param,     const char *name)	{ return AddManagedParam(&param, name, 1, PF_Bool  ); }
	ManagedParam *AddManagedParam(int &param,      const char *name)	{ return AddManagedParam(&param, name, 1, PF_Int   ); }
	ManagedParam *AddManagedParam(float &param,    const char *name)	{ return AddManagedParam(&param, name, 1, PF_Float ); }
	ManagedParam *AddManagedParam(Vector2f &param, const char *name)	{ return AddManagedParam(&param, name, 2, PF_Float ); }
	ManagedParam *AddManagedParam(Vector3f &param, const char *name)	{ return AddManagedParam(&param, name, 3, PF_Float ); }
	ManagedParam *AddManagedParam(Vector4f &param, const char *name)	{ return AddManagedParam(&param, name, 4, PF_Float ); }
	ManagedParam *AddManagedParam(double &param,   const char *name)	{ return AddManagedParam(&param, name, 1, PF_Double); }

	virtual ManagedParam *AddManagedParam(void *param, const char *name, int nComponents, uint32 flags);

	ManagedParamBlock *GetParamBlock()					{ return m_ParamBlock; }

	// If this was created from inputs and not programatically inputs will be passed in.
	virtual bool CreateManagedParams();
};



#endif