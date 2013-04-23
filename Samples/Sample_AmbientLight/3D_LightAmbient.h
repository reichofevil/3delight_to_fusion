#ifndef _3D_LIGHTCONSTANT_H_
#define _3D_LIGHTCONSTANT_H_

#include "3D_LightBase.h"
#include "3D_MaterialBase.h"			// TEMP
#include "3D_LightBaseImplGL.h"
#include "3D_LightBaseImplSW.h"


class Node3D;
class Scene3D;
class ShadeContext3D;





class LightAmbientData3D : public LightData3D
{
	FuDeclareClass(LightAmbientData3D, LightData3D);

public:

public:
	LightAmbientData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightAmbientData3D(const LightAmbientData3D &toCopy);
	virtual ~LightAmbientData3D();

	virtual const Registry *GetImplRegistry(uint32 type);

	virtual Data3D *CopyTagList(TagList &tags);

	virtual bool GetAmbientContribution(Color4f &c);
};



class LightAmbientGL3D : public LightImplGL3D
{
	FuDeclareClass(LightAmbientGL3D, LightImplGL3D);

public:
	class LightAmbientCg : public ShadeNodeCg
	{
	public:
		ParamCg InColor;

	public:
		LightAmbientCg() {}
		virtual ~LightAmbientCg() {}
		virtual void Free() {}
	};

public:
	LightAmbientGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightAmbientGL3D(const LightAmbientGL3D &toCopy);		// purposefully left unimplemented
	virtual ~LightAmbientGL3D();

	virtual bool Activate(RenderContextGL3D &rc,	TagList *tags = NULL);
	virtual void Deactivate(RenderContextGL3D &rc, TagList *tags = NULL);

	virtual const char *GetShaderFilename(RenderContextGL3D &rc, TagList *tags = NULL);
	virtual ShadeNodeCg *CreateShadeNode(RenderContextGL3D &rc, ShaderCg *shader, TagList *tags = NULL);
};





class LightAmbientSW3D : public LightImplSW3D
{
	FuDeclareClass(LightAmbientSW3D, LightImplSW3D);

public:
	class LightAmbientBlock : public ParamBlockSW {};
	Color4f Color;

public:
	LightAmbientSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightAmbientSW3D(const LightAmbientSW3D &toCopy);		// purposefully left unimplemented
	virtual ~LightAmbientSW3D();

	virtual ParamBlockSW *CreateParamBlock(RenderContextSW3D &rc, TagList *tags);
	virtual bool Illuminate(ShadeContext3D &sc, ParamBlockSW *lightBlock);
};






class LightAmbientInputs3D : public LightInputs3D
{
	FuDeclareClass(LightAmbientInputs3D, LightInputs3D);

public:

public:
	LightAmbientInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	LightAmbientInputs3D(const LightAmbientInputs3D &toCopy);
	virtual ~LightAmbientInputs3D();

	virtual bool AddInputsTagList(TagList &tags);
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);
};





class LightAmbient3D : public LightOperator3D
{
	FuDeclareClass(LightAmbient3D, LightOperator3D);

public:
	LightAmbient3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~LightAmbient3D();
};

#endif
