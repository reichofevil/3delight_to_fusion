#ifndef _CURVE_3D_H_
#define _CURVE_3D_H_

#include "Types.h"
#include "Object.h"

#include "3D_MaterialBase.h"
#include "3D_RenderContext.h"
#include "3D_MaterialConstant.h"

class TessArray;



//
//
// Curve3D
//
//
class Geometry3D;

class FuSYSTEM_API Curve3D : public Object
{
public:
	// The convention that we use when computing winding numbers is:
	//		counterclockwise revolutions -> positive
	//		clockwise revolutions		  -> negative
	// The winding number of a point is used to define if it is inside or outside of a region.
	// For example, if you choose ODD, all points with ODD winding numbers will be considered inside.
	enum winding_rule_t 
	{
		ODD,									// default
		NON_ZERO,
		POSITIVE,
		NEGATIVE,
		ABS_GREATER_EQUAL_TWO,
		NUM_CURVE_WINDING_RULES
	};

	//olour4D Color;									// color for all points, defaults to (1, 1, 1, 1)
	float32 Thickness;								// thickness for all points, defaults to 1.0
	Vector3f *Points;									// points, defaults to NULL
	int NumberOfPoints;								// number of points, defaults to 0
	Color4f *Colors;									// color at each point (if null then use the above color), defaults to NULL
	float32 *Thicknesses;							// thickness at each point (if null then use the above thickness), defaults to NULL
	Vector3f Normal;									// the normal to the plane that the curve sits in, optional, defaults to (0, 0, 1), if this curve is tesselated this normal will be used for lighting
	bool IsClosed;										// if true then the curve is closed, i.e. first and last points are connected, defaults to true
	int *ContourLengths;								// lengths of the contours making up this curve.  (if null then whole curve is one contour), defaults to NULL
	int NumberOfContours;							// number of coutours making up this curve, defaults to 0
	PFM_PolygonFillMode CurveFillMode;			// select if you want the curve drawn as points, lines, or polygons.  Defaults to lines.
	winding_rule_t WindingRule;					// the winding rule used when drawing this curve (only used when drawing filled polygons), defaults to ODD.
	Vector2f *TexCoord;								// Texture coordinates	

	FuPointer<MtlData3D> Mtl;

	int32 ObjectID;

	Matrix4f TextureMatrix;

public:
	Curve3D() { Init(); }
	Curve3D(const Curve3D &toCopy);				// purposefully left unimplemented
	~Curve3D() { Delete(); }

	// Transforms the vertices of this curve by the given vertex transform.  Transforms the normals and any other vector
	// attributes of this curve by the vectorTransform.  Usually, vectorTransform should be set to vertexTransform.InverseTranspose()
	virtual void Transform(const Matrix4 &vertexTransform, const Matrix4 &vectorTransform);

	// The returned Geometry3D object needs to be recycled.
	Geometry3D *GenerateGeometry();
	
	// Tesselates this curve into into triangles and stores the result into TesselationCache.  Only closed and planer curves 
	// should be tesselated.  
	bool Tesselate();

	TessArray *TesselationCache;

protected:
	void Init();

	void Delete();
};

/*
const char* const CurveWindingRules[Curve3D::NUM_CURVE_WINDING_RULES] = 
{ 
	"odd", 
	"non-zero", 
	"positive", 
	"negative", 
	"abs_greater_equal_two"				// names used by console renderer
};

const char* const CurveDrawStyles[Curve3D::NUM_CURVE_DRAW_STYLES] = 
{ 
	"points", 
	"lines", 
	"polygons" 
};
*/





#endif