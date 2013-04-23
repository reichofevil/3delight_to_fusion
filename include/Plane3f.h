#ifndef _PLANE3f_H_
#define _PLANE3f_H_

#include "VectorMath.h"
#include "3D_DataTypes.h"


class AABB3D;


// A plane in 3D space defined by the equation A*x + B*y + C*z + D = 0.
// The side of the plane on which the normal points is defined to be the positive half space.
// If p is in the positive half of the plane, we say p is 'outside' of the plane.
// If p is in the negative half of the plane, we say p is 'inside' the plane.
class FuSYSTEM_API Plane3f

{
public:
	union
	{
		struct
		{
			float32 A;
			float32 B;
			float32 C;
			float32 D;
		};

		struct
		{
			Vector3f n;
			float32 nD;
		};

		float32 Coeff[4];
	};

public:
	Plane3f() {}

	Plane3f(float32 coeff[4]) 
	{ 
		for (int i = 0; i < 4; i++)
			Coeff[i] = coeff[4];
	}

	Plane3f(float32 A, float32 B, float32 C, float32 D) 
	{ 
		this->A = A;
		this->B = B;
		this->C = C;
		this->D = D;
	}

	Plane3f &operator=(const Plane3f &rhs)
	{
		A = rhs.A;
		B = rhs.B;
		C = rhs.C;
		D = rhs.D;

		return *this;
	}

	// Creates a plane containing the triangle defined by (p0, p1, p2).  Assuming the points are provided in counter
	// clockwise order, the normal is determined via the RHR (right-hand rule).
	Plane3f(const Vector3f &p0, const Vector3f &p1, const Vector3f &p2)
	{
		Init(p0, p1, p2);
	}

	// Creates a plane containing the triangle defined by (p[0], p[1], p[2]).  Assuming the points are provided in counter
	// clockwise order, the normal is determined via the RHR (right-hand rule).
	Plane3f(Vector3f pts[3]) { Plane3f(pts[0], pts[1], pts[2]); }

	// Creates a plane containing the point p with the provided normal
	Plane3f(const Vector3f &norm, const Vector3f &p)
	{
		n = norm;
		nD = -Dot(norm, p);
	}

	void Init(const Vector3f &p0, const Vector3f &p1, const Vector3f &p2)
	{
		Vector3f perp = (p1 - p0) ^ (p2 - p1);

		Coeff[0] = perp.X;
		Coeff[1] = perp.Y;
		Coeff[2] = perp.Z;
		Coeff[3] = - (perp * p0);
	}

	// Returns the closest (unsigned) distance from the point p to this plane.  (c.f. SignedDistance())
	float32 Distance(const Vector3f &p) const
	{
		return fabs(A * p.X + B * p.Y + C * p.Z + D);
	}

	// Returns the signed distance from the point p to this plane.  The direction of the normal of the plane determines
	// the sign of the result.  If the point is on the same side of the plane as which the norm points, the result is
	// positive, if its on opposite side of the normal, the result it negative.
	float32 SignedDistance(const Vector3f &p) const
	{
		return A * p.X + B * p.Y + C * p.Z + D;
	}

	float32 SignedDistance(const Vector4f &pt) const
	{
		return A * pt.X + B * pt.Y + C * pt.Z + D * pt.W;
	}

	// Intersects the line segment l(t) = (p1 - p0)*t + p0 with this plane, and returns the parameter 't' of intersection.
	float32 SegmentIntersectParam(const Vector3f &p0, const Vector3f &p1) const
	{
		float32 t = -(D + n * p0) / (n * (p1 - p0));
		//ASSERT(t >= 0.0f && t <= 1.0f);
		return t;
	}

	// Flips the direction that this plane's normal is facing
	void Flip()
	{
		for (int i = 0; i < 4; i++)
			Coeff[i] = -Coeff[i];
	}

	// Normalizes the plane equation, so that Vector3f(A, B, C) is the unit normal for the plane.
	void Normalize()
	{
		float32 invLen = 1.0f / Length(n);
		A *= invLen;
		B *= invLen;
		C *= invLen;
		D *= invLen;
	}

	bool IsNormalized() const
	{
		return n.IsNormalized();
	}

	// Calculates the normal to this plane
	Vector3f Normal() const
	{
		return ::Normalize(n);
	}

	IT_IntersectionType Contains(const Vector3f &p, float32 planeThickness = 0.0f) const
	{
		float32 dist = SignedDistance(p);

		IT_IntersectionType ret;

		if (dist > planeThickness)
			ret = IT_Outside;
		else if (dist < -planeThickness)
			ret = IT_Inside;
		else
			ret = IT_Intersecting;

		return ret;
	}

	IT_IntersectionType Contains(const AABB3D &aabb) const;

	void SignedDistance(const AABB3D &aabb, float32 &nearDist, float32 &farDist) const;

	// Returns the nearest point on this plane to pt.
	Vector3f NearestPoint(const Vector3f &pt);

	bool IsFinite() const
	{
		return _finite(A) && _finite(B) && _finite(C) && _finite(D);
	}

	bool IsNaN() const
	{
		return _isnan(A) || _isnan(B) || _isnan(C) || _isnan(D);
	}
};



#endif