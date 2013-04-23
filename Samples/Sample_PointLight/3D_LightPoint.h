#ifndef _3D_LIGHTPOINT_H_
#define _3D_LIGHTPOINT_H_

#include "3D_LightBase.h"
#include "3D_LightBaseImplGL.h"
#include "3D_LightBaseImplSW.h"


class Node3D;
class Scene3D;
class ShadeContext3D;





const int NumPointLightUniforms = 3;



class LightPointData3D : public LightData3D
{
	FuDeclareClass(LightPointData3D, LightData3D);

public:

public:
	float32 blur;
	LightPointData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightPointData3D(const LightPointData3D &toCopy);
	virtual ~LightPointData3D();

	virtual Data3D *CopyTagList(TagList &tags);

	virtual const Registry *GetImplRegistry(uint32 type);
	
};



class LightPointGL3D : public LightImplGL3D
{
	FuDeclareClass(LightPointGL3D, LightImplGL3D);

public:
	class LightPointCg : public ShadeNodeCg
	{
	public:
		ParamCg InLightPos;
		ParamCg InColor;
		ParamCg InAttenuation;

	public:
		LightPointCg() : ShadeNodeCg() {}
		virtual ~LightPointCg() {}
		virtual void Free() {}
	};

public:
	LightPointGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightPointGL3D(const LightPointGL3D &toCopy);		// purposefully left unimplemented
	virtual ~LightPointGL3D();

	virtual bool Activate(RenderContextGL3D &rc,	TagList *tags = NULL);
	virtual void Deactivate(RenderContextGL3D &rc, TagList *tags = NULL);

	virtual const char *GetShaderFilename(RenderContextGL3D &rc, TagList *tags = NULL);
	virtual ShadeNodeCg *CreateShadeNode(RenderContextGL3D &rc, ShaderCg *shader, TagList *tags = NULL);
};





class LightPointSW3D : public LightImplSW3D
{
	FuDeclareClass(LightPointSW3D, LightImplSW3D);

public:
	class LightPointBlock : public ParamBlockSW
	{
	public:
		Vector3f *EyePosition;
	};

	Color4f Color;										// copied straight from LightPointDataSW3D
	Vector3f Position;								// transformed to eye space
	float32 ConstantDecay;							// copied straight from LightPointDataSW3D
	float32 LinearDecay, QuadraticDecay;		// precomputed divide by GlobalScale

public:
	LightPointSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightPointSW3D(const LightPointSW3D &toCopy);		// purposefully left unimplemented
	virtual ~LightPointSW3D();

	virtual ParamBlockSW *CreateParamBlock(RenderContextSW3D &rc, TagList *tags = NULL);

	virtual bool PreRender(RenderContextSW3D &rc, TagList *tags = NULL);
	virtual bool Illuminate(ShadeContext3D &sc, ParamBlockSW *lightBlock);
};






class LightPointInputs3D : public LightInputs3D
{
	FuDeclareClass(LightPointInputs3D, LightInputs3D);

public:
	Input *InDecayType;
	Input *InDecayRate;
	Input *InBlur;
	int CurrentDecayType;

public:
	LightPointInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightPointInputs3D(const LightPointInputs3D &toCopy);
	virtual ~LightPointInputs3D();

	virtual bool AddInputsTagList(TagList &tags);
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);
	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);
};






class LightPoint3D : public LightOperator3D
{
	FuDeclareClass(LightPoint3D, LightOperator3D); 

public:
	LightPoint3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~LightPoint3D();
};



#endif
