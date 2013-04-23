#ifndef _3D_LIGHTDIRECTIONAL_H_
#define _3D_LIGHTDIRECTIONAL_H_

#include "3D_LightBase.h"
#include "3D_LightBaseImplGL.h"
#include "3D_LightBaseImplSW.h"


class Node3D;
class Scene3D;
class ShadeContext3D;


const int NumDirectionalLightUniforms = 2;

const FuID CLSID_Light_Env_Operator;
const FuID CLSID_Light_Env_Inputs;
const FuID CLSID_Light_Env_Data;



class LightEnvData3D : public LightData3D
{
	FuDeclareClass(LightEnvData3D, LightData3D);

public:

public:
	LightEnvData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightEnvData3D(const LightEnvData3D &toCopy);
	virtual ~LightEnvData3D();

	virtual const Registry *GetImplRegistry(uint32 type);

	virtual Data3D *CopyTagList(TagList &tags);
};



class LightDirectionalGL3D : public LightImplGL3D
{
	FuDeclareClass(LightDirectionalGL3D, LightImplGL3D);

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
	LightDirectionalGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightDirectionalGL3D(const LightDirectionalGL3D &toCopy);		// purposefully left unimplemented
	virtual ~LightDirectionalGL3D();

	virtual bool Activate(RenderContextGL3D &rc,	TagList *tags = NULL);
	virtual void Deactivate(RenderContextGL3D &rc, TagList *tags = NULL);

	virtual const char *GetShaderFilename(RenderContextGL3D &rc, TagList *tags = NULL);
	virtual ShadeNodeCg *CreateShadeNode(RenderContextGL3D &rc, ShaderCg *shader, TagList *tags = NULL);	
};






class LightDirectionalSW3D : public LightImplSW3D
{
	FuDeclareClass(LightDirectionalSW3D, LightImplSW3D);

public:
	class LightDirectionalBlock : public ParamBlockSW {};

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
	LightDirectionalSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightDirectionalSW3D(const LightDirectionalSW3D &toCopy);		// purposefully left unimplemented
	virtual ~LightDirectionalSW3D();

	virtual ParamBlockSW *CreateParamBlock(RenderContextSW3D &rc, TagList *tags = NULL);
	virtual bool PreRender(RenderContextSW3D &rc, TagList *tags = NULL);
	virtual bool Illuminate(ShadeContext3D &sc, ParamBlockSW *lightBlock);
};





class LightEnvInputs3D : public LightInputs3D
{
	FuDeclareClass(LightEnvInputs3D, LightInputs3D);

public:
	Input *InMap;
	Input *InMulti;
	Input *InEnvSamples;
	Input *InEnvOcc;
	Input *InDiffuseColorNest;
	Input *InDiffuseR;
	Input *InDiffuseG;
	Input *InDiffuseB;
	const char *EnvMapPath;
public:
	LightEnvInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightEnvInputs3D(const LightEnvInputs3D &toCopy);
	virtual ~LightEnvInputs3D();

	virtual bool AddInputsTagList(TagList &tags);
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);
};






class LightEnv3D : public LightOperator3D
{
	FuDeclareClass(LightEnv3D, LightOperator3D);

public:

public:
	LightEnv3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~LightEnv3D();
};

#endif
