#ifndef _VECTOR_MATH_FU_H_
#define _VECTOR_MATH_FU_H_


#include "DFID.h"
#include "Types.h"
#include "Debug.h"
#include "Pixel.h"
#include "GLExtensions.h"

#include "dfmath.h"


#define _VECTOR_MATH_H_INCLUDED_

#define align16 __declspec(align(16))


const float32 FltEpsilon = 0.0001f;


class Vector2;
class Vector3;
class Vector4;

class Matrix3;
class Matrix4;
class Matrix4f;

class Vector2f;
class Vector3f;
class Vector4f;

#define Det2(a, b, c, d) ((a) * (d) - (b) * (c))

inline float32 Lerp(float32 a, float32 b, float32 t) { return (1.0f - t) * a + t * b; }

#include "Point3D.h"
#include "Matrix4.h"			// Must be before Matrix3.h
#include "Matrix3.h"

#include "Vector2f.h"		// vectors must be
#include "Vector3f.h"		// included before matrix
#include "Vector4f.h"
#include "Matrix4f.h"

extern FuSCRIPT_API const Vector2f ONE2F;
extern FuSCRIPT_API const Vector3f ONE3F;
extern FuSCRIPT_API const Vector4f ONE4F;

extern FuSCRIPT_API const Vector2f WHITE2F;
extern FuSCRIPT_API const Vector3f WHITE3F;
extern FuSCRIPT_API const Vector4f WHITE4F;			// tried to make these static class members (eg. Vector4f::White) but need a .cpp to define them in which is accessable to all DLLs (incld scripting)

extern FuSCRIPT_API const Vector2f ZERO2F;
extern FuSCRIPT_API const Vector3f ZERO3F;
extern FuSCRIPT_API const Vector4f ZERO4F;

extern FuSCRIPT_API const Vector2f BLACK2F;
extern FuSCRIPT_API const Vector3f BLACK3F;
extern FuSCRIPT_API const Vector4f BLACK4F;


// Vector2f
inline Vector2f::Vector2f(const Vector4 &v) : X(v.x), Y(v.y) {}
inline Vector2f::Vector2f(const Vector3f &v) : X(v.X), Y(v.Y) {}
inline Vector2f::Vector2f(const Vector4f &v) : X(v.X), Y(v.Y) {}


// Vector3f
inline Vector3f::Vector3f(const Vector2f &v) : X(v.X), Y(v.Y), Z(0.0f) {}
inline Vector3f::Vector3f(const Vector2f &v, float32 z) : X(v.X), Y(v.Y), Z(z) {}
inline Vector3f::Vector3f(const Vector4 &v) : X(v.x), Y(v.y), Z(v.z) {}
inline Vector3f::Vector3f(const Vector4f &v) : X(v.X), Y(v.Y), Z(v.Z) {}
inline Vector3f::Vector3f(const Vector3d &v) : X(v.X), Y(v.Y), Z(v.Z) {}


// Vector4f
inline Vector4f::Vector4f(const Vector4 &v) : X(v.x), Y(v.y), Z(v.z), W(v.w) {}
inline Vector4f::Vector4f(const Vector3f &v) : X(v.X), Y(v.Y), Z(v.Z), W(1.0) {}
inline Vector4f::Vector4f(const Vector2f &v) : X(v.X), Y(v.Y), Z(0.0f), W(1.0) {}
inline Vector4f::Vector4f(const Vector3f &v, float32 w) : X(v.X), Y(v.Y), Z(v.Z), W(w) {}


// Vector3d
inline Vector3d::Vector3d(const Vector4 &v) : X(v.x), Y(v.y), Z(v.z) {}
inline Vector3d::Vector3d(const Vector3f &v) : X(v.X), Y(v.Y), Z(v.Z) {}
inline Vector3d::Vector3d(const Vector4f &v) : X(v.X), Y(v.Y), Z(v.Z) {}
inline Vector3d Vector3d::operator*(const Matrix4 &m) const
{
	return Vector3d(
		X * m.n11 + Y * m.n21 + Z * m.n31 + m.n41,
		X * m.n12 + Y * m.n22 + Z * m.n32 + m.n42,
		X * m.n13 + Y * m.n23 + Z * m.n33 + m.n43);
}

// Multiplication (result = b * p) of a 4x4 matrix b by a 1x4 column vector p = (p.X, p.Y, p.Z, 1.0)
inline Vector3d operator*(const Matrix4 &m, const Vector3d &p) 
{
	return Vector3d(
		p.X * m.n11 + p.Y * m.n12 + p.Z * m.n13 + m.n14,
		p.X * m.n21 + p.Y * m.n22 + p.Z * m.n23 + m.n24,
		p.X * m.n31 + p.Y * m.n32 + p.Z * m.n33 + m.n34);
}



// Vector4 
inline Vector4::Vector4(const Vector3f &v) : x(v.X), y(v.Y), z(v.Z), w(1.0) {}
inline Vector4::Vector4(const Vector4f &v) : x(v.X), y(v.Y), z(v.Z), w(v.W) {}
inline Vector4 &Vector4::operator=(const Vector3f &v) { x = v.X;  y = v.Y;  z = v.Z;  w = 1.0;  return *this; }


// Matrix4
inline Vector3f Matrix4::TransformNormal(const Vector3f &norm) const
{
	// Vectors transform differently from points.  A vector is treated like (x, y, z, 0), while
	// a point is treat like (x, y, z, 1) by 4x4 homogenous transforms.
	Vector3f n;
	n.X = n11 * norm.X + n12 * norm.Y + n13 * norm.Z;
	n.Y = n21 * norm.X + n22 * norm.Y + n23 * norm.Z;
	n.Z = n31 * norm.X + n32 * norm.Y + n33 * norm.Z;

	return Normalize(n);
}

inline void Matrix4::BatchVertexTransform(Vector3f *vertices, int numVertices) const
{
	for (int i = 0; i < numVertices; i++)						// TODO - write a SSE version for this
		vertices[i] = Vector3f(*this * vertices[i]);
}

inline void Matrix4::BatchNormalTransform(Vector3f *normals, int numNormals) const
{
	for (int i = 0; i < numNormals; i++)						// TODO - write a SSE version for this
		normals[i] = TransformNormal(normals[i]);
}

inline Vector3f operator*(const Vector3f &p, const Matrix4 &m)
{
	return Vector3f(
		p.X * m.n11 + p.Y * m.n21 + p.Z * m.n31 + m.n41,
		p.X * m.n12 + p.Y * m.n22 + p.Z * m.n32 + m.n42,
		p.X * m.n13 + p.Y * m.n23 + p.Z * m.n33 + m.n43);
}

// Multiplication (result = b * p) of a 4x4 matrix b by a 1x4 column vector p = (p.X, p.Y, p.Z, 1.0)
inline Vector3f operator*(const Matrix4 &b, const Vector3f &p) 
{
	return Vector3f(
		p.X * b.n11 + p.Y * b.n12 + p.Z * b.n13 + b.n14,
		p.X * b.n21 + p.Y * b.n22 + p.Z * b.n23 + b.n24,
		p.X * b.n31 + p.Y * b.n32 + p.Z * b.n33 + b.n34);
}

inline Matrix4::Matrix4(const Matrix4f &m)
{
	n11 = m.M00;	n12 = m.M10;	n13 = m.M20;	n14 = m.M30;
	n21 = m.M01;	n22 = m.M11;	n23 = m.M21;	n24 = m.M31;
	n31 = m.M02;	n32 = m.M12;	n33 = m.M22;	n34 = m.M32;
	n41 = m.M03;	n42 = m.M13;	n43 = m.M23;	n44 = m.M33;
}


// Matrix4f
inline Matrix4f::Matrix4f(const Matrix4 &m)
{
	M00 = m.n11;	M10 = m.n12;	M20 = m.n13;	 M30 = m.n14;
	M01 = m.n21;	M11 = m.n22;	M21 = m.n23;	 M31 = m.n24;
	M02 = m.n31;	M12 = m.n32;	M22 = m.n33;	 M32 = m.n34;
	M03 = m.n41;	M13 = m.n42;	M23 = m.n43;	 M33 = m.n44;
}

#endif