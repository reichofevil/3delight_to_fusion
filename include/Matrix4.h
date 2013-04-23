// Matrix math header
// Supports 4x4 matrix math & 1x4 Vectors
#ifndef _MATRIX4_H_
#define _MATRIX4_H_



#ifndef _VECTOR_MATH_H_INCLUDED_
#error Include VectorMath.h
#endif

#if (defined(_M_IX86) || defined(_M_AMD64)) && (__INTEL_COMPILER >= 500 || _MSC_FULL_VER >= 12008804)
#include <emmintrin.h>
#elif (defined(_M_IX86) || defined(_M_AMD64)) && (__INTEL_COMPILER >= 400 || _MSC_FULL_VER >= 12008804)
#include <xmmintrin.h>
#elif (defined(_M_IX86) || (defined(_M_AMD64) && defined(__INTEL_COMPILER))) && (__INTEL_COMPILER >= 400 || _MSC_FULL_VER >= 12008804)
#include <mmintrin.h>
#endif


const int ReverseRotOrders[] = { RO_ZYX, RO_YZX, RO_ZXY, RO_YXZ, RO_XZY, RO_XYZ };

extern FuSCRIPT_API const FuID ROID_XYZ;
extern FuSCRIPT_API const FuID ROID_XZY;
extern FuSCRIPT_API const FuID ROID_YXZ;
extern FuSCRIPT_API const FuID ROID_YZX;
extern FuSCRIPT_API const FuID ROID_ZXY;
extern FuSCRIPT_API const FuID ROID_ZYX;

extern FuSCRIPT_API const FuID ROID_UVW;
extern FuSCRIPT_API const FuID ROID_UWV;
extern FuSCRIPT_API const FuID ROID_VUW;
extern FuSCRIPT_API const FuID ROID_VWU;
extern FuSCRIPT_API const FuID ROID_WUV;
extern FuSCRIPT_API const FuID ROID_WVU;

enum unmatrix_indices
{
	U_SCALEX = 0,
	U_SCALEY,
	U_SCALEZ,
	U_SHEARXY,
	U_SHEARXZ,
	U_SHEARYZ,
	U_ROTATEX,
	U_ROTATEY,
	U_ROTATEZ,
	U_TRANSX,
	U_TRANSY,
	U_TRANSZ,
	U_PERSPX,
	U_PERSPY,
	U_PERSPZ,
	U_PERSPW,

	U_LAST
};

class Vector4;

#if (defined(_M_IX86) || defined(_M_AMD64)) && (__INTEL_COMPILER >= 400 || _MSC_FULL_VER >= 12008804)
__declspec(align(16)) class Matrix4			// Should be align(32) but the Intel compiler 7.1 seems to only do 16, causing it to produce different alignment to Microsoft's compiler
#else
class Matrix4
#endif
{
public:
	union
	{
		struct
		{
			float64 n11, n12, n13, n14;
			float64 n21, n22, n23, n24;
			float64 n31, n32, n33, n34;
			float64 n41, n42, n43, n44;
		};

		float64 v[4][4];

		float64 m[16];
	};

	Matrix4();
	Matrix4(const Matrix4f &m);
	Matrix4(
		float64 t11, float64 t12, float64 t13, float64 t14,
		float64 t21, float64 t22, float64 t23, float64 t24,
		float64 t31, float64 t32, float64 t33, float64 t34,
		float64 t41, float64 t42, float64 t43, float64 t44);

	void Identity();

	Matrix4 RotX(float64 angle);
	Matrix4 RotY(float64 angle);
	Matrix4 RotZ(float64 angle);
	Matrix4 RotAxis(float64 axisX, float64 axisY, float64 axisZ, float64 radians);
	Matrix4 Rotate(float64 angleX, float64 angleY, float64 angleZ, int order = RO_XYZ);
	Matrix4 Rotate(float64 angleX, float64 angleY, float64 angleZ, const FuID &order);
	Matrix4 Move(float64 x, float64 y, float64 z);
	Matrix4 Scale(float64 x, float64 y, float64 z);
	Matrix4 Shear(float64 x, float64 y, float64 z);
	Matrix4 Project(float64 Perspective); // = 1/focallength
	Matrix4 Perspective(float64 fovy, float64 aspect, float64 zNear, float64 zFar);
	Matrix4 ProjectWindow(float64 width, float64 height, float64 znear, float64 zfar);
	Matrix4 Ortho(float64 width, float64 height, float64 depth);
	Matrix4 Ortho(float64 xmin, float64 xmax, float64 ymin, float64 ymax, float64 zmin, float64 zmax);
	Matrix4 LookAt(const Vector4 &eye, const Vector4 &at, const Vector4 &up);

	// Returns the transpose of the inverse of this matrix.  Used for transforming normals & vectors.
	Matrix4 InverseTranspose() const;

	// Transforms the normal by this matrix, treating it as a unit vector.  ie. assuming its w component is 0.
	// After the transform the vector is renormalized.
	Vector3f TransformNormal(const Vector3f &normal) const;	

	// Given an array of vertices, transform each element of that array by this matrix.  The vertices are transformed
	// as points, ie. assuming the w coordinate is 1.  Using the batched version of a transform can be significantly faster
	// if you have many vertices to transform.  
	void BatchVertexTransform(Vector3f *vertices, int NumVertices) const;

	// Given an array of unit vectors, transform each element of that array by this matrix and then normalize the
	// result.  The vectors are transformed as true vectors (not points), ie. assuming the w coordinate is 0.  
	// Using the batched version of a transform can be significantly faster if you have many vectors to transform.
	void BatchNormalTransform(Vector3f *normals, int numNormals) const;

	Matrix4 operator *(const Matrix4&) const;
	Matrix4 operator *=(const Matrix4&);

	bool MapQuad(float64 tlX, float64 tlY, float64 trX, float64 trY, float64 blX, float64 blY, float64 brX, float64 brY);
	void Adjoint();

	float64 Determinant() const;
	Matrix4 Inverse() const;
	Matrix4 Transpose() const;

	bool Unmatrix(float64 tran[16]) const;								// get all component transformations
	void GetTranslation(float64& tx, float64& ty, float64& tz) const;	// get simple translation
	void GetRotation(float64& rx, float64& ry, float64& rz, int order = RO_XYZ) const;		// get simple rotation
	void GetRotation(float64& rx, float64& ry, float64& rz, const FuID &order) const;		// get simple rotation
	void GetScale(float64& rx, float64& ry, float64& rz) const;			// get simple scale

	void Dump(char *msg = "Mat");
};

#define Color4d Vector4

#if (defined(_M_IX86) || defined(_M_AMD64)) && (__INTEL_COMPILER >= 400 || _MSC_FULL_VER >= 12008804)
__declspec(align(16)) class Vector4			// Should be align(32) but the Intel compiler 7.1 seems to only do 16, causing it to produce different alignment to Microsoft's compiler
#else
class Vector4
#endif
{
public:
	union
	{
		struct 
		{
			float64 x, y, z, w;
		};

		struct
		{
			float64 R;
			float64 G;
			float64 B;
			float64 A;
		};

		float64 V[4];
	};

	Vector4();
	Vector4(float64 tx);		// smears tx out into all 4 components
	Vector4(float64 tx, float64 ty, float64 tz = 0.0, float64 tw = 1.0);
	Vector4(const Vector3f &v);																		// sets w = 1.0f
	Vector4(const Vector4f &v);
	Vector4 Set(float64 tx, float64 ty, float64 tz = 0.0, float64 tw = 1.0);

	bool operator != (const Vector4 &v) const;
	Vector4 operator - () const;
	Vector4 operator + (const Vector4 &v) const;
	Vector4 operator += (const Vector4 &v);
	Vector4 operator - (const Vector4 &v) const;
	Vector4 operator -= (const Vector4 &v);
	float64  operator * (const Vector4 &v) const;
	Vector4 operator * (const float64 f) const;
	Vector4 operator / (const float64 f) const;
	friend Vector4 operator * (float64 f, const Vector4 &v);
	Vector4 operator *= (float64 f);
	Vector4 operator /= (float64 f);
	float64 Length() const;
	float64 Distance(const Vector4 &v) const;
	Vector4 Normalize();
	Vector4 Scale(float64 tx,  float64 ty, float64 tz, float64 tw);

	bool IsNaN() const;
	bool IsFinite() const;
	bool IsBounded(float64 s) const;

	Vector4 operator *(const Matrix4&) const;
	Vector4& operator *=(const Matrix4& b);

	Vector4 &operator =(const Vector3f &pt);

	void Dump(char *msg = "Vec");

	// Mirror shader operations
	void Mul(const Vector4 &v);					// per component multiplication
	void Div(const Vector4 &v);					// per component division
	Vector4 Dot3(const Vector4 &v) const;		// 3D dot product
	Vector4 Dot4(const Vector4 &v) const;		// 4D dot product

	friend Vector4 Mul(const Vector4 &v1, const Vector4 &v2);
	friend Vector4 Div(const Vector4 &v1, const Vector4 &v2);
	friend float64 Dot3(const Vector4 &v1, const Vector4 &v2);
	friend float64 Dot4(const Vector4 &v1, const Vector4 &v2);
	friend Vector4 Cross(const Vector4 &v1, const Vector4 &v2);
	friend Vector4 Lerp(float64 t, const Vector4 &v0, const Vector4 &v1);				// (1 - t) * v0 + t * v1
	friend Vector4 Lerp(const Vector4 &t, const Vector4 &v0, const Vector4 &v1);		// (1 - t) * v0 + t * v1 (per component lerp)
};





inline int ReverseRotOrder(int order)
{
	return ReverseRotOrders[order];
}

inline FuID ReverseRotOrder(const FuID &order)
{
	if	     (order == ROID_XYZ)	return ROID_ZYX;
	else if (order == ROID_XZY)	return ROID_YZX;
	else if (order == ROID_YXZ)	return ROID_ZXY;
	else if (order == ROID_YZX)	return ROID_XZY;
	else if (order == ROID_ZXY)	return ROID_YXZ;
	else /*if (order == ROID_ZYX)*/	return ROID_XYZ;
}

inline RotationOrders ConvertRotIDToRotOrder(const FuID &order)
{	
	if	     (order == ROID_XYZ)	return RO_XYZ;
	else if (order == ROID_XZY)	return RO_XZY;
	else if (order == ROID_YXZ)	return RO_YXZ;
	else if (order == ROID_YZX)	return RO_YZX;
	else if (order == ROID_ZXY)	return RO_ZXY;
	else /*if (order == ROID_ZYX)*/	return RO_ZYX;
}



// Defaults to the Identity Matrix
inline Matrix4::Matrix4()
{
	Identity();
}

inline void Matrix4::Identity()
{
	n11 = 1.0; n12 = 0.0; n13 = 0.0; n14 = 0.0;
	n21 = 0.0; n22 = 1.0; n23 = 0.0; n24 = 0.0;
	n31 = 0.0; n32 = 0.0; n33 = 1.0; n34 = 0.0;
	n41 = 0.0; n42 = 0.0; n43 = 0.0; n44 = 1.0;
}


inline Matrix4::Matrix4( float64 t11, float64 t12, float64 t13, float64 t14,
		float64 t21, float64 t22, float64 t23, float64 t24,
		float64 t31, float64 t32, float64 t33, float64 t34,
		float64 t41, float64 t42, float64 t43, float64 t44 )
{
	n11 = t11; n12 = t12; n13 = t13; n14 = t14;
	n21 = t21; n22 = t22; n23 = t23; n24 = t24;
	n31 = t31; n32 = t32; n33 = t33; n34 = t34;
	n41 = t41; n42 = t42; n43 = t43; n44 = t44;
}

inline Matrix4 Matrix4::InverseTranspose() const
{
	return this->Inverse().Transpose();							// TODO - we can do this faster by expanding these
}

inline Matrix4 Matrix4::RotX(float64 angle)
{
	if (angle)
	{
		float64 radangle = DegToRad(angle);
		float64 c = cos(radangle), s = sin(radangle);

		Matrix4 rotx(
			n11, n12*c - n13*s, n13*c + n12*s, n14,
			n21, n22*c - n23*s, n23*c + n22*s, n24,
			n31, n32*c - n33*s, n33*c + n32*s, n34,
			n41, n42*c - n43*s, n43*c + n42*s, n44);

		*this = rotx;

/*	
		Matrix4 rotx(
			1.0, 0.0, 0.0, 0.0,
			0.0, c,   s,   0.0,
			0.0, -s,  c,   0.0,
			0.0, 0.0, 0.0, 1.0 );

		(*this) *= rotx;
*/
	}

	return (*this);
}

inline Matrix4 Matrix4::RotY(float64 angle)
{
	if (angle)
	{
		float64 radangle = DegToRad(angle);
		float64 c = cos(radangle), s = sin(radangle);

		Matrix4 roty(
			n11*c + n13*s, n12, n13*c - n11*s, n14,
			n21*c + n23*s, n22, n23*c - n21*s, n24,
			n31*c + n33*s, n32, n33*c - n31*s, n34,
			n41*c + n43*s, n42, n43*c - n41*s, n44);

		*this = roty;


/*		Matrix4 roty(
			c,   0.0, -s,  0.0,
			0.0, 1.0, 0.0, 0.0,
			s,   0.0, c,   0.0,
			0.0, 0.0, 0.0, 1.0 );

		(*this) *= roty;
*/	}

	return (*this);
}

inline Matrix4 Matrix4::RotZ(float64 angle)
{
	if (angle)
	{
		float64 radangle = DegToRad(angle);
		float64 c = cos(radangle), s = sin(radangle);

		Matrix4 rotz(
			n11*c - n12*s, n12*c + n11*s, n13, n14, 
			n21*c - n22*s, n22*c + n21*s, n23, n24, 
			n31*c - n32*s, n32*c + n31*s, n33, n34, 
			n41*c - n42*s, n42*c + n41*s, n43, n44);

		*this = rotz;

/*		Matrix4 rotz(
			c,   s,   0.0, 0.0,
			-s,  c,   0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0 );

		(*this) *= rotz;
*/	}

	return (*this);
}

// Multiplies this matrix on the right by a rotation matrix that represents an arbitrary
// rotation about the given axis = (axisX, axisY, axisZ)
inline Matrix4 Matrix4::RotAxis(float64 axisX, float64 axisY, float64 axisZ, float64 radians)
{
	ASSERT(fabs(axisX*axisX + axisY*axisY + axisZ*axisZ - 1.0) < 0.00001);

	float64 cosA = cos(radians);
	float64 sinA = sin(radians);
	float64 oCosA = 1.0 - cosA;

	float64 rx = axisX;
	float64 ry = axisY;
	float64 rz = axisZ;

	Matrix4 rot(oCosA*rx*rx + cosA,		oCosA*rx*ry + rz*sinA,	oCosA*rx*rz - ry*sinA,	0,
					oCosA*rx*ry - rz*sinA,	oCosA*ry*ry + cosA,		oCosA*ry*rz + rx*sinA,	0,
					oCosA*rx*rz + ry*sinA,	oCosA*ry*rz - rx*sinA,	oCosA*rz*rz + cosA,		0,
					0,								0,								0,								1); 

	*this *= rot;

	return *this;
}

inline Matrix4 Matrix4::Rotate(float64 angleX, float64 angleY, float64 angleZ, int order)
{
	switch (order)
	{
	case RO_XYZ :
		RotX(angleX);
		RotY(angleY);
		RotZ(angleZ);
		break;
	case RO_XZY :
		RotX(angleX);
		RotZ(angleZ);
		RotY(angleY);
		break;
	case RO_YXZ :
		RotY(angleY);
		RotX(angleX);
		RotZ(angleZ);
		break;
	case RO_YZX :
		RotY(angleY);
		RotZ(angleZ);
		RotX(angleX);
		break;
	case RO_ZXY :
		RotZ(angleZ);
		RotX(angleX);
		RotY(angleY);
		break;
	case RO_ZYX :
		RotZ(angleZ);
		RotY(angleY);
		RotX(angleX);
		break;
	}

	return (*this);
}

inline Matrix4 Matrix4::Rotate(float64 angleX, float64 angleY, float64 angleZ, const FuID &order)
{
	if (order == ROID_XYZ)
	{
		RotX(angleX);
		RotY(angleY);
		RotZ(angleZ);
	}
	else if (order == ROID_XZY)
	{
		RotX(angleX);
		RotZ(angleZ);
		RotY(angleY);
	}
	else if (order == ROID_YXZ)
	{
		RotY(angleY);
		RotX(angleX);
		RotZ(angleZ);
	}
	else if (order == ROID_YZX)
	{
		RotY(angleY);
		RotZ(angleZ);
		RotX(angleX);
	}
	else if (order == ROID_ZXY)
	{
		RotZ(angleZ);
		RotX(angleX);
		RotY(angleY);
	}
	else if (order == ROID_ZYX)
	{
		RotZ(angleZ);
		RotY(angleY);
		RotX(angleX);
	}

	return (*this);
}

inline Matrix4 Matrix4::Move(float64 x, float64 y, float64 z)
{
	Matrix4 move( 
		n11 + n14*x, n12 + n14*y, n13 + n14*z, n14,
		n21 + n24*x, n22 + n24*y, n23 + n24*z, n24,
		n31 + n34*x, n32 + n34*y, n33 + n34*z, n34,
		n41 + n44*x, n42 + n44*y, n43 + n44*z, n44);

	*this = move;

/*	Matrix4 move( 
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		x,   y,   z,   1.0);

	(*this) *= move;
*/
	return (*this);
}

inline Matrix4 Matrix4::Scale(float64 x, float64 y, float64 z)
{
/*	Matrix4 scale(
		x,   0.0, 0.0, 0.0,
		0.0, y,   0.0, 0.0,
		0.0, 0.0, z,   0.0,
		0.0, 0.0, 0.0, 1.0 );

	(*this) *= scale;
*/

	n11 *= x;	n12 *= y;	n13 *= z;
	n21 *= x;	n22 *= y;	n23 *= z;
	n31 *= x;	n32 *= y;	n33 *= z;
	n41 *= x;	n42 *= y;	n43 *= z;

	return (*this);
}


inline Matrix4 Matrix4::Shear(float64 x, float64 y, float64 z)
{

	Matrix4 shear(
		n11 - n12*x, n12 - n11*y, n13, n14,
		n21 - n22*x, n22 - n21*y, n23, n24,
		n31 - n32*x, n32 - n31*y, n33, n34,
		n41 - n42*x, n42 - n41*y, n43, n44);

	*this = shear;

/*	Matrix4 shear(
		1.0, -y,  0.0, 0.0, 
		-x,  1.0, 0.0, 0.0, 
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0 );

	(*this) *= shear;
*/
	return (*this);
}


inline Matrix4 Matrix4::Project(float64 perspective)
{
/*	Matrix4 proj(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, perspective,
		0.0, 0.0, 0.0, 1.0);
	(*this) *= proj;
*/
	n14 += n13 * perspective;
	n24 += n23 * perspective;
	n34 += n33 * perspective;
	n44 += n43 * perspective;

	return (*this);
}

// Here znear and zfar are the distances to the near & far clipping planes, not their actual z values.
inline Matrix4 Matrix4::Perspective(float64 fovy, float64 aspect, float64 znear, float64 zfar)
{
	Matrix4 p;
	float64 f = 1.0 / tan(DegToRad(fovy) * 0.5);

	p.n11 = f / aspect;
	p.n22 = f;
	p.n33 = (zfar + znear) / (znear - zfar);
	p.n43 = (2 * zfar * znear) / (znear - zfar);
	p.n34 = -1.0;
	p.n44 = 0.0;
	return (*this *= p);                  
}

inline Matrix4 Matrix4::ProjectWindow(float64 width, float64 height, float64 znear, float64 zfar)
{
	Matrix4 p;

	p.n11 = 2 * znear / width;
	p.n22 = 2 * znear / height;
	p.n33 = -(zfar + znear) / (zfar - znear);
	p.n43 = (2 * znear * zfar) / (zfar - znear);
	p.n34 = -1.0;

	return (*this *= p);
}

inline Matrix4 Matrix4::Ortho(float64 width, float64 height, float64 depth)
{
	return Ortho(0.0, width, 0.0, height, 0.0, depth);
}

// Note that because the camera is oriented looking down the negative axis, we always expect
// to have zmin (z value of near clipping plane) > zmax (z value of far clipping plane).  Note:  In
// the glOrtho function, zmin/zmax are alway positive (they get internally negated) so they may actually 
// be viewed as the z *distance* to the near/far planes (ie, both positive).
inline Matrix4 Matrix4::Ortho(float64 xmin, float64 xmax, float64 ymin, float64 ymax, float64 zmin, float64 zmax)
{
	ASSERT(xmax > xmin && ymax > ymin && zmin > zmax && zmin <= 0.0 && zmax <= 0.0);
	Matrix4 p(
		2.0 / (xmax-xmin),	       0.0,						        0.0,						      0.0,
		0.0,						       2.0 / (ymax-ymin),	        0.0,						      0.0,
		0.0,						       0.0,						        2.0 / (zmax-zmin),	         0.0,
	  -(xmax+xmin) / (xmax-xmin),	-(ymin+ymax) / (ymax-ymin), -(zmax+zmin) / (zmax-zmin),  1.0);
	
	return (*this *= p);
}

inline Matrix4 Matrix4::LookAt(const Vector4 &eye, const Vector4 &at, const Vector4 &up)
{
	Vector4 zaxis = (at - eye).Normalize();
	Vector4 xaxis = Cross(zaxis, up).Normalize();
	Vector4 yaxis = Cross(xaxis, zaxis).Normalize();

	Matrix4 p(
		xaxis.x,	xaxis.y,	xaxis.z,	-Dot3(xaxis, eye),
		yaxis.x,	yaxis.y,	yaxis.z,	-Dot3(yaxis, eye),
		zaxis.x,	zaxis.y,	zaxis.z,	-Dot3(zaxis, eye),
		0.0,		0.0,		0.0,		1.0);

	return (*this *= p);
}

inline Matrix4 Matrix4::operator *(const Matrix4& b) const
{
	Matrix4 c(
		n11*b.n11 + n12*b.n21 + n13*b.n31 + n14*b.n41,	n11*b.n12 + n12*b.n22 + n13*b.n32 + n14*b.n42,	n11*b.n13 + n12*b.n23 + n13*b.n33 + n14*b.n43,	n11*b.n14 + n12*b.n24 + n13*b.n34 + n14*b.n44,
		n21*b.n11 + n22*b.n21 + n23*b.n31 + n24*b.n41,	n21*b.n12 + n22*b.n22 + n23*b.n32 + n24*b.n42,	n21*b.n13 + n22*b.n23 + n23*b.n33 + n24*b.n43,	n21*b.n14 + n22*b.n24 + n23*b.n34 + n24*b.n44,
		n31*b.n11 + n32*b.n21 + n33*b.n31 + n34*b.n41,	n31*b.n12 + n32*b.n22 + n33*b.n32 + n34*b.n42,	n31*b.n13 + n32*b.n23 + n33*b.n33 + n34*b.n43,	n31*b.n14 + n32*b.n24 + n33*b.n34 + n34*b.n44,
		n41*b.n11 + n42*b.n21 + n43*b.n31 + n44*b.n41,	n41*b.n12 + n42*b.n22 + n43*b.n32 + n44*b.n42,	n41*b.n13 + n42*b.n23 + n43*b.n33 + n44*b.n43,	n41*b.n14 + n42*b.n24 + n43*b.n34 + n44*b.n44);

	return c;
}

inline Matrix4 Matrix4::operator *=(const Matrix4& b)
{
	Matrix4 c;

	c.n11 = n11*b.n11 + n12*b.n21 + n13*b.n31 + n14*b.n41;
	c.n12 = n11*b.n12 + n12*b.n22 + n13*b.n32 + n14*b.n42;
	c.n13 = n11*b.n13 + n12*b.n23 + n13*b.n33 + n14*b.n43;
	c.n14 = n11*b.n14 + n12*b.n24 + n13*b.n34 + n14*b.n44;
	c.n21 = n21*b.n11 + n22*b.n21 + n23*b.n31 + n24*b.n41;
	c.n22 = n21*b.n12 + n22*b.n22 + n23*b.n32 + n24*b.n42;
	c.n23 = n21*b.n13 + n22*b.n23 + n23*b.n33 + n24*b.n43;
	c.n24 = n21*b.n14 + n22*b.n24 + n23*b.n34 + n24*b.n44;
	c.n31 = n31*b.n11 + n32*b.n21 + n33*b.n31 + n34*b.n41;
	c.n32 = n31*b.n12 + n32*b.n22 + n33*b.n32 + n34*b.n42;
	c.n33 = n31*b.n13 + n32*b.n23 + n33*b.n33 + n34*b.n43;
	c.n34 = n31*b.n14 + n32*b.n24 + n33*b.n34 + n34*b.n44;
	c.n41 = n41*b.n11 + n42*b.n21 + n43*b.n31 + n44*b.n41;
	c.n42 = n41*b.n12 + n42*b.n22 + n43*b.n32 + n44*b.n42;
	c.n43 = n41*b.n13 + n42*b.n23 + n43*b.n33 + n44*b.n43;
	c.n44 = n41*b.n14 + n42*b.n24 + n43*b.n34 + n44*b.n44;

	n11 = c.n11; n12 = c.n12; n13 = c.n13; n14 = c.n14;
	n21 = c.n21; n22 = c.n22; n23 = c.n23; n24 = c.n24;
	n31 = c.n31; n32 = c.n32; n33 = c.n33; n34 = c.n34;
	n41 = c.n41; n42 = c.n42; n43 = c.n43; n44 = c.n44;

	return (*this);
}

inline void Matrix4::Dump(char *msg)
{
	dprintf(("%s:\t%5lf %5lf %5lf %5lf\n"
			"\t\t\t\t%5lf %5lf %5lf %5lf\n"
			"\t\t\t\t%5lf %5lf %5lf %5lf\n"
			"\t\t\t\t%5lf %5lf %5lf %5lf\n",
			msg, n11,n12,n13,n14,n21,n22,n23,n24,n31,n32,n33,n34,n41,n42,n43,n44));
}


inline float64 Det3
	(float64 a1, float64 a2, float64 a3, 
	 float64 b1, float64 b2, float64 b3, 
	 float64 c1, float64 c2, float64 c3)
{
	return a1 * Det2( b2, b3, c2, c3 ) - b1 * Det2( a2, a3, c2, c3 ) + c1 * Det2( a2, a3, b2, b3 );
}


/*
void Matrix4::Adjoint()
{
	float64 b11 =      (n22*n33 - n23*n32);
	float64 b12 = -1 * (n21*n33 - n23*n31);
	float64 b13 =      (n21*n32 - n22*n31);
	float64 b21 = -1 * (n12*n33 - n13*n32);
	float64 b22 =      (n11*n33 - n13*n31);
	float64 b23 = -1 * (n11*n32 - n12*n31);
	float64 b31 =      (n12*n23 - n13*n22);
	float64 b32 = -1 * (n11*n23 - n21*n13);
	float64 b33 =      (n11*n22 - n12*n21);

	n11 = b11;	n12 = b21;	n13 = b31;
	n21 = b12;	n22 = b22;	n23 = b32;
	n31 = b13;	n32 = b23;	n33 = b33;
}
*/


inline void Matrix4::Adjoint()
{
	Matrix4 out;

	out.n11  =   Det3(n22, n32, n42, n23, n33, n43, n24, n34, n44);
	out.n21  = - Det3(n21, n31, n41, n23, n33, n43, n24, n34, n44);
	out.n31  =   Det3(n21, n31, n41, n22, n32, n42, n24, n34, n44);
	out.n41  = - Det3(n21, n31, n41, n22, n32, n42, n23, n33, n43);

	out.n12  = - Det3(n12, n32, n42, n13, n33, n43, n14, n34, n44);
	out.n22  =   Det3(n11, n31, n41, n13, n33, n43, n14, n34, n44);
	out.n32  = - Det3(n11, n31, n41, n12, n32, n42, n14, n34, n44);
	out.n42  =   Det3(n11, n31, n41, n12, n32, n42, n13, n33, n43);

	out.n13  =   Det3(n12, n22, n42, n13, n23, n43, n14, n24, n44);
	out.n23  = - Det3(n11, n21, n41, n13, n23, n43, n14, n24, n44);
	out.n33  =   Det3(n11, n21, n41, n12, n22, n42, n14, n24, n44);
	out.n43  = - Det3(n11, n21, n41, n12, n22, n42, n13, n23, n43);

	out.n14  = - Det3(n12, n22, n32, n13, n23, n33, n14, n24, n34);
	out.n24  =   Det3(n11, n21, n31, n13, n23, n33, n14, n24, n34);
	out.n34  = - Det3(n11, n21, n31, n12, n22, n32, n14, n24, n34);
	out.n44  =   Det3(n11, n21, n31, n12, n22, n32, n13, n23, n33);

	*this = out;
}


inline bool Matrix4::MapQuad(float64 tlX, float64 tlY, float64 trX, float64 trY, float64 blX, float64 blY, float64 brX, float64 brY)
{
	float64 px, py, dx1, dx2, dy1, dy2, det;

	dx1 = trX - brX;
	dx2 = blX - brX;
	dy1 = trY - brY;
	dy2 = blY - brY;

	det = Det2(dx1, dx2, dy1, dy2);

	if (det == 0)
		return FALSE;

	px = tlX - trX + brX - blX;
	py = tlY - trY + brY - blY;

	n13 = Det2(px, dx2, py, dy2) / det;
	n23 = Det2(dx1, px, dy1, py) / det;
	n33 = 1.0;
	n11 = trX - tlX + n13 * trX;
	n21 = blX - tlX + n23 * blX;
	n31 = tlX;
	n12 = trY - tlY + n13 * trY;
	n22 = blY - tlY + n23 * blY;
	n32 = tlY;

	return TRUE;
}

inline float64 Matrix4::Determinant() const
{
	return 
		  n11 * Det3(n22, n32, n42, n23, n33, n43, n24, n34, n44)
		- n12 * Det3(n21, n31, n41, n23, n33, n43, n24, n34, n44)
		+ n13 * Det3(n21, n31, n41, n22, n32, n42, n24, n34, n44)
		- n14 * Det3(n21, n31, n41, n22, n32, n42, n23, n33, n43);
}



inline Matrix4 Matrix4::Inverse() const
{
	float64 det = Determinant();
	Matrix4 out;

	if (det) 
	{
		out = *this;
		out.Adjoint();

		out.n11 /= det;	out.n21 /= det;	out.n31 /= det;	out.n41 /= det;
		out.n12 /= det;	out.n22 /= det;	out.n32 /= det;	out.n42 /= det;
		out.n13 /= det;	out.n23 /= det;	out.n33 /= det;	out.n43 /= det;
		out.n14 /= det;	out.n24 /= det;	out.n34 /= det;	out.n44 /= det;
	}

	return out; 
}

inline Matrix4 Matrix4::Transpose() const
{
	Matrix4 out;

	out.n11 = n11; out.n21 = n12;	out.n31 = n13;	out.n41 = n14;
	out.n12 = n21;	out.n22 = n22; out.n32 = n23;	out.n42 = n24;
	out.n13 = n31;	out.n23 = n32; out.n33 = n33; out.n43 = n34;
	out.n14 = n41;	out.n24 = n42;	out.n34 = n43; out.n44 = n44;

	return out;
}

/* Unmatrix - Decompose a non-degenerate 4x4 transformation matrix into
 * 	the sequence of transformations that produced it.
 * [Sx][Sy][Sz][Shearx/y][Sx/z][Sz/y][Rx][Ry][Rz][Tx][Ty][Tz][P(x,y,z,w)]
 *
 * The coefficient of each transformation is returned in the corresponding
 * element of the vector tran.
 *
 * Returns true upon success, false if the matrix is singular.
 */
inline bool Matrix4::Unmatrix(float64 tran[16]) const
{
	Matrix4 locmat;
	Matrix4 pmat, invpmat, tinvpmat;
	/* Vector4 type and functions need to be added to the common set. */
	Vector4 prhs, psol;
	Vector4 row[3];

	/* Normalize the matrix. */
	if (n44 == 0)
		return false;
	locmat.n11 = n11 / n44;	locmat.n21 = n21 / n44;	locmat.n31 = n31 / n44;	locmat.n41 = n41 / n44;
	locmat.n12 = n12 / n44;	locmat.n22 = n22 / n44;	locmat.n32 = n32 / n44;	locmat.n42 = n42 / n44;
	locmat.n13 = n13 / n44;	locmat.n23 = n23 / n44;	locmat.n33 = n33 / n44;	locmat.n43 = n43 / n44;
	locmat.n14 = n14 / n44;	locmat.n24 = n24 / n44;	locmat.n34 = n34 / n44;	locmat.n44 = 1.0;

	/* pmat is used to solve for perspective, but it also provides
	 * an easy way to test for singularity of the upper 3x3 component.
	 */
	pmat = locmat;
	pmat.n14 = pmat.n24 = pmat.n34 = 0;
	pmat.n44 = 1;

	if (pmat.Determinant() == 0.0)
		return false;

	/* First, isolate perspective.  This is the messiest. */
	if (locmat.n14 != 0 || locmat.n24 != 0 || locmat.n24 != 0)
	{
		/* prhs is the right hand side of the equation. */
		prhs.x = locmat.n14;
		prhs.y = locmat.n24;
		prhs.z = locmat.n34;
		prhs.w = locmat.n44;

		/* Solve the equation by inverting pmat and multiplying
		 * prhs by the inverse.  (This is the easiest way, not
		 * necessarily the best.)
		 * inverse function (and det4x4, above) from the Matrix
		 * Inversion gem in the first volume.
		 */
		invpmat = pmat.Inverse();
		tinvpmat = invpmat.Transpose();
		psol = prhs * tinvpmat;

		/* Stuff the answer away. */
		tran[U_PERSPX] = psol.x;
		tran[U_PERSPY] = psol.y;
		tran[U_PERSPZ] = psol.z;
		tran[U_PERSPW] = psol.w;
		/* Clear the perspective partition. */
		locmat.n14 = locmat.n24 = locmat.n34 = 0;
		locmat.n44 = 1;
	}
	else		/* No perspective. */
		tran[U_PERSPX] = tran[U_PERSPY] = tran[U_PERSPZ] = tran[U_PERSPW] = 0;

	/* Next take care of translation (easy). */
	tran[U_TRANSX] = locmat.n41;
	tran[U_TRANSY] = locmat.n42;
	tran[U_TRANSZ] = locmat.n43;
	locmat.n41 = locmat.n42 = locmat.n43 = 0;

	/* Now get scale and shear. */
	row[0].x = locmat.n11;	row[0].y = locmat.n12;	row[0].z = locmat.n13;
	row[1].x = locmat.n21;	row[1].y = locmat.n22;	row[1].z = locmat.n23;
	row[2].x = locmat.n31;	row[2].y = locmat.n32;	row[2].z = locmat.n33;

	/* Compute X scale factor and normalize first row. */
	tran[U_SCALEX] = row[0].Length();
	row[0].Normalize();

	/* Compute XY shear factor and make 2nd row orthogonal to 1st. */
	tran[U_SHEARXY] = Dot3(row[0], row[1]);
	row[1] = row[1] + row[0] * -tran[U_SHEARXY];

	/* Now, compute Y scale and normalize 2nd row. */
	tran[U_SCALEY] = row[1].Length();
	row[1].Normalize();
	tran[U_SHEARXY] /= tran[U_SCALEY];

	/* Compute XZ and YZ shears, orthogonalize 3rd row. */
	tran[U_SHEARXZ] = Dot3(row[0], row[2]);
	row[2] = row[2] + row[0] * -tran[U_SHEARXZ];
	tran[U_SHEARYZ] = Dot3(row[1], row[2]);
	row[2] = row[2] + row[1] * -tran[U_SHEARYZ];

	/* Next, get Z scale and normalize 3rd row. */
	tran[U_SCALEZ] = row[2].Length();
	row[2].Normalize();
	tran[U_SHEARXZ] /= tran[U_SCALEZ];
	tran[U_SHEARYZ] /= tran[U_SCALEZ];
 
	/* At this point, the matrix (in rows[]) is orthonormal.
	 * Check for a coordinate system flip.  If the determinant
	 * is -1, then negate the matrix and the scaling factors.
	 */
	if (Dot3(row[0], Cross(row[1], row[2])) < 0.0)
	{
		for (int i = 0; i < 3; i++)
		{
			tran[U_SCALEX + i] *= -1;
			row[i] *= -1;
		}
	}

	/* Now, get the rotations out, as described in the gem. */
	tran[U_ROTATEY] = asin(-row[0].z);
	if (cos(tran[U_ROTATEY]) != 0)
	{
		tran[U_ROTATEX] = atan2(row[1].z, row[2].z);
		tran[U_ROTATEZ] = atan2(row[0].y, row[0].x);
	}
	else
	{
		tran[U_ROTATEX] = atan2(row[1].x, row[1].y);
		tran[U_ROTATEZ] = 0;
	}
	/* All done! */
	return true;
}

// These work for simple rot/scale/tranlate matrices only (or maybe not at all)
// get simple translation
inline void Matrix4::GetTranslation(float64& tx, float64& ty, float64& tz) const
{
	tx = n41;
	ty = n42;
	tz = n43;
}

// get simple rotation, in XYZ order I suppose
inline void Matrix4::GetRotation(float64& rx, float64& ry, float64& rz, int order) const
{
	float64 sx, sy, sz;
	GetScale(sx, sy, sz);

	Matrix4 tm; tm.Scale(sx != 0.0 ? 1.0 / sx : 1.0, sy != 0.0 ? 1.0 / sy : 1.0, sz != 0.0 ? 1.0 / sz : 1.0);
	tm *= *this;

	switch (order)
	{
	case RO_XYZ :
		ry = asin(-tm.n13);
		if (cos(ry) != 0)
		{
			rx = atan2(tm.n23, tm.n33);
			rz = atan2(tm.n12, tm.n11);
		}
		else
		{
			rx = atan2(tm.n21, tm.n22);
			rz = 0;
		}
		break;
	case RO_XZY :
		rz = asin(tm.n12);
		if (cos(rz) != 0)
		{
			rx = atan2(-tm.n32, tm.n22);
			ry = atan2(-tm.n13, tm.n11);
		}
		else
		{
			rx = atan2(-tm.n31, tm.n33);
			ry = 0;
		}
		break;
	case RO_YXZ :
		rx = asin(tm.n23);
		if (cos(rx) != 0)
		{
			ry = atan2(-tm.n13, tm.n33);
			rz = atan2(-tm.n21, tm.n22);
		}
		else
		{
			ry = atan2(-tm.n12, tm.n11);
			rz = 0;
		}
		break;
	case RO_YZX :
		rz = asin(-tm.n21);
		if (cos(rz) != 0)
		{
			ry = atan2(tm.n31, tm.n11);
			rx = atan2(tm.n23, tm.n22);
		}
		else
		{
			ry = atan2(tm.n32, tm.n33);
			rx = 0;
		}
		break;
	case RO_ZXY :
		rx = asin(-tm.n32);
		if (cos(rx) != 0)
		{
			rz = atan2(tm.n12, tm.n22);
			ry = atan2(tm.n31, tm.n33);
		}
		else
		{
			rz = atan2(tm.n13, tm.n11);
			ry = 0;
		}
		break;
	case RO_ZYX :
		ry = asin(n31);
		if (cos(ry) != 0)
		{
			rz = atan2(-tm.n21, tm.n11);
			rx = atan2(-tm.n32, tm.n33);
		}
		else
		{
			rz = atan2(-tm.n23, tm.n22);
			rx = 0;
		}
		break;
	}
}

// get simple rotation, in XYZ order I suppose
inline void Matrix4::GetRotation(float64& rx, float64& ry, float64& rz, const FuID &order) const
{
	if (order == ROID_XYZ)
		GetRotation(rx, ry, rz, RO_XYZ);
	else if (order == ROID_XZY)
		GetRotation(rx, ry, rz, RO_XZY);
	else if (order == ROID_YXZ)
		GetRotation(rx, ry, rz, RO_YXZ);
	else if (order == ROID_YZX)
		GetRotation(rx, ry, rz, RO_YZX);
	else if (order == ROID_ZXY)
		GetRotation(rx, ry, rz, RO_ZXY);
	else if (order == ROID_ZYX)
		GetRotation(rx, ry, rz, RO_ZYX);
}

// get simple scale
inline void Matrix4::GetScale(float64& rx, float64& ry, float64& rz) const
{
	rx = sqrt(n11 * n11 + n12 * n12 + n13 * n13);
	ry = sqrt(n21 * n21 + n22 * n22 + n23 * n23);
	rz = sqrt(n31 * n31 + n32 * n32 + n33 * n33);
}





// Same sort of stuff for 1x4 Vector

inline Vector4::Vector4()
{
	x = y = z = 0.0;
	w = 1.0;
}

inline Vector4::Vector4(float64 tx)
{
	x = y = z = w = tx;
}

inline Vector4::Vector4(float64 tx, float64 ty, float64 tz, float64 tw)
{
	x = tx; y = ty; z = tz; w = tw;
}

inline Vector4 Vector4::Set (float64 tx, float64 ty, float64 tz, float64 tw)
{
	x = tx; y = ty; z = tz; w = tw;
	return *this;
}

inline bool Vector4::IsNaN() const
{
	for (int i = 0; i < 4; i++)
		if (_isnan(V[i]))
			return true;

	return false;
}

inline bool Vector4::IsFinite() const
{
	for (int i = 0; i < 4; i++)
		if (!_finite(V[i]))
			return false;

	return true;
}

inline bool Vector4::IsBounded(float64 s) const
{
	return fabs(x) < s && fabs(y) < s && fabs(z) < s && fabs(w) < s;
}

inline Vector4 Vector4::operator *(const Matrix4& m) const
{
	Vector4 c;

	c.x = x*m.n11 + y*m.n21 + z*m.n31 + w*m.n41;
	c.y = x*m.n12 + y*m.n22 + z*m.n32 + w*m.n42;
	c.z = x*m.n13 + y*m.n23 + z*m.n33 + w*m.n43;
	c.w = x*m.n14 + y*m.n24 + z*m.n34 + w*m.n44;

	return c;
}

inline Vector4& Vector4::operator *=(const Matrix4& m)
{
	*this = Vector4(
		x*m.n11 + y*m.n21 + z*m.n31 + w*m.n41,
		x*m.n12 + y*m.n22 + z*m.n32 + w*m.n42,
		x*m.n13 + y*m.n23 + z*m.n33 + w*m.n43,
		x*m.n14 + y*m.n24 + z*m.n34 + w*m.n44);

	return (*this);
}

inline bool Vector4::operator != (const Vector4 &v) const 
{
	return (x!= v.x) || (y!= v.y) || (z!= v.z) || (w!= v.w);
}

inline Vector4 Vector4::operator - () const 
{
	Vector4 r(-x, -y, -z, -w);
	return r;
}

inline Vector4 Vector4::operator * (const float64 f) const 
{
	Vector4 r(x * f, y * f, z * f, w * f);

	return r;
}

inline Vector4 Vector4::operator / (const float64 f) const 
{
	Vector4 r(x / f, y / f, z / f, w / f);

	return r;
}

inline Vector4 Vector4::operator + (const Vector4 &v) const 
{
	Vector4 r(x + v.x, y + v.y, z + v.z, w + v.w);
	return r;
}

inline Vector4 Vector4::operator += (const Vector4 &v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}


inline Vector4 Vector4::operator - (const Vector4 &v) const 
{
	Vector4 r(x - v.x, y - v.y, z - v.z, w - v.w);
	return r;
}


inline Vector4 Vector4::operator -= (const Vector4 &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}

inline float64 Vector4::operator * (const Vector4 &v) const 
{
	return x * v.x + y * v.y + z * v.z /* + w * v.w */;
}


inline Vector4 operator * (float64 f, const Vector4 &v) 
{
	Vector4 r(f * v.x, f * v.y, f * v.z, f * v.w);
	return r;
}

inline Vector4 Vector4::operator *= (float64 f)
{
	x *= f;
	y *= f;
	z *= f;
	w *= f;
	return *this;
}

inline Vector4 Vector4::operator /= (float64 f)
{
	x /= f;
	y /= f;
	z /= f;
	w /= f;
	return *this;
}


inline float64 Vector4::Length () const 
{
	return sqrt(x * x + y * y + z * z /* + w * w */);
}


inline float64 Vector4::Distance (const Vector4 &v) const 
{
	return sqrt(
		(v.x - x) * (v.x - x) + 
		(v.y - y) * (v.y - y) + 
		(v.z - z) * (v.z - z)); 
		/* + (v.w - w) * (v.w - w) */
}


inline Vector4 Vector4::Normalize() 
{
	float64 len = Length();
	if (len>0.0)
		return (1.0/len) * (*this);
	else
		return (*this);
}


inline Vector4 Vector4::Scale(float64 tx,  float64 ty, float64 tz, float64 tw)
{
	Vector4 r ( x * tx, y * ty, z * tz, w * tw );
	return r;
}


inline void Vector4::Dump(char *msg)
{
	dprintf(("%s:%5lf %5lf %5lf %5lf\n",msg,x,y,z,w));
}


inline void Vector4::Mul(const Vector4 &v) 
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;
}

inline void Vector4::Div(const Vector4 &v) 
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	w /= v.w;
}

inline Vector4 Lerp(float64 t, const Vector4 &v0, const Vector4 &v1)
{
	return (1.0 - t) * v0 + t * v1;
}

inline Vector4 Lerp(const Vector4 &t, const Vector4 &v0, const Vector4 &v1)
{
	return Mul(Vector4(1.0) - t, v0) + Mul(t, v1);
}

inline Vector4 Mul(const Vector4 &v1, const Vector4 &v2)
{
	return Vector4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}

inline Vector4 Div(const Vector4 &v1, const Vector4 &v2)
{
	return Vector4(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
}

inline float64 Dot3(const Vector4 &v1, const Vector4 &v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline float64 Dot4(const Vector4 &v1, const Vector4 &v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

inline Vector4 Cross(const Vector4 &v1, const Vector4 &v2)
{
	Vector4 out;

	out.x = v1.y * v2.z - v1.z * v2.y;
	out.y = v1.z * v2.x - v1.x * v2.z;
	out.z = v1.x * v2.y - v1.y * v2.x;

	return out;
}








#endif //_MATRIX4_H_
