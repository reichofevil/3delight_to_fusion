#ifndef _LOOKUPTABLE3D_H_
#define _LOOKUPTABLE3D_H_

#include "LookUpTable.h"
#include "Pixel.h"


#define LOOKUPTABLE3D_ID zMAKEID('LUT3')

#define LOOKUPTABLE3D_BASE (LOOKUPTABLE_BASE + TAGOFFSET)


enum LOOKUPTABLE3D_ATTRS
{
	LUT3_Entries = LOOKUPTABLE3D_BASE | TAG_INT,
};



class FuSYSTEM_API LUT3D : public LUT
{
	FuDeclareClass(LUT3D, LUT); // declare ClassID static member, etc.

public:
	int32 m_NumEntries3D;		// one dimension only (LUT::m_NumEntries is m_NumEntries3D ^ 3)

public:
	LUT3D(int32 numentries, LUTTypes type);
	virtual ~LUT3D();

	LUT3D(const LUT3D &lut);

	LUT3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual bool Save(ScriptVal &table, const TagList &tags);

	uint32  GetValue(uint32 invalR, uint32 invalG, uint32 invalB);
	float64 GetValue(float64 invalR, float64 invalG, float64 invalB);
	uint32  GetValue(Pixel &p)						{ return GetValue(p.R, p.G, p.B);		}
	float64 GetValue(FltPixel &fp)				{ return GetValue(fp.R, fp.G, fp.B);	}
};



class FuSYSTEM_API LookUpTable3D : public LookUpTable
{
	FuDeclareClass(LookUpTable3D, LookUpTable); // declare ClassID static member, etc.

protected:
	FuPointer<LUT3D> m_Cache3D;

public:
	LookUpTable3D(FuPointer<LUT3D> lut3D);
	LookUpTable3D(Object *obj, Output *out, TimeStamp time, void *context = NULL);
	
	virtual ~LookUpTable3D();
	
	LookUpTable3D(LookUpTable3D &param);
	virtual Parameter *Copy();
	
	LookUpTable3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual bool Save(ScriptVal &table, const TagList &tags);

	static const LookUpTable3D &Get(Input *in, Request *req) { return *(LookUpTable3D *) in->GetValue(req); }

	virtual bool CopyAttr(Tag tag, void *addr) const;

	uint32  GetValue(uint32 invalR, uint32 invalG, uint32 invalB);
	float64 GetValue(float64 invalR, float64 invalG, float64 invalB);

	virtual FuPointer<LUT> _GetTable(TagList &tags);

	static void ApplyLookUpTables(Image *img, LookUpTable3D *lutr, LookUpTable3D *lutg, LookUpTable3D *lutb);

#ifndef USE_NOSCRIPTING
	DECLARE_LUATYPE(LookUpTable3D);
#endif
};

#endif
