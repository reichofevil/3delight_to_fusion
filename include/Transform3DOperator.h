#ifndef _TRANSFORM3DOPERATOR_H_
#define _TRANSFORM3DOPERATOR_H_

#include "3D_Operator.h"

class Scene3D;
class Node3D;
class MtlData3D;


enum Transform3DOperatorReg_Tags
{
	T3DREG_TranslationControls = TAG_INT + REG_USER,	// Default: TRUE
	T3DREG_RotationControls,									// Default: TRUE
	T3DREG_RotationPivotControls,								// Default: TRUE
	T3DREG_ScaleControls,										// Default: TRUE
	T3DREG_ScaleAxisControls,									// Default: FALSE
	T3DREG_TargetControls,										// Default: TRUE
	T3DREG_SceneInput,											// Default: TRUE
};

extern FuSYSTEM_API const FuID T3DOP_Inputs;			// Input group ID
extern FuSYSTEM_API const FuID T3DOP_TranslateX;	// Translation
extern FuSYSTEM_API const FuID T3DOP_TranslateY;
extern FuSYSTEM_API const FuID T3DOP_TranslateZ;
extern FuSYSTEM_API const FuID T3DOP_RotateX;		// Rotation
extern FuSYSTEM_API const FuID T3DOP_RotateY;
extern FuSYSTEM_API const FuID T3DOP_RotateZ;
extern FuSYSTEM_API const FuID T3DOP_PivotX;			// Rotation pivot
extern FuSYSTEM_API const FuID T3DOP_PivotY;
extern FuSYSTEM_API const FuID T3DOP_PivotZ;
extern FuSYSTEM_API const FuID T3DOP_ScaleX;			// Scale
extern FuSYSTEM_API const FuID T3DOP_ScaleY;
extern FuSYSTEM_API const FuID T3DOP_ScaleZ;
extern FuSYSTEM_API const FuID T3DOP_TargetX;		// Target Position
extern FuSYSTEM_API const FuID T3DOP_TargetY;
extern FuSYSTEM_API const FuID T3DOP_TargetZ;
extern FuSYSTEM_API const FuID T3DOP_RotOrder;		// Rotation order
extern FuSYSTEM_API const FuID T3DOP_ScaleLock;
extern FuSYSTEM_API const FuID T3DOP_UseTarget;

struct Transform3DData
{
	float64 Rx, Ry, Rz, Tx, Ty, Tz, Sx, Sy, Sz;
	bool LockScale;
	FuID RotationOrder;
	float64 RAx, RAy, RAz;
	bool UseTarget;
	float64 Tgx, Tgy, Tgz;

	Transform3DData() { Rx = Ry = Rz = Tx = Ty = Tz = Sx = Sy = Sz = RAx = RAy = RAz = Tgx = Tgy = Tgz = 0.0; LockScale = UseTarget = false; RotationOrder = ROID_XYZ; }
};

class FuSYSTEM_API Transform3DOperator : public Operator3D
{
	FuDeclareClass(Transform3DOperator, Operator3D);

public:
	Input *In3D;
	Output *Out3D;

	// model transforms
	Input *InRx, *InRy, *InRz, *InTx, *InTy, *InTz, *InSx, *InSy, *InSz;
	Input *InLockScaleXYZ, *InRotationOrder;
	Input *InRAx, *InRAy, *InRAz;
	Input *InUseTarget, *InTgLabel;
	Input *InTgx, *InTgy, *InTgz;
	Input *InXFormImport;

	Transform3DData Data;
	Matrix4 Target;

	Vector4 StartPos, StartRot;
	FuID StartRO;

	uint8 Pad[64];

protected:
	void SetTransformInputControls(const Registry *reg);
	Matrix4 GetTransformMatrix(Request *req);
	bool IsTransformInput(Input *in);
	void RebuildTransform(Input *mainin, Input *changedin, Parameter *param, TimeStamp time);

	/**
	 * Gets the next available number for use with an inputs LINK_Main.  The LINK_Main values need to be unique.  This function
	 * is not for outputs.  This function helps avoid duplicate LINK_Main values.  When a connection is dragged to another tool 
	 * it is connected to the lowest LINK_Main with compatibile datatypes.
	 * @param reserve if true the returned value is reserved and will not be returned again
	 */
	virtual int GetNextMainValue(bool reserve = true, TagList *tags = NULL);

	/**
	 * Reads the transform from the inputs on this tool and mutliplies it into the transform on the supplied root node.
	 */
	void ProcessTransform(Request *req, Node3D *root);

public:
	Transform3DOperator(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~Transform3DOperator();

	virtual void PreCalcProcess(Request *req);

	/**
	 * Default implementation will:
	 *  1) create a new scene with the root node provided by CreateScene()
	 *  2) call ProcessTransform() to assign a transform to the root node
	 *  3) the input scene from In3D is merged into the root node
	 *  4) call ComputeBoundingBox() on the root node
	 */
	virtual void Process(Request *req);

	/**
	 * Returns the new root node.
	 */
	virtual Node3D *CreateScene(Request *req, TagList *tags = NULL);

	static bool RegInit(Registry *regnode);
	static void RegCleanup(Registry *regnode);

	virtual void OnAddToFlow(TagList *tags = NULL);
	virtual void OnRemoveFromFlow(TagList *tags = NULL);

	virtual void Use();
	virtual void Recycle();

	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);

	// TODO: split these into a separate interface that anyone can expose
	virtual void StartTransform();								// call to fix starting position for pan/zoom/rotate
	virtual void Move(float64 x, float64 y, float64 z);
	virtual void Rotate(float64 x, float64 y, float64 z, const FuID &rotOrder = ROID_XYZ);

	// Returns the 'look' vector.  This is used for orienting the previewcontrols correctly.  For lights and cameras it is (0, 0, -1)
	// and for meshes it is (0, 0, 1).
	virtual Vector4 GetDirection();

	// Just to make sure there's an implementation, so that derived classes don't
	// skip this should we ever want to do something in these functions
	virtual void GetAvailableEvents(EventList *events, const FuID &eventid = NOID);
	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);
	virtual bool OnEvent(const FuID &eventid, uint_ptr subid, TagList &tags);
};

#endif
