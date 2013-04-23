#ifndef _AABB_3D_H_
#define _AABB_3D_H_

#include "VectorMath.h"


class Matrix4f;



// AABB - axis-aligned bounding box collision primitive

class FuSYSTEM_API AABB3D //: public BoundingVolume3D
{
public:
	union
	{
		struct
		{
			Vector3f Min;			// if you are setting these values directly instead of using an accessor, you must also make 'Set = true'
			Vector3f Max;
		};

		struct
		{
			Vector3f MinMax[2];
		};
	};

public:
	AABB3D();
	AABB3D(Vector3f &min, Vector3f &max);
	~AABB3D();

	void Copy(const AABB3D &a);
	AABB3D Union(const AABB3D &a);
	
	AABB3D& operator=(const AABB3D &a)
	{
		Copy(a);
		return *this;
	}

	AABB3D& operator |= (const AABB3D &a);
	AABB3D& operator |= (const Vector3f &p);
	AABB3D operator | (const AABB3D &a) { return Union(a); }

	void GlDrawLines() const;

	// Returns closest (unsigned) distance to the AABB.  If the point is inside the AABB ~0.0f is returned.
	float32 DistanceTo(Vector3f &pt) const;

	// This method returns the perpendicular distance from the point to nearest and furthest points on the AABB.  It is assumed that the point
	// and AABB are defined in the same coordinate system.  If the point is contained in the AABB, it returns ~0.0f for near and the furthest
	// distance to far.  The returned distances are unsigned.
	void DistanceTo(const Vector3f &pt, float32 &near, float32 &far) const;

	//void Transform(const Matrix4f &m, AABB3D &out) const;

	void Init();								// initializes to an inverted bbox (min = +FLT_MAX, max = -FLT_MAX)	
	void Zero();								// initializes to a zero volume bounding box located at (0,0,0)

	void Inflate(float32 f);				// f should be >= 0.0f
	void Inflate(const Vector3f &v);		// v should be >= 0.0f
	void Inflate(float32 x, float32 y, float32 z);

	Vector3f Center() const					{ return 0.5f * (Max + Min); }
	Vector3f Size() const					{ return Max - Min; }

	/** 
	 * Returns true if this bounding box is bounded by a [-s,+s]^3 cube centered at the origin.  Used for checking for unitialized bounding
	 * boxes at various points.
	 */
	bool IsBounded(float32 s = 1000000000.0f) const;

	void GetBoundingSphere(Vector3f &center, float32 &radius) const;

	float GetSurfaceArea() const
	{ 
		Vector3f s = Max - Min;
		return 2.0f * (s.X * s.Y + s.X * s.Z + s.Y * s.Z); 
	}

	/** 
	 * Gets radius of smallest sphere containing this aabb.
	 */
	float32 GetRadius() const;

	void GetValue(Vector3f &min, Vector3f &max) const;
	void SetValue(const Vector3f &min, const Vector3f &max);
};

FuSYSTEM_API AABB3D operator*(const Matrix4f &m, const AABB3D &aabb);


#endif