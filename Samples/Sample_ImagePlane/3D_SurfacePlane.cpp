//@ Project "Fu3D"

#include "stdafx.h"

#define MODULENAME "3DSurfacePlane"

#define ThisClass SurfacePlaneInputs3D 
#define BaseClass SurfaceInputs3D

#ifdef FuPLUGIN
#include "FuPlugin.h"
#endif

#include "Resource.h"
#include "Geometry3D.h"
#include "RegistryLists.h"

#include "3D_AABB.h"
#include "3D_SurfacePlane.h"



//-------------------------------
// SurfacePlaneInputs3D class
//-------------------------------

FuRegisterClass(COMPANY_ID_DOT + CLSID_Surface_Plane_Inputs, CT_SurfaceInputs3D)
	REGS_Name,					COMPANY_ID "RIB Archive Loader",
	TAG_DONE);

SurfacePlaneInputs3D::SurfacePlaneInputs3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{	
	Init();
}

SurfacePlaneInputs3D::~SurfacePlaneInputs3D()
{
}

bool SurfacePlaneInputs3D::Init()
{
	//InTriangles = NULL;
	//InSubdivision = NULL;

	OldMode = -1;		// no old mode
	IsImagePlane = false;

	return true;
}

bool SurfacePlaneInputs3D::AddInputsTagList(TagList &tags)
{
	InLockSizeXY = AddInput("Lock Width/Height", "SizeLock",
		LINKID_DataType,			CLSID_DataType_Number,
		INPID_InputControl,		CHECKBOXCONTROL_ID,
		INP_MinAllowed,			0.0,
		INP_MaxAllowed,			1.0,
		INP_Default,				1.0,
		INP_External,				FALSE,
		INP_DoNotifyChanged,		TRUE,
		IC_Visible,				FALSE,
		TAG_DONE);

	if (IsImagePlane)
	{
		InWidth = NULL;
		InHeight = NULL;
	}
	else
	{
		InWidth = AddInput("Size", "Width",
			LINKID_DataType,		CLSID_DataType_Number,
			INPID_InputControl,	SLIDERCONTROL_ID,
			INP_MinScale,			0.0,
			INP_MaxScale,			10.0,
			INP_Default,			1.0,
			INP_DoNotifyChanged, TRUE,
			IC_Visible,				FALSE,
			TAG_DONE);

		InHeight = AddInput("Height", "Height",
			LINKID_DataType,		CLSID_DataType_Number,
			INPID_InputControl,	SLIDERCONTROL_ID,
			INP_MinScale,			0.0,
			INP_MaxScale,			10.0,
			INP_Default,			1.0,
			IC_Visible,				FALSE,
			INP_DoNotifyChanged, TRUE,
			TAG_DONE);
	}

	InSubdivisionWidth = AddInput("Subdivision Level", "SubdivisionWidth",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			TRUE,
		IC_Steps,				10,
		INP_MinScale,			1.0,
		INP_MaxScale,			40.0,
		INP_MinAllowed,		1.0,
		INP_Default,			10.0,
		INP_DoNotifyChanged, TRUE,
		IC_Visible,				FALSE,
		TAG_DONE);

	InSubdivisionHeight = AddInput("Subdivision Height", "SubdivisionHeight",
		LINKID_DataType,		CLSID_DataType_Number,
		INPID_InputControl,	SLIDERCONTROL_ID,
		INP_Integer,			TRUE,
		IC_Steps,				10,
		INP_MinScale,			1.0,
		INP_MaxScale,			40.0,
		INP_MinAllowed,		1.0,
		INP_Default,			10.0,
		IC_Visible,				FALSE,
		INP_DoNotifyChanged, TRUE,
		TAG_DONE);

	InRIBFile = AddInput("RIB file (must be a RIB archive!)", "rib_path", 
		LINKID_DataType,		CLSID_DataType_Text,
		INPID_InputControl,	FILECONTROL_ID,
		INP_Required,			FALSE,
		INP_External,			FALSE,
		INP_DoNotifyChanged,	TRUE,
		FCS_FilterString,		"rib (*.rib)|*.rib",		
		TAG_DONE);

	BaseClass::AddInputsTagList(tags);

	return true;
}

void SurfacePlaneInputs3D::NotifyChanged(Input *in, Parameter *param, TimeStamp time, TagList *tags)
{
	if (param)
	{
		if (in == InLockSizeXY)
		{
			LockSize = *((Number *)param) >= 0.5;

			if (InWidth)
				InWidth->SetAttrs(SLCS_LowName, LockSize ? "Size" : "Width", TAG_DONE);
			if (InHeight)
				InHeight->SetAttrs(SLCS_LowName, LockSize ? "" : "Height", IC_Visible, !LockSize, TAG_DONE);

			InSubdivisionWidth->SetAttrs(SLCS_LowName, LockSize ? "Subdivision Level" : "Subdivision Width", TAG_DONE);
			InSubdivisionHeight->SetAttrs(SLCS_LowName, LockSize ? "" : "Subdivision Height",
				IC_Visible,		!LockSize,
				TAG_DONE);
		}
	}

	BaseClass::NotifyChanged(in, param, time, tags);
}

Data3D *SurfacePlaneInputs3D::ProcessTagList(Request *req, Data3D *data, TagList &tags)
{
	SurfacePlaneData3D *ret = (SurfacePlaneData3D *) data;
	
	if (!ret)
		ret = new SurfacePlaneData3D;

	if (ret)
	{
		bool lockSize = *InLockSizeXY->GetValue(req) > 0.5;

		ret->WidthSub = *InSubdivisionWidth->GetValue(req);
		ret->HeightSub = lockSize ? ret->WidthSub : *InSubdivisionHeight->GetValue(req);
		
		Text *RibText = (Text *) InRIBFile->GetValue(req);
		ret->RIBPath = (const char *)*(RibText);

		if (IsImagePlane)
		{
			ret->Width = 1.0;
			ret->Height = 1.0;
		}
		else
		{
			ret->Width = *InWidth->GetValue(req); 
			ret->Height = lockSize ? ret->Width : *InHeight->GetValue(req);
		}
		
	}
	
	return BaseClass::ProcessTagList(req, ret, tags);
}

void SurfacePlaneInputs3D::ShowInputs(bool visible)
{
	if (visible)
	{
		InLockSizeXY->HideInputControls();
		InSubdivisionHeight->HideInputControls();
		InSubdivisionWidth->HideInputControls();

		if (IsImagePlane == false)
		{
			InWidth->HideInputControls();
			InHeight->HideInputControls();
		}
	}
	else
	{
		InLockSizeXY->HideInputControls();
		InSubdivisionHeight->HideInputControls();
		InSubdivisionWidth->HideInputControls();

		if (IsImagePlane == false)
		{
			InWidth->HideInputControls();
			InHeight->HideInputControls();
		}
	}

	BaseClass::ShowInputs(visible);
}





//
//
// SurfacePlaneData3D class
//
//
#undef ThisClass
#undef BaseClass
#define ThisClass SurfacePlaneData3D 
#define BaseClass SurfaceData3D
FuRegisterClass(COMPANY_ID_DOT + CLSID_Surface_Plane_Data, CT_SurfaceData3D)
	REGS_Name,					COMPANY_ID "Plane Surface Data",
	TAG_DONE);

SurfacePlaneData3D::SurfacePlaneData3D()
{
	m_RegNode = &Reg;
	Init();
}

SurfacePlaneData3D::SurfacePlaneData3D(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	Init();
}

void SurfacePlaneData3D::Init()
{
	Width = Height = 0;

	ClipType = PT_Splitter;
	QuickClipType = PT_Splitter;
}

SurfacePlaneData3D::SurfacePlaneData3D(const SurfacePlaneData3D &toCopy) : BaseClass(toCopy)
{
	Width			 = toCopy.Width;
	Height		 = toCopy.Height;
	WidthSub		 = toCopy.WidthSub;
	HeightSub	 = toCopy.HeightSub;
}

SurfacePlaneData3D::~SurfacePlaneData3D()
{
}

Data3D *SurfacePlaneData3D::CopyTagList(TagList &tags) // throws CMemoryException
{	
	return new SurfacePlaneData3D(*this);
}

bool SurfacePlaneData3D::BuildBoundingBox()
{
	float32 hWidth = 0.5f * Width;
	float32 hHeight = 0.5f * Height;
	
	AABBSet = true;
	AABB.SetValue(Vector3f(-hWidth, - hHeight, 0.0f), Vector3f(hWidth, hHeight, 0.0f));

	return true;
}

bool SurfacePlaneData3D::CreateGeometryTagList(std::vector<Geometry3D*> &geometry, TagList &tags) // throws CMemoryException
{
	float32 uscale = 0.5, vscale = 0.5, uoffset = 0.5, voffset = 0.5;
	int i, j, numberOfIndices, index1, index2;
	
	if (WidthSub < 1)
		WidthSub = 1;

	if (HeightSub < 1)
		HeightSub = 1;

	int n = (WidthSub + 1) * (HeightSub + 1);

	Geometry3D *g = NULL;
	IndexArray3D *ia = NULL;
	Stream3D *posStream = NULL;
	Stream3D *normStream = NULL;
	Stream3D *tcStream = NULL;

	try
	{
		g = NewGeometry();

		g->PrimitiveType = PT_Quads;
		g->NumPrimitives = WidthSub * HeightSub;
		numberOfIndices = g->NumPrimitives * 4;

		ia = IndexArray3D::Alloc(numberOfIndices);

		posStream  = Stream3D::Alloc(Stream_Position, CS_Model, n, 3);
		normStream = Stream3D::Alloc(Stream_Normal, CS_Model, n, 3);
		tcStream   = Stream3D::Alloc(Stream_TexCoord0, CS_Unset,  n, 2);
	}
	catch (CMemoryException *e)
	{
		SAFE_RECYCLE(g);
		SAFE_RECYCLE(ia);
		SAFE_RECYCLE(posStream);
		SAFE_RECYCLE(normStream);
		SAFE_RECYCLE(tcStream);
		throw e;
	}

	g->Indices = ia;

	g->Streams.Add(posStream);
	g->Streams.Add(normStream);
	g->Streams.Add(tcStream);

	normStream->Fill(Vector3f(0.0f, 0.0f, 1.0f));

	Vector3f *pos = (Vector3f *) posStream->Tuples;
	Vector2f *tc = (Vector2f *) tcStream->Tuples;

	float32 hwidth = Width * 0.5f;
	float32 hheight = Height * 0.5f;

	float32 dx = Width / WidthSub;
	float32 dy = Height / HeightSub;
	float32 dz = 0.0f;

	uscale = (hwidth >= 0.0001f ? 0.5f / hwidth : 0.0f);			// prevent NaNs
	vscale = (hheight >= 0.0001f ? 0.5f / hheight : 0.0f);

	Vector3f *vptr;
	Vector2f *tptr;

	float32 x, y, z;

	for (i = 0; i <= HeightSub; i++)
	{
		x = -hwidth;
		y = -hheight + i * dy;
		z = 0.0f;

		vptr = pos + i * (WidthSub + 1);
		tptr = tc + i * (WidthSub + 1);
		
		for (j = 0; j <= WidthSub; j++)
		{
			*vptr++ = Vector3f(x, y, z);
			*tptr++ = Vector2f(x * uscale + uoffset, y * vscale + voffset);
			x += dx;
		}
	}

	// set the indices
	uint32 *iptr = ia->Data;
	for (i = 0; i < HeightSub; i++)
	{
		index1 = i * (WidthSub + 1);
		index2 = (i + 1) * (WidthSub + 1);

		//if (UseTriangles)										// counter clockwise
		//{
		//	for (j = 0; j < WidthSub; j++)
		//	{
				// first triangle
		//		*iptr++ = index1;
		//		*iptr++ = index1 + 1;
		//		*iptr++ = index2 + 1;
				// second triangle
		//		*iptr++ = index1;
		//		*iptr++ = index2 + 1;
		//		*iptr++ = index2;
		//		index1++;
		//		index2++;
		//	}
		//}
		//else	// clockwise
		//{
			for (j = 0; j < WidthSub; j++)
			{
				// quad
				*iptr++ = index1;
				*iptr++ = index1 + 1;
				*iptr++ = index2 + 1;
				*iptr++ = index2;

				index1++;
				index2++;
			}
		//}
	}

	geometry.push_back(g);

	return true;
}