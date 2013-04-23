#ifndef _3D_LIGHTDIRECTIONAL_H_
#define _3D_LIGHTDIRECTIONAL_H_

#include "3D_LightBase.h"
#include "3D_LightBaseImplGL.h"
#include "3D_LightBaseImplSW.h"


class Node3D;
class Scene3D;
class ShadeContext3D;


const int NumDirectionalLightUniforms = 2;




class LightDirectionalData3D : public LightData3D
{
	FuDeclareClass(LightDirectionalData3D, LightData3D);

public:

public:
	float32 blur;
	LightDirectionalData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightDirectionalData3D(const LightDirectionalData3D &toCopy);
	virtual ~LightDirectionalData3D();

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





class LightDirectionalInputs3D : public LightInputs3D
{
	FuDeclareClass(LightDirectionalInputs3D, LightInputs3D);

public:
	Input *InBlur;

public:
	LightDirectionalInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightDirectionalInputs3D(const LightDirectionalInputs3D &toCopy);
	virtual ~LightDirectionalInputs3D();

	virtual bool AddInputsTagList(TagList &tags);
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);
};






class LightDirectional3D : public LightOperator3D
{
	FuDeclareClass(LightDirectional3D, LightOperator3D);

public:

public:
	LightDirectional3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~LightDirectional3D();
};

#endif
