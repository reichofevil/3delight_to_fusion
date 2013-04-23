#ifndef _3D_SURFACE_GEOMETRY_H_
#define _3D_SURFACE_GEOMETRY_H_

#include "3D_DataTypes.h"
#include "3D_SurfaceBase.h"





// SurfaceGeometryData3D is a wrapper for Geometry3D so that one can wrap up a Geometry3D as a surface and stuff into a scene.  Tools like
// Bender3D, Displace3D which need access to polygons will call GetGeometry() on the surfaces in a scene and then stuff their results into
// SurfaceGeometryData3D.
class SurfaceGeometryData3D : public SurfaceData3D
{
	FuDeclareClass(SurfaceGeometryData3D, SurfaceData3D);

protected:
	std::vector<Geometry3D*> Geometry;		// ref counted

public:
	SurfaceGeometryData3D();
	SurfaceGeometryData3D(const Registry *reg, const ScriptVal &table, const TagList &tags);
	SurfaceGeometryData3D(const SurfaceGeometryData3D &toCopy);
	virtual ~SurfaceGeometryData3D();

	void Init();

	virtual Data3D *CopyTagList(TagList &tags); // throws CMemoryException
	virtual bool BuildBoundingBox();

	virtual void AddGeometry(Geometry3D *geometry);
	virtual void AddGeometry(std::vector<Geometry3D*> &geometry);
	//virtual bool GetGeometryTagList(std::vector<Geometry3D*> &geometry, bool modifiable, TagList &tags);

	virtual bool CreateGeometryTagList(std::vector<Geometry3D*> &geometry, TagList &tags); // throws CMemoryException
};


#endif
