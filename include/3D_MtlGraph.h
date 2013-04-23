#ifndef _MTL_GRAPH_H_
#define _MTL_GRAPH_H_

#include "Parameter.h"
#include "3D_MaterialBase.h"


//class MtlNode3D
//{
//public:
//	MtlNode3D()
//	{
//		Next = NULL;
//		Child = NULL;
//	}
//
//	FuPointer<MtlData3D> Data;	
//
//	MtlNode3D *Next;
//	MtlNode3D *Child;
//
//public:
//	void SetData(MtlData3D *mtlData);
//
//	void AddChild(MtlNode3D *child)
//	{
//		uassert(child->Next == NULL);
//
//		if (Child)
//		{
//			child->Next = Child;
//			Child = child;
//		}
//		else
//			Child = child;
//	}
//};


class FuSYSTEM_API MtlGraph3D : public Parameter
{
	FuDeclareClass(MtlGraph3D, Parameter);

public:
	bool IsInitialized;
	bool IsSetup;
	LockableObject SetupLock;

	TimeStamp Time;
	int32 Proxy, NoData;
	int32 Quality, RealQuality;

	MtlData3D *Root;			// this can validly be NULL, when setting this remember that the mtlgraph itself will call Root->Recycle() when it goes away

public:
	MtlGraph3D(FusionDoc *doc, Request *req);
	MtlGraph3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	~MtlGraph3D();

	void Init(FusionDoc *doc, Request *req);
	void Init(FusionDoc *doc, TimeStamp time, bool hiQ, int32 proxy, bool precalc);

	virtual void Free();

	virtual void SetRoot(MtlData3D *root);			// consumes the root object

	virtual void GetNaturalDims(float32 dims[4]);

	virtual void MapIn();
	virtual void MapOut();
	virtual void FreeTempData();

	virtual bool HandleEvent(const FuID &eventid, uint_ptr subid, TagList &tags);
	virtual bool CopyAttr(Tag tag, void *addr) const;

protected:
	virtual bool _IsValid();

	#ifndef USE_NOSCRIPTING
		DECLARE_LUATYPE(MtlGraph3D);
	#endif
};


#endif