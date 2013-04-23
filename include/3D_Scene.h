#ifndef _SCENE_3D_H_
#define _SCENE_3D_H_

#include "Parameter.h"

#include "3D_Node.h"
#include "3D_DataTypes.h"
#include "3D_Allocator.h"

class KdTree;
class BSPTree;
class MtlData3D;
class SceneInfo3D;


struct MtlEntry3D
{
	MtlData3D *MtlData;
	MtlEntry3D *Next;
	uint32 NodeIndex;
	uint32 MtlIndex;
};

struct NodePair
{
	NodePair(Node3D *a, Node3D *b) : A(a), B(b) {}
	Node3D *A;
	Node3D *B;
};




class FuSYSTEM_API Scene3D : public Parameter
{
	FuDeclareClass(Scene3D, Parameter);

public:
	bool IsInitialized;
	bool IsSetup;

	TimeStamp Time;								// the time of the request that this scene was created to fullfill

	int32 Proxy, NoData;
	int32 Quality, RealQuality;

	uint64 MeshVideoMemory;						// amount of GPU memory that would be used if all meshes were uploaded to the card at the same time in bytes
	uint64 MaterialVideoMemory;				// amount of GPU memory that would be used if all materials were uploaded to the card at the same time in bytes
	uint64 LargestMaterialVideoMemory;		// amount of GPU memory that would be used if the largest material was uploaded to the card in bytes

	int NumNodes;									// the number of nodes in this scene

	std::vector<Node3D*> NodeList;
	std::vector<uint32> LightList;			// nodes that contain an *enabled* light
	std::vector<uint32> ProjectorList;		// nodes that contain an *enabled* projector
	std::vector<uint32> GeomList;				// nodes that contain geometry - points, lines, curves, surfaces
	std::vector<MtlEntry3D*> MtlList;		// mtls in this scene (with duplicates removed)

	StructAllocator3D<MtlEntry3D> MEA;

	BSPTree *BSPCache, *QuickBSPCache;		// protected by BSPLock, use CacheBSP() and GetCachedBSP()
	LockableObject BSPLock;

	Node3D *RootNode;								// The root node of the scene node heirarchy

	bool IsDone;

	std::vector<Data3D*> DataList;			// list of resources used by the scene, filled out when Done() is called

	Node3D *DefaultLightNode;					// valid after Done() is called

	LONG UID;

//protected:
	void Free();

	LockableObject SceneInfoLock;
	std::vector<SceneInfo3D*> SceneInfo;

public:
	Scene3D(FusionDoc *doc, Request *req);
	Scene3D(const Registry *reg, const ScriptVal &table, const TagList &tags);		// if you use this ctor, you must immediately call Init() after
	Scene3D(Scene3D &scene);	
	virtual ~Scene3D();

	virtual void Init(FusionDoc *doc, TimeStamp time, bool hiQ, int32 proxy, bool precalc);
	virtual void Init(FusionDoc *doc, Request *req);

	static const Scene3D Get(Input *in, Request *req) { return *(Scene3D *) in->GetValue(req); }

	virtual Parameter *Copy();
	void Copy(const Scene3D &a);
	Scene3D& operator=(const Scene3D &a);

	virtual bool CopyAttr(Tag tag, void *addr) const;
	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);

	//virtual SceneInfo3D *ObtainSceneInfo();
	//virtual void ReleaseSceneInfo(SceneInfo3D *si);

	uint64 EstimateMemoryUsage();			// estimates the amount of RAM (in bytes) currently in use by this scene

	// auxillary methods to get camera nodes
	int	GetNumberOfCameras();
	bool	GetCamera(int index, Node3D **camera, Node3D **transform = NULL);

	Node3D *GetNextNode(Node3D *&node);
	Node3D *GetNextNode(Node3D *&node, NodeType3D nodeType);

	bool CountNodeType(Node3D *node, NodeType3D nodeType, int &count);
	bool GetNodeType(Node3D *node, NodeType3D nodeType, int &index, Node3D **rnode, bool &done);

	Node3D *GetNodeByID(Node3D *node, uint_ptr id, uint_ptr subid = 0);
	Node3D *GetNodeByName(Node3D *node, const char *name);

	bool CalculateAABBs();						// recalculates the AABBs, can fail (eg) if a surface/curve fails to construct its geometry

	Node3D *GetRootNode();

	void SetRootNode(Node3D *rootNode);

	// When done building a scene, you must call Done() to allow the scene to any post processing it may have to do.
	virtual bool Done();

	virtual void MapIn();
	virtual void MapOut();
	virtual void FreeTempData();

	// Does a bunch of precomputations and stores their results in the scene.  Specifically, this function:
	//  - builds a list of light nodes and the nodes that they affect
	//  - traverses the node tree and concatentates matrices to build up the model and model inverse transpose matrices for each node
	//  - builds the AABB heirarchy
	virtual bool SetupTagList(TagList &tags);
	bool Setup(Tag firstTag = _TAG_DONE, ...);

	virtual bool DebugIsValid();		// Setup() must be called first

	void CacheBSP(BSPTree *tree);
	BSPTree *GetCachedBSP(bool isQuick);			// must recycle the returned value

	KdTree *GetCachedKdTree();

	//// Updates the bounding volumes for each node, starting at the leaves.  This should only be done under a SetupLock.
	//virtual bool BuildBVHierarchyTagList(RenderContext3D &rc, TagList &tags);
	//bool BuildBVHierarchy(RenderContext3D &rc, Tag firstTag = _TAG_DONE, ...);

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(Scene3D);
#endif

protected:
	static LONG NextUID;
	virtual bool _IsValid();
	bool CalculateAABBsRecursive(Node3D *n);
	void CalculateVideoMemoryUsage();
};


// An iterator object used for traversing the scene.
// Usage:
//   SceneItor itor;
//   Node3D *n = itor.BeginTraversal(scene);
//   while (n)
//			n = itor.NextNode();
//   itor.EndTraversal();
// Notes:
//  - BeginTraversal/EndTraversal must occur in matched pairs
//  - BeginTraversal/NextNode return NULL if there are no further nodes to visit
//  - the iterator will always visit the nodes in the scene in the same pattern that a LocalSceneItor would visit the nodes in the corresponding SceneInfo
//  - calling NextNode() after BeginTraversal() returns NULL will blow up the iterator - this is not checked on purpose for speed reasons - no need for double checking

class SceneItor3D
{
public:
	std::vector<Node3D*> ToVisit;

public:
	// Visits all the nodes in the scene including the root node.  May return NULL.
	Node3D *BeginTraversal(Scene3D *scene)
	{
		Node3D *root = scene->RootNode;
		if (root)
		{
			if (root->Next)
				ToVisit.push_back(root->Next);
			if (root->Child)
				ToVisit.push_back(root->Child);
		}

		return root;
	}

	// Visits the supplied node and all its children.  The supplied node must not be NULL.
	Node3D *BeginTraversal(Node3D *node)
	{
		if (node->Child)
			ToVisit.push_back(node->Child);
		return node;
	}

	Node3D *NextNode()
	{
		Node3D *n = NULL;
		
		if (!ToVisit.empty())
		{
			n = ToVisit.back();
			ToVisit.pop_back();

			if (n->Next)
				ToVisit.push_back(n->Next);
			if (n->Child)
				ToVisit.push_back(n->Child);
		}

		return n;
	}

	void EndTraversal()
	{
		ToVisit.clear();
	}
};


// Use for duplicating a scene node by node, but with the option to change the contents of a node at each step in the traversal.
// Adding/removing nodes is not possible using this iterator.
class FuSYSTEM_API SceneDuplicator3D
{
public:
	std::vector<Node3D*> ToVisitOld;
	std::vector<Node3D*> ToVisitNew;

	Node3D *OldNode;
	Node3D *NewNode;
	
	// BeginTraversal() or NextNode() will set this to TRUE if they fail, if it fails should should Recycle() the new scene.  This flag doesn't
	// get cleared until BeginTraversal() is called again.
	bool CopyFailed;		

public:
	SceneDuplicator3D();
	SceneDuplicator3D(const SceneDuplicator3D &toCopy);		// purposefully left unimplemented
	~SceneDuplicator3D();

	// Returns the old node.  NULL is returned if the oldScene doesn't have a root node.
	Node3D *BeginTraversal(Scene3D *newScene, const Scene3D *oldScene);

	// Returns a pointer to the new node, where you can modify the NodeType and Data.  Because the new scene is being constructed on the fly,
	// you cannot trust the nodes Child/Next pointers to be valid, but it Parent ptr should be fine.
	Node3D *GetNewNode() { return NewNode; }
	Node3D *GetOldNode() { return OldNode; }

	// Advances to the next node and returns the node from the oldScene, NULL if traversal is done (can also return NULL on failure, 
	// but you can check CopyFailed == true for this)
	Node3D *NextNode();

	// EndTraversal() must be paired with BeginTraversal().  EndTraversal() can be called at any time, even if the last node has not been reached.
	// Setting error to true causes CopyFailed to be set true.  EndTraversal() can be called several times with no ill effects.
	void EndTraversal(bool error = false);
};




// Attempts to matches nodes between two scenes at different times.  This is non-trivial because of any tool may modify the scene
// in unexpected ways (eg. Duplicate3D/Bender3D) such as adding/removing/coalescing nodes.  The returned array 'AToB' is used thus:
//   indexToNodeB = AToB[nodeA->NodeIndex] or 0xffffffff if the match couldn't be found.
extern FuSYSTEM_API uint32 *BuildMatchArray(const Scene3D *sceneA, const Scene3D *sceneB);
extern FuSYSTEM_API void FreeMatchArray(uint32 *matchArray);






/* TODO - commented this out for now, when it later gets uncommented you need to figure out a way pass a FusionDoc ptr into the Scene3D ctor
class AutoScene3D : public AutoParameter
{
	FuDeclareClass(AutoScene3D, AutoParameter); // declare ClassID static member, etc.

public:
	AutoScene3D()	{ Param = new Scene3D; }
	AutoScene3D(Input *in, TimeStamp time) : AutoParameter(in, time) {}
	operator Scene3D *()					{ return (Scene3D *)Param; }
};
*/



#endif