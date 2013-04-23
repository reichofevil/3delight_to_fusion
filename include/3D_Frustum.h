#ifndef _3D_FRUSTUM_H_
#define _3D_FRUSTUM_H_

#include "Plane3f.h"

#include "3D_DataTypes.h"
#include "3D_BoundingVolume.h"


class Matrix4f;
class AABB3D;


// The standard computer graphics frustum always in *world* space.
class FuSYSTEM_API Frustum3D : public RefObject
{
public:
	const static int NumFrustumPlanes = 6;

	enum FrustumPlanes
	{
		Right,
		Left,
		Bottom,
		Top,
		Far,
		Near
	};

	Plane3f Plane[NumFrustumPlanes];			// these planes are always normalized (Note: the normals point outwards from the frustum)

public:
	Frustum3D() {}
	Frustum3D(const Matrix4f &m) { Init(m); }
	~Frustum3D() {}

	void Init(const Matrix4f &m);			// eg. m = Projection * WorldToEye will give a frustum in world space

	// The near plane is oriented so that the normal on the near plane points towards the eye
	const Plane3f &NearPlane() const;	
	const Plane3f &FarPlane() const;

	IT_IntersectionType Contains(const AABB3D &aabb);

	bool Intersects(const Frustum3D &f);

	// Figures out the largest zNear and smallest zFar values assumed by the AABB constrained to the frustum
	void ConstraintedDepthBounds(AABB3D &aabb, float32 &zNear, float32 &zFar);

	bool Contains(const Vector3f &pt);

	// Returns true if any portion of the sphere intersects any part of the frustum's volume
	bool Intersects(const Sphere3D &sphere);

	// Returns true if the whole sphere is contained within the frustum
	bool Contains(const Sphere3D &sphere);

	// A negative result indicates it is on the frustum side of the near plane.
	inline float32 SignedDistanceToNearPlane(const Vector3f &pt);

	// A negative result indicates it is on the frustum side of the near plane.
	float32 SignedDistanceToNearPlane(const Sphere3D &sphere);
};



inline const Plane3f &Frustum3D::NearPlane() const
{
	return Plane[Near];
}

inline const Plane3f &Frustum3D::FarPlane() const
{
	return Plane[Far];
}

inline bool Frustum3D::Contains(const Vector3f &pt)
{
	for (int i = 0; i < 6; i++)
		if (Plane[i].SignedDistance(pt) >= 0.0f)
			return false;
	return true;
}

inline bool Frustum3D::Intersects(const Sphere3D &sphere)
{
	for (int i = 0; i < 6; i++)
		if (Plane[i].SignedDistance(sphere.Center) >= sphere.Radius)
			return false;
	return true;
}

inline bool Frustum3D::Contains(const Sphere3D &sphere)
{
	for (int i = 0; i < 6; i++)
		if (Plane[i].SignedDistance(sphere.Center) >= -sphere.Radius)
			return false;
	return true;
}

inline float32 Frustum3D::SignedDistanceToNearPlane(const Vector3f &pt)
{
	return Plane[Near].SignedDistance(pt);
}

inline float32 Frustum3D::SignedDistanceToNearPlane(const Sphere3D &sphere)
{
	float32 dist = Plane[Near].SignedDistance(sphere.Center);

	if (dist > sphere.Radius)
		return dist - sphere.Radius;
	else if (dist < -sphere.Radius)
		return dist + sphere.Radius;
	else
		return 0.0f;
}









#endif