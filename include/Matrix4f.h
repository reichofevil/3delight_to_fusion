#ifndef _MATRIX_4F_H_
#define _MATRIX_4F_H_

#ifndef _VECTOR_MATH_H_INCLUDED_
#error Include VectorMath.h
#endif

#include "Vector3f.h"
#include "Vector4f.h"
#include "dfscript.h"				// for FuASSERT()


// A SSE optimized 32-bit floating point matrix class.  Matrices are stored in C/C++ row
// major style, not in column major OpenGL style.  Vectors should be viewed as column
// vectors, ie. 1 column with 4 rows.  To apply a matrix transform to a vector v to 
// produce a new vector v', matrix multiplication occurs as v' = M * v:
//
//  |v0'| = [m00 m01 m02 m03] |v0|
//  |v1'| = [m10 m11 m12 m13] |v1|
//  |v2'| = [m20 m21 m22 m23] |v3|
//  |v3'| = [m30 m31 m32 m33] |v4|
//
// If a vector v is to be transformed by M1, and then by M2, and then finally by M3,
// the concatentation would work as M3 * M2 * M1 * v.
class Matrix4f
{
public:
	union
	{
		float32 M[16];

		float32 V[4][4];								// first index specifies row, second index specifies column

		struct
		{
			float32 M00, M01, M02, M03;			// first index is the row, second is the column
			float32 M10, M11, M12, M13;
			float32 M20, M21, M22, M23;
			float32 M30, M31, M32, M33;
		};
	};

public:
	// Constructs an identity matrix
	Matrix4f() 
	{
		Identity();
	}

	// allocation of uninitialized arrays of matrices (avoids ctor calls, so its faster)
	static Matrix4f *AllocArray(int n) { return (Matrix4f *) malloc(sizeof(Matrix4f) * n); }
	static void FreeArray(Matrix4f *arr) { free(arr); }

	Matrix4f(const Matrix4 &m);

	// m is of the format { m00, m01, m02, m03, m10, m11, m12, m13, .... m33 }
	Matrix4f(float32 m[16])
	{
		M00 = m[0];		M01 = m[1];		M02 = m[2];		M03 = m[3];
		M10 = m[4];		M11 = m[5];		M12 = m[6];		M13 = m[7];
		M20 = m[8];		M21 = m[9];		M22 = m[10];	M23 = m[11];
		M30 = m[12];	M31 = m[13];	M32 = m[14];	M33 = m[15];		
	}

	Matrix4f(float32 row0[4], float32 row1[4], float32 row2[4], float32 row3[4])
	{
		M00 = row0[0];	M01 = row0[1];	M02 = row0[2];	M03 = row0[3];
		M10 = row1[0];	M11 = row1[1];	M12 = row1[2];	M13 = row1[3];
		M20 = row2[0];	M21 = row2[1];	M22 = row2[2];	M23 = row2[3];
		M30 = row3[0];	M31 = row3[1];	M32 = row3[2];	M33 = row3[3];	
	}

	Matrix4f(Vector4f &row0, Vector4f &row1, Vector4f &row2, Vector4f &row3) 
	{
		M00 = row0.X;	M01 = row0.Y;	M02 = row0.Z;	M03 = row0.W;
		M10 = row1.X;	M11 = row1.Y;	M12 = row1.Z;	M13 = row1.W;
		M20 = row2.X;	M21 = row2.Y;	M22 = row2.Z;	M23 = row2.W;
		M30 = row3.X;	M31 = row3.Y;	M32 = row3.Z;	M33 = row3.W;	
	}

	Matrix4f(float32 m00, float32 m01, float32 m02, float32 m03,
				float32 m10, float32 m11, float32 m12, float32 m13,
				float32 m20, float32 m21, float32 m22, float32 m23,
				float32 m30, float32 m31, float32 m32, float32 m33)
	{
		M00 = m00;	M01 = m01;	M02 = m02;	M03 = m03;
		M10 = m10;	M11 = m11;	M12 = m12;	M13 = m13;
		M20 = m20;	M21 = m21;	M22 = m22;	M23 = m23;
		M30 = m30;	M31 = m31;	M32 = m32;	M33 = m33;
	}

	Matrix4f(const Matrix4f &m)
	{
		*this = m;
	}

	~Matrix4f() {}

	bool IsIdentity(float32 epsilon = 0.0f);

	Matrix4f &operator=(const Matrix4f &m)
	{
		M00 = m.M00;	M01 = m.M01;	M02 = m.M02;	M03 = m.M03;
		M10 = m.M10;	M11 = m.M11;	M12 = m.M12;	M13 = m.M13;
		M20 = m.M20;	M21 = m.M21;	M22 = m.M22;	M23 = m.M23;
		M30 = m.M30;	M31 = m.M31;	M32 = m.M32;	M33 = m.M33;

		return *this;
	}

	//bool operator==(const Matrix4f &m) {}
	//bool operator!=(const Matrix4f &m) {}

	// Sets this matrix to all zeros.
	void Zero() 
	{  
		M00 = M01 = M02 = M03 = M10 = M11 = M12 = M13 = M20 = M21 = M22 = M23 = M30 = M31 = M32 = M33 = 0.0f;
	}

	// Sets this matrix to the 4x4 identity matrix
	void Identity() 
	{
		M00 = 1.0f;		M01 = 0.0f;		M02 = 0.0f;		M03 = 0.0f;
		M10 = 0.0f;		M11 = 1.0f;		M12 = 0.0f;		M13 = 0.0f;
		M20 = 0.0f;		M21 = 0.0f;		M22 = 1.0f;		M23 = 0.0f;
		M30 = 0.0f;		M31 = 0.0f;		M32 = 0.0f;		M33 = 1.0f;		
	}

	// Multiplies this matrix by a translation matrix, that is, if M is this matrix and T is a translation matrix,
	// then M is replaced with M * T.
	Matrix4f &Translate(float32 tx, float32 ty, float32 tz)
	{
		M03 = M00 * tx + M01 * ty + M02 * tz + M03;
		M13 = M10 * tx + M11 * ty + M12 * tz + M13;
		M23 = M20 * tx + M21 * ty + M22 * tz + M23;

		return *this;
	}

	Matrix4f &Translate(const Vector3f &t)
	{
		return Translate(t.X, t.Y, t.Z);
	}

	/**
	 * If M is this matrix and P is the perspective projection matrix then M is replaced with M * P.  There is one caveat to be aware of...
	 * as in glPerspective(), the arguments n and f are the distances to the near and far planes along the -z axis, not the actual z 
	 * values of the near and far planes.  So one expects f > n > 0.0.  The actual z values for the near/far planes are -n
	 * and -f and the the matrix operates on z values in the range -n <= z <= -f.  This matrix will map the near (z = -n) and 
	 * far (z = -f) planes to -1 and +1 respectively.
	 * @param fovY   The angle subtended between the top & bottom frustum planes in degrees.  
	 * @param aspect width / height
	 * @param n      Distance to near plane along the -z axis.  eg. if the near plane is at z = -0.05 in eyespace you would set n = 0.05f
	 * @param f      Distance to the far plane along the -z axis.  eg. if the far plane is at z = -1000.0 in eyespace you would set f = 1000.0f
	 */
	Matrix4f &Perspective(float32 fovy, float32 aspect, float32 n, float32 f)
	{
		FuASSERT((n > 0.0f && f > 0.0f && f > n), ("invalid near/far"));

		Matrix4f p;

		float32 F = 1.0f / tanf(DegToRad(fovy) * 0.5f);
		float32 ifn = 1.0f / (f - n);

		p.M00 = F / aspect;
		p.M11 = F;
		p.M22 = -(f + n) * ifn;
		p.M23 = -2.0f * f * n * ifn;
		p.M32 = -1.0f;
		p.M33 = 0.0f;
		p.M01 = p.M02 = p.M03 = p.M10 = p.M12 = p.M13 = p.M20 = p.M21 = p.M30 = p.M31 = 0.0f;

		return (*this *= p);                  
	}

	/**
	 * If M is this matrix and F is the perspective projection matrix then M is replaced with M * F.  There is one caveat to be aware of...
	 * as in glFrustum(), the arguments n and f are the distances to the near and far planes along the -z axis, not the actual z 
	 * values of the near and far planes.  So one expects f > n > 0.0.  The actual z values for the near/far planes are -n
	 * and -f and the the matrix operates on z values in the range -n <= z <= -f.  This matrix will map the near (z = -n) and 
	 * far (z = -f) planes to -1 and +1 respectively.  The other arguements (l/r/t/b) are the actual positions of their respective 
	 * clip planes in eye space so one expects r > 0.0 > l and t > 0.0 > b.
	 * @param l The x value of the left edge of the frustum at the near plane (z = -n)
	 * @param r The x value of the right edge of the frustum at the near plane (z = -n)
	 * @param t The y value of the top edge of the frustum at the near plane (z = -n)
	 * @param b The y value of the bottom edge of the frustum at the near plane (z = -n)
	 * @param n Distance to near plane along the -z axis.  eg. if the near plane is at z = -0.05 in eyespace you would set n = 0.05f
	 * @param f Distance to the far plane along the -z axis.  eg. if the far plane is at z = -1000.0 in eyespace you would set f = 1000.0f
	 */
	Matrix4f &Frustum(float32 l, float32 r, float32 b, float32 t, float32 n, float32 f)
	{
		FuASSERT((l < r && b < t && f > n), ("invalid frustum"));

		Matrix4f p;

		float32 irl = 1.0f / (r - l);
		float32 itb = 1.0f / (t - b);
		float32 ifn = 1.0f / (f - n);

		p.M00 = 2.0f * n * irl;
		p.M02 = (r + l) * irl;
		p.M11 = 2.0f * n * itb;
		p.M12 = (t + b) * itb;
		p.M22 = -(f + n) * ifn;
		p.M23 = -2.0f * f * n * ifn;
		p.M32 = -1.0f;
		p.M01 = p.M03 = p.M10 = p.M13 = p.M20 = p.M21 = p.M30 = p.M31 = p.M33 = 0.0f;

		return (*this *= p);  
	}

	//// Constructs a perspective matrix for use with a W-buffer.  Unlike with Perspective(), its perfectly valid to set zNear = 0.0f
	//Matrix4f &PerspectiveW(float32 fovy, float32 aspect, float32 zNear, float32 zFar)
	//{
	//	uassert(znear > 0.0f && zfar > 0.0f);
	//	float32 w = 2.0f / viewportWidth;
	//	float32 h = 2.0f / viewportHeight;
	//	float32 q = zFar / (zFar - zNear);
	//	Matrix4f p(
	//		w,		0,		0,		0,
	//		0,		h,		0,		0,
	//		0,		0,		q,		-q * zNear,
	//		0,		0,		1,		0);
	//	return (*this *= p);
	//}

	inline Matrix4f Ortho(float32 width, float32 height, float32 depth)
	{
		return Ortho(0.0, width, 0.0, height, 0.0, depth);
	}

	/**
	 * If M is this matrix and O is the orthographic projection matrix then M is replaced with M * O.  There is one caveat to be aware of...
	 * as in glOrtho(), the arguments n and f are the distances to the near and far planes along the -z axis, not the actual z 
	 * values of the near and far planes.  So one expects f > n > 0.0.  The actual z values for the near/far planes are -n
	 * and -f and the the matrix operates on z values in the range -n <= z <= -f.  This matrix will map the near (z = -n) and 
	 * far (z = -f) planes to -1 and +1 respectively.  The other arguements (l/r/t/b) are the actual positions of their respective 
	 * clip planes in eye space so one expects r > 0.0 > l and t > 0.0 > b.
	 * @param l The x value of the left edge of the frustum
	 * @param r The x value of the right edge of the frustum
	 * @param t The y value of the top edge of the frustum
	 * @param b The y value of the bottom edge of the frustum
	 * @param n Distance to near plane along the -z axis.  eg. if the near plane is at z = -0.05 in eyespace you would set n = 0.05f
	 * @param f Distance to the far plane along the -z axis.  eg. if the far plane is at z = -1000.0 in eyespace you would set f = 1000.0f
	 */
	inline Matrix4f Ortho(float32 l, float32 r, float32 b, float32 t, float32 n, float32 f)
	{
		FuASSERT((l <=  0.0f && r >  0.0f && l < r), ("invalid left/right"));
		FuASSERT((b <=  0.0f && t >  0.0f && b < t), ("invalid bottom/top"));
		FuASSERT(f > n, ("invalid near/far"));

		float32 irl = 1.0f / (r - l);
		float32 itb = 1.0f / (t - b);
		float32 ifn = 1.0f / (f - n);

		Matrix4f p(
			2.0f * irl,		0.0f,				0.0f,				-(r + l) * irl,
			0.0f,				2.0f * itb,		0.0f,				-(b + t) * itb,
			0.0f,				0.0f,			  -2.0f * ifn,		-(f + n) * ifn,
			0.0f,				0.0f,				0.0f,				1.0f);
		 
		return (*this *= p);
	}

	// Returns true if the individual elements of this matrix are equal to those of n plus or minus epsilon.
	bool Equals(const Matrix4f &m, float epsilon = FltEpsilon)
	{
		float32 diff = 0.0f;
		for (int i = 0; i < 16; i++)
			diff += fabs(M[i] - m.M[i]);

		return diff < epsilon;
	}

	void SetRow(int i, Vector4f &p)
	{
		uassert(i >=0 && i < 4);
		V[i][0] = p.X;	V[i][1] = p.Y;	V[i][2] = p.Z;	V[i][3] = p.W;
	}

	void SetColumn(int i, Vector4f &p)
	{
		uassert(0);
	}

	// Loads this matrix into OpenGL (uses whatever matrix mode is active, doesn't affect any OpenGL state)
	void glLoadMatrix() const
	{
		uassert(glLoadTransposeMatrixfARB);
		glLoadTransposeMatrixfARB(M);				// OpenGL stores its matrices transposed in memory (but not with the actual components transposed)
	}

	// If C is the current GL transform, then its replaced with C * M.  Vectors get transformed thus:  v' = C * M * v
	void glMultMatrix() const
	{
		uassert(glMultTransposeMatrixfARB);
		glMultTransposeMatrixfARB(M);
	}

	void NoTrans();
	void NoRot();
	void NoScale();

	void Scale(float32 s)
	{
		Scale(s, s, s);
	}

	void Scale(float32 sx, float32 sy, float32 sz) 
	{
		M00 *= sx;	M01 *= sy;	M02 *= sz;
		M10 *= sx;	M11 *= sy;	M12 *= sz;
		M20 *= sx;	M21 *= sy;	M22 *= sz;
		M30 *= sx;	M31 *= sy;	M32 *= sz;
	}

	void Scale(const Vector3f &s)
	{
		Scale(s.X, s.Y, s.Z);
	}
	
	// Multiplies this matrix by a matrix that does a rotation around the positive X-axis.  That is, if M is this matrix and Rx is the
	// rotation matrix, then M is replaced with M * Rx.  All rotations are counterclockwise, measured in degrees, and have their 
	// rotation direction defined by the RHR (right handed-rule).
	Matrix4f &RotX(float32 degrees);

	// Multiplies this matrix by a matrix that does a rotation around the positive Y-axis.  That is, if M is this matrix and Ry is the
	// rotation matrix, then M is replaced with M * Ry.  All rotations are counterclockwise, measured in degrees, and have their 
	// rotation direction defined by the RHR (right handed-rule).
	Matrix4f &RotY(float32 degrees);

	// Multiplies this matrix by a matrix that does a rotation around the positive Z-axis.  That is, if M is this matrix and Rz is the
	// rotation matrix, then M is replaced with M * Rz.  All rotations are counterclockwise, measured in degrees, and have their 
	// rotation direction defined by the RHR (right handed-rule).
	Matrix4f &RotZ(float32 degrees);

	Matrix4f &RotAxis(const Vector3f &axis, float32 radians);

	// Angles are in degrees.
	Matrix4f Rotate(float32 angleX, float32 angleY, float32 angleZ, int order);

	// Multiplies this matrix by a matrix that does a translation along the vector (x, y, z).  That is, if M is this matrix and T is the
	// translation matrix, then M is replaced with M * T 
	Matrix4f Move(Vector3f &v);
	Matrix4f Move(float32 x, float32 y, float32 z);

	
	//Matrix4f &operator-=(const Matrix4f &m) {}
	//Matrix4f &operator+=(const Matrix4f &m) {}

	Matrix4f &operator*=(const Matrix4f &m) 
	{
		*this = *this * m;
		/*
		Matrix4f c;

		c.M00 = M00 * m.M00 + M01 * m.M10 + M02 * m.M20 + M03 * m.M30;
		c.M01 = M00 * m.M01 + M01 * m.M11 + M02 * m.M21 + M03 * m.M31;
		c.M02 = M00 * m.M02 + M01 * m.M12 + M02 * m.M22 + M03 * m.M32;
		c.M03 = M00 * m.M03 + M01 * m.M13 + M02 * m.M23 + M03 * m.M33;

		c.M10 = M10 * m.M00 + M11 * m.M10 + M12 * m.M20 + M13 * m.M30;
		c.M11 = M10 * m.M01 + M11 * m.M11 + M12 * m.M21 + M13 * m.M31;
		c.M12 = M10 * m.M02 + M11 * m.M12 + M12 * m.M22 + M13 * m.M32;
		c.M13 = M10 * m.M03 + M11 * m.M13 + M12 * m.M23 + M13 * m.M33;

		c.M20 = M20 * m.M00 + M21 * m.M10 + M22 * m.M20 + M23 * m.M30;
		c.M21 = M20 * m.M01 + M21 * m.M11 + M22 * m.M21 + M23 * m.M31;
		c.M22 = M20 * m.M02 + M21 * m.M12 + M22 * m.M22 + M23 * m.M32;
		c.M23 = M20 * m.M03 + M21 * m.M13 + M22 * m.M23 + M23 * m.M33;

		c.M30 = M30 * m.M00 + M31 * m.M10 + M32 * m.M20 + M33 * m.M30;
		c.M31 = M30 * m.M01 + M31 * m.M11 + M32 * m.M21 + M33 * m.M31;
		c.M32 = M30 * m.M02 + M31 * m.M12 + M32 * m.M22 + M33 * m.M32;
		c.M33 = M30 * m.M03 + M31 * m.M13 + M32 * m.M23 + M33 * m.M33;

		*this = c;
		*/

		return *this;
	}

	Matrix4f operator*(const Matrix4f &m) const 
	{
		Matrix4f c;

		c.M00 = M00 * m.M00 + M01 * m.M10 + M02 * m.M20 + M03 * m.M30;
		c.M01 = M00 * m.M01 + M01 * m.M11 + M02 * m.M21 + M03 * m.M31;
		c.M02 = M00 * m.M02 + M01 * m.M12 + M02 * m.M22 + M03 * m.M32;
		c.M03 = M00 * m.M03 + M01 * m.M13 + M02 * m.M23 + M03 * m.M33;

		c.M10 = M10 * m.M00 + M11 * m.M10 + M12 * m.M20 + M13 * m.M30;
		c.M11 = M10 * m.M01 + M11 * m.M11 + M12 * m.M21 + M13 * m.M31;
		c.M12 = M10 * m.M02 + M11 * m.M12 + M12 * m.M22 + M13 * m.M32;
		c.M13 = M10 * m.M03 + M11 * m.M13 + M12 * m.M23 + M13 * m.M33;

		c.M20 = M20 * m.M00 + M21 * m.M10 + M22 * m.M20 + M23 * m.M30;
		c.M21 = M20 * m.M01 + M21 * m.M11 + M22 * m.M21 + M23 * m.M31;
		c.M22 = M20 * m.M02 + M21 * m.M12 + M22 * m.M22 + M23 * m.M32;
		c.M23 = M20 * m.M03 + M21 * m.M13 + M22 * m.M23 + M23 * m.M33;

		c.M30 = M30 * m.M00 + M31 * m.M10 + M32 * m.M20 + M33 * m.M30;
		c.M31 = M30 * m.M01 + M31 * m.M11 + M32 * m.M21 + M33 * m.M31;
		c.M32 = M30 * m.M02 + M31 * m.M12 + M32 * m.M22 + M33 * m.M32;
		c.M33 = M30 * m.M03 + M31 * m.M13 + M32 * m.M23 + M33 * m.M33;

		return c;
	}

	//Matrix4f operator+(const Matrix4f &m) const {}
	//Matrix4f operator-(const Matrix4f &m) const {}

	// Computes u = M * v, where v is treated as a *column* vector.
	Vector4f operator*(const Vector4f &v) const 
	{
		return Vector4f(
			M00 * v.X + M01 * v.Y + M02 * v.Z + M03 * v.W,
			M10 * v.X + M11 * v.Y + M12 * v.Z + M13 * v.W,
			M20 * v.X + M21 * v.Y + M22 * v.Z + M23 * v.W,
			M30 * v.X + M31 * v.Y + M32 * v.Z + M33 * v.W);
	}

	// Treats the w component of the in vectors is 1.0f if its not provided.
	Vector3f TransformPoint(const Vector3f &in) const;
	void TransformPoint(Vector3f &out, const Vector3f &in) const;
	void TransformPoint(Vector4f &out, const Vector3f &in) const;
	void TransformPoint(Vector4f &out, const Vector4f &in) const;
	void BatchTransformPoint(Vector3f *pOut, Vector3f *pIn, int num) const;
	void BatchTransformPoint(Vector4f *pOut, Vector3f *pIn, int num) const;
	void BatchTransformPoint(Vector4f *pOut, Vector4f *pIn, int num) const  { uassert(0); }

	// Treats the w component of the in vectors as 0.0f even if it is provided.
	Vector3f TransformVector(const Vector3f &in) const;
	void TransformVector(Vector3f &out, const Vector3f &in) const;
	void BatchTransformVector(Vector3f *pOut, Vector3f *pIn, int num) const;
	void BatchTransformUnitVector(Vector3f *pOut, Vector3f *pIn, int num) const;
	void BatchTransformVector(Vector4f *pOut, Vector4f *pIn, int num) const 
	{ 
		uassert(0); 
	}

	//Matrix4f operator*(float32 s) const { uassert(0); }
	//Matrix4f operator/(float32 s) const { uassert(0); }
	//Matrix4f operator+(float32 s) const { uassert(0); }
	//Matrix4f operator-(float32 s) const { uassert(0); }

	Matrix4f &operator*=(float32 s);
	Matrix4f &operator/=(float32 s);

	//Matrix4f operator-() { uassert(0); }

	void Invert() { uassert(0); }

	Matrix4f Transpose() const;
	Matrix4f Adjoint() const;
	Matrix4f Inverse() const;
	Matrix4f InverseTranspose() const;

	float32 Det3() const;				// returns the determinant of the upper left 3x3 submatrix
	float32 Det4() const;				// computes 4x4 determinant
	float32 Determinant() const;		// same as Det4()

	inline bool IsNaN() const
	{
		for (int i = 0; i < 16; i++)
			if (_isnan(M[i]))
				return true;

		return false;
	}

	inline bool IsFinite() const
	{
		for (int i = 0; i < 16; i++)
			if (!_finite(M[i]))
				return false;

		return true;
	}

	//static Matrix4f IdentityMatrix;
};

extern FuSCRIPT_API const Matrix4f IdentityMatrix4f;



//inline float32 Det2(float32 a, float32 b, float32 c, float32 d) 
//{
//	return a * d - b * c;
//}

// Returns the determinant of the 3x3 matrix with rows a, b, and c.
inline float32 Det3
    (float32 a0, float32 a1, float32 a2, 
     float32 b0, float32 b1, float32 b2, 
     float32 c0, float32 c1, float32 c2)
{
	return a0 * Det2(b1, b2, c1, c2) - b0 * Det2(a1, a2, c1, c2) + c0 * Det2(a1, a2, b1, b2);
}

inline Matrix4f Matrix4f::Rotate(float32 angleX, float32 angleY, float32 angleZ, int order)
{
	switch (order)
	{
	case RO_XYZ :
		RotZ(angleZ);
		RotY(angleY);
		RotX(angleX);
		break;
	case RO_XZY :
		RotY(angleY);
		RotZ(angleZ);
		RotX(angleX);
		break;
	case RO_YXZ :
		RotZ(angleZ);
		RotX(angleX);
		RotY(angleY);
		break;
	case RO_YZX :
		RotX(angleX);
		RotZ(angleZ);
		RotY(angleY);
		break;
	case RO_ZXY :
		RotY(angleY);
		RotX(angleX);
		RotZ(angleZ);
		break;
	case RO_ZYX :
		RotX(angleX);
		RotY(angleY);
		RotZ(angleZ);
		break;
	}

	return *this;
}

inline float32 Matrix4f::Det4() const
{
	return Determinant();		// TODO - if we every clean up matrix4f remove Determinant() method
}

inline float32 Matrix4f::Det3() const
{
	return ::Det3(M00, M01, M02, M10, M11, M12, M20, M21, M22);
}

inline float32 Matrix4f::Determinant() const
{
	return 
		  M00 * ::Det3(M11, M12, M13, M21, M22, M23, M31, M32, M33)
      - M01 * ::Det3(M10, M12, M13, M20, M22, M23, M30, M32, M33)
      + M02 * ::Det3(M10, M11, M13, M20, M21, M23, M30, M31, M33)
      - M03 * ::Det3(M10, M11, M12, M20, M21, M22, M30, M31, M32);
}

inline Matrix4f Matrix4f::Transpose() const
{
	Matrix4f m;
	
	m.M00 = M00;	m.M01 = M10;	m.M02 = M20;	m.M03 = M30;
	m.M10 = M01;	m.M11 = M11;	m.M12 = M21;	m.M13 = M31;
	m.M20 = M02;	m.M21 = M12;	m.M22 = M22;	m.M23 = M32;
	m.M30 = M03;	m.M31 = M13;	m.M32 = M23;	m.M33 = M33;

	return m;
}


inline Matrix4f Matrix4f::Adjoint() const
{
	Matrix4f m;

	m.M00 = +::Det3(M11, M12, M13, M21, M22, M23, M31, M32, M33);
	m.M01 = -::Det3(M01, M02, M03, M21, M22, M23, M31, M32, M33);
	m.M02 = +::Det3(M01, M02, M03, M11, M12, M13, M31, M32, M33);
	m.M03 = -::Det3(M01, M02, M03, M11, M12, M13, M21, M22, M23);

	m.M10 = -::Det3(M10, M12, M13, M20, M22, M23, M30, M32, M33);
	m.M11 = +::Det3(M00, M02, M03, M20, M22, M23, M30, M32, M33);
	m.M12 = -::Det3(M00, M02, M03, M10, M12, M13, M30, M32, M33);
	m.M13 = +::Det3(M00, M02, M03, M10, M12, M13, M20, M22, M23);

	m.M20 = +::Det3(M10, M11, M13, M20, M21, M23, M30, M31, M33);
	m.M21 = -::Det3(M00, M01, M03, M20, M21, M23, M30, M31, M33);
	m.M22 = +::Det3(M00, M01, M03, M10, M11, M13, M30, M31, M33);
	m.M23 = -::Det3(M00, M01, M03, M10, M11, M13, M20, M21, M23);

	m.M30 = -::Det3(M10, M11, M12, M20, M21, M22, M30, M31, M32);
	m.M31 = +::Det3(M00, M01, M02, M20, M21, M22, M30, M31, M32);
	m.M32 = -::Det3(M00, M01, M02, M10, M11, M12, M30, M31, M32);
	m.M33 = +::Det3(M00, M01, M02, M10, M11, M12, M20, M21, M22);

	return m;
}

inline Matrix4f Matrix4f::Inverse() const
{
	Matrix4f m = Adjoint();
	float32 det = M00 * m.M00 + M10 * m.M01 + M20 * m.M02 + M30 * m.M03;
	return m /= det;
}

inline Matrix4f Matrix4f::InverseTranspose() const
{
	Matrix4f m = Inverse();
	return m.Transpose();
}

inline Matrix4f &Matrix4f::operator*=(float32 s)
{
	M00 *= s;	M01 *= s;	M02 *= s;	M03 *= s;
	M10 *= s;	M11 *= s;	M12 *= s;	M13 *= s;
	M20 *= s;	M21 *= s;	M22 *= s;	M23 *= s;
	M30 *= s;	M31 *= s;	M32 *= s;	M33 *= s;

	return *this;
}

inline Matrix4f &Matrix4f::operator/=(float32 s)
{
	*this *= 1.0f / s;
	return *this;
}

inline Vector3f Matrix4f::TransformPoint(const Vector3f &in) const
{
	return Vector3f(M00 * in.X + M01 * in.Y + M02 * in.Z + M03, M10 * in.X + M11 * in.Y + M12 * in.Z + M13, M20 * in.X + M21 * in.Y + M22 * in.Z + M23);
}

inline void Matrix4f::TransformPoint(Vector3f &out, const Vector3f &in) const
{
	uassert(&in != &out);
	out.X = M00 * in.X + M01 * in.Y + M02 * in.Z + M03;
	out.Y = M10 * in.X + M11 * in.Y + M12 * in.Z + M13;
	out.Z = M20 * in.X + M21 * in.Y + M22 * in.Z + M23;
}

inline void Matrix4f::TransformPoint(Vector4f &out, const Vector3f &in) const
{
	uassert((Vector4f *) &in != &out);
	out.X = M00 * in.X + M01 * in.Y + M02 * in.Z + M03;
	out.Y = M10 * in.X + M11 * in.Y + M12 * in.Z + M13;
	out.Z = M20 * in.X + M21 * in.Y + M22 * in.Z + M23;
	out.W = M30 * in.X + M31 * in.Y + M32 * in.Z + M33;
}

inline void Matrix4f::TransformPoint(Vector4f &out, const Vector4f &in) const
{
	uassert((Vector4f *) &in != &out);
	out.X = M00 * in.X + M01 * in.Y + M02 * in.Z + M03 * in.W;
	out.Y = M10 * in.X + M11 * in.Y + M12 * in.Z + M13 * in.W;
	out.Z = M20 * in.X + M21 * in.Y + M22 * in.Z + M23 * in.W;
	out.W = M30 * in.X + M31 * in.Y + M32 * in.Z + M33 * in.W;
}

inline void Matrix4f::BatchTransformPoint(Vector3f *pOut, Vector3f *pIn, int num) const 
{ 
	uassert(pIn != pOut);
	for (int i = 0; i < num; i++)
		TransformPoint(pOut[i], pIn[i]);					// TODO - just do this for now, later on we can write an optimized SSE version
}

inline void Matrix4f::BatchTransformPoint(Vector4f *pOut, Vector3f *pIn, int num) const
{
	for (int i = 0; i < num; i++)
		TransformPoint(pOut[i], pIn[i]);					// TODO - just do this for now, later on we can write an optimized SSE version
}

inline Vector3f Matrix4f::TransformVector(const Vector3f &in) const
{
	return Vector3f(M00 * in.X + M01 * in.Y + M02 * in.Z, M10 * in.X + M11 * in.Y + M12 * in.Z, M20 * in.X + M21 * in.Y + M22 * in.Z);
}

inline void Matrix4f::TransformVector(Vector3f &out, const Vector3f &in) const
{
	out.X = M00 * in.X + M01 * in.Y + M02 * in.Z;
	out.Y = M10 * in.X + M11 * in.Y + M12 * in.Z;
	out.Z = M20 * in.X + M21 * in.Y + M22 * in.Z;
}

inline void Matrix4f::BatchTransformVector(Vector3f *pOut, Vector3f *pIn, int num) const
{
	uassert(pIn != pOut);
	for (int i = 0; i < num; i++)
		TransformVector(pOut[i], pIn[i]);					// TODO - just do this for now, later on we can write an optimized SSE version
}

inline void Matrix4f::BatchTransformUnitVector(Vector3f *pOut, Vector3f *pIn, int num) const
{
	Vector3f temp;
	uassert(pIn != pOut);
	for (int i = 0; i < num; i++)
	{
		TransformVector(pOut[i], pIn[i]);					// TODO - just do this for now, later on we can write an optimized SSE version
		pOut[i].Normalize();
	}
}

inline Matrix4f Matrix4f::Move(Vector3f &v)
{
	return Move(v.X, v.Y, v.Z);
}

inline Matrix4f Matrix4f::Move(float32 x, float32 y, float32 z)
{
	float32 col[3];
	
	col[0] = x * M00 + y * M01 + z * M02 + M03;
	col[1] = x * M10 + y * M11 + z * M12 + M13;
	col[2] = x * M20 + y * M21 + z * M22 + M23;

	M33 = x * M30 + y * M31 + z * M32 + M33;
	M03 = col[0];
	M13 = col[1];
	M23 = col[2];

	return *this;
}

inline Matrix4f &Matrix4f::RotX(float32 degrees) 
{
	float32 radians = DegToRad(degrees);
	float32 c = cosf(radians);
	float32 s = sinf(radians);		

	float32 tmp = M01;
	M01 =  c * M01 + s * M02;
	M02 = -s * tmp + c * M02;

	tmp = M11;
	M11 =  c * M11 + s * M12;
	M12 = -s * tmp + c * M12;

	tmp = M21;
	M21 =  c * M21 + s * M22;
	M22 = -s * tmp + c * M22;

	tmp = M31;
	M31 =  c * M31 + s * M32;
	M32 = -s * tmp + c * M32;

	return *this;
}

inline Matrix4f &Matrix4f::RotY(float32 degrees) 
{
	float32 radians = DegToRad(degrees);
	float32 c = cosf(radians);
	float32 s = sinf(radians);

	float32 tmp = M00;
	M00 = c * M00 - s * M02;
	M02 = s * tmp + c * M02;

	tmp = M10;
	M10 = c * M10 - s * M12;
	M12 = s * tmp + c * M12;

	tmp = M20;
	M20 = c * M20 - s * M22;
	M22 = s * tmp + c * M22;

	tmp = M30;
	M30 = c * M30 - s * M32;
	M32 = s * tmp + c * M32;
	
	return *this;
}

inline Matrix4f &Matrix4f::RotZ(float32 degrees) 
{
	float32 radians = DegToRad(degrees);
	float32 c = cosf(radians);
	float32 s = sinf(radians);

	float32 tmp = M00;
	M00 =  c * M00 + s * M01;
	M01 = -s * tmp + c * M01;

	tmp = M10;
	M10 =  c * M10 + s * M11;
	M11 = -s * tmp + c * M11;

	tmp = M20;
	M20 =  c * M20 + s * M21;
	M21 = -s * tmp + c * M21;

	tmp = M30;
	M30 =  c * M30 + s * M31;
	M31 = -s * tmp + c * M31;

	return *this;
}

inline Matrix4f &Matrix4f::RotAxis(const Vector3f &axis, float32 rads)
{
	ASSERT(fabs(axis.X * axis.X + axis.Y * axis.Y + axis.Z * axis.Z - 1.0) < 0.00001);

	float32 cosA = cosf(rads);
	float32 sinA = sinf(rads);
	float32 oCosA = 1.0f - cosA;

	float32 rx = axis.X;
	float32 ry = axis.Y;
	float32 rz = axis.Z;

	Matrix4f rot(oCosA * rx * rx + cosA,		oCosA * rx * ry - rz * sinA,	oCosA * rx * rz + ry * sinA,	0,
					 oCosA * rx * ry + rz * sinA,	oCosA * ry * ry + cosA,		   oCosA * ry * rz - rx * sinA,	0,
					 oCosA * rx * rz - ry * sinA,	oCosA * ry * rz + rx * sinA,	oCosA * rz * rz + cosA,		   0,
					 0,								   0,								      0,								      1); 

	*this = *this * rot;

	return *this;
}

inline bool Matrix4f::IsIdentity(float32 epsilon)
{
	return Equals(IdentityMatrix4f, epsilon);
}


//Matrix4f Transpose(const Matrix4f &m) {}
//Matrix4f Adjoint(const Matrix4f &m) {}
//Matrix4f Inverse(const Matrix4f &m) {}
//float32 Determinant(const Matrix4f &m) {}


/*
class Matrix4fStaticsInitializer
{
public:
	Matrix4fStaticsInitializer()
	{
		Matrix4f::IdentityMatrix.Identity();
	}
};
*/

#endif