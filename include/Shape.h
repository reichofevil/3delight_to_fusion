#ifndef _Shape_H_
#define _Shape_H_

#include "Object.h"

#include "VectorMath.h"
#include "ImageRegion.h"

enum FillOverlap	
{
	FO_Solid,
	FO_Transparent,
	FO_Composite,
};

enum OutlineLineType	
{
	OLT_Solid,
	OLT_Dash,
	OLT_Dot,
	OLT_DashDot,
	OLT_DashDotDot,
	OLT_Custom,
};

enum OutlineJoinType	
{
	OJT_Bevel,
	OJT_Miter,
	OJT_Round,
};

// ***** Windows Shape

class FuSYSTEM_API WindowsShape
{
public :
	int PointCount;
	LPPOINT Points;
	LPBYTE Types;

	WindowsShape::WindowsShape();
	WindowsShape::~WindowsShape();
};

// ***** Shape Array for scanline rendering

typedef struct
{
	double X, Y;
	int YInt;
	unsigned char WindIndex;
} ShapeArrayPoint;

typedef struct
{
	int YInt;
	int Point;
	int PointPrevious, PointNext;
} ShapeArrayPointIndex;

typedef struct
{
	double X, Dx;
	int XInt, YIntOther;
	signed char Winding;
	unsigned char WindIndex;
} ShapeArrayEdge;

// ***** Shape

enum ShapePointType	
{
	PPT_MoveTo,
	PPT_LineTo,
	PPT_ConicTo,
	PPT_BezierTo,
};

enum ShapeWindingMode
{
	SWM_NoChange = -1,
	SWM_Normal = 0,
	SWM_Clear,
	SWM_Solid
};

class ShapePoint
{
public :
	Vector2 v;
	ShapePointType Type;
	bool Close;
	int WindIndex;

	ShapePoint *Previous, *Next;

	ShapePoint(ShapePoint *p);
	ShapePoint(ShapePoint *p, double offsetX, double offsetY);
	ShapePoint(ShapePoint *p, const Matrix4 &transform);
	ShapePoint(double x = 0.0, double y = 0.0, ShapePointType type = PPT_MoveTo, bool close = FALSE);
	ShapePoint(Vector2 v, ShapePointType type = PPT_MoveTo, bool close = FALSE);
};

enum ShapePresetType
{
	PPT_Ellipse,
	PPT_Rectangle, 
};


class Vector3f;

class FuSYSTEM_API Shape : public Object
{
	FuDeclareClass(Shape, Object); // declare ClassID static member, etc.

	ShapePoint *PointFirst, *PointLast;
	int Count, CountConic, CountBezier;

	Shape *OffsetOfShape_Miter(double offset, bool doClipping);
	Shape *OffsetOfShape_Bevel(double offset, bool doClipping);
	Shape *OffsetOfShape_Rounded(double offset, bool doClipping, int precision);

	Shape *InternalOutlineOfShape(double thickness, OutlineLineType linetype, OutlineJoinType jointype, int precision);

	void AddPoint(ShapePoint *p);

public :
	Shape();
	Shape(const Registry *reg, const ScriptVal &table, const TagList &tags);
	~Shape();

	void MoveTo(const Vector2 &v, bool close = FALSE);
	void LineTo(const Vector2 &v, bool close = FALSE);
	void ConicTo(const Vector2 &v1, const Vector2 &v2, bool close = FALSE);
	void BezierTo(const Vector2 &v1, const Vector2 &v2, const Vector2 &v3, bool close = FALSE);

	void FlatConicTo(const Vector2 &v1, const Vector2 &v2, int precision = 8, bool close = FALSE);
	void FlatBezierTo(const Vector2 &v1, const Vector2 &v2, const Vector2 &v3, int precision = 8, bool close = FALSE);

	void AddPointCopy(ShapePoint *p);

	void AddShape(Shape *Shape, bool reverse = FALSE, ShapeWindingMode mode = SWM_NoChange, bool canmove = FALSE);
	void AddShape(Shape *Shape, double offsetX, double offsetY, bool reverse = FALSE, ShapeWindingMode mode = SWM_NoChange);
	ImageRegion AddShape(Shape *Shape, Matrix4 &transform, bool reverse = FALSE, ShapeWindingMode mode = SWM_NoChange);
	void AddShape(ShapePresetType preset, double centerX, double centerY, double angle, double sizeX, double sizeY, int precision = 8);
	void AddRectangle(double left, double right, double top, double bottom, double roundCorner = 0.0, int precision = 8);

	void Close();

	void Clear();
	bool IsEmpty();
	bool IsFlat();

	ShapePoint *GetFirstPoint();
	int GetCount();

	Shape *CopyOfShape();
	Shape *TransformOfShape(const Matrix4 &transform);
	Shape *FlattenOfShape(int precision = 8);
	Shape *FlattenOfShape(int precision, double scale);
	Shape *OffsetOfShape(double offset, bool doClipping = TRUE, OutlineJoinType jointype = OJT_Round, int precision = 8, int flattenprecision = 8);
	Shape *OutlineOfShape(double thickness, OutlineLineType linetype = OLT_Solid, OutlineJoinType jointype = OJT_Round, int precision = 8, ShapeWindingMode mode = SWM_Solid, int flattenprecision = 8);
	Shape *ExpandOfShape(double thickness, int precision = 8, int flattenprecision = 8, bool canmodify = FALSE);

	// Just like ExpandOfShape(), except that 'canmodify' is always TRUE, meaning that it could return itself modified.
	Shape *ExpandShape(double thickness, int precision = 8, int flattenprecision = 8);

	void Boundary(Vector2 &low, Vector2 &high);
	void FitSourceTo(const Vector2 &sourcemin, const Vector2 &sourcemax, const Vector2 &destmin, const Vector2 &destmax, bool keepaspect = TRUE);
	void FitTo(const Vector2 &destmin, const Vector2 &destmax, bool keepaspect = TRUE);

	void ConvertFrom(WindowsShape *shape, Matrix4 *transform = NULL);
	void ConvertTo(int &nPoints, ShapeArrayPoint **points, ShapeArrayPointIndex **indices, ShapeArrayEdge **edges,
		double xOffset = 0.0, double yOffset = 0.0, double xScale = 1.0, double yScale = 1.0, int precision = 8);
	void ConvertTo(Vector3f **points, int &nPoints, int **contourLengths, int &nContours);
	void FlatBSplineTo(const Vector2 *v, int vcount, double *knots, int knots_count, int order, BYTE *lin, double *w, int precision, bool close = FALSE);
	void FlatTBSplineTo(const Vector2 *v, int vcount, double *knots, int knots_count, int order, double *w, int precision, bool close = FALSE);

#if !defined(USE_NOSCRIPTING) && !defined(ROTATION)
	DECLARE_LUATYPE(Shape);
#endif
};

#endif // _Shape_H_
