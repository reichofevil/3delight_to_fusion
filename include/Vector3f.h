#ifndef _VECTOR_3F_H_
#define _VECTOR_3F_H_

#ifndef _VECTOR_MATH_H_INCLUDED_
#error Include VectorMath.h
#endif


//#pragma pack(push, 4) -- TODO


class Vector3f
{
public:
	union
	{
		float32 V[3];

		struct
		{
			float32 X;
			float32 Y;
			float32 Z;
		};

		struct
		{
			float32 R;
			float32 G;
			float32 B;
		};
	};

public:
	Vector3f();														// does nothing
	explicit Vector3f(float32 s);
	explicit Vector3f(float32 x, float32 y);				// 3rd component to 0
	explicit Vector3f(const Vector4 &v);
	explicit Vector3f(const Vector2f &v);					// 3 component to 0
	explicit Vector3f(const Vector2f &v, float32 z);
	explicit Vector3f(const Vector3d &v);
	explicit Vector3f(const Vector4f &v);
	Vector3f(float32 v[3]);										// initialize from array
	Vector3f(float32 x, float32 y, float32 z);			
#if !defined(__INTEL_COMPILER) || __INTEL_COMPILER < 700
	Vector3f(const Vector3f &v);								// copy ctor	
#endif
	~Vector3f();													// does nothing

	bool operator==(const Vector3f &v);						// checks exact equality of elements (no epsilon testing)
	bool operator!=(const Vector3f &v);						// checks exact inequality of elements (no epsilon testing)

	Vector3f &operator=(const Vector3f &v);				// assignment

	float &operator[](int i);
	const float operator[](int i) const;

	Vector3f &operator+=(const Vector3f &p);
	Vector3f &operator-=(const Vector3f &p);

	Vector3f operator*(const float32 s) const;
	Vector3f operator/(const float32 s) const;
	
	Vector3f &operator+=(float32 s);							// adds scalar s to each component
	Vector3f &operator-=(float32 s);							// adds scalar s to each component
	Vector3f &operator*=(float32 s);							// multiplies each component by scalar s
	Vector3f &operator/=(float32 s);							// divides each component by scalar s

	Vector3f operator^(const Vector3f &v) const;			// cross product
	Vector3f operator+(const Vector3f &v) const;			// per component addition
	Vector3f operator-(const Vector3f &v) const;			// per component subtraction
	friend Vector3f Mul(const Vector3f &v1, const Vector3f &v2);	// per component multiplication
	Vector3f operator-() const;

   float32 operator*(const Vector3f &v) const;			// dot product

	bool IsNaN() const;
	bool IsFinite() const;
	bool IsBounded(float32 s = 1000000000.0f) const;

	float32 Length() const;
	float32 Length2() const;

	bool IsZero(float32 epsilonSquared = 0.0f) const;

	bool Equals(const Vector3f &v, float epsilon = 0.001f);

	void Normalize();
	bool IsNormalized() const;

	void Project(const Vector3f &axis);
};



inline float32 Length(const Vector3f &p)
{
	return p.Length();
}

inline float32 L1Norm(const Vector3f &p)
{
	return fabs(p.X) + fabs(p.Y) + fabs(p.Z);
}

inline float32 L2Norm(const Vector3f &p)
{
	return p.Length();
}

inline Vector3f Normalize(const Vector3f &p)
{
	float32 len = p.Length();
	if (len != 0.0)
	{
		float32 invLen = 1.0f / len;
		return Vector3f(p.X * invLen, p.Y * invLen, p.Z * invLen);
	}
	return p;
}

inline Vector3f operator*(float32 s, const Vector3f &v)
{
	return v * s;
}

inline float32 Dot(const Vector3f &p, const Vector3f &q)
{
	return p * q;
}

inline Vector3f Cross(const Vector3f &p, const Vector3f &q)
{
	return p ^ q;
}

// Lerp from p to q
inline Vector3f Lerp(const Vector3f &p, const Vector3f &q, float32 t)
{
	return (1.0f - t) * p + t * q;
}

inline Vector3f Lerp(const Vector3f &p, const Vector3f &q, float64 t)
{
	float32 ft = t;
	return (1.0f - ft) * p + ft * q;
}

inline Vector3f Min(const Vector3f &p, const Vector3f &q)
{
	return Vector3f(min(p.X, q.X), min(p.Y, q.Y), min(p.Z, q.Z));
}

inline Vector3f Max(const Vector3f &p, const Vector3f &q)
{
	return Vector3f(max(p.X, q.X), max(p.Y, q.Y), max(p.Z, q.Z));
}




inline Vector3f::Vector3f() {}

inline Vector3f::Vector3f(float32 s) : X(s), Y(s), Z(s) {}

inline Vector3f::Vector3f(float32 x, float32 y) : X(x), Y(y), Z(0.0f) {}

inline Vector3f::Vector3f(float32 v[3]) : X(v[0]), Y(v[1]), Z(v[2]) {}

inline Vector3f::Vector3f(float32 x, float32 y, float32 z) : X(x), Y(y), Z(z) {}

#if !defined(__INTEL_COMPILER) || __INTEL_COMPILER < 700
inline Vector3f::Vector3f(const Vector3f &v)
{ 
	*this = v; 
}
#endif

inline Vector3f::~Vector3f() {}

inline float &Vector3f::operator [] (int i) 
{
	return V[i]; 
}

inline const float Vector3f::operator [] (int i) const 
{ 
	return V[i]; 
}

inline Vector3f &Vector3f::operator=(const Vector3f &v)
{
	X = v.X;
	Y = v.Y;
	Z = v.Z;
	return *this;
}

inline Vector3f &Vector3f::operator += (const Vector3f &p)
{
	X += p.X;
	Y += p.Y;
	Z += p.Z;

	return *this;
}

inline Vector3f &Vector3f::operator -= (const Vector3f &p)
{
	X -= p.X;
	Y -= p.Y;
	Z -= p.Z;

	return *this;
}

inline Vector3f Vector3f::operator * (const float32 s) const
{
	return Vector3f(X * s, Y * s, Z * s);
}

inline Vector3f Vector3f::operator / (const float32 s) const
{
	float32 invScale = 1.0f / s;
	return Vector3f(X * invScale, Y * invScale, Z * invScale);
}

inline Vector3f &Vector3f::operator+=(float32 s)
{
	X += s;
	Y += s;
	Z += s;

	return *this;
}

inline Vector3f &Vector3f::operator-=(float32 s)
{
	X -= s;
	Y -= s;
	Z -= s;

	return *this;
}

inline Vector3f &Vector3f::operator*=(float32 s)
{
	X *= s;
	Y *= s;
	Z *= s;

	return *this;
}

inline Vector3f &Vector3f::operator/=(float32 s)
{
	float32 is = 1.0f / s;
	X *= is;
	Y *= is;
	Z *= is;

	return *this;
}

inline bool Vector3f::IsNaN() const
{
	for (int i = 0; i < 3; i++)
		if (_isnan(V[i]))
			return true;

	return false;
}

inline bool Vector3f::IsFinite() const
{
	for (int i = 0; i < 3; i++)
		if (!_finite(V[i]))
			return false;

	return true;
}

inline bool Vector3f::IsBounded(float32 s) const
{
	return fabs(X) < s && fabs(Y) < s && fabs(Z) < s;
}

inline Vector3f Vector3f::operator^(const Vector3f &v) const
{
	return Vector3f(Y * v.Z - Z * v.Y, -(X * v.Z - Z * v.X), X * v.Y - Y * v.X);
}

inline Vector3f Vector3f::operator+(const Vector3f &v) const			// per component addition
{
	return Vector3f(X + v.X, Y + v.Y, Z + v.Z);
}

inline Vector3f Vector3f::operator-(const Vector3f &v) const			// per component subtraction
{
	return Vector3f(X - v.X, Y - v.Y, Z - v.Z);
}

inline Vector3f Mul(const Vector3f &v1, const Vector3f &v2)				// per component multiplication
{
	return Vector3f(v1.X * v2.X, v1.Y * v2.Y, v1.Z * v2.Z);
}

inline Vector3f operator/(float32 f, const Vector3f &v)
{
	return Vector3f(f / v.X, f / v.Y, f / v.Z);
}

inline float32 Vector3f::Length() const { return sqrtf(X * X + Y * Y + Z * Z); }

inline float32 Vector3f::Length2() const { return (X * X + Y * Y + Z * Z); }

inline bool Vector3f::Equals(const Vector3f &v, float epsilon)
{
	Vector3f tmp = *this - v;
	return Dot(tmp, tmp) < epsilon;
}

inline Vector3f Vector3f::operator-() const { return Vector3f(-X, -Y, -Z); }

inline float32 Vector3f::operator*(const Vector3f &v) const			// dot product
{
	return X * v.X + Y * v.Y + Z * v.Z;
}

inline bool Vector3f::IsZero(float32 epsilonSquared) const
{
	return fabs(Dot(*this, *this)) <= epsilonSquared;
}

inline void Vector3f::Normalize()
{
	float32 len = Length();

	if (len != 0.0f)
	{
		float32 invLen = 1.0f / len;

		X *= invLen;
		Y *= invLen;
		Z *= invLen;
	}
}

inline Vector3f fabs(const Vector3f &v)
{
	return Vector3f(fabs(v.X), fabs(v.Y), fabs(v.Z));
}

inline bool Vector3f::IsNormalized() const { return fabs(Length() - 1.0f) < 0.00001f; }

inline void Vector3f::Project(const Vector3f &axis)
{
	X = (X - axis.X) / (Z - axis.Z) + axis.X;
	Y = (Y - axis.Y) / (Z - axis.Z) + axis.Y;
}

inline bool Vector3f::operator==(const Vector3f &v)
{
	return v.X == X && v.Y == Y && v.Z == Z;
}

inline bool Vector3f::operator!=(const Vector3f &v)
{
	return !(*this == v);
}


//#pragma pack(pop)


#endif