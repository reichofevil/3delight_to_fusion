#ifndef _VECTOR2F_FU_H_
#define _VECTOR2F_FU_H_

#ifndef _VECTOR_MATH_H_INCLUDED_
#error Include VectorMath.h
#endif



class Vector2f
{
public:
	union
	{
		float32 V[2];

		struct
		{
			float32 X;
			float32 Y;
		};
	};

public:
	Vector2f();
	Vector2f(float32 x, float32 y);
	Vector2f(float32 v[2]);

	explicit Vector2f(const Vector4 &v);
	explicit Vector2f(const Vector3f &v);
	explicit Vector2f(const Vector4f &v);
	
	float32 Length() const;
	float32 Length2() const;			// length squared (can be much faster to compute than Length())

	bool operator==(const Vector2f &v);				// checks exact equality of elements (no epsilon testing)
	bool operator!=(const Vector2f &v);				// checks exact inequality of elements (no epsilon testing)

	Vector2f &operator=(const Vector2f &v);

	Vector2f operator-();

	Vector2f &operator+=(const Vector2f &v);
	Vector2f &operator-=(const Vector2f &v);

	Vector2f &operator*=(float32 s);								// multiplies each component of this vector by s
	Vector2f &operator/=(float32 s);								// divides each component of this vector by s

	Vector2f operator-(const Vector2f &v) const;
	Vector2f operator+(const Vector2f &v) const;

	friend Vector2f Mul(const Vector2f &v0, const Vector2f &v1);

	float32 operator*(const Vector2f &v) const;				// 2D dot product

	Vector2f operator*(float32 s) const;

	bool IsFinite() const;
	bool IsNaN() const;

	friend float32 Dot(const Vector2f &v0, const Vector2f &v1);						// the 2d dot product of these vectors
	friend Vector2f operator*(float32 s, const Vector2f &v);
	friend Vector2f Lerp(const Vector2f &p, const Vector2f &q, float32 t);		// lerps from p to q by lerp factor 't'
	friend Vector2f Perp(const Vector2f &v);												// sets this vector to v rotated by 90 degrees to the left.  This gives the normal to the 2D line defined by this vector.
};

inline float32 Length(const Vector2f &p) 
{
	return p.Length();
}

inline float32 L1Norm(const Vector2f &v)
{
	return fabs(v.X) + fabs(v.Y);
}

inline float32 L2Norm(const Vector2f &p)
{
	return p.Length();
}

inline Vector2f Normalize(const Vector2f &v)
{
	float32 len = v.Length();
	if (len != 0.0)
	{
		float32 invLen = 1.0f / len;
		return Vector2f(v.X * invLen, v.Y * invLen);
	}

	return v;
}

inline Vector2f Mul(const Vector2f &v0, const Vector2f &v1)
{
	return Vector2f(v0.X * v1.X, v0.Y * v1.Y);
}

inline float32 Dot(const Vector2f &v0, const Vector2f &v1)
{
	return v0.X * v1.X + v0.Y * v1.Y;
}

inline Vector2f operator*(float32 s, const Vector2f &v) 
{ 
	return v * s; 
}

inline Vector2f Lerp(const Vector2f &p, const Vector2f &q, float32 t)
{
	return (1.0f - t) * p + t * q;
}

// Rotates this vector by 90 degrees to the left.  This gives the normal to the 2D line defined by this vector.
inline Vector2f Perp(const Vector2f &v)
{
	return Vector2f(-v.Y, v.X);
}







inline Vector2f::Vector2f() 
{
}

inline Vector2f::Vector2f(float32 x, float32 y) : X(x), Y(y) 
{
}

inline Vector2f::Vector2f(float32 v[2]) : X(v[0]), Y(v[1]) 
{
}

inline float32 Vector2f::Length2() const
{
	return X * X + Y * Y;
}

inline float32 Vector2f::Length() const
{
	return sqrtf(X * X + Y * Y);
}

inline Vector2f &Vector2f::operator=(const Vector2f &v) 
{
	X = v.X;
	Y = v.Y;
	return *this;
}

inline bool Vector2f::operator==(const Vector2f &v)
{
	return X == v.X && Y == v.Y;
}

inline bool Vector2f::operator!=(const Vector2f &v)
{
	return X != v.X || Y != v.Y;
}

inline Vector2f Vector2f::operator-() 
{ 
	return Vector2f(-X, -Y); 
}

inline Vector2f Vector2f::operator-(const Vector2f &v) const 
{ 
	return Vector2f(X - v.X, Y - v.Y); 
}

inline Vector2f Vector2f::operator+(const Vector2f &v) const 
{ 
	return Vector2f(X + v.X, Y + v.Y); 
}

inline float32 Vector2f::operator*(const Vector2f &v) const 
{ 
	return X * v.X + Y * v.Y; 
}

inline Vector2f Vector2f::operator*(float32 s) const 
{ 
	return Vector2f(s * X, s * Y); 
}

inline Vector2f &Vector2f::operator+=(const Vector2f &v)
{
	X += v.X;
	Y += v.Y;
	return *this;
}

inline Vector2f &Vector2f::operator-=(const Vector2f &v)
{
	X -= v.X;
	Y -= v.Y;
	return *this;
}

inline Vector2f &Vector2f::operator*=(float32 s)
{
	X *= s;
	Y *= s;
	return *this;
}

inline Vector2f &Vector2f::operator/=(float32 s)
{
	float32 invS = 1.0f / s;
	X *= invS;
	Y *= invS;
	return *this;
}

inline bool Vector2f::IsFinite() const 
{ 
	return _finite(X) && _finite(Y); 
}

inline bool Vector2f::IsNaN() const 
{ 
	return _isnan(X) || _isnan(Y); 
}

inline Vector2f fabs(const Vector2f &v)
{
	return Vector2f(fabs(v.X), fabs(v.Y));
}



#endif