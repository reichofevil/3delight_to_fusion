#ifndef _NODE3D_H_
#define _NODE3D_H_

#include "3D_AABB.h"
#include "3D_DataTypes.h"



class Data3D;
class MtlGraph3D;
class Geometry3D;
class LightData3D;
class CameraData3D;
class SurfaceData3D;


/**
 * A set of materials indexed by name and by slot number.  MtlSet3D is a ref counted shared object and consequently you should never modify 
 * a MtlSet3D on an existing Node3D unless you can be sure that that MtlSet3D is not shared.
 */
class FuSYSTEM_API MtlSet3D : public RefObject
{
public:
	class MtlInfo
	{
	public:
		FuID m_Name;
		MtlData3D *m_Mtl;					// refcounted
		int32 m_Slot;
	};

	int32 m_NextSlot;
	std::vector<MtlInfo> m_Mtls;				

public:
	MtlSet3D();
	MtlSet3D(const MtlSet3D &toCopy);		// nyi
	~MtlSet3D();

	MtlSet3D &operator=(const MtlSet3D &ms);	// nyi

	// When passing a material in you are passing ownership of the mtl pointer.  You should only add with replace = false when you
	// are absolutely sure that a material with the given name and slot doesn't previously exist.
	// @param name if its NOID a default value will be chosen (ie. "Material1", "Material2", "Material3", ...)
	// @return slot number
	int32 Add(MtlData3D *mtl, const FuID name = NOID, bool replace = false);
	//void AddMtl(int32 slot, MtlData3D *mtl, bool replace = false)

	// The returned material will have an extra use count put on it and must be Recycle()d when you are done with it.
	// The returned material can be NULL.
	MtlData3D *Get(const FuID &name);

	// returns the number of materials in this set
	int32 Size();

	// returns name of material at the specified index
	const FuID &GetName(int32 index);
	MtlData3D *Get(int32 index);

	// If newName is NOID then the old name is used
	void Replace(int32 index, MtlData3D *newMtl, const FuID newName = NOID);

	void Clear();

	// Shallow copy will just ref count the materials across
	MtlSet3D *Copy(bool deep = true);

	void AllocEmptySlots(int nSlots);

	//MtlData3D *GetMtl(int32 slot);

	//int32 GetSlot(const FuID &name);

	// gets the highest slot number in this set
	//int32 GetMaxSlot();

	virtual void Use();
	virtual void Recycle();
};





class FuSYSTEM_API Node3D
{
public:
	uint_ptr	ID;
	uint_ptr	SubID;
	FuID Name;

	NodeType3D NodeType;

	uint32 Flags;	

	// LocalAABB - the AABB in local coords of all geometry in this node and its children.  Local coordinates means before it has been transformed by this 
	//					nodes matrix M. 
	bool AABBSet;									// this can be false if (1) Scene::Setup() has not yet been called or (2) it is not meaningful for this node to have an AABB (eg. lights/cameras do not currently have valid AABBs or a merge node with no surface/curve children)
	AABB3D ModelAABB;								// Use SetBoundingBox() and GetBoundingBox() to access.  This bounding box only includes geometry/curves.  Lights/cameras do not contribute.  This is the bbox before it has been transformed by M.
	AABB3D WorldAABB;								// the AABB given in world space - this is just LocalAABB transformed by ModelToWorld

	bool IsRenderable;							// this is filled in when the scene reaches the renderer (basically its true if the object is a surface or curve)

	bool IsDirty;
	bool IsActive;

	Node3D *Next;
	Node3D *Prev;
	Node3D *Parent;
	Node3D *Child;

	FuPointer<Data3D> Data;						// the actual data stored in this node

	Matrix4f M;										// the transform matrix for this node - transforms vertices in this node into its parent's coordinate system
	Matrix4f ModelToWorld;						// valid after Scene3D::Setup() has been called.  note that world space is scene-relative (there is no real concept of a global worldspace in fusion because of the way the flow works)

	uint32 NodeIndex;								// an absolute indexing of all the nodes in the scene - can be used to match Nodes and LocalNodes

	bool ShowVertexNormals;						// set to true if you want to show the vertex normals (NDET_Surface nodes only)
	bool ShowFaceNormals;						// set to true if you want to show the face normals (NDET_Surface nodes only)

	bool PassThroughLights;

	//std::vector<Node3D*> LightList;
	FuPointer<MtlSet3D> Mtls;					// set of materials attached to this node (ref counted)
		
protected:
	void Init();
	Node3D *RecursiveCopy(Node3D *parentOfCopy);

public:
	~Node3D();
	Node3D();
	Node3D(const Node3D &n);					// Doesn't copy over the node connections (Next, Prev, Child, and Parent)
	Node3D(NodeType3D nodeType);
	Node3D(NodeType3D nodeType, uint_ptr id, uint_ptr subID);
	Node3D &operator=(const Node3D &n);
	Node3D *Copy();								// Returns a copy of this node and all of its subnodes.  The caller is responsible for calling Free() on the returned node.  Note: the method does not copy neighbors (Next, Prev)

	NodeType3D GetNodeType();

	void Free();									// Recursively frees this node and all its children.  Note: the function does not free Next/Prev neighbors

	Matrix4f	GetTransformMatrix();
	Matrix4f	GetTransformMatrixAll();		// returns matrix multiplied by all ancestors
	uint_ptr	GetID();
	uint_ptr	GetSubID();
	uint32	GetFlags();
	FuID    &GetName();
	bool		GetVisible();
	bool		GetBBoxVisible();
	bool		GetIsDirty();
	bool		GetIsActive();

	bool IsSurface()	{ return NodeType == NDET_Surface; }
	bool IsCurve()		{ return NodeType == NDET_Curve; }
	bool IsCamera()	{ return NodeType == NDET_Camera; }
	bool IsLight()		{ return NodeType == NDET_Light; }
	bool IsMerge()		{ return NodeType == NDET_Merge; }

	Node3D *GetNextPointer();
	Node3D *GetPrevPointer();
	Node3D *GetParentPointer();
	Node3D *GetChildPointer();
	Node3D *GetRootParent();

	Node3D *GetNextChildNode(Node3D *&node);
	Node3D *GetNextChildNode(Node3D *&node, NodeType3D nodeType);

	void SetTransformMatrix(const Matrix4f &m);
	void MultTransformMatrix(const Matrix4f &m);
	void SetID(uint_ptr id);
	void SetSubID(uint_ptr subid);
	void SetFlags(uint32 flags);
	void SetName(const char *name);			// name may be NULL
	void SetName(FuID &name);
	void SetVisible(bool visible);
	void SetBBoxVisible(bool bboxVisible);
	void SetIsDirty(bool isDirty);
	void SetIsActive(bool isActive);

	const AABB3D &GetModelAABB();

	void SetRenderableStatus();

	void AddNext(Node3D *node);
	void AddPrev(Node3D *node);
	void AddParent(Node3D *node);
	void AddChild(Node3D *node);				// adds node to the end of the child list
	void PrependChild(Node3D *node);			// adds node to the beginning of the child list

	void SetData(Data3D *data);				// sets this nodes data (data becomes owned by this node)
	Data3D *GetData();							// returns a (non-referenced counted) pointer to this node's data
	CameraData3D *GetCamera();
	LightData3D *GetLight();
	SurfaceData3D *GetSurface();

	void Swap(Node3D *n);						// Swaps this node with the provided node in the tree

	/**
	 * Finds the first parent merge node of this light that doesn't have "pass through lights" enabled.
	 */
	Node3D *GetLightTopNode();

	// Gets the geometry for this node if it is a surface or curve.  If modifiable is true, the geometry is copied before placing it into the array.
	// False is returned on failure.  If this node is not a surface or curve, then true is still returned.  The returned geometry must be recycled.
	bool GetGeometry(std::vector<Geometry3D*> &geometry, bool modifiable);

	// Gets the i'th geometry associated with this node.  There should be as many geom as materials.  Should this return true/false to differentiate
	// between failure and I don't have that geom? TODO
	Geometry3D *GetGeometry(int i, bool modifiable);

	int GetNumMtls();
	MtlData3D *GetMtl(int index);
	int32 AddMtl(MtlData3D *mtl, const FuID name = NOID, bool replace = false);
	void ClearMtls();

	bool AssignDefaultMaterial(FusionDoc *doc, const Color4f &diffuse = DefaultDiffuseColor, float32 opacity = 1.0f, const Color4f &specular = DefaultSpecularColor, 
		float32 specularExponent = DefaultSpecularExponent);

	bool DebugIsValid();

protected:
};

inline void Node3D::SetRenderableStatus()
{
	if (NodeType == NDET_Surface || NodeType == NDET_Curve)
		IsRenderable = true;
}



#endif