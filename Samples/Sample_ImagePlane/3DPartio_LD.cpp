//@ Project "Fu3D"

#include "stdafx.h"

#define MODULENAME "3DImagePlane2"

#define BaseClass Transform3DOperator
#define ThisClass ImagePlane3D2

#define ALLOW_GL_COLOR_FUNCTIONS
#define ALLOW_LIGHT_MODEL_COLOR_CONTROL

#ifdef FuPLUGIN
#include "FuPlugin.h"
#endif

#include "PipelineDoc.h"

#include "3D_Scene.h"
#include "3D_MtlGraph.h"
#include "3DPartio_LD.h"
#include "3D_MaterialBase.h"
#include "3D_TextureImage.h"
#include "3D_MaterialConstant.h"

FuRegisterClass(COMPANY_ID_DOT "PartioLD", CT_3DFilterSource)		// for internal eyeon builds COMPANY_ID_DOT/COMPANY_ID_SPACE are ""
	REGS_Name,					COMPANY_ID_SPACE "Partio Loader",
	REGS_Category,				COMPANY_ID_DBS "3D",
	REG_TileID,					IDB_TILE_3D_IMAGE,
	REGS_OpIconString,		"Partio_LD",
	REGS_OpDescription,		"Reads a Partio and send it the RIB Renderer",
	//REGS_HelpTopic,			"Tools/3D/ImagePlane",
	REG_OpNoMask,				TRUE,		// perhaps this can be used in some other way
//	REG_NoAutoProxy,			TRUE,
	REG_NoMotionBlurCtrls,	TRUE,
	REG_ToolbarDefault,		TRUE,
	TAG_DONE);




ImagePlane3D2::ImagePlane3D2(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{
	// add the plane's geometry inputs to the controls page
	AddControlPage("Controls");

	if (InSurfacePlane = (PartioPlaneInputs3D *) ClassRegistry->New(COMPANY_ID_DOT + CLSID_Partio_Plane_Inputs, CT_SurfaceInputs3D, table, tags))
	{
		RegisterInputs(InSurfacePlane);
		InSurfacePlane->IsImagePlane = true;

		InSurfacePlane->SetAttrs(
			SI_ObjectIDInputName,		"ObjectID",
			SI_ObjectIDInputID,			"ObjectID",
			TAG_DONE);

		const FuID &id = InSurfacePlane->m_RegNode->m_ID;

		BeginGroup(id, id, true, false);
		InSurfacePlane->AddInputs();
		EndGroup();
	}


	 //add the main material input
	InMaterial = AddInput("MaterialInput", "MaterialInput",
		LINKID_DataType,			CLSID_DataType_MtlGraph3D,
		LINKID_AllowedDataType,	CLSID_DataType_Image,
		LINKID_AllowedDataType,	CLSID_DataType_Mask,
		LINK_Main,					GetNextMainValue(),
		LINKID_LegacyID,			FuID("Input"),					// the ID must remain "Input" so as not to break existing Fusion5 flows
		INP_Required,				FALSE, 
		INP_ConnectRequired,		TRUE,
		IC_Visible,				FALSE,
		TAG_DONE);
		


	// add the default material inputs which get used when an external material is not connected to InMaterial
	AddControlPage("Material");
	
	InDefaultMaterial = (MtlStdInputs3D *) ClassRegistry->New(CLSID_MtlStdInputs, CT_MtlInputs3D, table, tags);
	RegisterInputs(InDefaultMaterial);

	BeginGroup(InDefaultMaterial->m_RegNode->m_ID, InDefaultMaterial->m_RegNode->m_ID, true, false);
	InDefaultMaterial->AddInputs(MSI_DiffuseTextureInput,	InMaterial, TAG_DONE);
	EndGroup();

	UsingDefaultMaterial = true;
}

ImagePlane3D2::~ImagePlane3D2()
{
	SAFE_RECYCLE(InSurfacePlane);
	SAFE_RECYCLE(InDefaultMaterial);
}

void ImagePlane3D2::CheckRequest(Request *req)
{
	BaseClass::CheckRequest(req);

	if (req->GetPri() == 0 && req->GetBaseTime() != TIME_UNDEFINED)
		req->SetInputTime(InMaterial, req->GetBaseTime());
}

Node3D *ImagePlane3D2::CreateScene(Request *req, TagList *tags)
{
	Node3D *planeNode = new Node3D(NDET_Surface);

	bool failed = false;
	
	if (planeNode)
	{
		planeNode->SetID((uint_ptr) this);		// use the operator pointer as the id
		planeNode->SetSubID(0);
		planeNode->SetName(GetName());
		planeNode->SetTransformMatrix(Matrix4f());

		MtlData3D *mtl;
		if (UsingDefaultMaterial)
		{
			mtl = (MtlData3D *) InDefaultMaterial->CreateData(req);
			InDefaultMaterial->Process(req, mtl);
		}
		else
			mtl = ConvertToMtlData(InMaterial->GetValue(req));

		float32 dims[4];
		mtl->GetNaturalDims(dims);

		PartioPlaneData3D *surface = (PartioPlaneData3D *) InSurfacePlane->Process(req, NULL);

		if (surface)
		{
			int nMtls = surface->GetNumMtlSlots();
			for (int i = 0; i < nMtls; i++)			// Note: nMtls = 1 for the plane surface so this loop really not necessary
			{
				if (i > 0)
					mtl->Use();
				planeNode->AddMtl(mtl, surface->GetMtlSlotName(i));
			}

			float32 w = dims[0];
			float32 h = dims[1];
			float32 wa = dims[2];
			float32 ha = h * dims[3] / w;

			surface->Width = wa;
			surface->Height = ha;

			SAFE_ASSIGN(planeNode->Data, surface);
		}
		else
			failed = true;
	}
	else
		failed = true;

	if (failed)
		SAFE_DELETE(planeNode);

	return planeNode;
}

bool ImagePlane3D2::OnConnect(Link *src, Link *dest, TagList *tags)
{
	bool ret = BaseClass::OnConnect(src, dest, tags);

	if (src == InMaterial)
	{
		if (!dest || dest->m_DataType == CLSID_DataType_Image || dest->m_DataType == CLSID_DataType_Mask)		// if we're switching to the default material (ie. disconnecting or input is the diffuse image)
		{
			if (!UsingDefaultMaterial)
			{
				UsingDefaultMaterial = true;

				if (InDefaultMaterial)
					InDefaultMaterial->UsingExternalMaterial(false);
			}
		}
		else
		{
			FuASSERT(dest->m_DataType == CLSID_DataType_MtlGraph3D, (""));

			if (UsingDefaultMaterial)
			{
				UsingDefaultMaterial = false;

				if (InDefaultMaterial)
					InDefaultMaterial->UsingExternalMaterial(true);
			}
		}
	}

	return ret;
}
