#ifndef _3D_MATERIAL_H_
#define _3D_MATERIAL_H_


#include "Parameter.h"
#include "LockableObject.h"

#include "3D_BaseData.h"




//
//
//class MtlImplNode3D
//{
//public:
//	MtlImplSW3D *Data;	
//
//public:
//};
//
//
//class MtlImplGraph3D : public Object
//{
//	FuDeclareClass(MtlImplGraph3D, Object);
//
//public:
//
//public:
//};
//
//




// This is old code, not sure if its still used in the renderer used for gridwarp, etc
#include "StandardControls.h"
#include "3D_Common.h"

#define DEF_MTL_ID 0

#define DEF_MTL_AMBIENT_R 0.2
#define DEF_MTL_AMBIENT_G 0.2
#define DEF_MTL_AMBIENT_B 0.2
#define DEF_MTL_AMBIENT_A 1.0

#define DEF_MTL_DIFFUSE_R 0.8
#define DEF_MTL_DIFFUSE_G 0.8
#define DEF_MTL_DIFFUSE_B 0.8
#define DEF_MTL_DIFFUSE_A 1.0

#define DEF_MTL_SPECULAR_R 1.0
#define DEF_MTL_SPECULAR_G 1.0
#define DEF_MTL_SPECULAR_B 1.0
#define DEF_MTL_SPECULAR_A 1.0

#define DEF_MTL_EMISSION_R 0.0
#define DEF_MTL_EMISSION_G 0.0
#define DEF_MTL_EMISSION_B 0.0
#define DEF_MTL_EMISSION_A 1.0

#define DEF_MTL_SHININESS 5.0

#define MIN_MTL_SHININESS 1.0
#define MAX_MTL_SHININESS 128.0


enum PRMaterialParamType
{
	PR_Material_Undefined = -1,
	PR_Material_Ambient = 0,
	PR_Material_Diffuse,
	PR_Material_Specular,
	PR_Material_Emission,
	PR_Material_Shininess,
	PR_Material_Ambient_And_Diffuse,
	PR_Material_Color_Indexes
};

enum PRMaterialFaceType
{
	PR_Material_Undefined_Face = -1,
	PR_Material_Front_Face = 0,
	PR_Material_Back_Face,
	PR_Material_Front_And_Back_Face
};

class MaterialData3D
{
protected:
	int MaterialID;

	float FrontFaceAmbientR, FrontFaceAmbientG, FrontFaceAmbientB, FrontFaceAmbientA;
	float FrontFaceDiffuseR, FrontFaceDiffuseG, FrontFaceDiffuseB, FrontFaceDiffuseA;
	float FrontFaceSpecularR, FrontFaceSpecularG, FrontFaceSpecularB, FrontFaceSpecularA;
	float FrontFaceEmissionR, FrontFaceEmissionG, FrontFaceEmissionB, FrontFaceEmissionA;
	float FrontFaceShininess;

	float BackFaceAmbientR, BackFaceAmbientG, BackFaceAmbientB, BackFaceAmbientA;
	float BackFaceDiffuseR, BackFaceDiffuseG, BackFaceDiffuseB, BackFaceDiffuseA;
	float BackFaceSpecularR, BackFaceSpecularG, BackFaceSpecularB, BackFaceSpecularA;
	float BackFaceEmissionR, BackFaceEmissionG, BackFaceEmissionB, BackFaceEmissionA;
	float BackFaceShininess;

public:
	MaterialData3D();
	~MaterialData3D();
	
	void Init(void);
	
	void SetMaterialID(int materialID);

	void SetAmbient(int faceType, float r, float g, float b, float a);
	void SetDiffuse(int faceType, float r, float g, float b, float a);
	void SetSpecular(int faceType, float r, float g, float b, float a);
	void SetEmission(int faceType, float r, float g, float b, float a);
	void SetShininess(int faceType, float shininess);

	int GetMaterialID(void);

	void GetAmbient(int faceType, float &r, float &g, float &b, float &a);
	void GetDiffuse(int faceType, float &r, float &g, float &b, float &a);
	void GetSpecular(int faceType, float &r, float &g, float &b, float &a);
	void GetEmission(int faceType, float &r, float &g, float &b, float &a);
	float GetShininess(int faceType);

	bool operator != (MaterialData3D& m);
};

#endif
