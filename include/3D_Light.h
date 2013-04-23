#ifndef _3D_LIGHT_H_
#define _3D_LIGHT_H_

#define SHININESS_LOOKUP_TABLE 1

#include "ThreadedOperator.h"
#include "StandardControls.h"
#include "VectorMath.h"
#include "3D_Common.h"

#define DEF_LIGHT_ENABLE 0
#define DEF_LIGHT_ID -1
#define DEF_LIGHT_MODEL -1

#define DEF_LIGHT_MODEL_LOCAL_VIEWER 0
#define DEF_LIGHT_MODEL_TWO_SIDED 0

#define DEF_LIGHT_MODEL_AMBIENT_R 0.0
#define DEF_LIGHT_MODEL_AMBIENT_G 0.0
#define DEF_LIGHT_MODEL_AMBIENT_B 0.0
#define DEF_LIGHT_MODEL_AMBIENT_A 1.0

#define DEF_LIGHT_AMBIENT_R 1.0
#define DEF_LIGHT_AMBIENT_G 1.0
#define DEF_LIGHT_AMBIENT_B 1.0
#define DEF_LIGHT_AMBIENT_A 1.0			// Alpha is meaningless for lights

#define DEF_LIGHT_DIFFUSE_R 1.0
#define DEF_LIGHT_DIFFUSE_G 1.0
#define DEF_LIGHT_DIFFUSE_B 1.0
#define DEF_LIGHT_DIFFUSE_A 1.0

#define DEF_LIGHT_SPECULAR_R 1.0
#define DEF_LIGHT_SPECULAR_G 1.0
#define DEF_LIGHT_SPECULAR_B 1.0
#define DEF_LIGHT_SPECULAR_A 1.0

#define DEF_LIGHT_POSITION_X 1.0
#define DEF_LIGHT_POSITION_Y 0.0
#define DEF_LIGHT_POSITION_Z 1.0
#define DEF_LIGHT_POSITION_W 0.0

#define DEF_LIGHT_VIEW_POSITION_X 0.0
#define DEF_LIGHT_VIEW_POSITION_Y 0.0
#define DEF_LIGHT_VIEW_POSITION_Z 0.0

#define DEF_LIGHT_DIRECTION_X 0.0
#define DEF_LIGHT_DIRECTION_Y 0.0
#define DEF_LIGHT_DIRECTION_Z -1.0

#define DEF_LIGHT_SPOT_EXPONENT 20.0
#define DEF_LIGHT_SPOT_INNER_CUTOFF 40.0
#define DEF_LIGHT_SPOT_OUTER_CUTOFF 45.0
#define DEF_LIGHT_SPOT_SAME_INNER_OUTER_CUTOFF FALSE

#define DEF_LIGHT_CONSTANT_ATTENUATION 0.0
#define DEF_LIGHT_LINEAR_ATTENUATION 0.0
#define DEF_LIGHT_QUADRATIC_ATTENUATION 1.0

#define DEF_LIGHT_INTENSITY 1.0
#define DEF_LIGHT_AMBIENT_INTENSITY 0.0
#define DEF_LIGHT_DIFFUSE_INTENSITY 1.0
#define DEF_LIGHT_SPECULAR_INTENSITY 0.3

#define DEF_LIGHT_ATTENUATION_TYPE 3.0
#define DEF_LIGHT_TYPE 0.0

#define MIN_LIGHT_SPOT_EXPONENT 0.0
#define MAX_LIGHT_SPOT_EXPONENT 128.0

#define MIN_LIGHT_SPOT_CUTOFF 0.0
#define MAX_LIGHT_SPOT_CUTOFF 90.0
#define LIGHT_SPOT_CUTOFF_SPECIAL 180.0

#define MIN_LIGHT_CONSTANT_ATTENUATION 0.0
#define MIN_LIGHT_LINEAR_ATTENUATION 0.0
#define MIN_LIGHT_QUADRATIC_ATTENUATION 0.0

#define MIN_LIGHT_INTENSITY 0.0

#define MIN_LIGHT_DECAY_RATE 0.0
#define DEF_LIGHT_DECAY_RATE 2.0

#define DEF_LIGHT_TX 0.0
#define DEF_LIGHT_TY 0.0
#define DEF_LIGHT_TZ -1.0
#define DEF_LIGHT_RX 0.0
#define DEF_LIGHT_RY 0.0
#define DEF_LIGHT_RZ 0.0
#define DEF_LIGHT_SX 1.0
#define DEF_LIGHT_SY 1.0
#define DEF_LIGHT_SZ 1.0
#define DEF_LIGHT_CX 0.0
#define DEF_LIGHT_CY 0.0
#define DEF_LIGHT_CZ 0.0

#define SHININESS_TABLE_SIZE 4096

enum PRLightParamType
{
	PR_Light_Undefined = -1,
	PR_Light_Ambient_Color = 0,
	PR_Light_Diffuse_Color,
	PR_Light_Specular_Color,
	PR_Light_View_Position,
	PR_Light_Position,
	PR_Light_Direction,
	PR_Light_Spot_Exponent,
	PR_Light_Spot_Inner_Cutoff,
	PR_Light_Spot_Outer_Cutoff,
	PR_Light_Constant_Attenuation,
	PR_Light_Linear_Attenuation,
	PR_Light_Quadratic_Attenuation,
	PR_Light_Intensity
};

enum PRLightModelType
{
	PR_Light_Model_Undefined = -1,
	PR_Light_Model_Ambient = 0,
	PR_Light_Model_Color,
	PR_Light_Model_Local_Viewer,
	PR_Light_Model_Two_Side
};

enum PRLightType
{
	PR_Light_Directional = 0,
	PR_Light_Point,
	PR_Light_Spot,
	PR_Light_Ambient,
	PR_Light_Area
};

// This is a light class, only used internally by the software DF renderer
class DFLight3D
{
protected:
	bool Enable;
	int LightID;
	int LightType;

#ifdef SHININESS_LOOKUP_TABLE
	double *ShininessTable;
#endif

	Matrix4 M;

	bool LightModelLocalViewer;
	bool LocalViewerPositionNonOrigin;
	double AmbientProductR, AmbientProductG, AmbientProductB, AmbientProductA;
	double DiffuseProductR, DiffuseProductG, DiffuseProductB, DiffuseProductA;
	double SpecularProductR, SpecularProductG, SpecularProductB, SpecularProductA;
	double MatShininess;
	
	double AmbientR, AmbientG, AmbientB, AmbientA;
	double DiffuseR, DiffuseG, DiffuseB, DiffuseA;
	double SpecularR, SpecularG, SpecularB, SpecularA;
	double ViewPositionX, ViewPositionY, ViewPositionZ;
	double PositionX, PositionY, PositionZ, PositionW;
	double TPositionX, TPositionY, TPositionZ, TPositionW; // transformed position
	double DirectionX, DirectionY, DirectionZ;
	double TDirectionX, TDirectionY, TDirectionZ; // transformed direction
	double SpotExponent, SpotInnerCutoff, SpotOuterCutoff;
	double CosSpotInnerCutoff, CosSpotOuterCutoff, CosSpotCutoffFactor;
	bool SpotSameInnerOuterCutoff;
	double ConstantAttenuation, LinearAttenuation, QuadraticAttenuation;
	double Intensity;

public:
	DFLight3D();
	~DFLight3D();
	
	void Init(void);
	
	void SetEnable(bool enable);
	void SetLightID(int lightID);

	void SetMatrix(Matrix4 &m);

	void SetAmbientProduct(double r, double g, double b, double a);
	void SetDiffuseProduct(double r, double g, double b, double a);
	void SetSpecularProduct(double r, double g, double b, double a);
	void SetMatShininess(double shininess);
	
	void SetLightModelLocalViewer(bool lightModelLocalViewer);
	void SetAmbient(double r, double g, double b, double a);
	void SetDiffuse(double r, double g, double b, double a);
	void SetSpecular(double r, double g, double b, double a);
	void SetViewPosition(double x, double y, double z);
	void SetPosition(double x, double y, double z, double w);
	void SetDirection(double dx, double dy, double dz);
	void SetSpotExponent(double exponent);
	void SetSpotInnerCutoff(double innerCutoff);
	void SetSpotOuterCutoff(double outerCutoff);
	void SetConstantAttenuation(double constantAttenuation);
	void SetLinearAttenuation(double linearAttenuation);
	void SetQuadraticAttenuation(double quadraticAttenuation);
	void SetIntensity(double intensity);

	bool GetEnable(void);
	int GetLightID(void);

	Matrix4 GetMatrix(void);

	bool GetLightModelLocalViewer(void);
	void GetAmbient(double &r, double &g, double &b, double &a);
	void GetDiffuse(double &r, double &g, double &b, double &a);
	void GetSpecular(double &r, double &g, double &b, double &a);
	void GetViewPosition(double &x, double &y, double &z);
	void GetPosition(double &x, double &y, double &z, double &w);
	void GetDirection(double &dx, double &dy, double &dz);
	double GetSpotExponent(void);
	double GetSpotInnerCutoff(void);
	double GetSpotOuterCutoff(void);
	double GetConstantAttenuation(void);
	double GetLinearAttenuation(void);
	double GetQuadraticAttenuation(void);
	double GetIntensity(void);
	void InitLightContribution(void);
	void GetLightContribution(
		double vx, double vy, double vz,
		double nx, double ny, double nz,
		double &r, double &g, double &b, double &a);
	int GetLightType(void);
	void GetTransformedPosition(double &x, double &y, double &z, double &w);
	void GetTransformedDirection(double &dx, double &dy, double &dz);
};

#endif
