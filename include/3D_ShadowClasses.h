#ifndef _3D_SHADOW_BASE_CLASSES_H_
#define _3D_SHADOW_BASE_CLASSES_H_


#include "3D_Datatypes.h"
#include "3D_BaseInputs.h"


enum SMT_ShadowMapType
{
	SMT_Planar,
	SMT_DualParabloid,				// nyi
	SMT_CubeMap,						// nyi
};

enum SMCT_ChannelType
{
	SMCT_Z,
	SMCT_AZ,								// nyi
	SMCT_RGBAZ,
};

enum SMA_Algorithm
{
	SMA_Basic,							// basic shadowmapping
	SMA_Perspective,					// nyi
	SMA_Trapezoidal,					// nyi
	SMA_NumAlgorithms
};

enum SMSF_Softness
{
	SMSF_None,
	SMSF_Constant,
	SMSF_Variable
};

enum SMST_SamplingType
{
	SMST_Nearest,
	SMST_Bilinear,
	SMST_Area
};


class FuSYSTEM_API ShadowLightData3D : public Object
{
	FuDeclareClass(ShadowLightData3D, Object);

public:
	bool ShadowsEnabled;												// defaults to false

	Color4f ShadowColor;												// the alpha component is meaningless
	float32 ShadowDensity;

	SMA_Algorithm Algorithm;

	uint32 ShadowMapSize;											// may be clamped to pow2 depending on renderer

	bool AutoBias;
	float32 AdditiveBias;											// a number close to 0.0f
	float32 MultiplicativeBias;									// a number close to, but less than 1.0f

	bool ForceAllMaterialsNonTransmissive;

	SMST_SamplingType SampleType;

	SMSF_Softness SoftnessType;
	float32 ConstantSoftness;										// used only in const softness case - a cooked constant between 0.0f and infinity
	float32 Spread;										// used only in non-const softness case
	float32 MinSoftness;												// used only in non-const softness case
	float32 FilterSize;												// used only in non-const softness case

public:
	ShadowLightData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	ShadowLightData3D(const ShadowLightData3D &toCopy);
	virtual ~ShadowLightData3D();

	virtual bool Init();
	virtual ShadowLightData3D *Copy() const;

	virtual uint64 EstimateMemoryUsage();
};




class FuSYSTEM_API ShadowLightInputs3D : public Inputs3D
{
	FuDeclareClass(ShadowLightInputs3D, Inputs3D);

public:
	Input *InShadowsEnabled;				// enable shadows for this light

	Input *InShadowColorR;	
	Input *InShadowColorG;
	Input *InShadowColorB;
	Input *InShadowDensity;

	Input *InShadowMapAlgorithm;			// used to select the type of shadowmap algorithm to use

	Input *InShadowMapSize;					// shadowmaps are never rectangular, always square
	Input *InShadowMapProxy;

	Input *InAutoBias;						// uncheck to do manual biasing
	Input *InAdditiveBias;					// manual bias adjustment for multiplicative and additive factors
	Input *InMultiplicativeBias;
	int32 AutoBiasShowCount;

	Input *InForceAllMaterialsNonTransmissive;

	Input *InShadowMapSamplingType;

	SMSF_Softness OldSoftnessType;
	Input *InShadowMapSoftnessType;
	Input *InConstantSoftness;				// only shown when softness type == constant
	Input *InSpread;							// only shown when softness type == variable
	Input *InMinSoftness;					// only shown when softness type == variable
	Input *InFilterSize;						// only shown when softness type == variable

public:
	// The ctor taglist must always contain a (OBJP_Owner, Operator*) pair so that we can add our inputs to it
	ShadowLightInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~ShadowLightInputs3D();

	// The Owner should always pass on NotifyChanged messages to this set of input controls
	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);

	virtual ShadowLightData3D *Process(Request *req, TagList *tags = NULL);

	bool Init();

	virtual bool AddInputsTagList(TagList &tags);
	virtual void ShowInputs(bool visible);
};





#endif