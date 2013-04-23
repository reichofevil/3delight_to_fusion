#ifndef _BOUNDING_VOLUME3D_H_
#define _BOUNDING_VOLUME3D_H_

#include "3D_DataTypes.h"


class AABB3D;

/*

class FuSYSTEM_API BoundingVolume3D
{
public:
	bool Contains(Vector3f &p);
	bool Contains(Vector4f &p);
	bool Contains(Vector3f &p);

	//// Returns the distances from the plane to the nearest/furthest point on this object (the distances don't have to be
	//// exact, but the interval must enclose the actual values). 
	//void Distance(Plane3D &p, float32 zNear, float32 zFar);

	virtual void glDraw();
	virtual void glDrawWireframe();

};
*/



// TODO - we'll probably want a sphere and eventually a convex hull(kDOP) class

// The AABBs should only be built once we know the the Model matrices
// 

// oriented bounding box
class FuSYSTEM_API OBB3D
{
public:
};

class FuSYSTEM_API Sphere3D
{
public:
	Vector3f Center;
	float32 Radius;
};

/* sphere, ellipsoid, capsules, other bounding volumes... will we need these? -- probably spheres (TODO)
class FuSYSTEM_API SphereBV
{
public:
};

*/

#endif