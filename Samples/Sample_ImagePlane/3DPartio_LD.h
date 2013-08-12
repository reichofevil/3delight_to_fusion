#ifndef _3D_IMAGEPLANE_H_
#define _3D_IMAGEPLANE_H_

class MtlStdInputs3D;

#include "3D_Datatypes.h"
#include "3D_PartioPlane.h"
#include "Transform3DOperator.h"




class ImagePlane3D2: public Transform3DOperator
{
	FuDeclareClass(ImagePlane3D2, Transform3DOperator);

protected:
	PartioPlaneInputs3D *InSurfacePlane;

	Input *InMaterial;									// this functions as a material input or as the diffuse texture input for the default material

	bool UsingDefaultMaterial;				
	MtlStdInputs3D *InDefaultMaterial;					// the material that appears in the material tab

	Input *InObjectID;

public:
	ImagePlane3D2(const Registry *reg, const ScriptVal &table, const TagList &tags);
	~ImagePlane3D2();

	virtual void CheckRequest(Request *req);
	virtual bool OnConnect(Link *src, Link *dest, TagList *tags);

	virtual Node3D *CreateScene(Request *req, TagList *tags = NULL);
};


#endif

