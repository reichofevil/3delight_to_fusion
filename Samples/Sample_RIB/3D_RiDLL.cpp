//@ Project "Fu3D"

#include "stdafx.h"

#define MODULENAME "3DRiDLL"

#define BaseClass Object
#define ThisClass RmDLL

#ifdef FuPLUGIN
#include "FuPlugin.h"		// make sure you change the macro COMPANY_ID in FuPlugin.h which defaults to "samples"
#endif

#include "3D_RiDLL.h"
#include "RegistryLists.h"



const FuID CLSID_RmDLL = "RmDLL";
const FuID CLSID_Light_Env_Data = "";
const FuID CLSID_Light_AO_Data = "";
//
//
// RiDll3D
//
//

FuRegisterClass(COMPANY_ID_DOT + CLSID_RmDLL, CT_Any)
	REGS_Name,					"RmDLL",
	REGS_AboutDescription,	"Wrapper for a Renderman DLL",
	TAG_DONE);

RmDLL::RmDLL() 
{
}

RmDLL::RmDLL(const Registry *reg, const ScriptVal &table, const TagList &tags) : BaseClass(reg, table, tags)
{	
}

RmDLL::~RmDLL() 
{
}

static RtToken GetVarAddress(HMODULE &dll, const char *name)
{
	RtToken *tok = (RtToken *) GetProcAddress(dll, name);
	return tok ? *tok : NULL;
}

bool RmDLL::AttachTo(HMODULE &dll) 
{
	// We need to be careful here.  The problem comes in when using renderers that are not fully compliant with the Renderman specification.
	// Some renderers will not implement or not export some of the Ri function below and because we're checking "success" for all them some
	// renderers will fail to load.  If that happens you can probably just comment out all the "success" checks and everything will be fine
	// since its usually a more obscure function that fails.
	bool success = true;

	RiBezierBasis		= (RtBasis *) GetProcAddress(dll, "RiBezierBasis");
	RiBSplineBasis		= (RtBasis *) GetProcAddress(dll, "RiBSplineBasis");
	RiCatmullRomBasis	= (RtBasis *) GetProcAddress(dll, "RiCatmullRomBasis");
	RiHermiteBasis		= (RtBasis *) GetProcAddress(dll, "RiHermiteBasis");
	RiPowerBasis		= (RtBasis *) GetProcAddress(dll, "RiPowerBasis");
	success = success && RiBezierBasis && RiBSplineBasis && RiCatmullRomBasis && RiHermiteBasis && RiPowerBasis;

	RiBoxFilter				= (PfnRiBoxFilter) GetProcAddress(dll, "RiBoxFilter");
	RiTriangleFilter		= (PfnRiTriangleFilter) GetProcAddress(dll, "RiTriangleFilter");
	RiCatmullRomFilter	= (PfnRiCatmullRomFilter) GetProcAddress(dll, "RiCatmullRomFilter");
	RiGaussianFilter		= (PfnRiGaussianFilter) GetProcAddress(dll, "RiGaussianFilter");
	RiSincFilter			= (PfnRiSincFilter) GetProcAddress(dll, "RiSincFilter");
	success = success && RiGaussianFilter && RiBoxFilter && RiTriangleFilter && RiCatmullRomFilter && RiSincFilter;

	PixelFilterFunc[0] = RiBoxFilter;
	PixelFilterFunc[1] = RiTriangleFilter;
	PixelFilterFunc[2] = RiCatmullRomFilter;
	PixelFilterFunc[3] = RiGaussianFilter;
	PixelFilterFunc[4] = RiSincFilter;

	RiErrorIgnore	= (PfnRiErrorIgnore) GetProcAddress(dll, "RiErrorIgnore");
	RiErrorPrint	= (PfnRiErrorPrint) GetProcAddress(dll, "RiErrorPrint");
	RiErrorAbort	= (PfnRiErrorAbort) GetProcAddress(dll, "RiErrorAbort");
	success = success && RiErrorIgnore && RiErrorPrint && RiErrorAbort;

	RiProcDelayedReadArchive	= (PfnRiProcDelayedReadArchive) GetProcAddress(dll, "RiProcDelayedReadArchive");
	RiProcRunProgram				= (PfnRiProcRunProgram) GetProcAddress(dll, "RiProcRunProgram");
	RiProcDynamicLoad				= (PfnRiProcDynamicLoad) GetProcAddress(dll, "RiProcDynamicLoad");
	success = success && RiProcDelayedReadArchive && RiProcRunProgram && RiProcDynamicLoad;

	RiGetContext	= (PfnRiGetContext) GetProcAddress(dll, "RiGetContext");
	RiContext		= (PfnRiContext) GetProcAddress(dll, "RiContext");
	success = success && RiGetContext && RiContext;

	RiDeclare = (PfnRiDeclare) GetProcAddress(dll, "RiDeclare");
	success = success && RiDeclare;

	RiBegin			= (PfnRiBegin) GetProcAddress(dll, "RiBegin");
	RiEnd				= (PfnRiEnd) GetProcAddress(dll, "RiEnd");
	RiFrameBegin	= (PfnRiFrameBegin) GetProcAddress(dll, "RiFrameBegin");
	RiFrameEnd		= (PfnRiFrameEnd) GetProcAddress(dll, "RiFrameEnd");
	RiWorldBegin	= (PfnRiWorldBegin) GetProcAddress(dll, "RiWorldBegin");
	RiWorldEnd		= (PfnRiWorldEnd) GetProcAddress(dll, "RiWorldEnd");
	success = success && RiBegin && RiEnd && RiFrameBegin && RiFrameEnd && RiWorldBegin && RiWorldEnd;

	RiFormat					= (PfnRiFormat) GetProcAddress(dll, "RiFormat");
	RiFrameAspectRatio	= (PfnRiFrameAspectRatio) GetProcAddress(dll, "RiFrameAspectRatio");
	RiScreenWindow			= (PfnRiScreenWindow) GetProcAddress(dll, "RiScreenWindow");
	RiCropWindow			= (PfnRiCropWindow) GetProcAddress(dll, "RiCropWindow");
	RiProjection			= (PfnRiProjection) GetProcAddress(dll, "RiProjection");
	RiProjectionV			= (PfnRiProjectionV) GetProcAddress(dll, "RiProjectionV");
	RiClipping				= (PfnRiClipping) GetProcAddress(dll, "RiClipping");
	RiClippingPlane		= (PfnRiClippingPlane) GetProcAddress(dll, "RiClippingPlane");
	RiShutter				= (PfnRiShutter) GetProcAddress(dll, "RiShutter");
	RiDepthOfField			= (PfnRiDepthOfField) GetProcAddress(dll, "RiDepthOfField");
	success = success && RiFormat && RiFrameAspectRatio && RiScreenWindow && RiCropWindow && RiProjection && RiProjectionV && RiClipping && RiClippingPlane && RiShutter && RiDepthOfField;

	RiPixelVariance	= (PfnRiPixelVariance) GetProcAddress(dll, "RiPixelVariance");
	RiPixelSamples		= (PfnRiPixelSamples) GetProcAddress(dll, "RiPixelSamples");
	RiPixelFilter		= (PfnRiPixelFilter) GetProcAddress(dll, "RiPixelFilter");
	RiExposure			= (PfnRiExposure) GetProcAddress(dll, "RiExposure");
	RiImager				= (PfnRiImager) GetProcAddress(dll, "RiImager");
	RiImagerV			= (PfnRiImagerV) GetProcAddress(dll, "RiImagerV");
	RiQuantize			= (PfnRiQuantize) GetProcAddress(dll, "RiQuantize");
	RiDisplay			= (PfnRiDisplay) GetProcAddress(dll, "RiDisplay");
	RiDisplayV			= (PfnRiDisplayV) GetProcAddress(dll, "RiDisplayV");
	RiDisplayChannel	= (PfnRiDisplayChannel) GetProcAddress(dll, "RiDisplayChannel");
	RiDisplayChannelV	= (PfnRiDisplayChannelV) GetProcAddress(dll, "RiDisplayChannelV");
	success = success && RiPixelVariance && RiPixelSamples && RiPixelFilter && RiExposure && RiImager && RiImagerV && RiQuantize && RiDisplay && RiDisplayV && RiDisplayChannel && RiDisplayChannelV;

	RiHider				= (PfnRiHider) GetProcAddress(dll, "RiHider");
	RiHiderV				= (PfnRiHiderV) GetProcAddress(dll, "RiHiderV");
	RiColorSamples		= (PfnRiColorSamples) GetProcAddress(dll, "RiColorSamples");
	RiRelativeDetail	= (PfnRiRelativeDetail) GetProcAddress(dll, "RiRelativeDetail");
	RiOption				= (PfnRiOption) GetProcAddress(dll, "RiOption");
	RiOptionV			= (PfnRiOptionV) GetProcAddress(dll, "RiOptionV");
	success = success && RiHider && RiHiderV && RiColorSamples && RiRelativeDetail && RiOption && RiOptionV;

	RiAttributeBegin			= (PfnRiAttributeBegin) GetProcAddress(dll, "RiAttributeBegin");
	RiAttributeEnd				= (PfnRiAttributeEnd) GetProcAddress(dll, "RiAttributeEnd");
	RiColor						= (PfnRiColor) GetProcAddress(dll, "RiColor");
	RiOpacity					= (PfnRiOpacity) GetProcAddress(dll, "RiOpacity");
	RiTextureCoordinates		= (PfnRiTextureCoordinates) GetProcAddress(dll, "RiTextureCoordinates");
	success = success && RiAttributeBegin && RiAttributeEnd && RiColor && RiOpacity && RiTextureCoordinates;

	RiLightSource			= (PfnRiLightSource) GetProcAddress(dll, "RiLightSource");
	RiLightSourceV			= (PfnRiLightSourceV) GetProcAddress(dll, "RiLightSourceV");
	RiAreaLightSource		= (PfnRiAreaLightSource) GetProcAddress(dll, "RiAreaLightSource");
	RiAreaLightSourceV	= (PfnRiAreaLightSourceV) GetProcAddress(dll, "RiAreaLightSourceV");
	success = success && RiLightSource && RiLightSourceV && RiAreaLightSource && RiAreaLightSourceV;

	RiIlluminate				= (PfnRiIlluminate) GetProcAddress(dll, "RiIlluminate");
	RiSurface					= (PfnRiSurface) GetProcAddress(dll, "RiSurface");
	RiSurfaceV					= (PfnRiSurfaceV) GetProcAddress(dll, "RiSurfaceV");
	RiAtmosphere				= (PfnRiAtmosphere) GetProcAddress(dll, "RiAtmosphere");
	RiAtmosphereV				= (PfnRiAtmosphereV) GetProcAddress(dll, "RiAtmosphereV");
	RiInterior					= (PfnRiInterior) GetProcAddress(dll, "RiInterior");
	RiInteriorV					= (PfnRiInteriorV) GetProcAddress(dll, "RiInteriorV");
	RiExterior					= (PfnRiExterior) GetProcAddress(dll, "RiExterior");
	RiExteriorV					= (PfnRiExteriorV) GetProcAddress(dll, "RiExteriorV");
	RiShadingRate				= (PfnRiShadingRate) GetProcAddress(dll, "RiShadingRate");
	RiShadingInterpolation	= (PfnRiShadingInterpolation) GetProcAddress(dll, "RiShadingInterpolation");
	RiMatte						= (PfnRiMatte) GetProcAddress(dll, "RiMatte");
	success = success && RiIlluminate && RiSurface && RiSurfaceV && RiAtmosphere && RiAtmosphereV && RiInterior && RiInteriorV && RiExterior && RiExteriorV && RiShadingRate && RiShadingInterpolation && RiMatte;

	RiBound							= (PfnRiBound) GetProcAddress(dll, "RiBound");
	RiDetail							= (PfnRiDetail) GetProcAddress(dll, "RiDetail");
	RiDetailRange					= (PfnRiDetailRange) GetProcAddress(dll, "RiDetailRange");
	RiGeometricApproximation	= (PfnRiGeometricApproximation) GetProcAddress(dll, "RiGeometricApproximation");
	RiOrientation					= (PfnRiOrientation) GetProcAddress(dll, "RiOrientation");
	RiReverseOrientation			= (PfnRiReverseOrientation) GetProcAddress(dll, "RiReverseOrientation");
	RiSides							= (PfnRiSides) GetProcAddress(dll, "RiSides");
	success = success && RiBound && RiDetail && RiDetailRange && RiGeometricApproximation && RiOrientation && RiReverseOrientation && RiSides;

	RiIdentity				= (PfnRiIdentity) GetProcAddress(dll, "RiIdentity");
	RiTransform				= (PfnRiTransform) GetProcAddress(dll, "RiTransform");
	RiConcatTransform		= (PfnRiConcatTransform) GetProcAddress(dll, "RiConcatTransform");
	RiPerspective			= (PfnRiPerspective) GetProcAddress(dll, "RiPerspective");
	RiTranslate				= (PfnRiTranslate) GetProcAddress(dll, "RiTranslate");
	RiRotate					= (PfnRiRotate) GetProcAddress(dll, "RiRotate");
	RiScale					= (PfnRiScale) GetProcAddress(dll, "RiScale");
	RiSkew					= (PfnRiSkew) GetProcAddress(dll, "RiSkew");
	RiDeformation			= (PfnRiDeformation) GetProcAddress(dll, "RiDeformation");
	RiDeformationV			= (PfnRiDeformationV) GetProcAddress(dll, "RiDeformationV");
	RiDisplacement			= (PfnRiDisplacement) GetProcAddress(dll, "RiDisplacement");
	RiDisplacementV		= (PfnRiDisplacementV) GetProcAddress(dll, "RiDisplacementV");
	RiCoordinateSystem	= (PfnRiCoordinateSystem) GetProcAddress(dll, "RiCoordinateSystem");
	RiCoordSysTransform	= (PfnRiCoordSysTransform) GetProcAddress(dll, "RiCoordSysTransform");
	success = success && RiIdentity && RiTransform && RiConcatTransform && RiPerspective && RiTranslate && RiRotate && RiScale && RiSkew && RiDeformation && RiDeformationV && RiDisplacement && RiDisplacementV && RiCoordinateSystem && RiCoordSysTransform;

	RiTransformPoints = (PfnRiTransformPoints) GetProcAddress(dll, "RiTransformPoints");
	success = success && RiTransformPoints;

	RiTransformBegin	= (PfnRiTransformBegin) GetProcAddress(dll, "RiTransformBegin");
	RiTransformEnd		= (PfnRiTransformEnd) GetProcAddress(dll, "RiTransformEnd");
	success = success && RiTransformBegin && RiTransformEnd;

	RiAttribute		= (PfnRiAttribute) GetProcAddress(dll, "RiAttribute");
	RiAttributeV	= (PfnRiAttributeV) GetProcAddress(dll, "RiAttributeV");
	success = success && RiAttribute && RiAttributeV;

	RiPolygon						= (PfnRiPolygon) GetProcAddress(dll, "RiPolygon");
	RiPolygonV						= (PfnRiPolygonV) GetProcAddress(dll, "RiPolygonV");
	RiGeneralPolygon				= (PfnRiGeneralPolygon) GetProcAddress(dll, "RiGeneralPolygon");
	RiGeneralPolygonV				= (PfnRiGeneralPolygonV) GetProcAddress(dll, "RiGeneralPolygonV");
	RiPointsPolygons				= (PfnRiPointsPolygons) GetProcAddress(dll, "RiPointsPolygons");
	RiPointsPolygonsV				= (PfnRiPointsPolygonsV) GetProcAddress(dll, "RiPointsPolygonsV");
	RiPointsGeneralPolygons		= (PfnRiPointsGeneralPolygons) GetProcAddress(dll, "RiPointsGeneralPolygons");
	RiPointsGeneralPolygonsV	= (PfnRiPointsGeneralPolygonsV) GetProcAddress(dll, "RiPointsGeneralPolygonsV");
	RiBasis							= (PfnRiBasis) GetProcAddress(dll, "RiBasis");
	RiPatch							= (PfnRiPatch) GetProcAddress(dll, "RiPatch");
	RiPatchV							= (PfnRiPatchV) GetProcAddress(dll, "RiPatchV");
	RiPatchMesh						= (PfnRiPatchMesh) GetProcAddress(dll, "RiPatchMesh");
	RiPatchMeshV					= (PfnRiPatchMeshV) GetProcAddress(dll, "RiPatchMeshV");
	RiNuPatch						= (PfnRiNuPatch) GetProcAddress(dll, "RiNuPatch");
	RiNuPatchV						= (PfnRiNuPatchV) GetProcAddress(dll, "RiNuPatchV");
	RiTrimCurve						= (PfnRiTrimCurve) GetProcAddress(dll, "RiTrimCurve");
	success = success && RiPolygon && RiPolygonV && RiGeneralPolygon && RiGeneralPolygonV && RiPointsPolygons && RiPointsPolygonsV && RiPointsGeneralPolygons && RiPointsGeneralPolygonsV && RiBasis && RiPatch && RiPatchV && RiPatchMesh && RiPatchMeshV && RiNuPatch && RiNuPatchV && RiTrimCurve;

	RiSphere				= (PfnRiSphere) GetProcAddress(dll, "RiSphere");
	RiSphereV			= (PfnRiSphereV) GetProcAddress(dll, "RiSphereV");
	RiCone				= (PfnRiCone) GetProcAddress(dll, "RiCone");
	RiConeV				= (PfnRiConeV) GetProcAddress(dll, "RiConeV");
	RiCylinder			= (PfnRiCylinder) GetProcAddress(dll, "RiCylinder");
	RiCylinderV			= (PfnRiCylinderV) GetProcAddress(dll, "RiCylinderV");
	RiHyperboloid		= (PfnRiHyperboloid) GetProcAddress(dll, "RiHyperboloid");
	RiHyperboloidV		= (PfnRiHyperboloidV) GetProcAddress(dll, "RiHyperboloidV");
	RiParaboloid		= (PfnRiParaboloid) GetProcAddress(dll, "RiParaboloid");
	RiParaboloidV		= (PfnRiParaboloidV) GetProcAddress(dll, "RiParaboloidV");
	RiDisk				= (PfnRiDisk) GetProcAddress(dll, "RiDisk");
	RiDiskV				= (PfnRiDiskV) GetProcAddress(dll, "RiDiskV");
	RiTorus				= (PfnRiTorus) GetProcAddress(dll, "RiTorus");
	RiTorusV				= (PfnRiTorusV) GetProcAddress(dll, "RiTorusV");
	success = success && RiSphere && RiSphereV && RiCone && RiConeV && RiCylinder && RiCylinderV && RiHyperboloid && RiHyperboloidV && RiParaboloid && RiParaboloidV && RiDisk && RiDiskV && RiTorus && RiTorusV;

	RiBlobby		= (PfnRiBlobby) GetProcAddress(dll, "RiBlobby");
	RiBlobbyV	= (PfnRiBlobbyV) GetProcAddress(dll, "RiBlobbyV");
	success = success && RiBlobby && RiBlobbyV;

	RiCurves					= (PfnRiCurves) GetProcAddress(dll, "RiCurves");
	RiCurvesV				= (PfnRiCurvesV) GetProcAddress(dll, "RiCurvesV");
	RiPoints					= (PfnRiPoints) GetProcAddress(dll, "RiPoints");
	RiPointsV				= (PfnRiPointsV) GetProcAddress(dll, "RiPointsV");
	RiSubdivisionMesh		= (PfnRiSubdivisionMesh) GetProcAddress(dll, "RiSubdivisionMesh");
	RiSubdivisionMeshV	= (PfnRiSubdivisionMeshV) GetProcAddress(dll, "RiSubdivisionMeshV");
	success = success && RiCurves && RiCurvesV && RiPoints && RiPointsV && RiSubdivisionMesh && RiSubdivisionMeshV;

	RiProcedural	= (PfnRiProcedural) GetProcAddress(dll, "RiProcedural");
	RiGeometry		= (PfnRiGeometry) GetProcAddress(dll, "RiGeometry");
	RiGeometryV		= (PfnRiGeometryV) GetProcAddress(dll, "RiGeometryV");
	success = success && RiProcedural && RiGeometry && RiGeometryV;

	RiSolidBegin	= (PfnRiSolidBegin) GetProcAddress(dll, "RiSolidBegin");
	RiSolidEnd		= (PfnRiSolidEnd) GetProcAddress(dll, "RiSolidEnd");
	success = success && RiSolidBegin && RiSolidEnd;

	RiObjectBegin = (PfnRiObjectBegin) GetProcAddress(dll, "RiObjectBegin");
	RiObjectBeginV = (PfnRiObjectBeginV) GetProcAddress(dll, "RiObjectBeginV");
	success = success && RiObjectBegin && RiObjectBeginV;

	RiObjectEnd			= (PfnRiObjectEnd) GetProcAddress(dll, "RiObjectEnd");
	RiObjectInstance	= (PfnRiObjectInstance) GetProcAddress(dll, "RiObjectInstance");
	RiMotionBegin		= (PfnRiMotionBegin) GetProcAddress(dll, "RiMotionBegin");
	RiMotionBeginV		= (PfnRiMotionBeginV) GetProcAddress(dll, "RiMotionBeginV");
	RiMotionEnd			= (PfnRiMotionEnd) GetProcAddress(dll, "RiMotionEnd");
	success = success && RiObjectEnd && RiObjectInstance && RiMotionBegin && RiMotionBeginV && RiMotionEnd;

	RiMakeTexture					= (PfnRiMakeTexture) GetProcAddress(dll, "RiMakeTexture");
	RiMakeTextureV					= (PfnRiMakeTextureV) GetProcAddress(dll, "RiMakeTextureV");
	RiMakeBump						= (PfnRiMakeBump) GetProcAddress(dll, "RiMakeBump");
	RiMakeBumpV						= (PfnRiMakeBumpV) GetProcAddress(dll, "RiMakeBumpV");
	RiMakeLatLongEnvironment	= (PfnRiMakeLatLongEnvironment) GetProcAddress(dll, "RiMakeLatLongEnvironment");
	RiMakeLatLongEnvironmentV	= (PfnRiMakeLatLongEnvironmentV) GetProcAddress(dll, "RiMakeLatLongEnvironmentV");
	RiMakeCubeFaceEnvironment	= (PfnRiMakeCubeFaceEnvironment) GetProcAddress(dll, "RiMakeCubeFaceEnvironment");
	RiMakeCubeFaceEnvironmentV	= (PfnRiMakeCubeFaceEnvironmentV) GetProcAddress(dll, "RiMakeCubeFaceEnvironmentV");
	RiMakeShadow					= (PfnRiMakeShadow) GetProcAddress(dll, "RiMakeShadow");
	RiMakeShadowV					= (PfnRiMakeShadowV) GetProcAddress(dll, "RiMakeShadowV");
	success = success && RiMakeTexture && RiMakeTextureV && RiMakeBump && RiMakeBumpV && RiMakeLatLongEnvironment && RiMakeLatLongEnvironmentV && RiMakeCubeFaceEnvironment && RiMakeCubeFaceEnvironmentV && RiMakeShadow && RiMakeShadowV;

	RiArchiveRecord	= (PfnRiArchiveRecord) GetProcAddress(dll, "RiArchiveRecord");
	RiReadArchive		= (PfnRiReadArchive) GetProcAddress(dll, "RiReadArchive");
	RiReadArchiveV		= (PfnRiReadArchiveV) GetProcAddress(dll, "RiReadArchiveV");
	success = success && RiArchiveRecord && RiReadArchive && RiReadArchiveV;

	RiErrorHandler = (PfnRiErrorHandler) GetProcAddress(dll, "RiErrorHandler");
	success = success && RiErrorHandler;

	// these are only valid for PRMan dlls
	PRManBegin = (PfnPRManBegin) GetProcAddress(dll, "PRManBegin");
	PRManEnd = (PfnPRManEnd) GetProcAddress(dll, "PRManEnd");

	const char *RiClamp = (const char *) GetProcAddress(dll, "RI_CLAMP");

	if (!success)
		Detach();

	return success;
}

void RmDLL::Detach()
{
	RiBezierBasis = RiBSplineBasis = RiCatmullRomBasis = RiHermiteBasis = RiPowerBasis = NULL;

	RiGaussianFilter = NULL;
	RiBoxFilter = NULL;
	RiTriangleFilter = NULL;
	RiCatmullRomFilter = NULL;
	RiSincFilter = NULL;

	RiErrorIgnore = NULL;
	RiErrorPrint = NULL;
	RiErrorAbort = NULL;

	RiProcDelayedReadArchive = NULL;
	RiProcRunProgram = NULL;
	RiProcDynamicLoad = NULL;

	RiGetContext = NULL;
	RiContext = NULL;

	RiDeclare = NULL;

	RiBegin = NULL;
	RiEnd = NULL;
	RiFrameBegin = NULL;
	RiFrameEnd = NULL;
	RiWorldBegin = NULL;
	RiWorldEnd = NULL;

	RiFormat = NULL;
	RiFrameAspectRatio = NULL;
	RiScreenWindow = NULL;
	RiCropWindow = NULL;
	RiProjection = NULL;
	RiProjectionV = NULL;
	RiClipping = NULL;
	RiClippingPlane = NULL;
	RiShutter = NULL;

	RiPixelVariance = NULL;
	RiPixelSamples = NULL;
	RiPixelFilter = NULL;
	RiExposure = NULL;
	RiImager = NULL;
	RiImagerV = NULL;
	RiQuantize = NULL;
	RiDisplay = NULL;
	RiDisplayV = NULL;
	RiDisplayChannel = NULL;
	RiDisplayChannelV = NULL;


	RiHider = NULL;
	RiHiderV = NULL;
	RiColorSamples = NULL;
	RiRelativeDetail = NULL;
	RiOption = NULL;
	RiOptionV = NULL;

	RiAttributeBegin = NULL;
	RiAttributeEnd = NULL;
	RiColor = NULL;
	RiOpacity = NULL;
	RiTextureCoordinates = NULL;

	RiLightSource = NULL;
	RiLightSourceV = NULL;
	RiAreaLightSource = NULL;
	RiAreaLightSourceV = NULL;

	RiIlluminate = NULL;
	RiSurface = NULL;
	RiSurfaceV = NULL;
	RiAtmosphere = NULL;
	RiAtmosphereV = NULL;
	RiInterior = NULL;
	RiInteriorV = NULL;
	RiExterior = NULL;
	RiExteriorV = NULL;
	RiShadingRate = NULL;
	RiShadingInterpolation = NULL;
	RiMatte = NULL;

	RiBound = NULL;
	RiDetail = NULL;
	RiDetailRange = NULL;
	RiGeometricApproximation = NULL;
	RiOrientation = NULL;
	RiReverseOrientation = NULL;
	RiSides = NULL;

	RiIdentity = NULL;
	RiTransform = NULL;
	RiConcatTransform = NULL;
	RiPerspective = NULL;
	RiTranslate = NULL;
	RiRotate = NULL;
	RiScale = NULL;
	RiSkew = NULL;
	RiDeformation = NULL;
	RiDeformationV = NULL;
	RiDisplacement = NULL;
	RiDisplacementV = NULL;
	RiCoordinateSystem = NULL;
	RiCoordSysTransform = NULL;

	RiTransformPoints = NULL;

	RiTransformBegin = NULL;
	RiTransformEnd = NULL;

	RiAttribute = NULL;
	RiAttributeV = NULL;

	RiPolygon = NULL;
	RiPolygonV = NULL;
	RiGeneralPolygon = NULL;
	RiGeneralPolygonV = NULL;
	RiPointsPolygons = NULL;
	RiPointsPolygonsV = NULL;
	RiPointsGeneralPolygons = NULL;
	RiPointsGeneralPolygonsV = NULL;
	RiBasis = NULL;
	RiPatch = NULL;
	RiPatchV = NULL;
	RiPatchMesh = NULL;
	RiPatchMeshV = NULL;
	RiNuPatch = NULL;
	RiNuPatchV = NULL;
	RiTrimCurve = NULL;

	RiSphere = NULL;
	RiSphereV = NULL;
	RiCone = NULL;
	RiConeV = NULL;
	RiCylinder = NULL;
	RiCylinderV = NULL;
	RiHyperboloid = NULL;
	RiHyperboloidV = NULL;
	RiParaboloid = NULL;
	RiParaboloidV = NULL;
	RiDisk = NULL;
	RiDiskV = NULL;
	RiTorus = NULL;
	RiTorusV = NULL;

	RiBlobby = NULL;
	RiBlobbyV = NULL;

	RiCurves = NULL;
	RiCurvesV = NULL;
	RiPoints = NULL;
	RiPointsV = NULL;
	RiSubdivisionMesh = NULL;
	RiSubdivisionMeshV = NULL;

	RiProcedural = NULL;
	RiGeometry = NULL;
	RiGeometryV = NULL;

	RiSolidBegin = NULL;
	RiSolidEnd = NULL;

	RiObjectBegin = NULL;
	RiObjectBeginV = NULL;

	RiObjectEnd = NULL;
	RiObjectInstance = NULL;
	RiMotionBegin = NULL;
	RiMotionBeginV = NULL;
	RiMotionEnd = NULL;

	RiMakeTexture = NULL;
	RiMakeTextureV = NULL;
	RiMakeBump = NULL;
	RiMakeBumpV = NULL;
	RiMakeLatLongEnvironment = NULL;
	RiMakeLatLongEnvironmentV = NULL;
	RiMakeCubeFaceEnvironment = NULL;
	RiMakeCubeFaceEnvironmentV = NULL;
	RiMakeShadow = NULL;
	RiMakeShadowV = NULL;

	RiArchiveRecord = NULL;
	RiReadArchive = NULL;
	RiReadArchiveV = NULL;

	RiErrorHandler = NULL;

	PRManBegin = NULL;
	PRManEnd = NULL;
}

