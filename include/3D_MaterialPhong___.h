#ifndef _3D_MATERIALPHONG_H_
#define _3D_MATERIALPHONG_H_

#include "3D_MaterialBlinn.h"







class MtlPhongData3D : public MtlBlinnData3D
{
	FuDeclareClass(MtlPhongData3D, MtlBlinnData3D);

public:
	MtlPhongData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlPhongData3D(const MtlPhongData3D &toCopy);
	virtual ~MtlPhongData3D();

	virtual const Registry *GetImplRegistry(uint32 type);
};






class MtlPhongSW3D : public MtlBlinnSW3D
{
	FuDeclareClass(MtlPhongSW3D, MtlBlinnSW3D);

public:
	MtlPhongSW3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlPhongSW3D(const MtlPhongSW3D &toCopy);		// purposefully left unimplemented
	virtual ~MtlPhongSW3D();

	void ShadeFragment(ShadeContext3D &sc);
};






class MtlPhongGL3D : public MtlBlinnGL3D
{
	FuDeclareClass(MtlPhongGL3D, MtlBlinnGL3D);

public:

public:
	MtlPhongGL3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	MtlPhongGL3D(const MtlPhongGL3D &toCopy);		// purposefully left unimplemented
	virtual ~MtlPhongGL3D();

	virtual ShadeNodeCg *CreateShadeNode(RenderContextGL3D &rc);
	virtual const char *GetShaderFilename(RenderContextGL3D &rc);
	virtual const char *GetShadeNodeName(RenderContextGL3D &rc);
};






class MtlPhongInputs3D: public MtlBlinnInputs3D
{
	FuDeclareClass(MtlPhongInputs3D, MtlBlinnInputs3D);

public:
	MtlPhongInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~MtlPhongInputs3D();

	virtual bool AddInputsTagList(TagList &tags);
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);
};





class MtlPhong3D: public MtlBlinn3D
{
	FuDeclareClass(MtlPhong3D, MtlBlinn3D);

public:
	MtlPhong3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~MtlPhong3D();
};


#endif
