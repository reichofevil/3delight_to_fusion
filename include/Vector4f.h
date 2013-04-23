#ifndef _VECTOR4F_FU_H_
#define _VECTOR4F_FU_H_

#ifndef _VECTOR_MATH_H_INCLUDED_
#error Include VectorMath.h
#endif

#define Color4f Vector4f


#pragma pack(push, 4)

// A 32-bit floating point vector class.  Vectors should be thought of as column vectors.
class Vector4f
{
public:
	union
	{
		float32 V[4];

		struct
		{
			float32 X;
			float32 Y;
			float32 Z;
			float32 W;
		};

		struct
		{
			float32 R;
			float32 G;
			float32 B;
			float32 A;
		};
	};

	//static const Vector4f One;						// It would be nice to do this but eyeonScript potentially needs access to these for scripting
	//static const Vector4f Zero;						// and they don't get initialized until eyeonSystem is loaded.
	//static const Vector4f Black;
	//static const Vector4f White;

public:
	Vector4f();												// constructs an uninitialized vector
	explicit Vector4f(float32 s);						// smeared over all 4 components
	explicit Vector4f(const Vector4 &v);
	explicit Vector4f(const Vector3f &v);			// 4th component is set to 1.0f
	explicit Vector4f(const Vector2f &v);			// last two components set to 0 and 1
	explicit Vector4f(const FltPixel &p);
	Vector4f(const float32 v[4]);
	Vector4f(const float64 v[4]);
	Vector4f(float32 u, float32 v);
	Vector4f(float32 x, float32 y, float32 z);
	Vector4f(float32 x, float32 y, float32 z, float32 w);
	Vector4f(const Vector3f &v, float32 w);
	Vector4f(const Vector4f &v);
	~Vector4f();		

	Vector4f &operator=(const Vector4f &v);

	bool operator==(const Vector4f &v);				// checks exact equality of elements (no epsilon testing)
	bool operator!=(const Vector4f &v);				// checks exact inequality of elements (no epsilon testing)

	bool Equals(const Vector4f &v, float epsilon = 0.001f);

	Vector4f &operator+=(const Vector4f &v);		// per component addition of all 4 elements
	Vector4f &operator-=(const Vector4f &v);		// per component subtraction of all 4 elements
	Vector4f &operator*=(const Vector4f &v);		// per component multiplication of all 4 elements
	Vector4f &operator/=(const Vector4f &v);		// per component division of all 4 elements
	
	Vector4f operator*(float32 s);					// multiplies all 4 components of this vector by the scalar s
	Vector4f operator/(float32 s);					// divides all 4 components of this vector by the scalar s

	Vector4f &operator*=(float32 s);					// multiplies all 4 components by s
	Vector4f &operator/=(float32 s);					// divides all 4 components by s

	Vector4f operator^(const Vector4f &v);			// computes the cross product of this vector with v (treated at 3D vectors)

	float &operator[](int i);
	const float operator[](int i) const;

	Vector4f operator-() const;						// negates all 4 components of this vector

	float32 Length2() const;
	float32 Length3() const;							// returns the length of this vector (treating this as a 3-vector)
	float32 Length4() const;							// returns the length of this vector (treating this as a 4-vector)

	void Homogenize();									// divides this vector by its w component

	void Normalize3();									// normalizes this vector (treating this as a 3-vector and checks for length == 0 before dividing)
	void Normalize4();									// normalizes this vector (treating this as a 4-vector and checks for length == 0 before dividing)

	bool IsNaN() const;
	bool IsFinite() const;
	bool IsPositive() const;							// tests RGBA/XYZW components
	bool IsOpaque() const;								// checks alpha > 0.9999f
	bool IsZero(float32 epsilon = 0.0f) const;
	bool IsNormalized3() const;						// checks fabs(Length3() - 1.0f) < 0.0001f
	bool IsNormalized4() const;
	bool IsHDR() const;									// checks colors fall in [0, 1] range

	inline float32 Luminance() const;				// treats the xyz components as rgb colors and returns luminosity Y

	friend Vector4f operator+(const Vector4f &v0, const Vector4f &v1);	// computes the per component sum of the two vectors for all 4 elements
	friend Vector4f operator-(const Vector4f &v0, const Vector4f &v1);	// computes the per component difference of the two vectors for all 4 elements

	friend Vector4f operator*(float32 s, const Vector4f &v);
	friend Vector4f operator*(const Vector4f &v, float32 s);

	friend float32 Dot3(const Vector4f &u, const Vector4f &v);				// dot product between the first 3 components (X, Y, Z)
	friend float32 Dot4(const Vector4f &u, const Vector4f &v);				// 4 compnent dot product

	friend Vector4f Mul(const Vector4f &c0, const Vector4f &c1);
	friend Vector4f Div(const Vector4f &c0, const Vector4f &c1);				// c0 / c1 component-wise
	friend Vector4f Div(float32 s, const Vector4f &c1);						// s / c1 component-wise

	friend Vector4f Lerp(const Vector4f &c0, const Vector4f &c1, float32 t);
	friend Vector4f Lerp(const Vector4f &c0, const Vector4f &c1, float64 t);

	friend Vector4f fabs(const Vector4f &v);

	friend float32 Length2(const Vector4f &v);
	friend float32 Length3(const Vector4f &v);
	friend float32 Length4(const Vector4f &v);
};


inline Vector4f operator+(const Vector4f &v0, const Vector4f &v1)
{
	return Vector4f(v0.X + v1.X, v0.Y + v1.Y, v0.Z + v1.Z, v0.W + v1.W);
}

inline Vector4f operator-(const Vector4f &v0, const Vector4f &v1)
{
	return Vector4f(v0.X - v1.X, v0.Y - v1.Y, v0.Z - v1.Z, v0.W - v1.W);
}

inline Vector4f operator*(float32 s, const Vector4f &v) 
{
	return Vector4f(v.X * s, v.Y * s, v.Z * s, v.W * s);
}

inline Vector4f operator*(const Vector4f &v, float32 s) 
{
	return s * v;
}

inline float32 Dot3(const Vector4f &u, const Vector4f &v)
{
	return u.X * v.X + u.Y * v.Y + u.Z * v.Z;
}

inline float32 Dot4(const Vector4f &u, const Vector4f &v)
{
	return u.X * v.X + u.Y * v.Y + u.Z * v.Z + u.W * v.W;
}

inline Vector4f Mul(const Vector4f &c0, const Vector4f &c1)
{
	return Vector4f(c0.R * c1.R, c0.G * c1.G, c0.B * c1.B, c0.A * c1.A);
}

inline Vector4f Div(const Vector4f &c0, const Vector4f &c1)
{
	return Vector4f(c0.X / c1.X, c0.Y / c1.Y, c0.Z / c1.Z, c0.W / c1.W);
}

inline Vector4f Div(float32 s, const Vector4f &c1)
{
	return Vector4f(s / c1.X, s / c1.Y, s / c1.Z, s / c1.W);
}

inline Vector4f Lerp(const Vector4f &c0, const Vector4f &c1, float32 t)
{
	return (1.0f - t) * c0 + t * c1;
}

inline Vector4f Lerp(const Vector4f &c0, const Vector4f &c1, float64 t)
{
	float32 ft = t;
	return (1.0f - ft) * c0 + ft * c1;
}

inline Vector4f Lerp(const Vector4f &c0, const Vector4f &c1, const Vector4f &t)
{
	return Mul(Vector4f(1.0f) - t, c0) + Mul(t, c1);
}

inline Vector4f fabs(const Vector4f &v)
{
	return Vector4f(fabs(v.X), fabs(v.Y), fabs(v.Z), fabs(v.W));
}

inline float32 Length2(const Vector4f &v)
{
	return sqrtf(v.X * v.X + v.Y * v.Y);
}

inline float32 Length3(const Vector4f &v)
{
	return sqrtf(v.X * v.X + v.Y * v.Y + v.Z * v.Z);
}

inline float32 Length4(const Vector4f &v)
{
	return sqrtf(v.X * v.X + v.Y * v.Y + v.Z * v.Z + v.W * v.W);
}

inline Vector4f Saturate(const Vector4f &v)
{
	Vector4f ret = v;

	if (v.X > 1.0f)
		ret.X = 1.0f;
	else if (v.X < 0.0f)
		ret.X = 0.0f;

	if (v.Y > 1.0f)
		ret.Y = 1.0f;
	else if (v.Y < 0.0f)
		ret.Y = 0.0f;

	if (v.Z > 1.0f)
		ret.Z = 1.0f;
	else if (v.Z < 0.0f)
		ret.Z = 0.0f;

	if (v.W > 1.0f)
		ret.W = 1.0f;
	else if (v.W < 0.0f)
		ret.W = 0.0f;

	return ret;
}



inline Vector4f::Vector4f() {}

inline Vector4f::Vector4f(float32 s) : R(s), G(s), B(s), A(s) {}

inline Vector4f::Vector4f(const FltPixel &p) : R(p.R), G(p.G), B(p.B), A(p.A) {}

inline Vector4f::Vector4f(const float32 v[4]) : X(v[0]), Y(v[1]), Z(v[2]), W(v[3]) {}

inline Vector4f::Vector4f(const float64 v[4]) : X(v[0]), Y(v[1]), Z(v[2]), W(v[3]) {}

inline Vector4f::Vector4f(float32 u, float32 v) : X(u), Y(v) {}

inline Vector4f::Vector4f(float32 x, float32 y, float32 z) : X(x), Y(y), Z(z), W(1.0f) {}

inline Vector4f::Vector4f(float32 x, float32 y, float32 z, float32 w) : X(x), Y(y), Z(z), W(w) {}

inline Vector4f::~Vector4f() {}

inline Vector4f::Vector4f(const Vector4f &v) { *this = v; }

inline Vector4f &Vector4f::operator=(const Vector4f &v) 
{
	X = v.X;
	Y = v.Y;
	Z = v.Z;
	W = v.W;

	return *this;
}

inline bool Vector4f::operator==(const Vector4f &v) 
{
	return v.X == X && v.Y == Y && v.Z == Z && v.W == W;
}

inline bool Vector4f::operator!=(const Vector4f &v) 
{
	return !(*this == v);
}

inline bool Vector4f::Equals(const Vector4f &v, float epsilon)
{
	Vector4f tmp = *this - v;
	return Dot4(tmp, tmp) < epsilon;
}

inline Vector4f Vector4f::operator*(float32 s) 
{
	return Vector4f(X * s, Y * s, Z * s, W * s);
}

inline Vector4f &Vector4f::operator*=(float32 s) 
{
	X *= s;
	Y *= s;
	Z *= s;
	W *= s;

	return *this;
}

inline Vector4f Vector4f::operator/(float32 s) 
{ 
	float32 invS = 1.0f / s;
	return Vector4f(invS * X, invS * Y, invS * Z, invS * W);
}

inline Vector4f &Vector4f::operator/=(float32 s) 
{ 
	float32 invS = 1.0f / s;
	X *= invS;
	Y *= invS;
	Z *= invS;		
	W *= invS;
	return *this;
}

inline Vector4f Vector4f::operator^(const Vector4f &v) 
{ 
	return Vector4f(Y * v.Z - Z * v.Y, -(X * v.Z - Z * v.X), X * v.Y - Y * v.X, 1.0f);
}

inline Vector4f &Vector4f::operator+=(const Vector4f &v)
{ 
	X += v.X;
	Y += v.Y;
	Z += v.Z;
	W += v.W;
	return *this;
}

inline Vector4f &Vector4f::operator-=(const Vector4f &v)
{
	X -= v.X;
	Y -= v.Y;
	Z -= v.Z;
	W -= v.W;
	return *this;
}

inline Vector4f &Vector4f::operator*=(const Vector4f &v)
{
	X *= v.X;
	Y *= v.Y;
	Z *= v.Z;
	W *= v.W;
	return *this;
}

inline Vector4f &Vector4f::operator/=(const Vector4f &v)
{
	X /= v.X;
	Y /= v.Y;
	Z /= v.Z;
	W /= v.W;
	return *this;
}

inline float &Vector4f::operator [] (int i) 
{
	return V[i]; 
}

inline const float Vector4f::operator [] (int i) const 
{ 
	return V[i]; 
}

inline Vector4f Vector4f::operator - () const
{ 
	return Vector4f(-X, -Y, -Z, -W); 
}

inline float32 Vector4f::Length2() const 
{
	return sqrtf(X * X + Y * Y);
}

inline float32 Vector4f::Length3() const 
{
	return sqrtf(X * X + Y * Y + Z * Z);
}

inline float32 Vector4f::Length4() const 
{
	return sqrtf(X * X + Y * Y + Z * Z + W * W);
}

inline void Vector4f::Normalize3() 
{ 
	float32 length = Length3();

	if (length != 0.0f)
	{
		float32 iLen = 1.0f / length;
		X *= iLen;
		Y *= iLen;
		Z *= iLen;
	}
}

inline void Vector4f::Normalize4() 
{ 
	float32 length = Length4();

	if (length != 0.0f)
	{
		float32 iLen = 1.0f / length;
		X *= iLen;
		Y *= iLen;
		Z *= iLen;
		W *= iLen;
	}
}

inline Vector4f Normalize3(const Vector4f &p)
{
	float32 len = p.Length3();

	if (len != 0.0f)
	{
		float32 iLen = 1.0f / len;
		return Vector4f(p.X * iLen, p.Y * iLen, p.Z * iLen, p.W * iLen);
	}

	return p;
}

inline Vector4f Normalize4(const Vector4f &p)
{
	float32 iLen = 1.0f / p.Length4();
	return iLen * p;
}

inline void Vector4f::Homogenize()
{
	if (W != 0.0f)
	{
		float32 invW = 1.0f / W;

		X *= invW;
		Y *= invW;
		Z *= invW;

		W = 1.0f;
	}
}

inline float32 Vector4f::Luminance() const
{
	return 0.299f * R + 0.587f * G + 0.114f * B;
}

inline bool Vector4f::IsFinite() const
{
	for (int i = 0; i < 4; i++)
		if (!_finite(V[i]))
			return false;

	return true;
}

inline bool Vector4f::IsNaN() const
{
	for (int i = 0; i < 4; i++)
		if (_isnan(V[i]))
			return true;

	return false;
}

inline bool Vector4f::IsOpaque() const
{
	return A > 0.9999f;
}

inline bool Vector4f::IsZero(float32 epsilonSquared) const
{
	return fabs(Dot4(*this, *this)) <= epsilonSquared;
}

inline bool Vector4f::IsNormalized3() const
{
	return fabs(Length3() - 1.0f) < 0.0001f;
}

inline bool Vector4f::IsNormalized4() const
{
	return fabs(Length4() - 1.0f) < 0.0001f;
}

inline bool Vector4f::IsHDR() const
{
	return R > 1.0f || G > 1.0f || B > 1.0f || A > 1.0f || R < 0.0f || G < 0.0f || B < 0.0f || A < 0.0f;
}

#pragma pack(pop)

#endif