#ifndef _RI_DLL_3D_H_
#define _RI_DLL_3D_H_

#include "Object.h"
#include "3D_RiSymbols.h"


extern const FuID CLSID_RmDLL;





// RiDll3D - wrapper for a renderman compliant dll

class RmDLL : public Object
{
	FuDeclareClass(RmDLL, Object);

public:

public:
	RmDLL();
	RmDLL(const Registry *reg, const ScriptVal &table, const TagList &tags);
	virtual ~RmDLL();

	bool AttachTo(HMODULE &dll);
	void Detach();

public:
	RtBasis *RiBezierBasis, *RiBSplineBasis, *RiCatmullRomBasis, *RiHermiteBasis, *RiPowerBasis;

	PfnRiBoxFilter RiBoxFilter;
	PfnRiTriangleFilter RiTriangleFilter;
	PfnRiCatmullRomFilter RiCatmullRomFilter;
	PfnRiGaussianFilter RiGaussianFilter;
	PfnRiSincFilter RiSincFilter;

	RtFilterFunc PixelFilterFunc[5];

	PfnRiErrorIgnore RiErrorIgnore;
	PfnRiErrorPrint RiErrorPrint;
	PfnRiErrorAbort RiErrorAbort;

	PfnRiProcDelayedReadArchive RiProcDelayedReadArchive;
	PfnRiProcRunProgram RiProcRunProgram;
	PfnRiProcDynamicLoad RiProcDynamicLoad;

	PfnRiGetContext RiGetContext;
	PfnRiContext RiContext;

	PfnRiDeclare RiDeclare;

	PfnRiBegin RiBegin;
	PfnRiEnd RiEnd;
	PfnRiFrameBegin RiFrameBegin;
	PfnRiFrameEnd RiFrameEnd;
	PfnRiWorldBegin RiWorldBegin;
	PfnRiWorldEnd RiWorldEnd;

	PfnRiFormat RiFormat;
	PfnRiFrameAspectRatio RiFrameAspectRatio;
	PfnRiScreenWindow RiScreenWindow;
	PfnRiCropWindow RiCropWindow;
	PfnRiProjection RiProjection;
	PfnRiProjectionV RiProjectionV;
	PfnRiClipping RiClipping;
	PfnRiClippingPlane RiClippingPlane;
	PfnRiShutter RiShutter;
	PfnRiDepthOfField RiDepthOfField;

	PfnRiPixelVariance RiPixelVariance;
	PfnRiPixelSamples RiPixelSamples;
	PfnRiPixelFilter RiPixelFilter;
	PfnRiExposure RiExposure;
	PfnRiImager RiImager;
	PfnRiImagerV RiImagerV;
	PfnRiQuantize RiQuantize;
	PfnRiDisplay RiDisplay;
	PfnRiDisplayV RiDisplayV;
	PfnRiDisplayChannel RiDisplayChannel;
	PfnRiDisplayChannelV RiDisplayChannelV;

	PfnRiHider RiHider;
	PfnRiHiderV RiHiderV;
	PfnRiColorSamples RiColorSamples;
	PfnRiRelativeDetail RiRelativeDetail;
	PfnRiOption RiOption;
	PfnRiOptionV RiOptionV;

	PfnRiAttributeBegin RiAttributeBegin;
	PfnRiAttributeEnd RiAttributeEnd;
	PfnRiColor RiColor;
	PfnRiOpacity RiOpacity;
	PfnRiTextureCoordinates RiTextureCoordinates;

	PfnRiLightSource RiLightSource;
	PfnRiLightSourceV RiLightSourceV;
	PfnRiAreaLightSource RiAreaLightSource;
	PfnRiAreaLightSourceV RiAreaLightSourceV;

	PfnRiIlluminate RiIlluminate;
	PfnRiSurface RiSurface;
	PfnRiSurfaceV RiSurfaceV;
	PfnRiAtmosphere RiAtmosphere;
	PfnRiAtmosphereV RiAtmosphereV;
	PfnRiInterior RiInterior;
	PfnRiInteriorV RiInteriorV;
	PfnRiExterior RiExterior;
	PfnRiExteriorV RiExteriorV;
	PfnRiShadingRate RiShadingRate;
	PfnRiShadingInterpolation RiShadingInterpolation;
	PfnRiMatte RiMatte;

	PfnRiBound RiBound;
	PfnRiDetail RiDetail;
	PfnRiDetailRange RiDetailRange;
	PfnRiGeometricApproximation RiGeometricApproximation;
	PfnRiOrientation RiOrientation;
	PfnRiReverseOrientation RiReverseOrientation;
	PfnRiSides RiSides;

	PfnRiIdentity RiIdentity;
	PfnRiTransform RiTransform;
	PfnRiConcatTransform RiConcatTransform;
	PfnRiPerspective RiPerspective;
	PfnRiTranslate RiTranslate;
	PfnRiRotate RiRotate;
	PfnRiScale RiScale;
	PfnRiSkew RiSkew;
	PfnRiDeformation RiDeformation;
	PfnRiDeformationV RiDeformationV;
	PfnRiDisplacement RiDisplacement;
	PfnRiDisplacementV RiDisplacementV;
	PfnRiCoordinateSystem RiCoordinateSystem;
	PfnRiCoordSysTransform RiCoordSysTransform;

	PfnRiTransformPoints RiTransformPoints;

	PfnRiTransformBegin RiTransformBegin;
	PfnRiTransformEnd RiTransformEnd;

	PfnRiAttribute RiAttribute;
	PfnRiAttributeV RiAttributeV;

	PfnRiPolygon RiPolygon;
	PfnRiPolygonV RiPolygonV;
	PfnRiGeneralPolygon RiGeneralPolygon;
	PfnRiGeneralPolygonV RiGeneralPolygonV;
	PfnRiPointsPolygons RiPointsPolygons;
	PfnRiPointsPolygonsV RiPointsPolygonsV;
	PfnRiPointsGeneralPolygons RiPointsGeneralPolygons;
	PfnRiPointsGeneralPolygonsV RiPointsGeneralPolygonsV;
	PfnRiBasis RiBasis;
	PfnRiPatch RiPatch;
	PfnRiPatchV RiPatchV;
	PfnRiPatchMesh RiPatchMesh;
	PfnRiPatchMeshV RiPatchMeshV;
	PfnRiNuPatch RiNuPatch;
	PfnRiNuPatchV RiNuPatchV;
	PfnRiTrimCurve RiTrimCurve;

	PfnRiSphere RiSphere;
	PfnRiSphereV RiSphereV;
	PfnRiCone RiCone;
	PfnRiConeV RiConeV;
	PfnRiCylinder RiCylinder;
	PfnRiCylinderV RiCylinderV;
	PfnRiHyperboloid RiHyperboloid;
	PfnRiHyperboloidV RiHyperboloidV;
	PfnRiParaboloid RiParaboloid;
	PfnRiParaboloidV RiParaboloidV;
	PfnRiDisk RiDisk;
	PfnRiDiskV RiDiskV;
	PfnRiTorus RiTorus;
	PfnRiTorusV RiTorusV;

	PfnRiBlobby RiBlobby;
	PfnRiBlobbyV RiBlobbyV;

	PfnRiCurves RiCurves;
	PfnRiCurvesV RiCurvesV;
	PfnRiPoints RiPoints;
	PfnRiPointsV RiPointsV;
	PfnRiSubdivisionMesh RiSubdivisionMesh;
	PfnRiSubdivisionMeshV RiSubdivisionMeshV;

	PfnRiProcedural RiProcedural;
	PfnRiGeometry RiGeometry;
	PfnRiGeometryV RiGeometryV;

	PfnRiSolidBegin RiSolidBegin;
	PfnRiSolidEnd RiSolidEnd;

	PfnRiObjectBegin RiObjectBegin;
	PfnRiObjectBeginV RiObjectBeginV;

	PfnRiObjectEnd RiObjectEnd;
	PfnRiObjectInstance RiObjectInstance;
	PfnRiMotionBegin RiMotionBegin;
	PfnRiMotionBeginV RiMotionBeginV;
	PfnRiMotionEnd RiMotionEnd;

	PfnRiMakeTexture RiMakeTexture;
	PfnRiMakeTextureV RiMakeTextureV;
	PfnRiMakeBump RiMakeBump;
	PfnRiMakeBumpV RiMakeBumpV;
	PfnRiMakeLatLongEnvironment RiMakeLatLongEnvironment;
	PfnRiMakeLatLongEnvironmentV RiMakeLatLongEnvironmentV;
	PfnRiMakeCubeFaceEnvironment RiMakeCubeFaceEnvironment;
	PfnRiMakeCubeFaceEnvironmentV RiMakeCubeFaceEnvironmentV;
	PfnRiMakeShadow RiMakeShadow;
	PfnRiMakeShadowV RiMakeShadowV;

	PfnRiArchiveRecord RiArchiveRecord;
	PfnRiReadArchive RiReadArchive;
	PfnRiReadArchiveV RiReadArchiveV;

	PfnRiErrorHandler RiErrorHandler;

	PfnPRManBegin PRManBegin;
	PfnPRManEnd PRManEnd;
};


#endif