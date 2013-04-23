#ifndef __POLYPOINT_H__
#define __POLYPOINT_H__

#include "KeyPoint.h"

#define PT_GRAB				0.0001

#define PPT_NCTRL				-1000000.0

#define PPT_DEPTH				(1l << 0)
#define PPT_DISPLACEMENT		(1l << 1)
#define PPT_TIME				(1l << 2)
#define PPT_PRESSURE			(1l << 3)
#define PPT_HEIGHT				(1l << 4)
#define PPT_ANGLEXY				(1l << 5)
#define PPT_ANGLEZ				(1l << 6)
#define PPT_ROTATION			(1l << 7)
#define PPT_PUBLISH				(1l << 8)
#define PPT_OUTER				(1l << 9)
#define PPT_WEIGHT				(1l << 10)

class FuSYSTEM_API PolyPoint: public KeyPoint
{
public:
	double Z;         // depth (0.0 for now)
	double d;         // displacement
	double LeftCX;    // Left control point: Relative X position (i.e.absolute pos = X + LeftHX)
	double LeftCY;    // Left control point: Relative Y position
	double LeftCZ;	   // depth (0.0 for now)
	double RightCX;   //	Right control point: Relative X position
	double RightCY;   //	Right control point: Relative Y position
	double RightCZ;   // depth (0.0 for now)
	uint32 Time;		// time in milliseconds when pt was created (first is 0)
	uint32 Pressure;	// Pen values
	uint32 Height;
	uint32 AngleXY;
	uint32 AngleZ;
	uint32 Rotation;
	FuID PublishID;
	double W;

	PolyPoint(UINT sx = 0, float xx = 0.0, float yx = 0.0, float zx = 0.0, float dx = 0.0, float xl = 0.0, float yl = 0.0, float xr = 0.0, float yr = 0.0);

	PolyPoint& operator = (const PolyPoint& p);

	bool operator == (PolyPoint &p );

	PolyPoint& operator += (const PolyPoint& v) { X += v.X; Y += v.Y; Z += v.Z; W += v.W; Time += v.Time; Pressure += v.Pressure; Height += v.Height; AngleXY += v.AngleXY; AngleZ += v.AngleZ; Rotation += v.Rotation; return *this;}
	PolyPoint& operator -= (const PolyPoint& v) { X -= v.X; Y -= v.Y; Z -= v.Z; W -= v.W; Time -= v.Time; Pressure -= v.Pressure; Height -= v.Height; AngleXY -= v.AngleXY; AngleZ -= v.AngleZ; Rotation -= v.Rotation; return *this;} 
	PolyPoint& operator *= (const double v) { X *= v; Y *= v; Z *= v; W *= v;  Time *= v; Pressure *= v; Height *= v; AngleXY *= v; AngleZ *= v; Rotation *= v; return *this; }
	PolyPoint& operator /= (const double v) { X /= v; Y /= v; Z /= v; W /= v;  Time /= v; Pressure /= v; Height /= v; AngleXY /= v; AngleZ /= v; Rotation /= v; return *this; }

	PolyPoint *Next1(int i, UINT ex) { return (PolyPoint *)KeyPoint::Next1(i, ex); }
};

inline float32 NormaliseAngle(float32 angle)
{
	if(fabs(angle) > 2 * PI)
	{
		float64 ii;
		angle = (float32)modf(angle/(2 * PI),&ii);
		angle *= (2 * PI);
	}
	if(angle > PI) angle -= (2 * PI);
	else if(angle < -PI) angle += (2 * PI);
	return angle;
}

class FuSYSTEM_API FollowingPoint: public PolyPoint
{
public:
	uint8 FollowOne;

	float32 Rad;
	float32 Ang;

	float32 LRad;
	float32 LAng;
	float32 RRad;
	float32 RAng;

	FollowingPoint();
	PolyPoint *FindFollowed(KeyPoint *me, PolyPoint **pn, bool closed);
	void Follow(KeyPoint *me, float32 asp, bool closed);
	void SetOrg(float32 asp, bool closed);
	void OffsetOrgAngle(float32 angle, bool closed);
	FollowingPoint(uint64 follow_int_ext);
	void FollowExt(KeyPoint *me, double asp, bool closed);
	void SetOrgExt(KeyPoint *extp, double asp, bool closed);
	void OffsetOrgAngleExt(KeyPoint *pext, double angle);
	void Follow(KeyPoint *me, float32 asp, class Matrix4 *mt, bool closed);
	void SetHandleOrg(float32 asp, bool closed);
	void SetOrg(float32 asp, Matrix4 *mt, bool closed);
};

inline PolyPoint project(PolyPoint &a)
{
	PolyPoint p;
	p.X = a.X/a.W; p.Y = a.Y/a.W; p.Z = a.Z/a.W;
	return p ;
}

inline PolyPoint operator+(const PolyPoint &a, const PolyPoint &b)
{
	PolyPoint sum;
	sum = a;
	sum += b;
	return sum;
}

inline PolyPoint operator-(const PolyPoint &a, const PolyPoint &b)
{
	PolyPoint diff;
	diff = a;
	diff -= b ;
	return diff ;
}

inline PolyPoint operator * (const PolyPoint &a, const double v)
{
	PolyPoint mul;
	mul = a;
	mul *= v;
	return mul;
}

inline PolyPoint operator / (const PolyPoint &a, const double v)
{
	PolyPoint div;
	div = a;
	div /= v;
	return div;
}

inline double operator*(const PolyPoint& a, const PolyPoint& b)
{
	return a.X*b.X + a.Y*b.Y + a.Z*b.Z;
}

DLLExtern PolyPoint *CopyPolyPointLinkList( PolyPoint *ps, PolyPoint **hd );
DLLExtern uint32 IncrementID( uint32 id);
DLLExtern double SegmentLength(PolyPoint *ps, PolyPoint *pe, double us, double ue, double asp, double (*lfunc)(PolyPoint *, PolyPoint *, double , double));
DLLExtern void SetHullPoints(PolyPoint *p1,PolyPoint *p2, PolyPoint *pm);
DLLExtern double InverseSegmentLength(PolyPoint *ps, PolyPoint *pe, double s, double length, double asp, double (*lfunc)(PolyPoint *, PolyPoint *, double , double), double us = 0.0, double ue = 1.0);

inline void RotZ(double s, double c, double &x, double &y)
{
	double xx = c * x - s * y;
	double yy = s * x + c * y;
	x = xx;
	y = yy;
}

inline void RotZXY(double sinx, double cosx, double siny, double cosy, double sinz, double cosz, double asp, double &x, double &y, double &z)
{
/*	double xx = (cosz * cosy - sinz * sinx * siny) * x - sinz * cosx * y * asp + (cosz * siny + sinz * sinx * cosy) * z;
	double yy = ((sinz * cosy + cosz * sinx * siny) * x + cosz * cosx * y * asp + (sinz * siny - cosz * sinx * cosy) * z) / asp;
	double zz = -cosx * siny * x + sinx * y * asp + cosx * cosy * z;
	x = xx; y = yy; z = zz;*/
	double xx = (cosz * cosy + sinz * sinx * siny) * x + (cosz * sinx * siny - sinz * cosy) * y * asp + cosx * siny * z;
	double yy = (sinz * cosx * x + cosz * cosx * y * asp - sinx * z) / asp;
	double zz = (sinz * sinx * cosy - cosz * siny) * x + (sinz * siny + cosz * sinx * cosy) * y * asp + cosx * cosy * z;
	x = xx; y = yy; z = zz;
}

inline void RotYXZ(double sinx, double cosx, double siny, double cosy, double sinz, double cosz, double asp, double &x, double &y, double &z)
{
/*	double xx = (cosy * cosz + siny * sinx * sinz) * x + (siny * sinx * cosz - cosy * sinz) * y * asp + siny * cosx * z;
	double yy = (cosx * sinz * x + cosx * cosz * y * asp - sinx * z) / asp;
	double zz = (cosy * sinx * sinz - siny * cosz) * x + (siny * sinz + cosy * sinx *cosz) * y * asp + cosy * cosx * z; 
	x = xx; y = yy; z = zz;*/

	double xx = (cosy * cosz - siny * sinx * sinz) * x - cosx * sinz * y * asp + (siny * cosz + cosy * sinx * sinz) * z;
	double yy = ((cosy * sinz + siny * sinx * cosz) * x + cosx * cosz * y * asp + (siny * sinz - cosy * sinx * cosz) * z)/asp;
	double zz = -siny * cosx * x + sinx * y * asp + cosy * cosx *z;
	x = xx; y = yy; z = zz;
}

// this is in the case of finding a point in the Z==0 plane
inline void RotYXZ(double sinx, double cosx, double siny, double cosy, double sinz, double cosz, double asp, double &x, double &y)
{
/*	double z = cosx==0.0 || cosy==0.0? FLT_EPSILON : ((siny * cosz - cosy * sinx * sinz) * x - (siny * sinz + cosy * sinx * cosz) * y * asp)/(cosx * cosy);
	double xx = (cosy * cosz + siny * sinx * sinz) * x + (siny * sinx * cosz - cosy * sinz) * y * asp + siny * cosx * z;
	double yy = (cosx * sinz * x + cosx * cosz * y * asp - sinx * z) / asp;
	x = xx; y = yy;*/

	double z = cosx==0.0 || cosy==0.0? FLT_EPSILON : (siny*cosx * x - sinx * y * asp) / (cosx * cosy); 
	double xx = (cosy * cosz - siny * sinx * sinz) * x - cosx * sinz * y * asp + (siny * cosz + cosy * sinx * sinz) * z;
	double yy = ((cosy * sinz + siny * sinx * cosz) * x + cosx * cosz * y * asp + (siny * sinz - cosy * sinx * cosz) * z)/asp;
	x = xx; y = yy;
}
#endif