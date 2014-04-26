#ifndef _3D_SURFACEPLANE_H_
#define _3D_SURFACEPLANE_H_

#include "3D_MaterialBase.h"
#include "3D_SurfaceBase.h"

const FuID CLSID_Partio_Plane_Operator ="PartioPlaneOperator";
const FuID CLSID_Partio_Plane_Inputs ="PartioPlaneInputs";
const FuID CLSID_Partio_Plane_Data="PartioPlaneData";


class PartioPlaneData3D : public SurfaceData3D
{
	FuDeclareClass(PartioPlaneData3D, SurfaceData3D);

public:
	float32	Width;
	float32	Height;
	uint32	WidthSub;
	uint32	HeightSub;
	const char* RIBPath;
	uint32 ptype;
	float32 pradius, pradius_scale;

public:
	PartioPlaneData3D();
	PartioPlaneData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	PartioPlaneData3D(const PartioPlaneData3D &toCopy);
	virtual ~PartioPlaneData3D();

	void Init();

	virtual bool BuildBoundingBox();

	virtual Data3D *CopyTagList(TagList &tags); // throws CMemoryException
	virtual bool CreateGeometryTagList(std::vector<Geometry3D*> &geometry, TagList &tags); // throws CMemoryException
};


class PartioPlaneInputs3D : public SurfaceInputs3D
{
	FuDeclareClass(PartioPlaneInputs3D, SurfaceInputs3D); 

public:
	Input *InLockSizeXY;
	Input *InWidth, *InHeight;
	Input *InSubdivisionWidth, *InSubdivisionHeight;
	Input *InRIBFile, *InType, *InRadius, *InRadiusScale;

	int OldMode;
	bool IsImagePlane;
	bool LockSize;

public:
	PartioPlaneInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~PartioPlaneInputs3D();

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);

	bool Init();

	virtual bool AddInputsTagList(TagList &tags);
	virtual void ShowInputs(bool visible);
};


#endif
