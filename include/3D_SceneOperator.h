#ifndef _SCENE_OPERATOR_3D_H_
#define _SCENE_OPERATOR_3D_H_

#include "3D_Operator.h"

class Node3D;
class Scene3D;
class MtlData3D;
class Geometry3D;







/**
 * SceneOperator3D - abstract base class for tools that (1) operate on an input scene and (2) output a modified scene,
 *   where the modification is restricted to the internals of Node3Ds (eg. the geometry or material within a Node3D are 
 *   modified) but the tree structure of the scene is left unchanged.
 */
class FuSYSTEM_API SceneOperator3D : public Operator3D
{
	FuDeclareClass(SceneOperator3D, Operator3D);

public:
	Input *In3D;
	Output *Out3D;

	bool Enabled;		/**< if Enabled is set to false in PreProcessScene() the tool is assumed to be disabled and the scene is just passed through */

	Node3D *OldNode, *NewNode;
	Scene3D *OldScene, *NewScene;

	std::vector<Node3D*> ToVisitOld;
	std::vector<Node3D*> ToVisitNew;

public:
	SceneOperator3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	SceneOperator3D(const SceneOperator3D &toCopy);				// nyi
	~SceneOperator3D();

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);

	/**
	 * This will call PreProcessScene(), ProcessScene(), and then PostProcessScene().  It is not typically overridden as the 
	 * default impl is usually sufficient.
	 */
	virtual void Process(Request *req);

	/**
	 * Override PreProcessScene() to:
	 *  - process the inputs from the request into class member variables
	 *  - allocate any memory used during ProcessScene()
	 *  - return false to cause this tool to fail
	 *  - setting Enabled to false to cause the tool to passthrough the input scene
	 */
	virtual bool PreProcessScene(Request *req);

	/**
	 * Override PostProcessScene() to cleanup any memory allocated in PreProcessScene().  PostProcessScene() will always be 
	 * called if PreProcessScene() has been.  The 'success' arg indicates whether the ProcessScene() succeeded or failed.
	 */
	virtual void PostProcessScene(bool success);

	/**
	 * This will call ProcessNode() repeatedly to build up the new scene from the old one.  It is not typically overridden 
	 * as the default impl is usually sufficient.
	 */
	virtual bool ProcessScene(Scene3D *newScene, Scene3D *oldScene);

	/**
	 * Given an oldNode and a newNode which is an exact copy of the oldNode, this callback allows one to change things in the newNode 
	 * (such as geometry or materials or bounding boxes)
	 */
	virtual bool ProcessNode(Node3D *newNode, Node3D *oldNode);
};








/**
 * SceneGeometryOperator3D - the base class for tools that take as input a scene and output a new scene with 
 *   the geometry in each node modified in some way.  eg. Bender, Displace, UVW
 */
class FuSYSTEM_API SceneGeometryOperator3D : public SceneOperator3D
{
	FuDeclareClass(SceneGeometryOperator3D, SceneOperator3D);

public:
	std::vector<Geometry3D*> Geometry;
	int GeometryIndex;

	bool ModifiableGeometryInProcess;	// (true) if set to false, the geometry received in ProcessGeometry() should be treated as unmodifiable (this is an optimization to prevent an extra geometry copy)

public:
	SceneGeometryOperator3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	SceneGeometryOperator3D(const SceneGeometryOperator3D &toCopy);				// nyi
	~SceneGeometryOperator3D();

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);

	virtual bool PreProcessScene(Request *req);
	virtual bool ProcessScene(Scene3D *newScene, Scene3D *oldScene);
	virtual void PostProcessScene(bool success);

	/**
	 * The default impl gets the old geometry and calls ProcessGeometry() on it.  Not typically overridden.
	 */
	virtual bool ProcessNode(Node3D *newNode, Node3D *oldNode);

	/**
	 * Subclasses must override this to produce their output geometry.  Know that:
	 *  - you can access the node that geom came from by OldNode
	 *  - the geometry passed in is by default modifiable.  So you would do one of two things (1) modify 'geom' in place, Use() it 
	 *    and return it, or (2) create a new Geometry3D object and return that.
	 *  - if you change the bbox of the geometry you can optionally update the NewNode's bbox (it will be automatically updated if you don't)
	 */
	virtual Geometry3D *ProcessGeometry(Geometry3D *geom);
};









/**
 * SceneMaterialOperator3D - the base class for tools that take as input a scene and output a new scene with 
 *   the material in each node modified in some way.  eg. Fog, SoftClip, ReplaceMaterial
 */
class FuSYSTEM_API SceneMaterialOperator3D : public SceneOperator3D
{
	FuDeclareClass(SceneMaterialOperator3D, SceneOperator3D);

public:

public:
	SceneMaterialOperator3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	SceneMaterialOperator3D(const SceneMaterialOperator3D &toCopy);				// nyi
	~SceneMaterialOperator3D();

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);

	virtual bool PreProcessScene(Request *req);
	virtual bool ProcessScene(Scene3D *newScene, Scene3D *oldScene);
	virtual void PostProcessScene(bool success);

	/**
	 * The default impl gets the old materials and calls ProcessMaterial() on each of them.  Not typically overridden.
	 */
	virtual bool ProcessNode(Node3D *newNode, Node3D *oldNode);

	/**
	 * Subclasses must override this to produce their output material.  Know that:
	 *  - you can access the node that oldMaterial came from by OldNode
	 *  - if you are just tacking on a new root node to the material graph you can ref count across the old children nodes
	 */
	virtual MtlData3D *ProcessMaterial(MtlData3D *oldMaterial);
};


#endif