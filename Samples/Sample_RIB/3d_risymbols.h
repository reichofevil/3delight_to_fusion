#ifndef _RENDERMAN_INTERFACE_SYMBOLS_H_
#define _RENDERMAN_INTERFACE_SYMBOLS_H_


#ifdef __cplusplus
extern "C" {
#endif

#undef IGNORE
#undef DIFFERENCE

#define RI_DECL __cdecl //__stdcall

/* Definitions of Abstract Types used in RI */
typedef short RtBoolean;
typedef int RtInt;
typedef float RtFloat;
typedef char *RtToken;
typedef RtFloat RtColor[3];
typedef RtFloat RtPoint[3];
typedef RtFloat RtVector[3];
typedef RtFloat RtNormal[3];
typedef RtFloat RtHpoint[4];
typedef RtFloat RtMatrix[4][4];
typedef RtFloat RtBasis[4][4];
typedef RtFloat RtBound[6];
typedef char *RtString;
typedef void *RtPointer;
#define RtVoid void
typedef RtFloat (*RtFilterFunc)(RtFloat, RtFloat, RtFloat, RtFloat);
typedef RtVoid (*RtErrorHandler)(RtInt, RtInt, char *);
typedef RtVoid (*RtProcSubdivFunc)(RtPointer, RtFloat);
typedef RtVoid (*RtProcFreeFunc)(RtPointer);
typedef RtVoid (*RtArchiveCallback)(RtToken, char *, ...);
typedef RtPointer RtObjectHandle;
typedef RtPointer RtLightHandle;
typedef RtPointer RtContextHandle;

/* Extern Declarations for Predefined RI Data Structures */
#define RI_FALSE 0
#define RI_TRUE (! RI_FALSE)
#define RI_INFINITY 1.0e38
#define RI_EPSILON 1.0e-10
#define RI_NULL ((RtToken)0)

//extern RtToken RI_FRAMEBUFFER, RI_FILE;
//extern RtToken RI_RGB, RI_RGBA, RI_RGBZ, RI_RGBAZ, RI_A, RI_Z, RI_AZ;
//extern RtToken RI_PERSPECTIVE, RI_ORTHOGRAPHIC;
//extern RtToken RI_HIDDEN, RI_PAINT;
//extern RtToken RI_CONSTANT, RI_SMOOTH;
//extern RtToken RI_FLATNESS, RI_FOV;
//extern RtToken RI_AMBIENTLIGHT, RI_POINTLIGHT, RI_DISTANTLIGHT, RI_SPOTLIGHT;
//extern RtToken RI_INTENSITY, RI_LIGHTCOLOR, RI_FROM, RI_TO, RI_CONEANGLE, RI_CONEDELTAANGLE, RI_BEAMDISTRIBUTION;
//extern RtToken RI_MATTE, RI_METAL, RI_SHINYMETAL, RI_PLASTIC, RI_PAINTEDPLASTIC;
//extern RtToken RI_KA, RI_KD, RI_KS, RI_ROUGHNESS, RI_KR, RI_TEXTURENAME, RI_SPECULARCOLOR;
//extern RtToken RI_DEPTHCUE, RI_FOG, RI_BUMPY;
//extern RtToken RI_MINDISTANCE, RI_MAXDISTANCE, RI_BACKGROUND, RI_DISTANCE, RI_AMPLITUDE;
//extern RtToken RI_RASTER, RI_SCREEN, RI_CAMERA, RI_WORLD, RI_OBJECT;
//extern RtToken RI_INSIDE, RI_OUTSIDE, RI_LH, RI_RH;
//extern RtToken RI_P, RI_PZ, RI_PW, RI_N, RI_NP, RI_CS, RI_OS, RI_S, RI_T, RI_ST;
//extern RtToken RI_BILINEAR, RI_BICUBIC;
//extern RtToken RI_LINEAR, RI_CUBIC;
//extern RtToken RI_PRIMITIVE, RI_INTERSECTION, RI_UNION, RI_DIFFERENCE;
//extern RtToken RI_PERIODIC, RI_NONPERIODIC, RI_CLAMP, RI_BLACK;
//extern RtToken RI_IGNORE, RI_PRINT, RI_ABORT, RI_HANDLER;
//extern RtToken RI_COMMENT, RI_STRUCTURE, RI_VERBATIM;
//extern RtToken RI_IDENTIFIER, RI_NAME, RI_SHADINGGROUP;
//extern RtToken RI_WIDTH, RI_CONSTANTWIDTH;

extern RtBasis RiBezierBasis, RiBSplineBasis, RiCatmullRomBasis, RiHermiteBasis, RiPowerBasis;

#define RI_BEZIERSTEP ((RtInt)3)
#define RI_BSPLINESTEP ((RtInt)1)
#define RI_CATMULLROMSTEP ((RtInt)1)
#define RI_HERMITESTEP ((RtInt)2)
#define RI_POWERSTEP ((RtInt)4)

extern RtInt RiLastError;


/* Declarations of All the RenderMan Interface Subroutines */
typedef RtFloat (RI_DECL *PfnRiGaussianFilter)(RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth);
typedef RtFloat (RI_DECL *PfnRiBoxFilter)(RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth);
typedef RtFloat (RI_DECL *PfnRiTriangleFilter)(RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth);
typedef RtFloat (RI_DECL *PfnRiCatmullRomFilter)(RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth);
typedef RtFloat (RI_DECL *PfnRiSincFilter)(RtFloat x, RtFloat y, RtFloat xwidth, RtFloat ywidth);

typedef RtVoid (RI_DECL *PfnRiErrorIgnore)(RtInt code, RtInt severity, char *msg);
typedef RtVoid (RI_DECL *PfnRiErrorPrint)(RtInt code, RtInt severity, char *msg);
typedef RtVoid (RI_DECL *PfnRiErrorAbort)(RtInt code, RtInt severity, char *msg);

typedef RtVoid (RI_DECL *PfnRiProcDelayedReadArchive)(RtPointer data, RtFloat detail);
typedef RtVoid (RI_DECL *PfnRiProcRunProgram)(RtPointer data, RtFloat detail);
typedef RtVoid (RI_DECL *PfnRiProcDynamicLoad)(RtPointer data, RtFloat detail);

typedef RtContextHandle (RI_DECL *PfnRiGetContext)();
typedef RtVoid (RI_DECL *PfnRiContext)(RtContextHandle);

typedef RtToken (RI_DECL *PfnRiDeclare)(char *name, char *declaration);

typedef RtVoid (RI_DECL *PfnRiBegin)(RtToken name);
typedef RtVoid (RI_DECL *PfnRiEnd)();
typedef RtVoid (RI_DECL *PfnRiFrameBegin)(RtInt frame);
typedef RtVoid (RI_DECL *PfnRiFrameEnd)();
typedef RtVoid (RI_DECL *PfnRiWorldBegin)();
typedef RtVoid (RI_DECL *PfnRiWorldEnd)();

typedef RtVoid (RI_DECL *PfnRiFormat)(RtInt xres, RtInt yres, RtFloat aspect);
typedef RtVoid (RI_DECL *PfnRiFrameAspectRatio)(RtFloat aspect);
typedef RtVoid (RI_DECL *PfnRiScreenWindow)(RtFloat left, RtFloat right, RtFloat bot, RtFloat top);
typedef RtVoid (RI_DECL *PfnRiCropWindow)(RtFloat xmin, RtFloat xmax, RtFloat ymin, RtFloat ymax);
typedef RtVoid (RI_DECL *PfnRiProjection)(RtToken name, ...);
typedef RtVoid (RI_DECL *PfnRiProjectionV)(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiClipping)(RtFloat hither, RtFloat yon);
typedef RtVoid (RI_DECL *PfnRiClippingPlane)(RtFloat x, RtFloat y, RtFloat z, RtFloat nx, RtFloat ny, RtFloat nz);
typedef RtVoid (RI_DECL *PfnRiShutter)(RtFloat min, RtFloat max);
typedef RtVoid (RI_DECL *PfnRiDepthOfField)(RtFloat fstop, RtFloat focallen, RtFloat focaldist);

typedef RtVoid (RI_DECL *PfnRiPixelVariance)(RtFloat variation);
typedef RtVoid (RI_DECL *PfnRiPixelSamples)(RtFloat xsamples, RtFloat ysamples);
typedef RtVoid (RI_DECL *PfnRiPixelFilter)(RtFilterFunc filterfunc, RtFloat xwidth, RtFloat ywidth);
typedef RtVoid (RI_DECL *PfnRiExposure)(RtFloat gain, RtFloat gamma);
typedef RtVoid (RI_DECL *PfnRiImager)(RtToken name, ...);
typedef RtVoid (RI_DECL *PfnRiImagerV)(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiQuantize)(RtToken type, RtInt one, RtInt min, RtInt max, RtFloat ampl);
typedef RtVoid (RI_DECL *PfnRiDisplay)(char *name, RtToken type, RtToken mode, ...);
typedef RtVoid (RI_DECL *PfnRiDisplayV)(char *name, RtToken type, RtToken mode, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiDisplayChannel)(const char *channel, ...);
typedef RtVoid (RI_DECL *PfnRiDisplayChannelV)(const char *channel,	RtInt n, RtToken tokens[], RtPointer params[]);

typedef RtVoid (RI_DECL *PfnRiHider)(RtToken type, ...);
typedef RtVoid (RI_DECL *PfnRiHiderV)(RtToken type, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiColorSamples)(RtInt n, RtFloat nRGB[], RtFloat RGBn[]);
typedef RtVoid (RI_DECL *PfnRiRelativeDetail)(RtFloat relativedetail);
typedef RtVoid (RI_DECL *PfnRiOption)(RtToken name, ...);
typedef RtVoid (RI_DECL *PfnRiOptionV)(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);

typedef RtVoid (RI_DECL *PfnRiAttributeBegin)();
typedef RtVoid (RI_DECL *PfnRiAttributeEnd)();
typedef RtVoid (RI_DECL *PfnRiColor)(RtColor color);
typedef RtVoid (RI_DECL *PfnRiOpacity)(RtColor color);
typedef RtVoid (RI_DECL *PfnRiTextureCoordinates)(RtFloat s1, RtFloat t1, RtFloat s2, RtFloat t2, RtFloat s3, RtFloat t3, RtFloat s4, RtFloat t4);

typedef RtLightHandle (RI_DECL *PfnRiLightSource)(RtToken name, ...);
typedef RtLightHandle (RI_DECL *PfnRiLightSourceV)(RtToken name, RtInt n, RtToken tokens[],RtPointer parms[]);
typedef RtLightHandle (RI_DECL *PfnRiAreaLightSource)(RtToken name, ...);
typedef RtLightHandle (RI_DECL *PfnRiAreaLightSourceV)(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);

typedef RtVoid (RI_DECL *PfnRiIlluminate)(RtLightHandle light, RtBoolean onoff);
typedef RtVoid (RI_DECL *PfnRiSurface)(RtToken name, ...);
typedef RtVoid (RI_DECL *PfnRiSurfaceV)(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiAtmosphere)(RtToken name, ...);
typedef RtVoid (RI_DECL *PfnRiAtmosphereV)(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiInterior)(RtToken name, ...);
typedef RtVoid (RI_DECL *PfnRiInteriorV)(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiExterior)(RtToken name, ...);
typedef RtVoid (RI_DECL *PfnRiExteriorV)(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiShadingRate)(RtFloat size);
typedef RtVoid (RI_DECL *PfnRiShadingInterpolation)(RtToken type);
typedef RtVoid (RI_DECL *PfnRiMatte)(RtBoolean onoff);

typedef RtVoid (RI_DECL *PfnRiBound)(RtBound bound);
typedef RtVoid (RI_DECL *PfnRiDetail)(RtBound bound);
typedef RtVoid (RI_DECL *PfnRiDetailRange)(RtFloat minvis, RtFloat lowtran, RtFloat uptran, RtFloat maxvis);
typedef RtVoid (RI_DECL *PfnRiGeometricApproximation)(RtToken type, RtFloat value);
typedef RtVoid (RI_DECL *PfnRiOrientation)(RtToken orientation);
typedef RtVoid (RI_DECL *PfnRiReverseOrientation)();
typedef RtVoid (RI_DECL *PfnRiSides)(RtInt sides);

typedef RtVoid (RI_DECL *PfnRiIdentity)();
typedef RtVoid (RI_DECL *PfnRiTransform)(RtMatrix transform);
typedef RtVoid (RI_DECL *PfnRiConcatTransform)(RtMatrix transform);
typedef RtVoid (RI_DECL *PfnRiPerspective)(RtFloat fov);
typedef RtVoid (RI_DECL *PfnRiTranslate)(RtFloat dx, RtFloat dy, RtFloat dz);
typedef RtVoid (RI_DECL *PfnRiRotate)(RtFloat angle, RtFloat dx, RtFloat dy, RtFloat dz);
typedef RtVoid (RI_DECL *PfnRiScale)(RtFloat sx, RtFloat sy, RtFloat sz);
typedef RtVoid (RI_DECL *PfnRiSkew)(RtFloat angle, RtFloat dx1, RtFloat dy1, RtFloat dz1, RtFloat dx2, RtFloat dy2, RtFloat dz2);
typedef RtVoid (RI_DECL *PfnRiDeformation)(RtToken name, ...);
typedef RtVoid (RI_DECL *PfnRiDeformationV)(RtToken name, RtInt n,RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiDisplacement)(RtToken name, ...);
typedef RtVoid (RI_DECL *PfnRiDisplacementV)(RtToken name, RtInt n,RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiCoordinateSystem)(RtToken space);
typedef RtVoid (RI_DECL *PfnRiCoordSysTransform)(RtToken space);

typedef RtPoint *(RI_DECL *PfnRiTransformPoints)(RtToken fromspace, RtToken tospace, RtInt n, RtPoint points[]);

typedef RtVoid (RI_DECL *PfnRiTransformBegin)();
typedef RtVoid (RI_DECL *PfnRiTransformEnd)();

typedef RtVoid (RI_DECL *PfnRiAttribute)(RtToken name, ...);
typedef RtVoid (RI_DECL *PfnRiAttributeV)(RtToken name, RtInt n, RtToken tokens[], RtPointer parms[]);

typedef RtVoid (RI_DECL *PfnRiPolygon)(RtInt nverts, ...);
typedef RtVoid (RI_DECL *PfnRiPolygonV)(RtInt nverts, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiGeneralPolygon)(RtInt nloops, RtInt nverts[], ...);
typedef RtVoid (RI_DECL *PfnRiGeneralPolygonV)(RtInt nloops, RtInt nverts[], RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiPointsPolygons)(RtInt npolys, RtInt nverts[], RtInt verts[], ...);
typedef RtVoid (RI_DECL *PfnRiPointsPolygonsV)(RtInt npolys, RtInt nverts[], RtInt verts[], RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiPointsGeneralPolygons)(RtInt npolys, RtInt nloops[], RtInt nverts[], RtInt verts[], ...);
typedef RtVoid (RI_DECL *PfnRiPointsGeneralPolygonsV)(RtInt npolys, RtInt nloops[], RtInt nverts[], RtInt verts[], RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiBasis)(RtBasis ubasis, RtInt ustep, RtBasis vbasis, RtInt vstep);
typedef RtVoid (RI_DECL *PfnRiPatch)(RtToken type, ...);
typedef RtVoid (RI_DECL *PfnRiPatchV)(RtToken type, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiPatchMesh)(RtToken type, RtInt nu, RtToken uwrap, RtInt nv, RtToken vwrap, ...);
typedef RtVoid (RI_DECL *PfnRiPatchMeshV)(RtToken type, RtInt nu, RtToken uwrap, RtInt nv, RtToken vwrap, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiNuPatch)(RtInt nu, RtInt uorder, RtFloat uknot[], RtFloat umin, RtFloat umax, RtInt nv, RtInt vorder, RtFloat vknot[], RtFloat vmin, RtFloat vmax, ...);
typedef RtVoid (RI_DECL *PfnRiNuPatchV)(RtInt nu, RtInt uorder, RtFloat uknot[], RtFloat umin, RtFloat umax, RtInt nv, RtInt vorder, RtFloat vknot[], RtFloat vmin, RtFloat vmax, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiTrimCurve)(RtInt nloops, RtInt ncurves[], RtInt order[], RtFloat knot[], RtFloat min[], RtFloat max[], RtInt n[], RtFloat u[], RtFloat v[], RtFloat w[]);

typedef RtVoid (RI_DECL *PfnRiSphere)(RtFloat radius, RtFloat zmin, RtFloat zmax, RtFloat tmax, ...);
typedef RtVoid (RI_DECL *PfnRiSphereV)(RtFloat radius, RtFloat zmin, RtFloat zmax, RtFloat tmax, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiCone)(RtFloat height, RtFloat radius, RtFloat tmax, ...);
typedef RtVoid (RI_DECL *PfnRiConeV)(RtFloat height, RtFloat radius, RtFloat tmax, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiCylinder)(RtFloat radius, RtFloat zmin, RtFloat zmax, RtFloat tmax, ...);
typedef RtVoid (RI_DECL *PfnRiCylinderV)(RtFloat radius, RtFloat zmin, RtFloat zmax, RtFloat tmax, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiHyperboloid)(RtPoint point1, RtPoint point2, RtFloat tmax, ...);
typedef RtVoid (RI_DECL *PfnRiHyperboloidV)(RtPoint point1, RtPoint point2, RtFloat tmax, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiParaboloid)(RtFloat rmax,RtFloat zmin,RtFloat zmax,RtFloat tmax, ...);
typedef RtVoid (RI_DECL *PfnRiParaboloidV)(RtFloat rmax, RtFloat zmin, RtFloat zmax, RtFloat tmax, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiDisk)(RtFloat height, RtFloat radius, RtFloat tmax, ...);
typedef RtVoid (RI_DECL *PfnRiDiskV)(RtFloat height, RtFloat radius, RtFloat tmax, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiTorus)(RtFloat majrad, RtFloat minrad, RtFloat phimin, RtFloat phimax, RtFloat tmax, ...);
typedef RtVoid (RI_DECL *PfnRiTorusV)(RtFloat majrad,RtFloat minrad, RtFloat phimin, RtFloat phimax, RtFloat tmax, RtInt n, RtToken tokens[], RtPointer parms[]);

typedef RtVoid (RI_DECL *PfnRiBlobby)(RtInt nleaf, RtInt ncode, RtInt code[], RtInt nflt, RtFloat flt[], RtInt nstr, RtToken str[], ...);
typedef RtVoid (RI_DECL *PfnRiBlobbyV)(RtInt nleaf, RtInt ncode, RtInt code[], RtInt nflt, RtFloat flt[], RtInt nstr, RtToken str[], RtInt n , RtToken tokens[], RtPointer parms[]);

typedef RtVoid (RI_DECL *PfnRiCurves)(RtToken type, RtInt ncurves, RtInt nvertices[], RtToken wrap, ...);
typedef RtVoid (RI_DECL *PfnRiCurvesV)(RtToken type, RtInt ncurves, RtInt nvertices[], RtToken wrap, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiPoints)(RtInt nverts,...);
typedef RtVoid (RI_DECL *PfnRiPointsV)(RtInt nverts, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiSubdivisionMesh)(RtToken mask, RtInt nf, RtInt nverts[], RtInt verts[], RtInt ntags, RtToken tags[], RtInt numargs[], RtInt intargs[], RtFloat floatargs[], ...);
typedef RtVoid (RI_DECL *PfnRiSubdivisionMeshV)(RtToken mask, RtInt nf, RtInt nverts[], RtInt verts[], RtInt ntags, RtToken tags[], RtInt nargs[], RtInt intargs[], RtFloat floatargs[], RtInt n, RtToken tokens[], RtPointer *parms);

typedef RtVoid (RI_DECL *PfnRiProcedural)(RtPointer data, RtBound bound, RtVoid (*subdivfunc)(RtPointer, RtFloat), RtVoid (*freefunc)(RtPointer));
typedef RtVoid (RI_DECL *PfnRiGeometry)(RtToken type, ...);
typedef RtVoid (RI_DECL *PfnRiGeometryV)(RtToken type, RtInt n, RtToken tokens[], RtPointer parms[]);

typedef RtVoid (RI_DECL *PfnRiSolidBegin)(RtToken operation);
typedef RtVoid (RI_DECL *PfnRiSolidEnd)();

typedef RtObjectHandle (RI_DECL *PfnRiObjectBegin)();

typedef RtVoid (RI_DECL *PfnRiObjectEnd)();
typedef RtVoid (RI_DECL *PfnRiObjectInstance)(RtObjectHandle handle);
typedef RtVoid (RI_DECL *PfnRiMotionBegin)(RtInt n, ...);
typedef RtVoid (RI_DECL *PfnRiMotionBeginV)(RtInt n, RtFloat times[]);
typedef RtVoid (RI_DECL *PfnRiMotionEnd)();

typedef RtVoid (RI_DECL *PfnRiMakeTexture)(char *pic, char *tex, RtToken swrap, RtToken twrap, RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth, ...);
typedef RtVoid (RI_DECL *PfnRiMakeTextureV)(char *pic, char *tex, RtToken swrap, RtToken twrap, RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiMakeBump)(char *pic, char *tex, RtToken swrap, RtToken twrap, RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth, ...);
typedef RtVoid (RI_DECL *PfnRiMakeBumpV)(char *pic, char *tex, RtToken swrap, RtToken twrap, RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiMakeLatLongEnvironment)(char *pic, char *tex, RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth, ...);
typedef RtVoid (RI_DECL *PfnRiMakeLatLongEnvironmentV)(char *pic, char *tex, RtFilterFunc filterfunc, RtFloat swidth, RtFloat twidth, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiMakeCubeFaceEnvironment)(char *px, char *nx, char *py, char *ny, char *pz, char *nz, char *tex, RtFloat fov, RtFilterFunc filterfunc, RtFloat swidth, RtFloat ywidth, ...);
typedef RtVoid (RI_DECL *PfnRiMakeCubeFaceEnvironmentV)(char *px, char *nx, char *py, char *ny, char *pz, char *nz, char *tex, RtFloat fov, RtFilterFunc filterfunc, RtFloat swidth, RtFloat ywidth, RtInt n, RtToken tokens[], RtPointer parms[]);
typedef RtVoid (RI_DECL *PfnRiMakeShadow)(char *pic, char *tex, ...);
typedef RtVoid (RI_DECL *PfnRiMakeShadowV)(char *pic, char *tex, RtInt n, RtToken tokens[], RtPointer parms[]);

typedef RtVoid (RI_DECL *PfnRiArchiveRecord)(RtToken type, char *format, ...);
typedef RtVoid (RI_DECL *PfnRiReadArchive)(RtToken name, RtArchiveCallback callback, ...);
typedef RtVoid (RI_DECL *PfnRiReadArchiveV)(RtToken name, RtArchiveCallback callback, RtInt n, RtToken tokens[], RtPointer parms[]);

typedef RtVoid (RI_DECL *PfnRiErrorHandler)(RtErrorHandler handler);

typedef int (RI_DECL *PfnPRManBegin)(int argc, char *argv[]);
typedef int (RI_DECL *PfnPRManEnd)();

/*
Error Codes
1 - 10 System and File Errors
11 - 20 Program Limitations
21 - 40 State Errors
41 - 60 Parameter and Protocol Errors
61 - 80 Execution Errors
*/
#define RIE_NOERROR ((RtInt)0)
#define RIE_NOMEM ((RtInt)1)			/* Out of memory */
#define RIE_SYSTEM ((RtInt)2)			/* Miscellaneous system error */
#define RIE_NOFILE ((RtInt)3)			/* File nonexistent */
#define RIE_BADFILE ((RtInt)4)		/* Bad file format */
#define RIE_VERSION ((RtInt)5)		/* File version mismatch */
#define RIE_DISKFULL ((RtInt)6)		/* Target disk is full */
#define RIE_INCAPABLE ((RtInt)11)	/* Optional RI feature */
#define RIE_UNIMPLEMENT ((RtInt)12) /* Unimplemented feature */
#define RIE_LIMIT ((RtInt)13)			/* Arbitrary program limit */
#define RIE_BUG ((RtInt)14)			/* Probably a bug in renderer */
#define RIE_NOTSTARTED ((RtInt)23)	/* RiBegin not called */
#define RIE_NESTING ((RtInt)24)		/* Bad begin-end nesting */
#define RIE_NOTOPTIONS ((RtInt)25)	/* Invalid state for options */
#define RIE_NOTATTRIBS ((RtInt)26)	/* Invalid state for attribs */
#define RIE_NOTPRIMS ((RtInt)27)		/* Invalid state for primitives */
#define RIE_ILLSTATE ((RtInt)28)		/* Other invalid state */
#define RIE_BADMOTION ((RtInt)29)	/* Badly formed motion block */
#define RIE_BADSOLID ((RtInt)30)		/* Badly formed solid block */
#define RIE_BADTOKEN ((RtInt)41)		/* Invalid token for request */
#define RIE_RANGE ((RtInt)42)			/* Parameter out of range */
#define RIE_CONSISTENCY ((RtInt)43) /* Parameters inconsistent */
#define RIE_BADHANDLE ((RtInt)44)	/* Bad object/light handle */
#define RIE_NOSHADER ((RtInt)45)		/* Can't load requested shader */
#define RIE_MISSINGDATA ((RtInt)46) /* Required parameters not provided */
#define RIE_SYNTAX ((RtInt)47)		/* Declare type syntax error */
#define RIE_MATH ((RtInt)61)			/* Zerodivide, noninvert matrix, etc. */

/* Error severity levels */
#define RIE_INFO ((RtInt)0)			/* Rendering stats and other info */
#define RIE_WARNING ((RtInt)1)		/* Something seems wrong, maybe okay */
#define RIE_ERROR ((RtInt)2)			/* Problem. Results may be wrong */
#define RIE_SEVERE ((RtInt)3)			/* So bad you should probably abort */

#ifdef __cplusplus
}
#endif



#endif