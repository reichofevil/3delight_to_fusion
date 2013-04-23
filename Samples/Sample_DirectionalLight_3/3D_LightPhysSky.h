#ifndef _3D_LightPhysSky_H_
#define _3D_LightPhysSky_H_

#include "3D_LightBase.h"
#include "3D_LightBaseImplGL.h"
#include "3D_LightBaseImplSW.h"


class Node3D;
class Scene3D;
class ShadeContext3D;

const FuID CLSID_Light_PhysSky_Operator;
const FuID CLSID_Light_PhysSky_Inputs;
const FuID CLSID_Light_PhysSky_Data;


const int NumDirectionalLightUniforms = 2;




class LightPhysSkyData3D : public LightData3D
{
	FuDeclareClass(LightPhysSkyData3D, LightData3D);

public:

public:
	float32 longitude,latidue,day,time,turbi;
	LightPhysSkyData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightPhysSkyData3D(const LightPhysSkyData3D &toCopy);
	virtual ~LightPhysSkyData3D();

	virtual const Registry *GetImplRegistry(uint32 type);

	virtual Data3D *CopyTagList(TagList &tags);
};



class LightPhysSkyGL3D : public LightImplGL3D
{
	FuDeclareClass(LightPhysSkyGL3D, LightImplGL3D);

public:
	class LightPhysSkyCg : public ShadeNodeCg
	{
	public:
		ParamCg InLightDir;
		ParamCg InColor;

	public:
		LightPhysSkyCg() {}
		virtual ~LightPhysSkyCg() {}
		virtual void Free() {}
	};


public:
	LightPhysSkyGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightPhysSkyGL3D(const LightPhysSkyGL3D &toCopy);		// purposefully left unimplemented
	virtual ~LightPhysSkyGL3D();

	virtual bool Activate(RenderContextGL3D &rc,	TagList *tags = NULL);
	virtual void Deactivate(RenderContextGL3D &rc, TagList *tags = NULL);

	virtual const char *GetShaderFilename(RenderContextGL3D &rc, TagList *tags = NULL);
	virtual ShadeNodeCg *CreateShadeNode(RenderContextGL3D &rc, ShaderCg *shader, TagList *tags = NULL);	
};






class LightPhysSkySW3D : public LightImplSW3D
{
	FuDeclareClass(LightPhysSkySW3D, LightImplSW3D);

public:
	class LightPhysSkyBlock : public ParamBlockSW {};

	Vector3f Position;				// transformed to eye space
	Vector3f Direction;				// transformed to eye space
	float32 CosOuter;
	float32 CosInner;
	float32 Divisor;
	float32 LinearDecay;				// precomputed divide by GlobalScale
	float32 QuadraticDecay;

	// These guys are just duplicated here from LightSpotData3D for faster access in Illuminate()
	float32 ConstantDecay;
	float32 Falloff;
	Color4f Color;


public:
	LightPhysSkySW3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightPhysSkySW3D(const LightPhysSkySW3D &toCopy);		// purposefully left unimplemented
	virtual ~LightPhysSkySW3D();

	virtual ParamBlockSW *CreateParamBlock(RenderContextSW3D &rc, TagList *tags = NULL);
	virtual bool PreRender(RenderContextSW3D &rc, TagList *tags = NULL);
	virtual bool Illuminate(ShadeContext3D &sc, ParamBlockSW *lightBlock);
};





class LightPhysSkyInputs3D : public LightInputs3D
{
	FuDeclareClass(LightPhysSkyInputs3D, LightInputs3D);

public:
	Input *InTurbiditye;
	Input *InLongitude;
	Input *InLatitude;
	Input *InTime;
	Input *InDay;

public:
	LightPhysSkyInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightPhysSkyInputs3D(const LightPhysSkyInputs3D &toCopy);
	virtual ~LightPhysSkyInputs3D();

	virtual bool AddInputsTagList(TagList &tags);
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);
};






class LightPhysSky3D : public LightOperator3D
{
	FuDeclareClass(LightPhysSky3D, LightOperator3D);

public:

public:
	LightPhysSky3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~LightPhysSky3D();
};

#endif
