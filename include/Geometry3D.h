#ifndef _GEOMETRY_3D_H_
#define _GEOMETRY_3D_H_

#include "LockableObject.h"

#include "3D_StreamSet.h"
#include "3D_DataTypes.h"
#include "3D_RenderContext.h"

class GeometryCache3D;






// Geometry3D:
//  A collection of vertices to be drawn as points, lines, triangles, or quads.  All primitives must be the same type and have the same material.
//  An surface with multiple materials must decompose itself into multiple Geometry3D, one for each material.  Each vertex is defined as a 
//  collection of attributes, such as position, color, normal, texture coordinates, and user defined attributes.  All plugin surfaces must be 
//  able to convert themselves upon demand to Geometry3D objects.  Note that Geometry3D objects are shared across threads and renderers and 
//  may not be modified after the read only flag has been set.

class FuSYSTEM_API Geometry3D : public LockableObject
{
	FuDeclareClass(Geometry3D, LockableObject);

public:
	PT_PrimitiveType PrimitiveType;				// (PT_Triangles) How the data is ordered into primitives - Points, line segments, line loop, triangles, quads
	int NumPrimitives;								// (0) The number of points/lines/tris/quads
	StreamSet3D Streams;								// must minimally contain position and normal, never modify an existing stream since they are shared objects

	union
	{
		struct
		{
			int16 PositionIdx;						// Indices into Streams where these vertex attributes are stored.  -1 if they don't exist.
			int16 NormalIdx;							// Are valid after Setup() is called.
			int16 ColorIdx;
			int16 TexCoord0Idx, TexCoord1Idx, TexCoord2Idx;
			int16 TangentUIdx;
			int16 TangentVIdx;
			int16 EnvCoordIdx;
		};

		int16 AttribIdx[HW_NumStreams];
	};

	IndexArray3D *Indices;							// (NULL) indexes into all the streams

	bool AABBSet;										// (false)
	AABB3D AABB;

	//int MtlIndex;									// A cube consisting of six separately material'd Geometry3D would have NumMtls = 6 and 
	//int NumMtls;										// MtlIndex = 0..5 for each of the Geometry3D.

	bool CullFrontFace;								// nyi - front facing means vertices wound ccw when projected onto the 2D screen plane	
	bool CullBackFace;								// nyi

	bool CallModify;									// (false) if true, a renderer will call Modify() before using this Geometry to render
	bool IsSetup;

	bool SortPolygons;								// (false) a hint to sort polygons when rendered in the view

	PFM_PolygonFillMode	PolygonFillMode;		// (PFM_Filled) an override to force polys/quads/tris to be as points, lines, filled
	bool DoAntialiasing;								// (false) specified whether polygons should be rendered antialiased or not (note: probably only want to use this for lines/points as OpenGL AA of polygons doesn't work very well)
	float32 LineThickness;							// (1.0f) specifies thickness of lines and diameter of points

	FuSafePtr<Geometry3D> ModifyCache;			// (NULL) Modify() can optionally cache its result here (its refcounted)

	bool IsCachable;

	TagList Attrs;

	// Each triangle/quad/polygon belongs to a smooth group.  Smooth group membership is used when determining per-vertex 
	// normals for lighting.  If m_Type is triangles/quads/polygons and no smooth group array is specified, all faces are assumed
	// to belong to the same smooth group.
	uint32 *SmoothGroup;								// (NULL)

	// Edges of triangles/quads/polygons are either interior edges or boundary edges.  Each vertex index has an edge flag
	// indicating if it is preceeding a boundary edge.  This is used when drawing wrireframe and may also be used for determining
	// antialiased edges.  The size of this array will always be m_NumIndices.  If m_Type is triangles/quads/polygons and no edgeflag
	// array is specified, then all edges are assumed to be boundary edges.
	bool *EdgeFlags;									// nyi

	// Materials -- are either defined per mesh or for individual faces.  If you define a multimaterial mesh, the renderers will later call
	//  SplitByMaterial() on it to split it up, so in general prefer providing per-geometry materials to per face materials when possible.
	int NumMtlSlots;									/**< (1) must be >= largest member in MtlIndices or 1 if MtlIndices is not valid */
	uint32 *MtlIndices;								// Indices into m_Materials defining a material per face, the size of this array is always NumPrimitives

	int32 ObjectID;									/**< This is the user-assignable ObjectID that gets rendered per pixel.  It is typically pulled from an ObjectID input and set when the geometry is first created. At geometry creation it is initialized to -1.  A value of -1 indicates it is unset. */

	bool IsReadOnly;									/**< geometry will be marked readonly by the 3D framework because it can be shared among many objects */

	LockableObject CacheListLock;
	std::vector<GeometryCache3D*> CacheList;	/**< list of caches this geometry is included in.  Must lock this geometry before accessing it. */

	uint8 m_GeometryPad[16];

public:
	// these used internally by the renderer
	uint32 GLPrimitiveType;							// one of GL_POINTS, GL_LINES, GL_TRIANGLES, GL_QUADS
	uint32 PolygonVertexCount;						// one of 1, 2, 3, or 4

	virtual void Init();								// Initializes all member variables to their default values
	void Free();										// Free's all the memory and resources associated with this object

public:
	Geometry3D();										// Doesn't do any initialization, you must call Init() manually
	Geometry3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	Geometry3D(const Geometry3D &toCopy);		// purposefully left unimplemented
	virtual ~Geometry3D();							// Calls Free()

	virtual void Use();
	virtual void Recycle();

	virtual uint64 EstimateMemoryUsage();

	virtual void MapOut();
	virtual void MapIn();
	virtual void FreeTempData();

	virtual bool DebugIsValid();					// for debugging purposes

	virtual bool Setup();							// used by renderer

	void GetBoundingBox(AABB3D &aabb);

	/**
	 * Create a new geometry with recomputed tangents, binormals, and normals.  Overwrites existing TBN if they already exist.
	 * @param doNormals           if true normals will be created
	 * @param doTangents          if true the tangent/binormal will be recomputed
	 * @param smoothingAngle      faces with angles in degrees smaller than this will have their adjoining edges smoothed across
	 * @param weightMethod        normal weighting method
	 * @param respectSmoothGroups if set to true two faces that have different smoothgroups will not be smoothed across
	 * @param doPreWeldPass       sometimes vertices are duplicated even though they have the same position causing normals to be miscomputed
 	 * @return                    a newly created geometry with the modified normals/tangents, the returned pointer must be eventually recycled
	 */
	Geometry3D *ComputeTBN(float32 smoothingAngle = 30.0f, NWM_WeightMethod weightMethod = NWM_ByAngle, bool doNormals = true, 
		bool doTangents = true, bool respectSmoothGroups = true, bool doPreWeldPass = true);

	/**
	 * Creates a new geometry with recomputed tangent vectors.
	 */
	Geometry3D *CreateTangents(float32 smoothingAngle = 30.0f, NWM_WeightMethod weightMethod = NWM_ByAngle, 
		bool respectSmoothGroups = true, bool doPreWeldPass = true);

	/**
	 * Replaces this geometry with newGeom in any caches currently referencing this geometry.
	 */
	void ReplaceCachedCopiesBy(Geometry3D *newGeom);
	void RemoveCacheRef(GeometryCache3D *cache);

	int NumVerticesPerPrimitive() { return ::NumVerticesPerPrimitive(PrimitiveType); }

	bool HasTangents() { return TangentUIdx != -1 && TangentVIdx != -1; }

	/**
	 * This is a full copy - no reference counting is done, even the streams and index arrays are duplicated
	 */
	virtual Geometry3D *Copy(bool shallow = false);
	virtual Geometry3D *CopyNoData();

	// Splits this Geomtry3D into a linked list of Geometry3D each with a single material.  The returned pointer can point back to this object
	// eg. if there is only one material.  The returned pointer should be recycled.
	virtual bool SplitByMaterial(std::vector<Geometry3D*> &geom);

	// If 'CallModify' is true, this will be called by the renderer to get a 'replacement' mesh to render with.  In this way, view
	// dependent meshes like camera aligned particles can be created.
	virtual Geometry3D *Modify(RenderContext3D &rc); // throws CMemoryException

	/**
	 * Normally a vertex in the geometry can be referenced by multiple indices.  Flatten() turns the geometry into one index per vertex.  It duplicates 
	 * the per vertex data in the mesh so that the index array becomes [0 1 2 3 4 5 6 ...].  This can increase memory usage.  This function can be
	 * useful when you want to preform an operation on the geometry that requires a vertex per index (eg. shrinking each primitive about its center).
	 * @return returns a new/modifiable copy of the geometry that should be recycled or passed on appropiately
	 */
	virtual Geometry3D *Flatten(); // throws CMemoryException

	/**
	 * Split quads into triangles.  
	 * @return the returned pointer must be eventually recycled
	 */
	Geometry3D *Triangulate();

	Stream3D *GetPosition()		{ return Streams[PositionIdx]; }
	Stream3D *GetNormal()		{ return Streams[NormalIdx]; }
	Stream3D *GetTexCoord()		{ return TexCoord0Idx != -1 ? Streams[TexCoord0Idx] : NULL; }
	Stream3D *GetColor()			{ return ColorIdx     != -1 ? Streams[ColorIdx]     : NULL; }
	Stream3D *GetTangentU()		{ return TangentUIdx  != -1 ? Streams[TangentUIdx]  : NULL; }
	Stream3D *GetTangentV()		{ return TangentVIdx  != -1 ? Streams[TangentVIdx]  : NULL; }
	Stream3D *GetEnvCoord()		{ return EnvCoordIdx  != -1 ? Streams[EnvCoordIdx]  : NULL; }

	// Returns first matching stream, -1 if not found.
	//int32 FindStream(SN_StreamName name, CS_CoordSpace cs) { return FindStream(StreamID(name, cs)); }
	//int32 FindStream(StreamID id);

protected:
	void BuildBoundingBox();
};


extern FuSYSTEM_API Geometry3D *MergeGeometry(const std::vector<Geometry3D*> &geomList);


class FuSYSTEM_API GeometryCache3D : public LockableObject
{
public:
	GeometryCache3D();
	GeometryCache3D(const GeometryCache3D &toCopy);		// nyi
	virtual ~GeometryCache3D();

	virtual void Update(Geometry3D *newGeom, Geometry3D *oldGeom);
	virtual void Add(Geometry3D *g);
	virtual void Replace(std::vector<Geometry3D*> &geom);
	virtual void GetAll(std::vector<Geometry3D*> &geom);
	virtual void Clear();
	virtual uint64 EstimateMemoryUsage();

protected:
	std::vector<Geometry3D*> Geometry;
	uint64 MemUsage;
};






#endif