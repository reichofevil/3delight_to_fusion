#ifndef _3D_AO_H_
#define _3D_AO_H_

#include "3D_LightBase.h"
#include "3D_LightBaseImplGL.h"
#include "3D_LightBaseImplSW.h"


class Node3D;
class Scene3D;
class ShadeContext3D;


const int NumAOLightUniforms = 2;

const FuID CLSID_Light_AO_Operator;
const FuID CLSID_Light_AO_Inputs;
const FuID CLSID_Light_AO_Data;



class LightAOData3D : public LightData3D
{
	FuDeclareClass(LightAOData3D, LightData3D);

public:

public:
	LightAOData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightAOData3D(const LightAOData3D &toCopy);
	virtual ~LightAOData3D();

	virtual const Registry *GetImplRegistry(uint32 type);

	virtual Data3D *CopyTagList(TagList &tags);
};



class LightAOGL3D : public LightImplGL3D
{
	FuDeclareClass(LightAOGL3D, LightImplGL3D);

public:
	class LightDirectionalCg : public ShadeNodeCg
	{
	public:
		ParamCg InLightDir;
		ParamCg InColor;

	public:
		LightDirectionalCg() {}
		virtual ~LightDirectionalCg() {}
		virtual void Free() {}
	};


public:
	LightAOGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightAOGL3D(const LightAOGL3D &toCopy);		// purposefully left unimplemented
	virtual ~LightAOGL3D();

	virtual bool Activate(RenderContextGL3D &rc,	TagList *tags = NULL);
	virtual void Deactivate(RenderContextGL3D &rc, TagList *tags = NULL);

	virtual const char *GetShaderFilename(RenderContextGL3D &rc, TagList *tags = NULL);
	virtual ShadeNodeCg *CreateShadeNode(RenderContextGL3D &rc, ShaderCg *shader, TagList *tags = NULL);	
};






class LightAOSW3D : public LightImplSW3D
{
	FuDeclareClass(LightAOSW3D, LightImplSW3D);

public:
	class LightAOBlock : public ParamBlockSW {};

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
	LightAOSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightAOSW3D(const LightAOSW3D &toCopy);		// purposefully left unimplemented
	virtual ~LightAOSW3D();

	virtual ParamBlockSW *CreateParamBlock(RenderContextSW3D &rc, TagList *tags = NULL);
	virtual bool PreRender(RenderContextSW3D &rc, TagList *tags = NULL);
	virtual bool Illuminate(ShadeContext3D &sc, ParamBlockSW *lightBlock);
};





class LightAOInputs3D : public LightInputs3D
{
	FuDeclareClass(LightAOInputs3D, LightInputs3D);

public:
	Input *InMap;
	Input *InMulti;
	Input *InEnvSamples;
	Input *InEnvOcc;
	const char *EnvMapPath;
public:
	LightAOInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightAOInputs3D(const LightAOInputs3D &toCopy);
	virtual ~LightAOInputs3D();

	virtual bool AddInputsTagList(TagList &tags);
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);
};






class LightAO3D : public LightOperator3D
{
	FuDeclareClass(LightAO3D, LightOperator3D);

public:

public:
	LightAO3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~LightAO3D();
};

#endif
