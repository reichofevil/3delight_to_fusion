#ifndef _3D_SURFACEPLANE_H_
#define _3D_SURFACEPLANE_H_

#include "3D_MaterialBase.h"
#include "3D_SurfaceBase.h"


class SurfacePlaneData3D : public SurfaceData3D
{
	FuDeclareClass(SurfacePlaneData3D, SurfaceData3D);

public:
	float32	Width;
	float32	Height;
	uint32	WidthSub;
	uint32	HeightSub;
	const char* RIBPath;

public:
	SurfacePlaneData3D();
	SurfacePlaneData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	SurfacePlaneData3D(const SurfacePlaneData3D &toCopy);
	virtual ~SurfacePlaneData3D();

	void Init();

	virtual bool BuildBoundingBox();

	virtual Data3D *CopyTagList(TagList &tags); // throws CMemoryException
	virtual bool CreateGeometryTagList(std::vector<Geometry3D*> &geometry, TagList &tags); // throws CMemoryException
};


class SurfacePlaneInputs3D : public SurfaceInputs3D
{
	FuDeclareClass(SurfacePlaneInputs3D, SurfaceInputs3D); 

public:
	Input *InLockSizeXY;
	Input *InWidth, *InHeight;
	Input *InSubdivisionWidth, *InSubdivisionHeight;
	Input *InRIBFile;

	int OldMode;
	bool IsImagePlane;
	bool LockSize;

public:
	SurfacePlaneInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~SurfacePlaneInputs3D();

	virtual void NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags = NULL);
	virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);

	bool Init();

	virtual bool AddInputsTagList(TagList &tags);
	virtual void ShowInputs(bool visible);
};


#endif
