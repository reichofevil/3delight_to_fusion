#ifndef _MATRIX3_H_
#define _MATRIX3_H_

#ifndef _VECTOR_MATH_H_INCLUDED_
#error Include VectorMath.h
#endif

#include "Matrix4.h"

#define ColorNormal(c) (double(c)/65535.0)
#define ColorInt(c) ((int)((c)*65535.0+0.5))


class Vector2;

class Matrix3
{
public:
	double n11, n12, n13;
	double n21, n22, n23;
	double n31, n32, n33;

	Matrix3();
	Matrix3(double t11, double t12, double t13,
		double t21, double t22, double t23,
		double t31, double t32, double t33);
	void Identity();

	operator XFORM();
	
	Matrix3 Rot(double angle);
	Matrix3 Move(double x, double y);
	Matrix3 Scale(double x, double y);
	Matrix3 Shear(double x, double y);
	Matrix3 Taper(double x, double y);

	Matrix3 operator * (const Matrix3 &b) const;
	Matrix3 operator *= (const Matrix3 &b);

	double Determinant();
	Matrix3 Adjoint();
	Matrix3 Inverse();

	int Matrix3::MapSquareQuad(Vector2 corners[4]);
	int MapQuadRect(Vector2 v0, Vector2 v1, Vector2 corners[4]);
	int MapQuadQuad(Vector2 source[4], Vector2 destination[4]);
	int MapPoly(Vector2 source[4], Vector2 destination[4]);

	void Dump(char *msg = "Mat");
};


class Vector3
{
public:
	double x,y,z;

//	Vector3();
	Vector3(double tx = 0.0, double ty = 0.0, double tz = 0.0);

	bool operator != (const Vector3 &v) const;
	Vector3 operator - () const;
	Vector3 operator + (const Vector3 &v) const;
	Vector3 operator += (const Vector3 &v);
	Vector3 operator - (const Vector3 &v) const;
	Vector3 operator -= (const Vector3 &v);
	double operator * (const Vector3 &v) const;
	friend Vector3 operator * (double f, const Vector3 &v);

	Vector3 operator * (const double f) const;
	Vector3 operator *= (const double f);
	Vector3 operator / (const double f) const;
	Vector3 operator /= (const double f);

	Vector3 operator * (const Matrix3&) const;
	Vector3 operator *= (const Matrix3&);
	friend Vector3 operator * (const Matrix3&, const Vector3&);

	Vector3 operator * (const Matrix4&) const;
	Vector3 operator *= (const Matrix4&);

	Vector3 Set(double X, double Y, double Z);

	double Length() const;
	double LengthXY() const;
	double LengthXZ() const;
	double LengthYZ() const;

	double Distance (const Vector3 &v) const;
	Vector3 Normalize();
	Vector3 Cross(const Vector3 &v) const;
	Vector3 Scale(double x, double y, double z);

	Vector3 Perspective(double PerspectiveFactor);
	Vector3 UndoPerspective(double PerspectiveFactor);

//	Vector3 EulerToVector();

	void Dump(char *msg = "Vec");
};


class Vector2 
{
public :
	double x, y;

	Vector2(double tx = 0.0, double ty = 0.0);

	bool operator != (const Vector2 &v) const;
	bool operator == (const Vector2 &v) const;
	Vector2 operator - () const;
	Vector2 operator + (const Vector2 &v) const;
	Vector2 operator += (const Vector2 &v);
	Vector2 operator - (const Vector2 &v) const;
	Vector2 operator -= (const Vector2 &v);
	double operator * (const Vector2 &v) const;

	friend Vector2 operator *(const double f, const Vector2 &v);
	friend Vector2 operator *(const Vector2 &v, const double f);
	Vector2 operator / (double f) const;

	Vector2 operator *= (double f);
	Vector2 operator /= (double f);

	double operator %(const Vector2 &v);		// Cross product

	Vector2 Set(double X, double Y);

	double Length() const;
	double Distance(const Vector2 &v) const;
	Vector2 Normalize();

	bool IsNull();

	Vector2 operator * (const Matrix3& m) const;
	Vector2 operator * (const Matrix4& m) const;
};

//#include "stdafx.h"

//#define MODULENAME "Matrix3"

//#include "Debug.h"
//#include "Matrix3.h"


#define SMALL_NUMBER2 1.e-8

inline Matrix3::Matrix3()
{
	Identity();
}

inline Matrix3::Matrix3(	double t11, double t12, double t13,
					double t21, double t22, double t23,
					double t31, double t32, double t33 )
{
	n11 = t11; n12 = t12; n13 = t13;
	n21 = t21; n22 = t22; n23 = t23;
	n31 = t31; n32 = t32; n33 = t33;
}

inline void Matrix3::Identity()
{
	n11 = 1; n12 = 0; n13 = 0; 
	n21 = 0; n22 = 1; n23 = 0;
	n31 = 0; n32 = 0; n33 = 1;
}


inline Matrix3::operator XFORM()
{
	XFORM xform;
	xform.eM11 = n11;
	xform.eM12 = n12;
	xform.eM21 = n21;
	xform.eM22 = n22;
	xform.eDx = n31;
	xform.eDy = n32;
	return xform;
}

inline Matrix3 Matrix3::Rot(double angle)
{
	double c = cos(DegToRad(angle)), s = sin(DegToRad(angle));
	Matrix3 rotx(c, s,  0,
				 -s,  c,  0,
				 0,  0,  1 );

	(*this) *= rotx;

	return (*this);
}

inline Matrix3 Matrix3::Move(double x, double y)
{
	Matrix3 move( 1,  0,  0,
				  0,  1,  0,
				  x,  y,  1 );

	(*this) *= move;

	return (*this);
}

inline Matrix3 Matrix3::Scale(double x, double y)
{
	Matrix3 scale(x,  0,  0,
				  0,  y,  0, 
				  0,  0,  1 );

	(*this) *= scale;

	return (*this);
}

inline Matrix3 Matrix3::Shear(double x, double y)
{
	Matrix3 shear(1,  -y,  0,
				  -x,  1,  0, 
				  0,  0,  1 );

	(*this) *= shear;

	return (*this);
}

inline Matrix3 Matrix3::Taper(double x, double y)
{
	Matrix3 taper(x,  0,  0,
				  0,  y,  0, 
				  0,  0,  1 );

	(*this) *= taper;

	return (*this);
}


inline Matrix3 Matrix3::operator * (const Matrix3 &b) const
{
	Matrix3 c;

	c.n11 = n11*b.n11 + n12*b.n21 + n13*b.n31;
	c.n12 = n11*b.n12 + n12*b.n22 + n13*b.n32;
	c.n13 = n11*b.n13 + n12*b.n23 + n13*b.n33;
	c.n21 = n21*b.n11 + n22*b.n21 + n23*b.n31;
	c.n22 = n21*b.n12 + n22*b.n22 + n23*b.n32;
	c.n23 = n21*b.n13 + n22*b.n23 + n23*b.n33;
	c.n31 = n31*b.n11 + n32*b.n21 + n33*b.n31;
	c.n32 = n31*b.n12 + n32*b.n22 + n33*b.n32;
	c.n33 = n31*b.n13 + n32*b.n23 + n33*b.n33;

	return c;
}

inline Matrix3 Matrix3::operator *= (const Matrix3 &b)
{
	Matrix3 c;

	c.n11 = n11*b.n11 + n12*b.n21 + n13*b.n31;
	c.n12 = n11*b.n12 + n12*b.n22 + n13*b.n32;
	c.n13 = n11*b.n13 + n12*b.n23 + n13*b.n33;
	c.n21 = n21*b.n11 + n22*b.n21 + n23*b.n31;
	c.n22 = n21*b.n12 + n22*b.n22 + n23*b.n32;
	c.n23 = n21*b.n13 + n22*b.n23 + n23*b.n33;
	c.n31 = n31*b.n11 + n32*b.n21 + n33*b.n31;
	c.n32 = n31*b.n12 + n32*b.n22 + n33*b.n32;
	c.n33 = n31*b.n13 + n32*b.n23 + n33*b.n33;

	n11 = c.n11; n12 = c.n12; n13 = c.n13;
	n21 = c.n21; n22 = c.n22; n23 = c.n23;
	n31 = c.n31; n32 = c.n32; n33 = c.n33;

	return (*this);
}

inline void Matrix3::Dump(char *msg)
{
	dprintf(("%s:\t%5lf %5lf %5lf\n"
			"\t\t\t\t%5lf %5lf %5lf\n"
			"\t\t\t\t%5lf %5lf %5lf\n"
			"\t\t\t\t%5lf %5lf %5lf\n",
			msg, n11,n12,n13,n21,n22,n23,n31,n32,n33));
}

#define Det2x2(a, b, c, d) ((a) * (d) - (b) * (c))

inline double Matrix3::Determinant()
{
	return 
		n11 * Det2x2(n22, n32, n23, n33) + 
		n12 * Det2x2(n23, n33, n21, n31) +
		n13 * Det2x2(n21, n31, n22, n32);
}

inline Matrix3 Matrix3::Adjoint()
{
	Matrix3 out;

	out.n11 = Det2x2(n22, n32, n23, n33);
	out.n21 = Det2x2(n23, n33, n21, n31);
	out.n31 = Det2x2(n21, n31, n22, n32);
  
	out.n12 = Det2x2(n13, n33, n12, n32);
	out.n22 = Det2x2(n11, n31, n13, n33);
	out.n32 = Det2x2(n12, n32, n11, n31);
  
	out.n13 = Det2x2(n12, n22, n13, n23);
	out.n23 = Det2x2(n13, n23, n11, n21);
	out.n33 = Det2x2(n11, n21, n12, n22);
  
	return out;
}

inline Matrix3 Matrix3::Inverse ()
{
	double det = Determinant();
	Matrix3 out;

	if (fabs(det) >= SMALL_NUMBER2) 
	{
		out = Adjoint();

		out.n11 /= det;	out.n21 /= det;	out.n31 /= det;
		out.n12 /= det;	out.n22 /= det;	out.n32 /= det;
		out.n13 /= det;	out.n23 /= det;	out.n33 /= det;
	}

	return out; 
}

// ***** Mapping 

#define PMAP_BAD	-1
#define PMAP_AFFINE 0
#define PMAP_PROJECTIVE 1

#define TOLERANCE 1e-13
#define ZERO(x) ((x)<TOLERANCE && (x)>-TOLERANCE)

inline int Matrix3::MapSquareQuad(Vector2 corners[4])
{
	Vector2 p = corners[0] - corners[1] + corners[2] - corners[3];

	if (ZERO(p.x) && ZERO(p.y)) 
	{
		// affine
		n11 = corners[1].x - corners[0].x;
		n21 = corners[2].x - corners[1].x;
		n31 = corners[0].x;
		n12 = corners[1].y - corners[0].y;
		n22 = corners[2].y - corners[1].y;
		n32 = corners[0].y;
		n13 = 0.0;
		n23 = 0.0;
		n33 = 1.0;

		return PMAP_AFFINE;
	}
	else
	{
		// projective
		Vector2 d1 = corners[1] - corners[2];
		Vector2 d2 = corners[3] - corners[2];

		double del = Det2(d1.x, d2.x, d1.y, d2.y);
		if (del == 0.0)
			return PMAP_BAD;

		n13 = Det2(p.x, d2.x, p.y, d2.y) / del;
		n23 = Det2(d1.x, p.x, d1.y, p.y) / del;
		n33 = 1.0;
		n11 = corners[1].x - corners[0].x + n13 * corners[1].x;
		n21 = corners[3].x - corners[0].x + n23 * corners[3].x;
		n31 = corners[0].x;
		n12 = corners[1].y - corners[0].y + n13 * corners[1].y;
		n22 = corners[3].y - corners[0].y + n23 * corners[3].y;
		n32 = corners[0].y;

		return PMAP_PROJECTIVE;
	}
}
inline int Matrix3::MapQuadRect(Vector2 v0, Vector2 v1, Vector2 corners[4])
{
	int ret;
	Vector2 d;

	d = v1-v0;
	if (d.x==0.0 || d.y==0.0)
		return PMAP_BAD;

	Matrix3 rq, qr;

	ret = rq.MapSquareQuad(corners);
	if (ret==PMAP_BAD)
		return PMAP_BAD;

	rq.n11 /= d.x;
	rq.n21 /= d.y;
	rq.n31 -= rq.n11 * v0.x + rq.n21 * v0.y;
	rq.n12 /= d.x;
	rq.n22 /= d.y;
	rq.n32 -= rq.n12 * v0.x + rq.n22 * v0.y;
	rq.n13 /= d.x;
	rq.n23 /= d.y;
	rq.n33 -= rq.n13 * v0.x + rq.n23 * v0.y;

	qr = rq.Adjoint();

	if (rq.n11 * qr.n11 +  rq.n22 * qr.n22 + rq.n33 * qr.n33 == 0.0)
		return PMAP_BAD;

	*this = qr;

	return ret;
}

inline int Matrix3::MapQuadQuad(Vector2 source[4], Vector2 destination[4])
{
	int type1, type2;

	Matrix3 MS, SM, MT;

	type1 = MS.MapSquareQuad(source);

	SM = MS.Adjoint();

	if (MS.n11 * SM.n22 + MS.n22 * SM.n33 + MS.n33 * SM.n11 == 0.0)
	{
		int error = -1; // ERROR	
	}

	type2 = MT.MapSquareQuad(destination);

	if (type1 == PMAP_BAD || type2 == PMAP_BAD)
		return PMAP_BAD;

	*this = SM * MT;

	return type1 | type2;
}

inline int Matrix3::MapPoly(Vector2 source[4], Vector2 destination[4])
{
	if (destination[0].y == destination[1].y && destination[2].y == destination[3].y && 
		destination[1].x == destination[2].x && destination[3].x == destination[0].x)
	{
		return MapQuadRect(destination[0], destination[2], source);
	}
	else if (destination[0].x == destination[1].x && destination[2].x == destination[3].x &&
		destination[1].y == destination[2].y && destination[3].y == destination[0].y)
	{
		Vector2 scr[4];

		scr[0] = source[1];
		scr[1] = source[2];
		scr[2] = source[3];
		scr[3] = source[0];

		return MapQuadRect(destination[1], destination[3], scr);
	}
	else 
		return MapQuadQuad(source, destination);
}


// ***** Vector3

inline Vector3::Vector3(double tx, double ty, double tz)
{
	x = tx; y = ty; z = tz;
}

inline Vector3 Vector3::Set (double tx, double ty, double tz)
{
	x = tx; y = ty; z = tz;
	return *this;
}


inline Vector3 Vector3::operator * (const Matrix3 &b) const
{
	Vector3 c;

	c.x = x*b.n11 + y*b.n21 + z*b.n31;
	c.y = x*b.n12 + y*b.n22 + z*b.n32;
	c.z = x*b.n13 + y*b.n23 + z*b.n33;

	return c;
}

inline Vector3 Vector3::operator *= (const Matrix3 &b)
{
	*this = Vector3(
		x*b.n11 + y*b.n21 + z*b.n31,
		x*b.n12 + y*b.n22 + z*b.n32,
		x*b.n13 + y*b.n23 + z*b.n33);

	return *this;
}

inline Vector3 operator * (const Matrix3 &m, const Vector3 &v)
{
	Vector3 c;

	c.x = v.x*m.n11 + v.y*m.n12 + v.z*m.n13;
	c.y = v.x*m.n21 + v.y*m.n22 + v.z*m.n23;
	c.z = v.x*m.n31 + v.y*m.n32 + v.z*m.n33;

	return c;
}

inline void Vector3::Dump(char *msg)
{
	dprintf(("%s:%5lf %5lf %5lf %5lf\n",msg,x,y,z));
}

// Operators

inline bool Vector3::operator != (const Vector3 &v) const
{
	return (x!= v.x) || (y!= v.y) || (z!= v.z);
}

inline Vector3 Vector3::operator - () const
{
	return Vector3(-x, -y, -z);
}

inline Vector3 Vector3::operator + (const Vector3 &v) const
{
	return Vector3(x + v.x, y + v.y, z + v.z);
}

inline Vector3 Vector3::operator += (const Vector3 &v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

inline Vector3 Vector3::operator - (const Vector3 &v) const
{
	return Vector3(x - v.x, y - v.y, z - v.z);
}

inline Vector3 Vector3::operator -= (const Vector3 &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

inline double Vector3::operator * (const Vector3 &v) const
{
	return x * v.x + y * v.y + z * v.z;
}

inline Vector3 operator * (double f, const Vector3 &v)
{
	return Vector3(f * v.x, f * v.y, f * v.z);
}

inline Vector3 Vector3::operator * (const double f) const
{
	Vector3 r(x * f, y * f, z * f);

	return r;
}

inline Vector3 Vector3::operator *= (const double f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

inline Vector3 Vector3::operator / (const double f) const
{
	Vector3 r(x / f, y / f, z / f);

	return r;
}

inline Vector3 Vector3::operator /= (const double f)
{
	x /= f;
	y /= f;
	z /= f;
	return *this;
}

inline double Vector3::Length() const
{
	return sqrt(x * x + y * y + z * z);
}

inline double Vector3::LengthXY() const
{
	return sqrt(x * x + y * y);
}

inline double Vector3::LengthXZ() const
{
	return sqrt(x * x + z * z);
}

inline double Vector3::LengthYZ() const
{
	return sqrt(y * y + z * z);
}

inline double Vector3::Distance(const Vector3 &v) const
{
	return sqrt((v.x - x) * (v.x - x) + 
		(v.y - y) * (v.y - y) + 
		(v.z - z) * (v.z - z));
}

inline Vector3 Vector3::Normalize() 
{
	double len = Length();
	if (len > 0)
		return (1/len) * (*this);
	else
		return (*this);
}

inline Vector3 Vector3::Scale(double tx,  double ty, double tz)
{
	return Vector3(x * tx, y * ty, z * tz);
}

// Operator functions

inline Vector3 Vector3::Cross(const Vector3 &v) const
{
	return Vector3( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
}

// Perspective


inline Vector3 Vector3::Perspective(double PerspectiveFactor)
{
	double f = 1.0 / (z / PerspectiveFactor + 1.0);
	return Vector3(f * x, f * y, z);
}

inline Vector3 Vector3::UndoPerspective(double PerspectiveFactor)
{
	double f = z / PerspectiveFactor + 1.0;
	return Vector3(f * x, f * y, z);
}


/* Convert Euler angles to a lookat point. */

/*
Vector3 Vector3::EulerToVector()
{
	Vector3 r;

    Matrix4 Rotate1, Rotate2, temp;
    Vector4 m, result;

    Rotate1.RotateY(y);
    Rotate2.RotateX(x);
    temp = Rotate1 * Rotate2;
    Rotate2.RotateZ(z);
    Rotate1 = temp * Rotate2;

    m.M11 = 0.0;
    m.M12 = 0.0;
    m.M13 = 1.0;
    m.M14 = 1.0;
    result = m * Rotate1;

    r.Set(result.M11, result.M12, result.M13);
	return r;
}
  
*/

inline Vector3 Vector3::operator * (const Matrix4& m) const
{
	Vector3 c;

	c.x = x*m.n11 + y*m.n21 + z*m.n31 + m.n41;
	c.y = x*m.n12 + y*m.n22 + z*m.n32 + m.n42;
	c.z = x*m.n13 + y*m.n23 + z*m.n33 + m.n43;
	double w = x*m.n14 + y*m.n24 + z*m.n34 + m.n44;

	if (w != 0.0)
	{
		c.x /= w;
		c.y /= w;
	}

	return c;
}

inline Vector3 Vector3::operator *= (const Matrix4& m)
{
	Vector3 c;

	c.x = x*m.n11 + y*m.n21 + z*m.n31 + m.n41;
	c.y = x*m.n12 + y*m.n22 + z*m.n32 + m.n42;
	c.z = x*m.n13 + y*m.n23 + z*m.n33 + m.n43;
	double w = x*m.n14 + y*m.n24 + z*m.n34 + m.n44;

	if (w != 0.0)
	{
		c.x /= w;
		c.y /= w;
	}

	*this = c;
	return *this;
}

inline double L1Norm(const Vector3 &v)
{
	return fabs(v.x) + fabs(v.y) + fabs(v.z);
}

// ***** Vector2

inline Vector2::Vector2(double tx, double ty)
{
	x = tx; y = ty;
}

inline bool Vector2::operator != (const Vector2 &v) const
{
	return (x != v.x) || (y != v.y);
}

inline bool Vector2::operator == (const Vector2 &v) const
{
	return (x == v.x) && (y == v.y);
}

inline Vector2 Vector2::operator - () const
{
	return Vector2(-x, -y);
}

inline Vector2 Vector2::operator + (const Vector2 &v) const 
{
	return Vector2(x + v.x, y + v.y);
}

inline Vector2 Vector2::operator += (const Vector2 &v)
{
	x += v.x;
	y += v.y;
	return *this;
}


inline Vector2 Vector2::operator - (const Vector2 &v) const 
{
	return Vector2(x - v.x, y - v.y);
}

inline Vector2 Vector2::operator -= (const Vector2 &v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

inline double Vector2::operator * (const Vector2 &v) const 
{
	return x * v.x + y * v.y;
}

inline Vector2 operator * (const double f, const Vector2 &v) 
{
	return Vector2(f * v.x, f * v.y);
}

inline Vector2 operator * (const Vector2 &v, const double f) 
{
	return Vector2(f * v.x, f * v.y);
}

inline Vector2 Vector2::operator / (double f) const 
{
	if (f == 0.0)
		return *this;
	else
		return Vector2(x / f, y / f);
}

inline Vector2 Vector2::operator *= (double f)
{
	x *= f;
	y *= f;
	return *this;
}

inline Vector2 Vector2::operator /= (double f)
{
	x /= f;
	y /= f;
	return *this;
}

inline double Vector2::operator %(const Vector2 &v)
{
	return x * v.y - y * v.x;
}


inline Vector2 Vector2::Set(double X, double Y)
{
	x = X; y = Y;
	return *this;
}

inline double Vector2::Length() const 
{
	return sqrt(x * x + y * y);
}

inline double Vector2::Distance(const Vector2 &v) const 
{
	return sqrt((v.x - x) * (v.x - x) + 
		(v.y - y) * (v.y - y));
}

inline Vector2 Vector2::Normalize() 
{
	double len = Length();
	if (len > 0)
		return (1/len) * (*this);
	else
		return (*this);
}

inline bool Vector2::IsNull() 
{
	return x == 0.0 && y == 0.0;
}

inline Vector2 Vector2::operator * (const Matrix4 &m) const 
{
	Vector2 c;

	c.x = x*m.n11 + y*m.n21 + m.n41;
	c.y = x*m.n12 + y*m.n22 + m.n42;
	double w = x*m.n14 + y*m.n24 + m.n44;

	if (w != 0.0)
		c /= w;

	return c;
}

inline Vector2 Vector2::operator * (const Matrix3 &m) const 
{
	Vector2 c;

	c.x = x*m.n11 + y*m.n21 + m.n31;
	c.y = x*m.n12 + y*m.n22 + m.n32;
	double w = x*m.n13 + y*m.n23 + m.n33;

	if (w != 0.0)
		c /= w;

	return c;
}

inline double L1Norm(const Vector2 &v)
{
	return fabs(v.x) + fabs(v.y);
}


#endif //_MATRIX3_H_


