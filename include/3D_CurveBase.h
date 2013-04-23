#ifndef _3D_CURVEEBASE_H_
#define _3D_CURVEEBASE_H_

#include "Curve3D.h"
#include "Geometry3D.h"
#include "LockableObject.h"

#include "3D_BaseData.h"
#include "3D_DataTypes.h"
#include "3D_BaseInputs.h"
#include "3D_ShadowClasses.h"



class AABB3D;
class TessArray;
class CurveInputs3D;



class FuSYSTEM_API CurveData3D : public Data3D
{
	FuDeclareClass(CurveData3D, Object);

public:
	PCT_PolyClipType ClipType, QuickClipType;

	LockableObject TesselationCacheLock;
	TessArray *TesselationCache;					// Cached tesselation of this curve, stored in model space

	LockableObject CachedCurveLock;
	FuPointer<Curve3D> CachedCurve;							// Cached results of the last Render() of this curve  (TODO - we can't really cache this because the results of the Render() can depend on the modelview matrix, for example)

	CurveInputs3D *_Curve;

	int32 ObjectID;	// TODO - just do this for noww

	uint8 m_CurveDataPad[16];

public:
	CurveData3D();
	CurveData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	CurveData3D(const CurveData3D &toCopy);
	virtual ~CurveData3D();

	void Init();

	// Normally when you inherit from CurveData3D you would override this Render() call.
	//  It is the responsibility of your derived classed to take this object and convert it to a Curve3D.
	// You are responsible for allocating the Curve3D object and any sub-arrays contained therein. 
	//  The object calling the Render() callback is responsible for deleting the curve and any sub-
	//  arrays contained contained therein.
	// The value of *curve defaults to NULL.  If you don't wish to render anything simply leave it
	//  with its default value.
	virtual Curve3D *Render(RenderContext3D &rc);

	/**
	 * Subclasses must override this.
	 */
	virtual bool BuildBoundingBox();

	/**
	 * Subclasses must override this.
	 */
	virtual Data3D *CopyTagList(TagList &tags);

	virtual void MapOut();
	virtual void MapIn();
	virtual void FreeTempData();
	virtual uint64 EstimateMemoryUsage();

	// Note ConvertToGeometry can't be a virtual function like we did in SurfaceData3D, 
	// because CurveData3D::Render returns a Curve3D* but we need a pointer to Geometry3D
	Geometry3D *ConvertToGeometry();
};


class FuSYSTEM_API CurveInputs3D : public Inputs3D
{
	FuDeclareClass(CurveInputs3D, ShadowObjectInputs3D);

public:

public:
	CurveInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~CurveInputs3D();

	static bool RegInit(Registry *regnode);
	static void RegCleanup(Registry *regnode);

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);

	virtual CurveData3D *Process(Request *req, CurveData3D *data);
	virtual void ShowInputs(bool visible);
};

#endif
