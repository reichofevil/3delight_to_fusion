#ifndef _3D_SDK_SIMPLECUBE_H_
#define _3D_SDK_SIMPLECUBE_H_

/** 
 * @file  SimpleCube.h 
 * @brief Shows how to generate a cube covered with one material.  For a more complex version see the Cube SDK example.
 */

#include "3D_SurfaceBase.h"
#include "Transform3DOperator.h"



class SimpleCubeInputs : public SurfaceInputs3D
{
   FuDeclareClass(SimpleCubeInputs, SurfaceInputs3D);

public:
   Input *InCubeWidth, *InCubeHeight, *InCubeDepth, *InFile, *InPartDiv, *InPartColor, *InPartR, *InPartG, *InPartB;

public:
   SimpleCubeInputs(const Registry *reg, const ScriptVal &table, const TagList &tags);
   SimpleCubeInputs(const SimpleCubeInputs &toCopy);		// purposefully left unimplemented
   virtual ~SimpleCubeInputs();

   virtual Data3D *ProcessTagList(Request *req, Data3D *data, TagList &tags);
   virtual bool AddInputsTagList(TagList &tags);
};



class SimpleCubeData : public SurfaceData3D
{
   FuDeclareClass(SimpleCubeData, SurfaceData3D);

public:
   float32 Width, Height, Depth;
   const char* File;
   uint32 pDiv;
   Color4f PartColor;

public:
   SimpleCubeData(const Registry *reg, const ScriptVal &table, const TagList &tags);
   SimpleCubeData(const SimpleCubeData &toCopy);
   virtual ~SimpleCubeData();

   virtual bool BuildBoundingBox();
   virtual Data3D *CopyTagList(TagList &tags); // throws CMemoryException
   virtual bool CreateGeometryTagList(std::vector<Geometry3D*> &geometry, TagList &tags); // throws CMemoryException

	virtual int GetNumMtlSlots();
	virtual const FuID GetMtlSlotName(int i);
};




class SimpleCubeTool : public Transform3DOperator
{
   FuDeclareClass(SimpleCubeTool, Transform3DOperator);

public:
   // These are declared in the Transform3DOperator:
   //Input *In3D;													// "SimpleCube.SceneInput"
   //Output *Out3D;												// "SimpleCube.Output" 

   SimpleCubeInputs *InCubeInputs;							// width/height/depth inputs

   Input *InMaterial;										// the material for this cube

public:
   SimpleCubeTool(const Registry *reg, const ScriptVal &table, const TagList &tags);
   SimpleCubeTool(const SimpleCubeTool &toCopy);		// purposefully left unimplemented - tools should never be copy constructed
   ~SimpleCubeTool();

   virtual void Process(Request *req);	
   virtual Node3D *CreateScene(Request *req, TagList *tags = NULL);
   virtual void CheckRequest(Request *req);
};

#endif
