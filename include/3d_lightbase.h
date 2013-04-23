#ifndef _3D_LIGHTBASE_H_
#define _3D_LIGHTBASE_H_

#include "Operator.h"
#include "Transform3DOperator.h"

#include "3D_BaseData.h"
#include "3D_DataTypes.h"
#include "3D_BaseInputs.h"

class NodeInfo;
class Frustum3D;
class ShaderGLSL;
class LightOperator3D;
class RenderContext3D;
class BoundingVolume3D;
class RenderContextGL3D;
class RenderContextSW3D;
class ShadowLightData3D;
class ShadowLightInputs3D;



// These are the built in light control types
typedef enum LightControl_LightTypes
{
	LCT_Ambient,
	LCT_Directional,
	LCT_Point,
	LCT_Spotlight,
	LCT_Projector,
	LCT_Env,
	LCT_AO,
};


#define LIGHT_BASE	(OBJECT_BASE + TAGOFFSET)

enum Light_Tags ENUM_TAGS
{
	LGTI_BaseIntTag = TAG_INT + LIGHT_BASE,

	LGTP_BasePtrTag = TAG_PTR + LIGHT_BASE,
	LGTP_RenderContext,				// pointer to a RenderContext3D
	LGTP_LightData,					// pointer to a LightData3D
};






class FuSYSTEM_API LightData3D : public Data3D
{
	FuDeclareClass(LightData3D, Data3D);

public:
	// You'll note that we have things like PenumbraAngle and Direction in this base class but they're not used by all lights.
	// Originally they were in their respective classes but we decided to move them into this base class so plugin exporters/importers
	// could access them.  

	bool IsEnabled;						// (true) if disabled this light will not affect any objects within the scene, if enabled it will affect all surfaces below its TopNode

	Color4f Color;							// (1, 1, 1) color of the light (Color.A is ignored)

	Vector3f Position;					// (0, 0, 0) position of light in model space, must always be (0, 0, 0) -- use Node3D::M to position the light
	Vector3f Direction;					// (0, 0, -1) direction of light in model space, must always always be (0, 0, -1) -- use Node3D::M to orient the light

	float32 ConstantDecay;				// (1.0) These determine the attenuation of the light.  If you are at a distance 'd' away from the light:
	float32 LinearDecay;					// (0.0)    attenuationFactor = 1.0 / (c + l * d + q * d * d)
	float32 QuadraticDecay;				// (0.0) where c, l, q are the constant, linear, quadratic decay factors.

	// Fusions spotlight model is:
	//   float32 innerAngle = DegToRad(0.5 * ConeAngle)
	//   float32 cosInner   = cos(innerAngle)
	//   float32 cosOuter   = cos(innerAngle + DegToRad(PenumbraAngle))
	//   float32 spotFactor = pow(max(0, (cos(theta) - cosOuter) / (cosInner - cosOuter)), Dropoff)

	float32 ConeAngle;					// (40.0) the inner cone angle of a spotlight in degrees between one edge of the cone to the other side
	float32 PenumbraAngle;				// (1.0) softness angle of a spotlight in degrees -- if the cone is 40 degrees and the penumbra is 10 degres, the total angle subtended by the cone will be 60 = 10 + 40 + 10
	float32 Dropoff;						// (1.0) how the light falls off between the inner/outer cones

	ShadowLightData3D *ShadowData;	// (NULL)

	uint16 LightID;						// uniquely identifies this light class
	uint32 m_UserID;						// assigned by the user in the UI (eg. Projector ID for projectors)
	float32 m_ProjectorPriority;		// (0.0) a priority used to determine which projectors get projected ontop of others when using a Catcher3D tool in Blend mode

	//Operator *LightOperator;  - what is this for?

	uint8 m_LightDataPad[16];

	float32 env_multi;
	float32 env_samples;
	char env_map;
	const char* envPathFile;
	int env_occ;
	float32 angle;
	Color4f Diffuse;


public:
	//LightData3D();
	LightData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightData3D(const LightData3D &toCopy);
	virtual ~LightData3D();

	//bool Init();

	virtual Data3D *CopyTagList(TagList &tags);

	// By default this will search the registry for an implementation of the light.  This rather lengthy process can be avoided by providing an
	// overide, how getting it right is a bit tricky.  'type' is one of CT_LightImplSW3D CT_LightImplGL3D
	virtual const Registry *GetImplRegistry(uint32 type);

	// If the plugin light is ambient or contains an ambient contribution override this method.  It should return true
	// if the light contains an ambient contribution, and false otherwise.  The renderer uses this method to consolidate
	// contributions from all the ambient lights in the whole scene and render them all in one lighting computation instead
	// of doing separate lighting computations for each light.  Note: ambient light can be thought of as a global color that
	// is added to the per-pixel color of all surfaces
	virtual bool GetAmbientContribution(Color4f &c);

	virtual void Hack_QueryShadowParams(Matrix4f &lightView, float32 &fovY) {}

	virtual bool IsLightProjector();
};






// This is the base class for all light implemenations.  A light implementation can be thought of as a light belonging to 
// a specific renderer.
class FuSYSTEM_API LightImpl3D : public Object
{
	FuDeclareClass(LightImpl3D, Object);

public:
	// These are variables that the renderer uses and should not be directly accessed by the implementation (but we cannot
	// use private/friends because future plugin renderers may need to access them)
	LightData3D *LightData;							// the light data object that this object is an implementation of

	NodeInfo *LightNode;							// the node in the localscene that contains this light

	//int32 LightIndex;											// the renderer fills this in (anyway it wants)


	// If shadowing is disabled or there are no objects that receive shadows, then the light is turned into a non-shadow light
	// and all the litnodes are stored here.  If shadowing is enabled, this list is empty.
	//std::vector<NodeInfo*> LitNodes;					// the nodes lit by the light

	// On the GUI there are the following options:
	//  nodes that are unseen by the camera - not rendered when doing the main material pass
	//  nodes that cast shadows - used when rendering into the shadowbuffer (might be invisible to the camera)
	//  nodes that receive shadows - used when you do the material pass masked with the shadowbuffer

	// Each individual renderer may choose to use the 'CastShadows' variable in different ways.
	bool CastsShadows;	

	// hacked in shadow stuff
	Color4f ShadowColor;								// alpha is meaningless

	uint32 Flags;
	
	std::vector<NodeInfo*> LitNodes;				// the nodes lit by this light/projector

	// These are used to test for matching pairs of Activate/Deactivate, Implement/DeImplement, and PreRender/PostRender.
	// Note that if Activate() fails, then the matching Deactivate() will not get called, ie. Activate() must do its
	// own cleanup when it fails.  The same applies for PreRender/PostRender and Implement/Free.   These are used by the renderer.
	// TODO  bools -> bits
	bool ActivateCalled;
	bool PreRenderCalled;
	//bool ImplementCalled;

	BoundingVolume3D *LightCamBV;		// a volume that encloses intersection(light area of affect, camera frustum)
	BoundingVolume3D *ShadowBV;		// a volume that encloses the area where any present objects could cast shadows into the camera frustum

	uint16 LightID;

public:
	LightImpl3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightImpl3D(const LightImpl3D &toCopy);									// purposefully left unimplemented
	virtual ~LightImpl3D();

	virtual void ComputeLightCamBV(RenderContext3D &rc, Frustum3D *cameraFrustum);
	virtual void ComputeShadowBV(RenderContext3D &rc, Frustum3D *cameraFrustum);

	virtual const char *GetName();			// the name this light appears by in the class registry
	virtual const char *GetToolName();		// the name of the tool that produced this light

	/**
	 * Returns true if this light is a texture projector.  Note that a texture projector is not the same as a light that projects an image
	 * as colored light.  The catcher material only recognizes texture projectors.  By default IsTextureProjector() will return false. 
	 * Subclasses that are texture projectors should override this to return true.
	 */
	virtual bool IsTextureProjector();
	virtual bool IsLightProjector();

	// Init/Free are a matched pair of functions called internally by the renderer to init/free any renderer specific information 
	// attached to the light.  (eg. shadowbuffers, lists of shadow casters/receivers, nodes affected by this light)
	// I can't think of any reason a non-base implementation class would ever want to override this (they should be using
	// Implement/DeImplement instead.)
	virtual void Init();
	virtual void Free();
};





class FuSYSTEM_API LightInputs3D : public Inputs3D
{
	FuDeclareClass(LightInputs3D, Inputs3D);

public:
	// These are a set of standard controls used by most lights.  When you subclass LightOperator3D to create your own
	// light plugin, you may call AddStandardLightControls() in your constructor to add these inputs.
	Input *InEnabled;
	Input *InIntensity;
	Input *InRed, *InGreen, *InBlue;

	bool StandardInputsAdded;

	ShadowLightInputs3D *ShadowInputs;			// if this is non-NULL, the light is potentially a shadow caster

public:
	LightInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightInputs3D(const LightInputs3D &toCopy);
	virtual ~LightInputs3D();

	static bool RegInit(Registry *regnode);
	static void RegCleanup(Registry *regnode);

	virtual void OnAddToFlow(TagList *tags = NULL);
	virtual void OnRemoveFromFlow(TagList *tags = NULL);

	virtual void Use();
	virtual void Recycle();

	virtual bool SetAttrsTagList(const TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

	virtual bool AddShadowInputs();
	virtual void FreeShadowInputs();

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);

	void SetLightEnabled(TimeStamp t, bool isEnabled);
	void SetIntensity(TimeStamp t, float64 intensity);
	void SetColor(TimeStamp t, Color4d &col);				// Alpha value is ignored

	bool		GetLightEnabled(TimeStamp t);
	float64	GetIntensity(TimeStamp t);
	Color4d	GetColor(TimeStamp t);							// Alpha has no meaning for lights.  The returned alpha will always be set to 1.0

	/**
	 * Adds the standard set of light inputs to the this tool - enable/intensity/color inputs.  Subclasses typically call this 
	 * from AddInputsTagList()
	 */
	virtual bool AddStandardLightInputs(TagList *args = NULL);

	/**
	 * Plugin classes should override this function to return a LightData3D derived object.  If the passed in data object is NULL
	 * a new data object should be created.  Note that BaseClass::ProcessTagList() should be called.
	 */
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);

	/**
	 * Adds the inputs to the Owner.  When overriding this method, you should call the base class version.  In particular, 
	 * LightInputs3D::AddInputsTagList() will add the default enabled/color/shadows inputs if the correct attrs are set.
	 */
	virtual bool AddInputsTagList(TagList &tags);
};







class FuSYSTEM_API LightOperator3D : public Transform3DOperator
{
	FuDeclareClass(LightOperator3D, Transform3DOperator);

public:
	Inputs3D *LightInputs;

public:
	LightOperator3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightOperator3D(const LightOperator3D &toCopy);		// ni
	virtual ~LightOperator3D();

	virtual void SetLightInputs(Inputs3D *inputs);
	virtual Node3D *CreateScene(Request *req, TagList *tags = NULL);

	virtual Vector4 GetDirection();
};




// Some default values used in point/directional/spot implementations
const bool     FuDefault_LightIsEnabled = true;
const float64  FuDefault_LightColorRed = 1.0;
const float64  FuDefault_LightColorGreen = 1.0;
const float64  FuDefault_LightColorBlue = 1.0;
const float64  FuDefault_LightColorAlpha = 1.0;
const Color4f  FuDefault_LightColor = WHITE4F;
const float64  FuDefault_LightIntensity = 1.0;
const Vector3f FuDefault_LightPosition = ZERO3F;
const Vector3f FuDefault_LightDirection = Vector3f(0.0f, 0.0f, -1.0f);
const float64  FuDefault_LightConstantDecay = 1.0f;
const float64  FuDefault_LightLinearDecay = 1.0f;
const float64  FuDefault_LightQuadraticDecay = 1.0f;
const float64  FuDefault_LightConeAngle = 40.0f;
const float64	FuDefault_LightPenumbraAngle = 1.0f;
const float64	FuDefault_LightDropoff = 1.0f;



#endif
